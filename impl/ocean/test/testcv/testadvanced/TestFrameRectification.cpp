/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestFrameRectification.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterErosion.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/advanced/FrameRectification.h"

#include "ocean/math/Box2.h"
#include "ocean/math/Sphere3.h"
#include "ocean/math/Plane3.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestFrameRectification::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame Rectification Test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testPlanarRectangleObject(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testArbitraryRectangleObject(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTriangleObject(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPlanarRectangleObjectMask(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testArbitraryRectangleObjectMask(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTriangleObjectMask(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame Rectification Test succeeded.";
	}
	else
	{
		Log::info() << "Frame Rectification Test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameRectification, PlanarRectangleObject_200x200)
{
	Worker worker;
	EXPECT_TRUE(TestFrameRectification::testPlanarRectangleObject(200u, 200u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameRectification, ArbitraryRectangleObject_200x2000)
{
	Worker worker;
	EXPECT_TRUE(TestFrameRectification::testArbitraryRectangleObject(200u, 200u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameRectification, TriangleObject_200x200)
{
	Worker worker;
	EXPECT_TRUE(TestFrameRectification::testTriangleObject(200u, 200u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameRectification, PlanarRectangleObjectMask_200x200)
{
	Worker worker;
	EXPECT_TRUE(TestFrameRectification::testPlanarRectangleObjectMask(200u, 200u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameRectification, ArbitraryRectangleObjectMask_200x200)
{
	Worker worker;
	EXPECT_TRUE(TestFrameRectification::testArbitraryRectangleObjectMask(200u, 200u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameRectification, TriangleObjectMask_200x200)
{
	Worker worker;
	EXPECT_TRUE(TestFrameRectification::testTriangleObjectMask(200u, 200u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameRectification::testPlanarRectangleObject(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	double maximalAverageError = 0.0;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const IndexPairs32 objectResolutions = {IndexPair32(128u, 128u), IndexPair32(100u, 164u), IndexPair32(164u, 100u)};

	RandomGenerator randomGenerator;

	for (const bool lookup : {false, true})
	{
		if (lookup)
		{
			Log::info() << "Test lookup planar rectangle object " << width << "x" << height << ":";
		}
		else
		{
			Log::info() << "Test planar rectangle object " << width << "x" << height << ":";
		}

		Log::info() << " ";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			Log::info() << "... with " << channels << " channels:";

			const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

			HighPerformanceStatistic performanceSinglecore;
			HighPerformanceStatistic performanceMulticore;

			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
				HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

				const Timestamp startTimestamp(true);

				do
				{
					const IndexPair32 objectResolution = RandomI::random(randomGenerator, objectResolutions);

					Frame objectFrame = CV::CVUtilities::randomizedFrame(FrameType(objectResolution.first, objectResolution.second, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
					CV::FrameFilterGaussian::filter(objectFrame, 11u, &worker);

					Frame cameraFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

					const Frame copyCameraFrame(cameraFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					PinholeCamera pinholeCamera(cameraFrame.width(), cameraFrame.height(), Numeric::deg2rad(60));
					pinholeCamera.setRadialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.01), Random::scalar(randomGenerator, -1, 1) * Scalar(0.01)));
					pinholeCamera.setTangentialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.001), Random::scalar(randomGenerator, -1, 1) * Scalar(0.001)));

					const AnyCameraPinhole camera(pinholeCamera);

					const Vector2 objectDimension(1, Scalar(objectFrame.height()) / Scalar(objectFrame.width()));

					const Quaternion world_R_camera = Quaternion(Vector3(1, 0, 0), -Numeric::pi_2()) * Quaternion(Euler(Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50))));

					const HomogenousMatrix4 world_T_camera = determineCameraPose(camera, Box3(Vector3(0, 0, 0), Vector3(objectDimension.x(), 0, objectDimension.y())), world_R_camera);

					if (!renderPlanarRectangleObject(objectFrame, cameraFrame, camera, world_T_camera, objectDimension))
					{
						ocean_assert(false && "This should never happen!");
						maximalAverageError = NumericD::maxValue();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(cameraFrame, copyCameraFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					Frame resultFrame = CV::CVUtilities::randomizedFrame(objectFrame.frameType(), &randomGenerator);

					const Frame copyResultFrame(resultFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.start();
						CV::Advanced::FrameRectification::Comfort::planarRectangleObject(cameraFrame, camera, world_T_camera, Vector3(0, 0, 0), Vector3(objectDimension.x(), 0, 0), Vector3(0, 0, objectDimension.y()), resultFrame, useWorker, nullptr, lookup ? 20u : 0u);
					performance.stop();

					if (!CV::CVUtilities::isPaddingMemoryIdentical(resultFrame, copyResultFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					const double averageError = determineFrameError(objectFrame, resultFrame, nullptr, nullptr);

					if (averageError > maximalAverageError)
					{
						maximalAverageError = averageError;
					}
				}
				while (Timestamp(true) < startTimestamp + testDuration);
			}

			Log::info() << "Performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

			if (performanceMulticore.measurements() != 0u)
			{
				Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
				Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
			}

			Log::info() << " ";
		}
	}

	constexpr double threshold = 10.0;

	const bool allSucceeded = maximalAverageError < threshold;

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded (" << String::toAString(maximalAverageError, 1u) << " color error).";
	}
	else
	{
		Log::info() << "Validation: FAILED (" << String::toAString(maximalAverageError, 1u) << " color error)!";
	}

	return allSucceeded;
}

bool TestFrameRectification::testArbitraryRectangleObject(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	double maximalAverageError = 0.0;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const IndexPairs32 objectResolutions = {IndexPair32(128u, 128u), IndexPair32(100u, 164u), IndexPair32(164u, 100u)};

	RandomGenerator randomGenerator;

	for (const bool lookup : {false, true})
	{
		if (lookup)
		{
			Log::info() << "Test lookup arbitrary rectangle object " << width << "x" << height << ":";
		}
		else
		{
			Log::info() << "Test arbitrary rectangle " << width << "x" << height << ":";
		}

		Log::info() << " ";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			Log::info() << "... with " << channels << " channels:";

			const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

			HighPerformanceStatistic performanceSinglecore;
			HighPerformanceStatistic performanceMulticore;

			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
				HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

				const Timestamp startTimestamp(true);

				do
				{
					const IndexPair32 objectResolution = RandomI::random(randomGenerator, objectResolutions);

					Frame objectFrame = CV::CVUtilities::randomizedFrame(FrameType(objectResolution.first, objectResolution.second, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
					CV::FrameFilterGaussian::filter(objectFrame, 11u, &worker);

					Frame cameraFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

					const Frame copyCameraFrame(cameraFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					PinholeCamera pinholeCamera(cameraFrame.width(), cameraFrame.height(), Numeric::deg2rad(60));
					pinholeCamera.setRadialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.01), Random::scalar(randomGenerator, -1, 1) * Scalar(0.01)));
					pinholeCamera.setTangentialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.001), Random::scalar(randomGenerator, -1, 1) * Scalar(0.001)));

					const AnyCameraPinhole camera(pinholeCamera);

					const Vector2 objectDimension(1, Scalar(objectFrame.height()) / Scalar(objectFrame.width()));

					const Quaternion world_R_camera = Quaternion(Vector3(1, 0, 0), -Numeric::pi_2()) * Quaternion(Euler(Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50))));

					const HomogenousMatrix4 world_T_camera = determineCameraPose(camera, Box3(Vector3(0, 0, 0), Vector3(objectDimension.x(), 0, objectDimension.y())), world_R_camera);

					if (!renderPlanarRectangleObject(objectFrame, cameraFrame, camera, world_T_camera, objectDimension))
					{
						ocean_assert(false && "This should never happen!");
						maximalAverageError = NumericD::maxValue();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(cameraFrame, copyCameraFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					Frame resultFrame = CV::CVUtilities::randomizedFrame(objectFrame.frameType(), &randomGenerator);

					const Frame copyResultFrame(resultFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.start();
						CV::Advanced::FrameRectification::Comfort::arbitraryRectangleObject(cameraFrame, camera, world_T_camera, Vector3(0, 0, 0), Vector3(0, 0, objectDimension.y()), Vector3(objectDimension.x(), 0, objectDimension.y()), Vector3(objectDimension.x(), 0, 0), resultFrame, useWorker, nullptr,  lookup ? 20u : 0u);
					performance.stop();

					if (!CV::CVUtilities::isPaddingMemoryIdentical(resultFrame, copyResultFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					const double averageError = determineFrameError(objectFrame, resultFrame, nullptr, nullptr);

					if (averageError > maximalAverageError)
					{
						maximalAverageError = averageError;
					}
				}
				while (Timestamp(true) < startTimestamp + testDuration);
			}

			Log::info() << "Performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

			if (performanceMulticore.measurements() != 0u)
			{
				Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
				Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
			}

			Log::info() << " ";
		}
	}

	constexpr double threshold = 10.0;

	const bool allSucceeded = maximalAverageError < threshold;

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded (" << String::toAString(maximalAverageError, 1u) << " color error).";
	}
	else
	{
		Log::info() << "Validation: FAILED (" << String::toAString(maximalAverageError, 1u) << " color error)!";
	}

	return allSucceeded;
}

bool TestFrameRectification::testTriangleObject(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	double maximalAverageError = 0.0;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const IndexPairs32 objectResolutions = {IndexPair32(128u, 128u), IndexPair32(100u, 164u), IndexPair32(164u, 100u)};

	RandomGenerator randomGenerator;

	for (const bool lookup : {false, true})
	{
		if (lookup)
		{
			Log::info() << "Test lookup triangle object " << width << "x" << height << ":";
		}
		else
		{
			Log::info() << "Test triangle " << width << "x" << height << ":";
		}

		Log::info() << " ";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			Log::info() << "... with " << channels << " channels:";

			const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

			HighPerformanceStatistic performanceSinglecore;
			HighPerformanceStatistic performanceMulticore;

			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
				HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

				const Timestamp startTimestamp(true);

				do
				{
					const IndexPair32 objectResolution = RandomI::random(randomGenerator, objectResolutions);

					Frame objectFrame = CV::CVUtilities::randomizedFrame(FrameType(objectResolution.first, objectResolution.second, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
					CV::FrameFilterGaussian::filter(objectFrame, 11u, &worker);

					Frame cameraFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

					const Frame copyCameraFrame(cameraFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					PinholeCamera pinholeCamera(cameraFrame.width(), cameraFrame.height(), Numeric::deg2rad(60));
					pinholeCamera.setRadialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.01), Random::scalar(randomGenerator, -1, 1) * Scalar(0.01)));
					pinholeCamera.setTangentialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.001), Random::scalar(randomGenerator, -1, 1) * Scalar(0.001)));

					const AnyCameraPinhole camera(pinholeCamera);

					const Vector2 objectDimension(1, Scalar(objectFrame.height()) / Scalar(objectFrame.width()));

					constexpr size_t numberTriangles = 2;

					const Triangle2 triangles2[numberTriangles] =
					{
						Triangle2(Vector2(0, 0), Vector2(Scalar(0), Scalar(objectFrame.height())), Vector2(Scalar(objectFrame.width()), Scalar(objectFrame.height()))),
						Triangle2(Vector2(0, 0), Vector2(Scalar(objectFrame.width()), Scalar(objectFrame.height())), Vector2(Scalar(objectFrame.width()), 0))
					};

					const Triangle3 triangles3[numberTriangles] =
					{
						Triangle3(Vector3(0, 0, 0), Vector3(0, 0, objectDimension.y()), Vector3(objectDimension.x(), 0, objectDimension.y())),
						Triangle3(Vector3(0, 0, 0), Vector3(objectDimension.x(), 0, objectDimension.y()), Vector3(objectDimension.x(), 0, 0))
					};

					const Quaternion world_R_camera = Quaternion(Vector3(1, 0, 0), -Numeric::pi_2()) * Quaternion(Euler(Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50))));

					const HomogenousMatrix4 world_T_camera = determineCameraPose(camera, Box3(Vector3(0, 0, 0), Vector3(objectDimension.x(), 0, objectDimension.y())), world_R_camera);

					if (!renderPlanarRectangleObject(objectFrame, cameraFrame, AnyCameraPinhole(pinholeCamera), world_T_camera, objectDimension))
					{
						ocean_assert(false && "This should never happen!");
						maximalAverageError = NumericD::maxValue();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(cameraFrame, copyCameraFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					Frame resultFrame = CV::CVUtilities::randomizedFrame(objectFrame.frameType(), &randomGenerator);

					const Frame copyResultFrame(resultFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.start();
						for (unsigned int n = 0; n < numberTriangles; ++n)
						{
							CV::Advanced::FrameRectification::Comfort::triangleObject(cameraFrame, camera, world_T_camera, triangles2[n], triangles3[n], resultFrame, useWorker, nullptr, lookup ? 20u : 0u);
						}
					performance.stop();

					if (!CV::CVUtilities::isPaddingMemoryIdentical(resultFrame, copyResultFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					const double averageError = determineFrameError(objectFrame, resultFrame, nullptr, nullptr);

					if (averageError > maximalAverageError)
					{
						maximalAverageError = averageError;
					}
				}
				while (Timestamp(true) < startTimestamp + testDuration);
			}

			Log::info() << "Performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

			if (performanceMulticore.measurements() != 0u)
			{
				Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
				Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
			}

			Log::info() << " ";
		}
	}

	constexpr double threshold = 10.0;

	const bool allSucceeded = maximalAverageError < threshold;

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded (" << String::toAString(maximalAverageError, 1u) << " color error).";
	}
	else
	{
		Log::info() << "Validation: FAILED (" << String::toAString(maximalAverageError, 1u) << " color error)!";
	}

	return allSucceeded;
}

bool TestFrameRectification::testPlanarRectangleObjectMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	double maximalAverageError = 0.0;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const IndexPairs32 objectResolutions = {IndexPair32(128u, 128u), IndexPair32(100u, 164u), IndexPair32(164u, 100u)};

	RandomGenerator randomGenerator;

	for (const bool lookup : {false, true})
	{
		if (lookup)
		{
			Log::info() << "Test lookup planar rectangle object with mask " << width << "x" << height << ":";
		}
		else
		{
			Log::info() << "Test planar rectangle object with mask " << width << "x" << height << ":";
		}

		Log::info() << " ";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			Log::info() << "... with " << channels << " channels:";

			const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

			HighPerformanceStatistic performanceSinglecore;
			HighPerformanceStatistic performanceMulticore;

			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
				HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

				const Timestamp startTimestamp(true);

				do
				{
					const IndexPair32 objectResolution = RandomI::random(randomGenerator, objectResolutions);

					Frame objectFrame = CV::CVUtilities::randomizedFrame(FrameType(objectResolution.first, objectResolution.second, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

					CV::FrameFilterGaussian::filter(objectFrame, 11u, &worker);

					Frame cameraFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

					const Frame copyCameraFrame(cameraFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					PinholeCamera pinholeCamera(cameraFrame.width(), cameraFrame.height(), Numeric::deg2rad(60));
					pinholeCamera.setRadialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.01), Random::scalar(randomGenerator, -1, 1) * Scalar(0.01)));
					pinholeCamera.setTangentialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.001), Random::scalar(randomGenerator, -1, 1) * Scalar(0.001)));

					const AnyCameraPinhole camera(pinholeCamera);

					const Vector2 objectDimension(1, Scalar(objectFrame.height()) / Scalar(objectFrame.width()));

					const Quaternion world_R_camera = Quaternion(Vector3(1, 0, 0), -Numeric::pi_2()) * Quaternion(Euler(Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50))));

					const HomogenousMatrix4 world_T_camera = determineCameraPose(camera, Box3(Vector3(0, 0, 0), Vector3(objectDimension.x(), 0, objectDimension.y())), world_R_camera) * HomogenousMatrix4(Random::euler(randomGenerator, Numeric::deg2rad(20)));

					if (!renderPlanarRectangleObject(objectFrame, cameraFrame, camera, world_T_camera, objectDimension))
					{
						ocean_assert(false && "This should never happen!");
						maximalAverageError = NumericD::maxValue();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(cameraFrame, copyCameraFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					Frame resultFrame = CV::CVUtilities::randomizedFrame(objectFrame.frameType(), &randomGenerator);

					const Frame copyResultFrame(resultFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					constexpr uint8_t maskValue = 0xFF;

					Frame resultMask = CV::CVUtilities::randomizedFrame(FrameType(objectFrame, FrameType::FORMAT_Y8), &randomGenerator);

					const Frame copyResultMask(resultMask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.start();
						CV::Advanced::FrameRectification::Comfort::planarRectangleObjectMask(cameraFrame, camera, world_T_camera, Vector3(0, 0, 0), Vector3(objectDimension.x(), 0, 0), Vector3(0, 0, objectDimension.y()), resultFrame, resultMask, useWorker, maskValue, lookup ? 20u : 0u);
					performance.stop();

					if (!CV::CVUtilities::isPaddingMemoryIdentical(resultFrame, copyResultFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(resultMask, copyResultMask))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					// verifying and handling the mask

					unsigned int invalidMaskPixels = 0u;

					for (unsigned int y = 0u; y < resultFrame.height(); ++y)
					{
						const uint8_t* const maskRow = resultMask.constrow<uint8_t>(y);

						for (unsigned int x = 0u; x < resultFrame.width(); ++x)
						{
							if (maskRow[x] != maskValue)
							{
								memcpy(resultFrame.pixel<uint8_t>(x, y), objectFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * objectFrame.channels());

								const Scalar xObject = Scalar(x) * objectDimension.x() / Scalar(objectFrame.width());
								const Scalar zObject = Scalar(y) * objectDimension.y() / Scalar(objectFrame.height());

								const Vector2 projectedObjectPoint = camera.projectToImage(world_T_camera, Vector3(xObject, 0, zObject));

								const Scalar borderThreshold = lookup ? Scalar(3) : Scalar(1.5);

								if (camera.isInside(projectedObjectPoint, borderThreshold))
								{
									++invalidMaskPixels;
								}
							}
						}
					}

					const double averageError = determineFrameError(objectFrame, resultFrame, nullptr, nullptr);

					if (averageError > maximalAverageError)
					{
						maximalAverageError = averageError;
					}

					if (invalidMaskPixels > 2u)
					{
						maximalAverageError = 255.0;
					}
				}
				while (Timestamp(true) < startTimestamp + testDuration);
			}

			Log::info() << "Performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

			if (performanceMulticore.measurements() != 0u)
			{
				Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
				Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
			}

			Log::info() << " ";
		}
	}

	constexpr double threshold = 10.0;

	const bool allSucceeded = maximalAverageError < threshold;

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded (" << String::toAString(maximalAverageError, 1u) << " color error).";
	}
	else
	{
		Log::info() << "Validation: FAILED (" << String::toAString(maximalAverageError, 1u) << " color error)!";
	}

	return allSucceeded;
}

bool TestFrameRectification::testArbitraryRectangleObjectMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	double maximalAverageError = 0.0;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const IndexPairs32 objectResolutions = {IndexPair32(128u, 128u), IndexPair32(100u, 164u), IndexPair32(164u, 100u)};

	RandomGenerator randomGenerator;

	for (const bool lookup : {false, true})
	{
		if (lookup)
		{
			Log::info() << "Test lookup arbitrary rectangle object with mask " << width << "x" << height << ":";
		}
		else
		{
			Log::info() << "Test arbitrary rectangle object with mask " << width << "x" << height << ":";
		}

		Log::info() << " ";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			Log::info() << "... with " << channels << " channels:";

			const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

			HighPerformanceStatistic performanceSinglecore;
			HighPerformanceStatistic performanceMulticore;

			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
				HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

				const Timestamp startTimestamp(true);

				do
				{
					const IndexPair32 objectResolution = RandomI::random(randomGenerator, objectResolutions);

					Frame objectFrame = CV::CVUtilities::randomizedFrame(FrameType(objectResolution.first, objectResolution.second, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

					CV::FrameFilterGaussian::filter(objectFrame, 11u, &worker);

					Frame cameraFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

					const Frame copyCameraFrame(cameraFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					PinholeCamera pinholeCamera(cameraFrame.width(), cameraFrame.height(), Numeric::deg2rad(60));
					pinholeCamera.setRadialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.01), Random::scalar(randomGenerator, -1, 1) * Scalar(0.01)));
					pinholeCamera.setTangentialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.001), Random::scalar(randomGenerator, -1, 1) * Scalar(0.001)));

					const AnyCameraPinhole camera(pinholeCamera);

					const Vector2 objectDimension(1, Scalar(objectFrame.height()) / Scalar(objectFrame.width()));

					const Quaternion world_R_camera = Quaternion(Vector3(1, 0, 0), -Numeric::pi_2()) * Quaternion(Euler(Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50))));

					const HomogenousMatrix4 world_T_camera = determineCameraPose(camera, Box3(Vector3(0, 0, 0), Vector3(objectDimension.x(), 0, objectDimension.y())), world_R_camera) * HomogenousMatrix4(Random::euler(randomGenerator, Numeric::deg2rad(20)));

					if (!renderPlanarRectangleObject(objectFrame, cameraFrame, camera, world_T_camera, objectDimension))
					{
						ocean_assert(false && "This should never happen!");
						maximalAverageError = NumericD::maxValue();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(cameraFrame, copyCameraFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					Frame resultFrame = CV::CVUtilities::randomizedFrame(objectFrame.frameType(), &randomGenerator);

					const Frame copyResultFrame(resultFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					constexpr uint8_t maskValue = 0xFF;

					Frame resultMask = CV::CVUtilities::randomizedFrame(FrameType(objectFrame, FrameType::FORMAT_Y8), &randomGenerator);

					const Frame copyResultMask(resultMask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.start();
						CV::Advanced::FrameRectification::Comfort::arbitraryRectangleObjectMask(cameraFrame, camera, world_T_camera, Vector3(0, 0, 0), Vector3(0, 0, objectDimension.y()), Vector3(objectDimension.x(), 0, objectDimension.y()), Vector3(objectDimension.x(), 0, 0), resultFrame, resultMask, useWorker, maskValue, lookup ? 20u : 0u);
					performance.stop();

					if (!CV::CVUtilities::isPaddingMemoryIdentical(resultFrame, copyResultFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(resultMask, copyResultMask))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					// verifying and handling the mask

					unsigned int invalidMaskPixels = 0u;

					for (unsigned int y = 0u; y < resultFrame.height(); ++y)
					{
						const uint8_t* const maskRow = resultMask.constrow<uint8_t>(y);

						for (unsigned int x = 0u; x < resultFrame.width(); ++x)
						{
							if (maskRow[x] != maskValue)
							{
								memcpy(resultFrame.pixel<uint8_t>(x, y), objectFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * objectFrame.channels());

								const Scalar xObject = Scalar(x) * objectDimension.x() / Scalar(objectFrame.width());
								const Scalar zObject = Scalar(y) * objectDimension.y() / Scalar(objectFrame.height());

								const Vector2 projectedObjectPoint = camera.projectToImage(world_T_camera, Vector3(xObject, 0, zObject));

								const Scalar borderThreshold = lookup ? Scalar(3) : Scalar(1.5);

								if (camera.isInside(projectedObjectPoint, borderThreshold))
								{
									++invalidMaskPixels;
								}
							}
						}
					}

					const double averageError = determineFrameError(objectFrame, resultFrame, nullptr, nullptr);

					if (averageError > maximalAverageError)
					{
						maximalAverageError = averageError;
					}

					if (invalidMaskPixels > 2u)
					{
						maximalAverageError = 255.0;
					}
				}
				while (Timestamp(true) < startTimestamp + testDuration);
			}

			Log::info() << "Performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

			if (performanceMulticore.measurements() != 0u)
			{
				Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
				Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
			}

			Log::info() << " ";
		}
	}

	constexpr double threshold = 10.0;

	const bool allSucceeded = maximalAverageError < threshold;

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded (" << String::toAString(maximalAverageError, 1u) << " color error).";
	}
	else
	{
		Log::info() << "Validation: FAILED (" << String::toAString(maximalAverageError, 1u) << " color error)!";
	}

	return allSucceeded;
}

bool TestFrameRectification::testTriangleObjectMask(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	double maximalAverageError = 0.0;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	const IndexPairs32 objectResolutions = {IndexPair32(128u, 128u), IndexPair32(100u, 164u), IndexPair32(164u, 100u)};

	RandomGenerator randomGenerator;

	for (const bool lookup : {false, true})
	{
		if (lookup)
		{
			Log::info() << "Test lookup triangle object with mask " << width << "x" << height << ":";
		}
		else
		{
			Log::info() << "Test triangle object with mask " << width << "x" << height << ":";
		}

		Log::info() << " ";

		for (unsigned int channels = 1u; channels <= 4u; ++channels)
		{
			Log::info() << "... with " << channels << " channels:";

			const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

			HighPerformanceStatistic performanceSinglecore;
			HighPerformanceStatistic performanceMulticore;

			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
				HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

				const Timestamp startTimestamp(true);

				do
				{
					const IndexPair32 objectResolution = RandomI::random(randomGenerator, objectResolutions);

					Frame objectFrame = CV::CVUtilities::randomizedFrame(FrameType(objectResolution.first, objectResolution.second, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

					CV::FrameFilterGaussian::filter(objectFrame, 11u, &worker);

					Frame cameraFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

					const Frame copyCameraFrame(cameraFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					PinholeCamera pinholeCamera(cameraFrame.width(), cameraFrame.height(), Numeric::deg2rad(60));
					pinholeCamera.setRadialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.01), Random::scalar(randomGenerator, -1, 1) * Scalar(0.01)));
					pinholeCamera.setTangentialDistortion(PinholeCamera::DistortionPair(Random::scalar(randomGenerator, -1, 1) * Scalar(0.001), Random::scalar(randomGenerator, -1, 1) * Scalar(0.001)));

					const AnyCameraPinhole camera(pinholeCamera);

					const Vector2 objectDimension(1, Scalar(objectFrame.height()) / Scalar(objectFrame.width()));

					constexpr size_t numberTriangles = 2;

					const Triangle2 triangles2[numberTriangles] =
					{
						Triangle2(Vector2(0, 0), Vector2(Scalar(0), Scalar(objectFrame.height())), Vector2(Scalar(objectFrame.width()), Scalar(objectFrame.height()))),
						Triangle2(Vector2(0, 0), Vector2(Scalar(objectFrame.width()), Scalar(objectFrame.height())), Vector2(Scalar(objectFrame.width()), 0))
					};

					const Triangle3 triangles3[numberTriangles] =
					{
						Triangle3(Vector3(0, 0, 0), Vector3(0, 0, objectDimension.y()), Vector3(objectDimension.x(), 0, objectDimension.y())),
						Triangle3(Vector3(0, 0, 0), Vector3(objectDimension.x(), 0, objectDimension.y()), Vector3(objectDimension.x(), 0, 0))
					};

					const Quaternion world_R_camera = Quaternion(Vector3(1, 0, 0), -Numeric::pi_2()) * Quaternion(Euler(Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50)), Random::scalar(randomGenerator, Numeric::deg2rad(-50), Numeric::deg2rad(50))));

					const HomogenousMatrix4 world_T_camera = determineCameraPose(camera, Box3(Vector3(0, 0, 0), Vector3(objectDimension.x(), 0, objectDimension.y())), world_R_camera) * HomogenousMatrix4(Random::euler(randomGenerator, Numeric::deg2rad(20)));

					if (!renderPlanarRectangleObject(objectFrame, cameraFrame, camera, world_T_camera, objectDimension))
					{
						ocean_assert(false && "This should never happen!");
						maximalAverageError = NumericD::maxValue();
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(cameraFrame, copyCameraFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					Frame resultFrame = CV::CVUtilities::randomizedFrame(objectFrame.frameType(), &randomGenerator);

					const Frame copyResultFrame(resultFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					constexpr uint8_t maskValue = 0xFF;

					Frame resultMask = CV::CVUtilities::randomizedFrame(FrameType(objectFrame, FrameType::FORMAT_Y8), &randomGenerator);

					const Frame copyResultMask(resultMask, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

					performance.start();
						for (unsigned int n = 0; n < numberTriangles; ++n)
						{
							CV::Advanced::FrameRectification::Comfort::triangleObjectMask(cameraFrame, camera, world_T_camera, triangles2[n], triangles3[n], resultFrame, resultMask, useWorker, maskValue, lookup ? 20u : 0u);
						}
					performance.stop();

					if (!CV::CVUtilities::isPaddingMemoryIdentical(resultFrame, copyResultFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					if (!CV::CVUtilities::isPaddingMemoryIdentical(resultMask, copyResultMask))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}

					// verifying and handling the mask

					unsigned int invalidMaskPixels = 0u;

					for (unsigned int y = 0u; y < resultFrame.height(); ++y)
					{
						const uint8_t* const maskRow = resultMask.constrow<uint8_t>(y);

						for (unsigned int x = 0u; x < resultFrame.width(); ++x)
						{
							if (maskRow[x] != maskValue)
							{
								memcpy(resultFrame.pixel<uint8_t>(x, y), objectFrame.constpixel<uint8_t>(x, y), sizeof(uint8_t) * objectFrame.channels());

								const Scalar xObject = Scalar(x) * objectDimension.x() / Scalar(objectFrame.width());
								const Scalar zObject = Scalar(y) * objectDimension.y() / Scalar(objectFrame.height());

								const Vector2 projectedObjectPoint = camera.projectToImage(world_T_camera, Vector3(xObject, 0, zObject));

								const Scalar borderThreshold = lookup ? Scalar(3) : Scalar(1.5);

								if (camera.isInside(projectedObjectPoint, borderThreshold))
								{
									++invalidMaskPixels;
								}
							}
						}
					}

					const double averageError = determineFrameError(objectFrame, resultFrame, nullptr, nullptr);

					if (averageError > maximalAverageError)
					{
						maximalAverageError = averageError;
					}

					if (invalidMaskPixels > 2u)
					{
						maximalAverageError = 255.0;
					}
				}
				while (Timestamp(true) < startTimestamp + testDuration);
			}

			Log::info() << "Performance: Best: " << performanceSinglecore.bestMseconds() << "ms, worst: " << performanceSinglecore.worstMseconds() << "ms, average: " << performanceSinglecore.averageMseconds() << "ms";

			if (performanceMulticore.measurements() != 0u)
			{
				Log::info() << "Multicore performance: Best: " << performanceMulticore.bestMseconds() << "ms, worst: " << performanceMulticore.worstMseconds() << "ms, average: " << performanceMulticore.averageMseconds() << "ms";
				Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
			}

			Log::info() << " ";
		}
	}

	constexpr double threshold = 10.0;

	const bool allSucceeded = maximalAverageError < threshold;

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded (" << String::toAString(maximalAverageError, 1u) << " color error).";
	}
	else
	{
		Log::info() << "Validation: FAILED (" << String::toAString(maximalAverageError, 1u) << " color error)!";
	}

	return allSucceeded;
}

double TestFrameRectification::determineFrameError(const Frame& frame0, const Frame& frame1, const uint8_t* skipColor0, const uint8_t* skipColor1)
{
	ocean_assert(frame0.isValid() && frame1.isValid());
	ocean_assert(frame0.frameType() == frame1.frameType());

	ocean_assert(frame0.numberPlanes() == 1u);
	ocean_assert(frame0.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	if (!frame0.isValid() || !frame1.isValid() || frame0.frameType() != frame1.frameType())
	{
		return NumericD::maxValue();
	}

	const unsigned int channels = FrameType::channels(frame0.pixelFormat());

	unsigned int totalDifference = 0u;
	unsigned int values = 0u;

	for (unsigned int y = 2u; y < frame0.height() - 2u; ++y)
	{
		const uint8_t* row0 = frame0.constrow<uint8_t>(y) - channels;
		const uint8_t* row1 = frame1.constrow<uint8_t>(y) - channels;

		for (unsigned int x = 2u; x < frame0.width() - 2u; ++x)
		{
			row0 += channels;
			row1 += channels;

			if (skipColor0 != nullptr)
			{
				if (memcmp(row0, skipColor0, sizeof(uint8_t) * channels) == 0)
				{
					continue;
				}
			}

			if (skipColor1 != nullptr)
			{
				if (memcmp(row1, skipColor1, sizeof(uint8_t) * channels) == 0)
				{
					continue;
				}
			}

			for (unsigned int n = 0u; n < channels; ++n)
			{
				totalDifference += abs(row0[n] - row1[n]);
			}

			values += channels;
		}
	}

	ocean_assert(values != 0u);

	// we expect that at least 50% of all pixels have been checked
	ocean_assert(values * 2u >= frame0.pixels() * channels);
	if (values * 2u < frame0.pixels() * channels)
	{
		return Numeric::maxValue();
	}

	return double(totalDifference) / double(values);
}

bool TestFrameRectification::renderPlanarRectangleObject(const Frame& objectFrame, Frame& targetFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector2& objectDimension)
{
	ocean_assert(objectFrame && targetFrame);
	ocean_assert(camera.isValid() && world_T_camera.isValid());

	ocean_assert(objectFrame.pixelFormat() == targetFrame.pixelFormat() && objectFrame.pixelOrigin() == targetFrame.pixelOrigin());
	ocean_assert(Numeric::isNotEqualEps(objectDimension.x()) && Numeric::isNotEqualEps(objectDimension.y()));

	// xz-plane
	const Plane3 plane(Vector3(0, 0, 0), Vector3(0, 1, 0));
	const Box2 box2(Vector2(0, 0), objectDimension);

	const unsigned int channels = FrameType::channels(objectFrame.pixelFormat());

	for (unsigned int y = 0u; y < targetFrame.height(); ++y)
	{
		uint8_t* targetRow = targetFrame.row<uint8_t>(y);

		for (unsigned int x = 0u; x < targetFrame.width(); ++x)
		{
			const Vector2 targetPoint = Vector2(Scalar(x), Scalar(y));

			const Line3 ray(camera.ray(targetPoint, world_T_camera));

			Vector3 intersectionPoint;
			if (plane.intersection(ray, intersectionPoint))
			{
				const Vector2 point2(intersectionPoint.x(), intersectionPoint.z());

				if (box2.isInside(point2))
				{
					const Scalar objectX = point2.x() * Scalar(objectFrame.width()) / objectDimension.x();
					const Scalar objectY = point2.y() * Scalar(objectFrame.height()) / objectDimension.y();

					if (objectX <= Scalar(objectFrame.width() - 1u) && objectY <= Scalar(objectFrame.height() - 1u))
					{
						ocean_assert(objectX >= 0 && objectX <= Scalar(objectFrame.width() - 1u) && objectY >= 0 && objectY <= Scalar(objectFrame.height() - 1u));

						switch (channels)
						{
							case 1u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<1u, CV::PC_TOP_LEFT>(objectFrame.constdata<uint8_t>(), objectFrame.width(), objectFrame.height(), objectFrame.paddingElements(), Vector2(objectX, objectY), targetRow);
								break;

							case 2u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<2u, CV::PC_TOP_LEFT>(objectFrame.constdata<uint8_t>(), objectFrame.width(), objectFrame.height(), objectFrame.paddingElements(), Vector2(objectX, objectY), targetRow);
								break;

							case 3u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(objectFrame.constdata<uint8_t>(), objectFrame.width(), objectFrame.height(), objectFrame.paddingElements(), Vector2(objectX, objectY), targetRow);
								break;

							case 4u:
								CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<4u, CV::PC_TOP_LEFT>(objectFrame.constdata<uint8_t>(), objectFrame.width(), objectFrame.height(), objectFrame.paddingElements(), Vector2(objectX, objectY), targetRow);
								break;
						}
					}
				}
			}

			targetRow += channels;
		}
	}

	return true;
}

HomogenousMatrix4 TestFrameRectification::determineCameraPose(const AnyCamera& camera, const Box3& boundingBox, const Quaternion& world_R_camera)
{
	ocean_assert(camera.isValid() && boundingBox.isValid() && world_R_camera.isValid());

	const Sphere3 sphere(boundingBox);

	const Scalar fov = min(camera.fovX(), camera.fovY());

	ocean_assert(fov >= 0);
	if (fov <= Numeric::eps())
	{
		ocean_assert(false && "This should never happen!");
		return HomogenousMatrix4(false);
	}

	const Scalar viewDistance = sphere.radius() / Numeric::sin(fov * Scalar(0.5));
	const Vector3 viewDirection(world_R_camera * Vector3(0, 0, -1));
	ocean_assert(viewDirection.isUnit());

	const Vector3 viewPosition = sphere.center() - viewDirection * viewDistance;

	const HomogenousMatrix4 world_T_camera(viewPosition, world_R_camera);

#ifdef OCEAN_DEBUG
	Vector3 corners[8];
	const unsigned int numberCorners = boundingBox.corners(corners);
	ocean_assert(numberCorners <= 8u);

	for (unsigned int n = 0u; n < numberCorners; ++n)
	{
		const Vector2 projectedCorner = camera.projectToImage(world_T_camera, corners[n]);

		constexpr Scalar borderThreshold = Scalar(-2);
		ocean_assert(camera.isInside(projectedCorner, borderThreshold));
	}
#endif // OCEAN_DEBUG

	return world_T_camera;
}

}

}

}

}
