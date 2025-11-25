/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"
#include "ocean/test/testcv/testadvanced/TestAdvancedFrameChannels.h"
#include "ocean/test/testcv/testadvanced/TestAdvancedFrameFilterGaussian.h"
#include "ocean/test/testcv/testadvanced/TestAdvancedFrameFilterSeparable.h"
#include "ocean/test/testcv/testadvanced/TestAdvancedFrameFilterSobel.h"
#include "ocean/test/testcv/testadvanced/TestAdvancedFrameInterpolatorBilinear.h"
#include "ocean/test/testcv/testadvanced/TestAdvancedFrameShrinker.h"
#include "ocean/test/testcv/testadvanced/TestAdvancedMotion.h"
#include "ocean/test/testcv/testadvanced/TestAdvancedSumSquareDifferences.h"
#include "ocean/test/testcv/testadvanced/TestAdvancedZeroMeanSumSquareDifferences.h"
#include "ocean/test/testcv/testadvanced/TestFrameColorAdjustment.h"
#include "ocean/test/testcv/testadvanced/TestFrameRectification.h"
#include "ocean/test/testcv/testadvanced/TestPanoramaFrame.h"
#include "ocean/test/testcv/testadvanced/TestSumSquareDifferencesNoCenter.h"

#include "ocean/test/TestResult.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/String.h"
#include "ocean/base/TaskQueue.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/system/Process.h"

#ifdef _ANDROID
	#include "ocean/platform/android/Battery.h"
	#include "ocean/platform/android/ProcessorMonitor.h"
#endif

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool testCVAdvanced(const double testDuration, Worker& worker, const unsigned int width, const unsigned int height, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width >= 32u && height >= 32u);

	TestResult testResult("Ocean Advanced Computer Vision Library test");

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

	if (TestSelector subSelector = selector.shouldRun("advancedframechannels"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAdvancedFrameChannels::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("advancedframefilterseparable"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAdvancedFrameFilterSeparable::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("advancedframefiltergaussian"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAdvancedFrameFilterGaussian::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framecoloradjustment"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFrameColorAdjustment::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("advancedframeshrinker"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAdvancedFrameShrinker::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("advancedframefiltersobel"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAdvancedFrameFilterSobel::test(320u, 240u, 120u, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("advancedframeinterpolatorbilinear"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAdvancedFrameInterpolatorBilinear::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("advancedsumsquaredifferences"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAdvancedSumSquareDifferences::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("advancedzeromeansumsquaredifferences"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAdvancedZeroMeanSumSquareDifferences::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("sumsquaredifferencesnocenter"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestSumSquareDifferencesNoCenter::test(testDuration, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("framerectification"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFrameRectification::test(width, height, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("panoramaframe"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestPanoramaFrame::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("advancedmotion"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestAdvancedMotion::test(width, height, testDuration, worker, subSelector);
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	Log::info() << selector << " " << testResult;

	return testResult.succeeded();
}

static void testCVAdvancedAsynchronInternal(const double testDuration, const unsigned int width, const unsigned int height, const std::string testFunctions)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width >= 32u && height >= 32u);

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test for the Computer Vision Advanced library:";
	Log::info() << " ";
	Log::info() << "Platform: " << Build::buildString();
	Log::info() << " ";
	Log::info() << "Start: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	Log::info() << "Default test frame dimension: " << width << "x" << height;
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
		testCVAdvanced(testDuration, worker, width, height, testFunctions);
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

void testCVAdvancedAsynchron(const double testDuration, const unsigned int width, const unsigned int height, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testCVAdvancedAsynchronInternal, testDuration, width, height, testFunctions));
}

}

}

}

}
