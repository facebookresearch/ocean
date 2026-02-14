/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestHighPerformanceTimer.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Thread.h"

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

bool TestHighPerformanceTimer::test(const TestSelector& selector)
{
	TestResult testResult("Test high performance timer");
	Log::info() << " ";

	if (selector.shouldRun("precision"))
	{
		testResult = testPrecision();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

bool TestHighPerformanceStatistic::test(const TestSelector& selector)
{
	TestResult testResult("Test high performance statistic");
	Log::info() << " ";

	if (selector.shouldRun("reset"))
	{
		testResult = testReset();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestHighPerformanceTimer, Precision)
{
	EXPECT_TRUE(TestHighPerformanceTimer::testPrecision());
}

TEST(TestHighPerformanceStatistic, Rest)
{
	EXPECT_TRUE(TestHighPerformanceStatistic::testReset());
}

#endif // OCEAN_USE_GTEST

bool TestHighPerformanceTimer::testPrecision()
{
	Log::info() << "Test Precision:";
	Log::info() << " ";

	Log::info() << "The timer has " << String::insertCharacter(String::toAString(HighPerformanceTimer::precision()), ',', 3, false) << " ticks per second";

	Validation validation;

	// we determine how long a thread actually sleeps if 1ms is intended (some platforms sleep significantly longer e.g., 10ms)

	Log::info() << "Current Tick value: " << HighPerformanceTimer::ticks();
	Thread::sleep(1);
	Log::info() << "Tick value after sleeping 1ms: " << HighPerformanceTimer::ticks();

	HighPerformanceTimer timer;
	Thread::sleep(1);
	Log::info() << "Sleeping 1ms, exact: " << timer.mseconds() << "ms";


	// now we measure the precision of the default Unix timestamp compared to the high performance implementation

	{
		const Timestamp startTimestamp(true);

		timer.start();
		while(timer.seconds() < 2.0)
		{
			// nothing to do here
		}

		const Timestamp stopTimestamp(true);

		Log::info() << "Difference high performance timer to timestamp: " << (fabs(2.0 - double(stopTimestamp - startTimestamp))) * 100.0 / 2.0 << "%";

		OCEAN_EXPECT_LESS_EQUAL(validation, fabs(2.0 - double(stopTimestamp - startTimestamp)), 0.2);
	}

	{
		HighPerformanceStatistic performance;

		for (unsigned int n = 0u; n < 10u; ++n)
		{
			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

			const Timestamp startTimestamp(true);
			while (Timestamp(true) < startTimestamp + 1.5)
			{
				// nothing to do here
			}
		}

		Log::info() << "Performance statistic for 10 iterations for 1.5 seconds: " << performance.average() << "s";
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestHighPerformanceStatistic::testReset()
{
	Log::info() << "Test Reset:";
	Log::info() << " ";

	Validation validation;

	HighPerformanceStatistic highPerformanceStatistic;

	OCEAN_EXPECT_EQUAL(validation, highPerformanceStatistic.measurements(), size_t(0));

	highPerformanceStatistic.start();
	highPerformanceStatistic.stop();

	OCEAN_EXPECT_EQUAL(validation, highPerformanceStatistic.measurements(), size_t(1));

	highPerformanceStatistic.reset();

	OCEAN_EXPECT_EQUAL(validation, highPerformanceStatistic.measurements(), size_t(0));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
