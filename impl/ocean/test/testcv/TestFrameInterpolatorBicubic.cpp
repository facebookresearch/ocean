/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameInterpolatorBicubic.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameInterpolatorBicubic.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameInterpolatorBicubic::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Bicubic interpolation test:   ---";
	Log::info() << " ";

	bool result = true;

	result = testResize8BitPerChannel<1u>(width, height, 0.75f, testDuration, worker) && result;
	Log::info() << " ";
	result = testResize8BitPerChannel<1u>(width, height, 2.5f, testDuration, worker) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testResize8BitPerChannel<2u>(width, height, 0.75f, testDuration, worker) && result;
	Log::info() << " ";
	result = testResize8BitPerChannel<2u>(width, height, 2.5f, testDuration, worker) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testResize8BitPerChannel<3u>(width, height, 0.75f, testDuration, worker) && result;
	Log::info() << " ";
	result = testResize8BitPerChannel<3u>(width, height, 2.5f, testDuration, worker) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testResize8BitPerChannel<4u>(width, height, 0.75f, testDuration, worker) && result;
	Log::info() << " ";
	result = testResize8BitPerChannel<4u>(width, height, 2.5f, testDuration, worker) && result;

	Log::info() << " ";

	if (result)
		Log::info() << "Bicubic interpolation test succeeded.";
	else
		Log::info() << "Bicubic interpolation test FAILED!";

	return result;
}

template <unsigned int tChannels>
bool TestFrameInterpolatorBicubic::testResize8BitPerChannel(const unsigned int width, const unsigned int height, const float sizeFactor, const double testDuration, Worker& worker)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(sizeFactor > 0.0f);

	Log::info() << "Testing " << tChannels << " channels resizing frame with scaling factor " << String::toAString(sizeFactor, 1u) << "x:";

	RandomGenerator randomGenerator;

	const unsigned int targetWidth = (unsigned int)(float(width) * sizeFactor);
	const unsigned int targetHeight = (unsigned int)(float(height) * sizeFactor);

	HighPerformanceStatistic performanceSingleCore;
	HighPerformanceStatistic performanceMultiCore;

	for (const bool useWorker : {true, false})
	{
		const Timestamp startTimestamp(true);

		Worker* workerPtr = useWorker ? &worker : nullptr;
		HighPerformanceStatistic& performance = useWorker ? performanceMultiCore : performanceSingleCore;

		do
		{
			const unsigned sourcePaddingElements = RandomI::random(randomGenerator, 1u) == 0u ? RandomI::random(randomGenerator, 1u, 128u) : 0u;
			const unsigned targetPaddingElements = RandomI::random(randomGenerator, 1u) == 0u ? RandomI::random(randomGenerator, 1u, 128u) : 0u;

			Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
			Frame targetFrame(FrameType(sourceFrame, targetWidth, targetHeight), targetPaddingElements);

			CV::CVUtilities::randomizeFrame(sourceFrame, /* skipPaddingArea */ false, &randomGenerator);
			CV::CVUtilities::randomizeFrame(targetFrame, /* skipPaddingArea */ false, &randomGenerator);

			const Frame targetFrameClone(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			performance.start();
			CV::FrameInterpolatorBicubic::Comfort::resize(sourceFrame, targetFrame, workerPtr);
			performance.stop();

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, targetFrameClone))
			{
				ocean_assert(false && "This must never happen!");
				return false;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true) && performance.measurements() == 0u);
	}

	Log::info() << "Performance (single-core): Best: " << performanceSingleCore.bestMseconds() << "ms, worst: " << performanceSingleCore.worstMseconds() << "ms, average: " << performanceSingleCore.averageMseconds() << "ms";
	Log::info() << "Performance (multi-core): Best: " << performanceMultiCore.bestMseconds() << "ms, worst: " << performanceMultiCore.worstMseconds() << "ms, average: " << performanceMultiCore.averageMseconds() << "ms";
	Log::info() << "Multicore boost: Best: " << performanceSingleCore.best() / performanceMultiCore.best() << ", worst: " << performanceSingleCore.worst() / performanceMultiCore.worst() << ", average: " << performanceSingleCore.average() / performanceMultiCore.average();

	return true;
}

}

}

}
