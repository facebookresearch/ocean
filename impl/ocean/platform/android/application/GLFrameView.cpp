/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/application/GLFrameView.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/View.h"

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

	GLRendererView::initialize();

	if (intermediateBackgroundFrameMedium_)
	{
		setBackgroundMedium(intermediateBackgroundFrameMedium_, intermediateBackgroundAdjustFov_);
		intermediateBackgroundFrameMedium_.release();
	}

	Log::info() << "Succeeded to initialize the view";
	return true;
}

bool GLFrameView::release()
{
	Log::debug() << "GLFrameView::release()";

	const ScopedLock scopedLock(lock_);

	intermediateBackgroundFrameMedium_.release();
	background_.release();

	return GLRendererView::release();
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

}

}

}

}
