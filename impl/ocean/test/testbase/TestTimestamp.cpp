/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestTimestamp.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include <cmath>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestTimestamp::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Timestamp test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testResolution(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMilliseconds(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMicroseconds(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNanoseconds(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHasTimePassed(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Timestamp test succeeded.";
	}
	else
	{
		Log::info() << "Timestamp test FAILED!";
	}

	return allSucceeded;
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

	const bool succeeded = differentTimestampPerSecond >= 20u;

	if (succeeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return succeeded;
}

bool TestTimestamp::testMilliseconds(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Milliseconds test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		constexpr int64_t second_in_milliseconds = 1000ll;

		// random value in range [-1000, 1000] seconds (in milliseconds)
		const int64_t timestamp_in_milliseconds = int64_t(RandomI::random64() % uint64_t(second_in_milliseconds * 2000ll)) - second_in_milliseconds * 1000ll;

		const double timestamp_in_seconds = double(timestamp_in_milliseconds) / double(second_in_milliseconds);

		const Timestamp timestamp(timestamp_in_seconds);

		const int64_t milliseconds = Timestamp::seconds2milliseconds(double(timestamp));

		if (timestamp_in_milliseconds != milliseconds)
		{
			allSucceeded = false;
		}

		const double test_seconds = Timestamp::milliseconds2seconds(milliseconds);

		constexpr double threshold = 1.0 / double(second_in_milliseconds);

		if (std::fabs(timestamp_in_seconds - test_seconds) > threshold)
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestTimestamp::testMicroseconds(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Microseconds test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		constexpr int64_t second_in_milliseconds = 1000ll;
		constexpr int64_t second_in_microseconds = second_in_milliseconds * 1000ll;

		// random value in range [-100, 100] seconds (in microseconds)
		const int64_t timestamp_in_microseconds = int64_t(RandomI::random64() % uint64_t(second_in_microseconds * 200ll)) - second_in_microseconds * 100ll;

		const double timestamp_in_seconds = double(timestamp_in_microseconds) / double(second_in_microseconds);

		const Timestamp timestamp(timestamp_in_seconds);

		const int64_t microseconds = Timestamp::seconds2microseconds(double(timestamp));

		if (timestamp_in_microseconds != microseconds)
		{
			allSucceeded = false;
		}

		const double test_seconds = Timestamp::microseconds2seconds(microseconds);

		constexpr double threshold = 1.0 / double(second_in_microseconds);

		if (std::fabs(timestamp_in_seconds - test_seconds) > threshold)
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestTimestamp::testNanoseconds(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Nanoseconds test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		constexpr int64_t second_in_milliseconds = 1000ll;
		constexpr int64_t second_in_microseconds = second_in_milliseconds * 1000ll;
		constexpr int64_t second_in_nanoseconds = second_in_microseconds * 1000ll;

		// random value in range [-10, 10] seconds (in nanoseconds)
		const int64_t timestamp_in_nanoseconds = int64_t(RandomI::random64() % uint64_t(second_in_nanoseconds * 20ll)) - second_in_nanoseconds * 10ll;

		const double timestamp_in_seconds = double(timestamp_in_nanoseconds) / double(second_in_nanoseconds);

		const Timestamp timestamp(timestamp_in_seconds);

		const int64_t nanoseconds = timestamp.nanoseconds();

		if (timestamp_in_nanoseconds != nanoseconds)
		{
			allSucceeded = false;
		}

		const double test_seconds = Timestamp::nanoseconds2seconds(nanoseconds);

		constexpr double threshold = 1.0 / double(second_in_nanoseconds);

		if (std::fabs(timestamp_in_seconds - test_seconds) > threshold)
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestTimestamp::testHasTimePassed(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Has time passed test:";

	bool allSucceeded = true;

	const Timestamp loopStartTimestamp(true);

	do
	{
		Timestamp startTimestamp;

		if (!startTimestamp.hasTimePassed(double(RandomI::random(0, 1000)))) // an invalid start timestamp must always return true
		{
			allSucceeded = false;
		}

		startTimestamp = Timestamp(double(RandomI::random(-1000, 1000)));

		const Timestamp currentTimestamp = Timestamp(double(RandomI::random(-1000, 1000)));

		const double seconds = double(RandomI::random(0, 100));

		const bool result = startTimestamp.hasTimePassed(seconds, currentTimestamp);

		if (currentTimestamp < startTimestamp)
		{
			if (result)
			{
				allSucceeded = false;
			}
		}
		else
		{
			const bool expectedResult = currentTimestamp >= startTimestamp + seconds;

			if (result != expectedResult)
			{
				allSucceeded = false;
			}
		}
	}
	while (loopStartTimestamp + testDuration > Timestamp(true));

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
