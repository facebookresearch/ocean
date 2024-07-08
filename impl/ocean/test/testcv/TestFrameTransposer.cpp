/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameTransposer.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameTransposer.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameTransposer::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame transposer test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testTransposer(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRotate90(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRotate180(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRotate(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame transposer test succeeded.";
	}
	else
	{
		Log::info() << "Frame transposer test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameTransposer, Transposer_Uint8_1Channel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameTransposer::testTransposer<uint8_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameTransposer, TransposerUint8_2Channel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameTransposer::testTransposer<uint8_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameTransposer, TransposerUint8_3Channel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameTransposer::testTransposer<uint8_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameTransposer, TransposerUint8_4Channel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameTransposer::testTransposer<uint8_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameTransposer, TransposerUint8_Channel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameTransposer::testTransposer<uint8_t, 5u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameTransposer, TransposerFloat_1Channel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameTransposer::testTransposer<float, 1u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameTransposer, TransposerFloat_2Channel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameTransposer::testTransposer<float, 2u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameTransposer, TransposerFloat_3Channel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameTransposer::testTransposer<float, 3u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameTransposer, TransposerFloat_4Channel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameTransposer::testTransposer<float, 4u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameTransposer, TransposerFloat_5Channel_1920x1080)
{
	Worker worker;
	EXPECT_TRUE((TestFrameTransposer::testTransposer<float, 5u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameTransposer, Rotate90)
{
	Worker worker;
	EXPECT_TRUE(TestFrameTransposer::testRotate90(GTEST_TEST_DURATION, worker));
}


TEST(TestFrameTransposer, Rotate180)
{
	Worker worker;
	EXPECT_TRUE(TestFrameTransposer::testRotate180(GTEST_TEST_DURATION, worker));
}


TEST(TestFrameTransposer, Rotate)
{
	Worker worker;
	EXPECT_TRUE(TestFrameTransposer::testRotate(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameTransposer::testTransposer(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const IndexPairs32 sizes =
	{
		IndexPair32(640u, 480u),
		IndexPair32(1280u, 720u),
		IndexPair32(1920u, 1080u),
	};

	bool allSucceeded = true;

	Log::info() << "Transposer frame transposer:";
	Log::info() << " ";

	for (size_t n = 0; n < sizes.size(); ++n)
	{
		Log::info().newLine(n != 0u);
		Log::info().newLine(n != 0u);

		const unsigned int width = sizes[n].first;
		const unsigned int height = sizes[n].second;

		if (!testTransposer<uint8_t>(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}

		Log::info() << " ";
		Log::info() << " ";

		if (!testTransposer<int8_t>(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}

		Log::info() << " ";
		Log::info() << " ";

		if (!testTransposer<float>(width, height, testDuration, worker))
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameTransposer::testTransposer(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	Log::info() << "Testing frame dimension " << width << "x" << height << " with data type '" << TypeNamer::name<T>() << "':";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testTransposer<T, 1u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testTransposer<T, 2u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testTransposer<T, 3u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testTransposer<T, 4u>(width, height, testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testTransposer<T, 5u>(width, height, testDuration, worker) && allSucceeded;

	return allSucceeded;
}

template <typename T, unsigned int tChannels>
bool TestFrameTransposer::testTransposer(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(width >= 1u && height >= 1u && testDuration > 0.0);

	Log::info() << "... with " << tChannels << " channels:";

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<T, tChannels>();

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++ workerIteration)
	{
		// we use the worker (multi-core execution in the second iteration)
		HighPerformanceStatistic& performance = workerIteration == 0u ? performanceSinglecore : performanceMulticore;
		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 2000u);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 2000u);

				const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
				Frame transposed = CV::CVUtilities::randomizedFrame(FrameType(frame.frameType(), testHeight, testWidth), &randomGenerator);

				const Frame copyTransposed(transposed, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(performanceIteration);

				if (RandomI::random(1u) == 0u || tChannels > 4u)
				{
					CV::FrameTransposer::transpose<T, tChannels>(frame.constdata<T>(), transposed.data<T>(), frame.width(), frame.height(), frame.paddingElements(), transposed.paddingElements(), useWorker);
				}
				else
				{
					CV::FrameTransposer::transpose(frame, transposed, useWorker);
				}

				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(transposed, copyTransposed))
				{
					ocean_assert(false && "Invalid memory");
					return false;
				}

				if (!validateTransposer<T, tChannels>(frame.constdata<T>(), transposed.constdata<T>(), frame.width(), frame.height(), frame.paddingElements(), transposed.paddingElements()))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core Performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
		Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
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

bool TestFrameTransposer::testRotate90(const double testDuration, Worker& worker)
{
	Log::info() << "Test comfort rotate 90 degree function:";

	bool allSucceeded = true;

	FrameType::PixelFormats pixelFormats = CV::CVUtilities::definedPixelFormats();
	std::unordered_set<FrameType::PixelFormat> pixelFormatSet(pixelFormats.cbegin(), pixelFormats.cend());

	pixelFormatSet.erase(FrameType::FORMAT_Y10_PACKED);
	pixelFormatSet.erase(FrameType::FORMAT_BGGR10_PACKED);
	pixelFormatSet.erase(FrameType::FORMAT_RGGB10_PACKED);

	pixelFormats = FrameType::PixelFormats(pixelFormatSet.cbegin(), pixelFormatSet.cend());

	// ensuring that we have covered all pixel formats

	if (pixelFormats.size() != size_t(FrameType::FORMAT_END) - 4) // -3 due to missing packed formats
	{
		ocean_assert(false && "Missing pixel format!");
		allSucceeded = false;
	}

	for (const FrameType::PixelFormat pixelFormat : pixelFormats)
	{
		if (FrameType::formatIsPacked(pixelFormat))
		{
			ocean_assert(false && "Invalid pixel format!");
			allSucceeded = false;
		}
	}

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		FrameType::PixelFormat sourcePixelFormat = RandomI::random(randomGenerator, pixelFormats);

		if (RandomI::random(randomGenerator, 3u) == 3u)
		{
			// using a generic pixel format every 4th iteration

			const FrameType::DataType dataType = RandomI::random(randomGenerator, FrameType::definedDataTypes());
			const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

			sourcePixelFormat = FrameType::genericPixelFormat(dataType, channels);
		}

		const unsigned int widthMultiple = FrameType::widthMultiple(sourcePixelFormat) * FrameType::heightMultiple(sourcePixelFormat);
		const unsigned int heightMultiple = FrameType::heightMultiple(sourcePixelFormat) * FrameType::widthMultiple(sourcePixelFormat);

		const unsigned int width = RandomI::random(randomGenerator, 1u, 400u) * widthMultiple;
		const unsigned int height = RandomI::random(randomGenerator, 1u, 400u) * heightMultiple;

		const FrameType::PixelOrigin sourcePixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const FrameType sourceFrameType(width, height, sourcePixelFormat, sourcePixelOrigin);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(sourceFrameType, &randomGenerator);

		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		const bool clockwise = RandomI::random(randomGenerator, 1u) == 0u ? true : false;

		Frame targetFrame;
		Frame copyTargetFrame;

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrameType, height, width), &randomGenerator);

			copyTargetFrame = Frame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
		}

		if (CV::FrameTransposer::Comfort::rotate90(sourceFrame, targetFrame, clockwise, useWorker))
		{
			if (!validateRotate90(sourceFrame, targetFrame, clockwise))
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}

		if (copyTargetFrame.isValid())
		{
			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
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

bool TestFrameTransposer::testRotate180(const double testDuration, Worker& worker)
{
	Log::info() << "Test comfort rotate 180 degree function:";

	bool allSucceeded = true;

	FrameType::PixelFormats pixelFormats = CV::CVUtilities::definedPixelFormats();
	std::unordered_set<FrameType::PixelFormat> pixelFormatSet(pixelFormats.cbegin(), pixelFormats.cend());

	pixelFormatSet.erase(FrameType::FORMAT_Y10_PACKED);
	pixelFormatSet.erase(FrameType::FORMAT_BGGR10_PACKED);
	pixelFormatSet.erase(FrameType::FORMAT_RGGB10_PACKED);

	pixelFormats = FrameType::PixelFormats(pixelFormatSet.cbegin(), pixelFormatSet.cend());

	// ensuring that we have covered all pixel formats

	if (pixelFormats.size() != size_t(FrameType::FORMAT_END) - 4) // -3 due to missing packed formats
	{
		ocean_assert(false && "Missing pixel format!");
		allSucceeded = false;
	}

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		FrameType::PixelFormat sourcePixelFormat = RandomI::random(randomGenerator, pixelFormats);

		if (RandomI::random(randomGenerator, 3u) == 3u)
		{
			// using a generic pixel format every 4th iteration

			const FrameType::DataType dataType = RandomI::random(randomGenerator, FrameType::definedDataTypes());
			const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

			sourcePixelFormat = FrameType::genericPixelFormat(dataType, channels);
		}

		const unsigned int widthMultiple = FrameType::widthMultiple(sourcePixelFormat) * FrameType::heightMultiple(sourcePixelFormat);
		const unsigned int heightMultiple = FrameType::heightMultiple(sourcePixelFormat) * FrameType::widthMultiple(sourcePixelFormat);

		const unsigned int width = RandomI::random(randomGenerator, 1u, 400u) * widthMultiple;
		const unsigned int height = RandomI::random(randomGenerator, 1u, 400u) * heightMultiple;

		const FrameType::PixelOrigin sourcePixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const FrameType sourceFrameType(width, height, sourcePixelFormat, sourcePixelOrigin);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(sourceFrameType, &randomGenerator);

		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		Frame targetFrame;
		Frame copyTargetFrame;

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			targetFrame = CV::CVUtilities::randomizedFrame(sourceFrameType, &randomGenerator);

			copyTargetFrame = Frame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
		}

		if (CV::FrameTransposer::Comfort::rotate180(sourceFrame, targetFrame, useWorker))
		{
			if (!validateRotate180(sourceFrame, targetFrame))
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}

		if (copyTargetFrame.isValid())
		{
			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
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

bool TestFrameTransposer::testRotate(const double testDuration, Worker& worker)
{
	Log::info() << "Test comfort rotate +/- 90 degree steps function:";

	bool allSucceeded = true;

	FrameType::PixelFormats pixelFormats = CV::CVUtilities::definedPixelFormats();
	std::unordered_set<FrameType::PixelFormat> pixelFormatSet(pixelFormats.cbegin(), pixelFormats.cend());

	pixelFormatSet.erase(FrameType::FORMAT_Y10_PACKED);
	pixelFormatSet.erase(FrameType::FORMAT_BGGR10_PACKED);
	pixelFormatSet.erase(FrameType::FORMAT_RGGB10_PACKED);

	pixelFormats = FrameType::PixelFormats(pixelFormatSet.cbegin(), pixelFormatSet.cend());

	// ensuring that we have covered all pixel formats

	if (pixelFormats.size() != size_t(FrameType::FORMAT_END) - 4) // -3 due to missing packed formats
	{
		ocean_assert(false && "Missing pixel format!");
		allSucceeded = false;
	}

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		FrameType::PixelFormat sourcePixelFormat = RandomI::random(randomGenerator, pixelFormats);

		if (RandomI::random(randomGenerator, 3u) == 3u)
		{
			// using a generic pixel format every 4th iteration

			const FrameType::DataType dataType = RandomI::random(randomGenerator, FrameType::definedDataTypes());
			const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

			sourcePixelFormat = FrameType::genericPixelFormat(dataType, channels);
		}

		const unsigned int widthMultiple = FrameType::widthMultiple(sourcePixelFormat) * FrameType::heightMultiple(sourcePixelFormat);
		const unsigned int heightMultiple = FrameType::heightMultiple(sourcePixelFormat) * FrameType::widthMultiple(sourcePixelFormat);

		const unsigned int width = RandomI::random(randomGenerator, 1u, 400u) * widthMultiple;
		const unsigned int height = RandomI::random(randomGenerator, 1u, 400u) * heightMultiple;

		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		int angle = int(RandomI::random(randomGenerator, 100u) * 90u); // [0, 90 * 100]

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			angle = -angle;
		}

		for (const bool useComfort : {true, false})
		{
			if (useComfort)
			{
				const FrameType::PixelOrigin sourcePixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

				const FrameType sourceFrameType(width, height, sourcePixelFormat, sourcePixelOrigin);
				const Frame sourceFrame = CV::CVUtilities::randomizedFrame(sourceFrameType, &randomGenerator);

				Frame targetFrame;
				Frame copyTargetFrame;

				if (RandomI::random(randomGenerator, 1u) == 0u)
				{
					FrameType targetFrameType = sourceFrameType;

					if (NumericT<int>::abs(angle % 360) == 90 || NumericT<int>::abs(angle % 360) == 270)
					{
						targetFrameType = FrameType(targetFrameType, targetFrameType.height(), targetFrameType.width());
					}

					targetFrame = CV::CVUtilities::randomizedFrame(targetFrameType, &randomGenerator);

					copyTargetFrame = Frame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				}

				if (!CV::FrameTransposer::Comfort::rotate(sourceFrame, targetFrame, angle, useWorker))
				{
					allSucceeded = false;
				}

				if (copyTargetFrame.isValid())
				{
					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}
				}

				if (!validateRotate(sourceFrame, targetFrame, angle))
				{
					allSucceeded = false;
				}
			}
			else
			{
				constexpr FrameType::PixelOrigin sourcePixelOrigin = FrameType::ORIGIN_UPPER_LEFT;

				const FrameType sourceFrameType(width, height, sourcePixelFormat, sourcePixelOrigin);
				const Frame sourceFrame = CV::CVUtilities::randomizedFrame(sourceFrameType, &randomGenerator);

				const unsigned int targetWidth = NumericT<int>::abs(angle % 360) == 90 || NumericT<int>::abs(angle % 360) == 270 ? height : width;
				const unsigned int targetHeight = NumericT<int>::abs(angle % 360) == 90 || NumericT<int>::abs(angle % 360) == 270 ? width : height;

				Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, targetWidth, targetHeight), &randomGenerator);
				const Frame copyTargetFrame (targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				if (!rotate(sourceFrame, targetFrame, angle, useWorker))
				{
					allSucceeded = false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validateRotate(sourceFrame, targetFrame, angle))
				{
					allSucceeded = false;
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

template <typename T, unsigned int tChannels>
bool TestFrameTransposer::validateTransposer(const T* frame, const T* transposed, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int transposedPaddingElements)
{
	ocean_assert(frame && transposed);
	ocean_assert(width != 0u && height != 0u);

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const T* const framePixel = frame + y * (width * tChannels + framePaddingElements) + x * tChannels;
			const T* const transposedPixel = transposed + x * (height * tChannels + transposedPaddingElements) + y * tChannels;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				if (framePixel[n] != transposedPixel[n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TestFrameTransposer::validateRotate90(const Frame& sourceFrame, const Frame& targetFrame, const bool clockwise)
{
	ocean_assert(sourceFrame.isValid() && targetFrame.isValid());

	if (!sourceFrame.isValid() || !targetFrame.isValid())
	{
		return false;
	}

	if (targetFrame.frameType() != FrameType(sourceFrame, sourceFrame.height(), sourceFrame.width()))
	{
		return false;
	}

	const bool adjustedClockwise = sourceFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT ? clockwise : !clockwise;

	for (unsigned int planeIndex = 0u; planeIndex < sourceFrame.numberPlanes(); ++planeIndex)
	{
		const unsigned int bytesPerPlanePixel = sourceFrame.bytesPerDataType() * sourceFrame.planeChannels(planeIndex);

		const unsigned int sourcePlaneWidth = sourceFrame.planeWidth(planeIndex);
		const unsigned int sourcePlaneHeight = sourceFrame.planeHeight(planeIndex);

		for (unsigned int y = 0u; y < sourcePlaneHeight; ++y)
		{
			const unsigned int xRotated = adjustedClockwise ? sourcePlaneHeight - y - 1u : y;

			for (unsigned int x = 0u; x < sourcePlaneWidth; ++x)
			{
				const unsigned int yRotated = adjustedClockwise ? x : sourcePlaneWidth - x - 1u;

				const void* sourcePixel = sourceFrame.constpixel<void>(x, y, planeIndex);
				const void* targetPixel = targetFrame.constpixel<void>(xRotated, yRotated, planeIndex);

				if (memcmp(sourcePixel, targetPixel, bytesPerPlanePixel) != 0)
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TestFrameTransposer::validateRotate180(const Frame& sourceFrame, const Frame& targetFrame)
{
	ocean_assert(sourceFrame.isValid() && targetFrame.isValid());

	if (!sourceFrame.isValid() || !targetFrame.isValid())
	{
		return false;
	}

	if (targetFrame.frameType() != sourceFrame.frameType())
	{
		return false;
	}

	for (unsigned int planeIndex = 0u; planeIndex < sourceFrame.numberPlanes(); ++planeIndex)
	{
		const unsigned int bytesPerPlanePixel = sourceFrame.bytesPerDataType() * sourceFrame.planeChannels(planeIndex);

		const unsigned int sourcePlaneWidth = sourceFrame.planeWidth(planeIndex);
		const unsigned int sourcePlaneHeight = sourceFrame.planeHeight(planeIndex);

		for (unsigned int y = 0u; y < sourcePlaneHeight; ++y)
		{
			const unsigned int yRotated = sourcePlaneHeight - y - 1u;

			for (unsigned int x = 0u; x < sourcePlaneWidth; ++x)
			{
				const unsigned int xRotated = sourcePlaneWidth - x - 1u;

				const void* sourcePixel = sourceFrame.constpixel<void>(x, y, planeIndex);
				const void* targetPixel = targetFrame.constpixel<void>(xRotated, yRotated, planeIndex);

				if (memcmp(sourcePixel, targetPixel, bytesPerPlanePixel) != 0)
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TestFrameTransposer::validateRotate(const Frame& sourceFrame, const Frame& targetFrame, const int angle)
{
	ocean_assert(sourceFrame.isValid() && targetFrame.isValid());

	if (!sourceFrame.isValid() || !targetFrame.isValid())
	{
		return false;
	}

	if (!sourceFrame.isFrameTypeCompatible(FrameType(targetFrame, sourceFrame.width(), sourceFrame.height()), false))
	{
		return false;
	}

	const Scalar positiveAngle = Numeric::angleAdjustPositive(Numeric::deg2rad(Scalar(angle)));

	const Quaternion rotation(Vector3(0, 0, 1), sourceFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT ? positiveAngle : -positiveAngle);

	const Vector2 halfPixelOffset = Vector2(Scalar(0.5), Scalar(0.5));

	for (unsigned int planeIndex = 0u; planeIndex < sourceFrame.numberPlanes(); ++planeIndex)
	{
		const unsigned int bytesPerPlanePixel = sourceFrame.bytesPerDataType() * sourceFrame.planeChannels(planeIndex);

		const unsigned int sourcePlaneWidth = sourceFrame.planeWidth(planeIndex);
		const unsigned int sourcePlaneHeight = sourceFrame.planeHeight(planeIndex);

		const unsigned int targetPlaneWidth = targetFrame.planeWidth(planeIndex);
		const unsigned int targetPlaneHeight = targetFrame.planeHeight(planeIndex);

		const Vector2 sourceCenter(Scalar(sourcePlaneWidth) * Scalar(0.5), Scalar(sourcePlaneHeight) * Scalar(0.5));
		const Vector2 targetCenter(Scalar(targetPlaneWidth) * Scalar(0.5), Scalar(targetPlaneHeight) * Scalar(0.5));

		for (unsigned int y = 0u; y < sourcePlaneHeight; ++y)
		{
			for (unsigned int x = 0u; x < sourcePlaneWidth; ++x)
			{
				const Vector2 targetPosition = targetCenter + (rotation * Vector3(Vector2(Scalar(x), Scalar(y)) - sourceCenter + halfPixelOffset, 1)).xy() - halfPixelOffset;

				const int xRotated = Numeric::round32(targetPosition.x());
				const int yRotated = Numeric::round32(targetPosition.y());

				if (xRotated < 0 || xRotated >= int(targetPlaneWidth) || yRotated < 0 || yRotated >= int(targetPlaneHeight))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				const void* sourcePixel = sourceFrame.constpixel<void>(x, y, planeIndex);
				const void* targetPixel = targetFrame.constpixel<void>((unsigned int)(xRotated), (unsigned int)(yRotated), planeIndex);

				if (memcmp(sourcePixel, targetPixel, bytesPerPlanePixel) != 0)
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TestFrameTransposer::rotate(const Frame& sourceFrame, Frame& targetFrame, const int angle, Worker* worker)
{
	ocean_assert(sourceFrame.isValid());
	ocean_assert(targetFrame.isValid());

	switch (sourceFrame.dataType())
	{
		case FrameType::DT_SIGNED_INTEGER_8:
		case FrameType::DT_UNSIGNED_INTEGER_8:
			return rotate<uint8_t>(sourceFrame, targetFrame, angle, worker);

		case FrameType::DT_SIGNED_INTEGER_16:
		case FrameType::DT_UNSIGNED_INTEGER_16:
		case FrameType::DT_SIGNED_FLOAT_16:
			return rotate<uint16_t>(sourceFrame, targetFrame, angle, worker);

		case FrameType::DT_SIGNED_INTEGER_32:
		case FrameType::DT_UNSIGNED_INTEGER_32:
		case FrameType::DT_SIGNED_FLOAT_32:
			return rotate<uint32_t>(sourceFrame, targetFrame, angle, worker);

		case FrameType::DT_SIGNED_INTEGER_64:
		case FrameType::DT_UNSIGNED_INTEGER_64:
		case FrameType::DT_SIGNED_FLOAT_64:
			return rotate<uint64_t>(sourceFrame, targetFrame, angle, worker);

		case FrameType::DT_UNDEFINED:
		case FrameType::DT_END:
			break;
	}

	ocean_assert(false && "Invalid data type!");
	return false;
}

template <typename T>
bool TestFrameTransposer::rotate(const Frame& sourceFrame, Frame& targetFrame, const int angle, Worker* worker)
{
	ocean_assert(sourceFrame.isValid());
	ocean_assert(targetFrame.isValid());

	for (unsigned int planeIndex = 0u; planeIndex < sourceFrame.numberPlanes(); ++planeIndex)
	{
		switch (sourceFrame.planeChannels(planeIndex))
		{
			case 1u:
				CV::FrameTransposer::rotate<T, 1u>(sourceFrame.constdata<T>(planeIndex), targetFrame.data<T>(planeIndex), sourceFrame.planeWidth(planeIndex), sourceFrame.planeHeight(planeIndex), angle, sourceFrame.paddingElements(planeIndex), targetFrame.paddingElements(planeIndex), worker);
				break;

			case 2u:
				CV::FrameTransposer::rotate<T, 2u>(sourceFrame.constdata<T>(planeIndex), targetFrame.data<T>(planeIndex), sourceFrame.planeWidth(planeIndex), sourceFrame.planeHeight(planeIndex), angle, sourceFrame.paddingElements(planeIndex), targetFrame.paddingElements(planeIndex), worker);
				break;

			case 3u:
				CV::FrameTransposer::rotate<T, 3u>(sourceFrame.constdata<T>(planeIndex), targetFrame.data<T>(planeIndex), sourceFrame.planeWidth(planeIndex), sourceFrame.planeHeight(planeIndex), angle, sourceFrame.paddingElements(planeIndex), targetFrame.paddingElements(planeIndex), worker);
				break;

			case 4u:
				CV::FrameTransposer::rotate<T, 4u>(sourceFrame.constdata<T>(planeIndex), targetFrame.data<T>(planeIndex), sourceFrame.planeWidth(planeIndex), sourceFrame.planeHeight(planeIndex), angle, sourceFrame.paddingElements(planeIndex), targetFrame.paddingElements(planeIndex), worker);
				break;

			case 5u:
				CV::FrameTransposer::rotate<T, 5u>(sourceFrame.constdata<T>(planeIndex), targetFrame.data<T>(planeIndex), sourceFrame.planeWidth(planeIndex), sourceFrame.planeHeight(planeIndex), angle, sourceFrame.paddingElements(planeIndex), targetFrame.paddingElements(planeIndex), worker);
				break;

			default:
				ocean_assert(false && "Invalid channel number!");
				return false;
		}
	}

	return true;
}

}

}

}
