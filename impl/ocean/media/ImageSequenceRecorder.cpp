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
	recorderMode_(RM_IMMEDIATE),
	startIndex_(0u)
{
	recorderType_ = Type(recorderType_ | IMAGE_SEQUENCE_RECORDER);
}

ImageSequenceRecorder::~ImageSequenceRecorder()
{
	// nothing to do here
}

ImageSequenceRecorder::RecorderMode ImageSequenceRecorder::mode() const
{
	return recorderMode_;
}

unsigned int ImageSequenceRecorder::startIndex() const
{
	return startIndex_;

}

bool ImageSequenceRecorder::setMode(const RecorderMode mode)
{
	recorderMode_ = mode;
	return true;
}

bool ImageSequenceRecorder::setStartIndex(const unsigned int index)
{
	startIndex_ = index;
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
