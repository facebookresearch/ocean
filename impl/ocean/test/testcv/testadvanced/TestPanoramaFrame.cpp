/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestPanoramaFrame.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"

#include "ocean/math/Euler.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestPanoramaFrame::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Panorama Frame Test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testCameraFrame2cameraFrame(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCameraFrame2panoramaSubFrame(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRecreation(worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Panorama Frame Test succeeded.";
	}
	else
	{
		Log::info() << "Panorama Frame Test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

// Exact, mask, 1-4 channels

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Exact_Mask_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, true, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Exact_Mask_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, true, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Exact_Mask_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, true, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Exact_Mask_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, true, 4u, GTEST_TEST_DURATION, worker));
}

// Exact, no mask, 1-4 channels

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Exact_NoMask_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, false, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Exact_NoMask_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, false, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Exact_NoMask_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, false, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Exact_NoMask_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, false, false, 4u, GTEST_TEST_DURATION, worker));
}

// Approximated, mask, 1-4 channels

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Approximated_Mask_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, true, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Approximated_Mask_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, true, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Approximated_Mask_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, true, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Approximated_Mask_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, true, 4u, GTEST_TEST_DURATION, worker));
}

// Approximated, no mask, 1-4 channels

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Approximated_NoMask_1Channel)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, false, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Approximated_NoMask_2Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, false, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Approximated_NoMask_3Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, false, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestPanoramaFrame, CameraFrame2cameraFrame_Approximated_NoMask_4Channels)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2cameraFrame(GTEST_TEST_IMAGE_WIDTH, GTEST_TEST_IMAGE_HEIGHT, true, false, 4u, GTEST_TEST_DURATION, worker));
}


TEST(TestPanoramaFrame, CameraFrame2panoramaSubFrame)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testCameraFrame2panoramaSubFrame(GTEST_TEST_DURATION, worker));
}

// TODO This test needs to be optimized for Sandcastle. Currently, it will run too long and risk timing out.
#if 0
TEST(TestPanoramaFrame, Recreation)
{
	Worker worker;
	EXPECT_TRUE(TestPanoramaFrame::testRecreation(worker));
}
#endif

#endif // OCEAN_USE_GTEST

bool TestPanoramaFrame::testCameraFrame2cameraFrame(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	for (const bool useApproximation : {false, true})
	{
		for (const bool useSourceMask : {true, false})
		{
			for (unsigned int numberChannels = 1u; numberChannels <= 4u; ++numberChannels)
			{
				allSucceeded = TestPanoramaFrame::testCameraFrame2cameraFrame(1920u, 1080u, useApproximation, useSourceMask, numberChannels, testDuration, worker) && allSucceeded;

				Log::info() << " ";
			}
		}
	}

	return allSucceeded;
}

bool TestPanoramaFrame::testCameraFrame2cameraFrame(const unsigned int performanceWidth, const unsigned int performanceHeight, const bool useApproximation, const bool useSourceMask, const unsigned int numberChannels, const double testDuration, Worker& worker)
{
	ocean_assert(performanceWidth != 0u && performanceHeight != 0u && numberChannels != 0u && numberChannels <= 4u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing the conversion between frames with different orientations (" << (useApproximation ? "approximated" : "exact") << ", source mask: " << (useSourceMask ? "yes" : "no") << ", channels: " << numberChannels << "):";

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const bool performanceIteration : {true, false})
	{
		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				const unsigned int approximationBinSize = useApproximation ? RandomI::random(randomGenerator, 2u, 20u) : RandomI::random(randomGenerator, 1u);

				const unsigned int width = performanceIteration ? performanceWidth : RandomI::random(randomGenerator, 128u, 2048u);
				const unsigned int height = performanceIteration ? performanceHeight : RandomI::random(randomGenerator, 128u, 2048u);

				const Frame source = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(numberChannels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
				Frame target = CV::CVUtilities::randomizedFrame(source.frameType(), &randomGenerator);
				Frame targetMask = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const Frame clonedTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
				const Frame clonedTargetMask(targetMask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 1u) * 0xFFu);

				Frame sourceMask;

				if (useSourceMask)
				{
					sourceMask = CV::CVUtilities::randomizedBinaryMask(width, height, maskValue, &randomGenerator);
				}

				const PinholeCamera pinholeCamera(width, height, Random::scalar(randomGenerator, Numeric::deg2rad(Scalar(50)), Numeric::deg2rad(Scalar(70))));

				const SquareMatrix3 world_R_source = SquareMatrix3(Random::euler(randomGenerator, Numeric::deg2rad(Scalar(5))));
				const SquareMatrix3 world_R_target = SquareMatrix3(Random::euler(randomGenerator, Numeric::deg2rad(Scalar(5))));

				performance.startIf(performanceIteration);
					allSucceeded = CV::Advanced::PanoramaFrame::cameraFrame2cameraFrame(pinholeCamera, world_R_source, source, sourceMask, pinholeCamera, world_R_target, target, targetMask, maskValue, approximationBinSize, useWorker) && allSucceeded;
				performance.stopIf(performanceIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, clonedTarget) || !CV::CVUtilities::isPaddingMemoryIdentical(targetMask, clonedTargetMask))
				{
					Log::error() << "Padding area has been changed - potential memory access violation. Aborting immediately!";
					ocean_assert(false);

					return false;
				}

				if (!validateCameraFrame2cameraFrame(pinholeCamera, world_R_source, source, sourceMask, pinholeCamera, world_R_target, target, targetMask, maskValue, approximationBinSize))
				{
					allSucceeded = false;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
		Log::info() << "Multi-core boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
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

bool TestPanoramaFrame::testCameraFrame2panoramaSubFrame(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing creation of panorama sub-frame:";

	bool allSucceeded = true;

	CV::Advanced::PanoramaFrame panoramaFrame(1920u * 4u, 1920u * 2u, 0xFF, CV::Advanced::PanoramaFrame::UM_SET_ALL);

	const PinholeCamera pinholeCamera(1920u, 1080u, Numeric::deg2rad(60));

	Log::info() << "With camera frame resolution " << pinholeCamera.width() << "x" << pinholeCamera.height();

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		Log::info() << " ";
		Log::info() << "... for " << channels << " channels";

		uint64_t iterations = 0ull;
		uint64_t validIterations = 0ull;

		HighPerformanceStatistic performanceCreation;
		HighPerformanceStatistic performanceReconstruction;

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int cameraFramePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
			const unsigned int cameraMaskPaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

			Frame cameraFrame(FrameType(pinholeCamera.width(), pinholeCamera.height(), FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), cameraFramePaddingElements);

			Frame cameraMask(FrameType(cameraFrame, FrameType::FORMAT_Y8), cameraMaskPaddingElements);
			cameraMask.setValue(0xFF);

			const SquareMatrix3 orientation(Random::euler(Numeric::deg2rad(8)));

			CV::CVUtilities::randomizeFrame(cameraFrame, false);
			CV::FrameFilterGaussian::filter(cameraFrame, 5u, &worker);

			Frame subFrame;
			Frame subMask;

			performanceCreation.start();
			CV::PixelPositionI subFrameTopLeft;
			panoramaFrame.cameraFrame2panoramaSubFrame(pinholeCamera, cameraFrame, cameraMask, orientation, subFrame, subMask, subFrameTopLeft, 20u, &worker);
			performanceCreation.stop();

			ocean_assert(subFrameTopLeft.x() >= 0 && subFrameTopLeft.y() >= 0);

			Frame reconstructedCameraFrame;
			Frame reconstructedMaskFrame;

			performanceReconstruction.start();
				CV::Advanced::PanoramaFrame::panoramaFrame2cameraFrame(pinholeCamera, subFrame, subMask, panoramaFrame.dimensionWidth(), panoramaFrame.dimensionHeight(), CV::PixelPosition((unsigned int)(subFrameTopLeft.x()), (unsigned int)(subFrameTopLeft.y())), orientation, reconstructedCameraFrame, reconstructedMaskFrame, 0xFF, 20u, &worker);
			performanceReconstruction.stop();

			const double errorFrame = averageFrameError(cameraFrame, reconstructedCameraFrame);
			const double errorMask = averageFrameError(cameraMask, reconstructedMaskFrame);

			if (errorFrame <= 5.0 && errorMask <= 0.05)
			{
				validIterations++;
			}

			iterations++;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations > 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Average performance creation: " << String::toAString(performanceCreation.averageMseconds(), 1u) << "ms";
		Log::info() << "AVerage performance reconstruction: " << String::toAString(performanceReconstruction.averageMseconds(), 1u) << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "%";

		if (percent < 0.99)
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

bool TestPanoramaFrame::testRecreation(Worker& worker)
{
	Log::info() << "Testing re-creation:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int c = 1u; c <= 4u; ++c)
	{
		Log::info().newLine(c != 1u);
		Log::info().newLine(c != 1u);

		for (unsigned int a = 0u; a <= 1u; ++a)
		{
			Log::info().newLine(a != 0u);
			allSucceeded = testRecreation(c, a == 1u, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Re-creation validation: succeeded.";
	}
	else
	{
		Log::info() << "Re-creation validation: FAILED!";
	}

	return allSucceeded;
}

bool TestPanoramaFrame::testRecreation(const unsigned int channels, const bool approximate, Worker& worker)
{
	Log::info() << "... with " << channels << " channels, " << (approximate ? "approximated" : "non-approximated") << ":";

	bool allSucceeded = true;

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels);

	const unsigned int approximateBinSize = approximate ? 20u : 0u;

	const unsigned int cameraWidth = (unsigned int)(640 + RandomI::random(-10, 10));
	const unsigned int cameraHeight = (unsigned int)(480 + RandomI::random(-10, 10));

	const Scalar cameraFovX = Numeric::deg2rad(Scalar(60) + Random::scalar(-5, 5));

	const PinholeCamera pinholeCamera(cameraWidth, cameraHeight, cameraFovX);

	const unsigned int panoramaDimensionWidth = (unsigned int)(640 * 10 + RandomI::random(-100, 100));
	const unsigned int panoramaDimensionHeight = (unsigned int)(int(panoramaDimensionWidth) / 2 + RandomI::random(-20, 20));

	CV::Advanced::PanoramaFrame panoramaFrame(panoramaDimensionWidth, panoramaDimensionHeight, 0xFF, CV::Advanced::PanoramaFrame::UM_AVERAGE_LOCAL);

	for (Scalar pitch = Numeric::deg2rad(80); pitch >= -Numeric::deg2rad(81); pitch -= Numeric::deg2rad(40))
	{
		for (Scalar yaw = 0; yaw <= Numeric::pi2(); yaw += Numeric::deg2rad(30))
		{
			const unsigned int framePaddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

			Frame frame(FrameType(pinholeCamera.width(), pinholeCamera.height(), pixelFormat, FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
			CV::CVUtilities::randomizeFrame(frame, false);

			const SquareMatrix3 orientation(Euler(yaw, pitch, 0));

			panoramaFrame.addFrame(pinholeCamera, orientation, frame, Frame(), 20u, &worker);
		}
	}

	Eulers eulers;

	for (Scalar pitch = Numeric::deg2rad(80); pitch >= -Numeric::deg2rad(81); pitch -= Numeric::deg2rad(40))
	{
		for (Scalar yaw = 0; yaw <= Numeric::pi2(); yaw += Numeric::deg2rad(30))
		{
			const Scalar randomYaw = yaw + Numeric::deg2rad(Random::scalar(-2, 2));
			const Scalar randomPitch = pitch + Numeric::deg2rad(Random::scalar(-2, 2));
			const Scalar randomRoll = Numeric::deg2rad(Random::scalar(-5, 5));

			const Euler randomEuler(randomYaw, randomPitch, randomRoll);

			eulers.push_back(randomEuler);
		}
	}

	for (size_t n = 0; n < eulers.size(); ++n)
	{
		const size_t indexA = RandomI::random((unsigned int)(eulers.size()) - 1u);
		const size_t indexB = RandomI::random((unsigned int)(eulers.size()) - 1u);

		std::swap(eulers[indexA], eulers[indexB]);
	}

	CV::Advanced::PanoramaFrame newPanoramaFrame(panoramaFrame.dimensionWidth(), panoramaFrame.dimensionHeight(), 0xFF, CV::Advanced::PanoramaFrame::UM_SET_ALL);

	Frame frame;
	Frame mask;

	HighPerformanceTimer timerSingleCore;

	for (size_t n = 0; n < eulers.size(); ++n)
	{
		const SquareMatrix3 orientation(eulers[n]);

		panoramaFrame.extractFrame(pinholeCamera, orientation, frame, mask, approximateBinSize, nullptr);
		newPanoramaFrame.addFrame(pinholeCamera, orientation, frame, mask, approximateBinSize, nullptr);
	}

	Log::info() << "Performance: " << String::toAString(timerSingleCore.seconds(), 2u) << "s";

	if (averageFrameError(panoramaFrame.frame(), newPanoramaFrame.frame()) >= 20.0)
	{
		allSucceeded = false;

#ifdef OCEAN_DEBUG
		Frame distanceFrame(newPanoramaFrame.frame().frameType());

		for (unsigned int y = 0u; y < distanceFrame.height(); ++y)
		{
			const uint8_t* const newPanoramaFrameRow = newPanoramaFrame.frame().constrow<uint8_t>(y);
			const uint8_t* const panoramaFrameRow = panoramaFrame.frame().constrow<uint8_t>(y);

			uint8_t* const distanceRow = distanceFrame.row<uint8_t>(y);

			for (unsigned int n = 0u; n < distanceFrame.planeWidthElements(0u); ++n)
			{
				distanceRow[n] = uint8_t(abs(int(newPanoramaFrameRow[n]) - int(panoramaFrameRow[n])));
			}
		}
#endif // OCEAN_DEBUG
	}

	if (worker)
	{
		HighPerformanceTimer timerMulticore;

		for (size_t n = 0; n < eulers.size(); ++n)
		{
			const SquareMatrix3 orientation(eulers[n]);

			panoramaFrame.extractFrame(pinholeCamera, orientation, frame, mask, approximateBinSize, &worker);
			newPanoramaFrame.addFrame(pinholeCamera, orientation, frame, mask, approximateBinSize, &worker);
		}

		Log::info() << "Performance (multi-core): " << String::toAString(timerMulticore.seconds(), 2u) << "s";
		Log::info() << "Multi-core boost factor: " << String::toAString(timerSingleCore.seconds() / timerMulticore.seconds(), 1u) << "x";

		if (averageFrameError(panoramaFrame.frame(), newPanoramaFrame.frame()) >= 20.0)
		{
			allSucceeded = false;

#ifdef OCEAN_DEBUG
			Frame distanceFrame(newPanoramaFrame.frame().frameType());

			for (unsigned int y = 0u; y < distanceFrame.height(); ++y)
			{
				const uint8_t* const newPanoramaFrameRow = newPanoramaFrame.frame().constrow<uint8_t>(y);
				const uint8_t* const panoramaFrameRow = panoramaFrame.frame().constrow<uint8_t>(y);

				uint8_t* const distanceRow = distanceFrame.row<uint8_t>(y);

				for (unsigned int n = 0u; n < distanceFrame.planeWidthElements(0u); ++n)
				{
					distanceRow[n] = uint8_t(abs(int(newPanoramaFrameRow[n]) - int(panoramaFrameRow[n])));
				}
			}
#endif // OCEAN_DEBUG
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

double TestPanoramaFrame::averageFrameError(const Frame& frameA, const Frame& frameB)
{
	ocean_assert(frameA.isValid() && frameB.isValid());
	ocean_assert(frameA.numberPlanes() == 1u && frameA.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(frameA.frameType() == frameB.frameType());

	if (!frameA.isValid() || frameA.numberPlanes() != 1u || frameA.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return NumericD::maxValue();
	}

	if (!frameA.isFrameTypeCompatible(frameB, false))
	{
		return NumericD::maxValue();
	}

	double sumError = 0.0;

	for (unsigned int y = 0u; y < frameA.height(); ++y)
	{
		const uint8_t* rowA = frameA.constrow<uint8_t>(y);
		const uint8_t* rowB = frameB.constrow<uint8_t>(y);

		for (unsigned int n = 0u; n < frameA.planeWidthElements(0u); ++n)
		{
			sumError += double(NumericD::abs(rowA[n] - rowB[n]));
		}
	}

	const unsigned int elements = frameA.pixels() * frameA.channels();
	ocean_assert(elements != 0u);

	return sumError / double(elements);
}

bool TestPanoramaFrame::validateCameraFrame2cameraFrame(const PinholeCamera& sourceCamera, const SquareMatrix3& world_R_source, const Frame& source, const Frame& sourceMask, const PinholeCamera& targetCamera, const SquareMatrix3& world_R_target, const Frame& testTarget, const Frame& testTargetMask, const uint8_t maskValue, const unsigned int approximationBinSize)
{
	// **TODO** re-implement test to ensure that test is independent from actual implementation (e.g., not using cameraFrame2cameraFrameLookupTable etc.)

	ocean_assert(sourceCamera.isValid());
	ocean_assert(world_R_source.isOrthonormal());
	ocean_assert(source.isValid() && source.width() == sourceCamera.width() && source.height() == sourceCamera.height() && source.channels() <= 4u && source.numberPlanes() == 1u && source.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);
	ocean_assert(!sourceMask.isValid() || (sourceMask.width() == sourceCamera.width() && sourceMask.height() == sourceCamera.height() && sourceMask.channels() == 1u && sourceMask.numberPlanes() == 1u && sourceMask.pixelOrigin() == source.pixelOrigin()));

	ocean_assert(targetCamera.isValid());
	ocean_assert(world_R_target.isOrthonormal());
	ocean_assert(testTarget.isValid() && testTarget.width() == targetCamera.width() && testTarget.height() == targetCamera.height() && testTarget.isPixelFormatCompatible(source.pixelFormat()) && testTarget.pixelOrigin() == source.pixelOrigin());
	ocean_assert(testTargetMask.isValid() && testTargetMask.width() == targetCamera.width() && testTargetMask.height() == targetCamera.height() && testTargetMask.channels() == 1u && testTargetMask.numberPlanes() == 1u && testTargetMask.pixelOrigin() == source.pixelOrigin());

	ocean_assert(maskValue == 0x00u || maskValue == 0xFFu);

	const bool useSourceMask = sourceMask.isValid();
	const bool useApproximation = approximationBinSize > 1u;

	const SquareMatrix3 flippedWorld_T_source(world_R_source * PinholeCamera::flipMatrix3());
	const SquareMatrix3 flippedWorld_T_target(world_R_target * PinholeCamera::flipMatrix3());

	const SquareMatrix3 flippedSource_R_target(sourceCamera.intrinsic() * flippedWorld_T_source.inverted() * flippedWorld_T_target * targetCamera.invertedIntrinsic());

	uint8_t validationTargetPixel[4];
	uint8_t validationTargetMaskPixel;

	if (useApproximation)
	{
		const unsigned int binsX = min(targetCamera.width() / approximationBinSize, targetCamera.width() / 4u);
		const unsigned int binsY = min(targetCamera.height() / approximationBinSize, targetCamera.height() / 4u);
		CV::Advanced::PanoramaFrame::LookupTable lookupTable(targetCamera.width(), targetCamera.height(), binsX, binsY);

		cameraFrame2cameraFrameLookupTable(sourceCamera, world_R_source, targetCamera, world_R_target, lookupTable);

		if (useSourceMask)
		{
			for (unsigned int y = 0u; y < testTarget.height(); ++y)
			{
				for (unsigned int x = 0u; x < testTarget.width(); ++x)
				{
					const Vector2 sourcePosition = lookupTable.bilinearValue(Scalar(x), Scalar(y));

					if (!CV::Advanced::AdvancedFrameInterpolatorBilinear::Comfort::interpolatePixelWithMask8BitPerChannel(source.constdata<uint8_t>(), sourceMask.constdata<uint8_t>(), source.channels(), source.width(), source.height(), source.paddingElements(), sourceMask.paddingElements(), CV::PC_TOP_LEFT, sourcePosition, validationTargetPixel, validationTargetMaskPixel, maskValue))
					{
						ocean_assert(false && "Never be here!");
						return false;
					}

					const uint8_t* const testTargetPixel = testTarget.constpixel<uint8_t>(x, y);
					const uint8_t* const testTargetMaskPixel = testTargetMask.constpixel<uint8_t>(x, y);

					if (*testTargetMaskPixel != validationTargetMaskPixel)
					{
						return false;
					}

					if (validationTargetMaskPixel == maskValue)
					{
						for (unsigned int c = 0u; c < source.channels(); ++c)
						{
							if (testTargetPixel[c] != validationTargetPixel[c])
							{
								return false;
							}
						}
					}
					else
					{
						// Nothing to do - the values of validationTarget and testTargetPixel are undefined if the corresponding mask pixels are invalid.
					}
				}
			}
		}
		else
		{
			for (unsigned int y = 0u; y < testTarget.height(); ++y)
			{
				for (unsigned int x = 0u; x < testTarget.width(); ++x)
				{
					// Pixel center at (0.5, 0.5)
					const Vector2 sourcePosition = lookupTable.bilinearValue(Scalar(x), Scalar(y)) + Vector2(Scalar(0.5), Scalar(0.5));

					if (sourcePosition.x() >= Scalar(0) && sourcePosition.x() <= Scalar(source.width()) && sourcePosition.y() >= Scalar(0) && sourcePosition.y() <= Scalar(source.height()))
					{
						switch (source.channels())
						{
							case 1u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_CENTER>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, validationTargetPixel);
								break;

							case 2u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<2u, CV::PC_CENTER>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, validationTargetPixel);
								break;

							case 3u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_CENTER>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, validationTargetPixel);
								break;

							case 4u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<4u, CV::PC_CENTER>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, validationTargetPixel);
								break;

							default:
								ocean_assert(false && "Never be here!");
								return false;
						}

						validationTargetMaskPixel = maskValue;
					}
					else
					{
						// Values of validationTarget remain undefined.
						validationTargetMaskPixel = 0xFFu - maskValue;
					}

					const uint8_t* const testTargetPixel = testTarget.constpixel<uint8_t>(x, y);
					const uint8_t* const testTargetMaskPixel = testTargetMask.constpixel<uint8_t>(x, y);

					if (*testTargetMaskPixel != validationTargetMaskPixel)
					{
						return false;
					}

					if (validationTargetMaskPixel == maskValue)
					{
						for (unsigned int c = 0u; c < source.channels(); ++c)
						{
							if (testTargetPixel[c] != validationTargetPixel[c])
							{
								return false;
							}
						}
					}
					else
					{
						// Nothing to do - the values of validationTarget and testTargetPixel are undefined if the corresponding mask pixels are invalid.
					}
				}
			}
		}
	}
	else
	{
		if (useSourceMask)
		{
			for (unsigned int y = 0u; y < testTarget.height(); ++y)
			{
				for (unsigned int x = 0u; x < testTarget.width(); ++x)
				{
					const Vector2 sourcePosition = sourceCamera.distort<true>(flippedSource_R_target * targetCamera.undistort<true>(Vector2(Scalar(x), Scalar(y))));

					if (!CV::Advanced::AdvancedFrameInterpolatorBilinear::Comfort::interpolatePixelWithMask8BitPerChannel(source.constdata<uint8_t>(), sourceMask.constdata<uint8_t>(), source.channels(), source.width(), source.height(), source.paddingElements(), sourceMask.paddingElements(), CV::PC_TOP_LEFT, sourcePosition, validationTargetPixel, validationTargetMaskPixel, maskValue))
					{
						ocean_assert(false && "Never be here!");
						return false;
					}

					const uint8_t* const testTargetPixel = testTarget.constpixel<uint8_t>(x, y);
					const uint8_t* const testTargetMaskPixel = testTargetMask.constpixel<uint8_t>(x, y);

					if (*testTargetMaskPixel != validationTargetMaskPixel)
					{
						return false;
					}

					if (validationTargetMaskPixel == maskValue)
					{
						for (unsigned int c = 0u; c < source.channels(); ++c)
						{
							if (testTargetPixel[c] != validationTargetPixel[c])
							{
								return false;
							}
						}
					}
					else
					{
						// Nothing to do - the values of validationTarget and testTargetPixel are undefined if the corresponding mask pixels are invalid.
					}
				}
			}
		}
		else
		{
			for (unsigned int y = 0u; y < testTarget.height(); ++y)
			{
				for (unsigned int x = 0u; x < testTarget.width(); ++x)
				{
					// Pixel center at (0.5, 0.5)
					const Vector2 sourcePosition = sourceCamera.distort<true>(flippedSource_R_target * targetCamera.undistort<true>(Vector2(Scalar(x), Scalar(y)))) + Vector2(Scalar(0.5), Scalar(0.5));

					if (sourcePosition.x() >= Scalar(0) && sourcePosition.x() <= Scalar(source.width()) && sourcePosition.y() >= Scalar(0) && sourcePosition.y() <= Scalar(source.height()))
					{
						switch (source.channels())
						{
							case 1u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_CENTER>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, validationTargetPixel);
								break;

							case 2u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<2u, CV::PC_CENTER>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, validationTargetPixel);
								break;

							case 3u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_CENTER>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, validationTargetPixel);
								break;

							case 4u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<4u, CV::PC_CENTER>(source.constdata<uint8_t>(), source.width(), source.height(), source.paddingElements(), sourcePosition, validationTargetPixel);
								break;

							default:
								ocean_assert(false && "Never be here!");
								return false;
						}

						validationTargetMaskPixel = maskValue;
					}
					else
					{
						// Values of validationTarget remain undefined.
						validationTargetMaskPixel = 0xFFu - maskValue;
					}

					const uint8_t* const testTargetPixel = testTarget.constpixel<uint8_t>(x, y);
					const uint8_t* const testTargetMaskPixel = testTargetMask.constpixel<uint8_t>(x, y);

					if (*testTargetMaskPixel != validationTargetMaskPixel)
					{
						return false;
					}

					if (validationTargetMaskPixel == maskValue)
					{
						for (unsigned int c = 0u; c < source.channels(); ++c)
						{
							if (testTargetPixel[c] != validationTargetPixel[c])
							{
								return false;
							}
						}
					}
					else
					{
						// Nothing to do - the values of validationTarget and testTargetPixel are undefined if the corresponding mask pixels are invalid.
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

}
