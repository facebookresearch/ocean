/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_IOS_OPEN_GL_FRAME_MEDIUM_VIEW_CONTROLLER_H
#define META_OCEAN_PLATFORM_APPLE_IOS_OPEN_GL_FRAME_MEDIUM_VIEW_CONTROLLER_H

#include "ocean/platform/apple/ios/IOS.h"
#include "ocean/platform/apple/ios/OpenGLViewController.h"

#include "ocean/math/Line3.h"
#include "ocean/math/Quaternion.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/RGBAColor.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/UndistortedBackground.h"

#ifndef __OBJC__
	#error Platform::Apple::IOS::OpenGLFrameMediumViewController.h needs to be included from an ObjectiveC++ file
#endif

/**
 * The OpenGLViewController implements a view controller that is mainly able to display medium objects.
 * The view controller is derived from an OpenGLES view controller so that we can ensure the best performance possible.<br>
 * This class can be seen as a FrameMediumView equivalent using OpenGLES.
 * @see Apple::MacOS::FrameMediumView.
 * @ingroup platformappleios
 */
@interface OpenGLFrameMediumViewController : OpenGLViewController

/**
 * Sets or changes the frame medium to be displayed.
 * @param frameMedium The frame medium to be displayed, must be valid
 */
-(void)setFrameMedium:(const Ocean::Media::FrameMediumRef&)frameMedium;

/**
 * Sets or changes the frame medium to be displayed.
 * @param frameMedium The frame medium to be displayed, must be valid
 * @param adjustFov True, to adjust the view's field of view to the medium's field of view; False, to keep the view's field of view as is
 */
-(void)setFrameMedium:(const Ocean::Media::FrameMediumRef&)frameMedium andAdjustFov:(bool)adjustFov;

/**
 * Sets or changes the frame medium to be displayed.
 * @param frameMedium The frame medium to be displayed, must be valid
 * @param display_R_medium Explicit rotation between medium and display, and invalid rotation to use the medium's and framebuffer's transformations to determine the rotation
 */
-(void)setFrameMedium:(const Ocean::Media::FrameMediumRef&)frameMedium withRotation:(const Ocean::Quaternion&)display_R_medium;

/**
 * Sets or changes the frame medium to be displayed.
 * @param frameMedium The frame medium to be displayed, must be valid
 * @param display_R_medium Explicit rotation between medium and display, and invalid rotation to use the medium's and framebuffer's transformations to determine the rotation
 * @param adjustFov True, to adjust the view's field of view to the medium's field of view; False, to keep the view's field of view as is
 */
-(void)setFrameMedium:(const Ocean::Media::FrameMediumRef&)frameMedium withRotation:(const Ocean::Quaternion&)display_R_medium andAdjustFov:(bool)adjustFov;

/**
 * Returns the current frame medium.
 * @return The frame medium which is currently displayed
 */
-(Ocean::Media::FrameMediumRef)frameMedium;

/**
 * Converts a 2D location defined in the view's coordinate system to a location defined in the coordinate system of the medium (an image frame).
 * @param viewPoint The 2D location within this view
 * @return The corresponding 2D location within the image frame of the medium, a negative coordinate (-1, -1), in case no valid mapping exists
 */
-(CGPoint)view2medium:(CGPoint)viewPoint;

/**
 * Updates the rendering engine.
 */
-(void)update;

/**
 * Sets the horizontal field of view for this view.
 * @param fovx Field of view to set in radian
 * @return True, if succeeded
 */
-(bool)setFovX:(Ocean::Scalar)fovx;

/**
 * Sets the background color of this view.
 * @param color Background color to set
 * @return True, if succeeded
 */
-(bool)setBackgroundColor:(const Ocean::RGBAColor&)color;

/**
 * Sets whether the user can change the view's position and rotation by touch events.
 * @param enabled True, to allow the interaction; False, to disable the interaction
 */
-(void)setViewInteractionEnabled:(bool)enabled;

@end

@interface OpenGLFrameMediumViewController ()
{
	@protected

		/// The rendering engine to be used.
		Ocean::Rendering::EngineRef renderingEngine_;

		/// The framebuffer in which the result will be rendered.
		Ocean::Rendering::FramebufferRef renderingFramebuffer_;

		/// The rendering view.
		Ocean::Rendering::PerspectiveViewRef renderingView_;

		/// The undistorted background.
		Ocean::Rendering::UndistortedBackgroundRef renderingUndistortedBackground_;

		/// The frame medium of the background which is stored as long as the view hasn't been initialized.
		Ocean::Media::FrameMediumRef intermediateBackgroundFrameMedium_;

		/// True, to adjust the field of view of the view automatically so that the background medium is entirely covered.
		bool intermediateBackgroundAdjustFov_;

		/// State determining that the view's field of view has to be adjusted to the background's field of view.
		bool adjustFovXToBackground_;

		/// Previous horizontal touch position.
		float previousTouchX_;

		/// Previous vertical touch position.
		float previousTouchY_;

		/// Rendering start timestamp.
		Ocean::Timestamp renderingStartTimestamp_;

		/// Rendering iterations.
		unsigned int renderingIterations_;

		/// True, to allow the user to change the view's position and rotation by touch events; False, to disable the user interaction.
		bool viewInteractionEnabled_;

		/// View lock object.
		Ocean::Lock lock_;
}

@end

#endif // META_OCEAN_PLATFORM_APPLE_IOS_OPEN_GL_FRAME_MEDIUM_VIEW_CONTROLLER_H
