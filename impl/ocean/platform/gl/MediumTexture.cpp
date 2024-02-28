// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/gl/MediumTexture.h"

namespace Ocean
{

namespace Platform
{

namespace GL
{

bool MediumTexture::update()
{
	if (textureFrameMedium.isNull())
	{
		return false;
	}

	const FrameRef frame = textureFrameMedium->frame();

	if (frame && *frame && frame->timestamp() > textureFrameTimestamp)
	{
		if (!Texture::update(*frame))
		{
			return false;
		}

		textureFrameTimestamp = frame->timestamp();
	}

	return true;
}

}

}

}
