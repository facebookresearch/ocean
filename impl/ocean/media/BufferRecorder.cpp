// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
