/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestSignal.h"

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

TestSignal::SignalThread::SignalThread(const HighPerformanceTimer& timer, const double timeout, Signal& signal) :
	timer_(timer),
	timeout_(timeout),
	signal_(signal)
{
	ocean_assert(timeout_ > 0.0);
}

void TestSignal::SignalThread::threadRun()
{
	setThreadPriority(Thread::PRIORTY_HIGH);

	while (timer_.seconds() < timeout_)
	{
		Thread::sleep(0u);
	}

	signal_.pulse();
}

TestSignal::AsyncFunctionThread::AsyncFunctionThread()
{
	startThread();
}

TestSignal::AsyncFunctionThread::~AsyncFunctionThread()
{
	// first we tell the thread function that we want to stop the tread
	stopThread();

	// then, we release the signal for starting the async thread
	signalAsyncStart_.release();

	// now, we wait until the thread has actually stopped
	joinThread();

	// finally, we kill the thread if necessary
	stopThreadExplicitly();
}

void TestSignal::AsyncFunctionThread::startAsyncFunction()
{
	// we simply pulse the signal for the start of the async function
	signalAsyncStart_.pulse();
}

void TestSignal::AsyncFunctionThread::threadRun()
{
	RandomI::initialize();

	// we simply wait for a random time
	const unsigned int sleepTime = RandomI::random(20u);
	if (sleepTime != 0u)
	{
		Thread::sleep(sleepTime);
	}

	while (shouldThreadStop() == false)
	{
		// we wait until the async function is intended to be executed
		signalAsyncStart_.wait();

		// we ensure that the thread is still expected to be running
		if (shouldThreadStop() == false)
		{
			asyncFunction();
		}

		signalAsyncStopped_.pulse();
	}
}

void TestSignal::AsyncFunctionThread::asyncFunction()
{
	// we simply wait for a random time
	const unsigned int sleepTime = RandomI::random(20u);
	if (sleepTime != 0u)
	{
		Thread::sleep(sleepTime);
	}
}

bool TestSignal::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Signal test");
	Log::info() << " ";

	if (selector.shouldRun("signalbasics"))
	{
		testResult = testSignalBasics();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("singlesignalstandard"))
	{
		testResult = testSingleSignalStandard();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("singlesignaltimeout"))
	{
		testResult = testSingleSignalTimeout();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("singlesignalloop"))
	{
		testResult = testSingleSignalLoop();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("asyncfunction"))
	{
		testResult = testAsyncFunction(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("multiplesignals"))
	{
		testResult = testMultipleSignals();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("subsetsignalsstandard"))
	{
		testResult = testSubsetSignalsStandard();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("subsetsignalstimeout"))
	{
		testResult = testSubsetSignalsTimeout();

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestSignal, SignalBasics)
{
	EXPECT_TRUE(TestSignal::testSignalBasics());
}

TEST(TestSignal, SingleSignalStandard)
{
	EXPECT_TRUE(TestSignal::testSingleSignalStandard());
}

TEST(TestSignal, SingleSignalTimeout)
{
	EXPECT_TRUE(TestSignal::testSingleSignalTimeout());
}

TEST(TestSignal, SingleSignalLoop)
{
	EXPECT_TRUE(TestSignal::testSingleSignalLoop());
}

TEST(TestSignal, AsyncFunction)
{
	EXPECT_TRUE(TestSignal::testAsyncFunction(GTEST_TEST_DURATION));
}

TEST(TestSignal, MultipleSignals)
{
	EXPECT_TRUE(TestSignal::testMultipleSignals());
}

TEST(TestSignal, SubsetSignalsStandard)
{
	EXPECT_TRUE(TestSignal::testSubsetSignalsStandard());
}

TEST(TestSignal, SubsetSignalsTimeout)
{
	EXPECT_TRUE(TestSignal::testSubsetSignalsTimeout());
}

#endif // OCEAN_USE_GTEST

bool TestSignal::testSignalBasics()
{
	Log::info() << "Test signal basics:";
	Log::info() << " ";

	Validation validation;

	Log::info() << "...resizing signals";
	Signals signals;
	signals.setSize(2u);

	OCEAN_EXPECT_EQUAL(validation, signals.size(), 2u);

	Log::info() << "...releasing signals";

	signals[0].release();
	signals[1].release();

	Log::info() << "...simple signal";

	Signal signal;
	signal.pulse();
	signal.wait();

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSignal::testSingleSignalStandard()
{
	Log::info() << "Test single signal (standard):";
	Log::info() << " ";

	Validation validation;

	Signal signal;
	HighPerformanceTimer timer;

	constexpr double interval = 2.0;

	SignalThread thread(timer, interval, signal);

	timer.start();
	thread.startThread();

	const Timestamp startTimestamp(true);
		signal.wait();
	const Timestamp stopTimestamp(true);

	const double actualInterval = double(stopTimestamp - startTimestamp);
	ocean_assert(actualInterval >= 0.0);

	const double error = std::fabs(actualInterval - interval);

	constexpr double threshold = 0.1;

	OCEAN_EXPECT_LESS_EQUAL(validation, error, threshold);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSignal::testSingleSignalTimeout()
{
	Log::info() << "Test single signal with timeout:";
	Log::info() << " ";

	Validation validation;

	Signal signal;
	HighPerformanceTimer timer;

	constexpr double interval = 2.0;

	SignalThread thread(timer, interval, signal);

	timer.start();
	thread.startThread();

	constexpr double timeout = 1.0;
	constexpr unsigned int timeoutMs = (unsigned int)(timeout * 1000.0);

	const Timestamp startTimestamp(true);
		const bool waitResult = signal.wait(timeoutMs);
	const Timestamp stopTimestamp(true);

	OCEAN_EXPECT_FALSE(validation, waitResult);

	const double actualInterval = double(stopTimestamp - startTimestamp);
	ocean_assert(actualInterval >= 0.0);

	const double error = std::fabs(actualInterval - timeout);

#if defined(OCEAN_USE_GTEST) && defined(OCEAN_PLATFORM_BUILD_APPLE)
	constexpr double threshold = 0.5; // using an extremely generous threshold in case the test is not executed on a real device
#else
	constexpr double threshold = 0.1;
#endif

	OCEAN_EXPECT_LESS_EQUAL(validation, error, threshold);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSignal::testSingleSignalLoop()
{
	Log::info() << "Test single signal with loop:";
	Log::info() << " ";

	Validation validation;

	Signal signal;
	HighPerformanceTimer timer;

	constexpr double interval = 0.1;
	constexpr unsigned int iterations = 100u;

	SignalThread thread(timer, interval, signal);

	const Timestamp startTimestamp(true);

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			timer.start();

			thread.startThread();
				signal.wait();
			thread.joinThread();
		}

	const Timestamp stopTimestamp(true);

	const double actualDuration = double(stopTimestamp - startTimestamp);
	ocean_assert(actualDuration >= 0.0);

	constexpr double expectedDuration = interval * double(iterations);

	const double error = std::fabs(actualDuration - expectedDuration);

#if defined(OCEAN_USE_GTEST) && defined(OCEAN_PLATFORM_BUILD_APPLE)
	constexpr double threshold = 1.5; // using an extremely generous threshold in case the test is not executed on a real device
#else
	constexpr double threshold = 0.5;
#endif

	OCEAN_EXPECT_LESS_EQUAL(validation, error, threshold);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSignal::testAsyncFunction(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test async function (pulse, wait, release):";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 10u; ++n)
		{
			AsyncFunctionThread thread;

			// let's sleep for a random time
			unsigned int sleepTime = RandomI::random(randomGenerator, 20u);
			if (sleepTime != 0u)
			{
				Thread::sleep(sleepTime);
			}

			if (RandomI::boolean(randomGenerator))
			{
				thread.startAsyncFunction();

				// let's sleep for a random time
				sleepTime = RandomI::random(randomGenerator, 20u);
				if (sleepTime != 0u)
				{
					Thread::sleep(sleepTime);
				}
			}

			// we simply ensure that the thread is disposed without any issues
			// thus, we mainly wait for a crash if something is going wrong
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSignal::testMultipleSignals()
{
	Log::info() << "Test multiple signals:";
	Log::info() << " ";

	Validation validation;

	Signals signals(4);

	HighPerformanceTimer timer;

	SignalThread signalThread0(timer, 1.0, signals[0]);
	SignalThread signalThread1(timer, 1.5, signals[1]);
	SignalThread signalThread2(timer, 2.0, signals[2]);
	SignalThread signalThread3(timer, 2.5, signals[3]);

	SignalThread* threads[4] =
	{
		&signalThread0,
		&signalThread1,
		&signalThread2,
		&signalThread3
	};

	Log::info() << "...without timeout";

	timer.start();
	Timestamp startTimestamp(true);
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.wait();
	Timestamp stopTimestamp(true);

	OCEAN_EXPECT_LESS(validation, fabs(double(stopTimestamp - startTimestamp) - 2.5), 0.1);


	Log::info() << "...with timeout";

	timer.start();
	startTimestamp.toNow();
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.wait(2000u);
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 2.0) < 0.1)
	{
		// success, nothing to do
	}
	else
	{
#ifdef _ANDROID
		Log::info() << "The test failed, however as this function is not available on Android platforms we rate the result as expected.";
#else
		OCEAN_SET_FAILED(validation);
#endif
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSignal::testSubsetSignalsStandard()
{
	Log::info() << "Test subset signals (standard):";
	Log::info() << " ";

	Validation validation;

	HighPerformanceTimer timer;

	const std::vector<double> intervals = {1.0, 1.5, 2.0, 2.5};

	// we define four threads with individual signals, the first thread will wait 1.0 second, the second 1.5 seconds,
	// the third 2.0 seconds and the fourth thread will wait 2.5 seconds until the corresponding signals will be invoked/pulsed

	Signals signals((unsigned int)(intervals.size()));

	std::vector<std::unique_ptr<SignalThread>> threads;
	threads.reserve(intervals.size());

	for (size_t n = 0; n < intervals.size(); ++n)
	{
		threads.emplace_back(std::make_unique<SignalThread>(timer, intervals[n], signals[(unsigned int)(n)]));
	}

	double error = -1.0;

	constexpr double threshold = 0.1;

	for (const unsigned int subset : {1u, 2u, 3u, 4u, 9u})
	{
		Log::info() << "... with subset " << subset;

		timer.start();

		const Timestamp startTimestamp(true);
			for (std::unique_ptr<SignalThread>& thread : threads)
			{
				thread->startThread();
			}

			signals.waitSubset(subset);
		const Timestamp stopTimestamp(true);

		for (std::unique_ptr<SignalThread>& thread : threads)
		{
			thread->joinThread();
		}

		signals.reset();

		const double actualInterval = double(stopTimestamp - startTimestamp);
		ocean_assert(actualInterval >= 0.0);

		ocean_assert(subset >= 1u);
		const double expectedInterval = intervals[std::min(size_t(subset - 1u), intervals.size() - 1)];

		const double subsetError = std::fabs(actualInterval - expectedInterval);

		if (subsetError > threshold)
		{
			Log::debug() << "Subset error: " << subsetError;
		}

		if (subsetError > error)
		{
			error = subsetError;
		}
	}

	OCEAN_EXPECT_LESS_EQUAL(validation, error, threshold);

	Log::info() << " ";
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestSignal::testSubsetSignalsTimeout()
{
	Log::info() << "Test subset signals with timeout:";
	Log::info() << " ";

	Validation validation;

	HighPerformanceTimer timer;

	const std::vector<double> intervals = {1.0, 1.5, 2.0, 2.5};

	// we define four threads with individual signals, the first thread will wait 1.0 second, the second 1.5 seconds,
	// the third 2.0 seconds and the fourth thread will wait 2.5 seconds until the corresponding signals will be invoked/pulsed

	Signals signals((unsigned int)(intervals.size()));

	std::vector<std::unique_ptr<SignalThread>> threads;
	threads.reserve(intervals.size());

	for (size_t n = 0; n < intervals.size(); ++n)
	{
		threads.emplace_back(std::make_unique<SignalThread>(timer, intervals[n], signals[(unsigned int)(n)]));
	}

	double error = -1.0;

	const Indices32 subsets = {1u, 2u, 3u, 4u, 4u};
	const Indices32 timeouts = {2000u, 2000u, 2000u, 2000u, 5000u};
	const std::vector<double> expectedIntervals = {1.0, 1.5, 2.0, 2.0, 2.5};

	ocean_assert(subsets.size() == timeouts.size());
	ocean_assert(subsets.size() == expectedIntervals.size());

	constexpr double threshold = 0.1;

	for (size_t n = 0; n < subsets.size(); ++n)
	{
		const unsigned int subset = subsets[n];
		const unsigned int timeout = timeouts[n];
		const double expectedInterval = expectedIntervals[n];

		Log::info() << "... with subset " << subset;

		timer.start();

		const Timestamp startTimestamp(true);
			for (std::unique_ptr<SignalThread>& thread : threads)
			{
				thread->startThread();
			}

			signals.waitSubset(subset, timeout);
		const Timestamp stopTimestamp(true);

		for (std::unique_ptr<SignalThread>& thread : threads)
		{
			thread->joinThread();
		}

		signals.reset();

		const double actualInterval = double(stopTimestamp - startTimestamp);
		ocean_assert(actualInterval >= 0.0);

		const double subsetError = std::fabs(actualInterval - expectedInterval);

		if (subsetError > threshold)
		{
			Log::debug() << "Subset error: " << subsetError;
		}

		if (subsetError > error)
		{
			error = subsetError;
		}
	}

	OCEAN_EXPECT_LESS_EQUAL(validation, error, threshold);

	Log::info() << " ";
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
