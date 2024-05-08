/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_FRAME_MEMORY_RECORDER_H
#define META_OCEAN_MEDIA_FRAME_MEMORY_RECORDER_H

#include "ocean/media/Media.h"
#include "ocean/media/FrameRecorder.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class FrameMemoryRecorder;

/**
 * Definition of a object reference holding a frame memory recorder.
 * @see SmartObjectRef, FrameMemoryRecorder.
 * @ingroup media
 */
typedef Ocean::SmartObjectRef<FrameMemoryRecorder, Recorder> FrameMemoryRecorderRef;

/**
 * This class implements a frame recorder forwarding the frame to a shared memory.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT FrameMemoryRecorder : virtual public FrameRecorder
{
	protected:

		/**
		 * Creates a new frame memory recorder object.
		 */
		FrameMemoryRecorder();

		/**
		 * Destructs a frame memory recorder object.
		 */
		~FrameMemoryRecorder() override;
};

}

}

#endif // META_OCEAN_MEDIA_FRAME_MEMORY_RECORDER_H
