/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/BufferRecorder.h"

namespace Ocean
{

namespace Media
{

BufferRecorder::BufferRecorder() :
	Recorder()
{
	recorderType = Type(recorderType | BUFFER_RECORDER);
}

BufferRecorder::~BufferRecorder()
{
	// nothing to do here
}

const std::string& BufferRecorder::bufferType() const
{
	return recorderBufferType;
}

bool BufferRecorder::setBufferType(const std::string& bufferType)
{
	recorderBufferType = bufferType;
	return true;
}

}

}
