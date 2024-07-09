/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestHistogram.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/Histogram.h"
#include "ocean/cv/OpenCVUtilities.h"

#include "ocean/math/Random.h"

#include <opencv2/imgproc.hpp>

namespace Ocean
{
namespace Test
{
namespace TestCV
{
namespace TestOpenCV
{
#ifdef OCEAN_USE_GTEST

TEST(TestOpenCV, TestCLAHE)
{
	EXPECT_TRUE(TestHistogram::testCLAHE(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestHistogram::test(const double testDuration)
{
	assert(testDuration > 0.0);

	bool success = true;

	Log::info() << "---   Histogram test:   ---";
	Log::info() << " ";

	success = testCLAHE(testDuration) && success;

	Log::info() << " ";

	Log::info() << "Histogram test " << (success ? "finished successfully" : "FAILED");

	return success;
}

bool TestHistogram::testCLAHE(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Worker worker;

	Log::info() << "CLAHE test:";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	const FrameType::DataType dataType = FrameType::DT_UNSIGNED_INTEGER_8;
	const unsigned int channels = 1u;

	// Performance
	Log::info() << "Performance tests (duration per image size: " << testDuration << "s)";
	Log::info() << "Image size (WxH), Iterations, Ocean [worst, median, best] ms, OpenCV [worst, median, best] ms, Ratio";

	// Minimum number of required iterations in order to obtain result
	const unsigned int minIterations = 2u;

	HighPerformanceStatistic performanceOcean;
	HighPerformanceStatistic performanceOpenCV;

	// Temporarily disable parallel execution in OpenCV
	cv::setNumThreads(0);

	const std::vector<std::pair<unsigned int, unsigned int>> performanceSourceImageSizes =
		{
			{ 128u, 128u },
			{ 256u, 256u },
			{ 512u, 512u },
			{ 640u, 480u },
			{ 1280u, 720u },
			{ 1920u, 1080u }
		};

	for (const std::pair<unsigned int, unsigned int>& performanceSourceImageSize : performanceSourceImageSizes)
	{
		unsigned int totalIteration = 0u;
		unsigned int totalIterationOcean = 0u;
		unsigned int totalIterationOpenCV = 0u;

		const Scalar clipLimit = Scalar(40);
		const unsigned int horizontalTiles = 8u;
		const unsigned int verticalTiles = 8u;
		ocean_assert(clipLimit > 0.0 && horizontalTiles != 0u && verticalTiles != 0u);

		const unsigned int performanceSourceImageWidth = performanceSourceImageSize.first;
		const unsigned int performanceSourceImageHeight = performanceSourceImageSize.second;
		ocean_assert(performanceSourceImageWidth % horizontalTiles == 0u && performanceSourceImageHeight % verticalTiles == 0u);
		ocean_assert(performanceSourceImageWidth != 0u && performanceSourceImageHeight != 0u);

		const Timestamp startTimePerformance(true);

		do
		{
			// Generate random test images, value range: [0, 255] as in the original
			Frame sourceImage = Frame(FrameType(performanceSourceImageWidth, performanceSourceImageHeight, FrameType::genericPixelFormat(dataType, channels), FrameType::ORIGIN_UPPER_LEFT));
			CV::CVUtilities::randomizeFrame(sourceImage, /* skipPaddingArea */ true, &randomGenerator);

			Frame oceanResult(sourceImage.frameType());
			CV::CVUtilities::randomizeFrame(oceanResult, /* skipPaddingArea */ true, &randomGenerator);

			if (totalIteration % minIterations == 0u)
			{
				// Ocean CLAHE
				performanceOcean.start();
				CV::ContrastLimitedAdaptiveHistogram::equalization8BitPerChannel(sourceImage.constdata<uint8_t>(), sourceImage.width(), sourceImage.height(), oceanResult.data<uint8_t>(), clipLimit, horizontalTiles, verticalTiles, sourceImage.paddingElements(), oceanResult.paddingElements(), nullptr /* worker - enforce single-core execution */);
				performanceOcean.stop();

				totalIterationOcean++;
			}
			else
			{
				// OpenCV CLAHE
				const cv::Mat cvSourceImage = CV::OpenCVUtilities::toCvMat(sourceImage, false);
				cv::Mat cvOpenCVResult;

				performanceOpenCV.start();
				cv::Ptr<cv::CLAHE> cvCLAHE = cv::createCLAHE(clipLimit, cv::Size((int)horizontalTiles, (int)verticalTiles));
				ocean_assert(cv::getNumThreads() == 1 && "Enforce single-core execution");
				cvCLAHE->apply(cvSourceImage, cvOpenCVResult);
				performanceOpenCV.stop();

				totalIterationOpenCV++;
			}

			totalIteration++;
		} while (totalIteration < minIterations || startTimePerformance + testDuration > Timestamp(true));

		std::ostringstream oss;
		oss << performanceSourceImageWidth << " x " << performanceSourceImageHeight << ", ";
		oss << totalIteration << ", ";

		ocean_assert_and_suppress_unused(totalIterationOcean != 0u, totalIterationOcean);
		ocean_assert_and_suppress_unused(totalIterationOpenCV != 0u, totalIterationOpenCV);

		oss << "[" << String::toAString(performanceOcean.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean.worstMseconds(), 3u) << "] ms, ";
		oss << "[" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms, ";
		oss << "[" << String::toAString(performanceOpenCV.best() / performanceOcean.best(), 2u) << ", " << String::toAString(performanceOpenCV.median() / performanceOcean.median(), 2u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOcean.worst(), 2u) << "] x";

		Log::info() << oss.str();
	}

	// Re-enable parallel execution in OpenCV
	cv::setNumThreads(-1 /* == default number of CPUs */);

	Log::info() << " ";

	// Validation
	bool validationSuccessful = true;

	for (unsigned int iter = 0u; iter < 2u; ++iter)
	{
		const bool imageSizeMultipleOfTiles = iter == 0u ? true : false;
		const double maxAllowedError = imageSizeMultipleOfTiles ? 10.0 : 40.0;
		bool iterValidationSuccessful = true;
		double maxMeasuredError = 0.0;

		const Timestamp startTimeValidation(true);

		do
		{
			const Scalar clipLimit = Random::scalar(Scalar(1.0), Scalar(100.0));
			const unsigned int horizontalTiles = RandomI::random(2u, 10u);
			const unsigned int verticalTiles = RandomI::random(2u, 10u);
			ocean_assert(clipLimit > 0.0 && horizontalTiles != 0u && verticalTiles != 0u);

			const unsigned int sourceImageWidth_ = RandomI::random(16u * horizontalTiles, 2000u);
			const unsigned int sourceImageHeight_ = RandomI::random(16u * verticalTiles, 2000u);
			const unsigned int sourceImageWidth = sourceImageWidth_ - (imageSizeMultipleOfTiles ? (sourceImageWidth_ % horizontalTiles) : 0u);
			const unsigned int sourceImageHeight = sourceImageHeight_ - (imageSizeMultipleOfTiles ? (sourceImageHeight_ % verticalTiles) : 0u);
			ocean_assert(imageSizeMultipleOfTiles == false || (sourceImageWidth % horizontalTiles == 0u && sourceImageHeight % verticalTiles == 0u));
			ocean_assert(sourceImageWidth != 0u && sourceImageHeight != 0u);

			// Generate random test images
			Frame randomImage = Frame(FrameType(sourceImageWidth, sourceImageHeight, FrameType::genericPixelFormat(dataType, channels), FrameType::ORIGIN_UPPER_LEFT));
			CV::CVUtilities::randomizeFrame(randomImage, /* skipPaddingArea */ true, &randomGenerator);

			Frame sourceImage(randomImage.frameType());
			ocean_assert(sourceImage.isContinuous() && randomImage.isContinuous());
			CV::FrameFilterGaussian::filter<uint8_t, uint32_t>(randomImage.constdata<uint8_t>(), sourceImage.data<uint8_t>(), randomImage.width(), randomImage.height(), 1u, randomImage.paddingElements(), sourceImage.paddingElements(), 3u, 3u, -1.0f, &worker);

			Frame oceanResult(sourceImage.frameType());
			CV::CVUtilities::randomizeFrame(oceanResult, /* skipPaddingArea */ true, &randomGenerator);

			CV::ContrastLimitedAdaptiveHistogram::equalization8BitPerChannel(sourceImage.constdata<uint8_t>(), sourceImage.width(), sourceImage.height(), oceanResult.data<uint8_t>(), clipLimit, horizontalTiles, verticalTiles, sourceImage.paddingElements(), oceanResult.paddingElements(), nullptr /* worker */);

			const cv::Mat cvSourceImage = CV::OpenCVUtilities::toCvMat(sourceImage, false);
			cv::Mat cvOpenCVResult;

			cv::Ptr<cv::CLAHE> cvCLAHE = cv::createCLAHE(clipLimit, cv::Size((int)horizontalTiles, (int)verticalTiles));
			cvCLAHE->apply(cvSourceImage, cvOpenCVResult);

			const cv::Mat cvOceanResult = CV::OpenCVUtilities::toCvMat(oceanResult, false);
			ocean_assert(cvOceanResult.size() == cvOpenCVResult.size());
			ocean_assert(cvOceanResult.type() == cvOpenCVResult.type());

			const unsigned int count = cvOpenCVResult.cols * cvOpenCVResult.rows;

			ocean_assert(oceanResult.isContinuous() && cvOpenCVResult.isContinuous());
			for (unsigned int i = 0u; i < count; ++i)
			{
				const double valueOcean = (double)oceanResult.constdata<unsigned char>()[i];
				const double valueOpenCV = (double)((unsigned char*)cvOpenCVResult.data)[i];
				const double error = std::abs(valueOcean - valueOpenCV);

				maxMeasuredError = std::max(error, maxMeasuredError);

				if (error > maxAllowedError)
				{
					iterValidationSuccessful = false;
				}
			}
		} 
		while (startTimeValidation + testDuration > Timestamp(true));

		Log::info() << "Validation (" << (imageSizeMultipleOfTiles ? "image size multiple of tiles count" : "image size random") << "): " << (iterValidationSuccessful ? "passed" : "FAILED");
		Log::info() << "Max. validation error: " << String::toAString(maxMeasuredError, 3u);
		Log::info() << " ";

		if (iterValidationSuccessful == false)
		{
			validationSuccessful = false;
		}
	}

	Log::info() << "Validation: " << (validationSuccessful ? "passed" : "FAILED");
	Log::info() << " ";

	return validationSuccessful;
}

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean
