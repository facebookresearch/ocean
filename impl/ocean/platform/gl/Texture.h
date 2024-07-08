/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_GL_GL_TEXTURE_H
#define META_OCEAN_PLATFORM_GL_GL_TEXTURE_H

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
 * This class implements a 2D OpenGL texture.
 * @ingroup platformgl
 */
class OCEAN_PLATFORM_GL_EXPORT Texture : public ContextAssociated
{
	public:

		/**
		 * Creates a new OpenGL texture object.
		 */
		Texture();

		/**
		 * Creates a new OpenGL texture object.
		 * @param frame The frame for the texture
		 */
		explicit Texture(const Frame& frame);

		/**
		 * Destructs a texture.
		 */
		~Texture();

		/**
		 * Returns the id of the texture.
		 * @return The texture's id, 0 if the texture has not been initialized yet
		 */
		inline GLuint id() const;

		/**
		 * Resizes the texture. without uploading any texture data.
		 * @param frameType The frameType to set
		 * @param enableFiltering If set, the best available filtering (either tri-linear filtering or anisotropic filtering) is enabled; otherwise nearest neighbor sampling is used.
		 * @return True, if succeeded
		 */
		bool resize(const FrameType& frameType, const bool enableFiltering = true);

		/**
		 * Sets the image content of the texture.
		 * Beware: If filtering is disabled, then no mipmaps are generated.
		 * @param frame The frame to set
		 * @param enableFiltering If set, the best available filtering (either tri-linear filtering or anisotropic filtering) is enabled; otherwise nearest neighbor sampling is used.
		 * @return True, if succeeded
		 */
		bool update(const Frame& frame, const bool enableFiltering = true);

		/**
		 * Generates mipmaps.
		 */
		void updateMipmap();

		/**
		 * Bind this texture.
		 */
		inline void bind();

		/**
		 * Bind this texture to a given shader program.
		 * @param programId The id of the shader program
		 * @param attributeName The name of the texture attribute
		 * @param index The index of the texture
		 * @return True, if succeeded
		 */
		bool bindToProgram(const GLuint programId, const std::string& attributeName, const unsigned int index = 0u);

		/**
		 * Unbinds this texture.
		 */
		inline void unbind();

		/**
		 * Releases the texture.
		 */
		void release();

		/**
		 * Returns the frame type of this texture object.
		 * @return The frame type
		 */
		inline const FrameType& frameType() const;

		/**
		 * Returns whether this object holds a valid OpenGL texture object.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Translates a pixel format to a corresponding OpenGL texture format.
		 * @param pixelFormat The pixel format to translate
		 * @param internalFormat Optional the corresponding internal texture format
		 * @return The corresponding texture format, 0 if no corresponding format exists
		 */
		static GLenum pixelFormat2textureFormat(const FrameType::PixelFormat pixelFormat, GLint* internalFormat = nullptr);

	protected:

		/// The OpenGL texture id.
		GLuint textureId;

		/// OpenGL texture (pixel) format.
		GLuint textureFormat;

		/// The frame type of the texture.
		FrameType textureFrameType;
};

inline GLuint Texture::id() const
{
	return textureId;
}

inline void Texture::bind()
{
	ocean_assert(GL_NO_ERROR == glGetError());
	glBindTexture(GL_TEXTURE_2D, textureId);
	ocean_assert(GL_NO_ERROR == glGetError());
}

inline void Texture::unbind()
{
	ocean_assert(GL_NO_ERROR == glGetError());
	glBindTexture(GL_TEXTURE_2D, 0);
	ocean_assert(GL_NO_ERROR == glGetError());
}

inline const FrameType& Texture::frameType() const
{
	return textureFrameType;
}

inline Texture::operator bool() const
{
	return textureId != 0u;
}

}

}

}

#endif // META_OCEAN_PLATFORM_GL_GL_TEXTURE_H
