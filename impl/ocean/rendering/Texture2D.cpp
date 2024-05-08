/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Texture2D.h"

namespace Ocean
{

namespace Rendering
{

Texture2D::Texture2D() :
	Texture()
{
	// nothing to do here
}

Texture2D::~Texture2D()
{
	// nothing to do here
}

Vector2 Texture2D::imageTextureRatio() const
{
	return Vector2(1, 1);
}

Texture2D::WrapType Texture2D::wrapTypeS() const
{
	throw NotSupportedException("Texture2D::wrapTypeS() is not supported.");
}

Texture2D::WrapType Texture2D::wrapTypeT() const
{
	throw NotSupportedException("Texture2D::wrapTypeT() is not supported.");
}

bool Texture2D::setWrapTypeS(const WrapType /*type*/)
{
	throw NotSupportedException("Texture2D::setWrapTypeS() is not supported.");
}

bool Texture2D::setWrapTypeT(const WrapType /*type*/)
{
	throw NotSupportedException("Texture2D::setWrapTypeT() is not supported.");
}

FrameType Texture2D::frameType() const
{
	return FrameType();
}

bool Texture2D::hasTransparentPixel() const
{
	return false;
}

Texture2D::ObjectType Texture2D::type() const
{
	return TYPE_TEXTURE_2D;
}

}

}
