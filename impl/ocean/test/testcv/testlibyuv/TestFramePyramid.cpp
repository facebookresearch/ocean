/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testlibyuv/TestFramePyramid.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FramePyramid.h"

#include <libyuv/scale_argb.h>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestLibyuv
{

void TestFramePyramid::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame pyramid benchmark:   ---";
	Log::info() << " ";

	testCreationFramePyramid(testDuration);

	Log::info() << " ";
	Log::info() << "Frame pyramid benchmark succeeded.";
}

void TestFramePyramid::testCreationFramePyramid(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Frame pyramid creation test:";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testCreationFramePyramid(1280u, 720u, n, testDuration);
		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testCreationFramePyramid(1920u, 1080u, n, testDuration);
		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << " ";

	for (unsigned int n = 1u; n <= 4u; ++n)
	{
		testCreationFramePyramid(3840u, 2160u, n, testDuration);
		Log::info() << " ";
	}

	Log::info() << "Frame pyramid creation test succeeded.";
}

void TestFramePyramid::testCreationFramePyramid(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(channels != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << width << "x" << height <<  ", " << channels << " channels:";
	Log::info() << " ";

	HighPerformanceStatistic performanceOceanSingleCore;
	HighPerformanceStatistic performanceOceanMultiCore;
	HighPerformanceStatistic performanceLibYUV;

	unsigned int iteration = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::findPixelFormat(channels * 8u), FrameType::ORIGIN_UPPER_LEFT));

		CV::FramePyramid framePyramid(CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, frame.frameType());

		if (iteration % 3u == 0u)
		{
			performanceOceanSingleCore.start();
				framePyramid.replace8BitPerChannel11(frame, framePyramid.layers(), true /*copyFirstLayer*/, nullptr);
			performanceOceanSingleCore.stop();
		}
		else if (iteration % 3u == 1u)
		{
			const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

			performanceOceanMultiCore.start();
				framePyramid.replace8BitPerChannel11(frame, framePyramid.layers(), true /*copyFirstLayer*/, scopedWorker());
			performanceOceanMultiCore.stop();
		}
		else if (channels == 4u) // libyuv does not provide a rescale function for Y frames, YA frames, or RGB frames
		{
			performanceLibYUV.start();

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				memcpy(framePyramid[0].row<void>(y), frame.constrow<void>(y), frame.planeWidthBytes(0u));
			}

			for (unsigned int n = 1u; n < framePyramid.layers(); ++n)
			{
				const Frame& finerLayer = framePyramid[n - 1u];
				Frame& coarserLayer = framePyramid[n];

				libyuv::ARGBScale(finerLayer.constdata<uint8_t>(), finerLayer.strideBytes(0u), finerLayer.width(), finerLayer.height(), coarserLayer.data<uint8_t>(), coarserLayer.strideBytes(0u), coarserLayer.width(), coarserLayer.height(), libyuv::kFilterBilinear);
			}

			performanceLibYUV.stop();
		}

		++iteration;
	}
	while (iteration < 3u || startTimestamp + testDuration > Timestamp(true));

	if (performanceLibYUV.measurements() != 0u)
	{
		Log::info() << "Performance libyuv: [" << String::toAString(performanceLibYUV.bestMseconds(), 3u) << ", " << String::toAString(performanceLibYUV.medianMseconds(), 3u) << ", " << String::toAString(performanceLibYUV.worstMseconds(), 3u) << "] ms";
		Log::info() << " ";
	}

	if (performanceOceanSingleCore.measurements() != 0u && performanceOceanMultiCore.measurements() != 0u)
	{
		Log::info() << "Performance Ocean (single-core): [" << String::toAString(performanceOceanSingleCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanSingleCore.worstMseconds(), 3u) << "] ms";
		Log::info() << "Performance Ocean (multi-core): [" << String::toAString(performanceOceanMultiCore.bestMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.medianMseconds(), 3u) << ", " << String::toAString(performanceOceanMultiCore.worstMseconds(), 3u) << "] ms";
		Log::info() << "Multi-core boost factor: [" << String::toAString(performanceOceanSingleCore.best() / performanceOceanMultiCore.best(), 1u) << ", " << String::toAString(performanceOceanSingleCore.median() / performanceOceanMultiCore.median(), 1u) << ", " << String::toAString(performanceOceanSingleCore.worst() / performanceOceanMultiCore.worst(), 1u) << "] x";
	}

	if (performanceLibYUV.measurements() != 0u && performanceOceanSingleCore.measurements() != 0u && performanceOceanMultiCore.measurements() != 0u)
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
