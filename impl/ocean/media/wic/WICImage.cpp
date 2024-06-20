/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/wic/WICImage.h"
#include "ocean/media/wic/Image.h"
#include "ocean/media/wic/WICLibrary.h"

#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

WICImage::WICImage(const std::string& url) :
	Medium(url),
	FrameMedium(url),
	Image(url)
{
	libraryName_ = nameWICLibrary();

	isValid_ = loadImage();
}

WICImage::~WICImage()
{
	stop();
}

bool WICImage::isStarted() const
{
	return imageStarted_;
}

Timestamp WICImage::startTimestamp() const
{
	return mediumStartTimestamp;
}

Timestamp WICImage::pauseTimestamp() const
{
	return mediumPauseTimestamp;
}

Timestamp WICImage::stopTimestamp() const
{
	return mediumStopTimestamp;
}

MediumRef WICImage::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return WICLibrary::newImage(url_, true);
	}

	return MediumRef();
}

bool WICImage::start()
{
	isValid_ = loadImage();
	imageStarted_ = isValid_;

	if (imageStarted_)
	{
		mediumStartTimestamp.toNow();
		mediumPauseTimestamp = Timestamp();
		mediumStopTimestamp = Timestamp();
	}

	return isValid_;
}

bool WICImage::pause()
{
	return false;
}

bool WICImage::stop()
{
	release();
	imageStarted_ = false;

	mediumStartTimestamp = Timestamp();
	mediumPauseTimestamp = Timestamp();
	mediumStopTimestamp.toNow();

	return true;
}

bool WICImage::setPreferredFramePixelFormat(const FrameType::PixelFormat format)
{
	if (format == preferredFrameType_.pixelFormat())
		return true;

	if (frameCollection_.isNull() || format == FrameType::FORMAT_UNDEFINED)
	{
		preferredFrameType_ = MediaFrameType(preferredFrameType_, format);
		return true;
	}

	return false;
}

bool WICImage::loadImage()
{
	if (frameCollection_)
	{
		return true;
	}

	Frame frame = WIC::Image::readImage(url());

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

	return deliverNewFrame(std::move(frame));
}

void WICImage::release()
{
	frameCollection_.clear();
}

}

}

}
