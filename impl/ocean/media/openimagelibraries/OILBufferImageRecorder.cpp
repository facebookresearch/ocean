/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/OILBufferImageRecorder.h"
#include "ocean/media/openimagelibraries/Image.h"

#include "ocean/base/String.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

OILBufferImageRecorder::OILBufferImageRecorder()
{
	// nothing to do here
}

OILBufferImageRecorder::~OILBufferImageRecorder()
{
	// nothing to do here
}

bool OILBufferImageRecorder::saveImage(const Frame& frame, const std::string& imageType, std::vector<unsigned char>& buffer)
{
	return Image::encodeImage(frame, imageType, buffer);
}

bool OILBufferImageRecorder::buffer(std::vector<unsigned char>& data) const
{
	const ScopedLock scopedLock(recorderLock);

	data = recorderBuffer;
	return true;
}

OILBufferImageRecorder::Encoders OILBufferImageRecorder::frameEncoders() const
{
	static Encoders encoders;

	if (encoders.empty())
	{
		encoders.push_back("bmp");
		encoders.push_back("dds");
		encoders.push_back("jpg");
		encoders.push_back("jpe");
		encoders.push_back("jpeg");
		encoders.push_back("pcx");
		encoders.push_back("png");
		encoders.push_back("raw");
		encoders.push_back("tga");
		encoders.push_back("tif");
		encoders.push_back("tiff");
		encoders.push_back("webp");
	}

	return encoders;
}

bool OILBufferImageRecorder::lockBufferToFill(Frame& recorderFrame, const bool /*respectFrameFrequency*/)
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

void OILBufferImageRecorder::unlockBufferToFill()
{
	const ScopedLock scopedLock(recorderLock);

	if (!recorderFrame_.isValid())
	{
		ocean_assert(false && "The image buffer hasn't been locked before");
	}
	else
	{
		saveImage(recorderFrame_, recorderBufferType, recorderBuffer);
	}

	recorderFrame_.release();
}

}

}

}
