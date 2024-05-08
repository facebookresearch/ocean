/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_TEXTURES_H
#define META_OCEAN_RENDERING_GI_TEXTURES_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIAttribute.h"

#include "ocean/math/RGBAColor.h"

#include "ocean/rendering/Textures.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a Global Illumination texture attributes object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GITextures :
	virtual public GIAttribute,
	virtual public Textures
{
	friend class GIFactory;

	public:

		/**
		 * Sets the texture and attribute for a specified texture layer.
		 * @see Textures::setTexture().
		 */
		void setTexture(const TextureRef& texture, const unsigned int layerIndex) override;

		/**
		 * Returns the texture color for a given texture coordinate.
		 * @param textureCoordinate Texture coordinate to return the color for
		 * @param color Resulting texture color
		 * @return True, if succeeded
		 */
		bool textureColor(const TextureCoordinate& textureCoordinate, RGBAColor& color) const;

	protected:

		/**
		 * Creates a new Global Illumination textures object.
		 */
		GITextures();

		/**
		 * Destructs a Global Illumination textures objets.
		 */
		~GITextures() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_TEXTURES_H
