/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testqrcodes/TestAlignmentPatternDetector.h"

#include "ocean/test/testcv/testdetector/testqrcodes/Utilities.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/detector/qrcodes/AlignmentPatternDetector.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestQRCodes
{

using namespace CV::Detector::QRCodes;

bool TestAlignmentPatternDetector::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for the alignment pattern detector:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDetectAlignmentPatternsSyntheticData(0u, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectAlignmentPatternsSyntheticData(1u, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectAlignmentPatternsSyntheticData(3u, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectAlignmentPatternsSyntheticData(5u, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectAlignmentPatternsSyntheticData(7u, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Alignment pattern detector test succeeded.";
	}
	else
	{
		Log::info() << "Alignment pattern detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAlignmentPatternDetector, TestDetectAlignmentPatternsSyntheticDataNoGaussianFilter)
{
	EXPECT_TRUE(TestAlignmentPatternDetector::testDetectAlignmentPatternsSyntheticData(0u, GTEST_TEST_DURATION));
}

TEST(TestAlignmentPatternDetector, TestDetectAlignmentPatternsSyntheticDataGaussianFilter1)
{
	EXPECT_TRUE(TestAlignmentPatternDetector::testDetectAlignmentPatternsSyntheticData(1u, GTEST_TEST_DURATION));
}

TEST(TestAlignmentPatternDetector, TestDetectAlignmentPatternsSyntheticDataGaussianFilter3)
{
	EXPECT_TRUE(TestAlignmentPatternDetector::testDetectAlignmentPatternsSyntheticData(3u, GTEST_TEST_DURATION));
}

TEST(TestAlignmentPatternDetector, TestDetectAlignmentPatternsSyntheticDataGaussianFilter5)
{
	EXPECT_TRUE(TestAlignmentPatternDetector::testDetectAlignmentPatternsSyntheticData(5u, GTEST_TEST_DURATION));
}

TEST(TestAlignmentPatternDetector, TestDetectAlignmentPatternsSyntheticDataGaussianFilter7)
{
	EXPECT_TRUE(TestAlignmentPatternDetector::testDetectAlignmentPatternsSyntheticData(7u, GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestAlignmentPatternDetector::testDetectAlignmentPatternsSyntheticData(const unsigned int gaussianFilterSize, const double testDuration)
{
	ocean_assert(gaussianFilterSize == 0u || gaussianFilterSize % 2u == 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Detect alignment patterns test using synthetic data (" << (gaussianFilterSize == 0u ? "no Gaussian filter" : "Gaussian filter: " + String::toAString(gaussianFilterSize)) + ")";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	Timestamp start(true);

	unsigned int numberTruePositives = 0u;
	unsigned int numberFalsePositives = 0u;
	unsigned int numberAllAlignmentPatterns = 0u;

	do
	{
		// Foreground and background intensities
		const unsigned int lowIntensity = RandomI::random(randomGenerator, 200u);
		const unsigned int highIntensity = RandomI::random(randomGenerator, lowIntensity + 30u, 255u);
		ocean_assert(highIntensity > lowIntensity && highIntensity - lowIntensity >= 30u);

		const bool isNormalReflectance = RandomI::random(randomGenerator, 1u) == 0u;

		const uint8_t foregroundValue = uint8_t(isNormalReflectance ? lowIntensity : highIntensity);
		const uint8_t backgroundValue = uint8_t(isNormalReflectance ? highIntensity : lowIntensity);

		const unsigned int grayThreshold = (foregroundValue + backgroundValue + 1u) / 2u;

		// Input frame
		const unsigned int frameWidth = RandomI::random(randomGenerator, 250u, 1280u);
		const unsigned int frameHeight = RandomI::random(randomGenerator, 250u, 1280u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame(FrameType(frameWidth, frameHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
		frame.setValue(backgroundValue);

		// Add synthetic alignment patterns
		const unsigned int numberAlignmentPatterns = RandomI::random(randomGenerator, 1u, 5u); // actual number may be lower

		Vectors2 alignmentPatternCenters;
		Scalars alignmentPatternSizes;
		Scalars alignmentPatternRotations;

		alignmentPatternCenters.reserve(numberAlignmentPatterns);
		alignmentPatternSizes.reserve(numberAlignmentPatterns);
		alignmentPatternRotations.reserve(numberAlignmentPatterns);

		for (unsigned int i = 0; i < numberAlignmentPatterns; ++i)
		{
			ocean_assert(alignmentPatternCenters.size() == alignmentPatternSizes.size());
			ocean_assert(alignmentPatternCenters.size() == alignmentPatternRotations.size());

			const Scalar newRotation = Random::scalar(randomGenerator, Scalar(0), Numeric::pi_2());
			const Scalar newPatternSizeInPixels = Random::scalar(randomGenerator, Scalar(20), Scalar(0.25) * Scalar(std::min(frame.width(), frame.height())));

			const Scalar border = newPatternSizeInPixels * Scalar(1.15);
			const Vector2 newCenter = Random::vector2(randomGenerator, border, Scalar(frame.width()) - border, border, Scalar(frame.height()) - border);

			bool tooClose = false;

			for (size_t iPattern = 0; iPattern < alignmentPatternCenters.size(); ++iPattern)
			{
				const Vector2& center = alignmentPatternCenters[iPattern];
				const Scalar patternSizeInPixels = alignmentPatternSizes[iPattern];

				// Minimum distance: 1.15x the sum of the diagonal pattern sizes
				const Scalar minDistance = (newPatternSizeInPixels + patternSizeInPixels) * Numeric::sqrt(Scalar(2)) * Scalar(1.15);

				if (center.distance(newCenter) <= minDistance)
				{
					tooClose = true;
					break;
				}
			}

			if (!tooClose)
			{
				drawAlignmentPattern(frame, newCenter, newPatternSizeInPixels, newRotation, foregroundValue, backgroundValue);

				alignmentPatternCenters.emplace_back(newCenter);
				alignmentPatternSizes.emplace_back(newPatternSizeInPixels);
				alignmentPatternRotations.emplace_back(newRotation);
			}
		}

		ocean_assert(alignmentPatternCenters.size() == alignmentPatternSizes.size());
		ocean_assert(alignmentPatternCenters.size() == alignmentPatternRotations.size());

		if (alignmentPatternCenters.empty())
		{
			// No test data - retry in the next iteration.
			continue;
		}

		// Add noisy samples
		const unsigned int numberNoiseSamples = RandomI::random(randomGenerator, 1u, 50u);

		Vectors2 noiseSamples;
		noiseSamples.reserve(numberNoiseSamples);

		for (unsigned int i = 0u; i < numberNoiseSamples; ++i)
		{
			const Vector2 noiseSample = Random::vector2(randomGenerator, Scalar(19u), Scalar(frame.width() - 20u), Scalar(19u), Scalar(frame.height() - 20u));

			bool tooClose = false;

			for (size_t iPattern = 0; iPattern < alignmentPatternCenters.size(); ++iPattern)
			{
				const Vector2& center = alignmentPatternCenters[iPattern];
				const Scalar patternSizeInPixels = alignmentPatternSizes[iPattern];

				if (center.distance(noiseSample) <= patternSizeInPixels * Scalar(2.15))
				{
					tooClose = true;
					break;
				}
			}

			if (!tooClose)
			{
				Utilities::drawNoisePattern(frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), noiseSample, randomGenerator, foregroundValue, /* extraBorder */ Scalar(0));

				noiseSamples.emplace_back(noiseSample);
			}
		}

		if (gaussianFilterSize != 0u)
		{
			CV::FrameFilterGaussian::filter(frame, gaussianFilterSize , WorkerPool::get().scopedWorker()());
		}

		numberAllAlignmentPatterns += (unsigned int)(alignmentPatternCenters.size());

		const AlignmentPatterns alignmentPatterns = AlignmentPatternDetector::detectAlignmentPatterns(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), /* searchX */ 0u, /* searchY */ 0u, /* searchWidth */ frame.width(), /* searchHeight */ frame.height(), isNormalReflectance, grayThreshold);

		for (size_t i = 0; i < alignmentPatternCenters.size(); ++i)
		{
			bool foundMatch = false;

			const Vector2& alignmentPatternCenter = alignmentPatternCenters[i];
			const Scalar alignmentPatternSize = alignmentPatternSizes[i];

			const Scalar segmentSize = Scalar(0.2) * alignmentPatternSize; // an alignment pattern consists of 5 segments/modules
			const Scalar maxSqrDistance = Numeric::sqr(Scalar(0.35) * segmentSize);

			for (const AlignmentPattern& alignmentPattern : alignmentPatterns)
			{
				if (alignmentPattern.center().sqrDistance(alignmentPatternCenter) <= maxSqrDistance)
				{
					const Scalar minSegmentSize = Scalar(0.2) * alignmentPatternSize * Scalar(0.5);
					const Scalar maxSegmentSize = Scalar(0.2) * alignmentPatternSize * Scalar(1.5);

					if (alignmentPattern.averageSegmentSize() >= minSegmentSize && alignmentPattern.averageSegmentSize() <= maxSegmentSize)
					{
						foundMatch = true;
						break;
					}
				}
			}

			if (foundMatch)
			{
				numberTruePositives++;
			}
		}

		// Backward check
		for (const AlignmentPattern& alignmentPattern : alignmentPatterns)
		{
			bool foundMatch = false;

			for (size_t i = 0; i < alignmentPatternCenters.size(); ++i)
			{
				const Vector2& alignmentPatternCenter = alignmentPatternCenters[i];
				const Scalar alignmentPatternSize = alignmentPatternSizes[i];

				const Scalar segmentSize = Scalar(0.2) * alignmentPatternSize;
				const Scalar maxSqrDistance = Numeric::sqr(Scalar(0.35) * segmentSize);

				if (alignmentPattern.center().sqrDistance(alignmentPatternCenter) <= maxSqrDistance)
				{
					const Scalar minSegmentSize = Scalar(0.2) * alignmentPatternSize * Scalar(0.5);
					const Scalar maxSegmentSize = Scalar(0.2) * alignmentPatternSize * Scalar(1.5);

					if (alignmentPattern.averageSegmentSize() >= minSegmentSize && alignmentPattern.averageSegmentSize() <= maxSegmentSize)
					{
						foundMatch = true;
						break;
					}
				}
			}

			if (!foundMatch)
			{
				numberFalsePositives++;
			}
		}
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

	ocean_assert(numberAllAlignmentPatterns != 0u);

	const double correctInPercent = double(numberTruePositives) / double(numberAllAlignmentPatterns);
	const double falsePositivesInPercent = double(numberFalsePositives) / double(numberAllAlignmentPatterns);

	Log::info() << "Correct detections: " << String::toAString(correctInPercent * 100.0, 2u) << "%";
	Log::info() << "False positives:    " << String::toAString(falsePositivesInPercent * 100.0, 2u) << "%";

	allSucceeded = correctInPercent >= 0.99 && falsePositivesInPercent <= 0.01 && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		Log::info() << "Random generator seed: " << randomGenerator.seed();
	}

	return allSucceeded;
}

void TestAlignmentPatternDetector::drawAlignmentPattern(Frame& yFrame, const Vector2& location, const Scalar patternSizeInPixels, const Scalar rotation, const uint8_t foregroundColor, const uint8_t backgroundColor)
{
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(FrameType::FORMAT_Y8, yFrame.pixelFormat()) && yFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);
	ocean_assert(patternSizeInPixels >= Scalar(5));
	ocean_assert(rotation >= 0 && rotation <= Numeric::pi_2());

	// Extra border in pixels in order to avoid interpolation artifacts near the image border
	constexpr unsigned int extraBorderInPixels = 2u;

	// Draw an axis-aligned version of the alignment pattern at approximately the right size.
	const unsigned int approximateModuleSizeInPixels = (unsigned int)Numeric::round32(patternSizeInPixels * Scalar(0.2));
	ocean_assert(approximateModuleSizeInPixels != 0u);

	const unsigned int approximateFrameWidth = 5u * approximateModuleSizeInPixels + 2u * extraBorderInPixels;

	Frame approximateFrame(FrameType(approximateFrameWidth, approximateFrameWidth, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	approximateFrame.setValue(backgroundColor);

	// TTTTTTTTTT
	// LL      RR
	// LL  CC  RR
	// LL      RR
	// BBBBBBBBBB
	const unsigned int alignmentPatternRectangles[20u] =
	{
		// clang-format off
		// x,                                                     y,                                                        width,                              height
		extraBorderInPixels,                                      extraBorderInPixels,                                      5u * approximateModuleSizeInPixels, 1u * approximateModuleSizeInPixels, // top (T)
		extraBorderInPixels,                                      extraBorderInPixels + 4u * approximateModuleSizeInPixels, 5u * approximateModuleSizeInPixels, 1u * approximateModuleSizeInPixels, // bottom (B)
		extraBorderInPixels,                                      extraBorderInPixels + 1u * approximateModuleSizeInPixels, 1u * approximateModuleSizeInPixels, 4u * approximateModuleSizeInPixels, // left (L)
		extraBorderInPixels + 4u * approximateModuleSizeInPixels, extraBorderInPixels + 1u * approximateModuleSizeInPixels, 1u * approximateModuleSizeInPixels, 4u * approximateModuleSizeInPixels, // right (R)
		extraBorderInPixels + 2u * approximateModuleSizeInPixels, extraBorderInPixels + 2u * approximateModuleSizeInPixels, 1u * approximateModuleSizeInPixels, 1u * approximateModuleSizeInPixels, // center (C)
		// clang-format on
	};

	for (size_t i = 0; i < 20; i += 4)
	{
		CV::Canvas::rectangle(approximateFrame, int(alignmentPatternRectangles[i + 0]), int(alignmentPatternRectangles[i + 1]), alignmentPatternRectangles[i + 2], alignmentPatternRectangles[i + 3], &foregroundColor);
	}

	// Compute the affine transformation that maps coordinates from yFrame into approximateFrame
	const Scalar moduleSizeInPixels = Scalar(0.2) * patternSizeInPixels;

	const SquareMatrix3 approximateFrameCenteredScaledRotated_T_yFrame(Vector3(Scalar(1), Scalar(0), Scalar(0)), Vector3(Scalar(0), Scalar(1), Scalar(0)), Vector3(-location, Scalar(1)));
	const SquareMatrix3 approximateFrameCenteredScaled_R_approximateFrameCenteredScaledRotated(Rotation(Vector3(0, 0, 1), rotation));
	const SquareMatrix3 approximateFrameCentered_S_approximateFrameCenteredScaled(Vector3(Scalar(approximateModuleSizeInPixels) / moduleSizeInPixels, 0, 0), Vector3(0, Scalar(approximateModuleSizeInPixels) / moduleSizeInPixels, 0), Vector3(0, 0, 1));
	const SquareMatrix3 approximateFrame_T_approximateFrameCentered(Vector3(Scalar(1), Scalar(0), Scalar(0)), Vector3(Scalar(0), Scalar(1), Scalar(0)), Vector3(Scalar(approximateFrameWidth / 2u), Scalar(approximateFrameWidth / 2u), Scalar(1)));

	const SquareMatrix3 approximateFrame_A_yFrame = approximateFrame_T_approximateFrameCentered * approximateFrameCentered_S_approximateFrameCenteredScaled * approximateFrameCenteredScaled_R_approximateFrameCenteredScaledRotated * approximateFrameCenteredScaledRotated_T_yFrame;
	ocean_assert(approximateFrame_A_yFrame.isAffine());

	// Note:
	// Rendering into yFrame directly using approximateFrame_A_yFrame (and FrameInterpolatorBilinear) will
	// overwrite ALL previous image data outside the transformed area of approximateFrame. This can be avoided
	// by rendering only into the sub-frame that is defined by the bounding box of the transformed area of
	// approximateFrame in yFrame (+ a few pixels added extra).

	// Compute the bounding box of the transformed area of approximateFrame in yFrame
	const SquareMatrix3 yFrame_A_approximateFrame = approximateFrame_A_yFrame.inverted();

	const Vector2 approximateFrameCorners[4] =
	{
		Vector2(Scalar(0), Scalar(0)), // TL
		Vector2(Scalar(0), Scalar(approximateFrame.height())), // BL
		Vector2(Scalar(approximateFrame.width()), Scalar(approximateFrame.height())), // BR
		Vector2(Scalar(approximateFrame.width()), Scalar(0)), // TR
	};

	Vector2 topLeft(Numeric::maxValue(), Numeric::maxValue());
	Vector2 bottomRight(Numeric::minValue(), Numeric::minValue());

	for (size_t i = 0; i < 4; ++i)
	{
		const Vector2 corner = yFrame_A_approximateFrame * approximateFrameCorners[i];

		topLeft.x() = std::min(topLeft.x(), corner.x());
		topLeft.y() = std::min(topLeft.y(), corner.y());

		bottomRight.x() = std::max(bottomRight.x(), corner.x());
		bottomRight.y() = std::max(bottomRight.y(), corner.y());
	}

	// Add some extra border to the bounding box, if possible
	topLeft.x() = std::max(Scalar(0), topLeft.x() - Scalar(2));
	topLeft.y() = std::max(Scalar(0), topLeft.y() - Scalar(2));

	bottomRight.x() = std::min(Scalar(yFrame.width() - 1u), bottomRight.x() + Scalar(2));
	bottomRight.y() = std::min(Scalar(yFrame.height() - 1u), bottomRight.y() + Scalar(2));

	const unsigned int topLeftX = (unsigned int)Numeric::round32(topLeft.x());
	const unsigned int topLeftY = (unsigned int)Numeric::round32(topLeft.y());

	const unsigned int bottomRightX = (unsigned int)Numeric::round32(bottomRight.x());
	const unsigned int bottomRightY = (unsigned int)Numeric::round32(bottomRight.y());

	ocean_assert(bottomRightX > topLeftX);
	ocean_assert(bottomRightY > topLeftY);

	// Compute the affine transformation that maps coordinates from the sub-frame in yFrame into approximateFrame
	const SquareMatrix3 yFrame_T_subFrame(Vector3(Scalar(1), Scalar(0), Scalar(0)), Vector3(Scalar(0), Scalar(1), Scalar(0)), Vector3(Scalar(topLeftX), Scalar(topLeftY), Scalar(1)));
	const SquareMatrix3 approximateFrame_A_subFrame = approximateFrame_A_yFrame * yFrame_T_subFrame;
	ocean_assert(approximateFrame_A_subFrame.isAffine());

	// Render the alignment pattern into the sub-frame
	const unsigned int subFrameWidth = bottomRightX - topLeftX + 1u;
	const unsigned int subFrameHeight = bottomRightY - topLeftY + 1u;

	ocean_assert(topLeftX + subFrameWidth <= yFrame.width());
	ocean_assert(topLeftY + subFrameHeight<= yFrame.height());

	Frame subFrame = yFrame.subFrame(topLeftX, topLeftY, subFrameWidth, subFrameHeight, Frame::CM_USE_KEEP_LAYOUT);

	CV::FrameInterpolatorBilinear::Comfort::affine(approximateFrame, subFrame, approximateFrame_A_subFrame, &backgroundColor, WorkerPool::get().scopedWorker()(), CV::PixelPositionI(0, 0));
}

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Test
