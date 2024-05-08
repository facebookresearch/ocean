/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_MEMORY_FRAMEBUFFER_H
#define META_OCEAN_PLATFORM_GL_MEMORY_FRAMEBUFFER_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/Framebuffer.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements a framebuffer allowing to render into an off-screen framebuffer memory and not into an on-screen framebuffer.
 * @see setFrame().
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT MemoryFramebuffer : public Framebuffer
{
	public:

		/**
		 * Creates a new framebuffer object with initially no associated context.
		 */
		MemoryFramebuffer();

		/**
		 * Creates a new framebuffer object with a given associated context.
		 * @param context The associated context
		 */
		explicit MemoryFramebuffer(Context& context);

		/**
		 * Destructs this object.
		 */
		virtual ~MemoryFramebuffer();

		/**
		 * Returns whether this framebuffer is currently bound.
		 * @return True, if so
		 * @see bind(), unbind().
		 */
		inline bool isBound() const;

		/**
		 * Sets the context of this object.
		 * @see Framebuffer::setContext().
		 */
		virtual bool setContext(Context& context);

		/**
		 * Sets the frame in which the framebuffer will be copied.
		 * The frame type of the given frame must match with the current dimension and pixel format of the framebuffer.
		 * @param frame The receiving the pixel data from the framebuffer
		 * @return True, if succeeded
		 * @see unsetTargetFrame().
		 */
		bool setTargetFrame(Frame& frame);

		/**
		 * Unsets the frame in which the framebuffer will be copied.
		 * @see setTargetFrame().
		 */
		inline void unsetTargetFrame();

		/**
		 * Resizes the off-screen framebuffer.
		 * @see Framebuffer::resize().
		 */
		virtual bool resize(const unsigned int width, const unsigned int height, const GLenum internalFormat = GL_RGBA8);

		/**
		 * Binds the framebuffer as target framebuffer.
		 * @see Framebuffer::bind(), isBound().
		 */
		virtual bool bind();

		/**
		 * Unbinds the framebuffer as target framebuffer.
		 * If a target frame is defined the pixel data of the framebuffer will be copied into the target frame just before the framebuffer is unbound.<br>
		 * @see Framebuffer::unbind(), isBound(), setTargetFrame().
		 */
		virtual bool unbind();

		/**
		 * Releases the object.
		 * @return True, if succeeded
		 */
		bool release();

	protected:

		/// The id of the framebuffer object.
		GLuint framebufferObjectId;

		/// The id of the render buffer.
		GLuint framebufferRenderBufferId;

		/// The id of the intermediate framebuffer object.
		GLuint framebufferIntermediateObjectId;

		/// The id of the intermediate render buffer object.
		GLuint framebufferIntermediateRenderBufferId;

		/// True, if this framebuffer is currently bound.
		bool framebufferIsBound;

		/// The frame in which the pixel data from the framebuffer will be copied, if defined
		Frame* framebufferFrame;
};

inline bool MemoryFramebuffer::isBound() const
{
	return framebufferIsBound;
}

inline void MemoryFramebuffer::unsetTargetFrame()
{
	framebufferFrame = nullptr;
}


}

}

}

#endif // META_OCEAN_PLATFORM_GL_MEMORY_FRAMEBUFFER_H
