/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_OPENXR_GLES_FRAMEBUFFER_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_GLES_FRAMEBUFFER_H

#include "ocean/platform/meta/quest/openxr/OpenXR.h"

#include "ocean/platform/gles/EGLContext.h"

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include <jni.h>

#ifndef XR_USE_GRAPHICS_API_OPENGL_ES
	#define XR_USE_GRAPHICS_API_OPENGL_ES
#endif

#ifndef XR_USE_PLATFORM_ANDROID
	#define XR_USE_PLATFORM_ANDROID
#endif

#include <openxr/openxr_platform.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

/// Forward declaration.
class GLESFramebuffer;

/**
 * Definition of a vector holding framebuffer objects.
 * @see GLESFramebuffer.
 * @ingroup platformmetaquestopenxr
 */
typedef std::vector<GLESFramebuffer> GLESFramebuffers;

/**
 * This class encapsulates an OpenGLES framebuffer for Meta Quest devices implementing OpenXR's swap chain.
 * @ingroup platformmetaquestopenxr
 */
class OCEAN_PLATFORM_META_QUEST_OPENXR_EXPORT GLESFramebuffer
{
	protected:

		typedef std::vector<XrSwapchainImageOpenGLESKHR> XrSwapchainImages;

	public:

		/**
		 * Creates a new uninitialized framebuffer object.
		 * @see initialize().
		 */
		GLESFramebuffer() = default;

		/**
		 * Move constructor.
		 * @param framebuffer The framebuffer to be moved
		 */
		GLESFramebuffer(GLESFramebuffer&& framebuffer);

		/**
		 * Disabled copy constructor.
		 * @param framebuffer The framebuffer which would be copied
		 */
		GLESFramebuffer(const GLESFramebuffer& framebuffer) = delete;

		/**
		 * Releases all framebuffer resources and disposes the object.
		 */
		~GLESFramebuffer();

		/**
		 * Returns the width of the framebuffer.
		 * @return The framebuffer's width, in pixel, with range [0, infinity)
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of the framebuffer.
		 * @return The framebuffer's height, in pixel, with range [0, infinity)
		 */
		inline unsigned int height() const;

		/**
		 * Returns the multisamples the framebuffer has.
		 * @return The number of multisampes, with range [0, infinity)
		 */
		inline unsigned int multisamples() const;

		/**
		 * The VrAPI's swap chain for the framebuffer.
		 * @return The framebuffer's swap chain
		 */
		inline const XrSwapchain& xrSwapchain() const;

		/**
		 * The index of the current (active) texture in the swap chain.
		 * @return The index of the current texturewith range [0, textureSwapChainLength_ - 1]
		 */
		inline size_t textureSwapChainIndex() const;

		/**
		 * Initializes the framebuffer.
		 * @param xrSession The OpenXR session to be used must be valid
		 * @param colorFormat The OpenGL color format the framebuffer will have, must be valid
		 * @param width The width of the framebuffer in pixel, with range [1, infinity)
		 * @param height The height of the framebuffer in pixel, with range [1, infinity)
		 * @param multisamples The number of multisamples the framebuffer will have, with range [0, infinity)
		 * @param useStencilBuffer If stencil buffer should be used
		 * @return True, if succeeded
		 * @see release().
		 */
		bool initialize(const XrSession& xrSession, const GLenum colorFormat, const unsigned int width, const unsigned int height, const unsigned int multisamples, const bool useStencilBuffer = false);

		/**
		 * Binds this framebuffer.
		 * @return True, if succeeded
		 * @see unbind().
		 */
		bool bind();

		/**
		 * Unbinds this framebuffer.
		 * @return True, if succeeded
		 * @see bind().
		 */
		bool unbind();

		/**
		 * Releases all framebuffer resources.
		 * @see initialize().
		 */
		void release();

		/**
		 * Returns whether this framebuffer is valid (whether the framebuffer is initialized) and whether it can be used.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns whether this framebuffer is valid (whether the framebuffer is initialized) and whether it can be used.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param framebuffer The framebuffer to be moved
		 * @return Reference to this framebuffer
		 */
		GLESFramebuffer& operator=(GLESFramebuffer&& framebuffer);

		/**
		 * Disabled copy operator.
		 * @param framebuffer The framebuffer which would be copied
		 * @return Reference to this framebuffer
		 */
		GLESFramebuffer& operator=(const GLESFramebuffer& framebuffer) = delete;

		/**
		 * Translates an OpenXR spwapchain format to a readable string.
		 * @param swapchainFormat The spawchain format to be translated
		 * @param readableSwapchainFormat The resulting readable string
		 * @return True, if the format could be translated
		 */
		static bool translateSwapchainFormat(const int64_t swapchainFormat, std::string& readableSwapchainFormat);

	protected:

		/// The width of the framebuffer in pixel, with range [0, infinity).
		unsigned int width_ = 0u;

		/// The height of the framebuffer in pixel, with range [0, infinity).
		unsigned int height_ = 0u;

		/// The number of multisamples the framebuffer applies, with range [0, infinity)
		unsigned int multisamples_ = 0u;

		/// The handle of the OpenXR swap chain.
		XrSwapchain xrSwapchain_ = XR_NULL_HANDLE;

		/// The images of the OpenXR swapchain.
		XrSwapchainImages xrSwapchainImages_;

		/// The index of the current (active) texture in the swap chain, with range [0, textureSwapChainLength_ - 1].
		size_t textureSwapChainIndex_ = 0;

		/// The color buffers of this framebuffer, one for each depth buffer.
		std::vector<GLuint> colorBuffers_;

		/// The depth buffers of this framebuffer, one for each color buffer.
		std::vector<GLuint> depthBuffers_;
};

inline unsigned int GLESFramebuffer::width() const
{
	return width_;
}

inline unsigned int GLESFramebuffer::height() const
{
	return height_;
}

inline unsigned int GLESFramebuffer::multisamples() const
{
	return multisamples_;
}

inline const XrSwapchain& GLESFramebuffer::xrSwapchain() const
{
	ocean_assert(xrSwapchain_ != XR_NULL_HANDLE);
	return xrSwapchain_;
}

inline size_t GLESFramebuffer::textureSwapChainIndex() const
{
	ocean_assert(textureSwapChainIndex_ < xrSwapchainImages_.size());
	return textureSwapChainIndex_;
}

inline GLESFramebuffer::operator bool() const
{
	return isValid();
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_GLES_FRAMEBUFFER_H
