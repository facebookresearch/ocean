/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_TEXTURE_H
#define META_OCEAN_RENDERING_TEXTURE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Object.h"
#include "ocean/rendering/ObjectRef.h"

#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Texture;

/**
 * Definition of a smart object reference holding a texture.
 * @see SmartObjectRef, Texture.
 * @ingroup rendering
 */
typedef SmartObjectRef<Texture> TextureRef;

/**
 * This class is the base class for all textures.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Texture : virtual public Object
{
	public:

		/**
		 * Definition of possible texture environment modes.
		 */
		enum EnvironmentMode
		{
			/// Invalid texture environment mode.
			MODE_INVALID,
			MODE_ADD,
			MODE_ADD_SIGNED,
			MODE_BLEND,
			MODE_MODULATE,
			MODE_REPLACE,
			MODE_SUBTRACT
		};

		/**
		 * Definition of possible minification filter modes.<br>
		 * Minification: One screen pixel corresponds to a large area of texels.
		 */
		enum MinFilterMode
		{
			/// Invalid minification filter mode.
			MIN_MODE_INVALID,
			/// Linear interpolation between neighboring texel only in one texture layer.
			MIN_MODE_LINEAR,
			/// Chooses the nearest pixel only in one texture layer.
			MIN_MODE_NEAREST,
			/// Chooses the nearest pixel in the nearest mipmap texture layer.
			MIN_MODE_NEAREST_MIPMAP_NEAREST,
			/// Linear interpolation between neighboring texel in the nearest mipmap texture layer.
			MIN_MODE_LINEAR_MIPMAP_NEAREST,
			/// Chooses the nearest pixel and interpolates between neighboring mipmap texture layers.
			MIN_MODE_NEAREST_MIPMAP_LINEAR,
			/// Linear interpolation between neighboring texel between neighboring mipmap texture layers.
			MIN_MODE_LINEAR_MIPMAP_LINEAR
		};

		/**
		 * Definition of possible magnification filter modes.<br>
		 * Magnification: A large screen area corresponds to a single texel.
		 */
		enum MagFilterMode
		{
			/// Invalid magnification filter mode.
			MAG_MODE_INVALID,
			/// Linear interpolation between neighboring texel.
			MAG_MODE_LINEAR,
			/// Chooses the nearest pixel.
			MAG_MODE_NEAREST
		};

		/**
		 * Definition of different texture wrapping types.
		 */
		enum WrapType
		{
			/// Invalid wrap mode.
			WRAP_INVALID,
			/// The texture will be clamped at the border.
			WRAP_CLAMP,
			/// The texture will be repeated at the border.
			WRAP_REPEAT
		};

	public:

		/**
		 * Returns the name of the texture e.g., in a shader.
		 * @return The texture's name
		 */
		virtual std::string textureName() const = 0;

		/**
		 * Returns the texture transformation matrix.
		 * @return Matrix transforming the corresponding texture
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual HomogenousMatrix4 transformation() const;

		/**
		 * Returns the environment mode for this texture.
		 * @return Environment mode
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual EnvironmentMode environmentMode() const;

		/**
		 * Returns the minification filter mode for this texture.
		 * @return Minification mode
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual MinFilterMode minificationFilterMode() const;

		/**
		 * Returns the magnification filter mode for this texture.
		 * @return Magnification mode
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual MagFilterMode magnificationFilterMode() const;

		/**
		 * Returns whether mipmaps are used.
		 * @return True, if so
		 */
		virtual bool usingMipmaps() const;

		/**
		 * Returns whether the internal texture size has been extended to power-of-two dimensions to hand the reduced graphic power of the system.
		 * Systems not providing the necessary graphic extensions to support non-power-of-two textures will use power-of-two textures automatically.
		 * Therefore, the texture transformation cannot be used on systems with reduced graphic power.
		 * @return True, if so
		 * @see imageTextureRatio().
		 */
		virtual bool adjustedToPowerOfTwo() const;

		/**
		 * Sets the name of the texture e.g., in a shader.
		 * @param name The texture's name
		 * @return True, if succeeded
		 */
		virtual bool setTextureName(const std::string& name) = 0;

		/**
		 * Sets the texture transformation matrix.
		 * @param transformation Matrix transforming the corresponding texture
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setTransformation(const HomogenousMatrix4& transformation);

		/**
		 * Sets the environment mode for this texture.
		 * @param mode New environment mode to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setEnvironmentMode(const EnvironmentMode mode);

		/**
		 * Sets the minification filter mode for this texture.
		 * @param mode New minification filter mode to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setMinificationFilterMode(const MinFilterMode mode);

		/**
		 * Sets the magnification filter mode for this texture.
		 * @param mode New magnification filter mode to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setMagnificationFilterMode(const MagFilterMode mode);

		/**
		 * Specifies whether mipmaps will be used.
		 * @param flag True, to use mipmaps; False, to avoid mipmaps
		 */
		virtual void setUseMipmaps(const bool flag);

		/**
		 * Returns whether the texture internally holds valid data.
		 * @return True, if so
		 */
		virtual bool isValid() const;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new texture object.
		 */
		 Texture();

		/**
		 * Destructs a texture object.
		 */
		~Texture() override;
};

}

}

#endif // META_OCEAN_RENDERING_TEXTURE_H
