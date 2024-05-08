/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestWorkerPool.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Processor.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestWorkerPool::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   WorkerPool tests:   ---";
	Log::info() << " ";

	const unsigned int cores = Processor::get().cores();

	if (cores <= 1u)
	{
		Log::info() << "We do not have a multi-core platform and therefore skip this test.";
		return true;
	}

	Log::info() << "Capacity of the worker pool: " << WorkerPool::get().capacity();

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testScopedWorker(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "WorkerPool test succeeded.";
	}
	else
	{
		Log::info() << "WorkerPool test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestWorkerPool, ScopedWorker)
{
	if (Processor::get().cores() > 1)
	{
		EXPECT_TRUE(TestWorkerPool::testScopedWorker(GTEST_TEST_DURATION));
	}
}

TEST(TestWorkerPool, SetCapacity)
{
	// actually we do not want to increase the capacity for this test (as we cannot reduce the capacity anymore),
	// so that we just check whether the function does not break

	EXPECT_TRUE(WorkerPool::get().setCapacity(WorkerPool::get().capacity()));
}

#endif // OCEAN_USE_GTEST

bool TestWorkerPool::testScopedWorker(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test ScopedWorker acquiring:";

	bool allSucceeded = true;

	HighPerformanceStatistic performanceFirst, performanceSecond, performanceThird;
	HighPerformanceStatistic performanceCreateDestroyFirst, performanceCreateDestroyTwo, performanceCreateDestroyThree;

	constexpr unsigned int constIterations = 10u;

	const size_t capacity = WorkerPool::get().capacity();

	if (capacity != 2)
	{
		allSucceeded = false;
	}

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			performanceFirst.start();
				const WorkerPool::ScopedWorker firstScopedWorker(WorkerPool::get().scopedWorker());
			performanceFirst.stop();

			performanceSecond.start();
				const WorkerPool::ScopedWorker secondScopedWorker(WorkerPool::get().scopedWorker());
			performanceSecond.stop();

			performanceThird.start();
				const WorkerPool::ScopedWorker thirdScopedWorker(WorkerPool::get().scopedWorker());
			performanceThird.stop();

			if (firstScopedWorker() == nullptr)
			{
				allSucceeded = false;
			}
			if (secondScopedWorker() == nullptr)
			{
				allSucceeded = false;
			}
			if (thirdScopedWorker() != nullptr)
			{
				allSucceeded = false;
			}
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceCreateDestroyFirst);

			for (unsigned int n = 0u; n < constIterations; ++n)
			{
				const WorkerPool::ScopedWorker firstScopedWorker(WorkerPool::get().scopedWorker());
			}
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceCreateDestroyTwo);

			for (unsigned int n = 0u; n < constIterations; ++n)
			{
				const WorkerPool::ScopedWorker firstScopedWorker(WorkerPool::get().scopedWorker());
				const WorkerPool::ScopedWorker secondScopedWorker(WorkerPool::get().scopedWorker());
			}
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedStatistic(performanceCreateDestroyThree);

			for (unsigned int n = 0u; n < constIterations; ++n)
			{
				const WorkerPool::ScopedWorker firstScopedWorker(WorkerPool::get().scopedWorker());
				const WorkerPool::ScopedWorker secondScopedWorker(WorkerPool::get().scopedWorker());
				const WorkerPool::ScopedWorker thirdScopedWorker(WorkerPool::get().scopedWorker());
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(constIterations != 0u);

	Log::info() << "Performance acquire first object: " << performanceFirst.averageMseconds() * 1000.0 << "mys";
	Log::info() << "Performance acquire second object: " << performanceSecond.averageMseconds() * 1000.0 << "mys";
	Log::info() << "Performance acquire third object: " << performanceThird.averageMseconds() * 1000.0 << "mys";
	Log::info() << "Performance acquire and surrender first object: " << performanceCreateDestroyFirst.averageMseconds() * 1000.0 / double(constIterations) << "mys";
	Log::info() << "Performance acquire and surrender two objects: " << performanceCreateDestroyTwo.averageMseconds() * 1000.0 / double(constIterations) << "mys";
	Log::info() << "Performance acquire and surrender three objects: " << performanceCreateDestroyThree.averageMseconds() * 1000.0 / double(constIterations) << "mys";

	if (WorkerPool::get().capacity() != 2)
	{
		allSucceeded = false;
	}

	return allSucceeded;
}

}

}

}
