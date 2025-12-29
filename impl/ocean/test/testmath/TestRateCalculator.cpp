/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestRateCalculator.h"

#include "ocean/test/TestResult.h"

#include "ocean/math/Random.h"
#include "ocean/math/RateCalculator.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestRateCalculator::test(const double testDuration, Worker& /*worker*/, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("RateCalculator test");

	Log::info() << " ";

	if (selector.shouldRun("rate"))
	{
		testResult = testRate(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestRateCalculator, Rate)
{
	EXPECT_TRUE(TestRateCalculator::testRate(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestRateCalculator::testRate(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Rate test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const double frequency = RandomD::scalar(randomGenerator, 10.0, 100.0);
		const double window = RandomD::scalar(randomGenerator, 1.0, 5.0);

		const Timestamp referenceTimestamp(RandomD::scalar(randomGenerator, -1000.0, 1000.0));

		RateCalculator rateCalculator(window);

		TimestampPairs timestampPairs;

		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const Timestamp currentTimestamp(referenceTimestamp + double(n) / frequency + RandomD::scalar(randomGenerator, -0.0001, 0.0001));
			const double quantity = RandomD::scalar(randomGenerator, 0.1, 10.0);

			rateCalculator.addOccurence(currentTimestamp, quantity);

			timestampPairs.emplace_back(currentTimestamp, quantity);

			if (n >= 50u)
			{
				const Timestamp queryTimestamp = Timestamp(currentTimestamp + RandomD::scalar(-0.0001, 0.0001));

				const double rate = rateCalculator.rate(queryTimestamp);

				double sumQuantity = 0.0;
				for (const TimestampPair& timestampPair : timestampPairs)
				{
					if (timestampPair.first > queryTimestamp - rateCalculator.window() && timestampPair.first <= queryTimestamp)
					{
						sumQuantity += timestampPair.second;
					}
				}

				const double expectedRate = sumQuantity / rateCalculator.window();

				if (!NumericD::isEqual(expectedRate, rate, 0.1))
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

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

}

}

}
