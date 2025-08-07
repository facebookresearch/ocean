/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/ImageRecorder.h"

namespace Ocean
{

namespace Media
{

ImageRecorder::ImageRecorder() :
	recorderSaveImage(false)
{
	recorderType = Type(recorderType | IMAGE_RECORDER);
}

ImageRecorder::~ImageRecorder()
{
	// nothing to do here
}

bool ImageRecorder::start()
{
	const ScopedLock scopedLock(recorderLock);

	if (recorderSaveImage)
		return false;

	recorderSaveImage = true;
	return true;
}

bool ImageRecorder::stop()
{
	return false;
}

bool ImageRecorder::isRecording() const
{
	return recorderSaveImage;
}

bool ImageRecorder::lockBufferToFill(Frame& /*recorderFrame*/, const bool /*respectFrameFrequency*/)
{
	// must be implemented in a derived class

	return false;
}

void ImageRecorder::unlockBufferToFill()
{
	// nothing to do here
}

}

}
