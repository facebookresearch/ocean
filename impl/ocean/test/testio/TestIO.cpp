/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestIO.h"
#include "ocean/test/testio/TestBase64.h"
#include "ocean/test/testio/TestBitstream.h"
#include "ocean/test/testio/TestCameraCalibrationManager.h"
#include "ocean/test/testio/TestCompression.h"
#include "ocean/test/testio/TestDirectory.h"
#include "ocean/test/testio/TestFile.h"
#include "ocean/test/testio/TestJSONParser.h"
#include "ocean/test/testio/TestUtilities.h"

#include "ocean/test/TestResult.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Processor.h"
#include "ocean/base/String.h"
#include "ocean/base/TaskQueue.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Utilities.h"

#ifdef _ANDROID
	#include "ocean/platform/android/Battery.h"
	#include "ocean/platform/android/ProcessorMonitor.h"
#endif

#include "ocean/system/Process.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

bool testIO(const double testDuration, const std::string& testFunctions)
{
	TestResult testResult("Ocean IO Library test");

	Log::info() << " ";

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
	Log::info() << "The binary contains at most SSE4.1 instructions.";
#endif

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	Log::info() << "The binary contains at most NEON1 instructions.";
#endif

#if defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 20
	Log::info() << "The binary contains at most AVX2 instructions.";
#elif defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 10
	Log::info() << "The binary contains at most AVX1 instructions.";
#endif

#if (!defined(OCEAN_HARDWARE_SSE_VERSION) || OCEAN_HARDWARE_SSE_VERSION == 0) && (!defined(OCEAN_HARDWARE_NEON_VERSION) || OCEAN_HARDWARE_NEON_VERSION == 0)
	static_assert(OCEAN_HARDWARE_AVX_VERSION == 0, "Invalid AVX version");
	Log::info() << "The binary does not contain any SIMD instructions.";
#endif

	Log::info() << "While the hardware supports the following SIMD instructions:";
	Log::info() << Processor::translateInstructions(Processor::get().instructions());

	Log::info() << " ";

	const TestSelector selector(testFunctions);

	if (TestSelector subSelector = selector.shouldRun("bitstream"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestBitstream::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("compression"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestCompression::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("base64"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestBase64::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("directory"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestDirectory::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("file"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFile::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("utilities"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestUtilities::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("jsonparser"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestJSONParser::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("cameracalibrationmanager"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestCameraCalibrationManager::test(testDuration, subSelector);
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	Log::info() << selector << " " << testResult;

	return testResult.succeeded();
}

static void testIOAsynchronInternal(const double testDuration, const std::string testFunctions)
{
	ocean_assert(testDuration > 0.0);

	System::Process::setPriority(System::Process::PRIORITY_ABOVE_NORMAL);
	Log::info() << "Process priority set to above normal";
	Log::info() << " ";

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test for the IO library:";
	Log::info() << "Platform: " << Build::buildString();
	Log::info() << "Start: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	Log::info() << "Function list: " << (testFunctions.empty() ? "All functions" : testFunctions);
	Log::info() << "Duration for each test: " << String::toAString(testDuration, 1u) << "s";
	Log::info() << " ";

#ifdef _ANDROID
	Platform::Android::ProcessorStatistic processorStatistic;
	processorStatistic.start();

	Log::info() << " ";
	Log::info() << "Battery: " << String::toAString(Platform::Android::Battery::currentCapacity(), 1u) << "%, temperature: " << String::toAString(Platform::Android::Battery::currentTemperature(), 1u) << "deg Celsius";
#endif

	Log::info() << " ";

	try
	{
		testIO(testDuration, testFunctions);
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

void testIOAsynchron(const double testDuration, const std::string& testFunctions)
{
	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testIOAsynchronInternal, testDuration, testFunctions));
}

}

}

}
