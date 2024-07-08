/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_TEXTURE_2D_H
#define META_OCEAN_RENDERING_GI_TEXTURE_2D_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GITexture.h"

#include "ocean/rendering/Texture2D.h"
#include "ocean/rendering/DynamicObject.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class wraps a Global Illumination texture object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GITexture2D :
	virtual public GITexture,
	virtual public Texture2D,
	virtual protected DynamicObject
{
	friend class GIFactory;

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
		 * Returns the texture color for a given texture coordinate.
		 * @param textureCoordinate Texture coordinate to return the color for
		 * @param color Resulting texture color
		 * @return True, if succeeded
		 */
		virtual bool textureColor(const TextureCoordinate& textureCoordinate, RGBAColor& color) const = 0;

	protected:

		/**
		 * Creates a new Global Illumination texture 2D object.
		 */
		GITexture2D();

		/**
		 * Destructs a Global Illumination texture 2D object.
		 */
		~GITexture2D() override;

	protected:

		/// Wrap type in S direction.
		WrapType wrapTypeS_;

		/// Warp type in T direction.
		WrapType wrapTypeT_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_TEXTURE_2D_H
