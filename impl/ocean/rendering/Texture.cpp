/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Texture.h"

namespace Ocean
{

namespace Rendering
{

Texture::Texture() :
	Object()
{
	// nothing to do here
}

Texture::~Texture()
{
	// nothing to do here
}

HomogenousMatrix4 Texture::transformation() const
{
	throw NotSupportedException("Texture::transformation() is not supported.");
}

Texture::EnvironmentMode Texture::environmentMode() const
{
	throw NotSupportedException("Texture::environmentMode() is not supported.");
}

Texture::MinFilterMode Texture::minificationFilterMode() const
{
	throw NotSupportedException("Texture::minificationFilterMode() is not supported.");
}

Texture::MagFilterMode Texture::magnificationFilterMode() const
{
	throw NotSupportedException("Texture::magnificationFilterMode() is not supported.");
}

bool Texture::usingMipmaps() const
{
	throw NotSupportedException("Texture::usingMipmaps() is not supported.");
}

bool Texture::adjustedToPowerOfTwo() const
{
	return false;
}

void Texture::setTransformation(const HomogenousMatrix4& /*matrix*/)
{
	throw NotSupportedException("Texture::setTransformation() is not supported.");
}

void Texture::setEnvironmentMode(const EnvironmentMode /*mode*/)
{
	throw NotSupportedException("Texture::setEnvironmentMode() is not supported.");
}

void Texture::setMinificationFilterMode(const MinFilterMode /*mode*/)
{
	throw NotSupportedException("Texture::setMinificationFilterMode() is not supported.");
}

void Texture::setMagnificationFilterMode(const MagFilterMode /*mode*/)
{
	throw NotSupportedException("Texture::setMagnificationFilterMode() is not supported.");
}

void Texture::setUseMipmaps(const bool /*flag*/)
{
	throw NotSupportedException("Texture::setUseMipmaps() is not supported.");
}

bool Texture::isValid() const
{
	return false;
}

Texture::ObjectType Texture::type() const
{
	return TYPE_TEXTURE;
}

}

}
