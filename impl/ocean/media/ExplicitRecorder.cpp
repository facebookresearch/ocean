// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/ExplicitRecorder.h"

namespace Ocean
{

namespace Media
{

ExplicitRecorder::ExplicitRecorder()
{
	recorderType = Type(recorderType | EXPLICIT_RECORDER);
}

ExplicitRecorder::~ExplicitRecorder()
{
	// nothing to do here
}

}

}
