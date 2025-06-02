/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/application/GLRendererView.h"

#include "ocean/base/String.h"

#include "ocean/rendering/Manager.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/View.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

namespace Application
{

GLRendererView::~GLRendererView()
{
	release();
}

bool GLRendererView::initialize()
{
	Log::debug() << "GLRendererView::initialize()";

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

	return true;
}

bool GLRendererView::release()
{
	Log::debug() << "GLRendererView::release()";

	const ScopedLock scopedLock(lock_);

	const std::string engineName(engine_->engineName());
	const Timestamp timestampNow(true);

	Log::info() << "Render iterations " << renderingIterations_;
	Log::info() << "Real performance: " << String::toAString(1000.0 * double(timestampNow - renderingStartTimestamp_) / max(1.0, double(renderingIterations_)), 8) << "ms / frame";

	framebuffer_.release();
	engine_.release();

	ocean_assert(Rendering::ObjectRefManager::get().hasEngineObject(engineName, true) == false);

	return GLView::release();
}

bool GLRendererView::setFovX(const Scalar fovx)
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

bool GLRendererView::setBackgroundColor(const RGBAColor& color)
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

bool GLRendererView::resize(const int width, const int height)
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

bool GLRendererView::render()
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

void GLRendererView::setViewInteractionEnabled(const bool enabled)
{
	const ScopedLock scopedLock(lock_);

	viewInteractionEnabled_ = enabled;
}

void GLRendererView::onTouchDown(const float x, const float y)
{
	const ScopedLock scopedLock(lock_);

	previousTouchX_ = x;
	previousTouchY_ = y;
}

void GLRendererView::onTouchMove(const float x, const float y)
{
	const ScopedLock scopedLock(lock_);

	if (previousTouchX_ != -1.0f && previousTouchY_ != -1.0f && framebuffer_)
	{
		if (viewInteractionEnabled_)
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
			}
			catch (const std::exception& exception)
			{
				Log::error() << exception.what();
			}
		}

		previousTouchX_ = x;
		previousTouchY_ = y;
	}
}

void GLRendererView::onTouchUp(const float x, const float y)
{
	const ScopedLock scopedLock(lock_);

	previousTouchX_ = -1.0f;
	previousTouchY_ = -1.0f;
}

}

}

}

}
