/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/wic/WICImageRecorder.h"
#include "ocean/media/wic/Image.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

WICImageRecorder::WICImageRecorder()
{
	// nothing to do here
}

WICImageRecorder::~WICImageRecorder()
{
	// nothing to do here
}

bool WICImageRecorder::saveImage(const Frame& frame, const std::string& filename)
{
	return Image::writeImage(frame, filename);
}

WICImageRecorder::Encoders WICImageRecorder::frameEncoders() const
{
	static Encoders encoders;

	if (encoders.empty())
	{
		encoders.push_back("bmp");
		encoders.push_back("gif");
		encoders.push_back("jpg");
		encoders.push_back("jpeg");
		encoders.push_back("png");
		encoders.push_back("tif");
		encoders.push_back("tiff");
		encoders.push_back("wmp");
	}

	return encoders;
}

bool WICImageRecorder::lockBufferToFill(Frame& recorderFrame, const bool /*respectFrameFrequency*/)
{
	const ScopedLock scopedLock(recorderLock);

	// **TODO** missing implementation, handle 'respectFrameFrequency'

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

void WICImageRecorder::unlockBufferToFill()
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
