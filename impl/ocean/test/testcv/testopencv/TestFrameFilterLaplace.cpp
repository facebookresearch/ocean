/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestFrameFilterLaplace.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterLaplace.h"
#include "ocean/cv/OpenCVUtilities.h"

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

	RandomGenerator randomGenerator;

	cv::Mat cvSourceFrame;
	cv::Mat cvTargetFrame;

	unsigned int iteration = 0u;

	HighPerformanceStatistic performanceOcean;
	HighPerformanceStatistic performanceOpenCV;

	unsigned int dummyValue = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		Frame targetFrame(FrameType(width, height, FrameType::genericPixelFormat<int16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));

		CV::CVUtilities::randomizeFrame(sourceFrame, /* skipPaddingArea */ true, &randomGenerator);
		CV::CVUtilities::randomizeFrame(targetFrame, /* skipPaddingArea */ true, &randomGenerator);

		const bool runOpenCV = iteration % 2u == 0u;

		if (runOpenCV)
		{
			CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
			CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

			constexpr int kernelSize = 3;
			constexpr int targetDepth = CV_16S;

			performanceOpenCV.start();
			cv::Laplacian(cvSourceFrame, cvTargetFrame, targetDepth, kernelSize, 1, 0, cv::BORDER_DEFAULT);
			performanceOpenCV.stop();

			dummyValue += cvTargetFrame.ptr<int16_t>(RandomI::random(0, cvTargetFrame.size().height - 1), RandomI::random(0, cvTargetFrame.size().width - 1))[0];
		}
		else
		{
			performanceOcean.start();
			CV::FrameFilterLaplace::filter1Channel8Bit(sourceFrame.constdata<uint8_t>(), targetFrame.data<int16_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements());
			performanceOcean.stop();

			dummyValue += targetFrame.constpixel<int16_t>(RandomI::random(0u, width - 1u), RandomI::random(0u, height - 1u))[0];
		}

		iteration++;
	}
	while (iteration < 3u || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance OpenCV: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
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
}

}

}

}

}
