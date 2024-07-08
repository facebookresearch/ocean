/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterSobel.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Memory.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterSobel.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterSobel::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width >= 3u && height >= 3u);

	Log::info() << "---   Sobel filter test with frame size " << width << "x" << height << ":   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testHorizontalVerticalFilter8BitPerChannel<int8_t>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testHorizontalVerticalFilter8BitPerChannel<int16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHorizontalVertical3Squared1Channel8BitRow(testDuration) && allSucceeded;

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
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHorizontalVerticalMaximumAbsolute8BitPerChannel<uint8_t>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	Log::info() << " ";
	allSucceeded = testHorizontalVerticalMaximumAbsolute8BitPerChannel<uint16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testComfort(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFilterPixelCoreHorizontalVertical3Squared1Channel(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Sobel filter test succeeded.";
	}
	else
	{
		Log::info() << "Sobel filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterSobel, HorizontalVerticalFilter8BitPerChannel_1920x1080_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSobel::testHorizontalVerticalFilter8BitPerChannel<int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSobel, HorizontalVerticalFilter8BitPerChannel_1920x1080_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSobel::testHorizontalVerticalFilter8BitPerChannel<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterSobel, HorizontalVertical3Squared1Channel8BitRow)
{
	EXPECT_TRUE(TestFrameFilterSobel::testHorizontalVertical3Squared1Channel8BitRow(GTEST_TEST_DURATION));
}


TEST(TestFrameFilterSobel, DiagonalFilter8BitPerChannel_1920x1080_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSobel::testDiagonalFilter8BitPerChannel<int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSobel, DiagonalFilter8BitPerChannel_1920x1080_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSobel::testDiagonalFilter8BitPerChannel<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterSobel, Filter8BitPerChannel_1920x1080_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSobel::testFilter8BitPerChannel<int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSobel, Filter8BitPerChannel_1920x1080_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSobel::testFilter8BitPerChannel<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterSobel, HorizontalVerticalMaximumAbsolute8BitPerChannel_1920x1080_uint8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSobel::testHorizontalVerticalMaximumAbsolute8BitPerChannel<uint8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameFilterSobel, HorizontalVerticalMaximumAbsolute8BitPerChannel_1920x1080_uint16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameFilterSobel::testHorizontalVerticalMaximumAbsolute8BitPerChannel<uint16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameFilterSobel, Comfort)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterSobel::testComfort(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterSobel, FilterPixelCoreHorizontalVertical3Squared1Channel)
{
	EXPECT_TRUE(TestFrameFilterSobel::testFilterPixelCoreHorizontalVertical3Squared1Channel(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename TTarget>
bool TestFrameFilterSobel::testHorizontalVerticalFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, int8_t>::value)
	{
		Log::info() << "Testing 8 bit horizontal and vertical Sobel filter, with response range [-128, 127]:";
	}
	else
	{
		Log::info() << "Testing 8 bit horizontal and vertical Sobel filter, with response range [-32768, 32767]:";
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

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame source(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source, FrameType::genericPixelFormat<TTarget>(2u * nChannels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					switch (source.channels())
					{
						case 1u:
							CV::FrameFilterSobel::filterHorizontalVertical8BitPerChannel<TTarget, 1u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 2u:
							CV::FrameFilterSobel::filterHorizontalVertical8BitPerChannel<TTarget, 2u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 3u:
							CV::FrameFilterSobel::filterHorizontalVertical8BitPerChannel<TTarget, 3u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 4u:
							CV::FrameFilterSobel::filterHorizontalVertical8BitPerChannel<TTarget, 4u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						default:
							ocean_assert(false && "Invalid channel number!");
							allSucceeded = false;
							break;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateHorizontalVerticalFilter8BitPerChannel<TTarget>(source, target))
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

bool TestFrameFilterSobel::testHorizontalVertical3Squared1Channel8BitRow(const double testDuration)
{
	ocean_assert(testDuration >= 0.0);

	Log::info() << "Testing Ixx, Iyy, Ixy filter for single row:";

	bool allSucceeded = true;

	const unsigned int minimalWidth = 10u;
	const unsigned int maximalWidth = 1920u;

	Timestamp startTimestamp(true);

	RandomI::initialize(1);

	do
	{
		const unsigned int width = RandomI::random(minimalWidth, maximalWidth);
		const unsigned int elements = RandomI::random(8u, width - 2u);

		const unsigned int framePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

		Frame frame(FrameType(width, 3u, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
		CV::CVUtilities::randomizeFrame(frame, false);

		Memory memoryResponsesXX = Memory::create<int16_t>(elements);
		Memory memoryResponsesYY = Memory::create<int16_t>(elements);
		Memory memoryResponsesXY = Memory::create<int16_t>(elements);

		int16_t* const responsesXX = memoryResponsesXX.data<int16_t>();
		int16_t* const responsesYY = memoryResponsesYY.data<int16_t>();
		int16_t* const responsesXY = memoryResponsesXY.data<int16_t>();

		CV::FrameFilterSobel::filterHorizontalVertical3Squared1Channel8BitRow(frame.constrow<uint8_t>(1u), width, elements, frame.paddingElements(), responsesXX, responsesYY, responsesXY);

		const uint8_t* row0 = frame.constpixel<uint8_t>(1u, 0u); // +1 to get the center pixel of the filter
		const uint8_t* row1 = frame.constpixel<uint8_t>(1u, 1u);
		const uint8_t* row2 = frame.constpixel<uint8_t>(1u, 2u);

		for (unsigned int n = 0u; n < elements; ++n)
		{
			// horizontal filter response (X)
			//      0 1 2
			// A | -1 0 1 |
			// B | -2 0 2 |
			// C | -1 0 1 |

			const int horizontalResponse = int(*(row0 + 1)) + int(*(row1 + 1)) * 2 + int(*(row2 + 1)) - (int(*(row0 - 1)) + int(*(row1 - 1)) * 2 + int(*(row2 - 1)));

			// vertical filter response (Y)
			//      0  1  2
			// A | -1 -2 -1 |
			// B |  0  0  0 |
			// C |  1  2  1 |

			const int verticalResponse = int(*(row2 - 1)) + int(*(row2 + 0)) * 2 + int(*(row2 + 1)) - (int(*(row0 - 1)) + int(*(row0 + 0)) * 2 + int(*(row0 + 1)));

			// we allow a rounding error of +/- 1

			const int Ix = horizontalResponse / 8;
			const int Iy = verticalResponse / 8;

			int IxxMin = (Ix - 1) * (Ix - 1);
			int IxxMiddle = (Ix + 0) * (Ix + 0);
			int IxxMax = (Ix + 1) * (Ix + 1);
			Utilities::sortLowestToFront3(IxxMin, IxxMiddle, IxxMax);

			int IyyMin = (Iy - 1) * (Iy - 1);
			int IyyMiddle = (Iy + 0) * (Iy + 0);
			int IyyMax = (Iy + 1) * (Iy + 1);
			Utilities::sortLowestToFront3(IyyMin, IyyMiddle, IyyMax);

			std::vector<int> Ixys =
			{
				(Ix - 1) * (Iy - 1),
				(Ix - 1) * (Iy + 1),
				(Ix + 1) * (Iy - 1),
				(Ix + 1) * (Iy + 1)
			};

			std::sort(Ixys.begin(), Ixys.end());

			if (int(responsesXX[n]) < IxxMin || int(responsesXX[n]) > IxxMax)
			{
				allSucceeded = false;
			}

			if (int(responsesYY[n]) < IyyMin || int(responsesYY[n]) > IyyMax)
			{
				allSucceeded = false;
			}

			if (int(responsesXY[n]) < Ixys.front() || int(responsesXY[n]) > Ixys.back())
			{
				allSucceeded = false;
			}

			row0++;
			row1++;
			row2++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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
bool TestFrameFilterSobel::testHorizontalVerticalMaximumAbsolute8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, uint8_t>::value || std::is_same<TTarget, uint16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(testDuration > 0.0);

	if (std::is_same<TTarget, uint8_t>::value)
	{
		Log::info() << "Testing uint8 maximum absolute horizontal and vertical Sobel filter, with response range [0, 255]:";
	}
	else
	{
		Log::info() << "Testing uint16 maximum absolute horizontal and vertical Sobel filter, with response range [0, 255 * 4]:";
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

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame source(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source, FrameType::genericPixelFormat<TTarget>(nChannels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					switch (source.channels())
					{
						case 1u:
							CV::FrameFilterSobel::filterHorizontalVerticalMaximumAbsolute8BitPerChannel<TTarget, 1u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 2u:
							CV::FrameFilterSobel::filterHorizontalVerticalMaximumAbsolute8BitPerChannel<TTarget, 2u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 3u:
							CV::FrameFilterSobel::filterHorizontalVerticalMaximumAbsolute8BitPerChannel<TTarget, 3u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 4u:
							CV::FrameFilterSobel::filterHorizontalVerticalMaximumAbsolute8BitPerChannel<TTarget, 4u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						default:
							ocean_assert(false && "Invalid channel number!");
							allSucceeded = false;
							break;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateHorizontalVerticalMaximumAbsolute8BitPerChannel<TTarget>(source, target))
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
bool TestFrameFilterSobel::testDiagonalFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, int8_t>::value)
	{
		Log::info() << "Testing 8 bit diagonal 45 and 135 degree Sobel filter, with response range [-128, 127]:";
	}
	else
	{
		Log::info() << "Testing 8 bit diagonal 45 and 135 degree Sobel filter, with response range [-32768, 32767]:";
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

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame source(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source, FrameType::genericPixelFormat<TTarget>(2u * nChannels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					switch (source.channels())
					{
						case 1u:
							CV::FrameFilterSobel::filterDiagonal8BitPerChannel<TTarget, 1u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 2u:
							CV::FrameFilterSobel::filterDiagonal8BitPerChannel<TTarget, 2u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 3u:
							CV::FrameFilterSobel::filterDiagonal8BitPerChannel<TTarget, 3u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 4u:
							CV::FrameFilterSobel::filterDiagonal8BitPerChannel<TTarget, 4u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						default:
							ocean_assert(false && "Invalid channel number!");
							allSucceeded = false;
							break;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateDiagonalFilter8BitPerChannel(source, target))
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
bool TestFrameFilterSobel::testFilter8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert((std::is_same<TTarget, int8_t>::value || std::is_same<TTarget, int16_t>::value), "Invalid data type!");

	ocean_assert(width >= 3u && height >= 3u);

	if (std::is_same<TTarget, int8_t>::value)
	{
		Log::info() << "Testing 8 bit horizontal, vertical, and diagonal Sobel filter, with response range [-128, 127]:";
	}
	else
	{
		Log::info() << "Testing 8 bit horizontal, vertical, and diagonal Sobel filter, with response range [-32768, 32767]:";
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

					const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
					const unsigned int targetPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

					Frame source(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t>(nChannels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
					Frame target(FrameType(source, FrameType::genericPixelFormat<TTarget>(4u * nChannels)), targetPaddingElements);

					CV::CVUtilities::randomizeFrame(source, false);
					CV::CVUtilities::randomizeFrame(target, false);

					const Frame targetCopy(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.startIf(performanceIteration);

					switch (source.channels())
					{
						case 1u:
							CV::FrameFilterSobel::filter8BitPerChannel<TTarget, 1u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 2u:
							CV::FrameFilterSobel::filter8BitPerChannel<TTarget, 2u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 3u:
							CV::FrameFilterSobel::filter8BitPerChannel<TTarget, 3u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						case 4u:
							CV::FrameFilterSobel::filter8BitPerChannel<TTarget, 4u>(source.constdata<uint8_t>(), target.data<TTarget>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), useWorker);
							break;

						default:
							ocean_assert(false && "Invalid channel number!");
							allSucceeded = false;
							break;
					}

					performance.stopIf(performanceIteration);

					if (!CV::CVUtilities::isPaddingMemoryIdentical(target, targetCopy))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!validateFilter8BitPerChannel(source, target))
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

bool TestFrameFilterSobel::testComfort(const double testDuration, Worker& worker)
{
	Log::info() << "Testing comfort functions:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		Worker* useWorker = RandomI::random(1u) == 0u ? nullptr : &worker;

		const unsigned int width = RandomI::random(3u, 1000u);
		const unsigned int height = RandomI::random(3u, 1000u);

		const unsigned int channels = RandomI::random(1u, 4u);

		const FrameType::DataType responseDataType = RandomI::random(1u) == 0u ? FrameType::DT_SIGNED_INTEGER_8 : FrameType::DT_SIGNED_INTEGER_16;
		const FrameType::PixelOrigin pixelOrigin = RandomI::random({FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), pixelOrigin));

		{
			// Comfort::filterHorizontalVertical

			const Frame sobel = CV::FrameFilterSobel::Comfort::filterHorizontalVertical(frame, responseDataType, useWorker);

			if (sobel.isValid())
			{
				 if (responseDataType == FrameType::DT_SIGNED_INTEGER_8)
				 {
					 if (!validateHorizontalVerticalFilter8BitPerChannel<int8_t>(frame, sobel))
					 {
						allSucceeded = false;
					 }
				 }
				 else
				 {
					ocean_assert(responseDataType == FrameType::DT_SIGNED_INTEGER_16);

					if (!validateHorizontalVerticalFilter8BitPerChannel<int16_t>(frame, sobel))
					 {
						allSucceeded = false;
					 }
				 }
			}
			else
			{
				allSucceeded = false;
			}
		}

		{
			// Comfort::filter

			const Frame sobel = CV::FrameFilterSobel::Comfort::filter(frame, responseDataType, useWorker);

			if (sobel.isValid())
			{
				if (!validateFilter8BitPerChannel(frame, sobel))
				{
					allSucceeded = false;
				}
			}
			else
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestFrameFilterSobel::testFilterPixelCoreHorizontalVertical3Squared1Channel(const double testDuration)
{
	Log::info() << "Testing pixel core filter function for three squared responses:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 3u, 1000u);
		const unsigned int height = RandomI::random(randomGenerator, 3u, 1000u);

		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 1u>(), pixelOrigin), &randomGenerator);

		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const unsigned int x = RandomI::random(randomGenerator, 1u, width - 2u);
			const unsigned int y = RandomI::random(randomGenerator, 1u, height - 2u);

			for (const int32_t normalization : {1, 4, 8})
			{
				for (const bool rounded : {false, true})
				{
					std::vector<int32_t> responses(4);
					const int32_t responseBack = int32_t(RandomI::random32(randomGenerator));

					responses.back() = responseBack;

					switch (normalization)
					{
						case 1u:
						{
							if (rounded)
							{
								CV::FrameFilterSobel::filterPixelCoreHorizontalVertical3Squared1Channel8Bit<int32_t, 1, true>(yFrame.constpixel<uint8_t>(x, y), yFrame.width(), responses.data(), yFrame.paddingElements());
							}
							else
							{
								CV::FrameFilterSobel::filterPixelCoreHorizontalVertical3Squared1Channel8Bit<int32_t, 1, false>(yFrame.constpixel<uint8_t>(x, y), yFrame.width(), responses.data(), yFrame.paddingElements());
							}
							break;
						}

						case 4u:
						{
							if (rounded)
							{
								CV::FrameFilterSobel::filterPixelCoreHorizontalVertical3Squared1Channel8Bit<int32_t, 4, true>(yFrame.constpixel<uint8_t>(x, y), yFrame.width(), responses.data(), yFrame.paddingElements());
							}
							else
							{
								CV::FrameFilterSobel::filterPixelCoreHorizontalVertical3Squared1Channel8Bit<int32_t, 4, false>(yFrame.constpixel<uint8_t>(x, y), yFrame.width(), responses.data(), yFrame.paddingElements());
							}
							break;
						}

						case 8u:
						{
							if (rounded)
							{
								CV::FrameFilterSobel::filterPixelCoreHorizontalVertical3Squared1Channel8Bit<int32_t, 8, true>(yFrame.constpixel<uint8_t>(x, y), yFrame.width(), responses.data(), yFrame.paddingElements());
							}
							else
							{
								CV::FrameFilterSobel::filterPixelCoreHorizontalVertical3Squared1Channel8Bit<int32_t, 8, false>(yFrame.constpixel<uint8_t>(x, y), yFrame.width(), responses.data(), yFrame.paddingElements());
							}
							break;
						}

						default:
							ocean_assert(false && "This should never happen!");
							allSucceeded = false;
							break;
					}

					if (responses.back() != responseBack)
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					ocean_assert(normalization > 0);
					int32_t Ix = filterResponse<0u>(yFrame, x, y, 0u);
					int32_t Iy = filterResponse<90u>(yFrame, x, y, 0u);

					if (rounded)
					{
						const int32_t absIx = (std::abs(Ix) + normalization / 2) / normalization;
						const int32_t absIy = (std::abs(Iy) + normalization / 2) / normalization;

						Ix = NumericT<int32_t>::copySign(absIx, Ix);
						Iy = NumericT<int32_t>::copySign(absIy, Iy);
					}
					else
					{
						Ix /= normalization;
						Iy /= normalization;
					}

					const int32_t Ixx = Ix * Ix;
					const int32_t Iyy = Iy * Iy;
					const int32_t Ixy = Ix * Iy;

					if (responses[0] != Ixx || responses[1] != Iyy || responses[2] != Ixy)
					{
						allSucceeded = false;
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

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
bool TestFrameFilterSobel::validateHorizontalVerticalFilter8BitPerChannel(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<int8_t>(2u * frame.channels())) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<int16_t>(2u * frame.channels())));
	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<TTarget>(2u * frame.channels())));

	const bool responseIsInt8 = response.dataType() == FrameType::DT_SIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < frame.height(); y++)
	{
		for (unsigned int x = 0u; x < frame.width(); x++)
		{
			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				const int32_t response0 = filterResponse<0u>(frame, x, y, channelIndex);
				const int32_t response90 = filterResponse<90u>(frame, x, y, channelIndex);

				if (responseIsInt8)
				{
					const int8_t normalizedResponse0 = int8_t(response0 / 8);
					const int8_t normalizedResponse90 = int8_t(response90 / 8);

					const int8_t* const responsePixel = response.constpixel<int8_t>(x, y) + 2u * channelIndex;

					if (normalizedResponse0 != responsePixel[0] || normalizedResponse90 != responsePixel[1])
					{
						return false;
					}
				}
				else
				{
					const int16_t* const responsePixel = response.constpixel<int16_t>(x, y) + 2u * channelIndex;

					if (response0 != int32_t(responsePixel[0]) || response90 != int32_t(responsePixel[1]))
					{
						return false;
					}
				}
			}
		}
	}

	// now we test the per-pixel filter function

	std::vector<TTarget> pixelResponse(frame.channels() * 2u);

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		for (unsigned int x = 0u; x < frame.width(); ++x)
		{
			switch (frame.channels())
			{
				case 1u:
					CV::FrameFilterSobel::filterPixelHorizontalVertical8BitPerChannel<TTarget, 1u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), x, y, pixelResponse.data(), frame.paddingElements());
					break;

				case 2u:
					CV::FrameFilterSobel::filterPixelHorizontalVertical8BitPerChannel<TTarget, 2u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), x, y, pixelResponse.data(), frame.paddingElements());
					break;

				case 3u:
					CV::FrameFilterSobel::filterPixelHorizontalVertical8BitPerChannel<TTarget, 3u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), x, y, pixelResponse.data(), frame.paddingElements());
					break;

				case 4u:
					CV::FrameFilterSobel::filterPixelHorizontalVertical8BitPerChannel<TTarget, 4u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), x, y, pixelResponse.data(), frame.paddingElements());
					break;

				default:
					ocean_assert(false && "Invalid channel number!");
					return false;
			}

			const TTarget* filter = response.constpixel<TTarget>(x, y);

			for (unsigned int n = 0u; n < frame.channels() * 2u; ++n)
			{
				if (filter[n] != pixelResponse[n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename TTarget>
bool TestFrameFilterSobel::validateHorizontalVerticalMaximumAbsolute8BitPerChannel(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<uint8_t>(frame.channels())) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<uint16_t>(frame.channels())));
	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<TTarget>(frame.channels())));

	const bool responseIsUInt8 = response.dataType() == FrameType::DT_UNSIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < frame.height(); y++)
	{
		for (unsigned int x = 0u; x < frame.width(); x++)
		{
			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				const int32_t response0 = abs(filterResponse<0u>(frame, x, y, channelIndex));
				const int32_t response90 = abs(filterResponse<90u>(frame, x, y, channelIndex));

				if (responseIsUInt8)
				{
					const uint8_t normalizedResponse = uint8_t(max((response0 + 2) / 4, (response90 + 2) / 4));

					const uint8_t responsePixel = response.constpixel<uint8_t>(x, y)[channelIndex];

					if (normalizedResponse != responsePixel)
					{
						return false;
					}
				}
				else
				{
					const int32_t normalizedResponse = max(response0, response90);

					const uint16_t responsePixel = response.constpixel<uint16_t>(x, y)[channelIndex];

					if (normalizedResponse != int32_t(responsePixel))
					{
						return false;
					}
				}
			}
		}
	}

	// now we test the per-pixel filter function

	std::vector<TTarget> pixelResponse(channels);

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		for (unsigned int x = 0u; x < frame.width(); ++x)
		{
			switch (channels)
			{
				case 1u:
					CV::FrameFilterSobel::filterPixelHorizontalVerticalMaximum8BitPerChannel<TTarget, 1u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), x, y, pixelResponse.data(), frame.paddingElements());
					break;

				case 2u:
					CV::FrameFilterSobel::filterPixelHorizontalVerticalMaximum8BitPerChannel<TTarget, 2u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), x, y, pixelResponse.data(), frame.paddingElements());
					break;

				case 3u:
					CV::FrameFilterSobel::filterPixelHorizontalVerticalMaximum8BitPerChannel<TTarget, 3u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), x, y, pixelResponse.data(), frame.paddingElements());
					break;

				case 4u:
					CV::FrameFilterSobel::filterPixelHorizontalVerticalMaximum8BitPerChannel<TTarget, 4u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), x, y, pixelResponse.data(), frame.paddingElements());
					break;

				default:
					ocean_assert(false && "Invalid channel number!");
					return false;
			}

			const TTarget* filter = response.constpixel<TTarget>(x, y);

			for (unsigned int n = 0u; n < frame.channels(); ++n)
			{
				if (filter[n] != pixelResponse[n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TestFrameFilterSobel::validateDiagonalFilter8BitPerChannel(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<int8_t>(2u * frame.channels())) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<int16_t>(2u * frame.channels())));

	const bool responseIsInt8 = response.dataType() == FrameType::DT_SIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < frame.height(); y++)
	{
		for (unsigned int x = 0u; x < frame.width(); x++)
		{
			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				const int32_t response45 = filterResponse<45u>(frame, x, y, channelIndex);
				const int32_t response135 = filterResponse<135u>(frame, x, y, channelIndex);

				if (responseIsInt8)
				{
					const int8_t normalizedResponse45 = int8_t(response45 / 8);
					const int8_t normalizedResponse135 = int8_t(response135 / 8);

					const int8_t* const responsePixel = response.constpixel<int8_t>(x, y) + 2u * channelIndex;

					if (normalizedResponse45 != responsePixel[0] || normalizedResponse135 != responsePixel[1])
					{
						return false;
					}
				}
				else
				{
					const int16_t* const responsePixel = response.constpixel<int16_t>(x, y) + 2u * channelIndex;

					if (response45 != int32_t(responsePixel[0]) || response135 != int32_t(responsePixel[1]))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool TestFrameFilterSobel::validateFilter8BitPerChannel(const Frame& frame, const Frame& response)
{
	ocean_assert(frame.width() == response.width());
	ocean_assert(frame.height() == response.height());

	ocean_assert(response.isPixelFormatCompatible(FrameType::genericPixelFormat<int8_t>(4u * frame.channels())) || response.isPixelFormatCompatible(FrameType::genericPixelFormat<int16_t>(4u * frame.channels())));

	const bool responseIsInt8 = response.dataType() == FrameType::DT_SIGNED_INTEGER_8;

	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < frame.height(); y++)
	{
		for (unsigned int x = 0u; x < frame.width(); x++)
		{
			for (unsigned int channelIndex = 0u; channelIndex < channels; ++channelIndex)
			{
				const int32_t response0 = filterResponse<0u>(frame, x, y, channelIndex);
				const int32_t response90 = filterResponse<90u>(frame, x, y, channelIndex);
				const int32_t response45 = filterResponse<45u>(frame, x, y, channelIndex);
				const int32_t response135 = filterResponse<135u>(frame, x, y, channelIndex);

				if (responseIsInt8)
				{
					const int8_t normalizedResponse0 = int8_t(response0 / 8);
					const int8_t normalizedResponse90 = int8_t(response90 / 8);
					const int8_t normalizedResponse45 = int8_t(response45 / 8);
					const int8_t normalizedResponse135 = int8_t(response135 / 8);

					const int8_t* const responsePixel = response.constpixel<int8_t>(x, y) + 4u * channelIndex;

					if (normalizedResponse0 != responsePixel[0] || normalizedResponse90 != responsePixel[1] || normalizedResponse45 != responsePixel[2] || normalizedResponse135 != responsePixel[3])
					{
						return false;
					}
				}
				else
				{
					const int16_t* const responsePixel = response.constpixel<int16_t>(x, y) + 4u * channelIndex;

					if (response0 != int32_t(responsePixel[0]) || response90 != int32_t(responsePixel[1]) || response45 != int32_t(responsePixel[2]) || response135 != int32_t(responsePixel[3]))
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
