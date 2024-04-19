// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
	imageStarted = isValid_;

	if (imageStarted)
	{
		mediumStartTimestamp.toNow();
		mediumPauseTimestamp = Timestamp();
		mediumStopTimestamp = Timestamp();
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

	imageStarted = false;

	mediumStartTimestamp = Timestamp();
	mediumPauseTimestamp = Timestamp();
	mediumStopTimestamp.toNow();

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

		bufferImage->setBufferImage(imageBuffer, imageBufferSize);
		return bufferImage;
	}

	return MediumRef();
}

bool IIOBufferImage::setPreferredFramePixelFormat(const FrameType::PixelFormat format)
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

bool IIOBufferImage::loadImage()
{
	if (imageBuffer == nullptr || imageBufferSize == 0)
	{
		return false;
	}

	Frame frame = ImageIO::Image::decodeImage(imageBuffer, imageBufferSize);

	if (!frame.isValid())
	{
		Log::error() << "Could not load the image \"" << url() << "\"";
		return false;
	}

	if (preferredFrameType_.pixelFormat() != FrameType::FORMAT_UNDEFINED && (frame.pixelFormat() != preferredFrameType_.pixelFormat() || frame.pixelOrigin() != preferredFrameType_.pixelOrigin()))
	{
		CV::FrameConverter::Comfort::change(frame, FrameType(frame, preferredFrameType_.pixelFormat(), preferredFrameType_.pixelOrigin()), true, WorkerPool::get().scopedWorker()());
	}

	ocean_assert(frame);

	return deliverNewFrame(std::move(frame));
}

void IIOBufferImage::release()
{
	frameCollection_.release();
}

}

}

}
