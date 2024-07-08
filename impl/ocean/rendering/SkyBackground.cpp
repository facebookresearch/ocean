/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/SkyBackground.h"

namespace Ocean
{

namespace Rendering
{

SkyBackground::SkyBackground() :
	Background()
{
	// nothing to do here
}

SkyBackground::~SkyBackground()
{
	// nothing to do here
}

SkyBackground::ObjectType SkyBackground::type() const
{
	return TYPE_SKY_BACKGROUND;
}

SkyBackground::ColorPairs SkyBackground::skyColors() const
{
	throw NotSupportedException("SkyBackground::skyColors() is not supported.");
}

SkyBackground::ColorPairs SkyBackground::groundColors() const
{
	throw NotSupportedException("SkyBackground::groundColor() is not supported.");
}

TexturesRef SkyBackground::frontTextures() const
{
	throw NotSupportedException("SkyBackground::frontTextures() is not supported.");
}

TexturesRef SkyBackground::backTextures() const
{
	throw NotSupportedException("SkyBackground::backTextures() is not supported.");
}

TexturesRef SkyBackground::leftTextures() const
{
	throw NotSupportedException("SkyBackground::leftTextures() is not supported.");
}

TexturesRef SkyBackground::rightTextures() const
{
	throw NotSupportedException("SkyBackground::rightTextures() is not supported.");
}

TexturesRef SkyBackground::topTextures() const
{
	throw NotSupportedException("SkyBackground::topTextures() is not supported.");
}

TexturesRef SkyBackground::bottomTextures() const
{
	throw NotSupportedException("SkyBackground::bottomTextures() is not supported.");
}

bool SkyBackground::setSkyColors(const ColorPairs& /*colors*/)
{
	throw NotSupportedException("SkyBackground::setSkyColors() is not supported.");
}

bool SkyBackground::setGroundColors(const ColorPairs& /*colors*/)
{
	throw NotSupportedException("SkyBackground::setGroundColors() is not supported.");
}

bool SkyBackground::setFrontTextures(const TexturesRef& /*textures*/)
{
	throw NotSupportedException("SkyBackground::setFrontTextures() is not supported.");
}

bool SkyBackground::setBackTextures(const TexturesRef& /*textures*/)
{
	throw NotSupportedException("SkyBackground::setBackTextures() is not supported.");
}

bool SkyBackground::setLeftTextures(const TexturesRef& /*textures*/)
{
	throw NotSupportedException("SkyBackground::setLeftTextures() is not supported.");
}

bool SkyBackground::setRightTextures(const TexturesRef& /*textures*/)
{
	throw NotSupportedException("SkyBackground::setRightTextures() is not supported.");
}

bool SkyBackground::setTopTextures(const TexturesRef& /*textures*/)
{
	throw NotSupportedException("SkyBackground::setTopTextures() is not supported.");
}

bool SkyBackground::setBottomTextures(const TexturesRef& /*textures*/)
{
	throw NotSupportedException("SkyBackground::setBottomTextures() is not supported.");
}

}

}
