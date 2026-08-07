/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Textures.h"
#include "ocean/rendering/Texture2D.h"

namespace Ocean
{

namespace Rendering
{

Textures::Textures() :
	Attribute()
{
	// nothing to do here
}

Textures::~Textures()
{
	for (const TextureRef& texture : textures_)
	{
		unregisterThisObjectAsParent(texture);
	}
}

TextureRef Textures::texture(const unsigned int layerIndex) const
{
	const ScopedLock scopedLock(objectLock);

	if (layerIndex < textures_.size())
	{
		return textures_[layerIndex];
	}

	return TextureRef();
}

void Textures::setTexture(const TextureRef& texture, const unsigned int layerIndex)
{
	const ScopedLock scopedLock(objectLock);

	if (layerIndex >= textures_.size())
	{
		textures_.resize(layerIndex + 1);
	}

	unregisterThisObjectAsParent(textures_[layerIndex]);
	registerThisObjectAsParent(texture);

	textures_[layerIndex] = texture;
}

void Textures::addTexture(const TextureRef& texture)
{
	if (texture.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	registerThisObjectAsParent(texture);
	textures_.push_back(texture);
}

bool Textures::hasAlphaTexture() const
{
	const ScopedLock scopedLock(objectLock);

	for (const TextureRef& textureObject : textures_)
	{
		const Texture2DRef texture(textureObject);

		if (texture)
		{
			return texture->hasTransparentPixel();
		}
	}

	return false;
}

Textures::ObjectType Textures::type() const
{
	return TYPE_TEXTURES;
}

}

}
