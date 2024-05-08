/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GITexture.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GITexture::GITexture() :
	GIObject(),
	Texture(),
	textureTransformation(true),
	textureEnvironmentMode(MODE_REPLACE),
	textureMinificationFilterMode(MIN_MODE_LINEAR),
	textureMagnificationFilterMode(MAG_MODE_LINEAR)
{
	// nothing to do here
}

GITexture::~GITexture()
{
	// nothing to do here
}

std::string GITexture::textureName() const
{
	const ScopedLock scopedLock(objectLock);

	return textureName_;
}

bool GITexture::setTextureName(const std::string& name)
{
	if (name.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(objectLock);

	textureName_ = name;

	return true;
}

HomogenousMatrix4 GITexture::transformation() const
{
	return textureTransformation;
}

GITexture::EnvironmentMode GITexture::environmentMode() const
{
	return textureEnvironmentMode;
}

GITexture::MinFilterMode GITexture::minificationFilterMode() const
{
	return textureMinificationFilterMode;
}

GITexture::MagFilterMode GITexture::magnificationFilterMode() const
{
	return textureMagnificationFilterMode;
}

void GITexture::setTransformation(const HomogenousMatrix4& transformation)
{
	textureTransformation = transformation;
}

void GITexture::setEnvironmentMode(const EnvironmentMode mode)
{
	textureEnvironmentMode = mode;
}

void GITexture::setMinificationFilterMode(const MinFilterMode mode)
{
	textureMinificationFilterMode = mode;
}

void GITexture::setMagnificationFilterMode(const MagFilterMode mode)
{
	textureMagnificationFilterMode = mode;
}

}

}

}
