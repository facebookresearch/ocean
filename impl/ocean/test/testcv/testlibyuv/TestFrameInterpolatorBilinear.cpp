/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testlibyuv/TestFrameInterpolatorBilinear.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/test/testcv/TestFrameInterpolatorBilinear.h"

#include <libyuv/scale_argb.h>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestLibyuv
{

void TestFrameInterpolatorBilinear::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame interpolation benchmark:   ---";
	Log::info() << " ";

	testResize(testDuration);

	Log::info() << " ";
	Log::info() << "Frame interpolation benchmark succeeded.";
}

void TestFrameInterpolatorBilinear::testResize(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test resize:";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testResize(1920u, 1080u, n, 192u, 192u, testDuration);
		Log::info() << " ";
	}

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testResize(1280u, 720u, n, 1155u, 691u, testDuration);
		Log::info() << " ";
	}

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testResize(1280u, 720u, n, 1317u, 788u, testDuration);
		Log::info() << " ";
	}

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testResize(1920u, 1080u, n, 1400u, 1000u, testDuration);
		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testResize(1920u, 1080u, n, 1000u, 900u, testDuration);
		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testResize(1920u, 1080u, n, 500u, 800u, testDuration);
		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testResize(3840u, 2160u, n, 1500u, 1500u, testDuration);
		Log::info() << " ";
	}

	Log::info() << "Resize test succeeded.";
}

void TestFrameInterpolatorBilinear::testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration)
{
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);
	ocean_assert(channels != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... resizing " << sourceWidth << "x" << sourceHeight << " -> " << targetWidth << "x" << targetHeight << ", " << channels << " channels:";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	const double xTargetToSource = double(sourceWidth) / double(targetWidth);
	const double yTargetToSource = double(sourceHeight) / double(targetHeight);

	double oceanSumAverageError = 0.0;
	unsigned int oceanMaxError = 0u;
	unsigned long long oceanIterations = 0ull;

	double libyuvSumAverageError = 0.0;
	unsigned int libyuvMaxError = 0u;
	unsigned long long libyuvIterations = 0ull;

	HighPerformanceStatistic performanceOceanSingleCore;
	HighPerformanceStatistic performanceOceanMultiCore;
	HighPerformanceStatistic performanceLibYUV;

	for (const bool testLibYuv : {false, true})
	{
		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);
			const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);

			Frame sourceFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
			Frame targetFrame(FrameType(targetWidth, targetHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

			CV::CVUtilities::randomizeFrame(sourceFrame, /* skipPaddingArea */ false, &randomGenerator);
			CV::CVUtilities::randomizeFrame(targetFrame, /* skipPaddingArea */ false, &randomGenerator);

			const Frame clonedTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			if (!testLibYuv)
			{
				performanceOceanSingleCore.start();
				CV::FrameInterpolatorBilinear::Comfort::resize(sourceFrame, targetFrame);
				performanceOceanSingleCore.stop();

				double averageError = 0.0;
				unsigned int maximalError = 0u;
				Test::TestCV::TestFrameInterpolatorBilinear::validateScaleFrame(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageError, &maximalError);

				oceanSumAverageError += averageError;
				oceanMaxError = max(oceanMaxError, maximalError);
				oceanIterations++;


				const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

				performanceOceanMultiCore.start();
				CV::FrameInterpolatorBilinear::Comfort::resize(sourceFrame, targetFrame, scopedWorker());
				performanceOceanMultiCore.stop();

				averageError = 0.0;
				maximalError = 0u;
				Test::TestCV::TestFrameInterpolatorBilinear::validateScaleFrame(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageError, &maximalError);

				oceanSumAverageError += averageError;
				oceanMaxError = max(oceanMaxError, maximalError);
				oceanIterations++;
			}
			else
			{
				// libyuv does not provide a rescale function for YA frames, or RGB frames (i.e. 2 or 3 channel frames)

				if (channels == 1u)
				{
					performanceLibYUV.start();
					libyuv::ScalePlane(sourceFrame.constdata<uint8_t>(), sourceFrame.strideBytes(), sourceFrame.width(), sourceFrame.height(), targetFrame.data<uint8_t>(), targetFrame.strideBytes(), targetFrame.width(), targetFrame.height(), libyuv::kFilterBilinear);
					performanceLibYUV.stop();

					double averageError = 0.0;
					unsigned int maximalError = 0u;
					Test::TestCV::TestFrameInterpolatorBilinear::validateScaleFrame(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageError, &maximalError);

					libyuvSumAverageError += averageError;
					libyuvMaxError = max(libyuvMaxError, maximalError);
					libyuvIterations++;
				}
				else if (channels == 4u)
				{
					performanceLibYUV.start();
					libyuv::ARGBScale(sourceFrame.constdata<uint8_t>(), sourceFrame.strideBytes(), sourceFrame.width(), sourceFrame.height(), targetFrame.data<uint8_t>(), targetFrame.strideBytes(), targetFrame.width(), targetFrame.height(), libyuv::kFilterBilinear);
					performanceLibYUV.stop();

					double averageError = 0.0;
					unsigned int maximalError = 0u;
					Test::TestCV::TestFrameInterpolatorBilinear::validateScaleFrame(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), xTargetToSource, yTargetToSource, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageError, &maximalError);

					libyuvSumAverageError += averageError;
					libyuvMaxError = max(libyuvMaxError, maximalError);
					libyuvIterations++;
				}
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, clonedTargetFrame))
			{
				Log::error() << "Invalid padding memory - risk of memory corruption! Aborting immediately!";
				ocean_assert(false && "This should never happen!");

				return;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	if (libyuvIterations != 0ull)
	{
		Log::info() << "Performance libyuv: [" << String::toAString(performanceLibYUV.bestMseconds(), 3u) << ", " << String::toAString(performanceLibYUV.medianMseconds(), 3u) << ", " << String::toAString(performanceLibYUV.worstMseconds(), 3u) << "] ms";
		Log::info() << "Validation: average error " << String::toAString(double(libyuvSumAverageError) / double(libyuvIterations), 2u) << ", maximal error: " << libyuvMaxError;
		Log::info() << " ";
	}

	if (oceanIterations != 0ull)
	{
		Log::info() << "Performance Ocean (single-core): [" << String::toAString(performanceOceanSingleCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.worstMseconds(), 3u) << "] ms";
		Log::info() << "Performance Ocean (multi-core): [" << String::toAString(performanceOceanMultiCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.worstMseconds(), 3u) << "] ms";
		Log::info() << "Multi-core boost factor: [" << String::toAString(performanceOceanSingleCore.best() / performanceOceanMultiCore.best(), 2u) << ", " << String::toAString(performanceOceanSingleCore.median() / performanceOceanMultiCore.median(), 2u) << ", " << String::toAString(performanceOceanSingleCore.worst() / performanceOceanMultiCore.worst(), 2u) << "] x";
		Log::info() << "Validation: average error " << String::toAString(double(oceanSumAverageError) / double(oceanIterations), 2u) << ", maximal error: " << oceanMaxError;
	}

	if (oceanIterations != 0ull && libyuvIterations != 0ull)
	{
		Log::info() << " ";
		Log::info() << "Performance factor (single-core): [" << String::toAString(performanceLibYUV.best() / performanceOceanSingleCore.best(), 2u) << ", " << String::toAString(performanceLibYUV.median() / performanceOceanSingleCore.median(), 2u) << ", " << String::toAString(performanceLibYUV.worst() / performanceOceanSingleCore.worst(), 2u) << "] x";
		Log::info() << "Performance factor (multi-core): [" << String::toAString(performanceLibYUV.best() / performanceOceanMultiCore.best(), 2u) << ", " << String::toAString(performanceLibYUV.median() / performanceOceanMultiCore.median(), 2u) << ", " << String::toAString(performanceLibYUV.worst() / performanceOceanMultiCore.worst(), 2u) << "] x";
	}
}

}

}

}

}
