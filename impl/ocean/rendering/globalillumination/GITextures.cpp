/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GITextures.h"
#include "ocean/rendering/globalillumination/GITexture.h"
#include "ocean/rendering/globalillumination/GITexture2D.h"


namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GITextures::GITextures() :
	GIAttribute(),
	Textures()
{
	// nothing to do here
}

GITextures::~GITextures()
{
	// nothing to do here
}

void GITextures::setTexture(const TextureRef& texture, const unsigned int layerIndex)
{
	const ScopedLock scopedLock(objectLock);
	Textures::setTexture(texture, layerIndex);
}

bool GITextures::textureColor(const TextureCoordinate& textureCoordinate, RGBAColor& color) const
{
	if (textures.empty())
	{
		return false;
	}

	const SmartObjectRef<GITexture2D> texture(textures.front());

	if (!texture)
	{
		return false;
	}

	return texture->textureColor(textureCoordinate, color);
}

}

}

}
