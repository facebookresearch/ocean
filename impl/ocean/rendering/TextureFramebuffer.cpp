/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/TextureFramebuffer.h"

namespace Ocean
{

namespace Rendering
{

TextureFramebuffer::~TextureFramebuffer()
{
	// nothing to do here
}

TextureFramebuffer::WrapType TextureFramebuffer::wrapTypeS() const
{
	throw NotSupportedException("TextureFramebuffer::wrapTypeS() is not supported.");
}

TextureFramebuffer::WrapType TextureFramebuffer::wrapTypeT() const
{
	throw NotSupportedException("TextureFramebuffer::wrapTypeT() is not supported.");
}

bool TextureFramebuffer::setWrapTypeS(const WrapType /*type*/)
{
	throw NotSupportedException("TextureFramebuffer::setWrapTypeS() is not supported.");
}

bool TextureFramebuffer::setWrapTypeT(const WrapType /*type*/)
{
	throw NotSupportedException("TextureFramebuffer::setWrapTypeT() is not supported.");
}

bool TextureFramebuffer::copyColorTextureToFrame(Frame& /*frame*/, const CV::PixelBoundingBox& /*subRegion*/)
{
	return false;
}

bool TextureFramebuffer::copyDepthTextureToFrame(Frame& /*frame*/, const CV::PixelBoundingBox& /*subRegion*/)
{
	return false;
}

}

}
