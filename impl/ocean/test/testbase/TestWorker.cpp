/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestWorker.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Processor.h"
#include "ocean/base/Timestamp.h"

#include <cmath>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestWorker::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Worker tests:   ---";
	Log::info() << " ";

	Worker worker;
	Log::info() << "The worker is composed of " << worker.threads() << " threads.";

	if (worker.threads() < 1)
	{
		Log::info() << "We do not have a multi-core platform and therefore skip this test.";
		return true;
	}

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testConstructor() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDelay(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStaticWorker(worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStaticWorkerSumOfSquares(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStaticWorkerSumOfSquareRoots(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAbortableFunction(worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSeparableAndAbortableFunction(worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Worker test succeeded.";
	}
	else
	{
		Log::info() << "Worker test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestWorker, Constructor)
{
	EXPECT_TRUE(TestWorker::testConstructor());
}

TEST(TestWorker, Delay)
{
	Worker worker;

	if (worker.threads() > 1)
	{
		EXPECT_TRUE(TestWorker::testDelay(GTEST_TEST_DURATION, worker));
	}
}

TEST(TestWorker, StaticWorker)
{
	Worker worker;

	if (worker.threads() > 1)
	{
		EXPECT_TRUE(TestWorker::testStaticWorker(worker));
	}
}

TEST(TestWorker, StaticWorkerSumOfSquares)
{
	Worker worker;

	if (worker.threads() > 1)
	{
		EXPECT_TRUE(TestWorker::testStaticWorkerSumOfSquares(GTEST_TEST_DURATION, worker));
	}
}

TEST(TestWorker, StaticWorkerSumOfSquareRoots)
{
	Worker worker;

	if (worker.threads() > 1)
	{
		EXPECT_TRUE(TestWorker::testStaticWorkerSumOfSquareRoots(GTEST_TEST_DURATION, worker));
	}
}

TEST(TestWorker, AbortableFunction)
{
	Worker worker;

	if (worker.threads() > 1)
	{
		EXPECT_TRUE(TestWorker::testAbortableFunction(worker));
	}
}

TEST(TestWorker, SeparableAndAbortableFunction)
{
	Worker worker;

	if (worker.threads() > 1)
	{
		EXPECT_TRUE(TestWorker::testSeparableAndAbortableFunction(worker));
	}
}

#endif // OCEAN_USE_GTEST

bool TestWorker::testConstructor()
{
	Log::info() << "Test constructor:";

	bool allSucceeded = true;

	{
		const Worker defaultWorker;

		const unsigned int expectedThreads = std::min(Processor::get().cores(), 16u);

		if (defaultWorker.threads() != expectedThreads)
		{
			allSucceeded = false;
		}
	}

	for (unsigned int threads = 1u; threads <= 64u; ++threads)
	{
		const Worker customWorker(threads, Worker::TYPE_CUSTOM);

		if (customWorker.threads() != threads)
		{
			allSucceeded = false;
		}
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

bool TestWorker::testDelay(const double testDuration, Worker& worker)
{
	ocean_assert(worker);

	Log::info() << "Test worker delay:";

	const Timestamp startTimestamp(true);

	uint64_t iterations = 0ull;

	double minimalFirstStartDelay = DBL_MAX;
	double maximalLastStartDelay = -DBL_MAX;

	double averageFirstStartDelay = 0.0;
	double averageLastStartDelay = 0.0;

	double minimalStopDelay = DBL_MAX;
	double maximalStopDelay = -DBL_MAX;
	double averageStopDelay = 0.0;

	do
	{
		std::vector<uint64_t> ticks(worker.threads(), 0ull);

		const uint64_t startTick = HighPerformanceTimer::ticks();
			worker.executeFunction(Worker::Function::createStatic(&TestWorker::staticWorkerFunctionDelay, ticks.data(), 0u, 0u), 0u, worker.threads(), 1u, 2u);
		const uint64_t stopTick = HighPerformanceTimer::ticks();

		uint64_t firstStartDelay = uint64_t(-1);
		uint64_t lastStartDelay = 0ull;

		for (size_t n = 0; n < ticks.size(); ++n)
		{
			firstStartDelay = min(firstStartDelay, ticks[n] - startTick);
			lastStartDelay = max(lastStartDelay, ticks[n] - startTick);
		}

		const double first = double(firstStartDelay) * double(1000 * 1000) / double(HighPerformanceTimer::precision());
		const double last = double(lastStartDelay) * double(1000 * 1000) / double(HighPerformanceTimer::precision());
		const double stop = double(stopTick - startTick) * double(1000 * 1000) / double(HighPerformanceTimer::precision());

		minimalFirstStartDelay = min(minimalFirstStartDelay, first);
		maximalLastStartDelay = max(maximalLastStartDelay, last);

		averageFirstStartDelay += first;
		averageLastStartDelay += last;

		minimalStopDelay = min(minimalStopDelay, stop);
		maximalStopDelay = max(maximalStopDelay, stop);
		averageStopDelay += stop;

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);

	Log::info() << "Minimal first start delay: " << minimalFirstStartDelay << "mys";
	Log::info() << "Average fist start delay: " << averageFirstStartDelay / double(iterations) << "mys";
	Log::info() << "Average last start delay: " << averageLastStartDelay / double(iterations) << "mys";
	Log::info() << "Maximal last start delay: " << maximalLastStartDelay << "mys";
	Log::info() << " ";
	Log::info() << "Minimal stop delay: " << minimalStopDelay << "mys";
	Log::info() << "Average stop delay: " << averageStopDelay / double(iterations) << "mys";
	Log::info() << "Maximal stop delay: " << maximalStopDelay << "mys";

	return true;
}

bool TestWorker::testStaticWorker(Worker& worker)
{
	ocean_assert(worker);

	Log::info() << "Test static worker function:";
	Log::info() << " ";

	Log::info() << "... with 1 elements";
	worker.executeFunction(Worker::Function::createStatic(&TestWorker::staticWorkerFunction, 0u, 0u), 0u, 1u, 0u, 1u);
	Log::info() << " ";

	Log::info() << "... with 2 elements";
	worker.executeFunction(Worker::Function::createStatic(&TestWorker::staticWorkerFunction, 0u, 0u), 0u, 2u, 0u, 1u);
	Log::info() << " ";

	Log::info() << "... with 9 elements";
	worker.executeFunction(Worker::Function::createStatic(&TestWorker::staticWorkerFunction, 0u, 0u), 0u, 9u, 0u, 1u);
	Log::info() << " ";

	Log::info() << "... with 9 elements and 6 minimum";
	worker.executeFunction(Worker::Function::createStatic(&TestWorker::staticWorkerFunction, 0u, 0u), 0u, 9u, 0u, 1u, 6u);
	Log::info() << " ";

	Log::info() << "... with 81 elements and 20 minimum";
	worker.executeFunction(Worker::Function::createStatic(&TestWorker::staticWorkerFunction, 0u, 0u), 0u, 81u, 0u, 1u, 20u);
	Log::info() << " ";

	Log::info() << "... with 100 elements";
	worker.executeFunction(Worker::Function::createStatic(&TestWorker::staticWorkerFunction, 0u, 0u), 0u, 100u, 0u, 1u);
	Log::info() << " ";

	Log::info() << "... with 100 elements and 35 minimum";
	worker.executeFunction(Worker::Function::createStatic(&TestWorker::staticWorkerFunction, 0u, 0u), 0u, 100u, 0u, 1u, 35u);
	Log::info() << " ";

	Log::info() << "... with 100 elements and 99 minimum";
	worker.executeFunction(Worker::Function::createStatic(&TestWorker::staticWorkerFunction, 0u, 0u), 0u, 100u, 0u, 1u, 99u);

	return true;
}

bool TestWorker::testStaticWorkerSumOfSquares(const double testDuration, Worker& worker)
{
	ocean_assert(worker);

	Log::info() << "Test performance of sum of squares function:";
	Log::info() << " ";

	const unsigned int numberValues = 1000u * 1000u;
	Indices32 values(numberValues);

	for (unsigned int n = 0u; n < numberValues; ++n)
	{
		values[n] = 2u;
	}

	staticWorkerFunctionSumOfSquares(values.data(), 0u, numberValues);

	HighPerformanceStatistic performance;
	Timestamp startTimestamp(true);

	do
	{
		performance.start();
			staticWorkerFunctionSumOfSquares(values.data(), 0u, numberValues);
		performance.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Single threaded performance: Best: " << performance.bestMseconds() << "ms, average: " << performance.averageMseconds() << "ms";

	HighPerformanceStatistic multicorePerformance;
	startTimestamp.toNow();

	do
	{
		multicorePerformance.start();
			worker.executeFunction(Worker::Function::createStatic(&TestWorker::staticWorkerFunctionSumOfSquares, values.data(), 0u, 0u), 0, numberValues, 1u, 2u);
		multicorePerformance.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Multi threaded performance: Best: " << multicorePerformance.bestMseconds() << "ms, average: " << multicorePerformance.averageMseconds() << "ms";
	Log::info() << "Multicore boost factor: Best: " << String::toAString(performance.best() / multicorePerformance.best(), 1u) << "x, average: " << String::toAString(performance.average() / multicorePerformance.average(), 1u) << "x";

	return true;
}

bool TestWorker::testStaticWorkerSumOfSquareRoots(const double testDuration, Worker& worker)
{
	ocean_assert(worker);

	Log::info() << "Test performance of sum of square roots function:";
	Log::info() << " ";

	unsigned int numberValues = 1000u * 1000u;
	std::vector<double> values(numberValues);

	for (unsigned int n = 0u; n < numberValues; ++n)
	{
		values[n] = 2u;
	}

	staticWorkerFunctionSumOfSquareRoots(values.data(), 0u, numberValues);

	HighPerformanceStatistic performance;
	Timestamp startTimestamp(true);

	do
	{
		performance.start();
			staticWorkerFunctionSumOfSquareRoots(values.data(), 0u, numberValues);
		performance.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Single threaded performance: Best: " << performance.bestMseconds() << "ms, average: " << performance.averageMseconds() << "ms";

	HighPerformanceStatistic multicorePerformance;
	startTimestamp.toNow();

	do
	{
		multicorePerformance.start();
			worker.executeFunction(Worker::Function::createStatic(&TestWorker::staticWorkerFunctionSumOfSquareRoots, values.data(), 0u, 0u), 0, numberValues, 1u, 2u);
		multicorePerformance.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Multi threaded performance: Best: " << multicorePerformance.bestMseconds() << "ms, average: " << multicorePerformance.averageMseconds() << "ms";
	Log::info() << "Multicore boost factor: Best: " << String::toAString(performance.best() / multicorePerformance.best(), 1u) << "x, average: " << String::toAString(performance.average() / multicorePerformance.average(), 1u) << "x";

	return true;
}

bool TestWorker::testAbortableFunction(Worker& worker)
{
	ocean_assert(worker);

	Log::info() << "Test validation of abortable function:";
	Log::info() << " ";

	double resultValue = 0.0;
	bool abortState = false;

	const Timestamp startTimestamp(true);
	Worker::AbortableFunction workerFunction(Worker::AbortableFunction::createStatic<double*, bool*>(&TestWorker::staticWorkerFunctionAbortable, &resultValue, &abortState));
	worker.executeAbortableFunction(workerFunction, 1u);
	const Timestamp stopTimestamp(true);

	// Normally we should expect a delay ~1 milli seconds.
	// However, Gtests are executed in parallel (e.g., 16 threads per worker * 10 tests).
	// It seems that the test servers have too much load,
	// so that we simply have to select an outstanding high test threshold: 0.5 seconds.

#ifdef OCEAN_USE_GTEST
	const double threshold = 0.5;
#else
	const double threshold = 0.1; // already quite generous
#endif

	const double delay = fabs(double(stopTimestamp - startTimestamp) - resultValue);

	const bool result = delay < threshold;

	if (result)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return result;
}

bool TestWorker::testSeparableAndAbortableFunction(Worker& worker)
{
	ocean_assert(worker);

	Log::info() << "Test validation of separable and abortable function:";
	Log::info() << " ";

	double resultValue = 0.0;
	bool abortState = false;

	const Timestamp startTimestamp(true);
	worker.executeSeparableAndAbortableFunction(Worker::AbortableFunction::createStatic(&TestWorker::staticWorkerFunctionSeparableAndAbortable, &resultValue, 0u, 0u, &abortState), 0u, 8u, 1u, 2u, 3u, 2u);
	const Timestamp stopTimestamp(true);

	// Normally we should expect a delay ~1 milliseconds.
	// However, Gtests are executed in parallel (e.g., 16 threads per worker * 10 tests).
	// It seems that the test servers have too much load,
	// so that we simply have to select an outstanding high test threshold ~0.5 seconds.

#ifdef OCEAN_USE_GTEST
	const double threshold = 0.5;
#else
	const double threshold = 0.1; // already quite generous
#endif

	const double delay = fabs(double(stopTimestamp - startTimestamp) - resultValue);

	const bool result = delay < threshold;

	if (result)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return result;
}

void TestWorker::staticWorkerFunctionDelay(uint64_t* time, const unsigned int first, const unsigned int size)
{
	ocean_assert(time);
	ocean_assert_and_suppress_unused(size == 1u, size);

	time[first] = HighPerformanceTimer::ticks();
}

void TestWorker::staticWorkerFunction(const unsigned int first, const unsigned int size)
{
	Log::info() << "Static worker function call: [" << first << ", " << first + size - 1u << "]: " << size << " elements";
}

void TestWorker::staticWorkerFunctionSumOfSquares(unsigned int* values, const unsigned int first, const unsigned int size)
{
	ocean_assert(values);

	const unsigned int* v = values + first;
	const unsigned int* const vEnd = v + size;

	unsigned int result = 0;

	while (v != vEnd)
	{
		result += *v * *v;
		++v;
	}

	*(values + first) = result;
}

void TestWorker::staticWorkerFunctionSumOfSquareRoots(double* values, const unsigned int first, const unsigned int size)
{
	ocean_assert(values);

	const double* v = values + first;
	const double* const vEnd = v + size;

	double result = 0.0;

	while (v != vEnd)
	{
		result += sqrt(*v);
		++v;
	}

	*(values + first) = result;
}

bool TestWorker::staticWorkerFunctionAbortable(double* result, bool* abort)
{
	ocean_assert(result && abort);

	const double timeout = 2.0 + double(rand()) * 10.0 / double(RAND_MAX);

	Log::info() << "Waiting " << timeout << "s";

	const Timestamp startTimestamp(true);
	while (*abort == false)
	{
		if (startTimestamp + timeout < Timestamp(true))
		{
			*result = timeout;
			Log::info() << "Finished waiting for " << timeout << "s";
			return true;
		}
	}

	Log::info() << "Aborted waiting for " << timeout << "s";

	return false;
}

bool TestWorker::staticWorkerFunctionSeparableAndAbortable(double* result, const unsigned int first, const unsigned int size, bool* abort)
{
	ocean_assert(result && abort);

	const double timeout = 2.0 + double(rand()) * 10.0 / double(RAND_MAX);

	Log::info() << "First: " << first << ", size: " << size;
	Log::info() << "Waiting " << timeout << "s";

	const Timestamp startTimestamp(true);
	while (*abort == false)
	{
		if (startTimestamp + timeout < Timestamp(true))
		{
			*result = timeout;
			Log::info() << "Finished waiting for " << timeout << "s";
			return true;
		}
	}

	Log::info() << "Aborted waiting for " << timeout << "s";

	return false;
}

}

}

}
