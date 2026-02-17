/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterMean.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterMean.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterMean::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(width >= 21u && height >= 21u && testDuration > 0.0);

	TestResult testResult("Mean filter");

	Log::info() << " ";

	if (selector.shouldRun("filtersizearbitrary"))
	{
		testResult = testFilterSizeArbitrary(width, height, testDuration, worker);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << " ";

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterMean, testFilterSizeArbitrary_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMean::testFilterSizeArbitrary(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameFilterMean::testFilterSizeArbitrary(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u && testDuration > 0.0);

	Log::info() << "Testing mean filter with arbitrary size:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	for (unsigned int nChannels = 1u; nChannels <= 4u; ++nChannels)
	{
		Log::info() << " ";
		Log::info() << "... for " << nChannels << " channels";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				for (const bool performanceIteration : {true, false})
				{
					const unsigned int filterSize = RandomI::random(randomGenerator, 3u, 11u) | 0x01u;
					ocean_assert(filterSize % 2u == 1u);

					const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, filterSize, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, filterSize, height);

					const unsigned int sourcePaddingMultiplier = RandomI::random(randomGenerator, 1u);
					const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * sourcePaddingMultiplier;

					const unsigned int targetPaddingMultiplier = RandomI::random(randomGenerator, 1u);
					const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * targetPaddingMultiplier;

					Frame source(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source.frameType()), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);
					if (!CV::FrameFilterMean::filter(source, target, filterSize, useWorker))
					{
						OCEAN_SET_FAILED(validation);
					}
					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validationFilter8BitPerChannel(source.constdata<uint8_t>(), target.constdata<uint8_t>(), source.width(), source.height(), source.channels(), filterSize, filterSize, source.paddingElements(), target.paddingElements()))
					{
						OCEAN_SET_FAILED(validation);
					}
				}
			}
			while (!startTimestamp.hasTimePassed(testDuration));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
		}
	}

	Log::info() << " ";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFrameFilterMean::validationFilter8BitPerChannel(const uint8_t* source, const uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int filterWidth, const unsigned int filterHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);

	ocean_assert(filterWidth % 2u == 1u && filterHeight % 2u == 1u);

	const unsigned int filterWidth_2 = filterWidth / 2u;
	const unsigned int filterHeight_2 = filterHeight / 2u;

	const unsigned int sourceStrideElements = width * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = width * channels + targetPaddingElements;

	unsigned int validElements = 0u;

	Indices32 sumValues(channels);

	for (unsigned int y = 0u; y < height; ++y)
	{
		const uint8_t* const targetRow = target + y * targetStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			memset(sumValues.data(), 0, sizeof(Index32) * sumValues.size());

			unsigned int filterArea = 0u;

			for (unsigned int yy = (unsigned int)(max(0, int(y) - int(filterHeight_2))); yy < min(y + filterHeight_2 + 1u, height); ++yy)
			{
				const uint8_t* const sourceRow = source + yy * sourceStrideElements;

				for (unsigned int xx = (unsigned int)(max(0, int(x) - int(filterWidth_2))); xx < min(x + filterWidth_2 + 1u, width); ++xx)
				{
					for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
					{
						sumValues[channelIndex] += sourceRow[xx * channels + channelIndex];
					}

					++filterArea;
				}
			}

			ocean_assert(filterArea <= filterWidth * filterHeight);

			const uint8_t* const targetPixel = targetRow + x * channels;

			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				const unsigned int meanValue = (sumValues[channelIndex] + filterArea / 2u) / filterArea;
				ocean_assert(meanValue <= 255u);

				const unsigned int error = (unsigned int)(abs(int(meanValue) - int(targetPixel[channelIndex])));

				if (error <= 2u)
				{
					++validElements;
				}
				else if (x <= filterWidth_2 || y <= filterHeight_2 || x >= width - filterWidth_2 - 1u || y >= height - filterHeight_2 - 1u)
				{
					// we are close to the image border

					if (error <= 20u)
					{
						++validElements;
					}
				}
				else
				{
					// we are in the center of the image

					return false;
				}
			}
		}
	}

	const double percent = double(validElements) / double(width * height * channels);

	return percent >= 0.97;
}

}

}

}
