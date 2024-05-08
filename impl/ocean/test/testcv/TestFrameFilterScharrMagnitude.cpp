/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterScharrMagnitude.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterScharrMagnitude.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterScharrMagnitude::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Scharr Magnitude filter test with frame size " << width << "x" << height << ":   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHorizontalVerticalFilter8BitPerChannel<int8_t>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testHorizontalVerticalFilter8BitPerChannel<int16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDiagonalFilter8BitPerChannel<int8_t>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testDiagonalFilter8BitPerChannel<int16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilter8BitPerChannel<int8_t>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testFilter8BitPerChannel<int16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Scharr Magnitude filter test succeeded.";
	}
	else
	{
		Log::info() << "Scharr Magnitude filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterScharrMagnitude, HorizontalVerticalFilter8BitPerChannel_1920x1080_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharrMagnitude::testHorizontalVerticalFilter8BitPerChannel<int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterScharrMagnitude, HorizontalVerticalFilter8BitPerChannel_1920x1080_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharrMagnitude::testHorizontalVerticalFilter8BitPerChannel<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterScharrMagnitude, DiagonalFilter8BitPerChannel_1920x1080_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharrMagnitude::testDiagonalFilter8BitPerChannel<int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterScharrMagnitude, DiagonalFilter8BitPerChannel_1920x1080_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharrMagnitude::testDiagonalFilter8BitPerChannel<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterScharrMagnitude, Filter8BitPerChannel_1920x1080_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharrMagnitude::testFilter8BitPerChannel<int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterScharrMagnitude, Filter8BitPerChannel_1920x1080_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterScharrMagnitude::testFilter8BitPerChannel<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

template <typename TTarget>
bool TestFrameFilterScharrMagnitude::testHorizontalVerticalFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, int8_t>::value)
	{
		Log::info() << "Testing 8 bit horizontal and vertical Scharr magnitude filter, with response range [-128, 127]:";
	}
	else
	{
		Log::info() << "Testing 8 bit horizontal and vertical Scharr magnitude filter, with response range [-32768, 32767]:";
	}

	bool allSucceeded = true;

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
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, height);

					const unsigned int framePaddingElements = RandomI::random(100u) * RandomI::random(1u);
					const unsigned int responsePaddingElements = RandomI::random(100u) * RandomI::random(1u);

					Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
					Frame response(FrameType(frame, FrameType::genericPixelFormat<TTarget, 2u>()), responsePaddingElements);

					CV::CVUtilities::randomizeFrame(frame);
					CV::CVUtilities::randomizeFrame(response);

					const Frame copyResponse(response, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					if (!CV::FrameFilterScharrMagnitude::Comfort::filterHorizontalVerticalAs1Channel(frame, response, useWorker))
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(response, copyResponse))
					{
						ocean_assert(false && "Invalid memory!");
						return false;
					}

					if (!validateFilterHorizontalVerticalAs1Channel8Bit(frame, response))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
		}
	}

	Log::info() << " ";

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

template <typename TTarget>
bool TestFrameFilterScharrMagnitude::testDiagonalFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, int8_t>::value)
	{
		Log::info() << "Testing 8 bit diagonal Scharr magnitude filter, with response range [-128, 127]:";
	}
	else
	{
		Log::info() << "Testing 8 bit diagonal Scharr magnitude filter, with response range [-32768, 32767]:";
	}

	bool allSucceeded = true;

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
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, height);

					const unsigned int framePaddingElements = RandomI::random(100u) * RandomI::random(1u);
					const unsigned int responsePaddingElements = RandomI::random(100u) * RandomI::random(1u);

					Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
					Frame response(FrameType(frame, FrameType::genericPixelFormat<TTarget, 2u>()), responsePaddingElements);

					CV::CVUtilities::randomizeFrame(frame);
					CV::CVUtilities::randomizeFrame(response);

					const Frame copyResponse(response, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					if (!CV::FrameFilterScharrMagnitude::Comfort::filterDiagonalAs1Channel(frame, response, useWorker))
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(response, copyResponse))
					{
						ocean_assert(false && "Invalid memory!");
						return false;
					}

					if (!validateFilterDiagonalAs1Channel8Bit(frame, response))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
		}
	}

	Log::info() << " ";

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

template <typename TTarget>
bool TestFrameFilterScharrMagnitude::testFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, int8_t>::value)
	{
		Log::info() << "Testing 8 bit horizontal, vertical, and diagonal Scharr magnitude filter, with response range [-128, 127]:";
	}
	else
	{
		Log::info() << "Testing 8 bit horizontal, vertical, and diagonal Scharr magnitude filter, with response range [-32768, 32767]:";
	}

	bool allSucceeded = true;

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
					const unsigned int testWidth = performanceIteration ? width : RandomI::random(3u, width);
					const unsigned int testHeight = performanceIteration ? height : RandomI::random(3u, height);

					const unsigned int framePaddingElements = RandomI::random(100u) * RandomI::random(1u);
					const unsigned int responsePaddingElements = RandomI::random(100u) * RandomI::random(1u);

					Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
					Frame response(FrameType(frame, FrameType::genericPixelFormat<TTarget, 4u>()), responsePaddingElements);

					CV::CVUtilities::randomizeFrame(frame);
					CV::CVUtilities::randomizeFrame(response);

					const Frame copyResponse(response, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					if (!CV::FrameFilterScharrMagnitude::Comfort::filterAs1Channel(frame, response, useWorker))
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(response, copyResponse))
					{
						ocean_assert(false && "Invalid memory!");
						return false;
					}

					if (!validateFilterAs1Channel8Bit(frame, response))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x";
		}
	}

	Log::info() << " ";

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

bool TestFrameFilterScharrMagnitude::validateFilterHorizontalVerticalAs1Channel8Bit(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<int8_t>(2u)) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<int16_t>(2u)));

	const bool responseIsInt8 = response.dataType() == FrameType::DT_SIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < frame.height(); y++)
	{
		for (unsigned int x = 0u; x < frame.width(); x++)
		{
			uint32_t bestMagnitude = 0u;
			int32_t bestResponse0 = 0;
			int32_t bestResponse90 = 0;

			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				int32_t response0 = TestFrameFilterScharr::filterResponse<0u>(frame, x, y, channelIndex);
				int32_t response90 = TestFrameFilterScharr::filterResponse<90u>(frame, x, y, channelIndex);

				if (responseIsInt8)
				{
					response0 = int8_t(response0 / 32);
					response90 = int8_t(response90 / 32);
				}

				const uint32_t magnitude = uint32_t(response0 * response0 + response90 * response90);

				if (magnitude > bestMagnitude)
				{
					bestMagnitude = magnitude;

					bestResponse0 = response0;
					bestResponse90 = response90;
				}
			}

			if (responseIsInt8)
			{
				const int8_t* const responsePixel = response.constpixel<int8_t>(x, y);

				if (int32_t(responsePixel[0]) != bestResponse0 || int32_t(responsePixel[1]) != bestResponse90)
				{
					// we need to check whether another channel may have the same magnitude

					const uint32_t responseMagnitude = uint32_t(responsePixel[0] * responsePixel[0] + responsePixel[1] * responsePixel[1]);

					if (responseMagnitude != bestMagnitude)
					{
						return false;
					}
				}
			}
			else
			{
				const int16_t* const responsePixel = response.constpixel<int16_t>(x, y);

				if (int32_t(responsePixel[0]) != bestResponse0 || int32_t(responsePixel[1]) != bestResponse90)
				{
					// we need to check whether another channel may have the same magnitude

					const uint32_t responseMagnitude = uint32_t(responsePixel[0] * responsePixel[0] + responsePixel[1] * responsePixel[1]);

					if (responseMagnitude != bestMagnitude)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool TestFrameFilterScharrMagnitude::validateFilterDiagonalAs1Channel8Bit(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<int8_t>(2u)) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<int16_t>(2u)));

	const bool responseIsInt8 = response.dataType() == FrameType::DT_SIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < frame.height(); y++)
	{
		for (unsigned int x = 0u; x < frame.width(); x++)
		{
			uint32_t bestMagnitude = 0u;
			int32_t bestResponse45 = 0;
			int32_t bestResponse135 = 0;

			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				int32_t response45 = TestFrameFilterScharr::filterResponse<45u>(frame, x, y, channelIndex);
				int32_t response135 = TestFrameFilterScharr::filterResponse<135u>(frame, x, y, channelIndex);

				if (responseIsInt8)
				{
					response45 = int8_t(response45 / 32);
					response135 = int8_t(response135 / 32);
				}

				const uint32_t magnitude = uint32_t(response45 * response45 + response135 * response135);

				if (magnitude > bestMagnitude)
				{
					bestMagnitude = magnitude;

					bestResponse45 = response45;
					bestResponse135 = response135;
				}
			}

			if (responseIsInt8)
			{
				const int8_t* const responsePixel = response.constpixel<int8_t>(x, y);

				if (int32_t(responsePixel[0]) != bestResponse45 || int32_t(responsePixel[1]) != bestResponse135)
				{
					// we need to check whether another channel may have the same magnitude

					const uint32_t responseMagnitude = uint32_t(responsePixel[0] * responsePixel[0] + responsePixel[1] * responsePixel[1]);

					if (responseMagnitude != bestMagnitude)
					{
						return false;
					}
				}
			}
			else
			{
				const int16_t* const responsePixel = response.constpixel<int16_t>(x, y);

				if (int32_t(responsePixel[0]) != bestResponse45 || int32_t(responsePixel[1]) != bestResponse135)
				{
					// we need to check whether another channel may have the same magnitude

					const uint32_t responseMagnitude = uint32_t(responsePixel[0] * responsePixel[0] + responsePixel[1] * responsePixel[1]);

					if (responseMagnitude != bestMagnitude)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool TestFrameFilterScharrMagnitude::validateFilterAs1Channel8Bit(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<int8_t>(4u)) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<int16_t>(4u)));

	const bool responseIsInt8 = response.dataType() == FrameType::DT_SIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < frame.height(); y++)
	{
		for (unsigned int x = 0u; x < frame.width(); x++)
		{
			uint32_t bestMagnitude = 0u;
			int32_t bestResponse0 = 0;
			int32_t bestResponse90 = 0;

			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				int32_t response0 = TestFrameFilterScharr::filterResponse<0u>(frame, x, y, channelIndex);
				int32_t response90 = TestFrameFilterScharr::filterResponse<90u>(frame, x, y, channelIndex);

				if (responseIsInt8)
				{
					response0 = int8_t(response0 / 32);
					response90 = int8_t(response90 / 32);
				}

				const uint32_t magnitude = uint32_t(response0 * response0 + response90 * response90);

				if (magnitude > bestMagnitude)
				{
					bestMagnitude = magnitude;

					bestResponse0 = response0;
					bestResponse90 = response90;
				}
			}

			if (responseIsInt8)
			{
				const int8_t* const responsePixel = response.constpixel<int8_t>(x, y);

				if (int32_t(responsePixel[0]) != bestResponse0 || int32_t(responsePixel[1]) != bestResponse90)
				{
					// we need to check whether another channel may have the same magnitude

					const uint32_t responseMagnitude = uint32_t(responsePixel[0] * responsePixel[0] + responsePixel[1] * responsePixel[1]);

					if (responseMagnitude != bestMagnitude)
					{
						return false;
					}
				}
			}
			else
			{
				const int16_t* const responsePixel = response.constpixel<int16_t>(x, y);

				if (int32_t(responsePixel[0]) != bestResponse0 || int32_t(responsePixel[1]) != bestResponse90)
				{
					// we need to check whether another channel may have the same magnitude

					const uint32_t responseMagnitude = uint32_t(responsePixel[0] * responsePixel[0] + responsePixel[1] * responsePixel[1]);

					if (responseMagnitude != bestMagnitude)
					{
						return false;
					}
				}
			}

			bestMagnitude = 0u;
			int32_t bestResponse45 = 0;
			int32_t bestResponse135 = 0;

			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				int32_t response45 = TestFrameFilterScharr::filterResponse<45u>(frame, x, y, channelIndex);
				int32_t response135 = TestFrameFilterScharr::filterResponse<135u>(frame, x, y, channelIndex);

				if (responseIsInt8)
				{
					response45 = int8_t(response45 / 32);
					response135 = int8_t(response135 / 32);
				}

				const uint32_t magnitude = uint32_t(response45 * response45 + response135 * response135);

				if (magnitude > bestMagnitude)
				{
					bestMagnitude = magnitude;

					bestResponse45 = response45;
					bestResponse135 = response135;
				}
			}

			if (responseIsInt8)
			{
				const int8_t* const responsePixel = response.constpixel<int8_t>(x, y);

				if (int32_t(int32_t(responsePixel[2]) != bestResponse45 || int32_t(responsePixel[3]) != bestResponse135))
				{
					// we need to check whether another channel may have the same magnitude

					const uint32_t responseMagnitude = uint32_t(responsePixel[2] * responsePixel[2] + responsePixel[3] * responsePixel[3]);

					if (responseMagnitude != bestMagnitude)
					{
						return false;
					}
				}
			}
			else
			{
				const int16_t* const responsePixel = response.constpixel<int16_t>(x, y);

				if (int32_t(int32_t(responsePixel[2]) != bestResponse45 || int32_t(responsePixel[3]) != bestResponse135))
				{
					// we need to check whether another channel may have the same magnitude

					const uint32_t responseMagnitude = uint32_t(responsePixel[2] * responsePixel[2] + responsePixel[3] * responsePixel[3]);

					if (responseMagnitude != bestMagnitude)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

}

}

}
