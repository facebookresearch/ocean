// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/FrameMemoryRecorder.h"

namespace Ocean
{

namespace Media
{

FrameMemoryRecorder::FrameMemoryRecorder()
{
	recorderType = Type(recorderType | FRAME_MEMORY_RECORDER);
}

FrameMemoryRecorder::~FrameMemoryRecorder()
{
	// nothing to do here
}

}

}
