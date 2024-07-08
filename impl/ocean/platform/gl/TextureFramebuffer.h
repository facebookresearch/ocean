/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_TEXTURE_FRAMEBUFFER_H
#define META_OCEAN_PLATFORM_GL_TEXTURE_FRAMEBUFFER_H

#include "ocean/platform/gl/GL.h"
#include "ocean/platform/gl/Framebuffer.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

/**
 * This class implements a texture framebuffer allowing to render into a texture object.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT TextureFramebuffer : public Framebuffer
{
	public:

		/**
		 * Creates a new framebuffer object.
		 */
		TextureFramebuffer() = default;

		/**
		 * Creates a new framebuffer object with know associated context.
		 * @param context The associated context of this framebuffer
		 */
		explicit TextureFramebuffer(Context& context);

		/**
		 * Returns the number of samples.
		 * @return The number of samples, with range [1, infinity), 1 by default
		 */
		inline unsigned int samples() const;

		/**
		 * Returns the id of the texture of this framebuffer.
		 * @return The texture's id
		 */
		inline GLuint textureId() const;

		/**
		 * (Re-)sets the number of samples of this texture framebuffer.
		 * @param samples The number of samples to set, with range [1, infinity)
		 * @return True, if succeeded
		 */
		bool setSamples(const unsigned int samples);

		/**
		 * Resizes the off-screen framebuffer.
		 * @see Framebuffer::resize().
		 */
		virtual bool resize(const unsigned int width, const unsigned int height, const GLenum internalFormat = GL_RGBA);

		/**
		 * Binds the framebuffer as target framebuffer.
		 * @see Framebuffer::bind().
		 */
		virtual bool bind();

		/**
		 * Bind this texture to a given shader program.
		 * @param programId The id of the shader program
		 * @param attributeName The name of the texture attribute
		 * @param index The index of the texture
		 * @return True, if succeeded
		 */
		bool bindToProgram(const GLuint programId, const std::string& attributeName, const unsigned int index = 0u);

		/**
		 * Unbinds the framebuffer as target framebuffer.
		 * @see Framebuffer::unbind().
		 */
		virtual bool unbind();

		/**
		 * Releases this framebuffer.
		 * @return True, if succeeded
		 */
		bool release();

		/**
		 * Returns whether this framebuffer holds a valid texture.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// The id of the framebuffer object.
		GLuint objectId_ = 0u;

		/// The id of the texture.
		GLuint textureId_ = 0u;

		/// The number of samples.
		unsigned int samples_ = 1u;
};

inline unsigned int TextureFramebuffer::samples() const
{
	return samples_;
}

inline GLuint TextureFramebuffer::textureId() const
{
	return textureId_;
}

inline TextureFramebuffer::operator bool() const
{
	ocean_assert(objectId_ == 0u && textureId_ == 0u || objectId_ != 0u && textureId_ != 0u);
	return textureId_ != 0u;
}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_TEXTURE_FRAMEBUFFER_H
