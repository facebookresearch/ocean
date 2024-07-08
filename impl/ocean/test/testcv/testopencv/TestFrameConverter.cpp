/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestFrameConverter.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/OpenCVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

void TestFrameConverter::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame converter test:   ---";
	Log::info() << " ";

	testCast(testDuration);

	Log::info() << " ";

	Log::info() << "Frame converter test finished.";
}

void TestFrameConverter::testCast(const double testDuration)
{
	Log::info() << "Testing frame cast 'unsigned char' to 'float':";
	Log::info() << " ";

	const unsigned int widths[] = {160u, 320u, 640u, 800u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {120u, 240u, 480u, 640u,  720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < sizeof(widths) / sizeof(widths[0]); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		Log::info() << "For image resolution " << width << "x" << height << ":";
		Log::info() << " ";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			testCast(width, height, channels, testDuration);
			Log::info() << " ";
		}
	}

	Log::info() << "Frame cast test finished.";
}

void TestFrameConverter::testCast(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(channels >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << channels << " channels:";
	Log::info() << " ";

  RandomGenerator randomGenerator;

	Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT));
	Frame targetFrame(FrameType(sourceFrame, FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, channels)));

	cv::Mat cvSourceFrame;
	cv::Mat cvTargetFrame;

	unsigned int iteration = 0u;

	HighPerformanceStatistic performanceOcean;
	HighPerformanceStatistic performanceOpenCV;

	const Timestamp startTimestamp(true);

	do
	{
		CV::CVUtilities::randomizeFrame(sourceFrame, /* skipPaddingArea */ true, &randomGenerator);
		CV::CVUtilities::randomizeFrame(targetFrame, /* skipPaddingArea */ true, &randomGenerator);

    const bool runOpenCV = iteration % 2u == 0u;

		if (runOpenCV)
		{
			CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
			CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

			performanceOpenCV.start();
			cvSourceFrame.convertTo(cvTargetFrame, CV_MAKETYPE(CV_32F, channels));
			performanceOpenCV.stop();
		}
    else
		{
			performanceOcean.start();
			CV::FrameConverter::cast<uint8_t, float>(sourceFrame.constdata<uint8_t>(), targetFrame.data<float>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), sourceFrame.paddingElements(), targetFrame.paddingElements());
			performanceOcean.stop();
		}

		iteration++;
	}
	while (iteration < 3u || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance OpenCV: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
	Log::info() << "Performance Ocean: [" << String::toAString(performanceOcean.bestMseconds(), 3u) << ", " << String::toAString(performanceOcean.medianMseconds(), 3u) << ", " << String::toAString(performanceOcean.worstMseconds(), 3u) << "] ms";

	Log::info() << "Ocean vs. OpenCV:";
	Log::info() << "Performance ratio: [" << String::toAString(performanceOpenCV.best() / performanceOcean.best(), 2u) << ", "<< String::toAString(performanceOpenCV.median() / performanceOcean.median(), 2u) << ", "<< String::toAString(performanceOpenCV.worst() / performanceOcean.worst(), 2u) << "] x";
}

}

}

}

}
