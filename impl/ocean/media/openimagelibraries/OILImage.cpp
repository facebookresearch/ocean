/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/OILImage.h"
#include "ocean/media/openimagelibraries/Image.h"
#include "ocean/media/openimagelibraries/OILBufferImage.h"
#include "ocean/media/openimagelibraries/OILLibrary.h"

#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include <fstream>

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

OILImage::OILImage(const std::string& url) :
	Medium(url),
	FrameMedium(url),
	Image(url)
{
	libraryName_ = nameOpenImageLibrariesLibrary();

	isValid_ = start();
}

OILImage::~OILImage()
{
	stop();
}

bool OILImage::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_.isValid();
}

Timestamp OILImage::startTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_;
}

Timestamp OILImage::pauseTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return pauseTimestamp_;
}

Timestamp OILImage::stopTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return stopTimestamp_;
}

MediumRef OILImage::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return OILLibrary::newImage(url_, true);
	}

	return MediumRef();
}

bool OILImage::start()
{
	const ScopedLock scopedLock(lock_);

	if (startTimestamp_.isValid())
	{
		return true;
	}

	isValid_ = loadImage();

	if (isValid_)
	{
		startTimestamp_.toNow();
		pauseTimestamp_.toInvalid();
		stopTimestamp_.toInvalid();
	}

	return isValid_;
}

bool OILImage::pause()
{
	return false;
}

bool OILImage::stop()
{
	const ScopedLock scopedLock(lock_);

	release();

	startTimestamp_ = Timestamp();
	pauseTimestamp_ = Timestamp();
	stopTimestamp_.toNow();

	return true;
}

bool OILImage::setPreferredFramePixelFormat(const FrameType::PixelFormat format)
{
	const ScopedLock scopedLock(lock_);

	if (format == preferredFrameType_.pixelFormat())
	{
		return true;
	}

	const FrameRef existingFrame = frameCollection_.get(Timestamp(true));

	if (existingFrame && *existingFrame)
	{
		// if we have loaded the frame already, we simply try to convert the frame to the preferred pixel format (if different)

		if (existingFrame->pixelFormat() == format)
		{
			preferredFrameType_ = MediaFrameType(preferredFrameType_, format);
		}
		else
		{
			Frame preferredFrame;
			if (!CV::FrameConverter::Comfort::convert(*existingFrame, format, preferredFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().conditionalScopedWorker(existingFrame->pixels() >= 400u * 400u)()))
			{
				return false;
			}

			preferredFrame.setTimestamp(existingFrame->timestamp());

			if (!deliverNewFrame(std::move(preferredFrame)))
			{
				return false;
			}
		}
	}
	else
	{
		// if we haven't loaded the frame yet, we simply store the preferred pixel format
		preferredFrameType_ = MediaFrameType(preferredFrameType_, format);
	}

	return true;
}

bool OILImage::loadImage()
{
	const ScopedLock scopedLock(lock_);

	const Timestamp frameTimestamp(true);

	Frame newFrame(OpenImageLibraries::Image::readImage(url_));

	if (!newFrame.isValid())
	{
		return false;
	}

	if (preferredFrameType_.pixelFormat() != FrameType::FORMAT_UNDEFINED && preferredFrameType_.pixelFormat() != newFrame.pixelFormat())
	{
		Frame convertedFrame;
		if (CV::FrameConverter::Comfort::convert(newFrame, preferredFrameType_.pixelFormat(), convertedFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().conditionalScopedWorker(newFrame.pixels() >= 400u * 400u)()))
		{
			std::swap(newFrame, convertedFrame);
		}

		// we do not handle the case when we cannot convert the new frame to the preferred pixel format
		// as we do not guarantee that the resulting pixel format matches with the preferred pixel format
	}

	newFrame.setTimestamp(frameTimestamp);

	return deliverNewFrame(std::move(newFrame));
}

void OILImage::release()
{
	const ScopedLock scopedLock(lock_);

	frameCollection_.clear();
}

}

}

}
