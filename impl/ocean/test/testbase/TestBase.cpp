/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestBase.h"
#include "ocean/test/testbase/TestBinary.h"
#include "ocean/test/testbase/TestCallback.h"
#include "ocean/test/testbase/TestCaller.h"
#include "ocean/test/testbase/TestCommandArguments.h"
#include "ocean/test/testbase/TestDataType.h"
#include "ocean/test/testbase/TestDateTime.h"
#include "ocean/test/testbase/TestFrame.h"
#include "ocean/test/testbase/TestHashMap.h"
#include "ocean/test/testbase/TestHashSet.h"
#include "ocean/test/testbase/TestHighPerformanceTimer.h"
#include "ocean/test/testbase/TestInheritance.h"
#include "ocean/test/testbase/TestKdTree.h"
#include "ocean/test/testbase/TestLock.h"
#include "ocean/test/testbase/TestMedian.h"
#include "ocean/test/testbase/TestMemory.h"
#include "ocean/test/testbase/TestMoveBehavior.h"
#include "ocean/test/testbase/TestRandomI.h"
#include "ocean/test/testbase/TestRingMap.h"
#include "ocean/test/testbase/TestScopedFunction.h"
#include "ocean/test/testbase/TestScopedObject.h"
#include "ocean/test/testbase/TestSegmentUnion.h"
#include "ocean/test/testbase/TestSingleton.h"
#include "ocean/test/testbase/TestStackHeapVector.h"
#include "ocean/test/testbase/TestStaticBuffer.h"
#include "ocean/test/testbase/TestStaticVector.h"
#include "ocean/test/testbase/TestSTL.h"
#include "ocean/test/testbase/TestSignal.h"
#include "ocean/test/testbase/TestString.h"
#include "ocean/test/testbase/TestSubset.h"
#include "ocean/test/testbase/TestThread.h"
#include "ocean/test/testbase/TestThreadPool.h"
#include "ocean/test/testbase/TestTimestamp.h"
#include "ocean/test/testbase/TestUtilities.h"
#include "ocean/test/testbase/TestValue.h"
#include "ocean/test/testbase/TestWorker.h"
#include "ocean/test/testbase/TestWorkerPool.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Processor.h"
#include "ocean/base/String.h"
#include "ocean/base/TaskQueue.h"

#ifdef _ANDROID
	#include "ocean/platform/android/Battery.h"
	#include "ocean/platform/android/ProcessorMonitor.h"
#endif

#include "ocean/system/Process.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool testBase(const double testDuration, Worker& worker, const std::string& testFunctions)
{
	static_assert(sizeof(bool) == 1, "Invalid data type");
	static_assert(sizeof(int) == 4, "Invalid data type");
	static_assert(sizeof(long long) == 8, "Invalid data type");

	TestResult testResult("Ocean Base Library test");
	Log::info() << " ";

	Log::info() << "Data types:";
	Log::info() << "Size of bool: " << sizeof(bool);
	Log::info() << "Size of int: " << sizeof(int);
	Log::info() << "Size of long: " << sizeof(long);
	Log::info() << "Size of size_t: " << sizeof(size_t);
	Log::info() << "Size of wchar_t: " << sizeof(wchar_t);
	Log::info() << "Size of long long: " << sizeof(long long);
	Log::info() << " ";
	Log::info() << "This device has " << Processor::get().cores() << " CPU cores (may include HT).";
	Log::info() << " ";

	const TestSelector selector(testFunctions);

	if (TestSelector subSelector = selector.shouldRun("datatype"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestDataType::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("frame"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFrame::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("stl"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSTL::testSTL(subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("commandarguments"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestCommandArguments::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("lock"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestLock::test(subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("singleton"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSingleton::test(subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("randomi"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestRandomI::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("ringmap"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestRingMap::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("string"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestString::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("timestamp"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestTimestamp::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("datetime"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestDateTime::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("highperformancetimer"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestHighPerformanceTimer::test(subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("highperformancestatistic"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestHighPerformanceStatistic::test(subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("inheritance"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestInheritance::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("callback"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestCallback::test(subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("caller"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestCaller::test(subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("signal"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSignal::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("worker"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestWorker::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("median"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestMedian::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("memory"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestMemory::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("utilities"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestUtilities::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("hashset"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestHashSet::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("hashmap"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestHashMap::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("kdtree"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestKdTree::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("workerpool"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestWorkerPool::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("subset"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSubset::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("segmentunion"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSegmentUnion::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("movebehavior"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestMoveBehavior::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("binary"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestBinary::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("value"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestValue::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("scopedobject"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestScopedObject::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("scopedfunction"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestScopedFunction::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("thread"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestThread::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("threadpool"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestThreadPool::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("staticbuffer"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestStaticBuffer::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("staticvector"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestStaticVector::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("stackheapvector"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestStackHeapVector::test(testDuration, subSelector);
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	Log::info() << selector << " " << testResult;

	return testResult.succeeded();
}

static void testBaseAsynchronInternal(const double testDuration, const std::string testFunctions)
{
	ocean_assert(testDuration > 0.0);

	System::Process::setPriority(System::Process::PRIORITY_ABOVE_NORMAL);
	Log::info() << "Process priority set to above normal";
	Log::info() << " ";

	const Timestamp startTimestamp(true);

	Log::info() << "Starting Ocean Base Library test";
	Log::info() << "Platform: " << Build::buildString();
	Log::info() << "Start: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	Log::info() << "Function list: " << (testFunctions.empty() ? "All functions" : testFunctions);
	Log::info() << "Duration for each test: " << String::toAString(testDuration, 1u) << "s";
	Log::info() << " ";

	Worker worker;

	Log::info() << "Used worker threads: " << worker.threads();

#ifdef _ANDROID
	Platform::Android::ProcessorStatistic processorStatistic;
	processorStatistic.start();

	Log::info() << " ";
	Log::info() << "Battery: " << String::toAString(Platform::Android::Battery::currentCapacity(), 1u) << "%, temperature: " << String::toAString(Platform::Android::Battery::currentTemperature(), 1u) << "deg Celsius";
#endif

	Log::info() << " ";

	try
	{
		testBase(testDuration, worker, testFunctions);
	}
	catch (const std::exception& exception)
	{
		Log::error() << "Unhandled exception: " << exception.what();
	}
	catch (...)
	{
		Log::error() << "Unhandled exception!";
	}

#ifdef _ANDROID
	processorStatistic.stop();

	Log::info() << " ";
	Log::info() << "Duration: " << " in " << processorStatistic.duration() << "s";
	Log::info() << "Measurements: " << processorStatistic.measurements();
	Log::info() << "Average active cores: " << processorStatistic.averageActiveCores();
	Log::info() << "Average frequency: " << processorStatistic.averageFrequency() << "kHz";
	Log::info() << "Minimal frequency: " << processorStatistic.minimalFrequency() << "kHz";
	Log::info() << "Maximal frequency: " << processorStatistic.maximalFrequency() << "kHz";
	Log::info() << "Average CPU performance rate: " << processorStatistic.averagePerformanceRate();

	Log::info() << " ";
	Log::info() << "Battery: " << String::toAString(Platform::Android::Battery::currentCapacity(), 1u) << "%, temperature: " << String::toAString(Platform::Android::Battery::currentTemperature(), 1u) << "deg Celsius";
#endif

	Log::info() << " ";

	const Timestamp endTimestamp(true);

	Log::info() << "Time elapsed: " << DateTime::seconds2string(double(endTimestamp - startTimestamp), true);
	Log::info() << "End: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";
}

void testBaseAsynchron(const double testDuration, const std::string& testFunctions)
{
	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testBaseAsynchronInternal, testDuration, testFunctions));
}

}

}

}
