/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/imageio/IIOImageRecorder.h"
#include "ocean/media/imageio/Image.h"

#include "ocean/base/StringApple.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

IIOImageRecorder::IIOImageRecorder()
{
	// nothing to do here
}

IIOImageRecorder::~IIOImageRecorder()
{
	// nothing to do here
}

bool IIOImageRecorder::saveImage(const Frame& frame, const std::string& filename)
{
	return ImageIO::Image::writeImage(frame, addOptionalSuffixToFilename(filename, recorderFilenameSuffixed), true);
}

IIOImageRecorder::Encoders IIOImageRecorder::frameEncoders() const
{
	static Encoders encoders;

	if (encoders.empty())
	{
		encoders.push_back("bmp");
		encoders.push_back("gif");
		encoders.push_back("jpg");
		encoders.push_back("jpe");
		encoders.push_back("jpeg");
		encoders.push_back("png");
		encoders.push_back("tif");
		encoders.push_back("tiff");
	}

	return encoders;
}

bool IIOImageRecorder::lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency)
{
	const ScopedLock scopedLock(recorderLock);

	if (recorderFrame_)
	{
		ocean_assert(false && "The image buffer is locked already");
		return false;
	}

	if (!recorderSaveImage)
	{
		return false;
	}

	recorderFrame_ = Frame(recorderFrameType);
	if (!recorderFrame_.isValid())
	{
		return false;
	}

	recorderSaveImage = false;

	recorderFrame = Frame(recorderFrame_, Frame::ACM_USE_KEEP_LAYOUT);

	return true;
}

void IIOImageRecorder::unlockBufferToFill()
{
	const ScopedLock scopedLock(recorderLock);

	if (!recorderFrame_.isValid())
	{
		ocean_assert(false && "The image buffer hasn't been locked before");
	}
	else
	{
		saveImage(recorderFrame_, recorderFilename);
	}

	recorderFrame_.release();
}

}

}

}
