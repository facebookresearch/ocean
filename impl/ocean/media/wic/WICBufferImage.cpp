/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/wic/WICBufferImage.h"
#include "ocean/media/wic/Image.h"
#include "ocean/media/wic/WICLibrary.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

WICBufferImage::WICBufferImage(const std::string& url) :
	Medium(url),
	FrameMedium(url),
	BufferImage(url)
{
	libraryName_ = nameWICLibrary();
	isValid_ = true;
}

WICBufferImage::~WICBufferImage()
{
	stop();
}

bool WICBufferImage::start()
{
	const ScopedLock scopedLock(lock_);

	isValid_ = loadImage();
	started_ = isValid_;

	if (started_)
	{
		startTimestamp_.toNow();
		pauseTimestamp_.toInvalid();
		stopTimestamp_.toInvalid();
	}

	return isValid_;
}

bool WICBufferImage::pause()
{
	return false;
}

bool WICBufferImage::stop()
{
	const ScopedLock scopedLock(lock_);

	release();

	started_ = false;

	startTimestamp_.toInvalid();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toNow();

	return true;
}

MediumRef WICBufferImage::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		const BufferImageRef bufferImage(WICLibrary::newImage(url_, true));
		ocean_assert(bufferImage);

		bufferImage->setBufferImage(memory_.constdata(), memory_.size());
		return bufferImage;
	}

	return MediumRef();
}

bool WICBufferImage::setPreferredFramePixelFormat(const FrameType::PixelFormat format)
{
	if (format == preferredFrameType_.pixelFormat())
	{
		return true;
	}

	if (frameCollection_.isNull() || format == FrameType::FORMAT_UNDEFINED)
	{
		preferredFrameType_ = MediaFrameType(preferredFrameType_, format);
		return true;
	}

	return false;
}

bool WICBufferImage::loadImage()
{
	if (memory_.isNull())
	{
		return false;
	}

	Frame frame = Image::decodeImage(memory_.constdata(), memory_.size(), bufferType_);

	if (!frame.isValid())
	{
		Log::error() << "Could not load the image \"" << url() << "\"";
		return false;
	}

	if (preferredFrameType_.pixelFormat() != FrameType::FORMAT_UNDEFINED && (frame.pixelFormat() != preferredFrameType_.pixelFormat() || frame.pixelOrigin() != preferredFrameType_.pixelOrigin()))
	{
		if (!CV::FrameConverter::Comfort::change(frame, preferredFrameType_.pixelFormat(), preferredFrameType_.pixelOrigin(), true, WorkerPool::get().scopedWorker()()))
		{
			return false;
		}
	}

	ocean_assert(frame);

	deliverNewFrame(std::move(frame));

	return true;
}

void WICBufferImage::release()
{
	frameCollection_.clear();
}

}

}

}
