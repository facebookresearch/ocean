// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/FrameTexture2D.h"

namespace Ocean
{

namespace Rendering
{

FrameTexture2D::FrameTexture2D() :
	Texture2D()
{
	// nothing to do here
}

FrameTexture2D::~FrameTexture2D()
{
	// nothing to do here
}

FrameTexture2D::ObjectType FrameTexture2D::type() const
{
	return TYPE_FRAME_TEXTURE_2D;
}

}

}
