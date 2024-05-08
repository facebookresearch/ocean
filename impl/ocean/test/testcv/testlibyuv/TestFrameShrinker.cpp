/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testlibyuv/TestFrameShrinker.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameShrinker.h"

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

void TestFrameShrinker::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame shrinker benchmark:   ---";
	Log::info() << " ";

	testDivideByTwo(testDuration);

	Log::info() << " ";
	Log::info() << "Frame shrinker benchmark succeeded.";
}

void TestFrameShrinker::testDivideByTwo(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test divide by two:";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testDivideByTwo(1280u, 720u, n, testDuration);
		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testDivideByTwo(1920u, 1080u, n, testDuration);
		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testDivideByTwo(3840u, 2160u, n, testDuration);
		Log::info() << " ";
	}

	Log::info() << "Resize test succeeded.";
}

void TestFrameShrinker::testDivideByTwo(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int channels, const double testDuration)
{
	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(channels != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... dividing by two " << sourceWidth << "x" << sourceHeight << ", " << channels << " channels:";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	double oceanSumAverageError = 0.0;
	unsigned int oceanMaxError = 0u;
	unsigned long long oceanIterations = 0ull;

	double libyuvSumAverageError = 0.0;
	unsigned int libyuvMaxError = 0u;
	unsigned long long libyuvIterations = 0ull;

	HighPerformanceStatistic performanceOceanSingleCore;
	HighPerformanceStatistic performanceOceanMultiCore;
	HighPerformanceStatistic performanceLibYUV;

	unsigned int iteration = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int soucePaddingelements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int targetPaddingelements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame sourceFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), soucePaddingelements);
		Frame targetFrame(FrameType(sourceFrame, sourceWidth / 2u, sourceHeight / 2u), targetPaddingelements);

		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

		if (iteration % 2u == 0u)
		{
			performanceOceanSingleCore.start();
			CV::FrameShrinker::downsampleByTwo11(sourceFrame, targetFrame);
			performanceOceanSingleCore.stop();

			double averageError = 0.0;
			unsigned int maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorBilinear::validateScaleFrame(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), 2.0, 2.0, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageError, &maximalError);

			oceanSumAverageError += averageError;
			oceanMaxError = max(oceanMaxError, maximalError);
			oceanIterations++;


			const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

			performanceOceanMultiCore.start();
			CV::FrameShrinker::downsampleByTwo11(sourceFrame, targetFrame, scopedWorker());
			performanceOceanMultiCore.stop();

			averageError = 0.0;
			maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorBilinear::validateScaleFrame(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), 2.0, 2.0, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageError, &maximalError);

			oceanSumAverageError += averageError;
			oceanMaxError = max(oceanMaxError, maximalError);
			oceanIterations++;
		}
		else if (channels == 4u) // libyuv does not provide a rescale function for Y frames, YA frames, or RGB frames
		{
			performanceLibYUV.start();
			libyuv::ARGBScale(sourceFrame.constdata<uint8_t>(), sourceFrame.strideBytes(0u), sourceFrame.width(), sourceFrame.height(), targetFrame.data<uint8_t>(), targetFrame.strideBytes(0u), targetFrame.width(), targetFrame.height(), libyuv::kFilterBilinear);
			performanceLibYUV.stop();

			double averageError = 0.0;
			unsigned int maximalError = 0u;
			Test::TestCV::TestFrameInterpolatorBilinear::validateScaleFrame(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), 2.0, 2.0, sourceFrame.paddingElements(), targetFrame.paddingElements(), &averageError, &maximalError);

			libyuvSumAverageError += averageError;
			libyuvMaxError = max(libyuvMaxError, maximalError);
			libyuvIterations++;
		}

		iteration++;
	}
	while (iteration < 2u || startTimestamp + testDuration > Timestamp(true));

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
		Log::info() << "Multi-core boost factor: [" << String::toAString(performanceOceanSingleCore.best() / performanceOceanMultiCore.best(), 1u) << ", " << String::toAString(performanceOceanSingleCore.median() / performanceOceanMultiCore.median(), 1u) << ", " << String::toAString(performanceOceanSingleCore.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";
		Log::info() << "Validation: average error " << String::toAString(double(oceanSumAverageError) / double(oceanIterations), 2u) << ", maximal error: " << oceanMaxError;
	}

	if (oceanIterations != 0ull && libyuvIterations != 0ull)
	{
		Log::info() << " ";
		Log::info() << "Performance factor (single-core): [" << String::toAString(performanceLibYUV.best() / performanceOceanSingleCore.best(), 1u) << ", " << String::toAString(performanceLibYUV.median() / performanceOceanSingleCore.median(), 1u) << ", " << String::toAString(performanceLibYUV.worst() / performanceOceanSingleCore.worst(), 1u) << "] x";
		Log::info() << "Performance factor (multi-core): [" << String::toAString(performanceLibYUV.best() / performanceOceanMultiCore.best(), 1u) << ", " << String::toAString(performanceLibYUV.median() / performanceOceanMultiCore.median(), 1u) << ", " << String::toAString(performanceLibYUV.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";
	}
}

}

}

}

}
