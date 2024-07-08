/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_FRAME_TEXTURE_2D_H
#define META_OCEAN_RENDERING_GLES_FRAME_TEXTURE_2D_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESTexture2D.h"

#include "ocean/rendering/FrameTexture2D.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class wraps a GLESceneGraph frame texture object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESFrameTexture2D final :
	virtual public GLESTexture2D,
	virtual public FrameTexture2D
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the name of the texture e.g., in a shader.
		 * @see Texture::textureName().
		 */
		std::string textureName() const override;

		/**
		 * Sets the name of the texture e.g., in a shader.
		 * @see Texture::setTextureName().
		 */
		bool setTextureName(const std::string& name) override;

		/**
		 * Sets or updates the texture with a given frame.
		 * @see FrameTexture2D::setTexture().
		 */
		bool setTexture(Frame&& frame) override;

		/**
		 * Sets or updates the texture with a given buffer.
		 * @see FrameTexture2D::setTexture().
		 */
		bool setTexture(CompressedFrame&& compressedFrame) override;

		/**
		 * Returns the frame type of this 2D texture.
		 * @see Texture2D::frameType()
		 */
		FrameType frameType() const override;

		/**
		 * Returns whether this texture contains at least one transparent pixel.
		 * @see Texture2D::hasTransparentPixel();
		 */
		bool hasTransparentPixel() const override;

		/**
		 * Returns the texture id of the texture.
		 * @return The id of the texture, 0 if invalid
		 */
		inline GLuint textureId() const;

		/**
		 * Binds this texture.
		 * @see GLESTexture::bindTexture().
		 */
		unsigned int bindTexture(GLESShaderProgram& shaderProgram, const unsigned int id) override;

		/**
		 * Returns whether the texture internally holds valid data.
		 * @see Texture::isValid().
		 */
		bool isValid() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph texture 2D object.
		 */
		GLESFrameTexture2D();

		/**
		 * Destructs a GLESceneGraph texture 2D object.
		 */
		~GLESFrameTexture2D() override;

		/**
		 * Updates the mipmap for this texture.
		 * @see GLESTexture::createMipmap().
		 */
		void createMipmap() override;

		/**
		 * Update function called by the framebuffer.
		 * @see DynamicObject::onDynamicUpdate().
		 */
		void onDynamicUpdate(const ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Updates the texture.
		 */
		void updateTexture();

		/**
		 * Determines the properties of the texture for a given frame type.
		 * @param frameType The frame type for which the properties will be determined, must be valid
		 * @param format The resulting GL format of the primary texture
		 * @param type The resulting GL type of the primary texture
		 * @return True, if succeeded; False, if the frame type is not supported
		 */
		static bool determineTextureProperties(const FrameType& frameType, GLenum& format, GLenum& type);

		/**
		 * Determines the OpenGL format for a given compressed texture format.
		 * @param compressedFormat The texture format for which the OpenGL format will be determined, must be valid
		 * @param internalFormat The resulting OpenGL texture format
		 * @return True, if succeeded
		 */
		static bool determineCompressedFormat(const CompressedFormat compressedFormat, GLenum& internalFormat);

		/**
		 * Returns the image size for a compressed texture.
		 * @param compressedFormat The texture format of the compressed texture, must be valid
		 * @param width The width of the texture in pixel, with range [1, infinity)
		 * @param height The height of the texture in pixel, with range [1, infinity)
		 * @param size The resulting size of the compressed texture, in bytes
		 * @return True, if succeeded
		 */
		static bool compressedImageSize(const CompressedFormat compressedFormat, const unsigned int width, const unsigned int height, unsigned int& size);

	protected:

		/// The OpenGL ES texture id.
		GLuint textureId_ = 0u;

		/// The texture's frame, if any.
		Frame frame_;

		/// The texture's compressed frame, if any.
		CompressedFrame compressedFrame_;

		/// True, if the texture needs to be updated.
		bool updateNeeded_ = false;

		/// The name of the texture in the shader.
		std::string textureName_ = std::string("primaryTexture");
};

inline GLuint GLESFrameTexture2D::textureId() const
{
	return textureId_;
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_FRAME_TEXTURE_2D_H
