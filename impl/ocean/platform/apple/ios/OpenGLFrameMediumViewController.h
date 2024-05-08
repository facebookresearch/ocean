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

#include "ocean/math/Quaternion.h"

#include "ocean/media/FrameMedium.h"

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
 * @param display_R_medium Explicit rotation between medium and display, and invalid rotation to use the medium's and framebuffer's transformations to determine the rotation
 */
-(void)setFrameMedium:(const Ocean::Media::FrameMediumRef&)frameMedium withRotation:(const Ocean::Quaternion&)display_R_medium;

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

@end

#endif // META_OCEAN_PLATFORM_APPLE_IOS_OPEN_GL_FRAME_MEDIUM_VIEW_CONTROLLER_H
