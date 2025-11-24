/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestSTL.h"

#include "ocean/test/TestSelector.h"

#include <cstdlib>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestSTL::testSTL(const TestSelector& /*selector*/)
{
	Log::info() << "---   Testing C++ STL functions:   ---";
	Log::info() << " ";

	Log::info() << "Testing atexit:";
	atexit(onAtExit);

	Log::info() << " ";
	Log::info() << "C++ STL succeeded.";

	return true;
}

void TestSTL::onAtExit()
{
	Log::info() << "STL::onAtExit executed successfully!";
}

}

}

}
