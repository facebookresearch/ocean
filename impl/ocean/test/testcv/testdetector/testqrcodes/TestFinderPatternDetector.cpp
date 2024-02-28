// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/testdetector/testqrcodes/TestFinderPatternDetector.h"

#include "ocean/test/testcv/testdetector/testqrcodes/Utilities.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameConverterY8.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"

#include "ocean/math/Random.h"
#include "ocean/math/Rotation.h"

#ifdef __APPLE__
	#include "ocean/media/imageio/Image.h"
#else
	#include "ocean/media/openimagelibraries/Image.h"
#endif

#ifdef OCEAN_ENABLED_EVERSTORE_CLIENT
	#include "ocean/network/everstore/EverstoreClient.h"
#endif // OCEAN_ENABLED_EVERSTORE_CLIENT

#include <random>

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

// #define APPLY_IMAGE_TEST

bool TestFinderPatternDetector::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Finder pattern detector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDetectFinderPatternSyntheticData(0u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectFinderPatternSyntheticData(3u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectFinderPatternSyntheticData(5u, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDetectFinderPatternSyntheticData(7u, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Finder pattern detector test succeeded.";
	}
	else
	{
		Log::info() << "Finder pattern detector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestCVDetectorQRCodesFinderPatternDetector, DetectFinderPatternSyntheticDataFilterSize0)
{
	Worker worker;
	EXPECT_TRUE(TestFinderPatternDetector::testDetectFinderPatternSyntheticData(0u, GTEST_TEST_DURATION, worker));
}

TEST(TestCVDetectorQRCodesFinderPatternDetector, DetectFinderPatternSyntheticDataFilterSize3)
{
	Worker worker;
	EXPECT_TRUE(TestFinderPatternDetector::testDetectFinderPatternSyntheticData(3u, GTEST_TEST_DURATION, worker));
}

TEST(TestCVDetectorQRCodesFinderPatternDetector, DetectFinderPatternSyntheticDataFilterSize5)
{
	Worker worker;
	EXPECT_TRUE(TestFinderPatternDetector::testDetectFinderPatternSyntheticData(5u, GTEST_TEST_DURATION, worker));
}

TEST(TestCVDetectorQRCodesFinderPatternDetector, TestDetectFinderPatternSyntheticDataFilterSize7)
{
	Worker worker;
	EXPECT_TRUE(TestFinderPatternDetector::testDetectFinderPatternSyntheticData(7u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFinderPatternDetector::testDetectFinderPatternSyntheticData(const unsigned int filterSize, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test: detect finder patterns (" << filterSize << ")";

	// Location and size of a finder pattern (size = width of horizontal intersection of axis aligned pattern, diagonal = sqrt(2) * size)
	struct GroundtruthFinderPattern
	{
		Vector2 location;
		Scalar length;
		Scalar rotation;
	};
	typedef std::vector<GroundtruthFinderPattern> GroundtruthFinderPatterns;

	unsigned long long finderPatternsTotal = 0ull;
	unsigned long long truePositiveDetections = 0ull;
	unsigned long long falsePositiveDetections = 0ull;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const uint8_t backgroundColor = uint8_t(RandomI::random(randomGenerator, 200u, 255u));

		const unsigned int width = RandomI::random(randomGenerator, 250u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 250u, 1920u);

		const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		GroundtruthFinderPatterns groundtruthFinderPatterns;
		const unsigned int finderPatternsCount = RandomI::random(randomGenerator, 1u, 50u);

		for (unsigned int n = 0u; n < finderPatternsCount; ++n)
		{
			const Scalar length = Random::scalar(randomGenerator, Scalar(25), Scalar(49));
			const Vector2 location = Random::vector2(randomGenerator, length * Scalar(2), Scalar(width) - length * Scalar(2) - Scalar(1), length * Scalar(2), Scalar(height) - length * Scalar(2) - Scalar(1));
			const Scalar rotation = Random::scalar(randomGenerator, Scalar(0), Numeric::pi_2());

			bool tooClose = false;

			for (const GroundtruthFinderPattern& groundtruthFinderPattern : groundtruthFinderPatterns)
			{
				if (groundtruthFinderPattern.location.distance(location) <= (groundtruthFinderPattern.length + length) * Numeric::sqrt(Scalar(2)) * Scalar(1.15))
				{
					tooClose = true;
					break;
				}
			}

			if (tooClose == false)
			{
				groundtruthFinderPatterns.push_back({ location, length, rotation });
			}
		}

		ocean_assert(groundtruthFinderPatterns.empty() == false);

		Vectors2 noiseLocations;
		const unsigned int noiseSamplesCount = RandomI::random(randomGenerator, 1u, 50u);

		for (unsigned int n = 0u; n < noiseSamplesCount; ++n)
		{
			const Vector2 location = Random::vector2(randomGenerator, Scalar(19u), Scalar(width - 20u), Scalar(19u), Scalar(height - 20u));

			bool tooClose = false;

			for (const GroundtruthFinderPattern& groundtruthFinderPattern : groundtruthFinderPatterns)
			{
				if (groundtruthFinderPattern.location.distance(location) <= (groundtruthFinderPattern.length) * Scalar(2.15))
				{
					tooClose = true;
					break;
				}
			}

			if (tooClose == false)
			{
				noiseLocations.push_back(location);
			}
		}

		Frame frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
		frame.setValue(backgroundColor);

		for (const GroundtruthFinderPattern& groundtruthFinderPattern : groundtruthFinderPatterns)
		{
			const uint8_t foregroundColor = uint8_t(RandomI::random(randomGenerator, 0u, 50u));

			paintFinderPattern(frame.data<uint8_t>(), width, height, groundtruthFinderPattern.location, groundtruthFinderPattern.length, groundtruthFinderPattern.rotation, foregroundColor, backgroundColor, frame.paddingElements(), &worker);
		}

		for (const Vector2& noiseLocation : noiseLocations)
		{
			const unsigned char foregroundColor = (unsigned char)(RandomI::random(randomGenerator, 0u, 50u));

			Utilities::drawNoisePattern(frame.data<uint8_t>(), width, height, frame.paddingElements(), noiseLocation, randomGenerator, foregroundColor);
		}

		if (filterSize != 0u)
		{
			CV::FrameFilterGaussian::filter(frame, filterSize, &worker);
		}

		finderPatternsTotal += groundtruthFinderPatterns.size();

		const FinderPatterns detectedFinderPatterns = FinderPatternDetector::detectFinderPatterns(frame.constdata<uint8_t>(), frame.width(), frame.height(), 10u, frame.paddingElements(), (RandomI::random(1) == 1u ? &worker : nullptr));

		// Forward check
		for (const GroundtruthFinderPattern& groundtruthFinderPattern : groundtruthFinderPatterns)
		{
			bool foundMatch = false;

			for (const FinderPattern& detectedFinderPattern : detectedFinderPatterns)
			{
				if (groundtruthFinderPattern.location.sqrDistance(detectedFinderPattern.position()) < Numeric::sqr(5))
				{
					if (Numeric::abs(groundtruthFinderPattern.length / detectedFinderPattern.length()) >= Scalar(0.5) && Numeric::abs(groundtruthFinderPattern.length / detectedFinderPattern.length()) <= Scalar(1.6))
					{
						foundMatch = true;
						break;
					}
				}
			}

			if (foundMatch)
			{
				truePositiveDetections++;
			}
		}

		// Backward check
		for (const FinderPattern& detectedFinderPattern : detectedFinderPatterns)
		{
			bool foundMatch = false;

			for (const GroundtruthFinderPattern& groundtruthFinderPattern : groundtruthFinderPatterns)
			{
				if (detectedFinderPattern.position().sqrDistance(groundtruthFinderPattern.location) < Numeric::sqr(5))
				{
					if (Numeric::abs(detectedFinderPattern.length() / groundtruthFinderPattern.length) >= Scalar(0.5) / Numeric::sqrt(Scalar(2)) && Numeric::abs(detectedFinderPattern.length() / groundtruthFinderPattern.length) <= Scalar(1.6))
					{
						foundMatch = true;
						break;
					}
				}
			}

			if (foundMatch == false)
			{
				falsePositiveDetections++;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(finderPatternsTotal != 0u);

	const double percentCorrect = double(truePositiveDetections) / double(finderPatternsTotal);
	const double percentFalsePositives = double(falsePositiveDetections) / double(finderPatternsTotal);

	Log::info() << "Correct detections: " << String::toAString(percentCorrect * 100.0, 2u) << "%";
	Log::info() << "False positives:    " << String::toAString(percentFalsePositives * 100.0, 2u) << "%";

	const bool succeeded = percentCorrect >= 0.99 && percentFalsePositives <= 0.01;

	if (succeeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
		Log::info() << "Random generator seed: " << randomGenerator.seed();
	}

	return succeeded;
}

void TestFinderPatternDetector::paintFinderPattern(uint8_t* yFrame, const unsigned int width, const unsigned int height, const Vector2& location, const Scalar& length, const Scalar& rotationAngle, const unsigned char foregroundColor, const unsigned char backgroundColor, const unsigned int paddingElements, Worker* worker)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 29u && height >= 29u);
	ocean_assert(length >= Scalar(7));
	ocean_assert(length < Scalar(width) && length < Scalar(height));
	ocean_assert(location.x() >= Scalar(Numeric::round32(length)));
	ocean_assert(location.y() >= Scalar(Numeric::round32(length)));
	ocean_assert(location.x() < Scalar(width) - Scalar(Numeric::round32(length)));
	ocean_assert(location.y() < Scalar(height) - Scalar(Numeric::round32(length)));

	const unsigned int patternFrameSize = (unsigned int)Numeric::round32(Scalar(2) * length);

	Frame finderPatternFrame(FrameType(patternFrameSize, patternFrameSize, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	memset(finderPatternFrame.data<uint8_t>(), backgroundColor, finderPatternFrame.size());

	// Draw the axis aligned finder pattern (size is rounded but will be correct later)

	const unsigned int bitWidth = (unsigned int)Numeric::round32(length / Scalar(7));
	ocean_assert(7u * bitWidth + 2u < width && 7u * bitWidth + 2u < height);
	const unsigned int padding = 2u;
	CV::Canvas::rectangle(finderPatternFrame, padding, padding, 7u * bitWidth, 7u * bitWidth, &foregroundColor);
	CV::Canvas::rectangle(finderPatternFrame, padding + bitWidth, padding + bitWidth, 5u * bitWidth, 5u * bitWidth, &backgroundColor);
	CV::Canvas::rectangle(finderPatternFrame, padding + 2u * bitWidth, padding + 2u * bitWidth, 3u * bitWidth, 3u * bitWidth, &foregroundColor);

	// Compute the transformation from the axis-aligned, integer-scaled pattern above to the rotated, correctly scaled one

	const int patternFrameOffsetX = Numeric::round32(location.x() - Scalar(patternFrameSize / 2u));
	const int patternFrameOffsetY = Numeric::round32(location.y() - Scalar(patternFrameSize / 2u));

	const Vector2 alignedPatternCenter(Scalar(padding) + Scalar(3.5) * Scalar(bitWidth), Scalar(padding) + Scalar(3.5) * Scalar(bitWidth));
	const Vector2 actualPatternCenter(location.x() - Scalar(patternFrameOffsetX), location.y() - Scalar(patternFrameOffsetY));

	const SquareMatrix3 translationToAlignedPatternCenter = SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(alignedPatternCenter, 1));
	const SquareMatrix3 translationToActualPatternCenter = SquareMatrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(actualPatternCenter, 1));
	const SquareMatrix3 rotation = SquareMatrix3(Rotation(Vector3(0, 0, 1), rotationAngle));
	const SquareMatrix3 scale = SquareMatrix3(Vector3(Scalar(7u * bitWidth) / length, 0, 0), Vector3(0, Scalar(7u * bitWidth) / length, 0), Vector3(0, 0, 1));
	const SquareMatrix3 affineTransform = translationToAlignedPatternCenter * scale * rotation.inverted() * translationToActualPatternCenter.inverted();

	// Apply the transformation and copy the result to the correct sub-frame

	Frame targetFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, Frame::CM_USE_KEEP_LAYOUT, paddingElements);
	Frame targetSubFrame = targetFrame.subFrame(patternFrameOffsetX, patternFrameOffsetY, finderPatternFrame.width(), finderPatternFrame.height());

	CV::FrameInterpolatorBilinear::Comfort::affine(finderPatternFrame, targetSubFrame, affineTransform, &backgroundColor, worker);
}

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namepace Test
