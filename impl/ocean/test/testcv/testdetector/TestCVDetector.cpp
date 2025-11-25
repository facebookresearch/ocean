/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/test/testcv/testdetector/TestDescriptor.h"
#include "ocean/test/testcv/testdetector/TestFASTDetector.h"
#include "ocean/test/testcv/testdetector/TestFeatureDetector.h"
#include "ocean/test/testcv/testdetector/TestFREAKDescriptor.h"
#include "ocean/test/testcv/testdetector/TestFrameChangeDetector.h"
#include "ocean/test/testcv/testdetector/TestHarrisDetector.h"
#include "ocean/test/testcv/testdetector/TestHemiCube.h"
#include "ocean/test/testcv/testdetector/TestLineDetectorHough.h"
#include "ocean/test/testcv/testdetector/TestLineDetectorULF.h"
#include "ocean/test/testcv/testdetector/TestLineEvaluator.h"
#include "ocean/test/testcv/testdetector/TestMessengerCodeDetector.h"
#include "ocean/test/testcv/testdetector/TestORBDetector.h"
#include "ocean/test/testcv/testdetector/TestPointTracking.h"
#include "ocean/test/testcv/testdetector/TestShapeDetector.h"

#include "ocean/test/TestResult.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Frame.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/TaskQueue.h"
#include "ocean/base/Utilities.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverterRGB24.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/System.h"

#include "ocean/system/Process.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(__APPLE__)
		#include "ocean/media/imageio/ImageIO.h"
	#elif defined(_ANDROID)
			#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
	#elif defined(_WINDOWS)
		#include "ocean/media/wic/WIC.h"
	#endif
#endif

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

namespace TestDetector
{

bool testCVDetector(const double testDuration, Worker& worker, const std::string& testImageFilename, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Ocean Computer Vision Detector Library test");

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

	Frame testFrame;
	if (!testImageFilename.empty())
	{
#ifdef OCEAN_RUNTIME_STATIC
		OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());
		OCEAN_APPLY_IF_ANDROID(Media::OpenImageLibraries::registerOpenImageLibrariesLibrary());
		OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
#else
		const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

		if (PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString()) == 0)
		{
			PluginManager::get().collectPlugins("plugins");
		}
		PluginManager::get().loadPlugins(Ocean::PluginManager::TYPE_MEDIA);
#endif

		testFrame = Media::Utilities::loadImage(testImageFilename);

		if (testFrame.isValid())
		{
			Log::info() << "Successfully loaded the test image: " << testImageFilename;
		}
		else
		{
			Log::info() << "Failed to load test image: " << testImageFilename;
		}
	}

	if (!testFrame.isValid())
	{
		Log::info() << "No specific test image is used";
	}

	Frame rgbFrame;
	if (testFrame && !CV::FrameConverter::Comfort::convert(testFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
	{
		Log::info() << "Failed to converted the defined test frame to a RGB24 test frame!";
	}

	Frame yFrame;
	if (testFrame && !CV::FrameConverter::Comfort::convert(testFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_ALWAYS_COPY))
	{
		Log::info() << "Failed to converted the defined test frame to a Y8 test frame!";
	}

	const TestSelector selector(testFunctions);

	Log::info() << " ";

	if (TestSelector subSelector = selector.shouldRun("linedetectorhough"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestLineDetectorHough::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("linedetectorulf"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestLineDetectorULF::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("fastdetector"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFASTDetector::test(yFrame, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("harrisdetector"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestHarrisDetector::test(yFrame, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("orbdetector"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestORBDetector::test(yFrame, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("featuredetector"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFeatureDetector::test(yFrame, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("pointtracking"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestPointTracking::test(rgbFrame, testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("lineevaluator"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestLineEvaluator::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("hemicube"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestHemiCube::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("messengercodedetector"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestMessengerCodeDetector::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("descriptor"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestDescriptor::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("freakdescriptor32"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFREAKDescriptor32::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("freakdescriptor64"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFREAKDescriptor64::test(testDuration, worker, subSelector);
	}

	if (TestSelector subSelector = selector.shouldRun("shapedetector"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestShapeDetector::test(testDuration, subSelector);
  }

	if (TestSelector subSelector = selector.shouldRun("framechangedetector"))
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		testResult = TestFrameChangeDetector::test(testDuration, worker, subSelector);
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	Log::info() << selector << " " << testResult;

#ifdef OCEAN_RUNTIME_STATIC
		OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
		OCEAN_APPLY_IF_ANDROID(Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary());
		OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
#else
		PluginManager::get().release();
#endif

	return testResult.succeeded();
}

static void testCVDetectorAsynchronInternal(const double testDuration, const std::string testImageFilename, const std::string testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test for the Computer Vision Detector library:";
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
		testCVDetector(testDuration, worker, testImageFilename, testFunctions);
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

void testCVDetectorAsynchron(const double testDuration, const std::string& testImageFilename, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testCVDetectorAsynchronInternal, testDuration, testImageFilename, testFunctions));
}

}

}

}

}
