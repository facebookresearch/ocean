/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/ImageSequenceRecorder.h"

namespace Ocean
{

namespace Media
{

ImageSequenceRecorder::ImageSequenceRecorder() :
	FrameRecorder(),
	recorderMode(RM_IMMEDIATE),
	recorderStartIndex(0u)
{
	recorderType = Type(recorderType | IMAGE_SEQUENCE_RECORDER);
}

ImageSequenceRecorder::~ImageSequenceRecorder()
{
	// nothing to do here
}

ImageSequenceRecorder::RecorderMode ImageSequenceRecorder::mode() const
{
	return recorderMode;
}

unsigned int ImageSequenceRecorder::startIndex() const
{
	return recorderStartIndex;

}

bool ImageSequenceRecorder::setMode(const RecorderMode mode)
{
	recorderMode = mode;
	return true;
}

bool ImageSequenceRecorder::setStartIndex(const unsigned int index)
{
	recorderStartIndex = index;
	return true;
}

bool ImageSequenceRecorder::addImage(const Frame& frame)
{
	if (!frame.isValid())
	{
		return false;
	}

	if (frameType() != frame.frameType())
	{
		return false;
	}

	Frame recorderFrame;
	if (!lockBufferToFill(recorderFrame, false))
	{
		return false;
	}

	ocean_assert(recorderFrame.isValid());
	ocean_assert(recorderFrame.frameType() == frame.frameType());

	recorderFrame.copy(frame);

	unlockBufferToFill();

	return true;
}

}

}
