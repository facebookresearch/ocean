/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestBinary.h"

#include "ocean/base/Binary.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestBinary::test(const double /*testDuration*/)
{
	Log::info() << "---   Binary test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHideString() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Binary test succeeded.";
	}
	else
	{
		Log::info() << "Binary test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestBinary, HideString)
{
	EXPECT_TRUE(TestBinary::testHideString());
}

#endif // OCEAN_USE_GTEST

bool TestBinary::testHideString()
{
	Log::info() << "Hide string test:";

	bool allSucceeded = true;

	{
		std::wstring value;
		HIDE_STRING_32(L"test", value, L"\x3C\x9F\x63\x44", L"\x25\xEA\x48\x7B", 4 + 2);

		if (value != L"test")
		{
			allSucceeded = false;
		}
	}

	{
		std::wstring value;
		HIDE_STRING_32(L"This is a long string", value, L"\x44\x77\x12\x91\xAF\x03\xB3\xC4\x40\x60\x80\x90\x76\x23\xEE\xCC\x81\x90\x45\x46\x30\x7E", L"\x56\xAE\x50\xB7\x61\xE5\x3A\x80\x56\xBC\x43\x45\x56\xCC\xD5\xB1\xB9\x30\x84\x27\x40", 21 + 2);

		if (value != L"This is a long string")
		{
			allSucceeded = false;
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

}

}

}
