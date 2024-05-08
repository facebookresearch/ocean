/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
