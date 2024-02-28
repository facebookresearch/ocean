// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testbase/TestSTL.h"

#include <cstdlib>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestSTL::testSTL()
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
