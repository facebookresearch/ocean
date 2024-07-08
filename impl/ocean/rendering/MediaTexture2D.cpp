/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/MediaTexture2D.h"

namespace Ocean
{

namespace Rendering
{

MediaTexture2D::MediaTexture2D() :
	Texture2D()
{
	// nothing to do here
}

MediaTexture2D::~MediaTexture2D()
{
	// nothing to do here
}

Media::FrameMediumRef MediaTexture2D::medium() const
{
	const ScopedLock scopedLock(objectLock);

	return frameMedium_;
}

void MediaTexture2D::setMedium(const Media::FrameMediumRef& medium)
{
	const ScopedLock scopedLock(objectLock);

	frameMedium_ = medium;
	frameTimestamp_.toInvalid();
}

FrameType MediaTexture2D::frameType() const
{
	return frameType_;
}

bool MediaTexture2D::hasTransparentPixel() const
{
	const ScopedLock scopedLock(objectLock);

	if (frameMedium_)
	{
		const FrameRef frame = frameMedium_->frame(frameTimestamp_);

		if (frame && frame->isValid())
		{
			if (frameMedium_->type() == Media::Medium::IMAGE)
			{
				if (frame->hasTransparentPixel<uint8_t>(0xFF))
				{
					return true;
				}
			}
			else if (FrameType::formatHasAlphaChannel(frame->pixelFormat()))
			{
				return true;
			}
		}
	}

	return false;
}

MediaTexture2D::ObjectType MediaTexture2D::type() const
{
	return TYPE_MEDIA_TEXTURE_2D;
}

}

}
