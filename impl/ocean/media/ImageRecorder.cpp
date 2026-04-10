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
	saveImage_(false)
{
	recorderType_ = Type(recorderType_ | IMAGE_RECORDER);
}

ImageRecorder::~ImageRecorder()
{
	// nothing to do here
}

bool ImageRecorder::start()
{
	const ScopedLock scopedLock(lock_);

	if (saveImage_)
		return false;

	saveImage_ = true;
	return true;
}

bool ImageRecorder::stop()
{
	return false;
}

bool ImageRecorder::isRecording() const
{
	return saveImage_;
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
