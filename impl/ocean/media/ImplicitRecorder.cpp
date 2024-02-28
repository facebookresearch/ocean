// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/ImplicitRecorder.h"

namespace Ocean
{

namespace Media
{

ImplicitRecorder::ImplicitRecorder()
{
	recorderType = Type(recorderType | IMPLICIT_RECORDER);
}

ImplicitRecorder::~ImplicitRecorder()
{
	// nothing to do here
}

}

}
