/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/OILImageRecorder.h"
#include "ocean/media/openimagelibraries/Image.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

OILImageRecorder::OILImageRecorder()
{
	// nothing to do here
}

OILImageRecorder::~OILImageRecorder()
{
	// nothing to do here
}

bool OILImageRecorder::saveImage(const Frame& frame, const std::string& filename)
{
	return Image::writeImage(frame, filename);
}

OILImageRecorder::Encoders OILImageRecorder::frameEncoders() const
{
	static Encoders encoders;

	if (encoders.empty())
	{
		encoders.emplace_back("bmp");
		encoders.emplace_back("jpg");
		encoders.emplace_back("jpe");
		encoders.emplace_back("jpeg");
		encoders.emplace_back("png");
		encoders.emplace_back("tif");
		encoders.emplace_back("tiff");
	}

	return encoders;
}

bool OILImageRecorder::lockBufferToFill(Frame& recorderFrame, const bool /*respectFrameFrequency*/)
{
	const ScopedLock scopedLock(lock_);

	if (recorderFrame_)
	{
		ocean_assert(false && "The image buffer is locked already");
		return false;
	}

	if (!saveImage_)
	{
		return false;
	}

	recorderFrame_ = Frame(frameType_);
	if (!recorderFrame_.isValid())
	{
		return false;
	}

	saveImage_ = false;

	recorderFrame = Frame(recorderFrame_, Frame::ACM_USE_KEEP_LAYOUT);

	return true;
}

void OILImageRecorder::unlockBufferToFill()
{
	const ScopedLock scopedLock(lock_);

	if (!recorderFrame_.isValid())
	{
		ocean_assert(false && "The image buffer hasn't been locked before");
	}
	else
	{
		saveImage(recorderFrame_, filename_);
	}

	recorderFrame_.release();
}

}

}

}
