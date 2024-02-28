// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/BufferImage.h"

namespace Ocean
{

namespace Media
{

BufferImage::BufferImage(const std::string& url) :
	Medium(url),
	FrameMedium(url),
	imageStarted(false),
	imageBuffer(nullptr),
	imageBufferSize(0)
{
	type_ = Type(type_ | BUFFER_IMAGE);
	isValid_ = true;
}

BufferImage::~BufferImage()
{
	free(imageBuffer);
}

bool BufferImage::isStarted() const
{
	return imageStarted;
}

Timestamp BufferImage::startTimestamp() const
{
	return mediumStartTimestamp;
}

Timestamp BufferImage::pauseTimestamp() const
{
	return mediumPauseTimestamp;
}

Timestamp BufferImage::stopTimestamp() const
{
	return mediumStopTimestamp;
}

bool BufferImage::setBufferImage(const void* buffer, const size_t size, const std::string& imageType)
{
	const ScopedLock scopedLock(lock_);

	if (!imageStarted)
	{
		if (size != imageBufferSize)
		{
			free(imageBuffer);
			imageBufferSize = 0;

			imageBuffer = malloc(size);
			ocean_assert(size == 0 || imageBuffer != nullptr);

			imageBufferSize = size;
		}

		memcpy(imageBuffer, buffer, size);

		imageBufferType = imageType;

		return true;
	}
	else
	{
		ocean_assert(false && "Pixel image is not started!");
	}

	return false;
}

}

}
