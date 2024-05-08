/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestScopedFunction.h"

#include "ocean/base/ScopedFunction.h"
#include "ocean/base/Timestamp.h"

#include <functional>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestScopedFunction::test(const double testDuration)
{
	ocean_assert_and_suppress_unused(testDuration > 0.0, testDuration);

	Log::info() << "---   ScopedFunction test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testRelease() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "ScopedObject test succeeded.";
	}
	else
	{
		Log::info() << "ScopedObject test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestScopedFunction, Release)
{
	EXPECT_TRUE(TestScopedFunction::testRelease());
}

#endif // OCEAN_USE_GTEST

bool TestScopedFunction::testRelease()
{
	Log::info() << "Testing release functionality:";

	bool allSucceeded = true;

	{
		// testing object

		Object object;

		for (size_t n = 0; n < 5; ++n)
		{
			if (object.number() != n)
			{
				allSucceeded = false;
			}

			object.invokeFunction();
		}
	}

	{
		Object object;

		{
			const ScopedFunctionVoid scopedFunction(std::bind(&Object::invokeFunction, &object));

			if (object.number() != 0)
			{
				allSucceeded = false;
			}
		}

		if (object.number() != 1)
		{
			allSucceeded = false;
		}
	}

	{
		Object object;

		{
			ScopedFunctionVoid scopedFunction(std::bind(&Object::invokeFunction, &object));

			if (object.number() != 0)
			{
				allSucceeded = false;
			}

			scopedFunction.release();

			if (object.number() != 1)
			{
				allSucceeded = false;
			}
		}

		if (object.number() != 1)
		{
			allSucceeded = false;
		}
	}

	{
		Object object;

		{
			ScopedFunctionVoid scopedFunction(std::bind(&Object::invokeFunction, &object));

			if (object.number() != 0)
			{
				allSucceeded = false;
			}

			scopedFunction.revoke();

			if (object.number() != 0)
			{
				allSucceeded = false;
			}
		}

		if (object.number() != 0)
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
