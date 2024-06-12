/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestOpenCV.h"
#include "ocean/test/testcv/testopencv/TestFeatureDetectors.h"
#include "ocean/test/testcv/testopencv/TestFourierTransform.h"
#include "ocean/test/testcv/testopencv/TestFrameConverter.h"
#include "ocean/test/testcv/testopencv/TestFrameFilterCanny.h"
#include "ocean/test/testcv/testopencv/TestFrameFilterLaplace.h"
#include "ocean/test/testcv/testopencv/TestFrameInterpolatorBilinear.h"
#include "ocean/test/testcv/testopencv/TestFrameInterpolatorNearestPixel.h"
#include "ocean/test/testcv/testopencv/TestFrameMinMax.h"
#include "ocean/test/testcv/testopencv/TestFramePyramid.h"
#include "ocean/test/testcv/testopencv/TestGaussianBlur.h"
#include "ocean/test/testcv/testopencv/TestGeometry.h"
#include "ocean/test/testcv/testopencv/TestHistogram.h"
#include "ocean/test/testcv/testopencv/TestMorphology.h"
#include "ocean/test/testcv/testopencv/TestPerformance.h"
#include "ocean/test/testcv/testopencv/TestPointTracking.h"
#include "ocean/test/testcv/testopencv/TestTransformation.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/String.h"
#include "ocean/base/TaskQueue.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Utilities.h"

#include "ocean/system/Process.h"

#ifdef _ANDROID
#include "ocean/platform/android/Battery.h"
#include "ocean/platform/android/ProcessorMonitor.h"
#endif

#include <opencv2/imgcodecs.hpp>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

bool testCVOpenCV(const double testDuration, const std::string& imageFilename, const std::string& testFunctions)
{
	bool allSucceeded = true;

	Log::info() << "+++  CV OpenCV library test (revision " << CV_VERSION "):   +++";
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

#ifdef _ANDROID
	cv::Mat3b image;
#else
	cv::Mat3b image = cv::imread(imageFilename, cv::IMREAD_COLOR);
#endif

	if (image.empty())
	{
		Log::info() << "The image file \"" << imageFilename << "\" could not be opened so that we us a random test frame.";

		image = cv::Mat3b(800, 640);
		cv::randu(image, 0, 255);
	}
	else
	{
		Log::info() << "Successfully loaded the test image: " << imageFilename;
	}
	ocean_assert(!image.empty());

	std::vector<std::string> tests(Utilities::separateValues(String::toLower(testFunctions), ',', true, true));
	const std::set<std::string> testSet(tests.begin(), tests.end());

	if (testSet.empty() || testSet.find("histogram") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestHistogram::test(testDuration);
	}

	if (testSet.empty() || testSet.find("gaussianblur") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestGaussianBlur::test(testDuration);
	}

	if (testSet.empty() || testSet.find("framepyramid") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFramePyramid::test(testDuration);
	}

	if (testSet.empty() || testSet.find("transformation") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestTransformation::testTransformation(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("frameinterpolatorbilinear") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameInterpolatorBilinear::test(testDuration);
	}

	if (testSet.empty() || testSet.find("frameinterpolatornearestpixel") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameInterpolatorNearestPixel::test(testDuration);
	}

	if (testSet.empty() || testSet.find("geometry") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestGeometry::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("featuredetectors") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFeatureDetectors::testFeatureDetectors(image, testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("pointtracking") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestPointTracking::testPointTracking(image, testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("performance") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestPerformance::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("fouriertransform") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		allSucceeded = TestFourierTransform::test(testDuration) && allSucceeded;
	}

	if (testSet.empty() || testSet.find("frameconverter") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameConverter::test(testDuration);
	}

	if (testSet.empty() || testSet.find("frameminmax") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameMinMax::test(testDuration);
	}

	if (testSet.empty() || testSet.find("framefilterlaplace") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameFilterLaplace::test(testDuration);
	}

	if (testSet.empty() || testSet.find("morphology") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestMorphology::test(testDuration);
	}

	if (testSet.empty() || testSet.find("canny") != testSet.end())
	{
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		Log::info() << " ";
		TestFrameFilterCanny::test(testDuration);
	}

	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " OpenCV library test succeeded.";
	}
	else
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " OpenCV library test FAILED!";
	}

	return allSucceeded;
}

static void testOpenCVAsynchronInternal(const double testDuration, const std::string imageFilename, const std::string testFunctions)
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
		testCVOpenCV(testDuration, imageFilename, testFunctions);
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

void testCVOpenCVAsynchron(const double testDuration, const std::string& imageFilename, const std::string& testFunctions)
{
	ocean_assert(testDuration > 0.0);

	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(&testOpenCVAsynchronInternal, testDuration, imageFilename, testFunctions));
}

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean
