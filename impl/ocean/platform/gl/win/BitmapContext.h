/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_WIN_BITMAP_CONTEXT_H
#define META_OCEAN_PLATFORM_GL_WIN_BITMAP_CONTEXT_H

#include "ocean/platform/gl/win/GLWin.h"

#include "ocean/platform/gl/Context.h"

#include "ocean/platform/win/Bitmap.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

namespace Win
{

/**
 * This class implements an OpenGL context based on a bitmap.
 * Beware: Only OpenGL 1.1 is supported.<br>
 * @ingroup platformglwin
 */
class OCEAN_PLATFORM_GL_WIN_EXPORT BitmapContext : public Context
{
	public:

		/**
		 * Creates a new bitmap framebuffer with default values.
		 */
		BitmapContext() = default;

		/**
		 * Creates a new bitmap framebuffer with specified dimension, pixel format and pixel origin.
		 * @param width The width of the framebuffer in pixel, with range [0, infinity)
		 * @param height The height of the framebuffer in pixel, with range [0, infinity)
		 * @param pixelFormat The pixel format of the framebuffer
		 * @param pixelOrigin The pixel origin of the framebuffer
		 */
		BitmapContext(const unsigned int width, const unsigned int height, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin);

		/**
		 * Destructs this context.
		 */
		~BitmapContext() override;

		/**
		 * Returns the bitmap of this framebuffer.
		 * @return The framebuffer's bitmap
		 */
		inline const Platform::Win::Bitmap& bitmap() const;

		/**
		 * Sets the size (the OpenGL viewport) of this framebuffer with upper left corner set to (0, 0).
		 * @param width The width of the framebuffer in pixel, with range [0, infinity)
		 * @param height The height of the framebuffer in pixel, with range [0, infinity)
		 * @return True, if succeeded
		 */
		bool setSize(const unsigned int width, const unsigned int height);

		/**
		 * Makes the OpenGL rendering context of this framebuffer the calling thread's current rendering context or makes the calling thread's current rendering context not longer current.
		 * @see OpenGLFramebuffer::makeCurrent().
		 */
		void makeCurrent(const bool state = true) override;

	protected:

		/**
		 * Creates the OpenGL context for this framebuffer.
		 * @see Context::createOpenGLContext().
		 */
		bool createOpenGLContext(const bool initializeOpenGL31 = false, const unsigned int multisamples = 1u) override;

		/**
		 * Releases the OpenGL context of this framebuffer.
		 * @see Context::releaseOpenGLContext().
		 */
		bool releaseOpenGLContext() override;

	protected:

		/// The bitmap of the framebuffer on which the OpenGL context will be created.
		Platform::Win::Bitmap bitmap_;

		/// The pixel format of the framebuffer.
		FrameType::PixelFormat pixelFormat_ = FrameType::FORMAT_UNDEFINED;

		//// The pixel origin of the framebuffer.
		FrameType::PixelOrigin pixelOrigin_ = FrameType::ORIGIN_INVALID;
};

inline const Platform::Win::Bitmap& BitmapContext::bitmap() const
{
	return bitmap_;
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_WIN_BITMAP_CONTEXT_H
