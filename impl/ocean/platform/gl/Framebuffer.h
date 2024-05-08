/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_GL_FRAMEBUFFER_H
#define META_OCEAN_PLATFORM_GL_GL_FRAMEBUFFER_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/ContextAssociated.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements the base class for an OpenGL framebuffers.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT Framebuffer : public ContextAssociated
{
	public:

		/**
		 * Destructs a framebuffer object.
		 */
		virtual ~Framebuffer();

		/**
		 * Returns the width of this framebuffer in pixel.
		 * @return The framebuffer's width, with range [0, infinity)
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of this framebuffer in pixel.
		 * @return The framebuffer's height, with range [0, infinity)
		 */
		inline unsigned int height() const;

		/**
		 * Returns the internal format of this framebuffer.
		 * @return The framebuffer's internal format
		 */
		inline GLenum internalFormat() const;

		/**
		 * Resizes the off-screen framebuffer.
		 * @param width The width of the framebuffer in pixel, with range [0, infinity)
		 * @param height The height of the framebuffer in pixel, with range [0, infinity)
		 * @param internalFormat The internal format of the framebuffer
		 * @return True, if succeeded
		 */
		virtual bool resize(const unsigned int width, const unsigned int height, const GLenum internalFormat = GL_RGBA8);

		/**
		 * Binds the framebuffer as target framebuffer.
		 * @return True, if succeeded
		 */
		virtual bool bind() = 0;

		/**
		 * Unbinds the framebuffer as target framebuffer.
		 * @return True, if succeeded
		 */
		virtual bool unbind() = 0;

		/**
		 * Initializes the OpenGL framebuffer.
		 * @return True, if succeeded
		 */
		virtual bool initOpenGL();

	protected:

		/**
		 * Creates a new framebuffer object which is initially not assocated with any context.
		 */
		inline Framebuffer();

		/**
		 * Creates a new framebuffer object.
		 * @param context The OpenGL context which will be associated with the framebuffer
		 */
		inline Framebuffer(Context& context);

		/**
		 * Translates an OpenGL framebuffer format to a corresponding pixel format.
		 * @param framebufferFormat The framebuffer format to translate
		 * @return The corresponding pixel format, FORMAT_UNDEFINED if no corresponding pixel format exists
		 */
		static FrameType::PixelFormat framebufferFormat2pixelFormat(const GLenum framebufferFormat);

	protected:

		/// The width of the framebuffer in pixel, with range [0, infinity).
		unsigned int framebufferWidth;

		/// The height of the framebuffer in pixel, with range [0, infinity).
		unsigned int framebufferHeight;

		/// The internal format of the framebuffer.
		GLenum framebufferInternalFormat;
};

inline Framebuffer::Framebuffer() :
	ContextAssociated(),
	framebufferWidth(0u),
	framebufferHeight(0u),
	framebufferInternalFormat(0u)
{
	// nothing to do here
}

inline Framebuffer::Framebuffer(Context& context) :
	ContextAssociated(context),
	framebufferWidth(0u),
	framebufferHeight(0u),
	framebufferInternalFormat(0u)
{
	// nothing to do here
}

inline unsigned int Framebuffer::width() const
{
	return framebufferWidth;
}

inline unsigned int Framebuffer::height() const
{
	return framebufferHeight;
}

inline GLenum Framebuffer::internalFormat() const
{
	return framebufferInternalFormat;
}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_GL_FRAMEBUFFER_H
