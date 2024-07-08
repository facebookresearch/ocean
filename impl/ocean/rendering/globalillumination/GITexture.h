/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_TEXTURE_H
#define META_OCEAN_RENDERING_GI_TEXTURE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIObject.h"

#include "ocean/rendering/Texture.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class is the base class for all Global Illumination texture objects.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GITexture :
	virtual public GIObject,
	virtual public Texture
{
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
		 * Returns the texture transformation matrix.
		 * @see Texture::transformation().
		 */
		HomogenousMatrix4 transformation() const override;

		/**
		 * Returns the environment mode for this texture.
		 * @see Texture::environmentMode().
		 */
		EnvironmentMode environmentMode() const override;

		/**
		 * Returns the minification filter mode for this texture.
		 * @see Texture::minificationFilterMode().
		 */
		MinFilterMode minificationFilterMode() const override;

		/**
		 * Returns the magnification filter mode for this texture.
		 * @see Texture::magnificationFilterMode().
		 */
		MagFilterMode magnificationFilterMode() const override;

		/**
		 * Sets the texture transformation matrix.
		 * @see Texture::setTransformation().
		 */
		void setTransformation(const HomogenousMatrix4& transformation) override;

		/**
		 * Sets the environment mode for this texture.
		 * @see Texture::setEnvironmentMode().
		 */
		void setEnvironmentMode(const EnvironmentMode mode) override;

		/**
		 * Sets the minification filter mode for this texture.
		 * @see Texture::setMinificationFilterMode().
		 */
		void setMinificationFilterMode(const MinFilterMode mode) override;

		/**
		 * Sets the magnification filter mode for this texture.
		 * @see Texture::setMagnificationFilterMode().
		 */
		void setMagnificationFilterMode(const MagFilterMode mode) override;

	protected:

		/**
		 * Creates a new Global Illumination texture object.
		 */
		GITexture();

		/**
		 * Destructs a Global Illumination texture object.
		 */
		~GITexture() override;

	protected:

		/// Homogeneous texture transformation matrix.
		HomogenousMatrix4 textureTransformation;

		/// Texture environment mode.
		EnvironmentMode textureEnvironmentMode;

		/// Texture minification filter mode.
		MinFilterMode textureMinificationFilterMode;

		/// Texture magnification filter mode.
		MagFilterMode textureMagnificationFilterMode;

		/// Flag determining whether the texture should be a mipmap texture.
		bool textureShouldUseMipmaps;

		/// The name of the texture, actually not used.
		std::string textureName_ = std::string("texture");
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_TEXTURE_H
