/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestTimestamp.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

#include <cmath>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestTimestamp::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Timestamp test");
	Log::info() << " ";

	if (selector.shouldRun("resolution"))
	{
		testResult = testResolution(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("milliseconds"))
	{
		testResult = testMilliseconds(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("microseconds"))
	{
		testResult = testMicroseconds(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("nanoseconds"))
	{
		testResult = testNanoseconds(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("hastimepassed"))
	{
		testResult = testHasTimePassed(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestTimestamp, Resolution)
{
	EXPECT_TRUE(TestTimestamp::testResolution(GTEST_TEST_DURATION));
}

TEST(TestTimestamp, Milliseconds)
{
	EXPECT_TRUE(TestTimestamp::testMilliseconds(GTEST_TEST_DURATION));
}

TEST(TestTimestamp, Microseconds)
{
	EXPECT_TRUE(TestTimestamp::testMicroseconds(GTEST_TEST_DURATION));
}

TEST(TestTimestamp, Nanoseconds)
{
	EXPECT_TRUE(TestTimestamp::testNanoseconds(GTEST_TEST_DURATION));
}

TEST(TestTimestamp, HasTimePassed)
{
	EXPECT_TRUE(TestTimestamp::testHasTimePassed(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestTimestamp::testResolution(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Resolution test:";

	Validation validation;

	unsigned int differentTimestamps = 0u;

	const Timestamp startTimestamp(true);
	Timestamp oldTimestamp(startTimestamp);

	do
	{
		const Timestamp newTimestamp(true);

		if (newTimestamp != oldTimestamp)
		{
			++differentTimestamps;
			oldTimestamp = newTimestamp;
		}
	}
	while (startTimestamp + max(1.0, testDuration) > Timestamp(true)); // we need at least one second for this test

	const double duration = double(oldTimestamp - startTimestamp);

	ocean_assert(duration > 0.0);
	const unsigned int differentTimestampPerSecond = (unsigned int)(double(differentTimestamps) / duration + 0.5);

	Log::info() << "Precision: Ticks " << String::insertCharacter(String::toAString(differentTimestampPerSecond), ',', 3, false) << " per second";

	OCEAN_EXPECT_GREATER_EQUAL(validation, differentTimestampPerSecond, 20u);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestTimestamp::testMilliseconds(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Milliseconds test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		constexpr int64_t second_in_milliseconds = 1000ll;

		// random value in range [-1000, 1000] seconds (in milliseconds)
		const int64_t timestamp_in_milliseconds = int64_t(RandomI::random64(randomGenerator) % uint64_t(second_in_milliseconds * 2000ll)) - second_in_milliseconds * 1000ll;

		const double timestamp_in_seconds = double(timestamp_in_milliseconds) / double(second_in_milliseconds);

		const Timestamp timestamp(timestamp_in_seconds);

		const int64_t milliseconds = Timestamp::seconds2milliseconds(double(timestamp));

		OCEAN_EXPECT_EQUAL(validation, timestamp_in_milliseconds, milliseconds);

		const double test_seconds = Timestamp::milliseconds2seconds(milliseconds);

		constexpr double threshold = 1.0 / double(second_in_milliseconds);

		OCEAN_EXPECT_LESS_EQUAL(validation, std::fabs(timestamp_in_seconds - test_seconds), threshold);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestTimestamp::testMicroseconds(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Microseconds test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		constexpr int64_t second_in_milliseconds = 1000ll;
		constexpr int64_t second_in_microseconds = second_in_milliseconds * 1000ll;

		// random value in range [-100, 100] seconds (in microseconds)
		const int64_t timestamp_in_microseconds = int64_t(RandomI::random64(randomGenerator) % uint64_t(second_in_microseconds * 200ll)) - second_in_microseconds * 100ll;

		const double timestamp_in_seconds = double(timestamp_in_microseconds) / double(second_in_microseconds);

		const Timestamp timestamp(timestamp_in_seconds);

		const int64_t microseconds = Timestamp::seconds2microseconds(double(timestamp));

		OCEAN_EXPECT_EQUAL(validation, timestamp_in_microseconds, microseconds);

		const double test_seconds = Timestamp::microseconds2seconds(microseconds);

		constexpr double threshold = 1.0 / double(second_in_microseconds);

		OCEAN_EXPECT_LESS_EQUAL(validation, std::fabs(timestamp_in_seconds - test_seconds), threshold);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestTimestamp::testNanoseconds(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Nanoseconds test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		constexpr int64_t second_in_milliseconds = 1000ll;
		constexpr int64_t second_in_microseconds = second_in_milliseconds * 1000ll;
		constexpr int64_t second_in_nanoseconds = second_in_microseconds * 1000ll;

		// random value in range [-10, 10] seconds (in nanoseconds)
		const int64_t timestamp_in_nanoseconds = int64_t(RandomI::random64(randomGenerator) % uint64_t(second_in_nanoseconds * 20ll)) - second_in_nanoseconds * 10ll;

		const double timestamp_in_seconds = double(timestamp_in_nanoseconds) / double(second_in_nanoseconds);

		const Timestamp timestamp(timestamp_in_seconds);

		const int64_t nanoseconds = timestamp.nanoseconds();

		OCEAN_EXPECT_EQUAL(validation, timestamp_in_nanoseconds, nanoseconds);

		const double test_seconds = Timestamp::nanoseconds2seconds(nanoseconds);

		constexpr double threshold = 1.0 / double(second_in_nanoseconds);

		OCEAN_EXPECT_LESS_EQUAL(validation, std::fabs(timestamp_in_seconds - test_seconds), threshold);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestTimestamp::testHasTimePassed(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Has time passed test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp loopStartTimestamp(true);

	do
	{
		Timestamp startTimestamp;

		OCEAN_EXPECT_TRUE(validation, startTimestamp.hasTimePassed(double(RandomI::random(randomGenerator, 0, 1000)))); // an invalid start timestamp must always return true

		startTimestamp = Timestamp(double(RandomI::random(randomGenerator, -1000, 1000)));

		const Timestamp currentTimestamp = Timestamp(double(RandomI::random(randomGenerator, -1000, 1000)));

		const double seconds = double(RandomI::random(randomGenerator, 0, 100));

		const bool result = startTimestamp.hasTimePassed(seconds, currentTimestamp);

		if (currentTimestamp < startTimestamp)
		{
			OCEAN_EXPECT_FALSE(validation, result);
		}
		else
		{
			const bool expectedResult = currentTimestamp >= startTimestamp + seconds;

			OCEAN_EXPECT_EQUAL(validation, result, expectedResult);
		}
	}
	while (loopStartTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
