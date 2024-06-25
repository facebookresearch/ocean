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
#include "ocean/test/testbase/TestThreadPool.h"
#include "ocean/test/testbase/TestTimestamp.h"
#include "ocean/test/testbase/TestUtilities.h"
#include "ocean/test/testbase/TestValue.h"
#include "ocean/test/testbase/TestWorker.h"
#include "ocean/test/testbase/TestWorkerPool.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Processor.h"
#include "ocean/base/String.h"
#include "ocean/base/TaskQueue.h"
#include "ocean/base/Utilities.h"

#ifdef _ANDROID
	#include "ocean/platform/android/Battery.h"
	#include "ocean/platform/android/ProcessorMonitor.h"
#endif

#include "ocean/system/Process.h"

#include <set>

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

	bool allSucceeded = true;

	Log::info() << "+++   Ocean Base Library test:   +++";
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

	std::vector<std::string> tests(Utilities::separateValues(String::toLower(testFunctions), ',', true, true));
	const std::set<std::string> testSet(tests.begin(), tests.end());

	if (testSet.empty() || testSet.find("datatype") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestDataType::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("frame") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFrame::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("stl") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSTL::testSTL() && allSucceeded;
	}

	if (testSet.empty() || testSet.find("commandarguments") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestCommandArguments::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("lock") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestLock::test() && allSucceeded;
	}

	if (testSet.empty() || testSet.find("singleton") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSingleton::test() && allSucceeded;
	}

	if (testSet.empty() || testSet.find("randomi") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestRandomI::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("ringmap") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestRingMap::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("string") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestString::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("timestamp") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestTimestamp::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("datetime") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestDateTime::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("highperformancetimer") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestHighPerformanceTimer::test() && allSucceeded;
	}

	if (testSet.empty() || testSet.find("highperformancestatistic") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestHighPerformanceStatistic::test() && allSucceeded;
	}

	if (testSet.empty() || testSet.find("inheritance") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestInheritance::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("callback") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestCallback::test() && allSucceeded;
	}

	if (testSet.empty() || testSet.find("caller") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestCaller::test() && allSucceeded;
	}

	if (testSet.empty() || testSet.find("signal") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSignal::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("worker") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestWorker::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("median") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestMedian::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("memory") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestMemory::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("utilities") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestUtilities::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("hashset") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestHashSet::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("hashmap") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestHashMap::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("kdtree") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestKdTree::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("workerpool") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestWorkerPool::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("subset") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSubset::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("segmentunion") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestSegmentUnion::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("movebehavior") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestMoveBehavior::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("binary") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestBinary::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("value") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestValue::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("scopedobject") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestScopedObject::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("scopedfunction") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestScopedFunction::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("threadpool") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestThreadPool::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("staticbuffer") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestStaticBuffer::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("staticvector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestStaticVector::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("stackheapvector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestStackHeapVector::test(testDuration) && allSucceeded;
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Ocean Base Library test succeeded!";
	}
	else
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Ocean Base Library test FAILED!";
	}

	return allSucceeded;
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
