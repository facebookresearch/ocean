/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_TEXTURE_2D_H
#define META_OCEAN_RENDERING_GLES_TEXTURE_2D_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESAttribute.h"
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
		 * Definition of individual layouts of the secondary texture.
		 */
		enum SecondaryLayout : uint32_t
		{
			/// The frame type does not need a secondary texture.
			SL_NONE = 0u,
			/// The second plane of the frame is copied into the secondary texture.
			SL_PLANE_1,
			/// The second and the third plane of the frame are stacked into the secondary texture, the second plane in the upper half.
			SL_PLANE_1_2,
			/// The second and the third plane of the frame are stacked into the secondary texture, the third plane in the upper half.
			SL_PLANE_2_1
		};

		/**
		 * This class holds the texture properties of one pixel format.
		 * The GL properties always describe the internal pixel format, so they can be used without resolving the conversion first.
		 */
		class TextureProperties
		{
			public:

				/**
				 * Creates properties for a pixel format which needs one texture only.
				 * @param programType The type of the shader program which can sample the pixel format, PT_UNKNOWN if no shader exists for it
				 * @param primaryFormat The GL format of the primary texture, must be valid
				 * @param primaryType The GL type of the primary texture, must be valid
				 */
				TextureProperties(const GLESAttribute::ProgramType programType, const GLenum primaryFormat, const GLenum primaryType);

				/**
				 * Creates properties for a pixel format which needs a secondary texture as well.
				 * @param programType The type of the shader program which can sample the pixel format, must not be PT_UNKNOWN
				 * @param primaryFormat The GL format of the primary texture, must be valid
				 * @param primaryType The GL type of the primary texture, must be valid
				 * @param secondaryFormat The GL format of the secondary texture, must be valid
				 * @param secondaryType The GL type of the secondary texture, must be valid
				 * @param secondaryWidthDivisor The divisor between the width of the frame and the width of the secondary texture, with range [1, infinity)
				 * @param secondaryHeightDivisor The divisor between the height of the frame and the height of the secondary texture, with range [1, infinity)
				 * @param secondaryLayout The layout of the secondary texture, must not be SL_NONE
				 */
				TextureProperties(const GLESAttribute::ProgramType programType, const GLenum primaryFormat, const GLenum primaryType, const GLenum secondaryFormat, const GLenum secondaryType, const unsigned int secondaryWidthDivisor, const unsigned int secondaryHeightDivisor, const SecondaryLayout secondaryLayout);

			public:

				/// The pixel format the frame needs to be converted to before it can be uploaded, FORMAT_UNDEFINED if the frame can be uploaded as it is.
				/// The remaining properties belong to this pixel format and not to the pixel format the properties are stored for.
				FrameType::PixelFormat internalPixelFormat_ = FrameType::FORMAT_UNDEFINED;

				/// The type of the shader program which can sample the pixel format, PT_UNKNOWN if no shader exists for it.
				GLESAttribute::ProgramType programType_ = GLESAttribute::PT_UNKNOWN;

				/// The GL format of the primary texture.
				GLenum primaryFormat_ = 0;

				/// The GL type of the primary texture.
				GLenum primaryType_ = 0;

				/// The GL format of the secondary texture, 0 if the pixel format does not need a secondary texture.
				GLenum secondaryFormat_ = 0;

				/// The GL type of the secondary texture, 0 if the pixel format does not need a secondary texture.
				GLenum secondaryType_ = 0;

				/// The divisor between the width of the frame and the width of the secondary texture, 0 if the pixel format does not need a secondary texture.
				unsigned int secondaryWidthDivisor_ = 0u;

				/// The divisor between the height of the frame and the height of the secondary texture, 0 if the pixel format does not need a secondary texture.
				unsigned int secondaryHeightDivisor_ = 0u;

				/// The layout of the secondary texture.
				SecondaryLayout secondaryLayout_ = SL_NONE;

			public:

				/**
				 * Returns whether a second texture is necessary.
				 * @return True, if so; False, if one texture is enough
				 */
				inline bool needsSecondaryTextureObjects() const;

				/**
				 * Returns the frame type of the internal frame for which a GL texture format exists.
				 * @param frameType The external (given) frame type for which the internal frame type will be determined, must be valid and must match these properties
				 * @return The resulting internal frame type
				 */
				FrameType internalFrameType(const FrameType& frameType) const;

				/**
				 * Determines the properties of the primary texture for a given frame type.
				 * @param frameType The frame type for which the properties will be determined, must be valid and must match these properties
				 * @param width The resulting width of the primary texture in pixel, with range [1, infinity)
				 * @param height The resulting height of the primary texture in pixel, with range [1, infinity)
				 * @param format The resulting GL format of the primary texture
				 * @param type The resulting GL type of the primary texture
				 */
				void primaryTextureProperties(const FrameType& frameType, unsigned int& width, unsigned int& height, GLenum& format, GLenum& type) const;

				/**
				 * Determines the properties of the secondary texture for a given frame type.
				 * @param frameType The frame type for which the properties will be determined, must be valid and must match these properties
				 * @param width The resulting width of the secondary texture in pixel, with range [1, infinity)
				 * @param height The resulting height of the secondary texture in pixel, with range [1, infinity)
				 * @param format The resulting GL format of the secondary texture
				 * @param type The resulting GL type of the secondary texture
				 * @return True, if succeeded; False, if the frame type does not need a secondary texture
				 */
				bool secondaryTextureProperties(const FrameType& frameType, unsigned int& width, unsigned int& height, GLenum& format, GLenum& type) const;
		};

	protected:

		/**
		 * Definition of a map mapping pixel formats to texture properties.
		 */
		using TexturePropertiesMap = std::unordered_map<FrameType::PixelFormat, TextureProperties>;

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

		/**
		 * Binds this texture.
		 * @see GLESTexture::bindTexture().
		 */
		unsigned int bindTexture(GLESShaderProgram& shaderProgram, const unsigned int id) override;

		/**
		 * Returns the texture properties of a given pixel format.
		 * @param pixelFormat The pixel format for which the properties will be returned, must be valid
		 * @return The properties of the pixel format, nullptr if the pixel format is not supported
		 */
		static const TextureProperties* textureProperties(const FrameType::PixelFormat pixelFormat);

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
		 * @param properties The texture properties of the frame type, must match the frame type
		 * @param frameType Frame type to create the new texture object for
		 * @return True, if succeeded
		 */
		bool defineTextureObject(const TextureProperties& properties, const FrameType& frameType);

		/**
		 * Creates a primary GLESceneGraph texture object.
		 * @param properties The texture properties of the frame type, must match the frame type
		 * @param frameType Frame type to create the new texture object for
		 * @return True, if succeeded
		 */
		bool definePrimaryTextureObject(const TextureProperties& properties, const FrameType& frameType);

		/**
		 * Creates a secondary GLESceneGraph texture object.
		 * @param properties The texture properties of the frame type, must match the frame type, must need a secondary texture
		 * @param frameType Frame type to create the new texture object for
		 * @return True, if succeeded
		 */
		bool defineSecondaryTextureObject(const TextureProperties& properties, const FrameType& frameType);

		/**
		 * Determines the unpack parameters for a plane of a frame.
		 * @param frame The frame providing the plane, must be valid
		 * @param planeIndex The index of the plane, with range [0, frame.numberPlanes())
		 * @param rowLength The resulting row length in plane pixels, as GL_UNPACK_ROW_LENGTH expects, with range [1, infinity)
		 * @param byteAlignment The resulting byte alignment, either 1, 2, or 4
		 * @return True, if succeeded
		 */
		static bool determineAlignment(const Frame& frame, const unsigned int planeIndex, unsigned int& rowLength, unsigned int& byteAlignment);

		/**
		 * Creates the map mapping all supported pixel formats to their texture properties.
		 * @return The map with the properties of all supported pixel formats
		 */
		static TexturePropertiesMap createTexturePropertiesMap();

		/**
		 * Adds the texture properties of one pixel format to a map.
		 * @param map The map to which the properties will be added
		 * @param pixelFormat The pixel format for which the properties will be added, must not be in the map already
		 * @param properties The properties of the pixel format
		 */
		static void addTextureProperties(TexturePropertiesMap& map, const FrameType::PixelFormat pixelFormat, const TextureProperties& properties);

		/**
		 * Adds the texture properties of one pixel format which needs to be converted before it can be uploaded.
		 * The properties are the properties of the pixel format the frame is converted to.
		 * @param map The map to which the properties will be added
		 * @param pixelFormat The pixel format for which the properties will be added, must not be in the map already
		 * @param internalPixelFormat The pixel format the frame needs to be converted to, must be in the map already and must not need a conversion itself
		 */
		static void addConvertedTextureProperties(TexturePropertiesMap& map, const FrameType::PixelFormat pixelFormat, const FrameType::PixelFormat internalPixelFormat);

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

		/**
		 * Updates the texture based on a given frame.
		 * @param frame The frame to be used to update the texture, must be valid
		 * @return True, if succeeded
		 */
		bool updateTexture(const Frame& frame);

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

		/// Optional temp conversion frame.
		Frame conversionFrame_;
};

inline bool GLESTexture2D::TextureProperties::needsSecondaryTextureObjects() const
{
	return secondaryLayout_ != SL_NONE;
}

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
