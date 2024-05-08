/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestIO.h"
#include "ocean/test/testio/TestBase64.h"
#include "ocean/test/testio/TestBitstream.h"
#include "ocean/test/testio/TestCompression.h"
#include "ocean/test/testio/TestDirectory.h"
#include "ocean/test/testio/TestFile.h"
#include "ocean/test/testio/TestUtilities.h"

#include "ocean/base/String.h"
#include "ocean/base/Utilities.h"

#include <set>

namespace Ocean
{

namespace Test
{

namespace TestIO
{

bool testIO(const double testDuration, const std::string& testFunctions)
{
	bool allSucceeded = true;

	Log::info() << "Ocean IO Library test:";

	std::vector<std::string> tests(Utilities::separateValues(String::toLower(testFunctions), ',', true, true));
	const std::set<std::string> testSet(tests.begin(), tests.end());

	if (testSet.empty() || testSet.find("bitstream") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
		allSucceeded = TestBitstream::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("compression") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
		allSucceeded = TestCompression::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("base64") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
		allSucceeded = TestBase64::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("directory") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
		allSucceeded = TestDirectory::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("file") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
		allSucceeded = TestFile::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("utilities") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
		allSucceeded = TestUtilities::test(testDuration) && allSucceeded;
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Ocean IO Library test succeeded.";
	}
	else
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Ocean IO Library test FAILED!";
	}

	return allSucceeded;
}

}

}

}
