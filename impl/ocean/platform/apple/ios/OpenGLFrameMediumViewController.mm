/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/ios/OpenGLFrameMediumViewController.h"

#include "ocean/base/String.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/Manager.h"
#include "ocean/rendering/ObjectRef.h"

#import <UIKit/UIKit.h>

using namespace Ocean;

@implementation OpenGLFrameMediumViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	Log::debug() << "OpenGLFrameMediumViewController::viewDidLoad()";

	const ScopedLock scopedLock(lock_);

	adjustFovXToBackground_ = false;
	intermediateBackgroundAdjustFov_ = false;
	previousTouchX_ = -1.0f;
	previousTouchY_ = -1.0f;
	renderingIterations_ = 0u;
	viewInteractionEnabled_ = true;

	ocean_assert(renderingEngine_.isNull());

	// We acquire the rendering engine by name
	renderingEngine_ = Rendering::Manager::get().engine("GLESceneGraph");

	if (renderingEngine_.isNull())
	{
		Log::error() << "Failed to create the GLESceneGraph rendering engine";
		return;
	}

	// we create a framebuffer in which we will draw the content
	renderingFramebuffer_ = renderingEngine_->createFramebuffer();

	if (renderingFramebuffer_.isNull())
	{
		Log::error() << "Failed to create a GLES framebuffer";
		return;
	}

	// we create a view with perspective projection model
	renderingView_ = renderingEngine_->factory().createPerspectiveView();
	ocean_assert(renderingView_);

	if (renderingView_.isNull())
	{
		return;
	}

	// we define the background as black
	renderingView_->setBackgroundColor(RGBAColor(0, 0, 0));
	// we set the initial horizontal viewing angle of the view
	renderingView_->setFovX(Numeric::deg2rad(30));
	// we set the initial camera position
	renderingView_->setTransformation(HomogenousMatrix4(Vector3(0, 0, 20)));

	// we connect the view with the framebuffer
	renderingFramebuffer_->setView(renderingView_);

	if (intermediateBackgroundFrameMedium_)
	{
		[self setFrameMedium:intermediateBackgroundFrameMedium_ andAdjustFov:intermediateBackgroundAdjustFov_];
		intermediateBackgroundFrameMedium_.release();
	}

	Log::info() << "Succeeded to initialize the view";
}

- (void)dealloc
{
	Log::debug() << "OpenGLFrameMediumViewController::dealloc()";

	const ScopedLock scopedLock(lock_);

	ocean_assert((renderingEngine_ && renderingFramebuffer_) || (!renderingEngine_ && !renderingFramebuffer_));

	if (renderingEngine_)
	{
		const std::string engineName(renderingEngine_->engineName());
		const Timestamp timestampNow(true);

		Log::info() << "Render iterations " << renderingIterations_;
		Log::info() << "Real performance: " << String::toAString(1000.0 * double(timestampNow - renderingStartTimestamp_) / max(1.0, double(renderingIterations_)), 8u) << "ms / frame";

		intermediateBackgroundFrameMedium_.release();
		renderingUndistortedBackground_.release();
		renderingFramebuffer_.release();
		renderingEngine_.release();

		ocean_assert(Rendering::ObjectRefManager::get().hasEngineObject(engineName, true) == false);
	}
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

-(bool)setFovX:(Scalar)fovx
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingView_)
		{
			return renderingView_->setFovX(fovx);
		}
	}
	catch (const std::exception& exception)
	{
		Log::error() << exception.what();
	}

	return false;
}

-(bool)setBackgroundColor:(const RGBAColor&)color
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingView_)
		{
			return renderingView_->setBackgroundColor(color);
		}
	}
	catch (const std::exception& exception)
	{
		Log::error() << exception.what();
	}

	return false;
}

-(void)setViewInteractionEnabled:(bool)enabled
{
	const ScopedLock scopedLock(lock_);

	viewInteractionEnabled_ = enabled;
}

- (void)update
{
	const ScopedLock scopedLock(lock_);

	if (renderingFramebuffer_ && renderingEngine_)
	{
		renderingEngine_->update(Timestamp(true));
	}
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

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
	[super touchesBegan:touches withEvent:event];

	const ScopedLock scopedLock(lock_);

	UITouch* touch = [touches anyObject];
	CGPoint point = [touch locationInView:self.view];

	previousTouchX_ = float(point.x);
	previousTouchY_ = float(point.y);
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
	[super touchesMoved:touches withEvent:event];

	const ScopedLock scopedLock(lock_);

	if (previousTouchX_ != -1.0f && previousTouchY_ != -1.0f && renderingFramebuffer_)
	{
		if (viewInteractionEnabled_)
		{
			try
			{
				UITouch* touch = [touches anyObject];
				CGPoint point = [touch locationInView:self.view];

				const float x = float(point.x);
				const float y = float(point.y);

				const Rendering::ViewRef view(renderingFramebuffer_->view());

				Scalar xDifference = Scalar(previousTouchX_ - x);
				Scalar yDifference = Scalar(previousTouchY_ - y);

				Quaternion orientation = view->transformation().rotation();

				Vector3 xAxis(1, 0, 0);
				Vector3 yAxis(0, 1, 0);
				Scalar factor = Scalar(0.5);

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
		else
		{
			UITouch* touch = [touches anyObject];
			CGPoint point = [touch locationInView:self.view];

			previousTouchX_ = float(point.x);
			previousTouchY_ = float(point.y);
		}
	}
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
	[super touchesEnded:touches withEvent:event];

	const ScopedLock scopedLock(lock_);

	previousTouchX_ = -1.0f;
	previousTouchY_ = -1.0f;
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
	[super touchesCancelled:touches withEvent:event];

	const ScopedLock scopedLock(lock_);

	previousTouchX_ = -1.0f;
	previousTouchY_ = -1.0f;
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
