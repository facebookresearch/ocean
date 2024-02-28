// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/imageio/IIOImageSequence.h"
#include "ocean/media/imageio/IIOLibrary.h"
#include "ocean/media/imageio/Image.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

IIOImageSequence::IIOImageSequence(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	FrameMedium(url),
	ImageSequence(url),
	ImageFileSequence(url)
{
	libraryName_ = nameImageIOLibrary();

	isValid_ = determineSequence();
}

IIOImageSequence::~IIOImageSequence()
{
	// nothing to do here
}

MediumRef IIOImageSequence::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return IIOLibrary::newImageSequence(url_, true);
	}

	return MediumRef();
}

bool IIOImageSequence::loadImage(const std::string& filename, const Timestamp timestamp, Frame* frame)
{
	Frame result = ImageIO::Image::readImage(filename);

	if (result.isNull())
	{
		return false;
	}

	result.setTimestamp(timestamp);

	if (frame != nullptr)
	{
		*frame = std::move(result);

		return true;
	}

	return deliverNewFrame(std::move(result));
}

}

}

}
