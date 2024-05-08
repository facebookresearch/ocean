/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testlibyuv/TestLibyuv.h"
#include "ocean/test/testcv/testlibyuv/TestFrameConverterBGRA32.h"
#include "ocean/test/testcv/testlibyuv/TestFrameConverterRGBA32.h"
#include "ocean/test/testcv/testlibyuv/TestFrameConverterY_UV12.h"
#include "ocean/test/testcv/testlibyuv/TestFrameConverterY_U_V12.h"
#include "ocean/test/testcv/testlibyuv/TestFrameConverterY_U_V24.h"
#include "ocean/test/testcv/testlibyuv/TestFrameConverterY_VU12.h"
#include "ocean/test/testcv/testlibyuv/TestFrameInterpolatorBilinear.h"
#include "ocean/test/testcv/testlibyuv/TestFramePyramid.h"
#include "ocean/test/testcv/testlibyuv/TestFrameShrinker.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Processor.h"
#include "ocean/base/String.h"
#include "ocean/base/TaskQueue.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/system/Memory.h"
#include "ocean/system/OperatingSystem.h"
#include "ocean/system/Process.h"

#ifdef _ANDROID
	#include "ocean/platform/android/Battery.h"
	#include "ocean/platform/android/ProcessorMonitor.h"
#endif

#include <libyuv/version.h>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestLibyuv
{

void testCVLibyuv(const double testDuration, const bool skipValidation, const std::string& testFunctions)
{
	Log::info() << "+++  CV Libyuv library test (revision " << LIBYUV_VERSION << "):   +++";
	Log::info() << " ";
	Log::info() << " ";

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 1
	Log::info() << "The hardware supports SSE instructions.";
#endif

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 1
	Log::info() << "The hardware supports NEON instructions.";
#endif

#if defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 2
	Log::info() << "The hardware supports AVX2 instructions.";
#elif defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 1
	Log::info() << "The hardware supports AVX1 instructions.";
#endif

#if (!defined(OCEAN_HARDWARE_SSE_VERSION) || OCEAN_HARDWARE_SSE_VERSION == 0) && (!defined(OCEAN_HARDWARE_NEON_VERSION) || OCEAN_HARDWARE_NEON_VERSION == 0)
	static_assert(OCEAN_HARDWARE_AVX_VERSION == 0, "Invalid AVX version");
	Log::info() << "The hardware does not support any SIMD instructions.";
#endif

	// we aquire the worker pool to ensure that we have a worker created when we need one
	WorkerPool::get().scopedWorker()();

	std::vector<std::string> tests(Utilities::separateValues(String::toLower(testFunctions), ',', true, true));
	const std::set<std::string> testSet(tests.begin(), tests.end());

	if (testSet.empty() || testSet.find("frameconverterbgra32") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameConverterBGRA32::test(testDuration, skipValidation);
	}

	if (testSet.empty() || testSet.find("frameconverterrgba32") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameConverterRGBA32::test(testDuration, skipValidation);
	}

	if (testSet.empty() || testSet.find("frameconvertery_u_v12") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameConverterY_U_V12::test(testDuration, skipValidation);
	}

	if (testSet.empty() || testSet.find("frameconvertery_u_v24") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameConverterY_U_V24::test(testDuration, skipValidation);
	}

	if (testSet.empty() || testSet.find("frameconvertery_uv12") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameConverterY_UV12::test(testDuration, skipValidation);
	}

	if (testSet.empty() || testSet.find("frameconvertery_vu12") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameConverterY_VU12::test(testDuration, skipValidation);
	}

	if (testSet.empty() || testSet.find("frameinterpolatorbilinear") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameInterpolatorBilinear::test(testDuration);
	}

	if (testSet.empty() || testSet.find("framepyramid") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFramePyramid::test(testDuration);
	}

	if (testSet.empty() || testSet.find("frameshrinker") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameShrinker::test(testDuration);
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
}

static void testLibyuvAsynchronInternal(const double testDuration, const bool skipValidation, const std::string testFunctions)
{
	ocean_assert(testDuration > 0.0);

	System::Process::setPriority(System::Process::PRIORITY_ABOVE_NORMAL);
	Log::info() << "Process priority set to above normal";
	Log::info() << " ";

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test for the Computer Vision library:";
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
		testCVLibyuv(testDuration, skipValidation, testFunctions);
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

void testCVLibyuvAsynchron(const double testDuration, const bool skipValidation, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testLibyuvAsynchronInternal, testDuration, skipValidation, testFunctions));
}

}

}

}

}
