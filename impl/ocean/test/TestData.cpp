/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/TestData.h"

namespace Ocean
{

namespace Test
{

TestData::TestData(Value&& value, Value&& expectation) :
	dataType_(DT_VALUE),
	value_(std::move(value)),
	expectation_(std::move(expectation))
{
	// nothing to do here
}

TestData::TestData(Frame&& image, Value&& expectation) :
	dataType_(DT_IMAGE),
	image_(std::move(image)),
	expectation_(std::move(expectation))
{
	// nothing to do here
}

}

}

