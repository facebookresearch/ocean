// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/test/testcv/testdetector/TestBlobFeatureDetector.h"
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

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Frame.h"
#include "ocean/base/PluginManager.h"
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

	bool allSucceeded = true;

	Log::info() << "+++   Ocean Computer Vision Detector Library test:   +++";
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

	if (testFrame.isNull())
	{
		Log::info() << "No specific test image is used";
	}

	Frame rgbFrame;
	if (testFrame && !CV::FrameConverter::Comfort::convert(testFrame, FrameType(testFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), rgbFrame, true, nullptr))
	{
		Log::info() << "Failed to converted the defined test frame to a RGB24 test frame!";
	}

	Frame yFrame;
	if (testFrame && !CV::FrameConverter::Comfort::convert(testFrame, FrameType(testFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, true, nullptr))
	{
		Log::info() << "Failed to converted the defined test frame to a Y8 test frame!";
	}

	std::vector<std::string> tests(Utilities::separateValues(String::toLower(testFunctions), ',', true, true));
	const std::set<std::string> testSet(tests.begin(), tests.end());

	Log::info() << " ";

	if (testSet.empty() || testSet.find("linedetectorhough") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestLineDetectorHough::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("linedetectorulf") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestLineDetectorULF::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("fastdetector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFASTDetector::test(yFrame, testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("harrisdetector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestHarrisDetector::test(yFrame, testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("blobfeaturedetector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestBlobFeatureDetector::test(yFrame, testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("orbdetector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestORBDetector::test(yFrame, testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("featuredetector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFeatureDetector::test(yFrame, testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("pointtracking") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestPointTracking::test(rgbFrame, testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("lineevaluator") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestLineEvaluator::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("hemicube") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestHemiCube::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("messengercodedetector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestMessengerCodeDetector::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("descriptor") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestDescriptor::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("freakdescriptor32") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFREAKDescriptor32::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("freakdescriptor64") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFREAKDescriptor64::test(testDuration, worker) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("shapedetector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestShapeDetector::test(testDuration) && allSucceeded;
  }

	if (testSet.empty() || testSet.find("framechangedetector") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFrameChangeDetector::test(testDuration, worker) && allSucceeded;
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Computer Vision Detector library test succeeded.";
	}
	else
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Computer Vision Detector library test FAILED!";
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
