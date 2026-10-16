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
	const ScopedLock scopedLock(lock_);

	data = buffer_;
	return true;
}

OILBufferImageRecorder::Encoders OILBufferImageRecorder::frameEncoders() const
{
	static Encoders encoders;

	if (encoders.empty())
	{
		encoders.emplace_back("bmp");
		encoders.emplace_back("dds");
		encoders.emplace_back("jpg");
		encoders.emplace_back("jpe");
		encoders.emplace_back("jpeg");
		encoders.emplace_back("pcx");
		encoders.emplace_back("png");
		encoders.emplace_back("raw");
		encoders.emplace_back("tga");
		encoders.emplace_back("tif");
		encoders.emplace_back("tiff");
		encoders.emplace_back("webp");
	}

	return encoders;
}

bool OILBufferImageRecorder::lockBufferToFill(Frame& recorderFrame, const bool /*respectFrameFrequency*/)
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

void OILBufferImageRecorder::unlockBufferToFill()
{
	const ScopedLock scopedLock(lock_);

	if (!recorderFrame_.isValid())
	{
		ocean_assert(false && "The image buffer hasn't been locked before");
	}
	else
	{
		saveImage(recorderFrame_, bufferType_, buffer_);
	}

	recorderFrame_.release();
}

}

}

}
