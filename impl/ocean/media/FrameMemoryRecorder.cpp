/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
