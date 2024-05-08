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
	for (TextureObjects::const_iterator i = textures.begin(); i != textures.end(); ++i)
		unregisterThisObjectAsParent(*i);
}

TextureRef Textures::texture(const unsigned int layerIndex) const
{
	const ScopedLock scopedLock(objectLock);

	if (layerIndex < textures.size())
		return textures[layerIndex];

	return TextureRef();
}

void Textures::setTexture(const TextureRef& texture, const unsigned int layerIndex)
{
	const ScopedLock scopedLock(objectLock);

	if (layerIndex >= textures.size())
		textures.resize(layerIndex + 1);

	unregisterThisObjectAsParent(textures[layerIndex]);
	registerThisObjectAsParent(texture);

	textures[layerIndex] = texture;
}

void Textures::addTexture(const TextureRef& texture)
{
	if (texture.isNull())
		return;

	const ScopedLock scopedLock(objectLock);

	registerThisObjectAsParent(texture);
	textures.push_back(texture);
}

bool Textures::hasAlphaTexture() const
{
	const ScopedLock scopedLock(objectLock);

	for (TextureObjects::const_iterator i = textures.begin(); i != textures.end(); ++i)
	{
		const Texture2DRef texture(*i);

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
