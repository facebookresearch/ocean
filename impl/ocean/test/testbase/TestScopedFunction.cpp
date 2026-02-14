/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestScopedFunction.h"

#include "ocean/base/ScopedFunction.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

#include <functional>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestScopedFunction::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert_and_suppress_unused(testDuration > 0.0, testDuration);

	TestResult testResult("ScopedFunction test");
	Log::info() << " ";

	if (selector.shouldRun("release"))
	{
		testResult = testRelease();

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	Validation validation;

	{
		// testing object

		Object object;

		for (size_t n = 0; n < 5; ++n)
		{
			OCEAN_EXPECT_EQUAL(validation, object.number(), n);

			object.invokeFunction();
		}
	}

	{
		Object object;

		{
			const ScopedFunctionVoid scopedFunction(std::bind(&Object::invokeFunction, &object));

			OCEAN_EXPECT_EQUAL(validation, object.number(), size_t(0));
		}

		OCEAN_EXPECT_EQUAL(validation, object.number(), size_t(1));
	}

	{
		Object object;

		{
			ScopedFunctionVoid scopedFunction(std::bind(&Object::invokeFunction, &object));

			OCEAN_EXPECT_EQUAL(validation, object.number(), size_t(0));

			scopedFunction.release();

			OCEAN_EXPECT_EQUAL(validation, object.number(), size_t(1));
		}

		OCEAN_EXPECT_EQUAL(validation, object.number(), size_t(1));
	}

	{
		Object object;

		{
			ScopedFunctionVoid scopedFunction(std::bind(&Object::invokeFunction, &object));

			OCEAN_EXPECT_EQUAL(validation, object.number(), size_t(0));

			scopedFunction.revoke();

			OCEAN_EXPECT_EQUAL(validation, object.number(), size_t(0));
		}

		OCEAN_EXPECT_EQUAL(validation, object.number(), size_t(0));
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
