/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/BufferImageRecorder.h"

namespace Ocean
{

namespace Media
{

BufferImageRecorder::BufferImageRecorder() :
	recorderSaveImage(false)
{
	recorderType = Type(recorderType | BUFFER_IMAGE_RECORDER);
}

BufferImageRecorder::~BufferImageRecorder()
{
	// nothing to do here
}

bool BufferImageRecorder::start()
{
	const ScopedLock scopedLock(recorderLock);

	if (recorderSaveImage)
		return false;

	recorderSaveImage = true;
	return true;
}

bool BufferImageRecorder::stop()
{
	return false;
}

bool BufferImageRecorder::isRecording() const
{
	return recorderSaveImage;
}

bool BufferImageRecorder::lockBufferToFill(Frame& /*recorderFrame*/, const bool /*respectFrameFrequency*/)
{
	// must be implemented in a derived class

	return false;
}

void BufferImageRecorder::unlockBufferToFill()
{
	// nothing to do here
}

}

}
