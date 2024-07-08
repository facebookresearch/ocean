/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterLaplace.h"

#include "ocean/base/DataType.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterLaplace.h"

#include "ocean/math/Variance.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterLaplace::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width >= 3u && height >= 3u);

	Log::info() << "---   Laplace filter test with frame size " << width << "x" << height << ":   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = test1Channel<uint8_t, int8_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = test1Channel<uint8_t, int16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = test1Channel<uint8_t, uint16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testVariance1Channel(width, height, testDuration);

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Laplace filter test succeeded.";
	}
	else
	{
		Log::info() << "Laplace filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterLaplace, Filter1Channel_uint8_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterLaplace::test1Channel<uint8_t, int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterLaplace, Filter1Channel_uint8_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterLaplace::test1Channel<uint8_t, int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterLaplace, Filter1Channel_uint8_uint16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterLaplace::test1Channel<uint8_t, uint16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterLaplace, Variance1Channel)
{
	EXPECT_TRUE((TestFrameFilterLaplace::testVariance1Channel(1920u, 1080u, GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

template <typename T, typename TResponse>
bool TestFrameFilterLaplace::test1Channel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(testDuration >= 0.0);

	Log::info() << "Testing '" << TypeNamer::name<T>() << "' -> '" << TypeNamer::name<TResponse>() << "':";

	bool allSucceeded = true;

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
			for (bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, 500u);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, 500u);

				const unsigned int framePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
				const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

				Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
				Frame target(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<TResponse, 1u>(), FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

				CV::CVUtilities::randomizeFrame(frame, false);
				CV::CVUtilities::randomizeFrame(target, false);

				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);

					if constexpr (std::is_signed<TResponse>::value)
					{
						CV::FrameFilterLaplace::filter1Channel8Bit(frame.constdata<T>(), target.data<TResponse>(), frame.width(), frame.height(), frame.paddingElements(), target.paddingElements(), useWorker);
					}
					else
					{
						CV::FrameFilterLaplace::filterMagnitude1Channel8Bit(frame.constdata<T>(), target.data<TResponse>(), frame.width(), frame.height(), frame.paddingElements(), target.paddingElements(), useWorker);
					}

				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				constexpr bool borderPixelZero = std::is_signed<TResponse>::value;

				if (!validate<T, TResponse>(frame, target, borderPixelZero))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms, first: " << String::toAString(performanceSinglecore.firstMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms, first: " << String::toAString(performanceMulticore.firstMseconds(), 2u) << "ms";

		Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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

bool TestFrameFilterLaplace::testVariance1Channel(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(testDuration >= 0.0);

	Log::info() << "Testing variance for 1-channel frame " << width << "x" << height << ":";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performance;

	Timestamp startTimestamp(true);

	do
	{
		for (bool performanceIteration : {true, false})
		{
			unsigned int testWidth = width;
			unsigned int testHeight = height;

			if (!performanceIteration)
			{
				testWidth = RandomI::random(randomGenerator, 3u, 33026u);

				const unsigned int maxHeight = std::max(3u, 2000u * 2000u / testWidth); // ensuring that the test image is not too large
				testHeight = RandomI::random(randomGenerator, 3u, maxHeight);
			}

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			performance.startIf(performanceIteration);
				const double variance = CV::FrameFilterLaplace::variance1Channel8Bit(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements());
			performance.stopIf(performanceIteration);

			VarianceD varianceObject;

			for (unsigned int y = 1u; y < frame.height() - 1u; ++y)
			{
				for (unsigned int x = 1u; x < frame.width() - 1u; ++x)
				{
					const double pixelResponse = double(frame.constpixel<uint8_t>(x, y)[0]) * 4.0 - double(frame.constpixel<uint8_t>(x, y - 1u)[0]) - double(frame.constpixel<uint8_t>(x - 1u, y)[0]) - double(frame.constpixel<uint8_t>(x + 1u, y)[0]) - double(frame.constpixel<uint8_t>(x, y + 1u)[0]);

					varianceObject.add(pixelResponse);
				}
			}

			const double testVariance = varianceObject.variance();

			if (NumericD::isNotEqual(variance, testVariance, 0.01))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 2u) << "ms, average: " << String::toAString(performance.averageMseconds(), 2u) << "ms, first: " << String::toAString(performance.firstMseconds(), 2u) << "ms";

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

template <typename T, typename TResponse>
bool TestFrameFilterLaplace::validate(const Frame& frame, const Frame& response, const bool borderPixelZero)
{
	ocean_assert(frame.isValid() && response.isValid());
	ocean_assert(FrameType(response, frame.pixelFormat()) == frame.frameType());

	if (!frame.isValid() || !response.isValid() || FrameType(response, frame.pixelFormat()) != frame.frameType())
	{
		return false;
	}

	ocean_assert(frame.isPixelFormatCompatible(FrameType::genericPixelFormat<T>(1u)));
	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<TResponse>(1u)));

	typedef typename FloatTyper<TResponse>::Type TFloat;

	/*
	 * Laplace with positive peak:
	 * |  0 -1  0 |
	 * | -1  4 -1 |
	 * |  0 -1  0 |
	 */

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		const TResponse* responseRow = response.constrow<TResponse>(y);

		for (unsigned int x = 0u; x < frame.width(); ++x)
		{
			TFloat pixelResponse = TFloat(0);

			if (x == 0u || y == 0u || x == frame.width() - 1u || y == frame.height() - 1u)
			{
				if (!borderPixelZero)
				{
					pixelResponse = responseBorderPixel<T, TFloat>(frame, x, y);
				}
			}
			else
			{
				pixelResponse = TFloat(frame.constpixel<T>(x, y)[0]) * TFloat(4) - TFloat(frame.constpixel<T>(x, y - 1u)[0]) - TFloat(frame.constpixel<T>(x - 1u, y)[0]) - TFloat(frame.constpixel<T>(x + 1u, y)[0]) - TFloat(frame.constpixel<T>(x, y + 1u)[0]);
			}

			if (std::is_same<TResponse, int8_t>::value)
			{
				pixelResponse *= TFloat(0.125);
			}
			else if (!std::is_signed<TResponse>::value)
			{
				pixelResponse = NumericT<TFloat>::abs(pixelResponse);
			}

			ocean_assert(pixelResponse >= TFloat(NumericT<TResponse>::minValue()) && pixelResponse <= TFloat(NumericT<TResponse>::maxValue()));

			if (TResponse(pixelResponse) != responseRow[x])
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T, typename TFloat>
TFloat TestFrameFilterLaplace::responseBorderPixel(const Frame& frame, const unsigned int x, const unsigned int y)
{
	ocean_assert(frame.isValid());

	const unsigned int width = frame.width();
	const unsigned int height = frame.height();

	ocean_assert(width >= 3u && height >= 3u);

	const unsigned int width_1 = width - 1u;
	const unsigned int height_1 = height - 1u;

	/*
	 * Laplace with positive peak:
	 * |  0 -1  0 |
	 * | -1  4 -1 |
	 * |  0 -1  0 |
	 */

	if (y == 0u)
	{
		if (x == 0u)
		{
			/*
			 * | 2 -1 |
			 * |-1  0 |
			 */

			return TFloat(frame.constpixel<T>(x, y)[0]) * TFloat(2) - TFloat(frame.constpixel<T>(x + 1u, y)[0]) - TFloat(frame.constpixel<T>(x, y + 1u)[0]);
		}

		if (x > 0u && x < width_1)
		{
			/*
			 * | -1  3 -1 |
			 * |  0 -1  0 |
			 */

			return TFloat(frame.constpixel<T>(x, y)[0]) * TFloat(3) - TFloat(frame.constpixel<T>(x - 1u, y)[0]) - TFloat(frame.constpixel<T>(x + 1u, y)[0]) - TFloat(frame.constpixel<T>(x, y + 1u)[0]);
		}

		ocean_assert(x == width_1);

		/*
		 * | -1  2 |
		 * |  0 -1 |
		 */

		return TFloat(frame.constpixel<T>(x, y)[0]) * TFloat(2) - TFloat(frame.constpixel<T>(x - 1u, y)[0]) - TFloat(frame.constpixel<T>(x, y + 1u)[0]);
	}

	if (y == height_1)
	{
		if (x == 0u)
		{
			/*
			 * | -1  0 |
			 * |  2 -1 |
			 */

			return TFloat(frame.constpixel<T>(x, y)[0]) * TFloat(2) - TFloat(frame.constpixel<T>(x + 1u, y)[0]) - TFloat(frame.constpixel<T>(x, y - 1u)[0]);
		}

		if (x > 0u && x < width_1)
		{
			/*
			 * |  0 -1  0 |
			 * | -1  3 -1 |
			 */

			return TFloat(frame.constpixel<T>(x, y)[0]) * TFloat(3) - TFloat(frame.constpixel<T>(x - 1u, y)[0]) - TFloat(frame.constpixel<T>(x + 1u, y)[0]) - TFloat(frame.constpixel<T>(x, y - 1u)[0]);
		}

		ocean_assert(x == width_1);

		/*
		 * |  0 -1 |
		 * | -1  2 |
		 */

		return TFloat(frame.constpixel<T>(x, y)[0]) * TFloat(2) - TFloat(frame.constpixel<T>(x - 1u, y)[0]) - TFloat(frame.constpixel<T>(x, y - 1u)[0]);
	}

	if (x == 0u)
	{
		/*
		 * | -1  0 |
		 * |  3 -1 |
		 * | -1  0 |
		 */

		return TFloat(frame.constpixel<T>(x, y)[0]) * TFloat(3) - TFloat(frame.constpixel<T>(x, y - 1u)[0]) - TFloat(frame.constpixel<T>(x, y + 1u)[0]) - TFloat(frame.constpixel<T>(x + 1u, y)[0]);
	}

	/*
	 * |  0 -1 |
	 * | -1  3 |
	 * |  0 -1 |
	 */
	return TFloat(frame.constpixel<T>(x, y)[0]) * TFloat(3) - TFloat(frame.constpixel<T>(x, y - 1u)[0]) - TFloat(frame.constpixel<T>(x, y + 1u)[0]) - TFloat(frame.constpixel<T>(x - 1u, y)[0]);
}

}

}

}
