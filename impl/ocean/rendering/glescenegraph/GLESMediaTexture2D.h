/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_MEDIA_TEXTURE_2D_H
#define META_OCEAN_RENDERING_GLES_MEDIA_TEXTURE_2D_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESTexture2D.h"

#include "ocean/rendering/MediaTexture2D.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class wraps a GLESceneGraph media texture object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESMediaTexture2D final :
	virtual public GLESTexture2D,
	virtual public MediaTexture2D
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
		 * Returns the texture id of the primary texture.
		 * @return The id of the primary texture, 0 if invalid
		 */
		inline GLuint primaryTextureId() const;

		/**
		 * Returns the texture id of the secondary texture.
		 * @return The id of the primary texture, 0 if invalid
		 */
		inline GLuint secondaryTextureId() const;

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

		/**
		 * Returns descriptive information about the object as string.
		 * @see Object::descriptiveInformation().
		 */
		std::string descriptiveInformation() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph texture 2D object.
		 */
		GLESMediaTexture2D();

		/**
		 * Destructs a GLESceneGraph texture 2D object.
		 */
		~GLESMediaTexture2D() override;

		/**
		 * Updates the mipmap for this texture.
		 * @see GLESTexture::createMipmap().
		 */
		void createMipmap() override;

		/**
		 * Creates a new GLESceneGraph texture object.
		 * @param frameType Frame type to create the new texture object for
		 * @return True, if succeeded
		 */
		bool defineTextureObject(const FrameType& frameType);

		/**
		 * Creates a primary GLESceneGraph texture object.
		 * @param frameType Frame type to create the new texture object for
		 * @return True, if succeeded
		 */
		bool definePrimaryTextureObject(const FrameType& frameType);

		/**
		 * Creates a secondary GLESceneGraph texture object.
		 * @param frameType Frame type to create the new texture object for
		 * @return True, if succeeded
		 */
		bool defineSecondaryTextureObject(const FrameType& frameType);

		/**
		 * Update function called by the framebuffer.
		 * @see DynamicObject::onDynamicUpdate().
		 */
		void onDynamicUpdate(const ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Returns whether a second texture is necessary for a given frame type.
		 * @param frameType The frame type for which the check will be done, must be valid
		 * @return True, if so; False, if the frame type can be handled with just one texture
		 */
		static bool needsSecondaryTextureObjects(const FrameType& frameType);

		/**
		 * Returns the frame type of the internal frame for which a GL texture format exists.
		 * @param frameType The external (given) frame type for which the internal frame type will be determined, must be valid
		 * @param internalFrameType The resulting internal frame type
		 * @return True, if succeeded
		 */
		static bool determineInternalFrameType(const FrameType& frameType, FrameType& internalFrameType);

		/**
		 * Determines the properties of the primary texture for a given frame type.
		 * @param frameType The frame type for which the properties will be determined, must be valid
		 * @param width The resulting width of the primary texture in pixel, with range [1, infinity)
		 * @param height The resulting height of the primary texture in pixel, with range [1, infinity)
		 * @param format The resulting GL format of the primary texture
		 * @param type The resulting GL type of the primary texture
		 * @return True, if succeeded
		 */
		static bool determinePrimaryTextureProperties(const FrameType& frameType, unsigned int& width, unsigned int& height, GLenum& format, GLenum& type);

		/**
		 * Determines the properties of the secondary texture for a given frame type.
		 * @param frameType The frame type for which the properties will be determined, must be valid
		 * @param width The resulting width of the secondary texture in pixel, with range [1, infinity)
		 * @param height The resulting height of the secondary texture in pixel, with range [1, infinity)
		 * @param format The resulting GL format of the secondary texture
		 * @param type The resulting GL type of the secondary texture
		 * @return True, if succeeded
		 */
		static bool determineSecondaryTextureProperties(const FrameType& frameType, unsigned int& width, unsigned int& height, GLenum& format, GLenum& type);

		/**
		 * Returns the name of the primary texture.
		 * The input must have the format '\<PRIMARY>,\<SECONDARY>'.
		 * @param names The names of all textures, must be valid
		 * @param name The resulting name of the primary texture
		 * @return True, if succeeded
		 */
		static bool primaryTextureName(const std::string& names, std::string& name);

		/**
		 * Returns the name of the secondary texture.
		 * The input must have the format '\<PRIMARY>,\<SECONDARY>'.
		 * @param names The names of all textures, must be valid
		 * @param name The resulting name of the secondary texture
		 * @return True, if succeeded
		 */
		static bool secondaryTextureName(const std::string& names, std::string& name);

	protected:

		/// The recent timestamp of the rendering engine.
		Timestamp renderTimestamp_;

		/// The OpenGL ES texture id.
		GLuint primaryTextureId_ = 0u;

		/// Additional OpenGL ES texture id.
		GLuint secondaryTextureId_ = 0u;

		/// Optional temp conversion frame.
		Frame conversionFrame_;

		/// The name of the texture in the shader.
		std::string textureName_ = std::string("primaryTexture,secondaryTexture");
};

inline GLuint GLESMediaTexture2D::primaryTextureId() const
{
	return primaryTextureId_;
}

inline GLuint GLESMediaTexture2D::secondaryTextureId() const
{
	return secondaryTextureId_;
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_MEDIA_TEXTURE_2D_H
