/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameShrinkerAlpha.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameShrinkerAlpha.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameShrinkerAlpha::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Test Alpha Frame Shrinker:   ---";
	Log::info() << " ";

	allSucceeded = testFrameDivideByTwo(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Alpha Frame Shrinker test succeeded.";
	}
	else
	{
		Log::info() << "Alpha Frame Shrinker test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameShrinkerAlpha, FrameDivideByTwo)
{
	Worker worker;
	EXPECT_TRUE(TestFrameShrinkerAlpha::testFrameDivideByTwo(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameShrinkerAlpha::testFrameDivideByTwo(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing divide by two...";
	Log::info() << " ";

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		if (channels != 1u)
		{
			Log::info() << " ";
			Log::info() << " ";
		}

		allSucceeded = testFrameDivideByTwo(width, height, channels, false, false, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testFrameDivideByTwo(width, height, channels, false, true, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testFrameDivideByTwo(width, height, channels, true, false, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testFrameDivideByTwo(width, height, channels, true, true, testDuration, worker) && allSucceeded;
	}

	return allSucceeded;
}

bool TestFrameShrinkerAlpha::testFrameDivideByTwo(const unsigned int width, const unsigned int height, const unsigned int channels, const bool alphaAtFront, const bool transparentIs0xFF, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 2u && height >= 2u && channels >= 1u);
	ocean_assert(testDuration > 0.0);

	if (alphaAtFront)
	{
		Log::info() << "... for " << width << "x" << height << ", and " << channels << " channels, alpha front, and " << (transparentIs0xFF ? "0xFF as transparent" : "0x00 as transparent") << ":";
	}
	else
	{
		Log::info() << "... for " << width << "x" << height << ", and " << channels << " channels, alpha back, and " << (transparentIs0xFF ? "0xFF as transparent" : "0x00 as transparent") << ":";
	}

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 2u, width);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 2u, height);

				const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				Frame source(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame target(FrameType(source, source.width() / 2u, source.height() / 2u), targetPaddingElements);

				CV::CVUtilities::randomizeFrame(source, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);

				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);
					invokeFrameDivideByTwo(source, target, alphaAtFront, transparentIs0xFF, useWorker);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validationDivideByTwo(source, target, alphaAtFront, transparentIs0xFF))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms, median: " << performanceSinglecore.medianMseconds() << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms, median: " << performanceMulticore.medianMseconds() << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

void TestFrameShrinkerAlpha::invokeFrameDivideByTwo(const Frame& source, Frame& target, const bool alphaAtFront, const bool transparentIs0xFF, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());

	switch (source.channels())
	{
		case 1u:
		{
			if (alphaAtFront)
			{
				if (transparentIs0xFF)
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<1u, true, true>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<1u, true, false>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			}
			else
			{
				if (transparentIs0xFF)
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<1u, false, true>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<1u, false, false>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			}
			break;
		}

		case 2u:
		{
			if (alphaAtFront)
			{
				if (transparentIs0xFF)
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<2u, true, true>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<2u, true, false>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			}
			else
			{
				if (transparentIs0xFF)
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<2u, false, true>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<2u, false, false>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			}
			break;
		}

		case 3u:
		{
			if (alphaAtFront)
			{
				if (transparentIs0xFF)
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<3u, true, true>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<3u, true, false>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			}
			else
			{
				if (transparentIs0xFF)
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<3u, false, true>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<3u, false, false>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			}
			break;
		}

		case 4u:
		{
			if (alphaAtFront)
			{
				if (transparentIs0xFF)
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<4u, true, true>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<4u, true, false>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			}
			else
			{
				if (transparentIs0xFF)
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<4u, false, true>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					CV::FrameShrinkerAlpha::divideByTwo8BitPerChannel<4u, false, false>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
			}
			break;
		}

		default:
			ocean_assert(false && "Invalid channels!");
	}
}

bool TestFrameShrinkerAlpha::validationDivideByTwo(const Frame& source, const Frame& target, const bool alphaAtFront, const bool transparentIs0xFF)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.width() >= 2u && source.height() >= 2u);
	ocean_assert(target.width() == source.width() / 2u && target.height() == source.height() / 2u);

	if (!source.isValid() || !target.isValid() || source.width() < 2u || source.height() < 2u)
	{
		return false;
	}

	if (source.isFrameTypeCompatible(FrameType(target, source.width() / 2u, source.height() / 2u), false))
	{
		return false;
	}

	const unsigned int channels = source.channels();

	const unsigned int alphaOffset = alphaAtFront ? 0u : channels - 1u;
	const unsigned int dataOffset = alphaAtFront ? 1u : 0u;

	const bool threeEndingColumns = source.width() % 2u != 0u;
	const bool threeEndingRows = source.height() % 2u != 0u;

	const Indices32 factors[4] =
	{
		{}, // first two are not used
		{},
		{1u, 1u},
		{1u, 2u, 1u}
	};

	for (unsigned int ty = 0u; ty < target.height(); ++ty)
	{
		for (unsigned int tx = 0u; tx < target.width(); ++tx)
		{
			const unsigned int sx = tx * 2u;
			const unsigned int sy = ty * 2u;

			unsigned int xPatch = 2u;
			unsigned int yPatch = 2u;

			if (tx + 1u == target.width() && threeEndingColumns)
			{
				xPatch = 3u;
			}

			if (ty + 1u == target.height() && threeEndingRows)
			{
				yPatch = 3u;
			}

			unsigned int denominator = 0u;

			Indices32 values(channels, 0u);
			unsigned int sumFactors = 0u;

			for (unsigned int xx = 0u; xx < xPatch; ++xx)
			{
				for (unsigned int yy = 0u; yy < yPatch; ++yy)
				{
					const uint8_t* sourcePixel = source.constpixel<uint8_t>(sx + xx, sy + yy);

					uint8_t alpha = sourcePixel[alphaOffset];

					if (transparentIs0xFF)
					{
						alpha = 0xFF - alpha;
					}

					const unsigned int factor = factors[xPatch][xx] * factors[yPatch][yy];
					ocean_assert(factor >= 1u && factor <= 4u);

					for (unsigned int n = 0u; n < channels - 1u; ++n)
					{
						values[dataOffset + n] += sourcePixel[dataOffset + n] * factor * alpha;
					}

					values[alphaOffset] += sourcePixel[alphaOffset] * factor;

					denominator += factor * alpha;
					sumFactors += factor;
				}
			}

			if (denominator != 0u)
			{
				for (unsigned int n = 0u; n < channels - 1u; ++n)
				{
					values[dataOffset + n] = (values[dataOffset + n] + denominator / 2u) / denominator;
				}
			}

			values[alphaOffset] = (values[alphaOffset] + sumFactors / 2u) / sumFactors;

			const uint8_t* targetPixel = target.constpixel<uint8_t>(tx, ty);

			for (unsigned int n = 0u; n < channels; ++n)
			{
				ocean_assert(values[n] <= 255u);

				if (targetPixel[n] != uint8_t(values[n]))
				{
					return false;
				}
			}
		}
	}

	return true;
}

}

}

}
