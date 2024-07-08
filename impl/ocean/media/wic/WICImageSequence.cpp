/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/wic/WICImageSequence.h"
#include "ocean/media/wic/Image.h"
#include "ocean/media/wic/WICLibrary.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

WICImageSequence::WICImageSequence(const std::string& url) :
	Medium(url),
	FiniteMedium(url),
	FrameMedium(url),
	ImageSequence(url),
	ImageFileSequence(url)
{
	libraryName_ = nameWICLibrary();

	isValid_ = determineSequence();
}

WICImageSequence::~WICImageSequence()
{
	// nothing to do here
}

MediumRef WICImageSequence::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return WICLibrary::newImageSequence(url_, true);
	}

	return MediumRef();
}

bool WICImageSequence::loadImage(const std::string& filename, const Timestamp timestamp, Frame* frame)
{
	Frame result = Image::readImage(filename);

	if (!result.isValid())
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
