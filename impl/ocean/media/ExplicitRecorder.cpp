/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
