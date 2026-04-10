/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/FrameRecorder.h"

namespace Ocean
{

namespace Media
{

FrameRecorder::FrameRecorder() :
	Recorder(),
	frameFrequency_(25)
{
	recorderType_ = Type(recorderType_ | FRAME_RECORDER);
}

FrameRecorder::~FrameRecorder()
{
	// nothing to do here
}

FrameRecorder::Encoders FrameRecorder::frameEncoders() const
{
	return Encoders();
}

bool FrameRecorder::frameEncoderHasConfiguration()
{
	return false;
}

bool FrameRecorder::setPreferredFrameType(const FrameType& type)
{
	frameType_ = type;
	return true;
}

bool FrameRecorder::setFrameFrequency(const double frequency)
{
	frameFrequency_ = frequency;
	return true;
}

bool FrameRecorder::setFrameEncoder(const std::string& encoder)
{
	frameEncoder_ = encoder;
	return true;
}

bool FrameRecorder::frameEncoderConfiguration(long long /*data*/)
{
	// must be implemented in a derived class

	return false;
}

}

}
