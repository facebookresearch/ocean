/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testbullseyes/TestBullseye.h"

#include "ocean/base/RandomI.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestBullseyes
{

using namespace CV::Detector::Bullseyes;

bool TestBullseye::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for Bullseye:   ---";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	allSucceeded = testBullseyeConstructor(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBullseyePosition(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBullseyeRadius(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testBullseyeGrayThreshold(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";


	allSucceeded = testBullseyeIsValid(testDuration, randomGenerator) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Bullseye test succeeded.";
	}
	else
	{
		Log::info() << "Bullseye test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

} // namespace TestBullseyes

TEST(TestBullseye, BullseyeConstructor)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseye::testBullseyeConstructor(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestBullseye, BullseyePosition)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseye::testBullseyePosition(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestBullseye, BullseyeRadius)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseye::testBullseyeRadius(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestBullseye, BullseyeGrayThreshold)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseye::testBullseyeGrayThreshold(GTEST_TEST_DURATION, randomGenerator));
}

TEST(TestBullseye, BullseyeIsValid)
{
	RandomGenerator randomGenerator;
	EXPECT_TRUE(TestDetector::TestBullseyes::TestBullseye::testBullseyeIsValid(GTEST_TEST_DURATION, randomGenerator));
}

namespace TestBullseyes
{

#endif // OCEAN_USE_GTEST

bool TestBullseye::testBullseyeIsValid(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Bullseye::isValid() test:";

	bool allSucceeded = true;

	Timestamp start(true);

	do
	{
		Bullseye bullseye;

		const bool isValid = RandomI::random(randomGenerator, 1u) == 0u;

		if (isValid)
		{
			bullseye = TestUtilities::createRandomValidBullseye(randomGenerator);
		}

		if (bullseye.isValid() != isValid)
		{
			allSucceeded = false;
			break;
		}
	}
	while (Timestamp(true) < start + testDuration);

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestBullseye::testBullseyeConstructor(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Bullseye::Bullseye() test:";

	bool allSucceeded = true;

	Timestamp start(true);

	do
	{
		// Test default constructor creates invalid bullseye
		{
			const Bullseye bullseye;

			if (bullseye.isValid())
			{
				allSucceeded = false;
			}

			if (bullseye.radius() != Bullseye::invalidRadius())
			{
				allSucceeded = false;
			}

			if (bullseye.grayThreshold() != Bullseye::invalidGrayThreshold())
			{
				allSucceeded = false;
			}
		}

		// Test parameterized constructor with valid values
		{
			const Bullseye bullseye = TestUtilities::createRandomValidBullseye(randomGenerator);

			if (!bullseye.isValid())
			{
				allSucceeded = false;
			}

			if (bullseye.radius() <= Scalar(0))
			{
				allSucceeded = false;
			}

			if (bullseye.grayThreshold() == 0u || bullseye.grayThreshold() >= 256u)
			{
				allSucceeded = false;
			}
		}
	}
	while (Timestamp(true) < start + testDuration);

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestBullseye::testBullseyePosition(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Bullseye::position() test:";

	bool allSucceeded = true;

	Timestamp start(true);

	do
	{
		const Bullseye bullseye = TestUtilities::createRandomValidBullseye(randomGenerator);

		if (bullseye.position().x() < Scalar(0) || bullseye.position().y() < Scalar(0))
		{
			allSucceeded = false;
		}
	}
	while (Timestamp(true) < start + testDuration);

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestBullseye::testBullseyeRadius(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Bullseye::radius() test:";

	bool allSucceeded = true;

	Timestamp start(true);

	do
	{
		const Bullseye bullseye = TestUtilities::createRandomValidBullseye(randomGenerator);

		if (bullseye.radius() <= Scalar(0))
		{
			allSucceeded = false;
		}
	}
	while (Timestamp(true) < start + testDuration);

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestBullseye::testBullseyeGrayThreshold(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Bullseye::grayThreshold() test:";

	bool allSucceeded = true;

	Timestamp start(true);

	do
	{
		const Bullseye bullseye = TestUtilities::createRandomValidBullseye(randomGenerator);

		if (bullseye.grayThreshold() == 0u || bullseye.grayThreshold() >= 256u)
		{
			allSucceeded = false;
		}
	}
	while (Timestamp(true) < start + testDuration);

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
