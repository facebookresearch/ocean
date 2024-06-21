/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestFrameFilterCanny.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterCanny.h"
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

void TestFrameFilterCanny::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Canny edge detection test:   ---";
	Log::info() << " ";

	filterCannySobel(testDuration);

	Log::info() << " ";

	Log::info() << "Canny edge detection test finished.";
}

void TestFrameFilterCanny::filterCannySobel(const double testDuration)
{
	Log::info() << "Testing Canny edge detection (8-bit, 1 channel)";
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

		filterCannySobel(width, height, testDuration);
		Log::info() << " ";
	}

	Log::info() << "Canny edge detection (8-bit, 1 channel) test finished.";
}

void TestFrameFilterCanny::filterCannySobel(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(testDuration > 0.0);

	Worker worker;
	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceOcean;
	HighPerformanceStatistic performanceOpenCV;

	unsigned int iterations = 0u;
	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);

		Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
		Frame targetFrame(FrameType(width, height, FrameType::genericPixelFormat<int16_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

		CV::CVUtilities::randomizeFrame(sourceFrame, /* skipPaddingArea */ false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(targetFrame, /* skipPaddingArea */ false, &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const int16_t lowThreshold = int16_t(RandomI::random(randomGenerator, 0u, 1019u));
		const int16_t highThreshold = int16_t(RandomI::random(randomGenerator, lowThreshold + 1u, 1020u));
		ocean_assert(lowThreshold < highThreshold && highThreshold <= 1020u);

		const unsigned int opencvImplementationIteration = iterations % 2u;

		for (unsigned int implementation = 0u; implementation < 2u; ++implementation)
		{
			if (implementation == opencvImplementationIteration)
			{
				const cv::Mat cvSourceFrame = CV::OpenCVUtilities::toCvMat(sourceFrame, /* copy */ false);
				cv::Mat cvTargetFrame = CV::OpenCVUtilities::toCvMat(targetFrame, /* copy */ false);

				performanceOpenCV.start();
				cv::Canny(cvSourceFrame, cvTargetFrame, double(lowThreshold), double(highThreshold), /* aperture size */ 3, /* L2 gradient */ true);
				performanceOpenCV.stop();
			}
			else
			{
				performanceOcean.start();
				CV::FrameFilterCanny::filterCannySobel(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), lowThreshold, highThreshold, &worker);
				performanceOcean.stop();
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
				Log::error() << "Invalid padding memory!";

				return;
			}
		}

		iterations++;
	}
	while (iterations < 2u || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: best/worst/average/median in ms";
	Log::info() << "  OpenCV: " << String::toAString(performanceOpenCV.bestMseconds(), 3u) << " / " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << " / " << String::toAString(performanceOpenCV.averageMseconds(), 3u) << " / " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << " ms";
	Log::info() << "  Ocean: " << String::toAString(performanceOcean.bestMseconds(), 3u) << " / " << String::toAString(performanceOcean.worstMseconds(), 3u) << " / " << String::toAString(performanceOcean.averageMseconds(), 3u) << " / " << String::toAString(performanceOcean.medianMseconds(), 3u) << " ms";
	Log::info() << " ";
	Log::info() << "Performance improvement: " << String::toAString(performanceOpenCV.best() / performanceOcean.best(), 2u) << " / " << String::toAString(performanceOpenCV.worst() / performanceOcean.worst(), 2u) << " / " << String::toAString(performanceOpenCV.average() / performanceOcean.average(), 2u) << " / " << String::toAString(performanceOpenCV.median() / performanceOcean.median(), 2u);
	Log::info() << " ";
}

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean
