// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/MovieRecorder.h"

namespace Ocean
{

namespace Media
{

MovieRecorder::MovieRecorder() :
	FrameRecorder()
{
	recorderType = Type(recorderType | MOVIE_RECORDER);
}

MovieRecorder::~MovieRecorder()
{
	// nothing to do here
}

}

}
