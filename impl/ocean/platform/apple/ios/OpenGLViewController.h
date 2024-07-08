/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_IOS_OPEN_GL_VIEW_CONTROLLER_H
#define META_OCEAN_PLATFORM_APPLE_IOS_OPEN_GL_VIEW_CONTROLLER_H

#include "ocean/platform/apple/ios/IOS.h"

#ifndef __OBJC__
	#error Platform::Apple::IOS::OpenGLViewController.h needs to be included from an ObjectiveC++ file
#endif

#import <GLKit/GLKit.h>

/**
 * The ViewController responsible for the OpenGL context.
 * The implementation is more or less identical to the GameViewController example of Xcode without any specific modifications.
 * @ingroup platformappleios
 */
@interface OpenGLViewController : GLKViewController

/**
 * Makes the OpenGL context of this controller the current context.
 */
- (void)makeOpenGLContextCurrent;

@end

#endif // META_OCEAN_PLATFORM_APPLE_IOS_OPEN_GL_VIEW_CONTROLLER_H
