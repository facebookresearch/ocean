/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/testserialization/TestIOSerialization.h"
#include "ocean/test/testio/testserialization/TestDataSample.h"
#include "ocean/test/testio/testserialization/TestDataSerializer.h"
#include "ocean/test/testio/testserialization/TestDataTimestamp.h"
#include "ocean/test/testio/testserialization/TestInputDataSerializer.h"
#include "ocean/test/testio/testserialization/TestMediaSerializer.h"
#include "ocean/test/testio/testserialization/TestOutputDataSerializer.h"
#include "ocean/test/testio/testserialization/TestVectorOutputStream.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Frame.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/TaskQueue.h"
#include "ocean/base/Utilities.h"

#include "ocean/system/Process.h"

#ifdef _ANDROID
	#include "ocean/platform/android/Battery.h"
	#include "ocean/platform/android/ProcessorMonitor.h"
#endif

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace TestSerialization
{

bool testIOSerialization(const double testDuration, Worker& /*worker*/, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "+++   Ocean IO Serialization Library test:   +++";
	Log::info() << " ";

	const TestSelector selector(testFunctions);

	if (TestSelector subSelector = selector.shouldRun("datatimestamp"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestDataTimestamp::test(testDuration, subSelector) && allSucceeded;
	}

	if (TestSelector subSelector = selector.shouldRun("datasample"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestDataSample::test(testDuration, subSelector) && allSucceeded;
	}

	if (TestSelector subSelector = selector.shouldRun("dataserializer"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestDataSerializer::test(testDuration, subSelector) && allSucceeded;
	}

	if (TestSelector subSelector = selector.shouldRun("inputdataserializer"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestInputDataSerializer::test(testDuration, subSelector) && allSucceeded;
	}

	if (TestSelector subSelector = selector.shouldRun("outputdataserializer"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestOutputDataSerializer::test(testDuration, subSelector) && allSucceeded;
	}

	if (TestSelector subSelector = selector.shouldRun("mediaserializer"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestMediaSerializer::test(testDuration, subSelector) && allSucceeded;
	}

	if (TestSelector subSelector = selector.shouldRun("vectoroutputstream"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestVectorOutputStream::test(testDuration, subSelector) && allSucceeded;
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << selector << " IO Serialization library test succeeded.";
	}
	else
	{
		Log::info() << selector << " IO Serialization library test FAILED!";
	}

	return allSucceeded;
}

static void testIOSerializationAsynchronInternal(const double testDuration, const std::string testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test for the IO Serialization library:";
	Log::info() << " ";
	Log::info() << "Platform: " << Build::buildString();
	Log::info() << " ";
	Log::info() << "Start: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	Log::info() << "Function list: " << (testFunctions.empty() ? "All functions" : testFunctions);
	Log::info() << "Duration for each test: " << String::toAString(testDuration, 1u) << "s";
	Log::info() << " ";

	RandomI::initialize();
	System::Process::setPriority(System::Process::PRIORITY_ABOVE_NORMAL);

	Log::info() << "Random generator initialized";
	Log::info() << "Process priority set to above normal";
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
		testIOSerialization(testDuration, worker, testFunctions);
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

void testIOSerializationAsynchron(const double testDuration, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testIOSerializationAsynchronInternal, testDuration, testFunctions));
}

}

}

}

}
