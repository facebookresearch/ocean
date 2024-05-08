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

GLFrameView::GLFrameView() :
	GLView(),
	viewInitialViewportWidth((unsigned int)(-1)),
	viewInitialViewportHeight((unsigned int)(-1)),
	viewPreviousTouchX(-1.0f),
	viewPreviousTouchY(-1.0f),
	viewRenderingIterations(0),
	adjustFovXToBackground(false)
{
	// nothing to do here
}

GLFrameView::~GLFrameView()
{
	release();
}

bool GLFrameView::initialize()
{
	Log::debug() << "GLFrameView::initialize()";

	const ScopedLock scopedLock(lock);

	GLView::initialize();

	ocean_assert(viewEngine.isNull());

	viewEngine = Rendering::Manager::get().engine("GLESceneGraph");
	if (viewEngine.isNull())
	{
		Log::error() << "Failed to create the GLESceneGraph rendering engine";
		return false;
	}

	viewFramebuffer = viewEngine->createFramebuffer();
	if (viewFramebuffer.isNull())
	{
		Log::error() << "Failed to create a GLES framebuffer";
		return false;
	}

	const Rendering::PerspectiveViewRef view(viewEngine->factory().createPerspectiveView());
	ocean_assert(view);

	view->setBackgroundColor(RGBAColor(0, 0, 0));
	view->setFovX(Numeric::deg2rad(30));
	view->setTransformation(HomogenousMatrix4(Vector3(0, 0, 20)));

	viewFramebuffer->setView(view);

	if (viewInitialViewportWidth != (unsigned int)(-1) && viewInitialViewportHeight != (unsigned int)(-1))
	{
		const float aspectRatio = float(viewInitialViewportWidth) / float(viewInitialViewportHeight);
		viewFramebuffer->setViewport(0, 0, viewInitialViewportWidth, viewInitialViewportHeight);

		view->setAspectRatio(aspectRatio);
	}

	if (intermediateBackgroundFrameMedium)
	{
		GLFrameView::setBackgroundMedium(intermediateBackgroundFrameMedium, intermediateBackgroundAdjustFov);
		intermediateBackgroundFrameMedium.release();
	}

	Log::info() << "Succeeded to initialize the view";
	return true;
}

bool GLFrameView::release()
{
	Log::debug() << "GLFrameView::release()";

	const ScopedLock scopedLock(lock);

	const std::string engineName(viewEngine->engineName());
	const Timestamp timestampNow(true);

	Log::info() << "Render iterations " << viewRenderingIterations;
	Log::info() << "Real performance: " << String::toAString(1000.0 * double(timestampNow - viewRenderingStartTimestamp) / max(1.0, double(viewRenderingIterations)), 8) << "ms / frame";

	intermediateBackgroundFrameMedium.release();
	viewBackground.release();
	viewFramebuffer.release();
	viewEngine.release();

	ocean_assert(Rendering::ObjectRefManager::get().hasEngineObject(engineName, true) == false);

	GLView::release();

	return true;
}

bool GLFrameView::setFovX(const Scalar fovx)
{
	const ScopedLock scopedLock(lock);

	try
	{
		const Rendering::PerspectiveViewRef view(viewFramebuffer->view());
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
	const ScopedLock scopedLock(lock);

	try
	{
		const Rendering::ViewRef view(viewFramebuffer->view());
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
	const ScopedLock scopedLock(lock);

	try
	{
		if (viewFramebuffer)
		{
			const Rendering::ViewRef view(viewFramebuffer->view());

			if (frameMedium)
			{
				if (viewBackground)
				{
					viewBackground->setMedium(frameMedium);
					frameMedium->start();
				}
				else
				{
					viewBackground = viewEngine->factory().createUndistortedBackground();

					if (viewBackground)
					{
						viewBackground->setMedium(frameMedium);

						const HomogenousMatrix4 device_T_display = viewFramebuffer->device_T_display();
						ocean_assert(device_T_display.isValid());

						const HomogenousMatrix4 display_T_camera = device_T_display.inverted() * HomogenousMatrix4(frameMedium->device_T_camera());

						Quaternion display_R_camera(display_T_camera.rotation());

						if ((display_R_camera * Vector3(0, 0, 1)) * Vector3(0, 0, 1) < 0)
						{
							// the camera is pointing towards the opposite direction of the display (e.g., user-facing camera)
							display_R_camera = Quaternion(Vector3(0, 1, 0), Numeric::pi()) * display_R_camera;
						}

						viewBackground->setOrientation(display_R_camera);

						frameMedium->start();

						view->addBackground(viewBackground);
					}
					else
					{
						Log::error() << "FAILED to create camera.";
					}
				}

				if (adjustFov)
				{
					adjustFovXToBackground = true;
				}
			}
			else
			{
				if (viewBackground)
				{
					view->removeBackground(viewBackground);
					viewBackground.release();
				}
			}
		}
		else
		{
			// we store the background information until the view has been initialized

			intermediateBackgroundFrameMedium = frameMedium;
			intermediateBackgroundAdjustFov = adjustFov;
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
	const ScopedLock scopedLock(lock);

	if (viewBackground)
	{
		return viewBackground->medium();
	}

	return intermediateBackgroundFrameMedium;
}

bool GLFrameView::resize(const int width, const int height)
{
	const ScopedLock scopedLock(lock);

	if (width <= 0 || height <= 0)
	{
		return false;
	}

	if (viewFramebuffer.isNull())
	{
		viewInitialViewportWidth = width;
		viewInitialViewportHeight = height;

		return true;
	}
	else
	{
		try
		{
			viewFramebuffer->setViewport(0, 0, width, height);

			const Rendering::ViewRef renderingView(viewFramebuffer->view());
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
			Log::error() << "Uncaught exception occured!";
		}
	}

	return false;
}

bool GLFrameView::render()
{
	const ScopedLock scopedLock(lock);

	if (viewEngine.isNull() || viewFramebuffer.isNull())
	{
		return false;
	}

	if (viewRenderingStartTimestamp.isInvalid())
	{
		viewRenderingStartTimestamp.toNow();
	}

	++viewRenderingIterations;

	try
	{
		if (adjustFovXToBackground)
		{
			const Rendering::PerspectiveViewRef perspectiveView(viewFramebuffer->view());
			if (perspectiveView)
			{
				bool validCamera = false;
				const Scalar idealFovX = perspectiveView->idealFovX(&validCamera);

				if (validCamera)
				{
					perspectiveView->setFovX(idealFovX);
					adjustFovXToBackground = false;

					Log::info() << "Adjusting the view's field of view to the background's field of view: " << Numeric::rad2deg(idealFovX) << "deg";
				}
			}
		}

		// Updates the rendering engine
		viewEngine->update(Timestamp(true));

		// Rendering the current frame
		viewFramebuffer->render();
		return true;
	}
	catch (const std::exception& exception)
	{
		Log::error() << exception.what();
	}
	catch (...)
	{
		Log::error() << "Uncaught exception occured during rendering!";
	}

	return false;
}

bool GLFrameView::screen2frame(const Scalar xScreen, const Scalar yScreen, Scalar& xFrame, Scalar& yFrame)
{
	const ScopedLock scopedLock(lock);

	if (xScreen < 0 || yScreen < 0)
	{
		return false;
	}

	if (!viewBackground || !viewFramebuffer || !viewFramebuffer->view())
	{
		return false;
	}

	try
	{
		ocean_assert(viewBackground);
		ocean_assert(viewFramebuffer);

		const PinholeCamera camera(viewBackground->camera());
		const Quaternion backgroundOrientation = viewBackground->orientation();

		unsigned int left, top, width, height;
		viewFramebuffer->viewport(left, top, width, height);

		const Line3 pickingRay(viewFramebuffer->view()->viewingRay((unsigned int)(xScreen + Scalar(0.5)), (unsigned int)(yScreen + Scalar(0.5)), (unsigned int)width, (unsigned int)height));

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
	const ScopedLock scopedLock(lock);

	viewPreviousTouchX = x;
	viewPreviousTouchY = y;
}

void GLFrameView::onTouchMove(const float x, const float y)
{
	const ScopedLock scopedLock(lock);

	if (viewPreviousTouchX != -1.0f && viewPreviousTouchY != -1.0f && viewFramebuffer)
	{
		try
		{
			const Rendering::ViewRef view(viewFramebuffer->view());

			Scalar xDifference = Scalar(viewPreviousTouchX - x);
			Scalar yDifference = Scalar(viewPreviousTouchY - y);

			Quaternion orientation = view->transformation().rotation();

			Vector3 xAxis(1, 0, 0);
			Vector3 yAxis(0, 1, 0);
			Scalar factor = 0.5;

			Quaternion xRotation(orientation * xAxis, Numeric::deg2rad(Scalar(yDifference)) * factor);
			Quaternion yRotation(orientation * yAxis, Numeric::deg2rad(Scalar(xDifference)) * factor);

			Quaternion rotation(xRotation * yRotation);
			rotation.normalize();

			view->setTransformation(HomogenousMatrix4(rotation) * view->transformation());

			viewPreviousTouchX = x;
			viewPreviousTouchY = y;
		}
		catch (const std::exception& exception)
		{
			Log::error() << exception.what();
		}
	}
}

void GLFrameView::onTouchUp(const float x, const float y)
{
	const ScopedLock scopedLock(lock);

	viewPreviousTouchX = -1.0f;
	viewPreviousTouchY = -1.0f;
}

}

}

}

}
