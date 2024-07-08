/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_WINDOW_FRAMEBUFFER_H
#define META_OCEAN_PLATFORM_GL_WINDOW_FRAMEBUFFER_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/Framebuffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements a framebuffer allowing to render into a window.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT WindowFramebuffer : public Framebuffer
{
	public:

		/**
		 * Creates a new framebuffer object with initially no associated context.
		 */
		WindowFramebuffer();

		/**
		 * Creates a new framebuffer object with a given associated context.
		 * @param context The associated context
		 */
		explicit WindowFramebuffer(Context& context);

		/**
		 * Binds the framebuffer as target framebuffer.
		 * @see Framebuffer::bind().
		 */
		virtual bool bind();

		/**
		 * Unbinds the framebuffer as target framebuffer.
		 * @see Framebuffer::unbind().
		 */
		virtual bool unbind();
};

}

}

}

#endif // META_OCEAN_PLATFORM_GL_WINDOW_FRAMEBUFFER_H
