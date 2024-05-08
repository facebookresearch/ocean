/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/ios/OpenGLFrameMediumViewController.h"

#include "ocean/base/Thread.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/Manager.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/UndistortedBackground.h"

#import <UIKit/UIKit.h>

using namespace Ocean;

@interface OpenGLFrameMediumViewController ()
{
	/// The rendering engine to be used.
	Rendering::EngineRef renderingEngine_;

	/// The framebuffer in which the result will be rendered.
	Rendering::FramebufferRef renderingFramebuffer_;

	/// The rendering view.
	Rendering::PerspectiveViewRef renderingView_;

	/// The undistorted background.
	Rendering::UndistortedBackgroundRef renderingUndistortedBackground_;
}

@end

@implementation OpenGLFrameMediumViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

	// we aquire any existing rendering engine using OpenGL ES (use individual param for other graphics API)
	renderingEngine_ = Rendering::Manager::get().engine("", Rendering::Engine::API_OPENGLES);

	// we ensure that we have a valid rendering engine before we proceed
	if (renderingEngine_.isNull())
	{
		return;
	}

	// we create a framebuffer in which we will draw the content
	renderingFramebuffer_ = renderingEngine_->createFramebuffer();

	// we create a view with perspective projection model
	renderingView_ = renderingEngine_->factory().createPerspectiveView();

	if (renderingView_.isNull())
	{
		return;
	}

	// we set the initial horizontal viewing angle of the view (however, this fov will be adjusted later so that it perfectly fits with the video-background)
	renderingView_->setFovX(Numeric::deg2rad(35));
	// we define the background as black (however, due to the video background its color will not be visible)
	renderingView_->setBackgroundColor(RGBAColor(0, 0, 0));

	// we connect the view with the framebuffer
	renderingFramebuffer_->setView(renderingView_);

	// as we have a valid video object we now can create the corresponding background, which will be added to the view
	renderingUndistortedBackground_ = renderingEngine_->factory().createUndistortedBackground();

	if (renderingUndistortedBackground_.isNull())
	{
		return;
	}

	// we connect the background to the view
	renderingView_->addBackground(renderingUndistortedBackground_);

	// in general, we want background threads to have the highest priority (as displaying the medium does not need much performance)
	Thread::setThreadPriority(Thread::PRIORITY_IDLE);
}

-(void)setFrameMedium:(const Media::FrameMediumRef&)frameMedium
{
	[self setFrameMedium:frameMedium withRotation:Quaternion(false)];
}

-(void)setFrameMedium:(const Media::FrameMediumRef&)frameMedium withRotation:(const Quaternion&)display_R_medium
{
	if (renderingUndistortedBackground_.isNull())
	{
		return;
	}

	if (frameMedium)
	{
		if (display_R_medium.isValid())
		{
			renderingUndistortedBackground_->setOrientation(display_R_medium);
		}
		else
		{
			const HomogenousMatrix4 device_T_display = renderingFramebuffer_.isNull() ? HomogenousMatrix4(true) : renderingFramebuffer_->device_T_display();
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
	}

	if (frameMedium)
	{
		// we connect the background to the live video
		renderingUndistortedBackground_->setMedium(frameMedium);
	}
	else
	{
		renderingView_->removeBackground(renderingUndistortedBackground_);

		renderingUndistortedBackground_ = renderingEngine_->factory().createUndistortedBackground();

		renderingView_->addBackground(renderingUndistortedBackground_);
	}
}

-(Ocean::Media::FrameMediumRef)frameMedium
{
	if (renderingUndistortedBackground_)
	{
		return renderingUndistortedBackground_->medium();
	}

	return Ocean::Media::FrameMediumRef();
}

- (void)update
{
	if (renderingFramebuffer_ && renderingEngine_)
	{
		renderingEngine_->update(Timestamp(true));
	}
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	if (renderingFramebuffer_.isNull() || renderingView_.isNull())
	{
		return;
	}

	const float scale = [UIScreen mainScreen].scale;

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

	// now we determine the ideal field of view for our device (the field of view that will be rendered should be slightly smaller than the field of view of the camera)
	const Scalar idealFovX = renderingView_->idealFovX();

	if (idealFovX != renderingView_->fovX())
	{
		renderingView_->setFovX(idealFovX);
		Log::info() << "The field of view has been adjusted to " << Numeric::rad2deg(idealFovX) << " degree.";
	}

	// we simply invoke the rendering of the framebuffer
	renderingFramebuffer_->render();
}

- (CGPoint)view2medium:(CGPoint)viewPoint
{
	if (renderingView_.isNull() || renderingUndistortedBackground_.isNull())
	{
		return CGPointMake(-1.0, -1.0);
	}

	ocean_assert(viewPoint.x >= 0.0 && viewPoint.x <= self.view.bounds.size.width);
	ocean_assert(viewPoint.y >= 0.0 && viewPoint.y <= self.view.bounds.size.height);

	const Scalar viewFovX = renderingView_->fovX();

	const unsigned int viewWidth = (unsigned int)self.view.bounds.size.width;
	const unsigned int viewHeight = (unsigned int)self.view.bounds.size.height;

	const PinholeCamera viewCamera(viewWidth, viewHeight, viewFovX);

	const Vector3 objectPoint = viewCamera.vector(Vector2(Scalar(viewPoint.x), Scalar(viewPoint.y)));

	const PinholeCamera mediumCamera = renderingUndistortedBackground_->camera();

	const Vector2 mediumPoint = mediumCamera.projectToImage<true>(HomogenousMatrix4(true), objectPoint, false);

	return CGPointMake(double(mediumPoint.x()), double(mediumPoint.y()));
}

@end
