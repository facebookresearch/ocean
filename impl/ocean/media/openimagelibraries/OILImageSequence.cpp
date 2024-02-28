// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/openimagelibraries/OILImageSequence.h"
#include "ocean/media/openimagelibraries/Image.h"
#include "ocean/media/openimagelibraries/OILLibrary.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

OILImageSequence::OILImageSequence(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	FrameMedium(url),
	ImageSequence(url),
	ImageFileSequence(url)
{
	libraryName_ = nameOpenImageLibrariesLibrary();

	isValid_ = determineSequence();
}

OILImageSequence::~OILImageSequence()
{
	// nothing to do here
}

MediumRef OILImageSequence::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return OILLibrary::newImageSequence(url_, true);
	}

	return MediumRef();
}

bool OILImageSequence::loadImage(const std::string& filename, const Timestamp timestamp, Frame* frame)
{
	Frame result = Image::readImage(filename);

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
