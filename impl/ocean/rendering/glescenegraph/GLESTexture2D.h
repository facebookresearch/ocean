/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_TEXTURE_2D_H
#define META_OCEAN_RENDERING_GLES_TEXTURE_2D_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESTexture.h"

#include "ocean/rendering/Texture2D.h"
#include "ocean/rendering/DynamicObject.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class wraps a GLESceneGraph texture object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESTexture2D :
	virtual public GLESTexture,
	virtual public Texture2D,
	virtual protected DynamicObject
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the texture wrap type in s direction.
		 * @see Texture2D::wrapTypeS().
		 */
		WrapType wrapTypeS() const override;

		/**
		 * Returns the texture wrap type in t direction.
		 * @see Texture2D::wrapTypeT().
		 */
		WrapType wrapTypeT() const override;

		/**
		 * Sets the texture wrap type in s direction.
		 * @see Texture2D::setWrapTypeS().
		 */
		bool setWrapTypeS(const WrapType type) override;

		/**
		 * Sets the texture wrap type in t direction.
		 * @see Texture2D::setWrapTypeT().
		 */
		bool setWrapTypeT(const WrapType type) override;

		/**
		 * Returns the name of the texture e.g., in a shader.
		 * @return The name of the texture
		 */
		std::string textureName() const override;

		/**
		 * Sets the name of the texture e.g., in a shader.
		 * @param name The name to set
		 * @return True, if succeeded
		 */
		bool setTextureName(const std::string& name) override;

		/**
		 * Returns the texture id of the primary texture.
		 * @return The id of the primary texture, 0 if invalid
		 */
		inline GLuint primaryTextureId() const;

		/**
		 * Returns the texture id of the secondary texture.
		 * @return The id of the secondary texture, 0 if invalid
		 */
		inline GLuint secondaryTextureId() const;

	protected:

		/**
		 * Creates a new GLESceneGraph texture 2D object.
		 */
		GLESTexture2D();

		/**
		 * Destructs a GLESceneGraph texture 2D object.
		 */
		~GLESTexture2D() override;

		/**
		 * Updates the mipmap for this texture.
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
		 * Determines the alignment for a plane.
		 * @param planeStrideBytes The stride of the plane in bytes, with range [1, infinity)
		 * @param rowLength The resulting row length, with range [1, infinity)
		 * @param byteAlignment The resulting byte alignment, either 1, 2, or 4
		 * @return True, if succeeded
		 */
		static bool determineAlignment(const unsigned int planeStrideBytes, unsigned int& rowLength, unsigned int& byteAlignment);

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

		/// The texture wrap s type.
		WrapType wrapTypeS_ = WRAP_CLAMP;

		/// The texture wrap t type.
		WrapType wrapTypeT_ = WRAP_CLAMP;

		/// The OpenGL ES primary texture id.
		GLuint primaryTextureId_ = 0u;

		/// The OpenGL ES secondary texture id.
		GLuint secondaryTextureId_ = 0u;

		/// The name of the texture in the shader.
		std::string textureName_ = std::string("primaryTexture,secondaryTexture");
};

inline GLuint GLESTexture2D::primaryTextureId() const
{
	return primaryTextureId_;
}

inline GLuint GLESTexture2D::secondaryTextureId() const
{
	return secondaryTextureId_;
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_TEXTURE_2D_H
