// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testbase/TestSignal.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include <cmath>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

TestSignal::SignalThread::SignalThread(const HighPerformanceTimer& timer, const double timeout, Signal& signal) :
	threadTimer(timer),
	threadTimeout(timeout),
	threadSignal(signal)
{
	ocean_assert(threadTimeout > 0.0);
}

void TestSignal::SignalThread::threadRun()
{
	while (threadTimer.seconds() < threadTimeout)
	{
		Thread::sleep(0u);
	}

	threadSignal.pulse();
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

bool TestSignal::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Signal tests:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testSignalBasics() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testSingleSignal() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testAsyncFunction(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testMultipleSignals() && allSucceeded;

	Log::info() << " ";
	Log::info() << " ";

	allSucceeded = testSubsetSignals() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Signal test succeeded.";
	}
	else
	{
		Log::info() << "Signal test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSignal, SignalBasics)
{
	EXPECT_TRUE(TestSignal::testSignalBasics());
}

#ifndef OCEAN_PLATFORM_BUILD_APPLE_IOS_SUMULATOR // iOS simulator does not allow for testing the precise signal performance

TEST(TestSignal, SingleSignal)
{
	EXPECT_TRUE(TestSignal::testSingleSignal());
}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_SUMULATOR

TEST(TestSignal, AsyncFunction)
{
	EXPECT_TRUE(TestSignal::testAsyncFunction(GTEST_TEST_DURATION));
}

TEST(TestSignal, MultipleSignals)
{
	EXPECT_TRUE(TestSignal::testMultipleSignals());
}

TEST(TestSignal, SubsetSignals)
{
	EXPECT_TRUE(TestSignal::testSubsetSignals());
}

#endif // OCEAN_USE_GTEST

bool TestSignal::testSignalBasics()
{
	bool allSucceeded = true;

	Log::info() << "Test signal basics:";
	Log::info() << " ";

	Log::info() << "...resizing signals";
	Signals signals;
	signals.setSize(2u);

	if (signals.size() == 2u)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
	}

	Log::info() << "...releasing signals";

	signals[0].release();
	signals[1].release();
	Log::info() << "Validation: succeeded.";

	Log::info() << "...simple signal";

	Signal signal;
	signal.pulse();
	signal.wait();
	Log::info() << "Validation: succeeded.";

	return allSucceeded;
}

bool TestSignal::testSingleSignal()
{
	bool allSucceeded = true;

	Log::info() << "Test single signal:";
	Log::info() << " ";

	Signal signal;
	HighPerformanceTimer timer;

	SignalThread thread(timer, 2.0, signal);

	Log::info() << "...without timeout";

	timer.start();
	thread.startThread();

	Timestamp startTimestamp(true);
	signal.wait();
	Timestamp stopTimestamp(true);

	if (fabs(double(stopTimestamp - startTimestamp) - 2.0) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
	}

	thread.joinThread();


	Log::info() << "...with timeout";
	timer.start();
	thread.startThread();

	startTimestamp.toNow();
	const bool waitResult = signal.wait(1000u);
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 1.0) < 0.1 && !waitResult)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
#ifdef _ANDROID
		Log::info() << "The test failed, however as this function is not available on Android platforms we rate the result as expected.";
#else
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
#endif
	}

	thread.joinThread();


	Log::info() << "...without timeout (multiple times)";

	SignalThread secondThread(timer, 0.1, signal);

	startTimestamp.toNow();
	for (unsigned int n = 0u; n < 100u; ++n)
	{
		timer.start();
		secondThread.startThread();
		signal.wait();
		secondThread.joinThread();
	}
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 10.0) < 0.5)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
	}

	return allSucceeded;
}

bool TestSignal::testAsyncFunction(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test async function (pulse, wait, release):";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 10u; ++n)
		{
			AsyncFunctionThread thread;

			// let's sleep for a random time
			unsigned int sleepTime = RandomI::random(20u);
			if (sleepTime != 0u)
			{
				Thread::sleep(sleepTime);
			}

			if (RandomI::random(1u) == 0u)
			{
				thread.startAsyncFunction();

				// let's sleep for a random time
				sleepTime = RandomI::random(20u);
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

bool TestSignal::testMultipleSignals()
{
	bool allSucceeded = true;

	Log::info() << "Test multiple signals:";
	Log::info() << " ";

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

	if (fabs(double(stopTimestamp - startTimestamp) - 2.5) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
	}


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
		Log::info() << "Validation: succeeded.";
	}
	else
	{
#ifdef _ANDROID
		Log::info() << "The test failed, however as this function is not available on Android platforms we rate the result as expected.";
#else
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
#endif
	}

	return allSucceeded;
}

bool TestSignal::testSubsetSignals()
{
	bool allSucceeded = true;

	Log::info() << "Test subset signals:";
	Log::info() << " ";

	HighPerformanceTimer timer;

	Signals signals(4u);

	// we define four threads with individual signals, the first thread will wait 1.0 second, the second 1.5 seconds,
	// the third 2.0 seconds and the fourth thread will wait 2.5 seconds until the corresponding signals will be invoked/pulsed
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


	Log::info() << "...without timeout (1)";
	timer.start();
	Timestamp startTimestamp(true);
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.waitSubset(1u);
	Timestamp stopTimestamp(true);

	if (fabs(double(stopTimestamp - startTimestamp) - 1.0) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
	}


	Log::info() << "...without timeout (2)";
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->joinThread();
	}
	signals.reset();

	startTimestamp.toNow();
	timer.start();
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.waitSubset(2u);
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 1.5) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
	}


	Log::info() << "...without timeout (3)";
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->joinThread();
	}
	signals.reset();

	startTimestamp.toNow();
	timer.start();
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.waitSubset(3u);
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 2.0) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
	}


	Log::info() << "...without timeout (4)";
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->joinThread();
	}
	signals.reset();

	startTimestamp.toNow();
	timer.start();
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.waitSubset(4u);
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 2.5) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
	}



	Log::info() << "...without timeout (9)";
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->joinThread();
	}
	signals.reset();

	startTimestamp.toNow();
	timer.start();
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.waitSubset(9u);
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 2.5) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
	}



	Log::info() << " ";
	Log::info() << "...with timeout (1)";
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->joinThread();
	}
	signals.reset();

	startTimestamp.toNow();
	timer.start();
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.waitSubset(1u, 2000u);
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 1.0) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
#ifdef _ANDROID
		Log::info() << "The test failed, however as this function is not available on Android platforms we rate the result as expected.";
#else
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
#endif
	}


	Log::info() << "...with timeout (2)";
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->joinThread();
	}
	signals.reset();

	startTimestamp.toNow();
	timer.start();
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.waitSubset(2u, 2000u);
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 1.5) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
#ifdef _ANDROID
		Log::info() << "The test failed, however as this function is not available on Android platforms we rate the result as expected.";
#else
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
#endif
	}


	Log::info() << "...with timeout (3)";
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->joinThread();
	}
	signals.reset();

	startTimestamp.toNow();
	timer.start();
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.waitSubset(3u, 2000u);
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 2.0) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
#ifdef _ANDROID
		Log::info() << "The test failed, however as this function is not available on Android platforms we rate the result as expected.";
#else
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
#endif
	}


	Log::info() << "...with timeout (4)";
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->joinThread();
	}
	signals.reset();

	startTimestamp.toNow();
	timer.start();
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.waitSubset(4u, 2000u);
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 2.0) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
#ifdef _ANDROID
		Log::info() << "The test failed, however as this function is not available on Android platforms we rate the result as expected.";
#else
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
#endif
	}


	Log::info() << "...with timeout (4)+";
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->joinThread();
	}
	signals.reset();

	startTimestamp.toNow();
	timer.start();
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		threads[n]->startThread();
	}

	signals.waitSubset(4u, 5000u);
	stopTimestamp.toNow();

	if (fabs(double(stopTimestamp - startTimestamp) - 2.5) < 0.1)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
#ifdef _ANDROID
		Log::info() << "The test failed, however as this function is not available on Android platforms we rate the result as expected.";
#else
		Log::info() << "Validation: FAILED!";
		allSucceeded = false;
#endif
	}

	return allSucceeded;
}

}

}

}
