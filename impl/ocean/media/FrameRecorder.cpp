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
	recorderFrameFrequency(25)
{
	recorderType = Type(recorderType | FRAME_RECORDER);
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
	recorderFrameType = type;
	return true;
}

bool FrameRecorder::setFrameFrequency(const double frequency)
{
	recorderFrameFrequency = frequency;
	return true;
}

bool FrameRecorder::setFrameEncoder(const std::string& encoder)
{
	recorderFrameEncoder = encoder;
	return true;
}

bool FrameRecorder::frameEncoderConfiguration(long long /*data*/)
{
	// must be implemented in a derived class

	return false;
}

}

}
