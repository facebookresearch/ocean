/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestFREAKDescriptor.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FramePyramid.h"

#include "ocean/geometry/Jacobian.h"

#include <bitset>
#include <cmath>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

using namespace CV::Detector;

template <size_t tSize>
bool TestFREAKDescriptorT<tSize>::test(const double testDuration, Worker& worker)
{
	static_assert(tSize == 32 || tSize == 64, "The FREAK descriptor test is only defined for 32 and 64 bytes descriptor lengths");

	ocean_assert(testDuration > 0.0);

	Log::info() << "---   FREAK descriptor test (" << (tSize == 32 ? "32" : "64") << " bytes):   ---";
	Log::info() << " ";

	constexpr unsigned int maxAllowedHammingDistance = 1u;

	bool allSucceeded = true;

	allSucceeded = testComputeDescriptor(testDuration, 1920u, 1080u, maxAllowedHammingDistance, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testComputeDescriptors(testDuration, 1920u, 1080u, maxAllowedHammingDistance, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCreateBlurredFramePyramid(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "FREAK descriptor test (" << (tSize == 32 ? "32" : "64") << " bytes) succeeded.";
	}
	else
	{
		Log::info() << "FREAK descriptor test (" << (tSize == 32 ? "32" : "64") << " bytes) FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

// 32-byte FREAK

#define GTEST_FREAK_MAX_HAMMING_DISTANCE 1u

TEST(TestFREAKDescriptor32, ComputeDescriptor_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFREAKDescriptor32::testComputeDescriptor(GTEST_TEST_DURATION, 1920u, 1080u, GTEST_FREAK_MAX_HAMMING_DISTANCE, worker));
}

TEST(TestFREAKDescriptor32, ComputeDescriptors_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFREAKDescriptor32::testComputeDescriptors(GTEST_TEST_DURATION, 1920u, 1080u, GTEST_FREAK_MAX_HAMMING_DISTANCE, worker));
}

TEST(TestFREAKDescriptor32, CreateBlurredFramePyramid)
{
	Worker worker;
	EXPECT_TRUE(TestFREAKDescriptor32::testCreateBlurredFramePyramid(GTEST_TEST_DURATION, worker));
}


// 64-byte FREAK

TEST(TestFREAKDescriptor64, ComputeDescriptor_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFREAKDescriptor64::testComputeDescriptor(GTEST_TEST_DURATION, 1920u, 1080u, GTEST_FREAK_MAX_HAMMING_DISTANCE, worker));
}

TEST(TestFREAKDescriptor64, ComputeDescriptors_1920x1080)
{
	Worker worker;
	EXPECT_TRUE(TestFREAKDescriptor64::testComputeDescriptors(GTEST_TEST_DURATION, 1920u, 1080u, GTEST_FREAK_MAX_HAMMING_DISTANCE, worker));
}

TEST(TestFREAKDescriptor64, CreateBlurredFramePyramid)
{
	Worker worker;
	EXPECT_TRUE(TestFREAKDescriptor64::testCreateBlurredFramePyramid(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

template <size_t tSize>
bool TestFREAKDescriptorT<tSize>::testComputeDescriptor(const double testDuration, const unsigned int imageWidth, const unsigned int imageHeight, const unsigned int hammingDistanceThreshold, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(imageWidth != 0u && imageHeight != 0u);

	Log::info() << "Testing FREAK descriptors for single points (" << imageWidth << " x " << imageHeight << " px):";
	Log::info().newLine();

	HighPerformanceStatistic oceanPerformance;
	HighPerformanceStatistic originalPerformance;

	RandomGenerator randomGenerator;
	const Timestamp startTime(true);

	uint64_t testedPointsTotal = 0u;
	uint64_t testedPointsFailed = 0u;

	do
	{
		// Generate a pyramid from a random image

		Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(imageWidth, imageHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		CV::FrameFilterGaussian::filter(yFrame, 5u, &worker);

		const CV::FramePyramid yFramePyramid(yFrame, /* FRL default number of layers */ 6u, false /*copyFirstLayer*/, &worker);

		// Compute descriptors for random points in all but the last pyramid layer

		for (unsigned int level = 0u; level < yFramePyramid.layers() - 1u; ++level)
		{
			const unsigned int width = yFramePyramid[level].width();
			const unsigned int height = yFramePyramid[level].height();

			const unsigned int distanceFromBorder = 32u;
			if (distanceFromBorder * 2u >= width || distanceFromBorder * 2u >= height)
			{
				break;
			}

			// TODO Distortion (also see calculatePointJacobian2x3() in CameraDerivativeFunctor)? Variable FOV?
			// TODO Add an offset to the principal point
			const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

			// Performance comparison of Ocean vs. the original implementation + validation
			//
			// Choose up to ~1% of number of pixels as synthetic keypoints but no more than 1K

			const unsigned int pointCount = std::min(1000u, std::max(1u, (width * height + 50u) / 100u));

			for (unsigned int i = 0u; i < pointCount; ++i)
			{
				const Vector2 ocnPoint = Random::vector2(randomGenerator, Scalar(distanceFromBorder), Scalar(width - distanceFromBorder), Scalar(distanceFromBorder), Scalar(height - distanceFromBorder));
				const Eigen::Vector2f point(float(ocnPoint.x()), float(ocnPoint.y()));

				CV::Detector::FREAKDescriptorT<tSize> oceanFreakDescriptor;
				CV::Detector::FREAKDescriptorT<tSize> originalFreakDescriptor;

				bool oceanComputationSuccessful = false;
				bool originalComputationSuccessful = false;

				const typename FREAKDescriptorT<tSize>::CameraDerivativeData cameraDerivativeData = FREAKDescriptorT<tSize>::PinholeCameraDerivativeFunctor::computeCameraDerivativeData(pinholeCamera, point);

				const unsigned oceanIteration = i % 2u == 1u;
				for (unsigned int implementationIteration = 0u; implementationIteration < 2u; ++implementationIteration)
				{
					if (implementationIteration == oceanIteration)
					{
						oceanPerformance.start();
							oceanComputationSuccessful = CV::Detector::FREAKDescriptorT<tSize>::computeDescriptor(yFramePyramid, point, level, oceanFreakDescriptor, cameraDerivativeData.unprojectRayIF, float(pinholeCamera.inverseFocalLengthX()), cameraDerivativeData.pointJacobianMatrixIF);
						oceanPerformance.stop();
					}
					else
					{
						originalPerformance.start();
							originalComputationSuccessful = TestFREAKDescriptorT<tSize>::computeOriginalDescriptor(yFramePyramid, point, level, originalFreakDescriptor, cameraDerivativeData.unprojectRayIF, float(pinholeCamera.inverseFocalLengthX()), cameraDerivativeData.pointJacobianMatrixIF);
						originalPerformance.stop();
					}
				}

				unsigned int hammingDistances[3] = {0u, 0u, 0u};
				if (oceanComputationSuccessful != originalComputationSuccessful || TestFREAKDescriptorT<tSize>::validateFREAKDescriptor(oceanFreakDescriptor, originalFreakDescriptor, hammingDistanceThreshold, hammingDistances) == false)
				{
					Log::debug() << "Bad descriptor: level: " << level << ", i: " << i << ", point: " << point.x() << ", " << point.y() << ", hamming: " << hammingDistances[0] << ", " << hammingDistances[1] << ", " << hammingDistances[2];

					testedPointsFailed++;
				}

				testedPointsTotal++;
			}
		}
	}
	while (startTime + testDuration > Timestamp(true));

	if (oceanPerformance.measurements() != 0u && oceanPerformance.measurements() == originalPerformance.measurements())
	{
		Log::info() << "Performance: Ocean vs Original (worst, best, average median in ms):";
		Log::info() << "  Ocean:        " << String::toAString(oceanPerformance.worstMseconds(), 3u) << ", " << String::toAString(oceanPerformance.bestMseconds(), 3u) << ", " << String::toAString(oceanPerformance.averageMseconds(), 3u) << ", " << String::toAString(oceanPerformance.medianMseconds(), 3u);
		Log::info() << "  Original:     " << String::toAString(originalPerformance.worstMseconds(), 3u) << ", " << String::toAString(originalPerformance.bestMseconds(), 3u) << ", " << String::toAString(originalPerformance.averageMseconds(), 3u) << ", " << String::toAString(originalPerformance.medianMseconds(), 3u);
		Log::info() << "  Improvements: " << String::toAString(originalPerformance.worstMseconds() / oceanPerformance.worstMseconds(), 2u) << ", " << String::toAString(originalPerformance.bestMseconds() / oceanPerformance.bestMseconds(), 2u) << ", " << String::toAString(originalPerformance.averageMseconds() / oceanPerformance.averageMseconds(), 2u) << ", " << String::toAString(originalPerformance.medianMseconds() / oceanPerformance.medianMseconds(), 2u) << " x";
	}

	const double testedPointsFailedRatio = testedPointsTotal != 0u ? double(testedPointsFailed) / double(testedPointsTotal) : 1.0;
	const bool succeeded = testedPointsFailedRatio < 0.002; // < 0.2%

	Log::info().newLine();

	if (succeeded)
	{
		Log::info() << "Validation successful";
	}
	else
	{
		Log::info() << "Validation FAILED!";
		Log::info() << "Percentage of failed validations: " << String::toAString(testedPointsFailedRatio * 100.0f, 2u) << " %";
	}

	return succeeded;
}

template <size_t tSize>
bool TestFREAKDescriptorT<tSize>::testComputeDescriptors(const double testDuration, const unsigned int imageWidth, const unsigned int imageHeight, const unsigned int maxAllowedHammingDistance, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(imageWidth != 0u && imageHeight != 0u);

	Log::info() << "Testing FREAK descriptors for vectors of points (" << imageWidth << " x " << imageHeight << " px):";
	Log::info().newLine();

	HighPerformanceStatistic oceanPerformance;
	HighPerformanceStatistic originalPerformance;

	HighPerformanceStatistic oceanPerformanceSinglecore;
	HighPerformanceStatistic oceanPerformanceMulticore;

	uint64_t testedPointsTotal = 0u;
	uint64_t testedPointsFailedSingleCore = 0u;
	uint64_t testedPointsFailedMultiCore = 0u;

	unsigned int iterations = 0u;

	RandomGenerator randomGenerator;

	const Timestamp startTime(true);

	do
	{
		// Generate a pyramid from a random image

		Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(imageWidth, imageHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		CV::FrameFilterGaussian::filter(yFrame, 5u, &worker);

		const CV::FramePyramid yFramePyramid(yFrame, /* FRL default number of layers */ 6u, false /*copyFirstLayer*/, &worker);

		// Compute descriptors for random points in all but the last pyramid layer

		// TODO Distortion (also see calculatePointJacobian2x3() in CameraDerivativeFunctor)? Variable FOV?
		// TODO Add an offset to the principal point
		const PinholeCamera pinholeCamera(yFrame.width(), yFrame.height(), Numeric::deg2rad(60));

		typename FREAKDescriptorT<tSize>::PinholeCameraDerivativeFunctor cameraDerivativeFunctor(pinholeCamera, yFramePyramid.layers());
		ocean_assert(cameraDerivativeFunctor.supportedPyramidLevels() == yFramePyramid.layers());

		for (unsigned int level = 0u; level < yFramePyramid.layers() - 1u; ++level)
		{
			const unsigned int width = yFramePyramid[level].width();
			const unsigned int height = yFramePyramid[level].height();

			const unsigned int distanceFromBorder = 32u;
			if (distanceFromBorder * 2u >= width || distanceFromBorder * 2u >= height)
			{
				break;
			}

			// Choose up to ~1% of number of pixels as synthetic key points but no more than 1K

			const unsigned int pointCount = std::min(1000u, std::max(1u, (width * height + 50u) / 100u));
			std::vector<Eigen::Vector2f> points;
			points.reserve(pointCount);

			for (unsigned int i = 0u; i < pointCount; ++i)
			{
				const Vector2 point = Random::vector2(randomGenerator, Scalar(distanceFromBorder), Scalar(width - distanceFromBorder), Scalar(distanceFromBorder), Scalar(height - distanceFromBorder));
				points.emplace_back(float(point.x()), float(point.y()));
			}

			// Performance comparison of single-core vs. multi-core implementation

			std::vector<FREAKDescriptorT<tSize>> oceanFreakDescriptorsSinglecore(pointCount);
			std::vector<FREAKDescriptorT<tSize>> oceanFreakDescriptorsMulticore(pointCount);
			std::vector<FREAKDescriptorT<tSize>> originalFreakDescriptors(pointCount);

			const unsigned int firstExecutionIndex = iterations % 3u;
			for (unsigned int i = 0u; i < 3u; ++i)
			{
				switch ((firstExecutionIndex + i) % 3u)
				{
					case 0u:
						oceanPerformanceSinglecore.start();
							FREAKDescriptorT<tSize>::computeDescriptors(yFramePyramid, points.data(), points.size(), level, oceanFreakDescriptorsSinglecore.data(), float(pinholeCamera.inverseFocalLengthX()), cameraDerivativeFunctor, nullptr);
						oceanPerformanceSinglecore.stop();
						break;

					case 1u:
						oceanPerformanceMulticore.start();
							FREAKDescriptorT<tSize>::computeDescriptors(yFramePyramid, points.data(), points.size(), level, oceanFreakDescriptorsMulticore.data(), float(pinholeCamera.inverseFocalLengthX()), cameraDerivativeFunctor, &worker);
						oceanPerformanceMulticore.stop();
						break;

					case 2u:
						originalPerformance.start();
							TestFREAKDescriptorT<tSize>::computeOriginalDescriptors(yFramePyramid, points, level, originalFreakDescriptors, float(pinholeCamera.inverseFocalLengthX()), cameraDerivativeFunctor);
						originalPerformance.stop();
						break;
				}
			}
			ocean_assert(oceanPerformanceSinglecore.measurements() == oceanPerformanceMulticore.measurements());

			// Validation

			for (unsigned int i = 0u; i < pointCount; ++i)
			{
				unsigned int hammingDistances[3] = {0u, 0u, 0u};
				if (originalFreakDescriptors[i].isValid() != oceanFreakDescriptorsSinglecore[i].isValid() || TestFREAKDescriptorT<tSize>::validateFREAKDescriptor(originalFreakDescriptors[i], oceanFreakDescriptorsSinglecore[i], maxAllowedHammingDistance, hammingDistances) == false)
				{
					Log::debug() << "Single-core, level: " << level << ", i: " << i << ", point: " << points[i].x() << ", " << points[i].y() << ", hamming: " << hammingDistances[0] << ", " << hammingDistances[1] << ", " << hammingDistances[2];

					testedPointsFailedSingleCore++;
				}

				if (originalFreakDescriptors[i].isValid() != oceanFreakDescriptorsMulticore[i].isValid() || TestFREAKDescriptorT<tSize>::validateFREAKDescriptor(originalFreakDescriptors[i], oceanFreakDescriptorsMulticore[i], maxAllowedHammingDistance, hammingDistances) == false)
				{
					Log::debug() << "Multi-core, level: " << level << ", i: " << i << ", point: " << points[i].x() << ", " << points[i].y() << ", hamming: " << hammingDistances[0] << ", " << hammingDistances[1] << ", " << hammingDistances[2];

					testedPointsFailedMultiCore++;
				}
			}

			testedPointsTotal += pointCount;
		}

		iterations++;
	}
	while (startTime + testDuration > Timestamp(true));

	if (oceanPerformanceSinglecore.measurements() != 0u && oceanPerformanceSinglecore.measurements() == oceanPerformanceMulticore.measurements())
	{
		Log::info() << "Performance: Ocean single-core vs Ocean multi-core (worst, best, average median in ms):";
		Log::info() << "  Ocean (single-core): " << String::toAString(oceanPerformanceSinglecore.worstMseconds(), 3u) << ", " << String::toAString(oceanPerformanceSinglecore.bestMseconds(), 3u) << ", " << String::toAString(oceanPerformanceSinglecore.averageMseconds(), 3u) << ", " << String::toAString(oceanPerformanceSinglecore.medianMseconds(), 3u);
		Log::info() << "  Ocean (multi-core):  " << String::toAString(oceanPerformanceMulticore.worstMseconds(), 3u) << ", " << String::toAString(oceanPerformanceMulticore.bestMseconds(), 3u) << ", " << String::toAString(oceanPerformanceMulticore.averageMseconds(), 3u) << ", " << String::toAString(oceanPerformanceMulticore.medianMseconds(), 3u);
		Log::info() << "  Improvements:        " << String::toAString(oceanPerformanceSinglecore.worstMseconds() / oceanPerformanceMulticore.worstMseconds(), 2u) << ", " << String::toAString(oceanPerformanceSinglecore.bestMseconds() / oceanPerformanceMulticore.bestMseconds(), 2u) << ", " << String::toAString(oceanPerformanceSinglecore.averageMseconds() / oceanPerformanceMulticore.averageMseconds(), 2u) << ", " << String::toAString(oceanPerformanceSinglecore.medianMseconds() / oceanPerformanceMulticore.medianMseconds(), 2u) << " x";
	}

	Log::info().newLine();

	if (originalPerformance.measurements() != 0u && originalPerformance.measurements() == oceanPerformanceSinglecore.measurements())
	{
		Log::info() << "Performance: Original vs. Ocean single-core (worst, best, average median in ms):";
		Log::info() << "  Original:            " << String::toAString(originalPerformance.worstMseconds(), 3u) << ", " << String::toAString(originalPerformance.bestMseconds(), 3u) << ", " << String::toAString(originalPerformance.averageMseconds(), 3u) << ", " << String::toAString(originalPerformance.medianMseconds(), 3u);
		Log::info() << "  Ocean (single-core): " << String::toAString(oceanPerformanceSinglecore.worstMseconds(), 3u) << ", " << String::toAString(oceanPerformanceSinglecore.bestMseconds(), 3u) << ", " << String::toAString(oceanPerformanceSinglecore.averageMseconds(), 3u) << ", " << String::toAString(oceanPerformanceSinglecore.medianMseconds(), 3u);
		Log::info() << "  Improvements:        " << String::toAString(originalPerformance.worstMseconds() / oceanPerformanceSinglecore.worstMseconds(), 2u) << ", " << String::toAString(originalPerformance.bestMseconds() / oceanPerformanceSinglecore.bestMseconds(), 2u) << ", " << String::toAString(originalPerformance.averageMseconds() / oceanPerformanceSinglecore.averageMseconds(), 2u) << ", " << String::toAString(originalPerformance.medianMseconds() / oceanPerformanceSinglecore.medianMseconds(), 2u) << " x";
	}

	Log::info().newLine();

	if (originalPerformance.measurements() != 0u && originalPerformance.measurements() == oceanPerformanceMulticore.measurements())
	{
		Log::info() << "Performance: Original vs. Ocean multi-core (worst, best, average median in ms):";
		Log::info() << "  Original:            " << String::toAString(originalPerformance.worstMseconds(), 3u) << ", " << String::toAString(originalPerformance.bestMseconds(), 3u) << ", " << String::toAString(originalPerformance.averageMseconds(), 3u) << ", " << String::toAString(originalPerformance.medianMseconds(), 3u);
		Log::info() << "  Ocean (multi-core):  " << String::toAString(oceanPerformanceMulticore.worstMseconds(), 3u) << ", " << String::toAString(oceanPerformanceMulticore.bestMseconds(), 3u) << ", " << String::toAString(oceanPerformanceMulticore.averageMseconds(), 3u) << ", " << String::toAString(oceanPerformanceMulticore.medianMseconds(), 3u);
		Log::info() << "  Improvements:        " << String::toAString(originalPerformance.worstMseconds() / oceanPerformanceMulticore.worstMseconds(), 2u) << ", " << String::toAString(originalPerformance.bestMseconds() / oceanPerformanceMulticore.bestMseconds(), 2u) << ", " << String::toAString(originalPerformance.averageMseconds() / oceanPerformanceMulticore.averageMseconds(), 2u) << ", " << String::toAString(originalPerformance.medianMseconds() / oceanPerformanceMulticore.medianMseconds(), 2u) << " x";
	}

	const double testedPointsFailedRatioSingleCore = testedPointsTotal != 0u ? double(testedPointsFailedSingleCore) / double(testedPointsTotal) : 1.0;
	const double testedPointsFailedRatioMultiCore = testedPointsTotal != 0u ? double(testedPointsFailedMultiCore) / double(testedPointsTotal) : 1.0;

	constexpr double failureAcceptanceThreshold = tSize == 64 ? 0.005 : 0.002; // 0.5% for 64-byte descriptors, otherwise 0.2%
	const bool succeeded = testedPointsFailedRatioSingleCore < failureAcceptanceThreshold && testedPointsFailedRatioMultiCore < failureAcceptanceThreshold;

	Log::info().newLine();

	if (succeeded)
	{
		Log::info() << "Validation successful";
	}
	else
	{
		Log::info() << "Validation FAILED!";
		Log::info() << "Percentage of failed validations (single-core): " << String::toAString(testedPointsFailedRatioSingleCore * 100.0f, 2u) << " %";
		Log::info() << "Percentage of failed validations (multi-core): " << String::toAString(testedPointsFailedRatioMultiCore * 100.0f, 2u) << " %";
	}

	return succeeded;
}

template <size_t tSize>
bool TestFREAKDescriptorT<tSize>::testCreateBlurredFramePyramid(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing creation of blurred frame pyramid:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTime(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 20u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 20u, 2000u);

		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, pixelOrigin), &randomGenerator);

		const unsigned int kernelWidth = RandomI::random(randomGenerator, 1u, 15u) | 0x01u; // ensuring that the kernel is odd
		const unsigned int kernelHeight = RandomI::random(randomGenerator, 1u, 15u) | 0x01u;

		Worker* useWorker = RandomI::boolean(randomGenerator) ? &worker : nullptr;

		const unsigned int maximalLayers = CV::FramePyramid::idealLayers(width, height, 0u);

		const unsigned int layers = RandomI::random(randomGenerator, 1u, maximalLayers);

		const CV::FramePyramid blurredFramePyramid = FREAKDescriptorT<tSize>::createFramePyramidWithBlur8BitsPerChannel(yFrame, kernelWidth, kernelHeight, layers, useWorker);

		if (blurredFramePyramid.layers() == layers)
		{
			// the first layer must be identical

			for (unsigned int y = 0u; y < yFrame.height(); ++y)
			{
				if (memcmp(yFrame.constrow<void>(y), blurredFramePyramid.finestLayer().constrow<void>(y), yFrame.planeWidthBytes(0u)) != 0)
				{
					allSucceeded = false;
				}
			}

			// the remaining layers are based on the blurred version of the finer pyramid layer and then downsampled

			Frame finerLayer(yFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

			for (unsigned int layerIndex = 1u; layerIndex < blurredFramePyramid.layers(); ++layerIndex)
			{
				if (kernelWidth <= finerLayer.width() && kernelHeight <= finerLayer.height()) // we skip the blur if the layer is already too small
				{
					if (!CV::FrameFilterGaussian::filter<uint8_t, uint32_t>(finerLayer.data<uint8_t>(), finerLayer.width(), finerLayer.height(), finerLayer.channels(), finerLayer.paddingElements(), kernelWidth, kernelHeight, -1.0f, &worker))
					{
						allSucceeded = false;
					}
				}

				const CV::FramePyramid twoLayerPyramid(CV::FramePyramid::DM_FILTER_11, std::move(finerLayer), 2u, &worker);

				const Frame& blurredFrameCoarserLayer = blurredFramePyramid[layerIndex];
				Frame testCoarserLayer(twoLayerPyramid.coarsestLayer(), Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

				ocean_assert(blurredFrameCoarserLayer.isFrameTypeCompatible(testCoarserLayer, false));

				for (unsigned int y = 0u; y < blurredFrameCoarserLayer.height(); ++y)
				{
					if (memcmp(testCoarserLayer.constrow<void>(y), blurredFrameCoarserLayer.constrow<void>(y), testCoarserLayer.planeWidthBytes(0u)) != 0)
					{
						allSucceeded = false;
					}
				}

				finerLayer = std::move(testCoarserLayer);
			}
		}
		else
		{
			allSucceeded = false;
		}
	}
	while (startTime + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: successful";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

// clang-format off

namespace frl
{

// <arvr/libraries/perception/components/features/freak/include/perception/features/freak/Freak.h>

// Binary descriptor as an array of bytes
// The struct is aligned to uint64_t so that we can speed up computing the distance between two
// descriptors by iterating through descriptors in 64 bit chunks
template <size_t Bytes>
struct alignas(uint64_t) SingleBinaryDescriptor : public std::array<uint8_t, Bytes> {};

// 3 descriptors for a given keypoint.
// First descriptor is computed at the same level as the keypoint was detected.
// Second is at a scale factor of 1.26 -> exp(log(2)*1/3)
// Third is at scale factor of 1.59 -> exp(log(2)*2/3)
// Because to compute second and third descriptors require accessing a bigger patch
// it might be possible that for points close to the image border we cannot compute
// them. The numValid value indicates how many descriptors are available.
template <size_t Bytes>
struct MultiLevelBinaryDescriptor {
  std::array<SingleBinaryDescriptor<Bytes>, 3> descriptors;
  uint8_t numValid = 0;
};

const size_t kBytesFREAK32 = 32;
using Multi3FREAK32 = MultiLevelBinaryDescriptor<kBytesFREAK32>;

const size_t kBytesFREAK64 = 64;
using Multi3FREAK64 = MultiLevelBinaryDescriptor<kBytesFREAK64>;

// Computes the distance between two multi level descriptors.
// This function performs only 5 comparison instead of brute force 9 comparisons
template <size_t Bytes>
int distance(
    const MultiLevelBinaryDescriptor<Bytes>& descriptors1,
    const MultiLevelBinaryDescriptor<Bytes>& descriptors2);

// Computes the distance between two single descriptors
template <size_t Bytes>
int distance(
    const SingleBinaryDescriptor<Bytes>& descriptor1,
    const SingleBinaryDescriptor<Bytes>& descriptor2);

// Struct with info and pointers to image data in a pyramid
template <typename PixelType, int MaxLevel>
struct ImagePyramidInfoAndPtr {
  const PixelType* dataAtLvl[MaxLevel];
  uint32_t pitchAtLvl[MaxLevel];
  uint32_t widthAtLvl[MaxLevel];
  uint32_t heightAtLvl[MaxLevel];
  int maxValidLevel = 0;
};

// Compute a multi level descriptor for a single input keypoint.
// Returns false if for some reason (e.g. point to close to the border) it cannot
// be computed. It also requires that the pyramid has one level more than the keypoint level
// otherwise the function will return false.
// It requires the unprojection ray (unit vector), the focal length in pixels
// and the jacobian of the projection at the level of the point wrt the ray.
// The orientation of the keypoint in the image is also returned in [-pi,pi]
template <typename PixelType, int MaxLevel, size_t Bytes>
bool computeDescriptor(
    const ImagePyramidInfoAndPtr<PixelType, MaxLevel>& pyr,
    const Eigen::Vector2f& posAtLvl,
    const int& lvl,
    const float& invFocalLength,
    const Eigen::Vector3f& ray,
    const Eigen::Matrix<float, 2, 3>& d_posAtLvl_ray,
    float& angleInRads,
    MultiLevelBinaryDescriptor<Bytes>& descriptors);
// </ arvr/libraries/perception/components/features/freak/include/perception/features/freak/Freak.h>

// <arvr/libraries/perception/components/features/freak/include/perception/features/freak/Freak.cpp>
const int rkernelSize = 149;
const int rkernelRadius = 7;
const float rx7[149] = {0,  -3, -2, -1, 0,  1,  2,  3,  -4, -3, -2, -1, 0,  1,  2,  3,  4,  -5, -4,
                        -3, -2, -1, 0,  1,  2,  3,  4,  5,  -6, -5, -4, -3, -2, -1, 0,  1,  2,  3,
                        4,  5,  6,  -6, -5, -4, -3, -2, -1, 0,  1,  2,  3,  4,  5,  6,  -6, -5, -4,
                        -3, -2, -1, 0,  1,  2,  3,  4,  5,  6,  -7, -6, -5, -4, -3, -2, -1, 0,  1,
                        2,  3,  4,  5,  6,  7,  -6, -5, -4, -3, -2, -1, 0,  1,  2,  3,  4,  5,  6,
                        -6, -5, -4, -3, -2, -1, 0,  1,  2,  3,  4,  5,  6,  -6, -5, -4, -3, -2, -1,
                        0,  1,  2,  3,  4,  5,  6,  -5, -4, -3, -2, -1, 0,  1,  2,  3,  4,  5,  -4,
                        -3, -2, -1, 0,  1,  2,  3,  4,  -3, -2, -1, 0,  1,  2,  3,  0};
const float ry7[149] = {-7, -6, -6, -6, -6, -6, -6, -6, -5, -5, -5, -5, -5, -5, -5, -5, -5, -4, -4,
                        -4, -4, -4, -4, -4, -4, -4, -4, -4, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
                        -3, -3, -3, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -1,
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  0,  0,  0,  0,  0,  0,  0,  0,
                        0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,
                        3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,
                        5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  7};

inline bool checkKernelCorners(
    const Eigen::Matrix2f& H,
    const Eigen::Vector2f& p,
    const uint32_t w,
    const uint32_t h,
    const int size) {
  const int cornerX[4] = {-size, -size, size, size};
  const int cornerY[4] = {-size, size, -size, size};
  for (int i = 0; i < 4; i++) {
    const Eigen::Vector2f warpedCorner = p + H * Eigen::Vector2f(cornerX[i], cornerY[i]);
    if (warpedCorner[0] < 0 || warpedCorner[0] >= float(w) || warpedCorner[1] < 0 ||
        warpedCorner[1] >= float(h)) {
      return false;
    }
  }
  return true;
}

template <typename PixelType, int MaxLevel>
bool computeOrientationAndWarping(
    const ImagePyramidInfoAndPtr<PixelType, MaxLevel>& pyr,
    const Eigen::Vector2f& posAtLvl,
    const int& lvl,
    const float& invFocalLength,
    const Eigen::Vector3f& ray,
    const Eigen::Matrix<float, 2, 3>& d_posAtLvl_ray,
    Eigen::Matrix<float, 2, 2>& warpMatrix,
    float& angleInRads) {
  // Get any two perpendicular vectors in the plane perpendicular to the ray
  const Eigen::Vector3f vy(0, 1, 0);
  const Eigen::Vector3f nx = vy.cross(ray).normalized() * invFocalLength;
  const Eigen::Vector3f ny = ray.cross(nx);

  // Compute warping matrix
  Eigen::Matrix<float, 3, 2> N;
  N.col(0) = nx;
  N.col(1) = ny;
  const Eigen::Matrix2f H = d_posAtLvl_ray * N;

  // Check the orientation kernel is inside the image
  if (!checkKernelCorners(H, posAtLvl, pyr.widthAtLvl[lvl], pyr.heightAtLvl[lvl], rkernelRadius)) {
    return false;
  }

  // Compute intensity centroid
  float mx = 0;
  float my = 0;
  const int pitch = static_cast<int>(pyr.pitchAtLvl[lvl]);
  const PixelType* data = pyr.dataAtLvl[lvl];
  for (int i = 0; i < rkernelSize; i++) {
    const Eigen::Vector2f p = posAtLvl + H * Eigen::Vector2f(rx7[i], ry7[i]);
    const int u = static_cast<int>(round(p[0])); // Original uses std::round, which isn't available on Android
    const int v = static_cast<int>(round(p[1])); // Original uses std::round, which isn't available on Android
    const float c = static_cast<float>(data[v * pitch + u]);

    mx += rx7[i] * c;
    my += ry7[i] * c;
  }

  // Compute axes aligned with keypoint orientation
  const Eigen::Vector3f gy = (nx * mx + ny * my).normalized() * invFocalLength;
  const Eigen::Vector3f gx = (gy.cross(ray));

  // Compute warping matrix
  Eigen::Matrix<float, 3, 2> G;
  G.col(0) = gx;
  G.col(1) = gy;

  warpMatrix = d_posAtLvl_ray * G;

  // Compute angle in image coordinates
  const Eigen::Vector2f patch_y = d_posAtLvl_ray * gy;
  angleInRads = std::atan2(patch_y[1], patch_y[0]);

  return true;
}

// FREAK cells
const int ncells = 43;
const float cellX[ncells] = {
    0,         -14.7216f, -14.7216f, 0,        14.7216f,  14.7216f,  -6.3745f, -12.749f,  -6.3745f,
    6.3745f,   12.749f,   6.3745f,   0,        -7.97392f, -7.97392f, 0,        7.97392f,  7.97392f,
    -3.18725f, -6.3745f,  -3.18725f, 3.18725f, 6.3745f,   3.18725f,  0,        -3.67983f, -3.67983f,
    0,         3.67983f,  3.67983f,  -1.4163f, -2.8326f,  -1.4163f,  1.4163f,  2.8326f,   1.4163f,
    0,         -1.84049f, -1.84049f, 0,        1.84049f,  1.84049f,  0};
const float cellY[ncells] = {
    16.9991f,  8.49895f,  -8.49895f, -16.9991f, -8.49895f, 8.49895f,  11.0406f,  0,
    -11.0406f, -11.0406f, 0,         11.0406f,  9.2071f,   4.60355f,  -4.60355f, -9.2071f,
    -4.60355f, 4.60355f,  5.52032f,  0,         -5.52032f, -5.52032f, 0,         5.52032f,
    4.25005f,  2.12445f,  -2.12445f, -4.25005f, -2.12445f, 2.12445f,  2.4536f,   0,
    -2.4536f,  -2.4536f,  0,         2.4536f,   2.12445f,  1.0628f,   -1.0628f,  -2.12445f,
    -1.0628f,  1.0628f,   0};

const int lvlUps[ncells] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int radius[ncells] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                            3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

// the pairs are already randomly shuffled so that subselecting any number will give good
// performance
const int npairs = 512;
const uint8_t pairs[npairs][2] = {
    {37, 4},  {38, 4},  {12, 0},  {39, 10}, {27, 7},  {37, 29}, {20, 16}, {33, 16}, {14, 0},
    {31, 3},  {17, 4},  {24, 12}, {33, 22}, {31, 7},  {35, 30}, {25, 6},  {34, 31}, {20, 19},
    {22, 17}, {16, 6},  {23, 5},  {26, 10}, {13, 5},  {31, 17}, {17, 10}, {31, 28}, {22, 4},
    {29, 11}, {28, 2},  {29, 19}, {30, 6},  {37, 10}, {31, 2},  {41, 13}, {14, 7},  {15, 3},
    {33, 4},  {18, 17}, {23, 19}, {33, 28}, {41, 24}, {34, 16}, {7, 1},   {26, 5},  {36, 13},
    {42, 9},  {20, 14}, {27, 26}, {41, 6},  {40, 19}, {26, 3},  {36, 29}, {23, 13}, {40, 7},
    {18, 0},  {28, 22}, {22, 9},  {26, 16}, {21, 16}, {39, 20}, {8, 3},   {14, 1},  {12, 11},
    {31, 25}, {29, 4},  {15, 1},  {41, 22}, {35, 1},  {26, 2},  {34, 14}, {25, 1},  {34, 17},
    {34, 29}, {16, 14}, {19, 3},  {26, 14}, {15, 5},  {25, 17}, {25, 5},  {34, 25}, {6, 0},
    {23, 10}, {29, 24}, {28, 16}, {20, 3},  {7, 4},   {25, 11}, {36, 24}, {27, 9},  {11, 10},
    {23, 7},  {32, 19}, {32, 16}, {37, 18}, {25, 24}, {19, 1},  {22, 20}, {38, 14}, {41, 31},
    {16, 10}, {19, 6},  {16, 11}, {31, 20}, {8, 0},   {14, 2},  {19, 0},  {37, 13}, {34, 4},
    {31, 14}, {6, 1},   {40, 1},  {24, 18}, {41, 1},  {41, 7},  {36, 23}, {40, 20}, {40, 27},
    {13, 0},  {19, 12}, {42, 38}, {16, 7},  {34, 7},  {9, 2},   {28, 4},  {11, 5},  {40, 38},
    {17, 2},  {5, 0},   {19, 14}, {12, 6},  {19, 17}, {40, 22}, {26, 7},  {19, 5},  {19, 11},
    {28, 26}, {12, 1},  {34, 0},  {5, 1},   {27, 16}, {21, 15}, {29, 25}, {19, 8},  {32, 26},
    {37, 17}, {11, 6},  {22, 6},  {39, 27}, {41, 37}, {21, 5},  {14, 11}, {31, 16}, {38, 28},
    {16, 0},  {29, 10}, {31, 26}, {10, 1},  {22, 13}, {10, 3},  {17, 3},  {42, 30}, {8, 4},
    {26, 6},  {22, 8},  {38, 27}, {26, 22}, {41, 10}, {42, 13}, {40, 34}, {13, 7},  {30, 11},
    {38, 22}, {33, 27}, {19, 15}, {29, 7},  {31, 10}, {26, 15}, {13, 12}, {29, 2},  {5, 3},
    {15, 7},  {28, 10}, {29, 17}, {40, 10}, {21, 1},  {15, 10}, {37, 11}, {40, 13}, {26, 1},
    {39, 21}, {34, 21}, {40, 31}, {19, 7},  {16, 5},  {40, 39}, {37, 7},  {30, 23}, {10, 9},
    {36, 30}, {38, 0},  {18, 6},  {40, 32}, {38, 10}, {22, 3},  {26, 19}, {18, 13}, {39, 22},
    {35, 17}, {31, 19}, {18, 11}, {28, 19}, {28, 0},  {37, 31}, {30, 7},  {27, 20}, {34, 10},
    {38, 3},  {37, 23}, {18, 7},  {38, 20}, {25, 19}, {20, 7},  {22, 18}, {7, 3},   {15, 2},
    {23, 12}, {26, 13}, {38, 7},  {11, 1},  {20, 8},  {33, 21}, {37, 36}, {17, 16}, {36, 35},
    {41, 2},  {37, 35}, {37, 2},  {15, 14}, {10, 7},  {41, 29}, {7, 6},   {32, 22}, {34, 26},
    {33, 2},  {38, 26}, {31, 0},  {11, 3},  {24, 23}, {13, 11}, {41, 19}, {41, 25}, {30, 13},
    {27, 10}, {39, 38}, {21, 3},  {31, 4},  {27, 14}, {37, 24}, {20, 2},  {25, 23}, {29, 1},
    {39, 28}, {17, 0},  {7, 0},   {9, 5},   {22, 2},  {33, 32}, {27, 21}, {30, 25}, {41, 23},
    {41, 30}, {15, 9},  {22, 10}, {31, 22}, {29, 5},  {34, 20}, {24, 13}, {31, 11}, {36, 25},
    {21, 19}, {19, 13}, {30, 29}, {33, 5},  {6, 4},   {5, 2},   {8, 2},   {10, 2},  {25, 13},
    {37, 19}, {28, 14}, {15, 4},  {10, 8},  {12, 5},  {14, 13}, {24, 1},  {31, 12}, {14, 10},
    {32, 27}, {19, 18}, {32, 4},  {22, 1},  {39, 26}, {17, 14}, {2, 1},   {1, 0},   {35, 23},
    {34, 2},  {33, 19}, {13, 3},  {39, 16}, {25, 2},  {41, 4},  {28, 7},  {31, 21}, {26, 4},
    {39, 19}, {24, 17}, {28, 20}, {21, 8},  {25, 7},  {34, 15}, {41, 36}, {16, 3},  {21, 20},
    {31, 15}, {26, 20}, {14, 5},  {38, 16}, {40, 2},  {18, 10}, {27, 8},  {29, 13}, {41, 18},
    {18, 12}, {40, 26}, {36, 0},  {21, 14}, {22, 0},  {27, 2},  {11, 0},  {21, 10}, {20, 10},
    {23, 6},  {13, 4},  {28, 21}, {22, 16}, {25, 22}, {35, 24}, {4, 0},   {31, 1},  {32, 21},
    {21, 4},  {37, 6},  {15, 8},  {8, 7},   {29, 22}, {28, 15}, {25, 18}, {41, 35}, {39, 14},
    {34, 12}, {23, 17}, {25, 10}, {39, 9},  {34, 13}, {22, 14}, {7, 2},   {20, 9},  {28, 11},
    {10, 4},  {40, 0},  {35, 13}, {38, 32}, {13, 2},  {39, 1},  {2, 0},   {38, 19}, {41, 11},
    {32, 28}, {39, 33}, {30, 17}, {16, 2},  {17, 6},  {13, 10}, {4, 1},   {10, 0},  {22, 19},
    {4, 3},   {12, 7},  {26, 21}, {9, 0},   {19, 16}, {34, 28}, {16, 9},  {9, 8},   {23, 0},
    {7, 5},   {10, 5},  {34, 18}, {14, 6},  {30, 5},  {31, 18}, {20, 15}, {34, 22}, {35, 12},
    {23, 1},  {35, 10}, {9, 3},   {27, 15}, {17, 13}, {37, 30}, {26, 0},  {28, 17}, {38, 33},
    {38, 5},  {16, 4},  {13, 1},  {28, 3},  {5, 4},   {12, 2},  {17, 9},  {31, 29}, {22, 11},
    {40, 17}, {25, 4},  {28, 27}, {29, 6},  {34, 1},  {14, 8},  {32, 15}, {39, 32}, {6, 5},
    {19, 4},  {18, 5},  {32, 20}, {38, 13}, {12, 10}, {24, 0},  {22, 15}, {36, 18}, {6, 3},
    {34, 23}, {33, 15}, {22, 7},  {22, 12}, {40, 28}, {35, 18}, {22, 5},  {29, 23}, {37, 34},
    {16, 13}, {23, 18}, {37, 22}, {29, 12}, {19, 2},  {14, 9},  {34, 19}, {19, 10}, {25, 12},
    {38, 21}, {28, 1},  {33, 20}, {27, 4},  {11, 7},  {31, 23}, {17, 7},  {17, 8},  {39, 8},
    {40, 21}, {16, 15}, {17, 5},  {30, 18}, {39, 7},  {37, 25}, {41, 34}, {30, 24}, {18, 1},
    {3, 1},   {9, 4},   {22, 21}, {31, 5},  {40, 3},  {35, 25}, {32, 2},  {4, 2},   {38, 31},
    {14, 3},  {21, 9},  {17, 12}, {16, 1},  {35, 29}, {23, 22}, {20, 1},  {34, 3},  {17, 1},
    {13, 6},  {40, 14}, {17, 11}, {38, 17}, {40, 16}, {20, 4},  {23, 11}, {12, 4},  {3, 2},
    {40, 33}, {14, 4},  {21, 2},  {33, 26}, {38, 34}, {29, 18}, {21, 7},  {16, 8}};

void warpCells(const Eigen::Matrix2f& H, float* warpedCellX, float* warpedCellY);

void warpCells(const Eigen::Matrix2f& H, float* warpedCellX, float* warpedCellY) {
  for (int i = 0; i < ncells; i++) {
    const Eigen::Vector2f p(cellX[i], cellY[i]);
    const Eigen::Vector2f Hp = H * p;
    warpedCellX[i] = Hp[0];
    warpedCellY[i] = Hp[1];
  }
}

// Average intensity kernels
const int kernel11Size = 5;
const int kernel11X[kernel11Size] = {0, -1, 0, 1, 0};
const int kernel11Y[kernel11Size] = {-1, 0, 0, 0, 1};

const int kernel22Size = 13;
const int kernel22X[kernel22Size] = {0, -1, 0, 1, -2, -1, 0, 1, 2, -1, 0, 1, 0};
const int kernel22Y[kernel22Size] = {-2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 2};

const int kernel33Size = 29;
const int kernel33X[kernel33Size] = {0, -2, -1, 0,  1,  2, -2, -1, 0,  1,  2, -3, -2, -1, 0,
                                     1, 2,  3,  -2, -1, 0, 1,  2,  -2, -1, 0, 1,  2,  0};
const int kernel33Y[kernel33Size] = {-3, -2, -2, -2, -2, -2, -1, -1, -1, -1, -1, 0, 0, 0, 0,
                                     0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2, 2, 3};

template <typename PixelType, int MaxLevel>
PixelType kernelAverage(
    const ImagePyramidInfoAndPtr<PixelType, MaxLevel>& pyr,
    const int& lvl,
    const int x,
    const int y,
    const int* kx,
    const int* ky,
    const int ks) {
  float sum = 0;
  int n = 0;
  const int pitch = static_cast<int>(pyr.pitchAtLvl[lvl]);
  const int w = static_cast<int>(pyr.widthAtLvl[lvl]);
  const int h = static_cast<int>(pyr.heightAtLvl[lvl]);
  for (int i = 0; i < ks; i++) {
    const int py = y + ky[i];
    const int px = x + kx[i];
    if (px >= 0 && px < w && py >= 0 && py < h) {
      sum += pyr.dataAtLvl[lvl][py * pitch + px];
      n++;
    }
  }
  return static_cast<PixelType>(sum / float(n));
}

template <typename PixelType, int MaxLevel>
PixelType getAverageIntensity(
    const ImagePyramidInfoAndPtr<PixelType, MaxLevel>& pyr,
    const int& lvl,
    const int x,
    const int y,
    const int r) {
  if (r == 1) {
    return kernelAverage(pyr, lvl, x, y, kernel11X, kernel11Y, kernel11Size);
  } else if (r == 2) {
    return kernelAverage(pyr, lvl, x, y, kernel22X, kernel22Y, kernel22Size);
  } else if (r == 3) {
    return kernelAverage(pyr, lvl, x, y, kernel33X, kernel33Y, kernel33Size);
  } else {
      //    throw std::runtime_error("kernel not implemented!");
      ocean_assert(false && "kernel not implemented");
      return PixelType(0);
  }
}

float scaleCoordinate(const float& pos, const float& scaleFactor);

float scaleCoordinate(const float& pos, const float& scaleFactor) {
  return (pos + 0.5f) * scaleFactor - 0.5f;
}

inline bool checkKernelCorners(const int x, const int y, const int w, const int h, const int size) {
  const int cornerX[4] = {-size, -size, size, size};
  const int cornerY[4] = {-size, size, -size, size};
  for (int i = 0; i < 4; i++) {
    const int px = x + cornerX[i];
    const int py = y + cornerY[i];
    if (px < 0 || px >= w || py < 0 || py >= h) {
      return false;
    }
  }
  return true;
}

template <typename PixelType, int MaxLevel>
bool getIntensityPyr(
    const ImagePyramidInfoAndPtr<PixelType, MaxLevel>& pyr,
    const int& lvl,
    const float& x,
    const float& y,
    const int& cellIdx,
    PixelType& outputIntensity) {
  const int sumLvl = lvl + lvlUps[cellIdx];
  if (sumLvl >= pyr.maxValidLevel) {
    return false;
  }

  const float scaleFactor = 1.0f / float(1 << (sumLvl - lvl));

  const int sumLvlX0 = static_cast<int>(round(scaleCoordinate(x, scaleFactor))); // Note: Original uses std::round, which isn't available on Android
  const int sumLvlY0 = static_cast<int>(round(scaleCoordinate(y, scaleFactor))); // Note: Original uses std::round, which isn't available on Android

  if (cellIdx < 6) {
    if (!checkKernelCorners(
            sumLvlX0,
            sumLvlY0,
            static_cast<int>(pyr.widthAtLvl[sumLvl]),
            static_cast<int>(pyr.heightAtLvl[sumLvl]),
            radius[cellIdx] / 2)) {
      return false;
    }
  }

  outputIntensity = getAverageIntensity(pyr, sumLvl, sumLvlX0, sumLvlY0, radius[cellIdx]);
  return true;
}

// Compute descriptor
template <typename PixelType, int MaxLevel, size_t Bytes>
bool computeDescriptor(
    const ImagePyramidInfoAndPtr<PixelType, MaxLevel>& pyr,
    const Eigen::Vector2f& posAtLvl,
    const int& lvl,
    const float& invFocalLength,
    const Eigen::Vector3f& ray,
    const Eigen::Matrix<float, 2, 3>& d_posAtLvl_ray,
    float& angleInRads,
    MultiLevelBinaryDescriptor<Bytes>& descriptors) {
  descriptors.numValid = 0;

  // Compute warping matrix
  Eigen::Matrix<float, 2, 2> H;
  if (!computeOrientationAndWarping<PixelType, MaxLevel>(
          pyr, posAtLvl, lvl, invFocalLength, ray, d_posAtLvl_ray, H, angleInRads)) {
    return false;
  }

  // Warp cells
  float warpedCellX[ncells];
  float warpedCellY[ncells];
  warpCells(H, warpedCellX, warpedCellY);

  // Compute a descriptor for each intra-level
  // 2^(0/3) = 1, 2^(1/3) = 1.2599, 2^(2/3) = 1.5874
  const float factorl[3] = {1.0f, 1.2599f, 1.5874f};
  for (uint8_t l = 0; l < 3; l++) {
    // Get cell intensities
    PixelType cellIntensities[ncells];
    bool bad = false;
    for (int i = 0; i < ncells; i++) {
      const float ptx = posAtLvl[0] + factorl[l] * warpedCellX[i];
      const float pty = posAtLvl[1] + factorl[l] * warpedCellY[i];

      if (!getIntensityPyr(pyr, lvl, ptx, pty, i, cellIntensities[i])) {
        bad = true;
        break;
      };
    }

    if (bad) {
      break;
    }

    // do the pair tests
    for (int i = 0; i < int(Bytes); i++) {
      uint32_t d = 0;
      for (int j = 0; j < 8; j++) {
        d = d << 1;

        const int idxPair = i * 8 + j;
        const int idx1 = pairs[idxPair][0];
        const int idx2 = pairs[idxPair][1];

        const float im1 = cellIntensities[idx1];
        const float im2 = cellIntensities[idx2];

        if (im1 > im2) {
	      d = d | 1;
        }
      }
	  descriptors.descriptors[l][i] = static_cast<uint8_t>(d);
    }

    descriptors.numValid = l + 1;
  }

  return descriptors.numValid > 0;
}
// </ arvr/libraries/perception/components/features/freak/include/perception/features/freak/Freak.cpp>

} // namespace frl

// clang-format off

template<size_t tSize>
bool TestFREAKDescriptorT<tSize>::computeOriginalDescriptor(const CV::FramePyramid& pyramid, const Eigen::Vector2f& point, const unsigned int pointPyramidLevel, CV::Detector::FREAKDescriptorT<tSize>& freakDescriptor, const Eigen::Vector3f& unprojectRayIF, const float inverseFocalLengthX, const Eigen::Matrix<float, 2, 3>& pointJacobianMatrixIF)
{
	ocean_assert(pyramid.layers() >= 6u && "perception::FREAK is hardcoded to accept a pyramid of 6 levels");
	ocean_assert(pointPyramidLevel < pyramid.layers());
	ocean_assert(inverseFocalLengthX > 0.0f);
	ocean_assert(pyramid.frameType().isPixelFormatCompatible(FrameType::FORMAT_Y8));

	frl::ImagePyramidInfoAndPtr<std::uint8_t, /* levels */ 6> imagePyramidInfoAndPtr;
	imagePyramidInfoAndPtr.maxValidLevel = int(pyramid.layers());
	for (unsigned int i = 0u; i < pyramid.layers(); ++i)
	{
		imagePyramidInfoAndPtr.dataAtLvl[i] = pyramid[i].constdata<std::uint8_t>();
		imagePyramidInfoAndPtr.pitchAtLvl[i] = int(pyramid[i].strideElements());
		imagePyramidInfoAndPtr.widthAtLvl[i] = int(pyramid[i].width());
		imagePyramidInfoAndPtr.heightAtLvl[i] = int(pyramid[i].height());
	}

	frl::MultiLevelBinaryDescriptor<tSize> descriptors;
	float orientation = 0.0f;
	const bool status = frl::computeDescriptor<std::uint8_t, 6, tSize>(imagePyramidInfoAndPtr, Eigen::Vector2f(float(point.x()), float(point.y())), int(pointPyramidLevel), inverseFocalLengthX, unprojectRayIF, pointJacobianMatrixIF, orientation, descriptors);

	ocean_assert(status == false || NumericF::isInsideRange(-NumericF::pi(), orientation, NumericF::pi()));
	ocean_assert(descriptors.numValid <= 3);

	if (status)
	{
		typename CV::Detector::FREAKDescriptorT<tSize>::MultilevelDescriptorData descriptorData = {{descriptors.descriptors[0], descriptors.descriptors[1], descriptors.descriptors[2]}};
		freakDescriptor = CV::Detector::FREAKDescriptorT<tSize>(std::move(descriptorData), descriptors.numValid, orientation);
	}

	return status;
}

template<size_t tSize>
void TestFREAKDescriptorT<tSize>::computeOriginalDescriptors(const CV::FramePyramid& pyramid, const std::vector<Eigen::Vector2f>& points, const unsigned int pointPyramidLevel, std::vector<CV::Detector::FREAKDescriptorT<tSize>>& freakDescriptors, const float inverseFocalLengthX, const typename CV::Detector::FREAKDescriptorT<tSize>::CameraDerivativeFunctor& cameraDerivativeFunctor)
{
	ocean_assert(pyramid.isValid());
	ocean_assert(pointPyramidLevel < pyramid.layers());

	freakDescriptors.resize(points.size());

	for (size_t  i = 0; i < points.size(); ++i)
	{
		const typename FREAKDescriptorT<tSize>::CameraDerivativeData cameraDerivativeData = cameraDerivativeFunctor.computeCameraDerivativeData(points[i], pointPyramidLevel);
		TestFREAKDescriptorT<tSize>::computeOriginalDescriptor(pyramid, points[i], pointPyramidLevel, freakDescriptors[i], cameraDerivativeData.unprojectRayIF, inverseFocalLengthX, cameraDerivativeData.pointJacobianMatrixIF);
	}
}

template<size_t tSize>
bool TestFREAKDescriptorT<tSize>::validateFREAKDescriptor(const CV::Detector::FREAKDescriptorT<tSize>& descriptor0, const CV::Detector::FREAKDescriptorT<tSize>& descriptor1, const unsigned int maxHammingDistanceThreshold, unsigned int* hammingDistances)
{
	if (descriptor0.descriptorLevels() != descriptor1.descriptorLevels())
	{
		return false;
	}

	unsigned int localHammingDistances[3];
	if (hammingDistances == nullptr)
	{
		hammingDistances = localHammingDistances;
	}

	memset(hammingDistances, 0u, descriptor0.descriptorLevels());

	for (unsigned int i = 0u; i < descriptor0.descriptorLevels(); ++i)
	{
		for (unsigned int byte = 0u; byte < tSize; ++byte)
		{
			const typename CV::Detector::FREAKDescriptorT<tSize>::PixelType difference = descriptor0.data()[i][byte] ^ descriptor1.data()[i][byte];
			hammingDistances[i] += (unsigned int)(std::bitset<sizeof(typename CV::Detector::FREAKDescriptorT<tSize>::PixelType)>(difference).count());
		}
	}

	return hammingDistances[0] <= maxHammingDistanceThreshold && hammingDistances[1] <= maxHammingDistanceThreshold && hammingDistances[2] <= maxHammingDistanceThreshold;
}

// Explicit instantiations of the test of the FREAK descriptor class
template class TestFREAKDescriptorT<32>;
template class TestFREAKDescriptorT<64>;

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
