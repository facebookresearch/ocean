/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
