// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/openimagelibraries/OILBufferImage.h"
#include "ocean/media/openimagelibraries/Image.h"
#include "ocean/media/openimagelibraries/OILLibrary.h"

#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

OILBufferImage::OILBufferImage(const std::string& url) :
	Medium(url),
	FrameMedium(url),
	BufferImage(url)
{
	libraryName_ = nameOpenImageLibrariesLibrary();
	isValid_ = true;
}

OILBufferImage::~OILBufferImage()
{
	stop();
}

bool OILBufferImage::start()
{
	const ScopedLock scopedLock(lock_);

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

bool OILBufferImage::pause()
{
	return false;
}

bool OILBufferImage::stop()
{
	const ScopedLock scopedLock(lock_);

	release();

	imageStarted = false;

	mediumStartTimestamp = Timestamp();
	mediumPauseTimestamp = Timestamp();
	mediumStopTimestamp.toNow();

	return true;
}

MediumRef OILBufferImage::clone() const
{
	const ScopedLock scopedLock(lock_);

	if (isValid_)
	{
		const BufferImageRef bufferImage(OILLibrary::newImage(url_, true));
		ocean_assert(bufferImage);

		bufferImage->setBufferImage(imageBuffer, imageBufferSize);
		return bufferImage;
	}

	return MediumRef();
}

void OILBufferImage::release()
{
	const ScopedLock scopedLock(lock_);

	frameCollection_.release();
}

bool OILBufferImage::setPreferredFramePixelFormat(const FrameType::PixelFormat pixelFormat)
{
	const ScopedLock scopedLock(lock_);

	if (pixelFormat == preferredFrameType_.pixelFormat())
	{
		return true;
	}

	const FrameRef existingFrame = frameCollection_.get(Timestamp(true));

	if (existingFrame && *existingFrame)
	{
		// if we have loaded the frame already, we simply try to convert the frame to the preferred pixel format (if different)

		if (existingFrame->pixelFormat() == pixelFormat)
		{
			preferredFrameType_ = MediaFrameType(preferredFrameType_, pixelFormat);
		}
		else
		{
			Frame preferredFrame;
			if (!CV::FrameConverter::Comfort::convert(*existingFrame, FrameType(*existingFrame, pixelFormat), preferredFrame, true, WorkerPool::get().conditionalScopedWorker(existingFrame->pixels() >= 400u * 400u)()))
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
		preferredFrameType_ = MediaFrameType(preferredFrameType_, pixelFormat);
	}

	return true;
}

bool OILBufferImage::loadImage()
{
	const ScopedLock scopedLock(lock_);

	if (imageBuffer == nullptr || imageBufferSize == 0)
	{
		return false;
	}

	const Timestamp frameTimestamp(true);

	Frame newFrame(Image::decodeImage(imageBuffer, imageBufferSize, imageBufferType));

	if (!newFrame.isValid())
	{
		return false;
	}

	if (preferredFrameType_.pixelFormat() != FrameType::FORMAT_UNDEFINED && preferredFrameType_.pixelFormat() != newFrame.pixelFormat())
	{
		Frame convertedFrame;
		if (CV::FrameConverter::Comfort::convert(newFrame, FrameType(newFrame, preferredFrameType_.pixelFormat()), convertedFrame, true, WorkerPool::get().conditionalScopedWorker(newFrame.pixels() >= 400u * 400u)()))
		{
			std::swap(newFrame, convertedFrame);
		}

		// we do not handle the case when we cannot convert the new frame to the preferred pixel format
		// as we do not guarantee that the resulting pixel format matches with the preferred pixel format
	}

	newFrame.setTimestamp(frameTimestamp);

	return deliverNewFrame(std::move(newFrame));
}

}

}

}
