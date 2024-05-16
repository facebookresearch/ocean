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

	allSucceeded = testP3PWithPointsFisheyeCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testP3PWithPointsAnyCamera(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testP3PWithRays(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testP3PWithRaysStressTest<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
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

TEST(TestP3P, P3PWithPointsFisheyeCamera)
{
	EXPECT_TRUE(TestP3P::testP3PWithPointsFisheyeCamera(GTEST_TEST_DURATION));
}

TEST(TestP3P, P3PWithPointsAnyCamera)
{
	EXPECT_TRUE(TestP3P::testP3PWithPointsAnyCamera(GTEST_TEST_DURATION));
}

TEST(TestP3P, P3PWithRays)
{
	EXPECT_TRUE(TestP3P::testP3PWithRays(GTEST_TEST_DURATION));
}

TEST(TestP3P, P3PWithRaysStressTestFloat)
{
	EXPECT_TRUE(TestP3P::testP3PWithRaysStressTest<float>(GTEST_TEST_DURATION));
}

TEST(TestP3P, P3PWithRaysStressTestDouble)
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

bool TestP3P::testP3PWithPointsFisheyeCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing P3P for 2D image points and fisheye camera:";

	const unsigned int width = 640u;
	const unsigned int height = 480u;

	const Scalar focalLength = Scalar(191.787);

	const Scalar principalX = Scalar(314.304);
	const Scalar principalY = Scalar(243.711);

	const Scalars radialDistortions = {Scalar(0.29192), Scalar(0.00329052), Scalar(-0.151158), Scalar(0.0952214), Scalar(-0.0230753), Scalar(0.00194645)};
	const Scalars tangentialDistortions = {Scalar(0.0003358), Scalar(-0.000474032)};

	const FisheyeCamera fisheyeCamera(width, height,  focalLength, focalLength, principalX, principalY, radialDistortions.data(), tangentialDistortions.data());

	bool allSucceeded = true;

	Vectors3 objectPoints(3);
	Vectors2 imagePoints(3);

	HighPerformanceStatistic performance;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);
	do
	{
		const HomogenousMatrix4 world_T_camera(Random::vector3(-5, 5), Random::quaternion());

		while (true)
		{
			for (unsigned int n = 0u; n < 3u; ++n)
			{
				imagePoints[n] = Random::vector2(Scalar(5), Scalar(fisheyeCamera.width() - 5u), Scalar(5), Scalar(fisheyeCamera.height() - 5u));
				objectPoints[n] = fisheyeCamera.ray(imagePoints[n], world_T_camera).point(Random::scalar(Scalar(0.5), 5));
			}

			if (imagePoints[0].isEqual(imagePoints[1], 5) || imagePoints[0].isEqual(imagePoints[2], 5) || imagePoints[1].isEqual(imagePoints[2], 5))
			{
				continue;
			}

			const Plane3 plane(world_T_camera.translation(), objectPoints[0], objectPoints[1]);
			ocean_assert(plane);

			if (Numeric::abs(plane.signedDistance(objectPoints[2])) <= Scalar(0.25))
			{
				continue;
			}

			break;
		}

		HomogenousMatrix4 poses[4];

		performance.start();
		const unsigned int numberPoses = Geometry::P3P::poses(fisheyeCamera, objectPoints.data(), imagePoints.data(), poses);

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
					maximalError = max(maximalError, imagePoints[i].distance(fisheyeCamera.projectToImage(poses[n], objectPoints[i])));
				}

				const Scalar pixelErrorThreshold = std::is_same<Scalar, double>::value ? Scalar(0.9) : Scalar(5);

				if (maximalError >= pixelErrorThreshold)
				{
					localProjectionAccurate = false;
				}

				if (std::is_same<Scalar, double>::value)
				{
					const Scalar translationError = world_T_camera.translation().distance(poses[n].translation());
					const Euler rotationError(world_T_camera * poses[n].inverted());

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

bool TestP3P::testP3PWithPointsAnyCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing P3P for 2D image points and AnyCamera:";
	Log::info() << " ";

	bool allSucceeded = true;

	Vectors3 objectPoints(3);
	Vectors2 imagePoints(3);

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		const std::shared_ptr<AnyCamera> anyCamera(Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u)));
		ocean_assert(anyCamera);

		HighPerformanceStatistic performance;

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		const Timestamp startTimestamp(true);

		do
		{
			const HomogenousMatrix4 world_T_camera(Random::vector3(-5, 5), Random::quaternion());

			while (true)
			{
				for (unsigned int n = 0u; n < 3u; ++n)
				{
					imagePoints[n] = Random::vector2(Scalar(5), Scalar(anyCamera->width() - 5u), Scalar(5), Scalar(anyCamera->height() - 5u));
					objectPoints[n] = anyCamera->ray(imagePoints[n], world_T_camera).point(Random::scalar(Scalar(0.5), 5));
				}

				if (imagePoints[0].isEqual(imagePoints[1], 5) || imagePoints[0].isEqual(imagePoints[2], 5) || imagePoints[1].isEqual(imagePoints[2], 5))
				{
					continue;
				}

				const Plane3 plane(world_T_camera.translation(), objectPoints[0], objectPoints[1]);
				ocean_assert(plane);

				if (Numeric::abs(plane.signedDistance(objectPoints[2])) <= Scalar(0.25))
				{
					continue;
				}

				break;
			}

			HomogenousMatrix4 poses[4];

			performance.start();
			const unsigned int numberPoses = Geometry::P3P::poses(*anyCamera, objectPoints.data(), imagePoints.data(), poses);

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
						maximalError = max(maximalError, imagePoints[i].distance(anyCamera->projectToImage(poses[n], objectPoints[i])));
					}

					const Scalar pixelErrorThreshold = std::is_same<Scalar, double>::value ? Scalar(0.9) : Scalar(5);

					if (maximalError >= pixelErrorThreshold)
					{
						localProjectionAccurate = false;
					}

					if (std::is_same<Scalar, double>::value)
					{
						const Scalar translationError = world_T_camera.translation().distance(poses[n].translation());
						const Euler rotationError(world_T_camera * poses[n].inverted());

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
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << anyCamera->name() << ":";

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 4u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 4u) << "ms, average: " << String::toAString(performance.averageMseconds(), 4u) << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		const double threshold = std::is_same<Scalar, float>::value ? 0.75 : 0.95;

		if (percent < threshold)
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

bool TestP3P::testP3PWithRays(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing P3P for 3D rays:";

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

		// transformation transforming 3D points defined in the coordinate system of the camera to 3D points defined in the woorld coordinate system
		const HomogenousMatrix4 perfectPose(Utilities::viewPosition(pinholeCamera, objectPoints, quaternion * Vector3(0, -1, 0)));

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
		{
			continue;
		}

		// transformation transforming 3D points defined in the world coordinate system to 3D points defined in the coordinate system of the camera
		HomogenousMatrix4 invertedPerfectPose(perfectPose);
		if (!invertedPerfectPose.invert())
		{
			ocean_assert(false && "This must never happen!");
			continue;
		}

		// we determine the 3D rays starting at the camera's center of projecting and pointing towards the 3D object points
		// the rays are defined in the coordinate system of the camera (and not in the coordinate system of the 3D object points)

		Vector3 rays[3] =
		{
			(invertedPerfectPose * objectPoints[0]).normalizedOrZero(),
			(invertedPerfectPose * objectPoints[1]).normalizedOrZero(),
			(invertedPerfectPose * objectPoints[2]).normalizedOrZero(),
		};

		ocean_assert(Numeric::isEqual(rays[0].length(), 1) && Numeric::isEqual(rays[1].length(), 1) && Numeric::isEqual(rays[2].length(), 1));

		HomogenousMatrix4 poses[4];

		performance.start();
		const unsigned int numberPoses = Geometry::P3P::poses(objectPoints.data(), rays, poses);

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
bool TestP3P::testP3PWithRaysStressTest(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Stress testing P3P for 3D rays for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;

	HomogenousMatrixT4<T> poses[4];

	unsigned long long dummyValue = 0u;

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

		const unsigned int numberPoses = Geometry::P3P::poses<T>(objectPoints, rays, poses);
		dummyValue += numberPoses;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (dummyValue % 2u == 0u)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: succeeded.";
	}

	// we return True in any case
	return (dummyValue % 2u == 0u) || (dummyValue % 2u == 1u);
}

}

}

}
