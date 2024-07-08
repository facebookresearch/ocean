/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameInterpolatorNearestPixel.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"

#include "ocean/geometry/Homography.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameInterpolatorNearestPixel::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width != 0u && height != 0u && testDuration > 0.0);

	Log::info() << "---   Nearest pixel interpolation test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testAffine(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomography<uint8_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomography<float>(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomographyMask(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testResize(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSpecialCasesResize400x400To224x224_8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTransform<true>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTransform<false>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTransformMask<true>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTransformMask<false>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRotate90(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Nearest pixel interpolation test succeeded.";
	}
	else
	{
		Log::info() << "Nearest pixel interpolation test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameInterpolatorNearestPixel, Homography_1980x1080_UnsignedChar_1Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testHomography<uint8_t>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Homography_1980x1080_UnsignedChar_2Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testHomography<uint8_t>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Homography_1980x1080_UnsignedChar_3Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testHomography<uint8_t>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Homography_1980x1080_UnsignedChar_4Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testHomography<uint8_t>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameInterpolatorNearestPixel, Homography_1980x1080_Float_1Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testHomography<float>(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Homography_1980x1080_Float_2Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testHomography<float>(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Homography_1980x1080_Float_3Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testHomography<float>(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Homography_1980x1080_Float_4Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testHomography<float>(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameInterpolatorNearestPixel, HomographyMask_1920x1080_1channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testHomographyMask(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorNearestPixel, HomographyMask_1920x1080_2channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testHomographyMask(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorNearestPixel, HomographyMask_1920x1080_3channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testHomographyMask(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorNearestPixel, HomographyMask_1920x1080_4channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testHomographyMask(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker));
}


// Affine Transform

TEST(TestFrameInterpolatorNearestPixel, Affine_random_UnsignedChar_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testAffine(0u, 0u, 1u /* channels */, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorNearestPixel, Affine_random_UnsignedChar_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testAffine(0u, 0u, 2u /* channels */, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorNearestPixel, Affine_random_UnsignedChar_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testAffine(0u, 0u, 3u /* channels */, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorNearestPixel, Affine_random_UnsignedChar_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testAffine(0u, 0u, 4u /* channels */, GTEST_TEST_DURATION, worker));
}


TEST(TestFrameInterpolatorNearestPixel, Affine_1980x1080_UnsignedChar_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testAffine(1920u, 1080u, 1u /* channels */, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorNearestPixel, Affine_1980x1080_UnsignedChar_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testAffine(1920u, 1080u, 2u /* channels */, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorNearestPixel, Affine_1980x1080_UnsignedChar_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testAffine(1920u, 1080u, 3u /* channels */, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorNearestPixel, Affine_1980x1080_UnsignedChar_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testAffine(1920u, 1080u, 4u /* channels */, GTEST_TEST_DURATION, worker));
}

// Resize

TEST(TestFrameInterpolatorNearestPixel, Resize_1920x1080_1850x980_UnsignedChar_1Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testResize<uint8_t, 1u>(1920u, 1080u, 1850u, 980u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Resize_1920x1080_1850x980_UnsignedChar_2Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testResize<uint8_t, 2u>(1920u, 1080u, 1850u, 980u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Resize_1920x1080_1850x980_UnsignedChar_3Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testResize<uint8_t, 3u>(1920u, 1080u, 1850u, 980u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Resize_1920x1080_1850x980_UnsignedChar_4Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testResize<uint8_t, 4u>(1920u, 1080u, 1850u, 980u, GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameInterpolatorNearestPixel, Resize_1920x1080_1850x980_Float_1Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testResize<float, 1u>(1920u, 1080u, 1850u, 980u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Resize_1920x1080_1850x980_Float_2Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testResize<float, 2u>(1920u, 1080u, 1850u, 980u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Resize_1920x1080_1850x980_Float_3Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testResize<float, 3u>(1920u, 1080u, 1850u, 980u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Resize_1920x1080_1850x980_Float_4Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testResize<float, 4u>(1920u, 1080u, 1850u, 980u, GTEST_TEST_DURATION, worker)));
}


// Special case resize functions

TEST(TestFrameInterpolatorNearestPixel, SpecialCasesResize400x400To224x224_8BitPerChannel)
{
	EXPECT_TRUE(TestFrameInterpolatorNearestPixel::testSpecialCasesResize400x400To224x224_8BitPerChannel(GTEST_TEST_DURATION));
}


// Transform

TEST(TestFrameInterpolatorNearestPixel, Transform_1920x1080_Offset)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testTransform<true>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Transform_1920x1080_Absolute)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testTransform<false>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


// Transform mask

TEST(TestFrameInterpolatorNearestPixel, TransformMask_1920x1080_Offset)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testTransformMask<true>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, TransformMask_1920x1080_Absolute)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testTransformMask<false>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}


// Rotate90

TEST(TestFrameInterpolatorNearestPixel, Rotate90_1920x1080_UnsignedChar_1Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testRotate90<uint8_t, 1u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Rotate90_1920x1080_UnsignedChar_2Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testRotate90<uint8_t, 2u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Rotate90_1920x1080_UnsignedChar_3Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testRotate90<uint8_t, 3u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Rotate90_1920x1080_UnsignedChar_4Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testRotate90<uint8_t, 4u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Rotate90_1920x1080_Double_1Channel)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testRotate90<double, 1u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Rotate90_1920x1080_Double_2Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testRotate90<double, 2u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Rotate90_1920x1080_Double_3Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testRotate90<double, 3u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameInterpolatorNearestPixel, Rotate90_1920x1080_Double_4Channels)
{
	Worker worker;
	EXPECT_TRUE((TestFrameInterpolatorNearestPixel::testRotate90<double, 4u>(1920u, 1080u, GTEST_TEST_DURATION, worker)));
}

#endif // OCEAN_USE_GTEST

bool TestFrameInterpolatorNearestPixel::testAffine(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Affine transformation interpolation test:";
	Log::info() << " ";

	bool allSucceeded = true;

	const std::vector<unsigned int> widths = {0u, 640u, 800u, 1280u, 1920u, 3840u};
	const std::vector<unsigned int> heights = {0u, 480u, 640u, 720u, 1080u, 2160u};
	ocean_assert(widths.size() == heights.size());

	for (unsigned int i = 0u; i < widths.size(); ++i)
	{
		const unsigned int width = widths[i];
		const unsigned int height = heights[i];

		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testAffine(width, height, channel, testDuration, worker) && allSucceeded;
			Log::info() << " ";
		}
	}

	return allSucceeded;
}

bool TestFrameInterpolatorNearestPixel::testAffine(const unsigned int width0, const unsigned int height0, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(channels >= 1u && channels <= 4u);

	bool allSucceeded = true;
	const bool randomImageSize = width0 == 0u || height0 == 0u;
	const unsigned int maxRandomImageWidth = 1920u;
	const unsigned int maxRandomImageHeight = 1080u;

	RandomGenerator randomGenerator;

	if (randomImageSize)
	{
		Log::info() << "... frame: random size (max. " << maxRandomImageWidth << " x " << maxRandomImageHeight << "px), " << channels << " channels:";
	}
	else
	{
		Log::info() << "... frame: " << width0 << " x " << height0 << "px, " << channels << " channels:";
	}

	const double maxErrorThreshold = 25.0;
	double globalMaximalAbsError = 0.0;

	const Timestamp startTimestamp(true);

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	do
	{
		// Sizes of random input and output (and intermediate blurring image)
		const unsigned int width = randomImageSize ? RandomI::random(randomGenerator, 1u, maxRandomImageWidth) : width0;
		const unsigned int height = randomImageSize ? RandomI::random(randomGenerator, 1u, maxRandomImageHeight) : height0;
		const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		const unsigned int targetFrameWidth = std::max(1u, RandomI::random(randomGenerator, (unsigned int)(Scalar(0.75) * Scalar(width)), (unsigned int)(Scalar(1.25) * Scalar(width))));
		const unsigned int targetFrameHeight = std::max(1u, RandomI::random(randomGenerator, (unsigned int)(Scalar(0.75) * Scalar(height)), (unsigned int)(Scalar(1.25) * Scalar(height))));
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		const unsigned int gaussianFilterSize = 11u;
		const unsigned int randomFrameWidth = width + gaussianFilterSize + sourcePaddingElements;
		const unsigned int randomFrameHeight = height + gaussianFilterSize;

		// Random affine transformation
		const Vector2 randomTranslation = Random::vector2(randomGenerator, -Scalar(width) * Scalar(0.05), Scalar(width) * Scalar(0.05), -Scalar(height) * Scalar(0.05), Scalar(height) * Scalar(0.05));
		const Vector2 randomScale = Random::vector2(randomGenerator, Scalar(0.90), Scalar(1.1), Scalar(0.90), Scalar(1.1));
		const Scalar randomRotation = Random::scalar(randomGenerator, Numeric::deg2rad(-10), Numeric::deg2rad(10));

		SquareMatrix3 randomAffine(Quaternion(Vector3(0, 0, 1), randomRotation));
		randomAffine(0, 0) *= randomScale.x();
		randomAffine(1, 0) *= randomScale.x();
		randomAffine(0, 1) *= randomScale.y();
		randomAffine(1, 1) *= randomScale.y();
		randomAffine(0, 2) = randomTranslation.x();
		randomAffine(1, 2) = randomTranslation.y();
		ocean_assert(randomAffine.isSingular() == false);

		for (unsigned int i = 0u; i < 2u; ++i)
		{
			const bool isSingleCore = i == 0u;

			Worker* currentWorker = isSingleCore ? nullptr : &worker;
			HighPerformanceStatistic& performance = isSingleCore ? performanceSinglecore : performanceMulticore;

			const unsigned int randomFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame randomFrame(FrameType(randomFrameWidth, randomFrameHeight, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), randomFramePaddingElements);
			CV::CVUtilities::randomizeFrame(randomFrame, false, &randomGenerator);

			const unsigned int blurredRandomFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
			Frame blurredRandomFrame(randomFrame.frameType(), blurredRandomFramePaddingElements);

			CV::FrameFilterGaussian::filter(randomFrame, blurredRandomFrame, gaussianFilterSize);

			Frame sourceFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
			sourceFrame.copy(blurredRandomFrame.subFrame(0u, 0u, width, height));

			Frame targetFrame(FrameType(sourceFrame, targetFrameWidth, targetFrameHeight), targetPaddingElements);
			CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

			const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			std::vector<uint8_t> backgroundColor(channels);
			for (size_t n = 0; n < backgroundColor.size(); ++n)
			{
				backgroundColor[n] = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			CV::PixelPositionI targetFrameOriginOffset(RandomI::random(randomGenerator, -5, 5), RandomI::random(randomGenerator, -5, 5));

			// Performance
			performance.start();
			switch (sourceFrame.channels())
			{
				case 1u:
					CV::FrameInterpolatorNearestPixel::affine8BitPerChannel<1u>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), randomAffine, backgroundColor.data(), targetFrame.data<uint8_t>(), targetFrameOriginOffset, targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), currentWorker);
					break;

				case 2u:
					CV::FrameInterpolatorNearestPixel::affine8BitPerChannel<2u>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), randomAffine, backgroundColor.data(), targetFrame.data<uint8_t>(), targetFrameOriginOffset, targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), currentWorker);
					break;

				case 3u:
					CV::FrameInterpolatorNearestPixel::affine8BitPerChannel<3u>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), randomAffine, backgroundColor.data(), targetFrame.data<uint8_t>(), targetFrameOriginOffset, targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), currentWorker);
					break;

				case 4u:
					CV::FrameInterpolatorNearestPixel::affine8BitPerChannel<4u>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), randomAffine, backgroundColor.data(), targetFrame.data<uint8_t>(), targetFrameOriginOffset, targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), currentWorker);
					break;

				default:
					ocean_assert(false && "Never be here!");
					break;
			}
			performance.stop();

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			// Validation
			double averageAbsError = NumericD::maxValue();
			double maximalAbsError = NumericD::maxValue();
			validateHomography<uint8_t>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), sourceFrame.channels(), randomAffine, backgroundColor.data(), targetFrameOriginOffset, &maximalAbsError, &averageAbsError);

			globalMaximalAbsError = std::max(globalMaximalAbsError, maximalAbsError);
			allSucceeded = maximalAbsError <= maxErrorThreshold && allSucceeded;

			if (maximalAbsError > maxErrorThreshold)
			{
				Log::info() << "Validation failure:";
				Log::info() << "  Error: " << maximalAbsError;
				Log::info() << "  Random seed: " << randomGenerator.seed();
				Log::info() << "  Input: " << width << " x " << height << ", padding = " << sourcePaddingElements << ", channels = " << channels;
				Log::info() << "  Output: " << targetFrameWidth << " x " << targetFrameHeight << ", padding = " << targetPaddingElements << ", channels = " << channels;
				Log::info() << " ";
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (randomImageSize == false)
	{
		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core Performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, median: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
		}
	}

	Log::info() << "Validation: " << (allSucceeded ? "successful" : "FAILED") << ", max error: " << globalMaximalAbsError;

	return allSucceeded;
}

template <typename T>
bool TestFrameInterpolatorNearestPixel::testHomography(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const IndexPairs32 dimensions =
	{
		{640u, 480u},
		{1280u, 720u},
		{1920u, 1080u},
		{3840u, 2160u}
	};

	Log::info() << "Homography interpolation test:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const IndexPair32& dimension : dimensions)
	{
		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testHomography<T>(dimension.first, dimension.second, channel, testDuration, worker) && allSucceeded;
			Log::info() << " ";
		}

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Homography interpolation validation succeeded.";
	}
	else
	{
		Log::info() << "Homography interpolation validation FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameInterpolatorNearestPixel::testHomography(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(width >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels and data type '" << TypeNamer::name<T>() << "':";

	std::vector<T> backgroundColor(channels);

	RandomGenerator randomGenerator;

	double sumAverageError = 0.0;
	double maximalError = 0.0;
	unsigned long long measurements = 0ull;

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
			for (bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 20u, 100u);
				const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 20u, 100u);

				const Scalar maxTranslation = Scalar(std::min(testWidth, testHeight)) * Scalar(0.25);

				const SquareMatrix3 transformation = Geometry::Utilities::createRandomHomography(testWidth, testHeight, maxTranslation);

				const unsigned int sourceFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				Frame sourceFrame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), sourceFramePaddingElements);
				CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator, true);

				CV::FrameFilterGaussian::filter(sourceFrame, 7u, &worker);

				const unsigned int targetFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				ocean_assert(sourceFrame.width() > 10u);
				Frame targetFrame(FrameType(sourceFrame, RandomI::random(sourceFrame.width() - 10u, sourceFrame.width() + 10u), RandomI::random(sourceFrame.height() - 10u, sourceFrame.height() + 10u)), targetFramePaddingElements);
				CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator, true);

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				for (size_t n = 0; n < backgroundColor.size(); ++n)
				{
					backgroundColor[n] = T(RandomI::random(randomGenerator, 255));
				}

				CV::PixelPositionI targetFrameOriginOffset(RandomI::random(-5, 5), RandomI::random(-5, 5));

				performance.startIf(performanceIteration);
				CV::FrameInterpolatorNearestPixel::Comfort::homography(sourceFrame, targetFrame, transformation, backgroundColor.data(), useWorker, targetFrameOriginOffset);
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				double localAverageError = NumericD::maxValue();
				double localMaximalError = NumericD::maxValue();
				validateHomography<T>(sourceFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), sourceFrame.paddingElements(), targetFrame.constdata<T>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), sourceFrame.channels(), transformation, backgroundColor.data(), targetFrameOriginOffset, &localMaximalError, &localAverageError);

				sumAverageError += localAverageError;
				maximalError = std::max(maximalError, localMaximalError);
				measurements++;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
		Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 2u) << "x";
	}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	const double averageErrorThreshold = 0.5;
#else
	const double averageErrorThreshold = 0.1;
#endif

	ocean_assert(measurements != 0ull);
	const double averageAbsError = sumAverageError / double(measurements);

	const bool allSucceeded = averageAbsError <= averageErrorThreshold;

	Log::info() << "Validation: average error: " << averageAbsError;

	if (!allSucceeded)
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorNearestPixel::testHomographyMask(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const IndexPairs32 dimensions =
	{
		{640u, 480u},
		{1280u, 720u},
		{1920u, 1080u},
		{3840u, 2160u}
	};

	Log::info() << "Homography mask interpolation test:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const IndexPair32& dimension : dimensions)
	{
		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testHomographyMask(dimension.first, dimension.second, channel, testDuration, worker) && allSucceeded;
			Log::info() << " ";
		}

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Homography interpolation validation succeeded.";
	}
	else
	{
		Log::info() << "Homography interpolation validation FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorNearestPixel::testHomographyMask(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(width >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		// we use the worker (multi-core execution in the second iteration)
		HighPerformanceStatistic& performance = workerIteration == 0u ? performanceSinglecore : performanceMulticore;
		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int inputFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame inputFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), inputFramePaddingElements);
			CV::CVUtilities::randomizeFrame(inputFrame, false, &randomGenerator);

			ocean_assert(inputFrame.width() > 10u);
			const unsigned int outputWidth = RandomI::random(randomGenerator, inputFrame.width() - 10u, inputFrame.width() + 10u);
			const unsigned int outputHeight = RandomI::random(randomGenerator, inputFrame.height() - 10u, inputFrame.height() + 10u);

			const unsigned int outputFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame outputFrame(FrameType(inputFrame, outputWidth, outputHeight), outputFramePaddingElements);
			CV::CVUtilities::randomizeFrame(outputFrame, false, &randomGenerator);

			const unsigned int outputMaskPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame outputMask(FrameType(outputFrame, FrameType::FORMAT_Y8), outputMaskPaddingElements);
			CV::CVUtilities::randomizeFrame(outputMask, false, &randomGenerator);

			const Frame copyOutputFrame(outputFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
			const Frame copyOutputMask(outputMask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			const Vector2 cornersInput[4] =
			{
				Vector2(0, 0),
				Vector2(0, Scalar(inputFrame.height() - 1u)),
				Vector2(Scalar(inputFrame.width() - 1u), Scalar(inputFrame.height() - 1u)),
				Vector2(Scalar(inputFrame.width() - 1u), 0)
			};

			const Vector2 cornersOutput[4] =
			{
				Random::vector2(randomGenerator, -20, 20),
				Vector2(0, Scalar(outputFrame.height() - 1u)) + Random::vector2(randomGenerator, -20, 20),
				Vector2(Scalar(outputFrame.width() - 1u), Scalar(outputFrame.height() - 1u)) + Random::vector2(randomGenerator, -20, 20),
				Vector2(Scalar(outputFrame.width() - 1u), 0) + Random::vector2(randomGenerator, -20, 20)
			};

			SquareMatrix3 input_H_output(false);
			if (Geometry::Homography::homographyMatrix(cornersOutput, cornersInput, 4, input_H_output))
			{
				CV::PixelPositionI outputFrameOriginOffset(RandomI::random(-5, 5), RandomI::random(-5, 5));

				performance.start();
					CV::FrameInterpolatorNearestPixel::Comfort::homographyMask(inputFrame, outputFrame, outputMask, input_H_output, useWorker, 0xFF, outputFrameOriginOffset);
				performance.stop();

				if (!CV::CVUtilities::isPaddingMemoryIdentical(outputFrame, copyOutputFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(outputMask, copyOutputMask))
				{
					ocean_assert(false && "Invalid padding memory!");
				}

				if (!validateHomographyMask8BitPerChannel(inputFrame, outputFrame, outputMask, input_H_output, outputFrameOriginOffset))
				{
					allSucceeded = false;
				}
			}
			else
			{
				allSucceeded = false;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Median performance: " << performanceSinglecore.medianMseconds() << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Median performance (multicore): " << performanceMulticore.medianMseconds() << "ms";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorNearestPixel::testResize(const double testDuration, Worker& worker)
{
	Log::info() << "Frame resizing test:";
	Log::info() << " ";

	const IndexPairs32 sourceResolutions =
	{
		IndexPair32(640u, 480u),
		IndexPair32(1920u, 1080u),
	};

	const IndexPairs32 targetResolutions =
	{
		IndexPair32(600u, 420u),
		IndexPair32(1850u, 980u),
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

		allSucceeded = testResize<uint8_t, 1u>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<uint8_t, 2u>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<uint8_t, 3u>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<uint8_t, 4u>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;

		Log::info() << " ";
		Log::info() << " ";

		allSucceeded = testResize<float, 1u>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<float, 2u>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<float, 3u>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testResize<float, 4u>(sourceWidth, sourceHeight, targetWidth, targetHeight, testDuration, worker) && allSucceeded;
	}

	return allSucceeded;
}

template <typename T, unsigned int tChannels>
bool TestFrameInterpolatorNearestPixel::testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration, Worker& worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number");

	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... resizing " << sourceWidth << "x" << sourceHeight << " -> " << targetWidth << "x" << targetHeight << ", with '" << TypeNamer::name<T>() << "' and " << tChannels << " channels:";

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<T, tChannels>();

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		// we use the worker (multi-core execution in the second iteration)
		HighPerformanceStatistic& performance = workerIteration == 0u ? performanceSinglecore : performanceMulticore;
		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testSourceWidth = performanceIteration ? sourceWidth : RandomI::random(randomGenerator, 1u, 2000u);
				const unsigned int testSourceHeight = performanceIteration ? sourceHeight : RandomI::random(randomGenerator, 1u, 2000u);

				const unsigned int testTargetWidth = performanceIteration ? targetWidth : RandomI::random(randomGenerator, 1u, 2000u);
				const unsigned int testTargetHeight = performanceIteration ? targetHeight : RandomI::random(randomGenerator, 1u, 2000u);

				const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				Frame sourceFrame(FrameType(testSourceWidth, testSourceHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame targetFrame(FrameType(testTargetWidth, testTargetHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

				CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.start();
				CV::FrameInterpolatorNearestPixel::resize<T, tChannels>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.paddingElements(), targetFrame.paddingElements(), useWorker);
				performance.stop();

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid memory!");
					return false;
				}

				if (!validateResizedFrame<T>(sourceFrame.constdata<T>(), targetFrame.constdata<T>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), tChannels, sourceFrame.paddingElements(), targetFrame.paddingElements()))
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

bool TestFrameInterpolatorNearestPixel::testSpecialCasesResize400x400To224x224_8BitPerChannel(const double testDuration)
{
	Log::info() << "Testing special case resize 400x400 to 224x224, FORMAT_Y8:";

	bool allSucceeded = true;

	unsigned int iterations = 0u;

	RandomGenerator randomGenerator;

	HighPerformanceStatistic performanceGeneral;
	HighPerformanceStatistic performanceSpecial;

	const Timestamp startTimestamp(true);

	const FrameType sourceFrameType(400u, 400u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT);
	const FrameType targetFrameType(sourceFrameType, 224u, 224u);

	do
	{
		const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame sourceFrame(sourceFrameType, sourcePaddingElements);

		Frame targetFrameSpecial(targetFrameType, targetPaddingElements);
		Frame targetFrameGeneral(targetFrameType, targetPaddingElements);

		CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(targetFrameSpecial, false, &randomGenerator);
		CV::CVUtilities::randomizeFrame(targetFrameGeneral, false, &randomGenerator);

		const Frame copyTargetFrameSpecial(targetFrameSpecial, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		for (const unsigned int iteration : {0u, 1u})
		{
			if (iterations % 2u == iteration) // toggling the execution order
			{
				performanceSpecial.start();
				CV::FrameInterpolatorNearestPixel::SpecialCases::resize400x400To224x224_8BitPerChannel(sourceFrame.constdata<uint8_t>(), targetFrameSpecial.data<uint8_t>(), sourceFrame.paddingElements(), targetFrameSpecial.paddingElements());
				performanceSpecial.stop();
			}
			else
			{
				performanceGeneral.start();
				CV::FrameInterpolatorNearestPixel::resize<uint8_t, 1u>(sourceFrame.constdata<uint8_t>(), targetFrameGeneral.data<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrameGeneral.width(), targetFrameGeneral.height(), sourceFrame.paddingElements(), targetFrameGeneral.paddingElements());
				performanceGeneral.stop();
			}
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrameSpecial, copyTargetFrameSpecial))
		{
			ocean_assert(false && "Invalid padding memory!");
			allSucceeded = false;
			break;
		}

		for (unsigned int y = 0u; y < targetFrameSpecial.height(); ++y)
		{
			if (memcmp(targetFrameSpecial.constrow<void>(y), targetFrameGeneral.constrow<void>(y), targetFrameSpecial.planeWidthBytes(0u)) != 0)
			{
				allSucceeded = false;
			}
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "General performance: Best: " << String::toAString(performanceGeneral.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceGeneral.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceGeneral.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceGeneral.medianMseconds(), 3u) << "ms";
	Log::info() << "Special performance: Best: " << String::toAString(performanceSpecial.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSpecial.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSpecial.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSpecial.medianMseconds(), 3u) << "ms";

	Log::info() << "Special boost: " << String::toAString(performanceGeneral.medianMseconds() / performanceSpecial.medianMseconds(), 2u) << "x";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <bool tOffset>
bool TestFrameInterpolatorNearestPixel::testTransform(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Frame lookup transformation on a " << width << "x" << height << " frame with " << (tOffset ? "offset" : "absolute") << " lookup:";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	CV::FrameInterpolatorNearestPixel::LookupTable lookupTable(width, height, 20u, 20u);

	for (unsigned int yBin = 0u; yBin <= lookupTable.binsY(); ++yBin)
	{
		for (unsigned int xBin = 0u; xBin <= lookupTable.binsX(); ++xBin)
		{
			if constexpr (tOffset)
			{
				lookupTable.setBinTopLeftCornerValue(xBin, yBin, Random::vector2(randomGenerator, -20, 20));
			}
			else
			{
				const Vector2 position = lookupTable.binTopLeftCornerPosition(xBin, yBin);

				lookupTable.setBinTopLeftCornerValue(xBin, yBin, position + Random::vector2(randomGenerator, -20, 20));
			}
		}
	}

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info() << "... with " << channels << " channels:";

		const FrameType frameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT);

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				Frame frame(frameType, framePaddingElements);
				Frame target(frameType, targetPaddingElements);

				CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);

				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				std::vector<uint8_t> borderColor(channels);

				for (uint8_t& value : borderColor)
				{
					value = uint8_t(RandomI::random(randomGenerator, 255u));
				}

				performance.start();
				CV::FrameInterpolatorNearestPixel::Comfort::transform(frame, target, lookupTable, tOffset, borderColor.data(), useWorker);
				performance.stop();

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					ocean_assert(false && "Invalid memory!");
					return false;
				}

				if (!validateTransformation<tOffset>(frame, target, lookupTable, borderColor.data()))
				{
					allSucceeded = false;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 2u) << "x";
		}

		Log::info() << " ";
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

template <bool tOffset>
bool TestFrameInterpolatorNearestPixel::testTransformMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Frame mask lookup transformation on a " << width << "x" << height << " frame with " << (tOffset ? "offset" : "absolute") << " lookup:";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	CV::FrameInterpolatorNearestPixel::LookupTable lookupTable(width, height, 20u, 20u);

	for (unsigned int yBin = 0u; yBin <= lookupTable.binsY(); ++yBin)
	{
		for (unsigned int xBin = 0u; xBin <= lookupTable.binsX(); ++xBin)
		{
			if constexpr (tOffset)
			{
				lookupTable.setBinTopLeftCornerValue(xBin, yBin, Random::vector2(randomGenerator, -20, 20));
			}
			else
			{
				const Vector2 position = lookupTable.binTopLeftCornerPosition(xBin, yBin);

				lookupTable.setBinTopLeftCornerValue(xBin, yBin, position + Random::vector2(randomGenerator, -20, 20));
			}
		}
	}

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info() << "... with " << channels << " channels:";

		const FrameType frameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT);

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetMaskPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				Frame frame(frameType, framePaddingElements);
				Frame target(frameType, targetPaddingElements);
				Frame targetMask(FrameType(frameType, FrameType::FORMAT_Y8), targetMaskPaddingElements);

				CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(target, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(targetMask, false, &randomGenerator);

				const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));

				const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame copyTargetMask(targetMask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.start();
					CV::FrameInterpolatorNearestPixel::Comfort::transformMask(frame, target, targetMask, lookupTable, tOffset, useWorker, maskValue);
				performance.stop();

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, copyTarget))
				{
					ocean_assert(false && "Invalid memory!");
					return false;
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetMask, copyTargetMask))
				{
					ocean_assert(false && "Invalid memory!");
					return false;
				}

				if (!validateTransformationMask<tOffset>(frame, target, targetMask, lookupTable, maskValue))
				{
					allSucceeded = false;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
			Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 2u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 2u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 2u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 2u) << "x";
		}

		Log::info() << " ";
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

bool TestFrameInterpolatorNearestPixel::testRotate90(const double testDuration, Worker& worker)
{
	const IndexPairs32 resolutions =
	{
		IndexPair32(640u, 480u),
		IndexPair32(643u, 477u),
		IndexPair32(1920u, 1080u),
		IndexPair32(1919u, 1081u)
	};

	ocean_assert(resolutions.empty() == false);

	bool allSucceeded = true;

	for (unsigned int n = 0u; n < resolutions.size(); ++n)
	{
		Log::info().newLine(n != 0u);
		Log::info().newLine(n != 0u);

		const unsigned int width = resolutions[n].first;
		const unsigned int height = resolutions[n].second;

		allSucceeded = testRotate90<uint8_t, 1u>(width, height, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testRotate90<uint8_t, 2u>(width, height, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testRotate90<uint8_t, 3u>(width, height, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testRotate90<uint8_t, 4u>(width, height, testDuration, worker) && allSucceeded;

		Log::info() << " ";
		Log::info() << " ";

		allSucceeded = testRotate90<double, 1u>(width, height, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testRotate90<double, 2u>(width, height, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testRotate90<double, 3u>(width, height, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testRotate90<double, 4u>(width, height, testDuration, worker) && allSucceeded;
	}

	return allSucceeded;
}

template <typename TElementType, unsigned int tChannels>
bool TestFrameInterpolatorNearestPixel::testRotate90(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test 90 degree rotation (size: " << width << " x " << height << ", channels: " << tChannels << ", type: " << TypeNamer::name<TElementType>() << "):";

	constexpr FrameType::DataType pixelFormatDataType = FrameType::dataType<TElementType>();

	bool allSucceeded = true;

	HighPerformanceStatistic performanceClockwiseSinglecore;
	HighPerformanceStatistic performanceClockwiseMulticore;

	HighPerformanceStatistic performanceCounterClockwiseSinglecore;
	HighPerformanceStatistic performanceCounterClockwiseMulticore;

	RandomGenerator randomGenerator;

	for (unsigned int workerIteration = 0u; workerIteration < 2u; ++workerIteration)
	{
		HighPerformanceStatistic& performanceClockwise = workerIteration == 0u ? performanceClockwiseSinglecore : performanceClockwiseMulticore;
		HighPerformanceStatistic& performanceCounterClockwise = workerIteration == 0u ? performanceCounterClockwiseSinglecore : performanceCounterClockwiseMulticore;

		Worker* useWorker = workerIteration == 0u ? nullptr : &worker;

		const Timestamp startTimestamp(true);

		do
		{
			for (const bool performanceIteration : {true, false})
			{
				const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 2000u);
				const unsigned int testHeight = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 2000u);

				if constexpr (pixelFormatDataType == FrameType::DT_UNSIGNED_INTEGER_8 || pixelFormatDataType == FrameType::DT_SIGNED_INTEGER_8)
				{
					const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
					const unsigned int clockwisePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
					const unsigned int counterClockwisePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

					Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat(pixelFormatDataType, tChannels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);

					CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);

					Frame clockwiseFrame;

					if (clockwisePaddingElements != 0u)
					{
						clockwiseFrame = Frame(FrameType(frame, frame.height(), frame.width()), clockwisePaddingElements);
					}

					performanceClockwise.startIf(performanceIteration);
						if (!CV::FrameInterpolatorNearestPixel::Comfort::rotate90(frame, clockwiseFrame, true /*clockwise*/, useWorker))
						{
							allSucceeded = false;
						}
					performanceClockwise.stopIf(performanceIteration);

					if (frame.width() != clockwiseFrame.height() || frame.height() != clockwiseFrame.width())
					{
						allSucceeded = false;
					}

					Frame counterClockwiseFrame;

					if (counterClockwisePaddingElements != 0u)
					{
						counterClockwiseFrame = Frame(FrameType(frame, frame.height(), frame.width()), counterClockwisePaddingElements);
					}

					performanceCounterClockwise.startIf(performanceIteration);
						if (!CV::FrameInterpolatorNearestPixel::Comfort::rotate90(frame, counterClockwiseFrame, false /*clockwise*/, useWorker))
						{
							allSucceeded = false;
						}
					performanceCounterClockwise.stopIf(performanceIteration);

					if (frame.width() != clockwiseFrame.height() || frame.height() != clockwiseFrame.width())
					{
						allSucceeded = false;
					}

					if (clockwiseFrame && counterClockwiseFrame)
					{
						if (!validateRotate90(frame.constdata<TElementType>(), tChannels, frame.width(), frame.height(), clockwiseFrame.constdata<TElementType>(), counterClockwiseFrame.constdata<TElementType>(), frame.paddingElements(), clockwiseFrame.paddingElements(), counterClockwiseFrame.paddingElements()))
						{
							allSucceeded = false;
						}
					}
				}

				const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				const unsigned int clockwisePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				const unsigned int counterClockwisePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

				Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat(pixelFormatDataType, tChannels), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
				Frame clockwiseFrame(FrameType(frame, frame.height(), frame.width()), clockwisePaddingElements);
				Frame counterClockwiseFrame(FrameType(frame, frame.height(), frame.width()), counterClockwisePaddingElements);

				CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(clockwiseFrame, false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(counterClockwiseFrame, false, &randomGenerator);

				const Frame clockwiseFrameCopy(clockwiseFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame counterClockwiseFrameCopy(counterClockwiseFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				switch (tChannels)
				{
					case 1u:
					{
						performanceClockwise.startIf(performanceIteration);
						CV::FrameInterpolatorNearestPixel::rotate90<TElementType, 1u>(frame.constdata<TElementType>(), clockwiseFrame.data<TElementType>(), frame.width(), frame.height(), true, frame.paddingElements(), clockwiseFrame.paddingElements(), useWorker);
						performanceClockwise.stopIf(performanceIteration);

						performanceCounterClockwise.startIf(performanceIteration);
						CV::FrameInterpolatorNearestPixel::rotate90<TElementType, 1u>(frame.constdata<TElementType>(), counterClockwiseFrame.data<TElementType>(), frame.width(), frame.height(), false, frame.paddingElements(), counterClockwiseFrame.paddingElements(), useWorker);
						performanceCounterClockwise.stopIf(performanceIteration);
						break;
					}

					case 2u:
					{
						performanceClockwise.startIf(performanceIteration);
						CV::FrameInterpolatorNearestPixel::rotate90<TElementType, 2u>(frame.constdata<TElementType>(), clockwiseFrame.data<TElementType>(), frame.width(), frame.height(), true, frame.paddingElements(), clockwiseFrame.paddingElements(), useWorker);
						performanceClockwise.stopIf(performanceIteration);

						performanceCounterClockwise.startIf(performanceIteration);
						CV::FrameInterpolatorNearestPixel::rotate90<TElementType, 2u>(frame.constdata<TElementType>(), counterClockwiseFrame.data<TElementType>(), frame.width(), frame.height(), false, frame.paddingElements(), counterClockwiseFrame.paddingElements(), useWorker);
						performanceCounterClockwise.stopIf(performanceIteration);
						break;
					}

					case 3u:
					{
						performanceClockwise.startIf(performanceIteration);
						CV::FrameInterpolatorNearestPixel::rotate90<TElementType, 3u>(frame.constdata<TElementType>(), clockwiseFrame.data<TElementType>(), frame.width(), frame.height(), true, frame.paddingElements(), clockwiseFrame.paddingElements(), useWorker);
						performanceClockwise.stopIf(performanceIteration);

						performanceCounterClockwise.startIf(performanceIteration);
						CV::FrameInterpolatorNearestPixel::rotate90<TElementType, 3u>(frame.constdata<TElementType>(), counterClockwiseFrame.data<TElementType>(), frame.width(), frame.height(), false, frame.paddingElements(), counterClockwiseFrame.paddingElements(), useWorker);
						performanceCounterClockwise.stopIf(performanceIteration);
						break;
					}

					case 4u:
					{
						performanceClockwise.startIf(performanceIteration);
						CV::FrameInterpolatorNearestPixel::rotate90<TElementType, 4u>(frame.constdata<TElementType>(), clockwiseFrame.data<TElementType>(), frame.width(), frame.height(), true, frame.paddingElements(), clockwiseFrame.paddingElements(), useWorker);
						performanceClockwise.stopIf(performanceIteration);

						performanceCounterClockwise.startIf(performanceIteration);
						CV::FrameInterpolatorNearestPixel::rotate90<TElementType, 4u>(frame.constdata<TElementType>(), counterClockwiseFrame.data<TElementType>(), frame.width(), frame.height(), false, frame.paddingElements(), counterClockwiseFrame.paddingElements(), useWorker);
						performanceCounterClockwise.stopIf(performanceIteration);
						break;
					}
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(clockwiseFrame, clockwiseFrameCopy) || !CV::CVUtilities::isPaddingMemoryIdentical(counterClockwiseFrame, counterClockwiseFrameCopy))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (!validateRotate90(frame.constdata<TElementType>(), tChannels, frame.width(), frame.height(), clockwiseFrame.constdata<TElementType>(), counterClockwiseFrame.constdata<TElementType>(), frame.paddingElements(), clockwiseFrame.paddingElements(), counterClockwiseFrame.paddingElements()))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Performance (clockwise):";
		Log::info() << "Single-core: " << String::toAString(performanceClockwiseSinglecore.bestMseconds(), 3u) << " / " << String::toAString(performanceClockwiseSinglecore.worstMseconds(), 3u) << " / " << String::toAString(performanceClockwiseSinglecore.averageMseconds(), 3u) << " / " << String::toAString(performanceClockwiseSinglecore.medianMseconds(), 3u) << " ms";
		if (performanceClockwiseMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core: " << String::toAString(performanceClockwiseMulticore.bestMseconds(), 3u) << " / " << String::toAString(performanceClockwiseMulticore.worstMseconds(), 3u) << " / " << String::toAString(performanceClockwiseMulticore.averageMseconds(), 3u) << " / " << String::toAString(performanceClockwiseMulticore.medianMseconds(), 3u) << " ms";
			Log::info() << "Multi-core boost: " << String::toAString(performanceClockwiseSinglecore.best() / performanceClockwiseMulticore.best(), 1u) << " / " << String::toAString(performanceClockwiseSinglecore.worst() / performanceClockwiseMulticore.worst(), 1u) << " / " << String::toAString(performanceClockwiseSinglecore.average() / performanceClockwiseMulticore.average(), 1u) << " / " << String::toAString(performanceClockwiseSinglecore.median() / performanceClockwiseMulticore.median(), 1u) << " x";
		}

		Log::info() << "Performance (counter-clockwise):";
		Log::info() << "Single-core: " << String::toAString(performanceCounterClockwiseSinglecore.bestMseconds(), 3u) << " / " << String::toAString(performanceCounterClockwiseSinglecore.worstMseconds(), 3u) << " / " << String::toAString(performanceCounterClockwiseSinglecore.averageMseconds(), 3u) << " / " << String::toAString(performanceCounterClockwiseSinglecore.medianMseconds(), 3u) << " ms";
		if (performanceCounterClockwiseMulticore.measurements() != 0u)
		{
			Log::info() << "Multi-core: " << String::toAString(performanceCounterClockwiseMulticore.bestMseconds(), 3u) << " / " << String::toAString(performanceCounterClockwiseMulticore.worstMseconds(), 3u) << " / " << String::toAString(performanceCounterClockwiseMulticore.averageMseconds(), 3u) << " / " << String::toAString(performanceCounterClockwiseMulticore.medianMseconds(), 3u) << " ms";
			Log::info() << "Multi-core boost: " << String::toAString(performanceCounterClockwiseSinglecore.best() / performanceCounterClockwiseMulticore.best(), 1u) << " / " << String::toAString(performanceCounterClockwiseSinglecore.worst() / performanceCounterClockwiseMulticore.worst(), 1u) << " / " << String::toAString(performanceCounterClockwiseSinglecore.average() / performanceCounterClockwiseMulticore.average(), 1u) << " / " << String::toAString(performanceCounterClockwiseSinglecore.median() / performanceCounterClockwiseMulticore.median(), 1u) << " x";
		}
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

template <typename T>
bool TestFrameInterpolatorNearestPixel::validateResizedFrame(const T* source, const T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source && target);

	ocean_assert(sourceWidth != 0u && sourceHeight != 0u);
	ocean_assert(targetWidth != 0u && targetHeight != 0u);

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;

	for (unsigned int yt = 0u; yt < targetHeight; ++yt)
	{
		const unsigned int ys = yt * sourceHeight / targetHeight;

		const T* sourceRow = source + ys * sourceStrideElements;
		const T* targetRow = target + yt * targetStrideElements;

		for (unsigned int xt = 0u; xt < targetWidth; ++xt)
		{
			const unsigned int xs = xt * sourceWidth / targetWidth;

			const T* sourcePixel = sourceRow + xs * channels;
			const T* targetPixel = targetRow + xt * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				if (sourcePixel[n] != targetPixel[n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <bool tOffset>
bool TestFrameInterpolatorNearestPixel::validateTransformation(const Frame& frame, const Frame& target, const CV::FrameInterpolatorNearestPixel::LookupTable& lookupTable, const uint8_t* borderColor)
{
	ocean_assert(frame.isValid() && target.isValid());
	ocean_assert(size_t(target.width()) == lookupTable.sizeX());
	ocean_assert(size_t(target.height()) == lookupTable.sizeY());

	const unsigned int channels = target.channels();

	for (unsigned int y = 0u; y < target.height(); ++y)
	{
		const uint8_t* targetRow = target.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < target.width(); ++x)
		{
			const Vector2 lookupValue = lookupTable.bilinearValue(Scalar(x), Scalar(y));
			const Vector2 inputPosition = tOffset ? lookupValue + Vector2(Scalar(x), Scalar(y)) : lookupValue;

			const int xInput = Numeric::round32(inputPosition.x());
			const int yInput = Numeric::round32(inputPosition.y());

			const uint8_t* targetPixel = targetRow + x * channels;

			if (xInput >= 0 && xInput < int(frame.width()) && yInput >= 0 && yInput < int(frame.height()))
			{
				const uint8_t* inputPixel = frame.constpixel<uint8_t>((unsigned int)(xInput), (unsigned int)(yInput));

				if (memcmp(inputPixel, targetPixel, sizeof(uint8_t) * channels) != 0)
				{
					return false;
				}
			}
			else
			{
				if (memcmp(borderColor, targetPixel, sizeof(uint8_t) * channels) != 0)
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <bool tOffset>
bool TestFrameInterpolatorNearestPixel::validateTransformationMask(const Frame& frame, const Frame& target, const Frame& targetMask, const CV::FrameInterpolatorNearestPixel::LookupTable& lookupTable, const uint8_t maskValue)
{
	ocean_assert(frame.isValid() && target.isValid());
	ocean_assert(size_t(target.width()) == lookupTable.sizeX());
	ocean_assert(size_t(target.height()) == lookupTable.sizeY());

	ocean_assert(target.isFrameTypeCompatible(FrameType(targetMask, target.pixelFormat()), false));

	const unsigned int channels = target.channels();

	const uint8_t nonMaskValue = 0xFFu - maskValue;

	for (unsigned int y = 0u; y < target.height(); ++y)
	{
		const uint8_t* targetRow = target.constrow<uint8_t>(y);
		const uint8_t* targetMaskRow = targetMask.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < target.width(); ++x)
		{
			const Vector2 lookupValue = lookupTable.bilinearValue(Scalar(x), Scalar(y));
			const Vector2 inputPosition = tOffset ? lookupValue + Vector2(Scalar(x), Scalar(y)) : lookupValue;

			const int xInput = Numeric::round32(inputPosition.x());
			const int yInput = Numeric::round32(inputPosition.y());

			const uint8_t* targetPixel = targetRow + x * channels;
			const uint8_t targetMaskPixel = targetMaskRow[x];

			if (xInput >= 0 && xInput < int(frame.width()) && yInput >= 0 && yInput < int(frame.height()))
			{
				const uint8_t* inputPixel = frame.constpixel<uint8_t>((unsigned int)(xInput), (unsigned int)(yInput));

				if (memcmp(inputPixel, targetPixel, sizeof(uint8_t) * channels) != 0)
				{
					return false;
				}

				if (targetMaskPixel != maskValue)
				{
					return false;
				}
			}
			else
			{
				if (targetMaskPixel != nonMaskValue)
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename TElementType>
bool TestFrameInterpolatorNearestPixel::validateRotate90(const TElementType* frame, const unsigned int channels, const unsigned int width, const unsigned int height, const TElementType* clockwise, const TElementType* counterClockwise, const unsigned int framePaddingElements, const unsigned int clockwisePaddingElements, const unsigned int counterClockwisePaddingElements)
{
	ocean_assert(frame != nullptr && clockwise != nullptr && counterClockwise != nullptr);
	ocean_assert(channels >= 1u && width >= 1u && height >= 1u);

	const unsigned int clockwiseWidth = height;
	const unsigned int clockwiseHeight = width;

	const unsigned int counterClockwiseWidth = height;
	const unsigned int counterClockwiseHeight = width;

	const unsigned int frameStrideElements = width * channels + framePaddingElements;

	const unsigned int clockwiseStrideElements = clockwiseWidth * channels + clockwisePaddingElements;
	const unsigned int counterClockwiseStrideElements = counterClockwiseWidth * channels + counterClockwisePaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const unsigned int clockwiseX = height - y - 1u;
			const unsigned int clockwiseY = x;

			ocean_assert_and_suppress_unused(clockwiseX < clockwiseWidth && clockwiseY < clockwiseHeight, clockwiseHeight);

			const unsigned int counterClockwiseX = y;
			const unsigned int counterClockwiseY = width - x - 1u;

			ocean_assert_and_suppress_unused(counterClockwiseX < counterClockwiseWidth && counterClockwiseY < counterClockwiseHeight, counterClockwiseHeight);

			const TElementType* framePixel = frame + y * frameStrideElements + x * channels;
			const TElementType* clockwisePixel = clockwise + clockwiseY * clockwiseStrideElements + clockwiseX * channels;
			const TElementType* counterClockwisePixel = counterClockwise + counterClockwiseY * counterClockwiseStrideElements + counterClockwiseX * channels;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				if (framePixel[n] != clockwisePixel[n])
				{
					return false;
				}

				if (framePixel[n] != counterClockwisePixel[n])
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TestFrameInterpolatorNearestPixel::validateHomographyMask8BitPerChannel(const Frame& inputFrame, const Frame& outputFrame, const Frame& outputMask, const SquareMatrix3& input_H_output, const CV::PixelPositionI& outputFrameOrigin)
{
	ocean_assert(inputFrame.isValid() && outputFrame.isValid() && outputMask.isValid());
	ocean_assert(inputFrame.isPixelFormatCompatible(outputFrame.pixelFormat()));

	ocean_assert(outputFrame.isFrameTypeCompatible(FrameType(outputMask, outputFrame.pixelFormat()), false));

	ocean_assert(!input_H_output.isSingular());

	unsigned int invalidValues = 0u;

	for (unsigned int y = 0u; y < outputFrame.height(); ++y)
	{
		for (unsigned int x = 0u; x < outputFrame.width(); ++x)
		{
			const uint8_t* const outputPixel = outputFrame.constpixel<uint8_t>(x, y);
			const uint8_t outputMaskPixel = outputMask.constpixel<uint8_t>(x, y)[0];

			ocean_assert(outputMaskPixel == 0x00 || outputMaskPixel == 0xFF);

			const Vector2 outputPosition = Vector2(Scalar(x) + Scalar(outputFrameOrigin.x()), Scalar(y) + Scalar(outputFrameOrigin.y()));
			const Vector2 inputPosition = input_H_output * outputPosition;

			const unsigned int inputX = (unsigned int)(Numeric::round32(inputPosition.x()));
			const unsigned int inputY = (unsigned int)(Numeric::round32(inputPosition.y()));

			if (inputX < inputFrame.width() && inputY < inputFrame.height())
			{
				const uint8_t* inputPixel = inputFrame.constpixel<uint8_t>(inputX, inputY);

				ocean_assert(inputFrame.channels() == inputFrame.channels());

				for (unsigned int n = 0u; n < std::max(inputFrame.channels(), outputFrame.channels()); ++n)
				{
					if (outputPixel[n] != inputPixel[n] || outputMaskPixel != 0xFF)
					{
						invalidValues++;
					}
				}
			}
			else
			{
				if (outputMaskPixel != 0x00)
				{
					invalidValues++;
				}
			}
		}
	}

	ocean_assert(invalidValues <= outputFrame.size()); // size - as we determine invalid values per pixel and channel
	const double percent = double(outputFrame.size() - invalidValues) / double(outputFrame.size());

	return percent >= 0.995;
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
