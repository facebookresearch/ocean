/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestGaussianBlur.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"
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

void TestGaussianBlur::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Gaussian blur test:   ---";
	Log::info() << " ";

	const unsigned int widths[] = {640u, 800u, 1280u, 1920u, 3840u};
	const unsigned int heights[] = {480u, 640u, 720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < sizeof(widths) / sizeof(widths[0]); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			testGaussianBlur(width, height, channel, testDuration);
			Log::info() << " ";
		}
	}

	Log::info() << " ";
	Log::info() << "Gaussian blur benchmark finished.";
}

void TestGaussianBlur::testGaussianBlur(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);
	ocean_assert(testDuration > 0.0);

	const Indices32 sizes = {3u, 5u, 7u, 11u, 15u};

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";
	Log::info() << " ";

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels);

	Frame sourceFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
	Frame targetFrame(sourceFrame.frameType());

	std::vector<unsigned char> backgroundColor(channels);

	cv::Mat cvSourceFrame(height, width, CV_MAKETYPE(CV_8U, channels));
	cv::Mat cvTargetFrame(height, width, CV_MAKETYPE(CV_8U, channels));

	for (const unsigned int size : sizes)
	{
		Log::info() << "... with kernel size " << size;

		HighPerformanceStatistic performanceOceanSingleCore;
		HighPerformanceStatistic performanceOceanMultiCore;
		HighPerformanceStatistic performanceOpenCV;

		unsigned int iteration = 0u;

		const Timestamp startTimestamp(true);

		do
		{
			CV::CVUtilities::randomizeFrame(sourceFrame);
			CV::CVUtilities::randomizeFrame(targetFrame);

			if (iteration % 2u == 0u)
			{
				performanceOceanSingleCore.start();
				CV::FrameFilterGaussian::filter(sourceFrame, targetFrame, size, nullptr);
				performanceOceanSingleCore.stop();

				const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

				performanceOceanMultiCore.start();
				CV::FrameFilterGaussian::filter(sourceFrame, targetFrame, size, scopedWorker());
				performanceOceanMultiCore.stop();
			}
			else
			{
				CV::OpenCVUtilities::toCvMat(sourceFrame).copyTo(cvSourceFrame);
				CV::OpenCVUtilities::toCvMat(targetFrame).copyTo(cvTargetFrame);

				const int intSize = int(size);
				const cv::Size cvFilterSize(intSize, intSize);

				performanceOpenCV.start();
				cv::GaussianBlur(cvSourceFrame, cvTargetFrame, cvFilterSize, 0.0, 0.0);
				performanceOpenCV.stop();
			}

			iteration++;
		}
		while (iteration < 2u || startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Performance OpenCV: [" << String::toAString(performanceOpenCV.bestMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.medianMseconds(), 3u) << ", " << String::toAString(performanceOpenCV.worstMseconds(), 3u) << "] ms";
		Log::info() << "Performance Ocean (single-core): [" << String::toAString(performanceOceanSingleCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.worstMseconds(), 3u) << "] ms";		Log::info() << "Performance Ocean (multi-core): [" << String::toAString(performanceOceanMultiCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.worstMseconds(), 3u) << "] ms,  Multi-core boost factor: [" << String::toAString(performanceOceanSingleCore.best() / performanceOceanMultiCore.best(), 1u) << ", " << String::toAString(performanceOceanSingleCore.median() / performanceOceanMultiCore.median(), 1u) << ", " << String::toAString(performanceOceanSingleCore.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";

		Log::info() << "Performance factor (single-core): [" << String::toAString(performanceOpenCV.best() / performanceOceanSingleCore.best(), 1u) << ", " << String::toAString(performanceOpenCV.median() / performanceOceanSingleCore.median(), 1u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOceanSingleCore.worst(), 1u) << "] x";
		Log::info() << "Performance factor (multi-core): [" << String::toAString(performanceOpenCV.best() / performanceOceanMultiCore.best(), 1u) << ", " << String::toAString(performanceOpenCV.median() / performanceOceanMultiCore.median(), 1u) << ", " << String::toAString(performanceOpenCV.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";

		Log::info() << " ";
	}
}

}

}

}

}
