/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
