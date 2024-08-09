/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestThread.h"

#include "ocean/base/RandomI.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestThread::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Thread tests:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testWaitForValueWithoutLock(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testWaitForValueWithLock(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Thread test succeeded.";
	}
	else
	{
		Log::info() << "Thread test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(Testthread, WaitForValueWithoutLock)
{
	EXPECT_TRUE(TestThread::testWaitForValueWithoutLock(GTEST_TEST_DURATION));
}

TEST(Testthread, WaitForValueWithLock)
{
	EXPECT_TRUE(TestThread::testWaitForValueWithLock(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestThread::testWaitForValueWithoutLock(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing waitForValue() without lock:";

	constexpr double timeoutThreshold = 0.05;

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// testing bool

			bool object = RandomI::boolean(randomGenerator);
			const bool expectedValue = RandomI::boolean(randomGenerator);

			const bool objectIsEqualExpectedValue = object == expectedValue;

			double timeout = double(RandomI::random(randomGenerator, 0, 100)) * 0.01;

			if (objectIsEqualExpectedValue && RandomI::boolean(randomGenerator))
			{
				timeout = -1.0;
			}

			const Timestamp preTimestamp(true);

				const bool result = Thread::waitForValue(object, expectedValue, timeout);

			const Timestamp postTimestamp(true);

			OCEAN_EXPECT_EQUAL(validation, result, objectIsEqualExpectedValue);

			ocean_assert(postTimestamp >= preTimestamp);
			const double duration = double(postTimestamp - preTimestamp);

			const double expectedDuration = objectIsEqualExpectedValue ? 0.0 : timeout;

			const double durationError = std::abs(duration - expectedDuration);

			OCEAN_EXPECT_LESS_EQUAL(validation, durationError, timeoutThreshold);
		}

		{
			// testing string

			std::string object = std::string(1, char(RandomI::random(randomGenerator, 1u, 255u)));
			const std::string expectedValue = std::string(1, char(RandomI::random(randomGenerator, 1u, 255u)));

			const bool objectIsEqualExpectedValue = object == expectedValue;

			double timeout = double(RandomI::random(randomGenerator, 0, 100)) * 0.01;

			if (objectIsEqualExpectedValue && RandomI::boolean(randomGenerator))
			{
				timeout = -1.0;
			}

			const Timestamp preTimestamp(true);

				const bool result = Thread::waitForValue(object, expectedValue, timeout);

			const Timestamp postTimestamp(true);

			OCEAN_EXPECT_EQUAL(validation, result, objectIsEqualExpectedValue);

			ocean_assert(postTimestamp >= preTimestamp);
			const double duration = double(postTimestamp - preTimestamp);

			const double expectedDuration = objectIsEqualExpectedValue ? 0.0 : timeout;

			const double durationError = std::abs(duration - expectedDuration);

			OCEAN_EXPECT_LESS_EQUAL(validation, durationError, timeoutThreshold);
		}

		{
			// testing atomic int

			std::atomic<int> object = RandomI::random(randomGenerator, 0, 10);
			const int expectedValue = RandomI::random(randomGenerator, 0, 10);

			const bool objectIsEqualExpectedValue = object == expectedValue;

			double timeout = double(RandomI::random(randomGenerator, 0, 100)) * 0.01;

			if (objectIsEqualExpectedValue && RandomI::boolean(randomGenerator))
			{
				timeout = -1.0;
			}

			const Timestamp preTimestamp(true);

				const bool result = Thread::waitForValue(object, expectedValue, timeout);

			const Timestamp postTimestamp(true);

			OCEAN_EXPECT_EQUAL(validation, result, objectIsEqualExpectedValue);

			ocean_assert(postTimestamp >= preTimestamp);
			const double duration = double(postTimestamp - preTimestamp);

			const double expectedDuration = objectIsEqualExpectedValue ? 0.0 : timeout;

			const double durationError = std::abs(duration - expectedDuration);

			OCEAN_EXPECT_LESS_EQUAL(validation, durationError, timeoutThreshold);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestThread::testWaitForValueWithLock(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing waitForValue() with lock:";

	constexpr double timeoutThreshold = 0.05;

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// testing bool

			bool object = RandomI::boolean(randomGenerator);
			const bool expectedValue = RandomI::boolean(randomGenerator);

			const bool objectIsEqualExpectedValue = object == expectedValue;

			const double delay = double(RandomI::random(randomGenerator, 0, 100)) * 0.01;
			double timeout = double(RandomI::random(randomGenerator, 0, 100)) * 0.01;

			if (RandomI::boolean(randomGenerator))
			{
				timeout = -1.0;
			}

			Lock lock;

			DelayedValueSetter<bool, bool> delayedValueSetter(object, expectedValue, delay, lock);

			const Timestamp preTimestamp(true);

				TemporaryScopedLock temporaryScopedLock(lock);
				const bool result = Thread::waitForValue(object, expectedValue, temporaryScopedLock, timeout);

			const Timestamp postTimestamp(true);

			const bool wasSet = delayedValueSetter.stop();

			const bool timeoutLongerThanDelay = timeout < 0.0 || timeout - delay >= 0.1;
			const bool timeoutShorterThanDelay = timeout >= 0.0 && delay - timeout >= 0.1;

			ocean_assert(postTimestamp >= preTimestamp);
			const double duration = double(postTimestamp - preTimestamp);

			double expectedDuration = std::min(timeout, delay);

			if (objectIsEqualExpectedValue)
			{
				expectedDuration = 0.0;
			}
			else if (timeout < 0.0)
			{
				expectedDuration = delay;
			}

			const double durationError = std::abs(duration - expectedDuration);

			if (timeoutLongerThanDelay)
			{
				OCEAN_EXPECT_TRUE(validation, result);

				OCEAN_EXPECT_LESS_EQUAL(validation, durationError, timeoutThreshold);
			}

			if (timeoutShorterThanDelay)
			{
				if (!wasSet)
				{
					OCEAN_EXPECT_EQUAL(validation, result, objectIsEqualExpectedValue);
				}

				OCEAN_EXPECT_LESS_EQUAL(validation, durationError, timeoutThreshold);
			}

			if (objectIsEqualExpectedValue)
			{
				OCEAN_EXPECT_TRUE(validation, result);
			}

			Thread::sleep(10u);
		}

		{
			// testing string

			std::string object = std::string(1, char(RandomI::random(randomGenerator, 1u, 255u)));
			const std::string expectedValue = std::string(1, char(RandomI::random(randomGenerator, 1u, 255u)));

			const bool objectIsEqualExpectedValue = object == expectedValue;

			const double delay = double(RandomI::random(randomGenerator, 0, 100)) * 0.01;
			double timeout = double(RandomI::random(randomGenerator, 0, 100)) * 0.01;

			if (RandomI::boolean(randomGenerator))
			{
				timeout = -1.0;
			}

			Lock lock;

			DelayedValueSetter<std::string, std::string> delayedValueSetter(object, expectedValue, delay, lock);

			const Timestamp preTimestamp(true);

				TemporaryScopedLock temporaryScopedLock(lock);
				const bool result = Thread::waitForValue(object, expectedValue, temporaryScopedLock, timeout);

			const Timestamp postTimestamp(true);

			const bool wasSet = delayedValueSetter.stop();

			const bool timeoutLongerThanDelay = timeout < 0.0 || timeout - delay >= 0.1;
			const bool timeoutShorterThanDelay = timeout >= 0.0 && delay - timeout >= 0.1;

			ocean_assert(postTimestamp >= preTimestamp);
			const double duration = double(postTimestamp - preTimestamp);

			double expectedDuration = std::min(timeout, delay);

			if (objectIsEqualExpectedValue)
			{
				expectedDuration = 0.0;
			}
			else if (timeout < 0.0)
			{
				expectedDuration = delay;
			}

			const double durationError = std::abs(duration - expectedDuration);

			if (timeoutLongerThanDelay)
			{
				OCEAN_EXPECT_TRUE(validation, result);

				OCEAN_EXPECT_LESS_EQUAL(validation, durationError, timeoutThreshold);
			}

			if (timeoutShorterThanDelay)
			{
				if (!wasSet)
				{
					OCEAN_EXPECT_EQUAL(validation, result, objectIsEqualExpectedValue);
				}

				OCEAN_EXPECT_LESS_EQUAL(validation, durationError, timeoutThreshold);
			}

			if (objectIsEqualExpectedValue)
			{
				OCEAN_EXPECT_TRUE(validation, result);
			}

			Thread::sleep(10u);
		}

		{
			// testing atomic int

			std::atomic<int> object = RandomI::random(randomGenerator, 0, 10);
			const int expectedValue = RandomI::random(randomGenerator, 0, 10);

			const bool objectIsEqualExpectedValue = object == expectedValue;

			const double delay = double(RandomI::random(randomGenerator, 0, 100)) * 0.01;
			double timeout = double(RandomI::random(randomGenerator, 0, 100)) * 0.01;

			if (RandomI::boolean(randomGenerator))
			{
				timeout = -1.0;
			}

			Lock lock;

			DelayedValueSetter<std::atomic<int>, int> delayedValueSetter(object, expectedValue, delay, lock);

			const Timestamp preTimestamp(true);

				TemporaryScopedLock temporaryScopedLock(lock);
				const bool result = Thread::waitForValue(object, expectedValue, temporaryScopedLock, timeout);

			const Timestamp postTimestamp(true);

			const bool wasSet = delayedValueSetter.stop();

			const bool timeoutLongerThanDelay = timeout < 0.0 || timeout - delay >= 0.1;
			const bool timeoutShorterThanDelay = timeout >= 0.0 && delay - timeout >= 0.1;

			ocean_assert(postTimestamp >= preTimestamp);
			const double duration = double(postTimestamp - preTimestamp);

			double expectedDuration = std::min(timeout, delay);

			if (objectIsEqualExpectedValue)
			{
				expectedDuration = 0.0;
			}
			else if (timeout < 0.0)
			{
				expectedDuration = delay;
			}

			const double durationError = std::abs(duration - expectedDuration);

			if (timeoutLongerThanDelay)
			{
				OCEAN_EXPECT_TRUE(validation, result);

				OCEAN_EXPECT_LESS_EQUAL(validation, durationError, timeoutThreshold);
			}

			if (timeoutShorterThanDelay)
			{
				if (!wasSet)
				{
					OCEAN_EXPECT_EQUAL(validation, result, objectIsEqualExpectedValue);
				}

				OCEAN_EXPECT_LESS_EQUAL(validation, durationError, timeoutThreshold);
			}

			if (objectIsEqualExpectedValue)
			{
				OCEAN_EXPECT_TRUE(validation, result);
			}

			Thread::sleep(10u);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
