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

#include <cmath>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestHighPerformanceTimer::test()
{
	Log::info() << "---   Test high performance timer:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testPrecision() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "High performance timer test succeeded.";
	}
	else
	{
		Log::info() << "High performance timer test FAILED!";
	}

	return allSucceeded;
}

bool TestHighPerformanceStatistic::test()
{
	Log::info() << "---   Test high performance statistic:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testReset() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "High performance statistic test succeeded.";
	}
	else
	{
		Log::info() << "High performance statistic test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;

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
		while(timer.seconds() < 2.0);

		const Timestamp stopTimestamp(true);

		Log::info() << "Difference high performance timer to timestamp: " << (fabs(2.0 - double(stopTimestamp - startTimestamp))) * 100.0 / 2.0 << "%";

		if (fabs(2.0 - double(stopTimestamp - startTimestamp)) > 0.2)
		{
			allSucceeded = false;
		}
	}

	{
		HighPerformanceStatistic performance;

		for (unsigned int n = 0u; n < 10u; ++n)
		{
			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

			const Timestamp startTimestamp(true);
			while (Timestamp(true) < startTimestamp + 1.5);
		}

		Log::info() << "Performance statistic for 10 iterations for 1.5 seconds: " << performance.average() << "s";
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

bool TestHighPerformanceStatistic::testReset()
{
	Log::info() << "Test Reset:";
	Log::info() << " ";

	bool allSucceeded = true;

	HighPerformanceStatistic highPerformanceStatistic;

	if (highPerformanceStatistic.measurements() != 0u)
	{
		allSucceeded = false;
	}

	highPerformanceStatistic.start();
	highPerformanceStatistic.stop();

	if (highPerformanceStatistic.measurements() != 1u)
	{
		allSucceeded = false;
	}

	highPerformanceStatistic.reset();

	if (highPerformanceStatistic.measurements() != 0u)
	{
		allSucceeded = false;
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
