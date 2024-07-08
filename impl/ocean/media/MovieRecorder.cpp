/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
