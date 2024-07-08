/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/imageio/IIOBufferImage.h"
#include "ocean/media/imageio/IIOLibrary.h"
#include "ocean/media/imageio/Image.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

IIOBufferImage::IIOBufferImage(const std::string& url) :
	Medium(url),
	FrameMedium(url),
	BufferImage(url)
{
	libraryName_ = nameImageIOLibrary();
	isValid_ = true;
}

IIOBufferImage::~IIOBufferImage()
{
	stop();
}

bool IIOBufferImage::start()
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

bool IIOBufferImage::pause()
{
	return false;
}

bool IIOBufferImage::stop()
{
	const ScopedLock scopedLock(lock_);

	release();

	started_ = false;

	startTimestamp_.toInvalid();
	pauseTimestamp_.toInvalid();
	stopTimestamp_.toNow();

	return true;
}

MediumRef IIOBufferImage::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		const BufferImageRef bufferImage(IIOLibrary::newImage(url_, true));
		ocean_assert(bufferImage);

		bufferImage->setBufferImage(memory_.constdata(), memory_.size());
		return bufferImage;
	}

	return MediumRef();
}

bool IIOBufferImage::setPreferredFramePixelFormat(const FrameType::PixelFormat format)
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

bool IIOBufferImage::loadImage()
{
	if (memory_.isNull())
	{
		return false;
	}

	Frame frame = ImageIO::Image::decodeImage(memory_.constdata(), memory_.size());

	if (!frame.isValid())
	{
		Log::error() << "Could not load the image \"" << url() << "\"";
		return false;
	}

	if (preferredFrameType_.pixelFormat() != FrameType::FORMAT_UNDEFINED && (frame.pixelFormat() != preferredFrameType_.pixelFormat() || frame.pixelOrigin() != preferredFrameType_.pixelOrigin()))
	{
		CV::FrameConverter::Comfort::change(frame, preferredFrameType_.pixelFormat(), preferredFrameType_.pixelOrigin(), true, WorkerPool::get().scopedWorker()());
	}

	ocean_assert(frame);

	return deliverNewFrame(std::move(frame));
}

void IIOBufferImage::release()
{
	frameCollection_.clear();
}

}

}

}
