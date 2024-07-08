/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmedia/TestMedia.h"
#include "ocean/test/testmedia/TestMovie.h"
#include "ocean/test/testmedia/TestOpenImageLibraries.h"
#include "ocean/test/testmedia/TestSpecial.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE
	#include "ocean/test/testmedia/TestAVFoundation.h"
	#include "ocean/test/testmedia/TestImageIO.h"
#endif

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS
	#include "ocean/test/testmedia/TestWIC.h"
#endif

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/TaskQueue.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Utilities.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#include "ocean/platform/android/Battery.h"
	#include "ocean/platform/android/ProcessorMonitor.h"
#endif

#include "ocean/system/Process.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

bool testMedia(const double testDuration, Worker& /*worker*/, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "+++   Ocean Media Library test:   +++";
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

	std::vector<std::string> tests(Utilities::separateValues(String::toLower(testFunctions), ',', true, true));
	const std::set<std::string> testSet(tests.begin(), tests.end());

	if (testSet.empty() || testSet.find("openimagelibraries") != testSet.end())
	{
		Log::info() << "\n\n\n\n";
		allSucceeded = TestOpenImageLibraries::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("special") != testSet.end())
	{
		Log::info() << "\n\n\n\n";
		allSucceeded = TestSpecial::test(testDuration) && allSucceeded;
	}

#ifdef OCEAN_PLATFORM_BUILD_APPLE

	if (testSet.empty() || testSet.find("imageio") != testSet.end())
	{
		Log::info() << "\n\n\n\n";
		allSucceeded = TestImageIO::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("avfoundation") != testSet.end())
	{
		Log::info() << "\n\n\n\n";
		allSucceeded = TestAVFoundation::test(testDuration) && allSucceeded;
	}

#endif

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS

	if (testSet.empty() || testSet.find("wic") != testSet.end())
	{
		Log::info() << "\n\n\n\n";
		allSucceeded = TestWIC::test(testDuration) && allSucceeded;
	}

#endif

	if (testSet.empty() || testSet.find("movie") != testSet.end())
	{
		Log::info() << "\n\n\n\n";
		allSucceeded = TestMovie::test(testDuration) && allSucceeded;
	}

	Log::info() << "\n\n\n\n";

	if (allSucceeded)
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Media library test succeeded.";
	}
	else
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Media library test FAILED!";
	}

	return allSucceeded;
}

static void testMediaAsynchronInternal(const double testDuration, const std::string testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test for the Media libraries:";
	Log::info() << "Platform: " << Build::buildString();
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

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	Platform::Android::ProcessorStatistic processorStatistic;
	processorStatistic.start();

	Log::info() << " ";
	Log::info() << "Battery: " << String::toAString(Platform::Android::Battery::currentCapacity(), 1u) << "%, temperature: " << String::toAString(Platform::Android::Battery::currentTemperature(), 1u) << "deg Celsius";
#endif

	Log::info() << " ";

	try
	{
		testMedia(testDuration, worker, testFunctions);
	}
	catch (const std::exception& exception)
	{
		Log::error() << "Unhandled exception: " << exception.what();
	}
	catch (...)
	{
		Log::error() << "Unhandled exception!";
	}

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
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

void testMediaAsynchron(const double testDuration, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testMediaAsynchronInternal, testDuration, testFunctions));
}

}

}

}
