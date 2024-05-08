/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/BufferImage.h"

namespace Ocean
{

namespace Media
{

BufferImage::BufferImage(const std::string& url) :
	Medium(url),
	FrameMedium(url)
{
	type_ = Type(type_ | BUFFER_IMAGE);
	isValid_ = true;
}

bool BufferImage::isStarted() const
{
	return started_;
}

Timestamp BufferImage::startTimestamp() const
{
	return startTimestamp_;
}

Timestamp BufferImage::pauseTimestamp() const
{
	return pauseTimestamp_;
}

Timestamp BufferImage::stopTimestamp() const
{
	return stopTimestamp_;
}

bool BufferImage::setBufferImage(const void* buffer, const size_t size, const std::string& imageType)
{
	ocean_assert(buffer != nullptr && size != 0);
	if (buffer == nullptr || size == 0)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (!started_)
	{
		if (memory_.size() != size)
		{
			memory_ = Memory(size);

			if (memory_.isNull())
			{
				return false;
			}
		}

		memcpy(memory_.data(), buffer, size);

		bufferType_ = imageType;

		return true;
	}
	else
	{
		ocean_assert(false && "Buffer image is not started!");
	}

	return false;
}

}

}
