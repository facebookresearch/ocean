/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/application/GLFrameView.h"

#include "ocean/base/String.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/rendering/Manager.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/View.h"

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

namespace Application
{

GLFrameView::~GLFrameView()
{
	release();
}

bool GLFrameView::initialize()
{
	Log::debug() << "GLFrameView::initialize()";

	const ScopedLock scopedLock(lock_);

	GLView::initialize();

	ocean_assert(engine_.isNull());

	engine_ = Rendering::Manager::get().engine("GLESceneGraph");
	if (engine_.isNull())
	{
		Log::error() << "Failed to create the GLESceneGraph rendering engine";
		return false;
	}

	framebuffer_ = engine_->createFramebuffer();
	if (framebuffer_.isNull())
	{
		Log::error() << "Failed to create a GLES framebuffer";
		return false;
	}

	const Rendering::PerspectiveViewRef view(engine_->factory().createPerspectiveView());
	ocean_assert(view);

	view->setBackgroundColor(RGBAColor(0, 0, 0));
	view->setFovX(Numeric::deg2rad(30));
	view->setTransformation(HomogenousMatrix4(Vector3(0, 0, 20)));

	framebuffer_->setView(view);

	if (initialViewportWidth_ != (unsigned int)(-1) && initialViewportHeight_ != (unsigned int)(-1))
	{
		const float aspectRatio = float(initialViewportWidth_) / float(initialViewportHeight_);
		framebuffer_->setViewport(0, 0, initialViewportWidth_, initialViewportHeight_);

		view->setAspectRatio(aspectRatio);
	}

	if (intermediateBackgroundFrameMedium_)
	{
		GLFrameView::setBackgroundMedium(intermediateBackgroundFrameMedium_, intermediateBackgroundAdjustFov_);
		intermediateBackgroundFrameMedium_.release();
	}

	Log::info() << "Succeeded to initialize the view";
	return true;
}

bool GLFrameView::release()
{
	Log::debug() << "GLFrameView::release()";

	const ScopedLock scopedLock(lock_);

	const std::string engineName(engine_->engineName());
	const Timestamp timestampNow(true);

	Log::info() << "Render iterations " << renderingIterations_;
	Log::info() << "Real performance: " << String::toAString(1000.0 * double(timestampNow - renderingStartTimestamp_) / max(1.0, double(renderingIterations_)), 8) << "ms / frame";

	intermediateBackgroundFrameMedium_.release();
	background_.release();
	framebuffer_.release();
	engine_.release();

	ocean_assert(Rendering::ObjectRefManager::get().hasEngineObject(engineName, true) == false);

	GLView::release();

	return true;
}

bool GLFrameView::setFovX(const Scalar fovx)
{
	const ScopedLock scopedLock(lock_);

	try
	{
		const Rendering::PerspectiveViewRef view(framebuffer_->view());
		return view->setFovX(fovx);
	}
	catch (const std::exception& exception)
	{
		Log::error() << exception.what();
		return false;
	}

	return false;
}

bool GLFrameView::setBackgroundColor(const RGBAColor& color)
{
	const ScopedLock scopedLock(lock_);

	try
	{
		const Rendering::ViewRef view(framebuffer_->view());
		return view->setBackgroundColor(color);
	}
	catch (const std::exception& exception)
	{
		Log::error() << exception.what();
		return false;
	}

	return false;
}

bool GLFrameView::setBackgroundMedium(const std::string& url, const std::string& type, const int preferredWidth, const int preferredHeight, const bool adjustFov)
{
	Media::Medium::Type mediumType = Media::Medium::FRAME_MEDIUM;

	if (type == "LIVE_VIDEO")
	{
		mediumType = Media::Medium::LIVE_VIDEO;
	}
	else if (type == "IMAGE")
	{
		mediumType = Media::Medium::IMAGE;
	}
	else if (type == "MOVIE")
	{
		mediumType = Media::Medium::MOVIE;
	}
	else if (type == "IMAGE_SEQUENCE")
	{
		mediumType = Media::Medium::IMAGE_SEQUENCE;
	}
	else
	{
		ocean_assert(false && "Invalid medium type");
	}

	Log::info() << "Request for a new background medium: " << url << ", (" << type << ")";

	const Media::FrameMediumRef frameMedium(Media::Manager::get().newMedium(url, mediumType));

	if (frameMedium)
	{
		Log::info() << "Succeeded to create the new medium.";
	}
	else
	{
		Log::error() << "FAILED to create the new medium.";
	}

	if (frameMedium && preferredWidth > 0 && preferredHeight > 0)
	{
		frameMedium->setPreferredFrameDimension((unsigned int)(preferredWidth), (unsigned int)(preferredHeight));
	}

	return setBackgroundMedium(frameMedium, adjustFov);
}

bool GLFrameView::setBackgroundMedium(const Media::FrameMediumRef& frameMedium, const bool adjustFov)
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (framebuffer_)
		{
			const Rendering::ViewRef view(framebuffer_->view());

			if (frameMedium)
			{
				if (background_)
				{
					background_->setMedium(frameMedium);
					frameMedium->start();
				}
				else
				{
					background_ = engine_->factory().createUndistortedBackground();

					if (background_)
					{
						background_->setMedium(frameMedium);

						const HomogenousMatrix4 device_T_display = framebuffer_->device_T_display();
						ocean_assert(device_T_display.isValid());

						const HomogenousMatrix4 display_T_camera = device_T_display.inverted() * HomogenousMatrix4(frameMedium->device_T_camera());

						Quaternion display_R_camera(display_T_camera.rotation());

						if ((display_R_camera * Vector3(0, 0, 1)) * Vector3(0, 0, 1) < 0)
						{
							// the camera is pointing towards the opposite direction of the display (e.g., user-facing camera)
							display_R_camera = Quaternion(Vector3(0, 1, 0), Numeric::pi()) * display_R_camera;
						}

						background_->setOrientation(display_R_camera);

						frameMedium->start();

						view->addBackground(background_);
					}
					else
					{
						Log::error() << "FAILED to create camera.";
					}
				}

				if (adjustFov)
				{
					adjustFovXToBackground_ = true;
				}
			}
			else
			{
				if (background_)
				{
					view->removeBackground(background_);
					background_.release();
				}
			}
		}
		else
		{
			// we store the background information until the view has been initialized

			intermediateBackgroundFrameMedium_ = frameMedium;
			intermediateBackgroundAdjustFov_ = adjustFov;
		}
	}
	catch (const std::exception& exception)
	{
		Log::error() << exception.what();
		return false;
	}

	return false;
}

Media::FrameMediumRef GLFrameView::backgroundMedium() const
{
	const ScopedLock scopedLock(lock_);

	if (background_)
	{
		return background_->medium();
	}

	return intermediateBackgroundFrameMedium_;
}

bool GLFrameView::resize(const int width, const int height)
{
	const ScopedLock scopedLock(lock_);

	if (width <= 0 || height <= 0)
	{
		return false;
	}

	if (framebuffer_.isNull())
	{
		initialViewportWidth_ = width;
		initialViewportHeight_ = height;

		return true;
	}
	else
	{
		try
		{
			framebuffer_->setViewport(0, 0, width, height);

			const Rendering::ViewRef renderingView(framebuffer_->view());
			if (renderingView)
			{
				renderingView->setAspectRatio(float(width) / float(height));
			}

			return true;
		}
		catch (const std::exception& exception)
		{
			Log::error() << exception.what();
		}
		catch (...)
		{
			Log::error() << "Uncaught exception occurred!";
		}
	}

	return false;
}

bool GLFrameView::render()
{
	const ScopedLock scopedLock(lock_);

	if (engine_.isNull() || framebuffer_.isNull())
	{
		return false;
	}

	if (renderingStartTimestamp_.isInvalid())
	{
		renderingStartTimestamp_.toNow();
	}

	++renderingIterations_;

	try
	{
		if (adjustFovXToBackground_)
		{
			const Rendering::PerspectiveViewRef perspectiveView(framebuffer_->view());
			if (perspectiveView)
			{
				bool validCamera = false;
				const Scalar idealFovX = perspectiveView->idealFovX(&validCamera);

				if (validCamera)
				{
					perspectiveView->setFovX(idealFovX);
					adjustFovXToBackground_ = false;

					Log::info() << "Adjusting the view's field of view to the background's field of view: " << Numeric::rad2deg(idealFovX) << "deg";
				}
			}
		}

		// Updates the rendering engine
		engine_->update(Timestamp(true));

		// Rendering the current frame
		framebuffer_->render();
		return true;
	}
	catch (const std::exception& exception)
	{
		Log::error() << exception.what();
	}
	catch (...)
	{
		Log::error() << "Uncaught exception occurred during rendering!";
	}

	return false;
}

bool GLFrameView::screen2frame(const Scalar xScreen, const Scalar yScreen, Scalar& xFrame, Scalar& yFrame)
{
	const ScopedLock scopedLock(lock_);

	if (xScreen < 0 || yScreen < 0)
	{
		return false;
	}

	if (!background_ || !framebuffer_ || !framebuffer_->view())
	{
		return false;
	}

	try
	{
		ocean_assert(background_);
		ocean_assert(framebuffer_);

		const PinholeCamera camera(background_->camera());
		const Quaternion backgroundOrientation = background_->orientation();

		unsigned int left, top, width, height;
		framebuffer_->viewport(left, top, width, height);

		const Line3 pickingRay(framebuffer_->view()->viewingRay((unsigned int)(xScreen + Scalar(0.5)), (unsigned int)(yScreen + Scalar(0.5)), (unsigned int)width, (unsigned int)height));

		Vector3 direction(backgroundOrientation.inverted() * pickingRay.direction());

		const Vector2 cameraCoordinate(camera.projectToImage<true>(HomogenousMatrix4(true), direction, false));

		xFrame = cameraCoordinate.x();
		yFrame = cameraCoordinate.y();

		return true;
	}
	catch (...)
	{
		// nothing to do here
	}

	return false;
}

void GLFrameView::onTouchDown(const float x, const float y)
{
	const ScopedLock scopedLock(lock_);

	previousTouchX_ = x;
	previousTouchY_ = y;
}

void GLFrameView::onTouchMove(const float x, const float y)
{
	const ScopedLock scopedLock(lock_);

	if (previousTouchX_ != -1.0f && previousTouchY_ != -1.0f && framebuffer_)
	{
		try
		{
			const Rendering::ViewRef view(framebuffer_->view());

			Scalar xDifference = Scalar(previousTouchX_ - x);
			Scalar yDifference = Scalar(previousTouchY_ - y);

			Quaternion orientation = view->transformation().rotation();

			Vector3 xAxis(1, 0, 0);
			Vector3 yAxis(0, 1, 0);
			Scalar factor = 0.5;

			Quaternion xRotation(orientation * xAxis, Numeric::deg2rad(Scalar(yDifference)) * factor);
			Quaternion yRotation(orientation * yAxis, Numeric::deg2rad(Scalar(xDifference)) * factor);

			Quaternion rotation(xRotation * yRotation);
			rotation.normalize();

			view->setTransformation(HomogenousMatrix4(rotation) * view->transformation());

			previousTouchX_ = x;
			previousTouchY_ = y;
		}
		catch (const std::exception& exception)
		{
			Log::error() << exception.what();
		}
	}
}

void GLFrameView::onTouchUp(const float x, const float y)
{
	const ScopedLock scopedLock(lock_);

	previousTouchX_ = -1.0f;
	previousTouchY_ = -1.0f;
}

}

}

}

}
