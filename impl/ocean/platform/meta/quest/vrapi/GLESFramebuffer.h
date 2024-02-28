// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_GLES_FRAMEBUFFER_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_GLES_FRAMEBUFFER_H

#include "ocean/platform/meta/quest/vrapi/VrApi.h"

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include <VrApi_Helpers.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

/// Forward declaration.
class GLESFramebuffer;

/**
 * Definition of a vector holding framebuffer objects.
 * @see GLESFramebuffer.
 * @ingroup platformmetaquestvrapi
 */
typedef std::vector<GLESFramebuffer> GLESFramebuffers;

/**
 * This class encapsulates an OpenGLES framebuffer for Meta Quest devices implementing VrAPI's swap chain.
 * @ingroup platformmetaquestvrapi
 */
class OCEAN_PLATFORM_META_QUEST_VRAPI_EXPORT GLESFramebuffer
{
	public:

		/**
		 * Creates a new uninitialized framebuffer object.
		 * @see initialize().
		 */
		GLESFramebuffer();

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
		inline ovrTextureSwapChain* colorTextureSwapChain() const;

		/**
		 * The index of the current (active) texture in the swap chain.
		 * @return The index of the current texturewith range [0, textureSwapChainLength_ - 1]
		 */
		inline size_t textureSwapChainIndex() const;

		/**
		 * Initializes the framebuffer.
		 * @param colorFormat The OpenGL color format the framebuffer will have, must be valid
		 * @param width The width of the framebuffer in pixel, with range [1, infinity)
		 * @param height The height of the framebuffer in pixel, with range [1, infinity)
		 * @param multisamples The number of multisamples the framebuffer will have, with range [0, infinity)
		 * @param useStencilBuffer If stencil buffer should be used
		 * @return True, if succeeded
		 * @see release().
		 */
		bool initialize(const GLenum colorFormat, const unsigned int width, const unsigned int height, const unsigned int multisamples, const bool useStencilBuffer = false);

		/**
		 * Binds this framebuffer.
		 */
		void bind();

		/**
		 * Swaps foreground and background buffers (within the swap chain).
		 */
		void swap();

		/**
		 * Discard the depth buffer, so the tiler won't need to write it back out to memory.
		 */
		void invalidateDepthBuffer();

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
		 * Unbinds any framebuffer.
		 */
		static inline void unbind();

	protected:

		/// The width of the framebuffer in pixel, with range [0, infinity).
		unsigned int width_;

		/// The height of the framebuffer in pixel, with range [0, infinity).
		unsigned int height_;

		/// The number of multisamples the framebuffer applies, with range [0, infinity)
		unsigned int multisamples_;

		/// The OVR texture swap chain.
		ovrTextureSwapChain* colorTextureSwapChain_;

		/// The number of textures in the swap chain, with range [0, infinity).
		size_t textureSwapChainLength_;

		/// The index of the current (active) texture in the swap chain, with range [0, textureSwapChainLength_ - 1].
		size_t textureSwapChainIndex_;

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

inline ovrTextureSwapChain* GLESFramebuffer::colorTextureSwapChain() const
{
	ocean_assert(colorTextureSwapChain_ != nullptr);
	return colorTextureSwapChain_;
}

inline size_t GLESFramebuffer::textureSwapChainIndex() const
{
	ocean_assert(textureSwapChainIndex_ < textureSwapChainLength_);
	return textureSwapChainIndex_;
}

inline GLESFramebuffer::operator bool() const
{
	return isValid();
}

inline void GLESFramebuffer::unbind()
{
	ocean_assert(GL_NO_ERROR == glGetError());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	ocean_assert(GL_NO_ERROR == glGetError());
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_GLES_FRAMEBUFFER_H
