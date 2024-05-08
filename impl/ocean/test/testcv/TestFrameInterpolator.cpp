/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameInterpolator.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Processor.h"

#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameInterpolator::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   FrameInterpolator test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testResize(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testResizeUseCase(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "FrameInterpolator test succeeded.";
	}
	else
	{
		Log::info() << "FrameInerplator test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

// RM_NEAREST_PIXEL
TEST(TestFrameInterpolator, ResizeUnsignedChar1Channel_1920x1080_400x235_NearestPixel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<1u, CV::FrameInterpolator::RM_NEAREST_PIXEL>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar2Channel_1920x1080_400x235_NearestPixel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<2u, CV::FrameInterpolator::RM_NEAREST_PIXEL>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar3Channel_1920x1080_400x235_NearestPixel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<3u, CV::FrameInterpolator::RM_NEAREST_PIXEL>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar4Channel_1920x1080_400x235_NearestPixel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<4u, CV::FrameInterpolator::RM_NEAREST_PIXEL>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}


// Bilinear
TEST(TestFrameInterpolator, ResizeUnsignedChar1Channel_1920x1080_400x235_Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<1u, CV::FrameInterpolator::RM_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar2Channel_1920x1080_400x235_Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<2u, CV::FrameInterpolator::RM_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar3Channel_1920x1080_400x235_Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<3u, CV::FrameInterpolator::RM_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar4Channel_1920x1080_400x235_Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<4u, CV::FrameInterpolator::RM_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}


// Nearest pyramid layer 11, bilinear
TEST(TestFrameInterpolator, ResizeUnsignedChar1Channel_1920x1080_400x235_NearestPyramidLayer11Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<1u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_11_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar2Channel_1920x1080_400x235_NearestPyramidLayer11Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<2u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_11_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar3Channel_1920x1080_400x235_NearestPyramidLayer11Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<3u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_11_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar4Channel_1920x1080_400x235_NearestPyramidLayer11Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<4u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_11_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}


// Nearest pyramid layer 14641, bilinear
TEST(TestFrameInterpolator, ResizeUnsignedChar1Channel_1920x1080_400x235_NearestPyramidLayer14641Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<1u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar2Channel_1920x1080_400x235_NearestPyramidLayer14641Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<2u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar3Channel_1920x1080_400x235_NearestPyramidLayer14641Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<3u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolator, ResizeUnsignedChar4Channel_1920x1080_400x235_NearestPyramidLayer14641Bilinear)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolator::testResize<4u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR>(1920u, 1080u, 400u, 235u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameInterpolator, ResizeUseCase_1)
{
	EXPECT_TRUE(TestFrameInterpolator::testResizeUseCase(GTEST_TEST_DURATION, 1u));
}

TEST(TestFrameInterpolator, ResizeUseCase_2)
{
	EXPECT_TRUE(TestFrameInterpolator::testResizeUseCase(GTEST_TEST_DURATION, 2u));
}

TEST(TestFrameInterpolator, ResizeUseCase_3)
{
	EXPECT_TRUE(TestFrameInterpolator::testResizeUseCase(GTEST_TEST_DURATION, 3u));
}

TEST(TestFrameInterpolator, ResizeUseCase_4)
{
	EXPECT_TRUE(TestFrameInterpolator::testResizeUseCase(GTEST_TEST_DURATION, 4u));
}

TEST(TestFrameInterpolator, ResizeUseCase_5)
{
	EXPECT_TRUE(TestFrameInterpolator::testResizeUseCase(GTEST_TEST_DURATION, 5u));
}

TEST(TestFrameInterpolator, ResizeUseCase_6)
{
	EXPECT_TRUE(TestFrameInterpolator::testResizeUseCase(GTEST_TEST_DURATION, 6u));
}

TEST(TestFrameInterpolator, ResizeUseCase_7)
{
	EXPECT_TRUE(TestFrameInterpolator::testResizeUseCase(GTEST_TEST_DURATION, 7u));
}

TEST(TestFrameInterpolator, ResizeUseCase_8)
{
	EXPECT_TRUE(TestFrameInterpolator::testResizeUseCase(GTEST_TEST_DURATION, 8u));
}

#endif // OCEAN_USE_GTEST

bool TestFrameInterpolator::testResize(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pixel interpolation test:";
	Log::info() << " ";

	const IndexPairs32 sourceResolutions =
	{
		IndexPair32(1920u, 1080u),
	};

	const IndexPairs32 targetResolutions =
	{
		IndexPair32(400u, 235u),
	};

	ocean_assert(sourceResolutions.size() == targetResolutions.size());

	bool allSucceeded = true;

	for (unsigned int n = 0u; n < sourceResolutions.size(); ++n)
	{
		Log::info().newLine(n != 0u);
		Log::info().newLine(n != 0u);

		const unsigned int sourceWidth = sourceResolutions[n].first;
		const unsigned int sourceHeight = sourceResolutions[n].second;

		const unsigned int targetWidth = targetResolutions[n].first;
		const unsigned int targetHeight = targetResolutions[n].second;

		allSucceeded = testResize<1u, CV::FrameInterpolator::RM_NEAREST_PIXEL>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<2u, CV::FrameInterpolator::RM_NEAREST_PIXEL>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<3u, CV::FrameInterpolator::RM_NEAREST_PIXEL>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<4u, CV::FrameInterpolator::RM_NEAREST_PIXEL>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;

		Log::info() << " ";
		Log::info() << " ";

		allSucceeded = testResize<1u, CV::FrameInterpolator::RM_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<2u, CV::FrameInterpolator::RM_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<3u, CV::FrameInterpolator::RM_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<4u, CV::FrameInterpolator::RM_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;

		Log::info() << " ";
		Log::info() << " ";

		allSucceeded = testResize<1u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_11_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<2u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_11_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<3u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_11_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<4u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_11_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;

		Log::info() << " ";
		Log::info() << " ";

		allSucceeded = testResize<1u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<2u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<3u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<4u, CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
	}

	return allSucceeded;
}

template <unsigned int tChannels, CV::FrameInterpolator::ResizeMethod tResizeMethod>
bool TestFrameInterpolator::testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration, Worker& worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(sourceWidth >= 4u && sourceHeight >= 4u);
	ocean_assert(targetWidth >= 4u && targetHeight >= 4u);
	ocean_assert(testDuration > 0.0);

	const std::vector<std::string> resizeMethodStrings =
	{
		"Invalid",
		"Nearest Pixel",
		"Bilinear",
		"Pyramid-based with 11 filter, Bilinear",
		"Pyramid-based with 14641 filter, Bilinear"
	};

	ocean_assert(size_t(tResizeMethod) < resizeMethodStrings.size());

	Log::info() << "... for '" << resizeMethodStrings[size_t(tResizeMethod)] << "' resizing " << sourceWidth << "x" << sourceHeight << " -> " << targetWidth << "x" << targetHeight << " with " << tChannels << " channels:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const Timestamp startTimestamp(true);

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		do
		{
			unsigned int useSourceWidth = sourceWidth;
			unsigned int useSourceHeight = sourceHeight;

			unsigned int useTargetWidth = targetWidth;
			unsigned int useTargetHeight = targetHeight;

			FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t, tChannels>();

			for (const bool benchmarkIteration : {true, false})
			{
				if (!benchmarkIteration)
				{
					do
					{
						pixelFormat = RandomI::random(randomGenerator, CV::CVUtilities::definedPixelFormats());
					}
					while (FrameType::dataType(pixelFormat) != FrameType::DT_UNSIGNED_INTEGER_8 || FrameType::formatIsPacked(pixelFormat)); // FrameInterpolator::resize() does not support packed pixel formats or data types other than uint8_t

					const unsigned int widthMultiple = FrameType::widthMultiple(pixelFormat);
					const unsigned int heightMultiple = FrameType::heightMultiple(pixelFormat);

					useSourceWidth = RandomI::random(randomGenerator, 2u / widthMultiple, useSourceWidth / widthMultiple) * widthMultiple;
					useSourceHeight = RandomI::random(randomGenerator, 2u / heightMultiple, useSourceHeight / heightMultiple) * heightMultiple;

					useTargetWidth = RandomI::random(randomGenerator, 2u / widthMultiple, useTargetWidth / widthMultiple) * widthMultiple;
					useTargetHeight = RandomI::random(randomGenerator, 2u / heightMultiple, useTargetHeight / heightMultiple) * heightMultiple;
				}

				const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(useSourceWidth, useSourceHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
				Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(useTargetWidth, useTargetHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				if (benchmarkIteration)
				{
					ocean_assert(sourceFrame.numberPlanes() == 1u);

					performance.start();
						const bool result = CV::FrameInterpolator::resize<uint8_t, tChannels, tResizeMethod>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
					performance.stop();

					if (!result)
					{
						allSucceeded = false;
					}
				}
				else
				{
					if (!CV::FrameInterpolator::resize(sourceFrame, targetFrame, tResizeMethod, useWorker))
					{
						allSucceeded = false;
					}
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validateResizedFrame(sourceFrame, targetFrame, tResizeMethod))
				{
					allSucceeded = false;
				}

				Frame copySourceFrame(sourceFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				if (CV::FrameInterpolator::resize(copySourceFrame, targetFrame.width(), targetFrame.height(), tResizeMethod, useWorker))
				{
					if (!validateResizedFrame(sourceFrame, copySourceFrame, tResizeMethod))
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

bool TestFrameInterpolator::testResizeUseCase(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing resize() with focus on production use case:";

	bool allSucceeded = true;

	for (unsigned int workerThreads : {1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 12u, 15u, 16u})
	{
		Log::info() << " ";

		if (!testResizeUseCase(testDuration, workerThreads))
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Resize with production use case validation: succeeded.";
	}
	else
	{
		Log::info() << "Resize with production use case validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolator::testResizeUseCase(const double testDuration, const unsigned int workerThreads)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << workerThreads << " worker threads:";

	const std::vector<CV::FrameInterpolator::ResizeMethod> resizeMethods =
	{
		CV::FrameInterpolator::RM_NEAREST_PIXEL,
		CV::FrameInterpolator::RM_BILINEAR,
		CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_11_BILINEAR,
		CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR,
		CV::FrameInterpolator::RM_AUTOMATIC
	};

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_Y8, FrameType::FORMAT_YA16, FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32,
		FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::genericPixelFormat<uint8_t, 2u>(), FrameType::genericPixelFormat<uint8_t, 3u>(), FrameType::genericPixelFormat<uint8_t, 4u>()
	};

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	Worker worker(workerThreads, Worker::TYPE_CUSTOM);

	const Timestamp startTimestamp(true);

	do
	{
		for (const int targetMaxDimension : {32, 64, 128, 384, 480, 512})
		{
			const unsigned int sourceWidth = RandomI::random(randomGenerator, 1u, 2000u);
			const unsigned int sourceHeight = RandomI::random(randomGenerator, 1u, 2000u);

			const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);
			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, pixelFormat, pixelOrigin), &randomGenerator);

			const int width = int(frame.width());
			const int height = int(frame.height());

			int targetWidth = width;
			int targetHeight = height;

			if (width > height)
			{
				targetWidth = targetMaxDimension;
				targetHeight = int(1.0f * float(height) / float(width) * float(targetWidth));

				targetHeight -= targetHeight % 32; // ensure targetHeight is a multiple of 32
			}
			else
			{
				targetHeight = targetMaxDimension;
				targetWidth = int(1.0f * float(width) / float(height) * float(targetHeight));

				targetWidth -= targetWidth % 32; // ensure targetWidth is a multiple of 32
			}

#ifdef OCEAN_DEBUG

			// adding several checks to prevent asserts in debug builds
			// however, we skip this part in release builds

			ocean_assert(targetWidth >= 0 && targetHeight >= 0);

			if (targetWidth == 0 || targetHeight == 0)
			{
				Log::debug() << "Skipped resolution " << sourceWidth << "x" << sourceHeight << " for " << targetMaxDimension;
				continue;
			}

			if (targetWidth < 1 || targetHeight < 1)
			{
				allSucceeded = false;
				continue;
			}

			ocean_assert(targetWidth % 32 == 0 && targetHeight % 32 == 0);

			if (targetWidth % 32 != 0 || targetHeight % 32 != 0)
			{
				allSucceeded = false;
				continue;
			}

#else

			// in release builds, we allow a targetWidth == 0 || targetHeight == 0 to ensure that the code does not crash

#endif // OCEAN_DEBUG

			const FrameType resizedFrameType(frame.frameType(), (unsigned int)(targetWidth), (unsigned int)(targetHeight));

			Frame resizedFrame = CV::CVUtilities::randomizedFrame(resizedFrameType, &randomGenerator);

			const Frame copyResizedFrame(resizedFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			const CV::FrameInterpolator::ResizeMethod resizeMethod = RandomI::random(randomGenerator, resizeMethods);

			Worker* useWorker = RandomI::boolean() ? &worker : nullptr;

			if (!CV::FrameInterpolator::resize(frame, resizedFrame, resizeMethod, useWorker))
			{
#ifdef OCEAN_DEBUG
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
#else
				if (targetWidth != 0 && targetHeight != 0)
				{
					allSucceeded = false;
				}
#endif
			}

			if (targetWidth != 0 && targetHeight != 0)
			{
				if (!CV::CVUtilities::isPaddingMemoryIdentical(resizedFrame, copyResizedFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validateResizedFrame(frame, resizedFrame, resizeMethod))
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

bool TestFrameInterpolator::validateResizedFrame(const Frame& source, const Frame& target, const CV::FrameInterpolator::ResizeMethod resizeMethod)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(source.pixelFormat() == target.pixelFormat() && source.pixelOrigin() == target.pixelOrigin());

	if (!source.isValid() || !target.isValid())
	{
		return false;
	}

	if (source.dataType() != FrameType::DT_UNSIGNED_INTEGER_8 || !source.isPixelFormatCompatible(target.pixelFormat()))
	{
		return false;
	}

	for (unsigned int planeIndex = 0u; planeIndex < source.numberPlanes(); ++planeIndex)
	{
		const unsigned int sourcePlaneWidth = source.planeWidth(planeIndex);
		const unsigned int sourcePlaneHeight = source.planeHeight(planeIndex);
		const unsigned int sourcePlanePaddingElements = source.paddingElements(planeIndex);

		const unsigned int planeChannels = source.planeChannels(planeIndex);
		ocean_assert(planeChannels == target.planeChannels(planeIndex));

		const FrameType::PixelFormat planePixelFormat = FrameType::genericPixelFormat<uint8_t>(planeChannels);

		const Frame sourcePlane(FrameType(sourcePlaneWidth, sourcePlaneHeight, planePixelFormat, source.pixelOrigin()), source.constdata<uint8_t>(planeIndex), Frame::CM_USE_KEEP_LAYOUT, sourcePlanePaddingElements);

		const unsigned int targetPlaneWidth = target.planeWidth(planeIndex);
		const unsigned int targetPlaneHeight = target.planeHeight(planeIndex);

		Frame targetPlane(FrameType(targetPlaneWidth, targetPlaneHeight, planePixelFormat, target.pixelOrigin()));

		switch (sourcePlane.channels())
		{
			case 1u:
			{
				if (!resizePlane<1u>(sourcePlane, targetPlane, resizeMethod))
				{
					return false;
				}

				break;
			}

			case 2u:
			{
				if (!resizePlane<2u>(sourcePlane, targetPlane, resizeMethod))
				{
					return false;
				}

				break;
			}

			case 3u:
			{
				if (!resizePlane<3u>(sourcePlane, targetPlane, resizeMethod))
				{
					return false;
				}

				break;
			}

			case 4u:
			{
				if (!resizePlane<4u>(sourcePlane, targetPlane, resizeMethod))
				{
					return false;
				}

				break;
			}

			default:
				ocean_assert(false && "This should never happen!");
				return false;
		}

		const unsigned int targetPlaneWidthBytes = target.planeWidthBytes(planeIndex);
		ocean_assert(targetPlaneWidthBytes == targetPlane.planeWidthBytes(0u));

		for (unsigned int y = 0u; y < targetPlane.height(); ++y)
		{
			const uint8_t* targetPlaneRow = targetPlane.constrow<uint8_t>(y);
			const uint8_t* targetFrameRow = target.constrow<uint8_t>(y, planeIndex);

			if (memcmp(targetPlaneRow, targetFrameRow, targetPlaneWidthBytes) != 0)
			{
				return false;
			}
		}
	}

	return true;
}

template <unsigned int tPlaneChannels>
bool TestFrameInterpolator::resizePlane(const Frame& sourcePlane, Frame& targetPlane, const CV::FrameInterpolator::ResizeMethod resizeMethod)
{
	static_assert(tPlaneChannels >= 1u, "Invalid number of channels!");

	ocean_assert(sourcePlane.isValid() && targetPlane.isValid());
	ocean_assert(sourcePlane.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	if (!sourcePlane.isPixelFormatCompatible(targetPlane.pixelFormat()))
	{
		return false;
	}

	if (resizeMethod == CV::FrameInterpolator::RM_NEAREST_PIXEL)
	{
		CV::FrameInterpolatorNearestPixel::resize<uint8_t, tPlaneChannels>(sourcePlane.constdata<uint8_t>(), targetPlane.data<uint8_t>(), sourcePlane.width(), sourcePlane.height(), targetPlane.width(), targetPlane.height(), sourcePlane.paddingElements(), targetPlane.paddingElements());
	}
	else if (resizeMethod == CV::FrameInterpolator::RM_BILINEAR)
	{
		CV::FrameInterpolatorBilinear::resize<uint8_t, tPlaneChannels>(sourcePlane.constdata<uint8_t>(), targetPlane.data<uint8_t>(), sourcePlane.width(), sourcePlane.height(), targetPlane.width(), targetPlane.height(), sourcePlane.paddingElements(), targetPlane.paddingElements());
	}
	else if (resizeMethod == CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR)
	{
		const unsigned int layers = CV::FramePyramid::idealLayers(sourcePlane.width(), sourcePlane.height(), targetPlane.width() - 1u, targetPlane.height() - 1u);

		if (layers >= 2u)
		{
			const CV::FramePyramid framePyramid(sourcePlane, CV::FramePyramid::DM_FILTER_14641, layers, true /*copyFirstLayer*/, nullptr);

			const Frame& coarsestPyramidLayer = framePyramid.coarsestLayer();

			CV::FrameInterpolatorBilinear::resize<uint8_t, tPlaneChannels>(coarsestPyramidLayer.constdata<uint8_t>(), targetPlane.data<uint8_t>(), coarsestPyramidLayer.width(), coarsestPyramidLayer.height(), targetPlane.width(), targetPlane.height(), coarsestPyramidLayer.paddingElements(), targetPlane.paddingElements());
		}
		else
		{
			CV::FrameInterpolatorBilinear::resize<uint8_t, tPlaneChannels>(sourcePlane.constdata<uint8_t>(), targetPlane.data<uint8_t>(), sourcePlane.width(), sourcePlane.height(), targetPlane.width(), targetPlane.height(), sourcePlane.paddingElements(), targetPlane.paddingElements());
		}
	}
	else if (resizeMethod == CV::FrameInterpolator::RM_NEAREST_PYRAMID_LAYER_11_BILINEAR)
	{
		const unsigned int layers = CV::FramePyramid::idealLayers(sourcePlane.width(), sourcePlane.height(), targetPlane.width() - 1u, targetPlane.height() - 1u);

		if (layers >= 2u)
		{
			const CV::FramePyramid framePyramid(sourcePlane, CV::FramePyramid::DM_FILTER_11, layers, true /*copyFirstLayer*/, nullptr);

			const Frame& coarsestPyramidLayer = framePyramid.coarsestLayer();

			CV::FrameInterpolatorBilinear::resize<uint8_t, tPlaneChannels>(coarsestPyramidLayer.constdata<uint8_t>(), targetPlane.data<uint8_t>(), coarsestPyramidLayer.width(), coarsestPyramidLayer.height(), targetPlane.width(), targetPlane.height(), coarsestPyramidLayer.paddingElements(), targetPlane.paddingElements());
		}
		else
		{
			CV::FrameInterpolatorBilinear::resize<uint8_t, tPlaneChannels>(sourcePlane.constdata<uint8_t>(), targetPlane.data<uint8_t>(), sourcePlane.width(), sourcePlane.height(), targetPlane.width(), targetPlane.height(), sourcePlane.paddingElements(), targetPlane.paddingElements());
		}
	}
	else
	{
		ocean_assert(false && "Invalid resize method!");
		return false;
	}

	return true;
}

}

}

}
