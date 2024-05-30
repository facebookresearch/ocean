/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestP3P.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/DataType.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/P3P.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/math/Euler.h"
#include "ocean/math/Line3.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Vector3.h"

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestP3P::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   P3P test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testP3PWithPointsPinholeCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testP3PWithPoints<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testP3PWithPoints<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testP3PWithRays<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testP3PWithRays<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testP3PWithPointsStressTest<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testP3PWithPointsStressTest<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testP3PWithRaysStressTest<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testP3PWithRaysStressTest<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "P3P test succeeded.";
	}
	else
	{
		Log::info() << "P3P test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestP3P, P3PWithPointsPinholeCamera)
{
	EXPECT_TRUE(TestP3P::testP3PWithPointsPinholeCamera(GTEST_TEST_DURATION));
}


TEST(TestP3P, P3PWithPoints_float)
{
	EXPECT_TRUE(TestP3P::testP3PWithPoints<float>(GTEST_TEST_DURATION));
}

TEST(TestP3P, P3PWithPoints_double)
{
	EXPECT_TRUE(TestP3P::testP3PWithPoints<double>(GTEST_TEST_DURATION));
}


TEST(TestP3P, P3PWithRays_float)
{
	EXPECT_TRUE(TestP3P::testP3PWithRays<float>(GTEST_TEST_DURATION));
}

TEST(TestP3P, P3PWithRays_double)
{
	EXPECT_TRUE(TestP3P::testP3PWithRays<double>(GTEST_TEST_DURATION));
}


TEST(TestP3P, P3PWithPointsStressTest_float)
{
	EXPECT_TRUE(TestP3P::testP3PWithPointsStressTest<float>(GTEST_TEST_DURATION));
}

TEST(TestP3P, P3PWithPointsStressTest_double)
{
	EXPECT_TRUE(TestP3P::testP3PWithPointsStressTest<double>(GTEST_TEST_DURATION));
}


TEST(TestP3P, P3PWithRaysStressTest_float)
{
	EXPECT_TRUE(TestP3P::testP3PWithRaysStressTest<float>(GTEST_TEST_DURATION));
}

TEST(TestP3P, P3PWithRaysStressTest_double)
{
	EXPECT_TRUE(TestP3P::testP3PWithRaysStressTest<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestP3P::testP3PWithPointsPinholeCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing P3P for 2D image points and pinhole camera:";

	bool allSucceeded = true;

	Vectors3 objectPoints(3);
	Vectors2 undistortedImagePoints(3);
	Vectors2 distortedImagePoints(3);

	HighPerformanceStatistic performance;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);
	do
	{
		// first, we create a random camera profile

		const Scalar aspectRatio = Random::scalar(Scalar(4.0 / 3.0), Scalar(16.0 / 9.0));
		ocean_assert(aspectRatio > Numeric::eps());

		const unsigned int width = RandomI::random(640u, 1920u);
		const unsigned int height = (unsigned int)(Scalar(width) / aspectRatio + Scalar(0.5));

		const Scalar fovX = Random::scalar(Numeric::deg2rad(35), Numeric::deg2rad(75));

		const Scalar focalLength = PinholeCamera::fieldOfViewToFocalLength(width, fovX);

		PinholeCamera::DistortionPair radialDistortion = {Random::scalar(Scalar(-0.05), Scalar(0.05)), Random::scalar(Scalar(-0.05), Scalar(0.05))};
		PinholeCamera::DistortionPair tangentialDistortion = {Random::scalar(Scalar(-0.001), Scalar(0.001)), Random::scalar(Scalar(-0.001), Scalar(0.001))};

		const Scalar principalPointX = Scalar(width) * Random::scalar(Scalar(0.4), Scalar(0.6));
		const Scalar principalPointY = Scalar(height) * Random::scalar(Scalar(0.4), Scalar(0.6));

		const PinholeCamera pinholeCamera(width, height, focalLength, focalLength, principalPointX, principalPointY, radialDistortion, tangentialDistortion);

		// determine random points inside a small 3D area

		objectPoints[0] = Vector3(Random::scalar(-1, 1), Random::scalar(Scalar(-0.1), Scalar(0.1)), Random::scalar(-1, 1));
		objectPoints[1] = Vector3(Random::scalar(-1, 1), Random::scalar(Scalar(-0.1), Scalar(0.1)), Random::scalar(-1, 1));

		while (objectPoints[0].distance(objectPoints[1]) < Scalar(0.01))
		{
			objectPoints[1] = Vector3(Random::scalar(-1, 1), Random::scalar(Scalar(-0.1), Scalar(0.1)), Random::scalar(-1, 1));
		}

		objectPoints[2] = Vector3(Random::scalar(-1, 1), Random::scalar(Scalar(-0.1), Scalar(0.1)), Random::scalar(-1, 1));

		while (Line3(objectPoints[0], (objectPoints[1] - objectPoints[0]).normalized()).distance(objectPoints[2]) < Scalar(0.01))
		{
			objectPoints[2] = Vector3(Random::scalar(-1, 1), Random::scalar(Scalar(-0.1), Scalar(0.1)), Random::scalar(-1, 1));
		}

		const Euler euler(Random::euler(Numeric::deg2rad(0), Numeric::deg2rad(30)));
		const Quaternion quaternion(euler);

		const HomogenousMatrix4 perfectPose(Utilities::viewPosition(pinholeCamera, objectPoints, quaternion * Vector3(0, -1, 0)));

		// determine the perfectly projected image points

		pinholeCamera.projectToImage<true>(perfectPose, objectPoints.data(), 3, false, undistortedImagePoints.data());
		pinholeCamera.projectToImage<true>(perfectPose, objectPoints.data(), 3, true, distortedImagePoints.data());

		bool imagePointsColinear = false;
		for (unsigned int n = 0u; n < 3u; ++n)
		{
			ocean_assert(pinholeCamera.isInside(undistortedImagePoints[n]));
			ocean_assert(pinholeCamera.isInside(distortedImagePoints[n]));

			const unsigned int n1 = (n + 1u) % 3u;
			const unsigned int n2 = (n + 2u) % 3u;

			const Line2 line(undistortedImagePoints[n], (undistortedImagePoints[n1] - undistortedImagePoints[n]).normalized());

			if (line.distance(undistortedImagePoints[n2]) < 5)
			{
				imagePointsColinear = true;
			}
		}

		if (imagePointsColinear)
			continue;

		HomogenousMatrix4 poses[4];

		performance.start();
		const unsigned int numberPoses = Geometry::P3P::poses(pinholeCamera, objectPoints.data(), undistortedImagePoints.data(), poses);

		if (numberPoses != 0u)
		{
			performance.stop();

			bool localProjectionAccurate = true;
			bool localPoseAccurate = std::is_same<Scalar, float>::value ? true : false; // we apply the test for 64 bit floating point values only

			for (unsigned int n = 0u; n < numberPoses; ++n)
			{
				Scalar maximalError = 0;
				for (unsigned int i = 0u; i < 3u; ++i)
				{
					maximalError = max(maximalError, distortedImagePoints[i].distance(pinholeCamera.projectToImage<true>(poses[n], objectPoints[i], pinholeCamera.hasDistortionParameters())));
				}

				const Scalar pixelErrorThreshold = std::is_same<Scalar, double>::value ? Scalar(0.9) : Scalar(5);

				if (maximalError >= pixelErrorThreshold)
				{
					localProjectionAccurate = false;
				}

				if (std::is_same<Scalar, double>::value)
				{
					const Scalar translationError = perfectPose.translation().distance(poses[n].translation());
					const Euler rotationError(perfectPose * poses[n].inverted());

					const Scalar absYawDeg = Numeric::rad2deg(Numeric::abs(rotationError.yaw()));
					const Scalar absPitchDeg = Numeric::rad2deg(Numeric::abs(rotationError.pitch()));
					const Scalar absRollDeg = Numeric::rad2deg(Numeric::abs(rotationError.roll()));

					if (translationError < Scalar(0.005) && absYawDeg < Scalar(0.01) && absPitchDeg < Scalar(0.01) && absRollDeg < Scalar(0.01))
					{
						localPoseAccurate = true;
					}
				}
			}

			if (localProjectionAccurate && localPoseAccurate)
			{
				++validIterations;
			}
		}
		else
		{
			performance.skip();
		}

		++iterations;
	}
	while (iterations == 0u || startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 4u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 4u) << "ms, average: " << String::toAString(performance.averageMseconds(), 4u) << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const double threshold = std::is_same<Scalar, float>::value ? 0.75 : 0.95;

	if (percent < threshold)
	{
		allSucceeded = false;
	}

	return allSucceeded;
}

template <typename T>
bool TestP3P::testP3PWithPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing P3P for 2D image points for '" << TypeNamer::name<T>() << "':";
	Log::info() << " ";

	bool allSucceeded = true;

	constexpr double successThreshold = std::is_same<T, float>::value ? 0.75 : 0.95;

	RandomGenerator randomGenerator;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const SharedAnyCameraT<T> anyCamera(Utilities::realisticAnyCamera<T>(anyCameraType, RandomI::random(1u)));
		ocean_assert(anyCamera);

		ValidationPrecision validation(successThreshold, randomGenerator);

		HighPerformanceStatistic performance;

		const Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const VectorT3<T> translation = RandomT<T>::vector3(-5, 5);
			const QuaternionT<T> rotation = RandomT<T>::quaternion();
			const HomogenousMatrixT4<T> world_T_camera(translation, rotation);

			VectorsT3<T> objectPoints(3);
			VectorsT2<T> imagePoints(3);

			while (true)
			{
				for (unsigned int n = 0u; n < 3u; ++n)
				{
					constexpr T cameraBorder = T(5);

					imagePoints[n] = RandomT<T>::vector2(randomGenerator, cameraBorder, T(anyCamera->width()) - cameraBorder, cameraBorder, T(anyCamera->height()) - cameraBorder);
					objectPoints[n] = anyCamera->ray(imagePoints[n], world_T_camera).point(RandomT<T>::scalar(randomGenerator, T(0.5), 5));
				}

				if (imagePoints[0].isEqual(imagePoints[1], 5) || imagePoints[0].isEqual(imagePoints[2], 5) || imagePoints[1].isEqual(imagePoints[2], 5))
				{
					continue;
				}

				const PlaneT3<T> plane(world_T_camera.translation(), objectPoints[0], objectPoints[1]);
				ocean_assert(plane.isValid());

				if (NumericT<T>::abs(plane.signedDistance(objectPoints[2])) <= T(0.25))
				{
					continue;
				}

				break;
			}

			HomogenousMatrixT4<T> world_T_cameraCandidates[4];

			performance.start();
				const unsigned int numberPoses = Geometry::P3P::poses(*anyCamera, objectPoints.data(), imagePoints.data(), world_T_cameraCandidates);
			performance.stop();

			if (numberPoses != 0u)
			{
				// one of the resulting poses must match our random pose
				bool onePoseIsAccuate = false;

				for (unsigned int n = 0u; n < numberPoses; ++n)
				{
					const HomogenousMatrixT4<T>& world_T_cameraCandidate = world_T_cameraCandidates[n];

					const HomogenousMatrixT4<T> flippedCameraCandidate_T_world(AnyCamera::standard2InvertedFlipped(world_T_cameraCandidate));

					// all object points must lie in front of the candidate camera
					for (const VectorT3<T>& objectPoint : objectPoints)
					{
						if (!AnyCameraT<T>::isObjectPointInFrontIF(flippedCameraCandidate_T_world, objectPoint))
						{
							OCEAN_SET_FAILED(validation);
						}
					}

					T maximalError = 0;
					for (unsigned int i = 0u; i < 3u; ++i)
					{
						maximalError = max(maximalError, imagePoints[i].distance(anyCamera->projectToImage(world_T_cameraCandidate, objectPoints[i])));
					}

					const T pixelErrorThreshold = std::is_same<T, double>::value ? T(0.9) : T(5);

					if (maximalError >= pixelErrorThreshold)
					{
						scopedIteration.setInaccurate();
					}

					if (std::is_same<T, double>::value)
					{
						const T translationError = world_T_camera.translation().distance(world_T_cameraCandidate.translation());
						const EulerT<T> rotationError(world_T_camera * world_T_cameraCandidate.inverted());

						const T absYawDeg = NumericT<T>::rad2deg(NumericT<T>::abs(rotationError.yaw()));
						const T absPitchDeg = NumericT<T>::rad2deg(NumericT<T>::abs(rotationError.pitch()));
						const T absRollDeg = NumericT<T>::rad2deg(NumericT<T>::abs(rotationError.roll()));

						if (translationError <= T(0.005) && absYawDeg <= T(0.01) && absPitchDeg <= T(0.01) && absRollDeg <= T(0.01))
						{
							onePoseIsAccuate = true;
						}
					}
				}

				if (std::is_same<T, double>::value)
				{
					if (!onePoseIsAccuate)
					{
						scopedIteration.setInaccurate();
					}
				}
			}
			else
			{
				scopedIteration.setInaccurate();
			}
		}
		while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

		Log::info() << anyCamera->name() << ":";

		Log::info() << "Performance: " << performance;
		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

template <typename T>
bool TestP3P::testP3PWithRays(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing P3P for 3D rays for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;

	constexpr double successThreshold = std::is_same<T, float>::value ? 0.75 : 0.95;
	ValidationPrecision validation(successThreshold, randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		// first, we create a random camera profile

		const T aspectRatio = RandomT<T>::scalar(randomGenerator, T(4.0 / 3.0), T(16.0 / 9.0));
		ocean_assert(aspectRatio > NumericT<T>::eps());

		const unsigned int width = RandomI::random(randomGenerator, 640u, 1920u);
		const unsigned int height = (unsigned int)(T(width) / aspectRatio + T(0.5));

		const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(35), NumericT<T>::deg2rad(75));

		const T focalLength = PinholeCameraT<T>::fieldOfViewToFocalLength(width, fovX);

		const T radialDistortion0 = RandomT<T>::scalar(randomGenerator, T(-0.05), T(0.05));
		const T radialDistortion1 = RandomT<T>::scalar(randomGenerator, T(-0.05), T(0.05));
		const typename PinholeCameraT<T>::DistortionPair radialDistortion = {radialDistortion0, radialDistortion1};

		const T tangentialDistortion0 = RandomT<T>::scalar(randomGenerator, T(-0.001), T(0.001));
		const T tangentialDistortion1 = RandomT<T>::scalar(randomGenerator, T(-0.001), T(0.001));
		const typename PinholeCameraT<T>::DistortionPair tangentialDistortion = {tangentialDistortion0, tangentialDistortion1};

		const T principalPointX = T(width) * RandomT<T>::scalar(randomGenerator, T(0.4), T(0.6));
		const T principalPointY = T(height) * RandomT<T>::scalar(randomGenerator, T(0.4), T(0.6));

		const PinholeCameraT<T> pinholeCamera(width, height, focalLength, focalLength, principalPointX, principalPointY, radialDistortion, tangentialDistortion);

		// determine random points inside a small 3D area

		VectorsT3<T> objectPoints(3);

		objectPoints[0] = RandomT<T>::vector3(randomGenerator, VectorT3<T>(T(1), T(0.1), T(1)));
		objectPoints[1] = RandomT<T>::vector3(randomGenerator, VectorT3<T>(T(1), T(0.1), T(1)));

		while (objectPoints[0].distance(objectPoints[1]) < T(0.01))
		{
			objectPoints[1] = RandomT<T>::vector3(randomGenerator, VectorT3<T>(T(1), T(0.1), T(1)));
		}

		objectPoints[2] = RandomT<T>::vector3(randomGenerator, VectorT3<T>(T(1), T(0.1), T(1)));

		while (LineT3<T>(objectPoints[0], (objectPoints[1] - objectPoints[0]).normalized()).distance(objectPoints[2]) < T(0.01))
		{
			objectPoints[2] = RandomT<T>::vector3(randomGenerator, VectorT3<T>(T(1), T(0.1), T(1)));
		}

		const EulerT<T> euler(RandomT<T>::euler(randomGenerator, NumericT<T>::deg2rad(0), NumericT<T>::deg2rad(30)));
		const QuaternionT<T> quaternion(euler);

		// transformation transforming 3D points defined in the coordinate system of the camera to 3D points defined in the world coordinate system
		const HomogenousMatrixT4<T> world_T_camera = Utilities::viewPosition(AnyCameraPinholeT<T>(pinholeCamera), objectPoints, quaternion * VectorT3<T>(0, -1, 0));

		VectorsT2<T> undistortedImagePoints(3);
		VectorsT2<T> distortedImagePoints(3);

		pinholeCamera.template projectToImage<true>(world_T_camera, objectPoints.data(), 3, false, undistortedImagePoints.data());
		pinholeCamera.template projectToImage<true>(world_T_camera, objectPoints.data(), 3, true, distortedImagePoints.data());

		bool imagePointsColinear = false;
		for (unsigned int n = 0u; n < 3u; ++n)
		{
			const unsigned int n1 = (n + 1u) % 3u;
			const unsigned int n2 = (n + 2u) % 3u;

			const LineT2<T> line(undistortedImagePoints[n], (undistortedImagePoints[n1] - undistortedImagePoints[n]).normalized());

			if (line.distance(undistortedImagePoints[n2]) < 5)
			{
				imagePointsColinear = true;
			}
		}

		if (imagePointsColinear)
		{
			continue;
		}

		// transformation transforming 3D points defined in the world coordinate system to 3D points defined in the coordinate system of the camera
		HomogenousMatrixT4<T> camera_T_world;
		if (!world_T_camera.invert(camera_T_world))
		{
			ocean_assert(false && "This must never happen!");
			continue;
		}

		// we determine the 3D rays starting at the camera's center of projecting and pointing towards the 3D object points
		// the rays are defined in the coordinate system of the camera (and not in the coordinate system of the 3D object points)

		const VectorT3<T> rays[3] =
		{
			(camera_T_world * objectPoints[0]).normalizedOrZero(),
			(camera_T_world * objectPoints[1]).normalizedOrZero(),
			(camera_T_world * objectPoints[2]).normalizedOrZero()
		};

		ocean_assert(rays[0].isUnit() && rays[1].isUnit() && rays[2].isUnit());

		HomogenousMatrixT4<T> world_T_cameraCandidates[4];

		performance.start();
			const unsigned int numberPoses = Geometry::P3P::poses(objectPoints.data(), rays, world_T_cameraCandidates);
		performance.stop();

		if (numberPoses != 0u)
		{
			// one of the resulting poses must match our random pose
			bool onePoseIsAccuate = false;

			for (unsigned int n = 0u; n < numberPoses; ++n)
			{
				const HomogenousMatrixT4<T>& world_T_cameraCandidate = world_T_cameraCandidates[n];

				T maximalError = 0;
				for (unsigned int i = 0u; i < 3u; ++i)
				{
					maximalError = max(maximalError, distortedImagePoints[i].distance(pinholeCamera.template projectToImage<true>(world_T_cameraCandidate, objectPoints[i], pinholeCamera.hasDistortionParameters())));
				}

				const T pixelErrorThreshold = std::is_same<T, double>::value ? T(0.9) : T(5);

				if (maximalError >= pixelErrorThreshold)
				{
					scopedIteration.setInaccurate();
				}

				if (std::is_same<T, double>::value)
				{
					const T translationError = world_T_camera.translation().distance(world_T_cameraCandidate.translation());
					const EulerT<T> rotationError(world_T_camera * world_T_cameraCandidate.inverted());

					const T absYawDeg = NumericT<T>::rad2deg(NumericT<T>::abs(rotationError.yaw()));
					const T absPitchDeg = NumericT<T>::rad2deg(NumericT<T>::abs(rotationError.pitch()));
					const T absRollDeg = NumericT<T>::rad2deg(NumericT<T>::abs(rotationError.roll()));

					if (translationError <= T(0.005) && absYawDeg <= T(0.01) && absPitchDeg <= T(0.01) && absRollDeg <= T(0.01))
					{
						onePoseIsAccuate = true;
					}
				}
			}

			if (std::is_same<T, double>::value)
			{
				if (!onePoseIsAccuate)
				{
					scopedIteration.setInaccurate();
				}
			}
		}
		else
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestP3P::testP3PWithPointsStressTest(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress testing P3P for 3D points for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// we create arbitrary 3D object points within the bounding box [-10000, 10000]
		const VectorT3<T> objectPoints[3] =
		{
			randomVector<T>(randomGenerator),
			randomVector<T>(randomGenerator),
			randomVector<T>(randomGenerator)
		};

		for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
		{
			const SharedAnyCameraT<T> camera = Utilities::realisticAnyCamera<T>(anyCameraType, RandomI::random(randomGenerator, 1u));

			const VectorT2<T> imagePoints[3] =
			{
				RandomT<T>::vector2(randomGenerator, T(0), T(camera->width()), T(0), T(camera->height())),
				RandomT<T>::vector2(randomGenerator, T(0), T(camera->width()), T(0), T(camera->height())),
				RandomT<T>::vector2(randomGenerator, T(0), T(camera->width()), T(0), T(camera->height()))
			};

			// we do not evaluate the resulting poses, we just want to ensure that the function does not crash

			HomogenousMatrixT4<T> world_T_cameras[4];
			const unsigned int numberPoses = Geometry::P3P::poses<T>(*camera, objectPoints, imagePoints, world_T_cameras);

			for (unsigned int n = 0u; n < numberPoses; ++n)
			{
				const HomogenousMatrixT4<T> flippedCamera_T_world(AnyCameraT<T>::standard2InvertedFlipped(world_T_cameras[n]));

				for (unsigned int o = 0u; o < 3u; ++o)
				{
					if (!AnyCameraT<T>::isObjectPointInFrontIF(flippedCamera_T_world, objectPoints[o]))
					{
						OCEAN_SET_FAILED(validation);
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestP3P::testP3PWithRaysStressTest(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress testing P3P for 3D rays for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;

	uint64_t dummyValue = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		// we create arbitrary 3D object points within the bounding box [-10000, 10000]
		const VectorT3<T> objectPoints[] =
		{
			randomVector<T>(randomGenerator),
			randomVector<T>(randomGenerator),
			randomVector<T>(randomGenerator),
		};

		// now we create random 3D ray with unit length
		// each ray must point towards the negative z space

		VectorT3<T> rays[3];

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			do
			{
				rays[n] = randomVector<T>(randomGenerator).normalizedOrZero();
			}
			while (rays[n].isNull());

			if (rays[n].z() >= Scalar(0))
			{
				rays[n].z() = -rays[n].z();
			}
		}

		// we do not evaluate the resulting poses, we just want to ensure that the function does not crash

		HomogenousMatrixT4<T> world_T_cameras[4];
		const unsigned int numberPoses = Geometry::P3P::poses<T>(objectPoints, rays, world_T_cameras);
		dummyValue += numberPoses;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (dummyValue % 2ull == 0ull)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: succeeded.";
	}

	// we return True in any case
	return (dummyValue % 2ull == 0ull) || (dummyValue % 2ull == 1ull);
}

template <typename T>
VectorT3<T> TestP3P::randomVector(RandomGenerator& randomGenerator)
{
	VectorT3<T> vector = RandomT<T>::vector3(randomGenerator, VectorT3<T>(1, 1, 1));

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		vector *= RandomT<T>::scalar(randomGenerator, -10, 10);
	}

	return vector;
}

}

}

}
