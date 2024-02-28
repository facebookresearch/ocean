// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testopencv/TestFrameFilterLaplace.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterLaplace.h"
#include "ocean/cv/OpenCVUtilities.h"

#include "ocean/test/testcv/TestFrameFilterLaplace.h"

#include <opencv2/imgproc.hpp>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

void TestFrameFilterLaplace::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Laplace filter test:   ---";
	Log::info() << " ";

	test1Channel8BitWith16BitResponse(testDuration);

	Log::info() << " ";

	Log::info() << "Laplace filter test finished.";
}

void TestFrameFilterLaplace::test1Channel8BitWith16BitResponse(const double testDuration)
{
	Log::info() << "Testing Laplace filter 'uint8_t' -> 'int16_t':";
	Log::info() << " ";

	const unsigned int widths[] = {160u, 320u, 640u, 800u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {120u, 240u, 480u, 640u, 720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < sizeof(widths) / sizeof(widths[0]); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		Log::info() << "For image resolution " << width << "x" << height << ":";
		Log::info() << " ";

		test1Channel8BitWith16BitResponse(width, height, testDuration);
		Log::info() << " ";
	}

	Log::info() << "Laplace filter 'uint8_t' -> 'int16_t' test finished.";
}

void TestFrameFilterLaplace::test1Channel8BitWith16BitResponse(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(testDuration > 0.0);

	cv::Mat cvSourceFrame;
	cv::Mat cvTargetFrame;

	unsigned int iteration = 0u;

	HighPerformanceStatistic performanceOcean;
	HighPerformanceStatistic performanceOpenCV;
	HighPerformanceStatistic performanceAML;

	unsigned int dummyValue = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame targetFrame(FrameType(width, height, FrameType::genericPixelFormat<int16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

		CV::CVUtilities::randomizeFrame(sourceFrame);
		CV::CVUtilities::randomizeFrame(targetFrame);

		if (iteration % 3u == 0u)
		{
			performanceOcean.start();
			CV::FrameFilterLaplace::filter1Channel8Bit(sourceFrame.constdata<uint8_t>(), targetFrame.data<int16_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements());
			performanceOcean.stop();

			dummyValue += targetFrame.constpixel<int16_t>(RandomI::random(0u, width - 1u), RandomI::random(0u, height - 1u))[0];
		}
		else if (iteration % 3u == 1u)
		{
			CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
			CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

			performanceAML.start();
			laplacian(cvSourceFrame, cvTargetFrame);
			performanceAML.stop();

			dummyValue += cvTargetFrame.ptr<int16_t>(RandomI::random(0, cvTargetFrame.size().height - 1), RandomI::random(0, cvTargetFrame.size().width - 1))[0];
		}
		else
		{
			ocean_assert(iteration % 3u == 2u);

			CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
			CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

			constexpr int kernelSize = 3;
			constexpr int targetDepth = CV_16S;

			performanceOpenCV.start();
			cv::Laplacian(cvSourceFrame, cvTargetFrame, targetDepth, kernelSize, 1, 0, cv::BORDER_DEFAULT);
			performanceOpenCV.stop();

			dummyValue += cvTargetFrame.ptr<int16_t>(RandomI::random(0, cvTargetFrame.size().height - 1), RandomI::random(0, cvTargetFrame.size().width - 1))[0];
		}

		iteration++;
	}
	while (iteration < 3u || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance OpenCV: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance AML: [" << String::toAString(performanceAML.bestMseconds(), 3u) << ", " << String::toAString(performanceAML.medianMseconds(), 3u) << ", " << String::toAString(performanceAML.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean: [" << String::toAString(performanceOcean.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean.worstMseconds(), 3u) << "] ms";

	if (dummyValue == 0u)
	{
		Log::info() << "Ocean vs. OpenCV:";
	}
	else
	{
		Log::info() << "Ocean vs. OpenCV:";
	}

	Log::info() << "Performance ratio: [" << String::toAString(performanceOpenCV.best() / performanceOcean.best(), 2u) << ", " << String::toAString(performanceOpenCV.median() / performanceOcean.median(), 2u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOcean.worst(), 2u) << "] x";

	Log::info() << "Ocean vs. AML:";
	Log::info() << "Performance ratio: [" << String::toAString(performanceAML.best() / performanceOcean.best(), 2u) << ", " << String::toAString(performanceAML.median() / performanceOcean.median(), 2u) << ", " << String::toAString(performanceAML.worst() / performanceOcean.worst(), 2u) << "] x";
}

// The following code is copied form AML's FaceTracker
// Source: https://phabricator.intern.facebook.com/diffusion/FBS/browse/master/xplat/aml/fsid/classifier/ClassifierHelpers.cpp
// e.g., D8244272

void TestFrameFilterLaplace::laplacian(const cv::Mat& src, cv::Mat& dst)
{
  cv::Mat mat_16s;
  src.convertTo(mat_16s, CV_16SC1);
  int laplacian_cols = mat_16s.cols - 2;
  int laplacian_rows = mat_16s.rows - 2;

  cv::Mat filter_1_3;
  cv::Mat filter_1_3_5;
  cv::Mat filter_1_3_5_7;

  cv::add(
      mat_16s(cv::Rect(1, 0, laplacian_cols, laplacian_rows)),
      mat_16s(cv::Rect(0, 1, laplacian_cols, laplacian_rows)),
      filter_1_3);
  cv::add(
      filter_1_3,
      mat_16s(cv::Rect(2, 1, laplacian_cols, laplacian_rows)),
      filter_1_3_5);
  cv::add(
      filter_1_3_5,
      mat_16s(cv::Rect(1, 2, laplacian_cols, laplacian_rows)),
      filter_1_3_5_7);
  cv::addWeighted(
      filter_1_3_5_7,
      1.00,
      mat_16s(cv::Rect(1, 1, laplacian_cols, laplacian_rows)),
      -4.00,
      0.00,
      dst);
}

}

}

}

}
