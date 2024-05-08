/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/test/testcv/testdetector/testqrcodes/TestAlignmentPatternDetector.h"
#include "ocean/test/testcv/testdetector/testqrcodes/TestQRCodeDetector2D.h"
#include "ocean/test/testcv/testdetector/testqrcodes/TestFinderPatternDetector.h"
#include "ocean/test/testcv/testdetector/testqrcodes/TestLegacyQRCodeDetector2D.h"
#include "ocean/test/testcv/testdetector/testqrcodes/TestQRCodeDecoder.h"
#include "ocean/test/testcv/testdetector/testqrcodes/TestQRCodeEncoder.h"
#include "ocean/test/testcv/testdetector/testqrcodes/TestUtilities.h"

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

namespace TestQRCodes
{

bool testCVDetectorQRCodes(const double testDuration, Worker& worker, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "+++   Ocean Computer Vision Detector Library QR Codes test:   +++";
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

	Log::info() << " ";

	std::vector<std::string> tests(Ocean::Utilities::separateValues(String::toLower(testFunctions), ',', true, true));
	const std::set<std::string> testSet(tests.begin(), tests.end());

	if (testSet.empty() || testSet.find("qrcodedecoder") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestQRCodeDecoder::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("qrcodeencoder") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestQRCodeEncoder::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("legacyqrcodedetector2d") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestLegacyQRCodeDetector2D::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("finderpatterndetector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFinderPatternDetector::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("alignmentpatterndetector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestAlignmentPatternDetector::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("qrcodedetector2d") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestQRCodeDetector2D::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("utilities") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestUtilities::test(testDuration) && allSucceeded;
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Computer Vision Detector QR Codes library test succeeded.";
	}
	else
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Computer Vision Detector QR Codes library test FAILED!";
	}

#ifdef OCEAN_RUNTIME_STATIC
		OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
		OCEAN_APPLY_IF_ANDROID(Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary());
		OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
#else
		PluginManager::get().release();
#endif

	return allSucceeded;
}

static void testCVDetectorQRCodesAsynchronousInternal(const double testDuration, const std::string testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test for the Computer Vision Detector QR Codes library:";
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
		testCVDetectorQRCodes(testDuration, worker, testFunctions);
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

void testCVDetectorQRCodesAsynchronous(const double testDuration, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testCVDetectorQRCodesAsynchronousInternal, testDuration, testFunctions));
}

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namepace Test
