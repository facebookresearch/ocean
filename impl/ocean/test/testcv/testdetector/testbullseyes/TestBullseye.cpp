/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testbullseyes/TestBullseye.h"

#include "ocean/base/RandomI.h"

#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"

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

	Validation validation(randomGenerator);

	Timestamp start(true);

	do
	{
		Bullseye bullseye;

		const bool isValid = RandomI::boolean(randomGenerator);

		if (isValid)
		{
			bullseye = TestUtilities::createRandomValidBullseye(randomGenerator);
		}

		OCEAN_EXPECT_EQUAL(validation, bullseye.isValid(), isValid);
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseye::testBullseyeConstructor(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Bullseye::Bullseye() test:";

	Validation validation(randomGenerator);

	Timestamp start(true);

	do
	{
		// Test default constructor creates invalid bullseye
		{
			const Bullseye bullseye;

			OCEAN_EXPECT_FALSE(validation, bullseye.isValid());

			OCEAN_EXPECT_EQUAL(validation, bullseye.radius(), Bullseye::invalidRadius());

			OCEAN_EXPECT_EQUAL(validation, bullseye.grayThreshold(), Bullseye::invalidGrayThreshold());
		}

		// Test parameterized constructor with valid values
		{
			const Bullseye bullseye = TestUtilities::createRandomValidBullseye(randomGenerator);

			OCEAN_EXPECT_TRUE(validation, bullseye.isValid());

			OCEAN_EXPECT_GREATER(validation, bullseye.radius(), Scalar(0));

			OCEAN_EXPECT_NOT_EQUAL(validation, bullseye.grayThreshold(), 0u);
			OCEAN_EXPECT_LESS(validation, bullseye.grayThreshold(), 256u);
		}
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseye::testBullseyePosition(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Bullseye::position() test:";

	Validation validation(randomGenerator);

	Timestamp start(true);

	do
	{
		const Bullseye bullseye = TestUtilities::createRandomValidBullseye(randomGenerator);

		OCEAN_EXPECT_GREATER_EQUAL(validation, bullseye.position().x(), Scalar(0));
		OCEAN_EXPECT_GREATER_EQUAL(validation, bullseye.position().y(), Scalar(0));
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseye::testBullseyeRadius(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Bullseye::radius() test:";

	Validation validation(randomGenerator);

	Timestamp start(true);

	do
	{
		const Bullseye bullseye = TestUtilities::createRandomValidBullseye(randomGenerator);

		OCEAN_EXPECT_GREATER(validation, bullseye.radius(), Scalar(0));
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestBullseye::testBullseyeGrayThreshold(const double testDuration, RandomGenerator& randomGenerator)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Bullseye::grayThreshold() test:";

	Validation validation(randomGenerator);

	Timestamp start(true);

	do
	{
		const Bullseye bullseye = TestUtilities::createRandomValidBullseye(randomGenerator);

		OCEAN_EXPECT_NOT_EQUAL(validation, bullseye.grayThreshold(), 0u);
		OCEAN_EXPECT_LESS(validation, bullseye.grayThreshold(), 256u);
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
