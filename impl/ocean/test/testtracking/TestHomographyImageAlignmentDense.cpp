/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/TestHomographyImageAlignmentDense.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/geometry/Homography.h"

#include "ocean/tracking/HomographyImageAlignmentDense.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

bool TestHomographyImageAlignmentDense::test(const double testDuration, Worker& worker)
{
	bool allSucceeded = true;

	Log::info() << "---   TestHomographyImageAlignmentDense test:   ---";
	Log::info() << " ";

	allSucceeded = testAdditive(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInverseCompositional(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMultiResolution(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "TestHomographyImageAlignmentDense test succeeded.";
	}
	else
	{
		Log::info() << "TestHomographyImageAlignmentDense test FAILED";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestHomographyImageAlignmentDense, Additive_1)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testAdditive(1u, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, Additive_2)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testAdditive(2u, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, Additive_3)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testAdditive(3u, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, Additive_4)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testAdditive(4u, GTEST_TEST_DURATION, worker));
}


TEST(TestHomographyImageAlignmentDense, InverseCompositional_1)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testInverseCompositional(1u, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, InverseCompositional_2)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testInverseCompositional(2u, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, InverseCompositional_3)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testInverseCompositional(3u, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, InverseCompositional_4)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testInverseCompositional(4u, GTEST_TEST_DURATION, worker));
}


TEST(TestHomographyImageAlignmentDense, MultiResolution_Additive_1)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testMultiResolution(1u, true, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, MultiResolution_Additive_2)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testMultiResolution(2u, true, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, MultiResolution_Additive_3)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testMultiResolution(3u, true, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, MultiResolution_Additive_4)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testMultiResolution(4u, true, GTEST_TEST_DURATION, worker));
}


TEST(TestHomographyImageAlignmentDense, MultiResolution_InverseCompositional_1)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testMultiResolution(1u, false, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, MultiResolution_InverseCompositional_2)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testMultiResolution(2u, false, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, MultiResolution_InverseCompositional_3)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testMultiResolution(3u, false, GTEST_TEST_DURATION, worker));
}

TEST(TestHomographyImageAlignmentDense, MultiResolution_InverseCompositional_4)
{
	Worker worker;
	EXPECT_TRUE(TestHomographyImageAlignmentDense::testMultiResolution(4u, false, GTEST_TEST_DURATION, worker));
}


#endif // OCEAN_USE_GTEST

bool TestHomographyImageAlignmentDense::testAdditive(const double testDuration, Worker& worker)
{
	Log::info() << "Additive alignment test:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info() << "... with " << channels << " channels:";

		if (!testAdditive(channels, testDuration, worker))
		{
			allSucceeded = false;
		}

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Additive alignment test succeeded";
	}
	else
	{
		Log::info() << "Additive alignment test FAILED!";
	}

	return allSucceeded;
}

bool TestHomographyImageAlignmentDense::testAdditive(const unsigned int channels, const double testDuration, Worker& /*worker*/)
{
	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceDefault;
	HighPerformanceStatistic performanceConsistency;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 400u, 800u);
		const unsigned int height = RandomI::random(randomGenerator, 400u, 800u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

		Frame templateFrame;
		Frame currentFrame;

		SquareMatrix3 current_H_template(false);

		if (!createRandomData(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), randomGenerator, templateFrame, currentFrame, current_H_template, Scalar(20)))
		{
			ocean_assert(false && "This should never happen!");

			allSucceeded = false;
			continue;
		}

		const CV::SubRegion subRegion = CV::SubRegion(CV::PixelBoundingBox(0u, 0u, templateFrame.width() - 1u, templateFrame.height() - 1u));

		constexpr bool zeroMean = false;

		const SquareMatrix3 roughCurrent_H_template(true);

		Tracking::HomographyImageAlignmentDense::ConsistencyDataRef externalConsistencyData;

		for (const bool useExternalConsistencyData : {false, true})
		{
			HighPerformanceStatistic& performance = useExternalConsistencyData ? performanceConsistency : performanceDefault;

			SquareMatrix3 estimatedCurrent_H_template(false);

			Scalar initialError = Numeric::maxValue();
			Scalar finalError = Numeric::maxValue();
			Scalars intermediateErrors;

			const unsigned int homographyParameters = RandomI::random(randomGenerator, 8u, 9u);

			performance.start();

			if (Tracking::HomographyImageAlignmentDense::optimizeAlignmentAdditive(templateFrame, subRegion, currentFrame, roughCurrent_H_template, homographyParameters, zeroMean, estimatedCurrent_H_template, 200u, Scalar(10), Scalar(10), &initialError, &finalError, &intermediateErrors, useExternalConsistencyData ? &externalConsistencyData : nullptr))
			{
				performance.stop();

				if (intermediateErrors.size() <= 1)
				{
					allSucceeded = false;
				}

				if constexpr (std::is_same<double, Scalar>::value)
				{
					if (finalError > initialError * Scalar(0.1))
					{
						allSucceeded = false;
					}
				}
				else
				{
					if (finalError >= initialError) // generous check for 32bit float
					{
						allSucceeded = false;
					}
				}

				double averageErrorInitial = NumericD::maxValue();
				double averageErrorFinal = NumericD::maxValue();
				if (determineError(templateFrame, currentFrame, estimatedCurrent_H_template, averageErrorInitial, averageErrorFinal))
				{
					if constexpr (std::is_same<double, Scalar>::value)
					{
						if (averageErrorFinal > averageErrorInitial * Scalar(0.1))
						{
							allSucceeded = false;
						}
					}
				}
				else
				{
					ocean_assert(false && "This should never happen!");
					allSucceeded = false;
				}
			}
			else
			{
				performance.skip();

				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Default performance: " << performanceDefault.averageMseconds() << "ms";
	Log::info() << "Consistency performance: " << performanceConsistency.averageMseconds() << "ms";

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

bool TestHomographyImageAlignmentDense::testInverseCompositional(const double testDuration, Worker& worker)
{
	Log::info() << "Inverse compositional alignment test:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info() << "... with " << channels << " channels:";

		if (!testInverseCompositional(channels, testDuration, worker))
		{
			allSucceeded = false;
		}

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Inverse compositional alignment test succeeded";
	}
	else
	{
		Log::info() << "Inverse compositional alignment test FAILED!";
	}

	return allSucceeded;
}

bool TestHomographyImageAlignmentDense::testInverseCompositional(const unsigned int channels, const double testDuration, Worker& /*worker*/)
{
	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performanceDefault;
	HighPerformanceStatistic performanceConsistency;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 400u, 800u);
		const unsigned int height = RandomI::random(randomGenerator, 400u, 800u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

		Frame templateFrame;
		Frame currentFrame;

		SquareMatrix3 current_H_template(false);

		if (!createRandomData(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), randomGenerator, templateFrame, currentFrame, current_H_template, Scalar(20)))
		{
			ocean_assert(false && "This should never happen!");

			allSucceeded = false;
			continue;
		}

		const CV::SubRegion subRegion = CV::SubRegion(CV::PixelBoundingBox(0u, 0u, templateFrame.width() - 1u, templateFrame.height() - 1u));

		constexpr bool zeroMean = false;

		const SquareMatrix3 roughCurrent_H_template(true);

		Tracking::HomographyImageAlignmentDense::ConsistencyDataRef externalConsistencyData;

		for (const bool useExternalConsistencyData : {false, true})
		{
			HighPerformanceStatistic& performance = useExternalConsistencyData ? performanceConsistency : performanceDefault;

			SquareMatrix3 estimatedCurrent_H_template(false);

			Scalar initialError = Numeric::maxValue();
			Scalar finalError = Numeric::maxValue();
			Scalars intermediateErrors;

			const unsigned int homographyParameters = RandomI::random(randomGenerator, 8u, 9u);

			performance.start();

			if (Tracking::HomographyImageAlignmentDense::optimizeAlignmentInverseCompositional(templateFrame, subRegion, currentFrame, roughCurrent_H_template, homographyParameters, zeroMean, estimatedCurrent_H_template, 200u, Scalar(10), Scalar(10), &initialError, &finalError, &intermediateErrors, useExternalConsistencyData ? &externalConsistencyData : nullptr))
			{
				performance.stop();

				if (intermediateErrors.size() <= 1)
				{
					allSucceeded = false;
				}

				if (finalError >= initialError) // quite generous
				{
					allSucceeded = false;
				}

				double averageErrorInitial = NumericD::maxValue();
				double averageErrorFinal = NumericD::maxValue();
				if (determineError(templateFrame, currentFrame, estimatedCurrent_H_template, averageErrorInitial, averageErrorFinal))
				{
					if constexpr (std::is_same<double, Scalar>::value)
					{
						if (averageErrorFinal >= averageErrorInitial) // quite generous
						{
							allSucceeded = false;
						}
					}
				}
				else
				{
					ocean_assert(false && "This should never happen!");
					allSucceeded = false;
				}
			}
			else
			{
				performance.skip();

				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Default performance: " << performanceDefault.averageMseconds() << "ms";
	Log::info() << "Consistency performance: " << performanceConsistency.averageMseconds() << "ms";

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

bool TestHomographyImageAlignmentDense::testMultiResolution(const double testDuration, Worker& worker)
{
	Log::info() << "Multi-resolution alignment test:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		for (const bool additive : {true, false})
		{
			Log::info() << "... with " << channels << " channels, and " << (additive ? "additive:" : "inverse compositional");

			if (!testMultiResolution(channels, additive, testDuration, worker))
			{
				allSucceeded = false;
			}
		}

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Multi-resolution alignment test succeeded";
	}
	else
	{
		Log::info() << "Multi-resolution alignment test FAILED!";
	}

	return allSucceeded;
}

bool TestHomographyImageAlignmentDense::testMultiResolution(const unsigned int channels, const bool additive, const double testDuration, Worker& /*worker*/)
{
	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 400u, 800u);
		const unsigned int height = RandomI::random(randomGenerator, 400u, 800u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

		Frame templateFrame;
		Frame currentFrame;

		SquareMatrix3 current_H_template(false);

		if (!createRandomData(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), randomGenerator, templateFrame, currentFrame, current_H_template, Scalar(20)))
		{
			ocean_assert(false && "This should never happen!");

			allSucceeded = false;
			continue;
		}

		const CV::SubRegion subRegion = CV::SubRegion(CV::PixelBoundingBox(0u, 0u, templateFrame.width() - 1u, templateFrame.height() - 1u));

		constexpr unsigned int numberPyramidLayers = 4u;
		constexpr bool levenbergMarquardtOptimization = true;
		constexpr bool zeroMean = false;

		const SquareMatrix3 roughCurrent_H_template(true);

		SquareMatrix3 estimatedCurrent_H_template(false);

		const unsigned int homographyParameters = RandomI::random(randomGenerator, 8u, 9u);

		performance.start();

		if (Tracking::HomographyImageAlignmentDense::optimizeAlignmentMultiResolution(templateFrame, subRegion, currentFrame, numberPyramidLayers, homographyParameters, additive, levenbergMarquardtOptimization, zeroMean, roughCurrent_H_template, estimatedCurrent_H_template, 200u, 200u, CV::FramePyramid::DM_FILTER_14641))
		{
			performance.stop();

			double averageErrorInitial = NumericD::maxValue();
			double averageErrorFinal = NumericD::maxValue();
			if (determineError(templateFrame, currentFrame, estimatedCurrent_H_template, averageErrorInitial, averageErrorFinal))
			{
				if constexpr (std::is_same<double, Scalar>::value)
				{
					if (averageErrorFinal > averageErrorInitial * Scalar(0.1))
					{
						allSucceeded = false;
					}
				}
			}
			else
			{
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
			}
		}
		else
		{
			performance.skip();

			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance.averageMseconds() << "ms";

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

bool TestHomographyImageAlignmentDense::createRandomData(const FrameType& frameType, RandomGenerator& randomGenerator, Frame& templateFrame, Frame& trackingFrame, SquareMatrix3& tracking_H_template, const Scalar maximalHomographyRadius)
{
	ocean_assert(frameType.isValid());
	ocean_assert(frameType.numberPlanes() == 1u && frameType.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(maximalHomographyRadius >= 0);

	templateFrame = CV::CVUtilities::randomizedFrame(frameType, &randomGenerator);

	const unsigned int channels = templateFrame.channels();

	// let's add some visual features

	for (unsigned int n = 0u; n < 100u; ++n)
	{
		std::vector<uint8_t> color(channels);

		for (uint8_t& value : color)
		{
			value = uint8_t(RandomI::random(randomGenerator, 255u));
		}

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			const unsigned int xCenter = RandomI::random(randomGenerator, templateFrame.width() - 1u);
			const unsigned int yCenter = RandomI::random(randomGenerator, templateFrame.height() - 1u);

			const unsigned int xSize = RandomI::random(randomGenerator, 3u, 100u) | 0x00000001u; // odd size
			const unsigned int ySize = RandomI::random(randomGenerator, 3u, 100u) | 0x00000001u;

			CV::Canvas::ellipse(templateFrame, CV::PixelPosition(xCenter, yCenter), xSize, ySize, color.data());
		}
		else
		{
			const unsigned int left = RandomI::random(randomGenerator, templateFrame.width() - 1u);
			const unsigned int top = RandomI::random(randomGenerator, templateFrame.height() - 1u);

			const unsigned int xSize = RandomI::random(randomGenerator, 1u, 100u);
			const unsigned int ySize = RandomI::random(randomGenerator, 1u, 100u);

			CV::Canvas::rectangle(templateFrame, left, top, xSize, ySize, color.data());
		}
	}

	// we add some Gaussian blur

	const bool resultGaussian = CV::FrameFilterGaussian::filter(templateFrame, 5u);
	ocean_assert_and_suppress_unused(resultGaussian, resultGaussian);

	// we determine a random homography

	const Vectors2 templatePoints =
	{
		Vector2(Scalar(0), Scalar(0)),
		Vector2(Scalar(0), Scalar(frameType.height())),
		Vector2(Scalar(frameType.width()), Scalar(frameType.height())),
		Vector2(Scalar(frameType.width()), Scalar(0))
	};

	Vectors2 trackingPoints =
	{
		templatePoints[0] + Random::vector2(randomGenerator, Scalar(0), Scalar(20), Scalar(0), maximalHomographyRadius),
		templatePoints[1] + Random::vector2(randomGenerator, Scalar(0), Scalar(20), -maximalHomographyRadius, Scalar(0)),
		templatePoints[2] + Random::vector2(randomGenerator, -maximalHomographyRadius, Scalar(0), -maximalHomographyRadius, Scalar(0)),
		templatePoints[3] + Random::vector2(randomGenerator, -maximalHomographyRadius, Scalar(0), Scalar(0), maximalHomographyRadius)
	};

	ocean_assert(Box2(templatePoints).isInside(Box2(trackingPoints)));

	tracking_H_template.toNull();
	if (!Geometry::Homography::homographyMatrix(templatePoints.data(), trackingPoints.data(), templatePoints.size(), tracking_H_template))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// we transform the template frame based on the homography

	trackingFrame = Frame(templateFrame.frameType());
	if (!CV::FrameInterpolatorBilinear::Comfort::homography(templateFrame, trackingFrame, tracking_H_template, nullptr))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return true;
}

bool TestHomographyImageAlignmentDense::determineError(const Frame& templateFrame, const Frame& trackingFrame, const SquareMatrix3& estimatedTracking_H_template, double& averageError, double& averageErrorEstimated)
{
	ocean_assert(templateFrame.isValid());
	ocean_assert(trackingFrame.isValid());
	ocean_assert(!estimatedTracking_H_template.isSingular());

	ocean_assert(templateFrame.isFrameTypeCompatible(trackingFrame, false));
	if (!templateFrame.isFrameTypeCompatible(trackingFrame, false))
	{
		return false;
	}

	SquareMatrix3 tracking_H_estimatedTracking;
	if (!estimatedTracking_H_template.invert(tracking_H_estimatedTracking))
	{
		return false;
	}

	Frame estimatedTrackingFrame(trackingFrame.frameType());
	if (!CV::FrameInterpolatorBilinear::Comfort::homography(templateFrame, estimatedTrackingFrame, tracking_H_estimatedTracking, nullptr))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	uint64_t sumDifferences = 0ull;
	uint64_t sumDifferencesEstimated = 0ull;

	for (unsigned int y = 0u; y < templateFrame.height(); ++y)
	{
		const uint8_t* templateRow = templateFrame.constrow<uint8_t>(y);
		const uint8_t* trackingRow = trackingFrame.constrow<uint8_t>(y);
		const uint8_t* estimatedTrackingRow = estimatedTrackingFrame.constrow<uint8_t>(y);

		for (unsigned int n = 0u; n < templateFrame.planeWidthElements(0u); ++n)
		{
			const int32_t differenceTemplateTracking = int32_t(templateRow[n]) - int32_t(trackingRow[n]);
			const int32_t differenceTrackingEstimated = int32_t(trackingRow[n]) - int32_t(estimatedTrackingRow[n]);

			sumDifferences += uint64_t(std::abs(differenceTemplateTracking));
			sumDifferencesEstimated += uint64_t(std::abs(differenceTrackingEstimated));
		}
	}

	const unsigned int elements = templateFrame.height() * templateFrame.planeWidthElements(0u);
	ocean_assert(elements >= 1u);

	averageError = double(sumDifferences) / double(elements);
	averageErrorEstimated = double(sumDifferencesEstimated) / double(elements);

	return true;
}

}

}

}
