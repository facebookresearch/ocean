/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_IOS_OPEN_GL_RENDERER_VIEW_CONTROLLER_H
#define META_OCEAN_PLATFORM_APPLE_IOS_OPEN_GL_RENDERER_VIEW_CONTROLLER_H

#include "ocean/platform/apple/ios/IOS.h"
#include "ocean/platform/apple/ios/OpenGLViewController.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/RGBAColor.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/PerspectiveView.h"

#ifndef __OBJC__
	#error Platform::Apple::IOS::OpenGLRendererViewController.h needs to be included from an ObjectiveC++ file
#endif

/**
 * The OpenGLRendererViewController implements a view controller using the OpenGLESceneGraph renderer for iOS platform applications.
 * @ingroup platformappleios
 */
@interface OpenGLRendererViewController : OpenGLViewController

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

/**
 * Updates the rendering engine.
 */
-(void)update;

@end

@interface OpenGLRendererViewController ()
{
	@protected

		/// The rendering engine to be used.
		Ocean::Rendering::EngineRef renderingEngine_;

		/// The framebuffer in which the result will be rendered.
		Ocean::Rendering::FramebufferRef renderingFramebuffer_;

		/// The rendering view.
		Ocean::Rendering::PerspectiveViewRef renderingView_;

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

#endif // META_OCEAN_PLATFORM_APPLE_IOS_OPEN_GL_RENDERER_VIEW_CONTROLLER_H
