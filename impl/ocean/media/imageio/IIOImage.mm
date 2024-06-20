/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/imageio/IIOImage.h"
#include "ocean/media/imageio/IIOLibrary.h"
#include "ocean/media/imageio/Image.h"

#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

IIOImage::IIOImage(const std::string& url) :
	Medium(url),
	FrameMedium(url),
	Image(url),
	imageStarted(false)
{
	libraryName_ = nameImageIOLibrary();

	isValid_ = loadImage();
}

IIOImage::~IIOImage()
{
	stop();
}

bool IIOImage::isStarted() const
{
	return imageStarted;
}

Timestamp IIOImage::startTimestamp() const
{
	return mediumStartTimestamp;
}

Timestamp IIOImage::pauseTimestamp() const
{
	return mediumPauseTimestamp;
}

Timestamp IIOImage::stopTimestamp() const
{
	return mediumStopTimestamp;
}

MediumRef IIOImage::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return IIOLibrary::newImage(url_, true);
	}

	return MediumRef();
}

bool IIOImage::start()
{
	isValid_ = loadImage();
	imageStarted = isValid_;

	if (imageStarted)
	{
		mediumStartTimestamp.toNow();
		mediumPauseTimestamp = Timestamp();
		mediumStopTimestamp = Timestamp();
	}

	return isValid_;
}

bool IIOImage::pause()
{
	return false;
}

bool IIOImage::stop()
{
	release();
	imageStarted = false;

	mediumStartTimestamp = Timestamp();
	mediumPauseTimestamp = Timestamp();
	mediumStopTimestamp.toNow();

	return true;
}

bool IIOImage::setPreferredFramePixelFormat(const FrameType::PixelFormat format)
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

bool IIOImage::loadImage()
{
	if (frameCollection_)
	{
		return true;
	}

	Frame frame = ImageIO::Image::readImage(url());

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

void IIOImage::release()
{
	frameCollection_.clear();
}

}

}

}
