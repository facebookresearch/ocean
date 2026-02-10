/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/ios/GLFrameViewController.h"

#include "ocean/media/Manager.h"

#import <UIKit/UIKit.h>

using namespace Ocean;

@implementation GLFrameViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	Log::debug() << "GLFrameViewController::viewDidLoad()";

	const ScopedLock scopedLock(lock_);

	adjustFovXToBackground_ = false;
	intermediateBackgroundAdjustFov_ = false;

	if (intermediateBackgroundFrameMedium_)
	{
		[self setFrameMedium:intermediateBackgroundFrameMedium_ andAdjustFov:intermediateBackgroundAdjustFov_];
		intermediateBackgroundFrameMedium_.release();
	}

	Log::info() << "Succeeded to initialize the frame medium view";
}

- (void)dealloc
{
	Log::debug() << "GLFrameViewController::dealloc()";

	const ScopedLock scopedLock(lock_);

	intermediateBackgroundFrameMedium_.release();
	renderingUndistortedBackground_.release();
}

-(void)setFrameMedium:(const Media::FrameMediumRef&)frameMedium
{
	[self setFrameMedium:frameMedium withRotation:Quaternion(false) andAdjustFov:false];
}

-(void)setFrameMedium:(const Media::FrameMediumRef&)frameMedium andAdjustFov:(bool)adjustFov
{
	[self setFrameMedium:frameMedium withRotation:Quaternion(false) andAdjustFov:adjustFov];
}

-(void)setFrameMedium:(const Media::FrameMediumRef&)frameMedium withRotation:(const Quaternion&)display_R_medium
{
	[self setFrameMedium:frameMedium withRotation:display_R_medium andAdjustFov:false];
}

-(void)setFrameMedium:(const Media::FrameMediumRef&)frameMedium withRotation:(const Quaternion&)display_R_medium andAdjustFov:(bool)adjustFov
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_)
		{
			if (frameMedium)
			{
				if (renderingUndistortedBackground_)
				{
					// Background already exists, just update the medium
					if (display_R_medium.isValid())
					{
						renderingUndistortedBackground_->setOrientation(display_R_medium);
					}

					renderingUndistortedBackground_->setMedium(frameMedium);
					frameMedium->start();
				}
				else
				{
					// Create the background lazily
					renderingUndistortedBackground_ = renderingEngine_->factory().createUndistortedBackground();

					if (renderingUndistortedBackground_)
					{
						renderingUndistortedBackground_->setMedium(frameMedium);

						if (display_R_medium.isValid())
						{
							renderingUndistortedBackground_->setOrientation(display_R_medium);
						}
						else
						{
							const HomogenousMatrix4 device_T_display = renderingFramebuffer_->device_T_display();
							ocean_assert(device_T_display.isValid());

							const HomogenousMatrix4 display_T_camera = device_T_display.inverted() * HomogenousMatrix4(frameMedium->device_T_camera());

							Quaternion display_R_camera(display_T_camera.rotation());

							if ((display_R_camera * Vector3(0, 0, 1)) * Vector3(0, 0, 1) < 0)
							{
								// the camera is pointing towards the opposite direction of the display (e.g., user-facing camera)
								display_R_camera = Quaternion(Vector3(0, 1, 0), Numeric::pi()) * display_R_camera;
							}

							renderingUndistortedBackground_->setOrientation(display_R_camera);
						}

						frameMedium->start();

						renderingView_->addBackground(renderingUndistortedBackground_);
					}
					else
					{
						Log::error() << "FAILED to create background.";
					}
				}

				if (adjustFov)
				{
					adjustFovXToBackground_ = true;
				}
			}
			else
			{
				// Setting null medium - remove and release background
				if (renderingUndistortedBackground_)
				{
					renderingView_->removeBackground(renderingUndistortedBackground_);
					renderingUndistortedBackground_.release();
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
	}
}

-(Ocean::Media::FrameMediumRef)frameMedium
{
	const ScopedLock scopedLock(lock_);

	if (renderingUndistortedBackground_)
	{
		return renderingUndistortedBackground_->medium();
	}

	return intermediateBackgroundFrameMedium_;
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	const ScopedLock scopedLock(lock_);

	if (renderingEngine_.isNull() || renderingFramebuffer_.isNull() || renderingView_.isNull())
	{
		return;
	}

	if (renderingStartTimestamp_.isInvalid())
	{
		renderingStartTimestamp_.toNow();
	}

	++renderingIterations_;

	try
	{
		const float scale = view.window.windowScene.screen.scale;

		const float widthPixels = rect.size.width * scale;
		const float heightPixels = rect.size.height * scale;

		ocean_assert(widthPixels == NumericF::floor(widthPixels));
		ocean_assert(heightPixels == NumericF::floor(heightPixels));

		if (widthPixels <= 0.0f || heightPixels <= 0.0f)
		{
			return;
		}

		renderingFramebuffer_->setViewport(0u, 0u, (unsigned int)(widthPixels), (unsigned int)(heightPixels));
		renderingView_->setAspectRatio(Scalar(widthPixels) / Scalar(heightPixels));

		if (adjustFovXToBackground_)
		{
			bool validCamera = false;
			const Scalar idealFovX = renderingView_->idealFovX(&validCamera);

			if (validCamera)
			{
				renderingView_->setFovX(idealFovX);
				adjustFovXToBackground_ = false;

				Log::info() << "Adjusting the view's field of view to the background's field of view: " << Numeric::rad2deg(idealFovX) << "deg";
			}
		}

		// Updates the rendering engine
		renderingEngine_->update(Timestamp(true));

		// Rendering the current frame
		renderingFramebuffer_->render();
	}
	catch (const std::exception& exception)
	{
		Log::error() << exception.what();
	}
	catch (...)
	{
		Log::error() << "Uncaught exception occurred during rendering!";
	}
}

- (CGPoint)view2medium:(CGPoint)viewPoint
{
	const ScopedLock scopedLock(lock_);

	if (viewPoint.x < 0.0 || viewPoint.y < 0.0)
	{
		return CGPointMake(-1.0, -1.0);
	}

	if (!renderingUndistortedBackground_ || !renderingFramebuffer_ || !renderingFramebuffer_->view())
	{
		return CGPointMake(-1.0, -1.0);
	}

	try
	{
		ocean_assert(renderingUndistortedBackground_);
		ocean_assert(renderingFramebuffer_);

		const SharedAnyCamera camera(renderingUndistortedBackground_->camera());

		if (camera)
		{
			const Quaternion backgroundOrientation = renderingUndistortedBackground_->orientation();

			unsigned int left, top, width, height;
			renderingFramebuffer_->viewport(left, top, width, height);

			const Line3 pickingRay(renderingFramebuffer_->view()->viewingRay((unsigned int)(viewPoint.x + Scalar(0.5)), (unsigned int)(viewPoint.y + Scalar(0.5)), width, height));

			Vector3 direction(backgroundOrientation.inverted() * pickingRay.direction());

			const Vector2 cameraCoordinate(camera->projectToImage(HomogenousMatrix4(true), direction));

			return CGPointMake(double(cameraCoordinate.x()), double(cameraCoordinate.y()));
		}
	}
	catch (...)
	{
		// nothing to do here
	}

	return CGPointMake(-1.0, -1.0);
}

@end
