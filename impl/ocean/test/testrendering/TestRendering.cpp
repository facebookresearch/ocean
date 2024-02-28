// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testrendering/TestRendering.h"

namespace Ocean
{

namespace Test
{

namespace TestRendering
{

bool testRendering(const double /*testDuration*/, Worker& /*worker*/)
{
	bool allSucceeded = true;

	Log::info() << "+++   Ocean Rendering Library test:   +++";

	std::vector<std::string> tests;
	const std::set<std::string> testSet(tests.begin(), tests.end());

	if (allSucceeded)
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Rendering library test succeeded!";
	}
	else
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Rendering library test FAILED!";
	}

	return allSucceeded;
}

}

}

}
