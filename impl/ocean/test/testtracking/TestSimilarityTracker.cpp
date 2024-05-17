/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/TestSimilarityTracker.h"

#include "ocean/base/Timestamp.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"

#include "ocean/tracking/point/SimilarityTracker.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

bool TestSimilarityTracker::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   SimilarityTracker test:   ---";
	Log::info() << " ";

	allSucceeded = testTracking(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStressTest(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "SimilarityTracker test succeeded.";
	}
	else
	{
		Log::info() << "SimilarityTracker test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestSimilarityTracker, Tracking)
{
	Worker worker;
	EXPECT_TRUE(TestSimilarityTracker::testTracking(GTEST_TEST_DURATION, worker));
}

TEST(TestSimilarityTracker, StressTest)
{
	Worker worker;
	EXPECT_TRUE(TestSimilarityTracker::testStressTest(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestSimilarityTracker::testTracking(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Tracking quality test:";

	RandomGenerator randomGenerator;
	bool failed = false;

	uint64_t iterations = 0u;
	uint64_t validIterations = 0u;

	const Timestamp startTimestamp(true);

	do
	{
		const IndexPair32 width_height = RandomI::random(randomGenerator, IndexPairs32({{640u, 480u}, {1280u, 720u}, {1920u, 1080u}}));

		unsigned int width = width_height.first;
		unsigned int height = width_height.second;

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			std::swap(width, height);
		}

		const unsigned int yFrame0PaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		Frame yFrame0(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame0PaddingElements);

		// let's create a random image (which will be simple to track)

		CV::CVUtilities::randomizeFrame(yFrame0, false, &randomGenerator);
		CV::FrameFilterGaussian::filter(yFrame0, 3u, &worker);

		const unsigned int pixels = width * height;

		for (unsigned int n = 0u; n < pixels / (16u * 10u); ++n)
		{
			const unsigned char color = (unsigned char)RandomI::random(randomGenerator, 255u);

			const unsigned int x = RandomI::random(randomGenerator, width - 1u);
			const unsigned int y = RandomI::random(randomGenerator, height - 1u);

			const unsigned int sizeX = RandomI::random(randomGenerator, 2u, 4u);
			const unsigned int sizeY = RandomI::random(randomGenerator, 2u, 4u);

			CV::Canvas::rectangle(yFrame0, x, y, sizeX, sizeY, &color);
		}

		for (unsigned int n = 0u; n < pixels / (100u * 10u); ++n)
		{
			const unsigned char color = (unsigned char)RandomI::random(randomGenerator, 255u);

			const unsigned int x = RandomI::random(randomGenerator, width - 1u);
			const unsigned int y = RandomI::random(randomGenerator, height - 1u);

			const unsigned int sizeX = 10u;
			const unsigned int sizeY = 10u;

			CV::Canvas::rectangle(yFrame0, x, y, sizeX, sizeY, &color);
		}

		for (unsigned int n = 0u; n < pixels / (400u * 10u); ++n)
		{
			const unsigned char color = (unsigned char)RandomI::random(randomGenerator, 255u);

			const unsigned int x = RandomI::random(randomGenerator, width - 1u);
			const unsigned int y = RandomI::random(randomGenerator, height - 1u);

			const unsigned int sizeX = 20u;
			const unsigned int sizeY = 20u;

			CV::Canvas::rectangle(yFrame0, x, y, sizeX, sizeY, &color);
		}

		for (unsigned int n = 0u; n < 50u; ++n)
		{
			const unsigned char color = (unsigned char)RandomI::random(randomGenerator, 255u);

			const Scalar x0 = Random::scalar(randomGenerator, Scalar(0), Scalar(width));
			const Scalar y0 = Random::scalar(randomGenerator, Scalar(0), Scalar(height));

			const Scalar x1 = Random::scalar(randomGenerator, Scalar(0), Scalar(width));
			const Scalar y1 = Random::scalar(randomGenerator, Scalar(0), Scalar(height));

			CV::Canvas::line8BitPerChannel<1u, 3u>(yFrame0.data<uint8_t>(), yFrame0.width(), yFrame0.height(), x0, y0, x1, y1, &color, yFrame0.paddingElements());
		}

		// let's select a random region of interest

		constexpr unsigned int border = 30u;

		const unsigned int regionWidth = RandomI::random(randomGenerator, 250u, 400u);
		const unsigned int regionHeight = RandomI::random(randomGenerator, 250u, 400u);

		ocean_assert(width >= regionWidth + border * 2u);
		ocean_assert(height >= regionHeight + border * 2u);

		const unsigned int regionLeft = RandomI::random(randomGenerator, border, width - regionWidth - border);
		const unsigned int regionTop = RandomI::random(randomGenerator, border, height - regionHeight - border);

		CV::PixelBoundingBox region(CV::PixelPosition(regionLeft, regionTop), regionWidth, regionHeight);
		ocean_assert(region.isValid());

		Tracking::Point::SimilarityTracker similarityTracker;

		SquareMatrix3 resultSimilarity(false);

		Vector2 resultTranslation(Numeric::minValue(), Numeric::minValue());
		Scalar resultRotationAngle = Scalar(-1);
		Scalar resultScale = Scalar(-1);

		if (similarityTracker.determineSimilarity(yFrame0, region, &resultSimilarity, &resultTranslation, &resultRotationAngle, &resultScale))
		{
			// we expect the identity for the first frame
			if (!resultSimilarity.isIdentity() || !resultTranslation.isNull() || Numeric::isNotEqualEps(resultRotationAngle) || Numeric::isNotEqual(resultScale, 1))
			{
				failed = true;
			}
		}
		else
		{
			failed = true;
		}

		SquareMatrix3 frameN_S_frame0(true);

		for (unsigned int frameIndex = 1u; frameIndex < 5u; ++frameIndex)
		{
			// let's create a random similarity transformation

			const bool largeOffset = RandomI::random(randomGenerator, 1u) == 0u;

			const Scalar maximalOffset = Scalar(std::min(width, height)) * (largeOffset ? Scalar(0.25) : Scalar(0.05)); // 5% or 25% of the image resolution

			const Vector2 translation = Random::vector2(randomGenerator, -maximalOffset, maximalOffset);
			const Scalar rotationAngle = Random::scalar(randomGenerator, -Numeric::deg2rad(7.5), Numeric::deg2rad(7.5));
			const Scalar scale = Random::scalar(randomGenerator, Scalar(0.85), Scalar(1.15));

			const Quaternion rotation(Vector3(0, 0, 1), rotationAngle);

			// | a  -b  tx |
			// | b   a  ty |
			// | 0   0   1 |
			const SquareMatrix3 localSimilarity(rotation * Vector3(scale, 0, 0), rotation * Vector3(0, scale, 0), Vector3(translation, 1));
			ocean_assert(localSimilarity.isSimilarity());

			const Vector2 regionCenter(Scalar(regionLeft) + Scalar(regionWidth) * Scalar(0.5), Scalar(regionTop) + Scalar(regionHeight) * Scalar(0.5));

			// let's shift the local similarity transformation to the center of the ROI

			const SquareMatrix3 shiftTransformation(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(regionCenter, 1));
			const SquareMatrix3 frame1_S_frame0(shiftTransformation * localSimilarity * shiftTransformation.inverted());

			frameN_S_frame0 = frame1_S_frame0 * frameN_S_frame0; // frame1_S_frame0 is actually frameN_S_frameN-1

			// let's ensure that the current region still covers enough visual information from the original image

			const Box2 newRegion = Box2(Scalar(regionWidth), Scalar(regionHeight), Vector2(Scalar(regionLeft), Scalar(regionTop))) * frameN_S_frame0;

			if (!Box2(-50, -50, Scalar(yFrame0.width() + 50u), Scalar(yFrame0.height() + 50u)).isInside(newRegion))
			{
				break;
			}

			// we need current_S_previous
			const SquareMatrix3 frame0_S_frame1 = frame1_S_frame0.inverted();

			Frame yFrame1(yFrame0.frameType());
			if (!CV::FrameInterpolatorBilinear::Comfort::homography(yFrame0, yFrame1, frame0_S_frame1, nullptr, &worker))
			{
				ocean_assert(false && "This must never happen!");
				failed = true;
			}

			Vector2 predictedTranslation = Vector2(0, 0);

			if (largeOffset)
			{
				// we have a large offset to handle, so that we need a predicted translation (the correct translation with some noise)
				predictedTranslation = translation + Random::vector2(randomGenerator, Scalar(-10), Scalar(10));
			}

			resultSimilarity.toNull();
			resultTranslation = Vector2(Numeric::minValue(), Numeric::minValue());
			resultRotationAngle = Scalar(-1);
			resultScale = Scalar(-1);

			Tracking::Point::SimilarityTracker::TrackerConfidence trackerConfidence = Tracking::Point::SimilarityTracker::TC_NONE;
			if (similarityTracker.determineSimilarity(yFrame1, region, &resultSimilarity, &resultTranslation, &resultRotationAngle, &resultScale, predictedTranslation, &trackerConfidence))
			{
				if (trackerConfidence != Tracking::Point::SimilarityTracker::TC_NONE)
				{
					if (Numeric::angleIsEqual(rotationAngle, resultRotationAngle, Scalar(2))
							&& Numeric::isEqual(scale, resultScale, Scalar(0.03))
							&& frame1_S_frame0.zAxis().xy().isEqual(resultTranslation, Scalar(1.5)))
					{
						Box2 newBoundingBox;

						newBoundingBox += resultSimilarity * region.topLeft().vector();
						newBoundingBox += resultSimilarity * region.bottomLeft().vector();
						newBoundingBox += resultSimilarity * region.bottomRight().vector();
						newBoundingBox += resultSimilarity * region.topRight().vector();

						unsigned int newBoundingBoxLeft;
						unsigned int newBoundingBoxTop;
						unsigned int newBoundingBoxWidth;
						unsigned int newBoundingBoxHeight;
						if (newBoundingBox.box2integer(yFrame1.width(), yFrame1.height(), newBoundingBoxLeft, newBoundingBoxTop, newBoundingBoxWidth, newBoundingBoxHeight))
						{
							region = CV::PixelBoundingBox(CV::PixelPosition(newBoundingBoxLeft, newBoundingBoxTop), newBoundingBoxWidth, newBoundingBoxHeight);

							validIterations++;
						}
					}
				}
			}
			else
			{
				failed = true;
			}

			iterations++;

			yFrame0 = std::move(yFrame1);
		}
	}
	while (iterations < 100ull || startTimestamp + testDuration > Timestamp(true));

	if (failed)
	{
		Log::info() << "Validation: FAILED!";
		return false;
	}

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " + String::toAString(percent * 100.0, 1u) + "% succeeded.";

	return percent >= 0.85;
}

bool TestSimilarityTracker::testStressTest(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress test:";

	// this test is not testing correctness or benchmarking performance
	// we simply ensure that the SimilarityTracker does not crash

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 40u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 40u, 1080u);

		Frame initialFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const CV::PixelPosition initialSubRegionPoint0(RandomI::random(randomGenerator, width - 1u), RandomI::random(randomGenerator, height - 1u));
		CV::PixelPosition initialSubRegionPoint1(RandomI::random(randomGenerator, width - 1u), RandomI::random(randomGenerator, height - 1u));

		while (initialSubRegionPoint0.sqrDistance(initialSubRegionPoint1) == 0u)
		{
			 initialSubRegionPoint1 = CV::PixelPosition(RandomI::random(randomGenerator, width - 1u), RandomI::random(randomGenerator, height - 1u));
		}

		const CV::PixelBoundingBox initialSubRegion(initialSubRegionPoint0, initialSubRegionPoint1);
		ocean_assert(initialSubRegion.isValid());

		Tracking::Point::SimilarityTracker similarityTracker;

		for (unsigned int n = 0u; n < 20u; ++n)
		{
			Frame frame;

			if (n > 0u && RandomI::random(randomGenerator, 4u) == 0u)
			{
				// we use the initial frame and apply a random similarity transformation

				const Scalar randomScale = Random::scalar(randomGenerator, Scalar(0.90), Scalar(1.1));
				const Scalar randomRotation = Random::scalar(randomGenerator, Numeric::deg2rad(-10), Numeric::deg2rad(10));
				const Vector2 randomTranslation = Random::vector2(randomGenerator, -Scalar(width) * Scalar(0.05), Scalar(width) * Scalar(0.05), -Scalar(height) * Scalar(0.05), Scalar(height) * Scalar(0.05));

				SquareMatrix3 randomSimilarity(Quaternion(Vector3(0, 0, 1), randomRotation));
				randomSimilarity(0, 0) *= randomScale;
				randomSimilarity(1, 0) *= randomScale;
				randomSimilarity(0, 1) *= randomScale;
				randomSimilarity(1, 1) *= randomScale;
				randomSimilarity(0, 2) = randomTranslation.x();
				randomSimilarity(0, 2) = randomTranslation.y();

				if (!frame.set(initialFrame.frameType(), true, true))
				{
					OCEAN_SET_FAILED(validation);
				}

				if (!CV::FrameInterpolatorBilinear::Comfort::homography(initialFrame, frame, randomSimilarity))
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				// we simply randomize the image again

				initialFrame = CV::CVUtilities::randomizedFrame(initialFrame.frameType(), &randomGenerator);

				frame = Frame(initialFrame, Frame::ACM_USE_KEEP_LAYOUT);
			}

			ocean_assert(frame.isValid());

			CV::PixelBoundingBox subRegion;

			if (n > 0u && RandomI::random(randomGenerator, 4u) == 0u)
			{
				// we use the initial frame and apply a random offset

				while (!subRegion.isValid())
				{
					const int offsetX = RandomI::random(randomGenerator, -20, 20);
					const int offsetY = RandomI::random(randomGenerator, -20, 20);

					const unsigned int newLeft = (unsigned int)(std::max(0, int(initialSubRegion.left()) + offsetX));
					const unsigned int newTop = (unsigned int)(std::max(0, int(initialSubRegion.top()) + offsetY));
					const CV::PixelPosition newTopLeft(newLeft, newTop);

					subRegion = CV::PixelBoundingBox(newTopLeft, initialSubRegion.width(), initialSubRegion.height()) && CV::PixelBoundingBox(0u, 0u, width - 1u, height - 1u);
				}
			}
			else
			{
				// we simply randomize the image again

				const CV::PixelPosition subRegionPoint0(RandomI::random(randomGenerator, width - 1u), RandomI::random(randomGenerator, height - 1u));
				CV::PixelPosition subRegionPoint1(RandomI::random(randomGenerator, width - 1u), RandomI::random(randomGenerator, height - 1u));

				while (subRegionPoint0.sqrDistance(subRegionPoint1) == 0u)
				{
					 subRegionPoint1 = CV::PixelPosition(RandomI::random(randomGenerator, width - 1u), RandomI::random(randomGenerator, height - 1u));
				}

				subRegion = CV::PixelBoundingBox(subRegionPoint0, subRegionPoint1);
			}

			ocean_assert(subRegion.isValid());

			SquareMatrix3 similarity(false);
			Vector2 translation(0, 0);
			Scalar rotation = -1;
			Scalar scale = -1;

			Vector2 predictedTranslation(0, 0);

			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				predictedTranslation = Random::vector2(randomGenerator, Scalar(-0.25) * Scalar(width), Scalar(0.25) * Scalar(width), Scalar(-0.25) * Scalar(height), Scalar(0.25) * Scalar(height));
			}

			Tracking::Point::SimilarityTracker::TrackerConfidence trackerConfidence = Tracking::Point::SimilarityTracker::TC_NONE;
			Tracking::Point::SimilarityTracker::RegionTextureness regionTextureness = Tracking::Point::SimilarityTracker::RT_UNKNOWN;

			Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? nullptr : &worker;

			const bool result = similarityTracker.determineSimilarity(frame, subRegion, &similarity, &translation, &rotation, &scale, predictedTranslation, &trackerConfidence, &regionTextureness, useWorker);
			OCEAN_SUPPRESS_UNUSED_WARNING(result);

			// dummy check to ensure that the similarity tracker is not stripped away
			if (int(trackerConfidence) < 0 ||  int(regionTextureness) < 0)
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
