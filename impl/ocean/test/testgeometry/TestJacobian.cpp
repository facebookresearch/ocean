/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestJacobian.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/geometry/Jacobian.h"
#include "ocean/geometry/Homography.h"

#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/Random.h"

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

bool TestJacobian::test(const double testDuration)
{
	Log::info() << "---   Jacobian test:   ---";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testOrientationalJacobian2x3<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOrientationalJacobian2x3<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testOrientationJacobian2nx3<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testOrientationJacobian2nx3<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPinholeCameraPoseJacobian2nx6(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFisheyeCameraPoseJacobian2x6(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAnyCameraPoseJacobian2nx6<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAnyCameraPoseJacobian2nx6<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPoseJacobianDampedDistortion2nx6(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPoseZoomJacobian2nx7(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPinholeCameraObjectTransformation2nx6(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFisheyeCameraObjectTransformation2nx6(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPinholeCameraPointJacobian2nx3(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFisheyeCameraPointJacobian2x3(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAnyCameraPointJacobian2x3(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPosesPointsJacobian2nx12(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSphericalObjectPoint3x3(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSphericalObjectPointOrientation2x3IF<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testSphericalObjectPointOrientation2x3IF<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCameraDistortionJacobian2x4(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCameraJacobian2x6(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCameraJacobian2x7(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCameraJacobian2x8(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testOrientationCameraJacobian2x11(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPoseCameraJacobian2x12(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPoseCameraJacobian2x14(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomography2x8(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomography2x9(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIdentityHomography2x8(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIdentityHomography2x9(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSimilarity2x4(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCalculateFisheyeDistortNormalized2x2<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testCalculateFisheyeDistortNormalized2x2<double>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Jacobian test succeeded.";
	}
	else
	{
		Log::info() << "Jacobian test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestJacobian, OrientationalJacobian2x3_float)
{
	EXPECT_TRUE((TestJacobian::testOrientationalJacobian2x3<float>(GTEST_TEST_DURATION)));
}

TEST(TestJacobian, OrientationalJacobian2x3_double)
{
	EXPECT_TRUE((TestJacobian::testOrientationalJacobian2x3<double>(GTEST_TEST_DURATION)));
}


TEST(TestJacobian, OrientationJacobian2nx3_float)
{
	EXPECT_TRUE((TestJacobian::testOrientationJacobian2nx3<float>(GTEST_TEST_DURATION)));
}

TEST(TestJacobian, OrientationJacobian2nx3_double)
{
	EXPECT_TRUE((TestJacobian::testOrientationJacobian2nx3<double>(GTEST_TEST_DURATION)));
}


TEST(TestJacobian, PinholeCameraPoseJacobian2nx6)
{
	EXPECT_TRUE(TestJacobian::testPinholeCameraPoseJacobian2nx6(GTEST_TEST_DURATION));
}

TEST(TestJacobian, FisheyeCameraPoseJacobian2nx6)
{
	EXPECT_TRUE(TestJacobian::testFisheyeCameraPoseJacobian2x6(GTEST_TEST_DURATION));
}

TEST(TestJacobian, AnyCameraPoseJacobian2nx6_float)
{
	EXPECT_TRUE(TestJacobian::testAnyCameraPoseJacobian2nx6<float>(GTEST_TEST_DURATION));
}

TEST(TestJacobian, AnyCameraPoseJacobian2nx6_double)
{
	EXPECT_TRUE(TestJacobian::testAnyCameraPoseJacobian2nx6<double>(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PoseJacobianDampedDistortion2nx6)
{
	EXPECT_TRUE(TestJacobian::testPoseJacobianDampedDistortion2nx6(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PoseZoomJacobian2nx7)
{
	EXPECT_TRUE(TestJacobian::testPoseZoomJacobian2nx7(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PinholeCameraObjectTransformation2nx6)
{
	EXPECT_TRUE(TestJacobian::testPinholeCameraObjectTransformation2nx6(GTEST_TEST_DURATION));
}

TEST(TestJacobian, FisheyeCameraObjectTransformation2nx6)
{
	EXPECT_TRUE(TestJacobian::testFisheyeCameraObjectTransformation2nx6(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PinholeCameraPointJacobian2nx3)
{
	EXPECT_TRUE(TestJacobian::testPinholeCameraPointJacobian2nx3(GTEST_TEST_DURATION));
}

TEST(TestJacobian, FisheyeCameraPointJacobian2x3)
{
	EXPECT_TRUE(TestJacobian::testFisheyeCameraPointJacobian2x3(GTEST_TEST_DURATION));
}

TEST(TestJacobian, AnyCameraPointJacobian2x3)
{
	EXPECT_TRUE(TestJacobian::testAnyCameraPointJacobian2x3(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PosesPointsJacobian2nx12)
{
	EXPECT_TRUE(TestJacobian::testPosesPointsJacobian2nx12(GTEST_TEST_DURATION));
}

TEST(TestJacobian, SphericalObjectPoint3x3)
{
	EXPECT_TRUE(TestJacobian::testSphericalObjectPoint3x3(GTEST_TEST_DURATION));
}


TEST(TestJacobian, SphericalObjectPointOrientation2x3IF_float)
{
	EXPECT_TRUE(TestJacobian::testSphericalObjectPointOrientation2x3IF<float>(GTEST_TEST_DURATION));
}

TEST(TestJacobian, SphericalObjectPointOrientation2x3IF_double)
{
	EXPECT_TRUE(TestJacobian::testSphericalObjectPointOrientation2x3IF<double>(GTEST_TEST_DURATION));
}


TEST(TestJacobian, CameraDistortionJacobian2x4)
{
	EXPECT_TRUE(TestJacobian::testCameraDistortionJacobian2x4(GTEST_TEST_DURATION));
}

TEST(TestJacobian, CameraJacobian2x6)
{
	EXPECT_TRUE(TestJacobian::testCameraJacobian2x6(GTEST_TEST_DURATION));
}

TEST(TestJacobian, CameraJacobian2x7)
{
	EXPECT_TRUE(TestJacobian::testCameraJacobian2x7(GTEST_TEST_DURATION));
}

TEST(TestJacobian, CameraJacobian2x8)
{
	EXPECT_TRUE(TestJacobian::testCameraJacobian2x8(GTEST_TEST_DURATION));
}

TEST(TestJacobian, OrientationCameraJacobian2x11)
{
	EXPECT_TRUE(TestJacobian::testOrientationCameraJacobian2x11(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PoseCameraJacobian2x12)
{
	EXPECT_TRUE(TestJacobian::testPoseCameraJacobian2x12(GTEST_TEST_DURATION));
}

TEST(TestJacobian, Homography2x8)
{
	EXPECT_TRUE(TestJacobian::testHomography2x8(GTEST_TEST_DURATION));
}

TEST(TestJacobian, Homography2x9)
{
	EXPECT_TRUE(TestJacobian::testHomography2x9(GTEST_TEST_DURATION));
}

TEST(TestJacobian, IdentityHomography2x8)
{
	EXPECT_TRUE(TestJacobian::testIdentityHomography2x8(GTEST_TEST_DURATION));
}

TEST(TestJacobian, IdentityHomography2x9)
{
	EXPECT_TRUE(TestJacobian::testIdentityHomography2x9(GTEST_TEST_DURATION));
}

TEST(TestJacobian, Similarity2x4)
{
	EXPECT_TRUE(TestJacobian::testSimilarity2x4(GTEST_TEST_DURATION));
}

TEST(TestJacobian, CalculateFisheyeDistortNormalized2x2_float)
{
	EXPECT_TRUE(TestJacobian::testCalculateFisheyeDistortNormalized2x2<float>(GTEST_TEST_DURATION));
}

TEST(TestJacobian, CalculateFisheyeDistortNormalized2x2_double)
{
	EXPECT_TRUE(TestJacobian::testCalculateFisheyeDistortNormalized2x2<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestJacobian::testOrientationalJacobian2x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing orientational Jacobian rodrigues 2x3 for " << numberPoints << " points with " << sizeof(T) * 8 << "-bit precision:";

	const std::vector<double> epsilons = {NumericD::weakEps(), NumericD::weakEps() / 10.0, NumericD::weakEps() * 10.0, NumericD::weakEps() / 100.0, NumericD::weakEps() * 100.0};

	RandomGenerator randomGenerator;

	constexpr double threshold = std::is_same<float, T>::value ? 0.95 : 0.99;

	ValidationPrecision validation(threshold, randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceOptimized;

	constexpr T cameraBorder = T(50);

	enum DistortionType : uint32_t
	{
		DT_NO_DISTORTION = 0u,
		DT_RADIAL_DISTORTION = 1u << 0u | DT_NO_DISTORTION,
		DT_FULL_DISTORTION = (1u << 1u) | DT_RADIAL_DISTORTION
	};

	const Timestamp startTimestamp(true);

	do
	{
		for (const DistortionType distortionType : {DT_NO_DISTORTION, DT_RADIAL_DISTORTION, DT_FULL_DISTORTION})
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			constexpr unsigned int width = 1280u;
			constexpr unsigned int height = 720u;

			const T width_2 = T(width) * T(0.5);
			const T height_2 = T(height) * T(0.5);

			const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(40), NumericT<T>::deg2rad(70));

			const T principalX = RandomT<T>::scalar(randomGenerator, width_2 - T(50), width_2 + T(50));
			const T principalY = RandomT<T>::scalar(randomGenerator, height_2 - T(50), height_2 + T(50));

			const VectorT3<T> translation_T_world(RandomT<T>::vector3(randomGenerator, -10, 10));
			const QuaternionT<T> flippedCamera_R_translation(RandomT<T>::quaternion(randomGenerator));

			const HomogenousMatrixT4<T> flippedCamera_T_world = HomogenousMatrixT4<T>(flippedCamera_R_translation) * HomogenousMatrixT4<T>(translation_T_world);

			/**
			 * flippedCamera_T_world:   rotational part   translational part
			 * | R | R t |                | R | 0 |         | I | t |
			 * | 0 |  1  |              = | 0 | 1 |    *    | 0 | 1 |
			 */
			ocean_assert(flippedCamera_T_world.rotation().isEqual(flippedCamera_R_translation, NumericT<T>::weakEps()));
			ocean_assert(flippedCamera_T_world.translation().isEqual(flippedCamera_R_translation * translation_T_world, NumericT<T>::weakEps()));

			const HomogenousMatrixT4<T> world_T_camera(AnyCamera::invertedFlipped2Standard(flippedCamera_T_world));

			PinholeCameraT<T> pinholeCamera(width, height, fovX, principalX, principalY);

			if ((distortionType & DT_RADIAL_DISTORTION) == DT_RADIAL_DISTORTION)
			{
				const T k1 = RandomT<T>::scalar(T(-0.5), T(0.5));
				const T k2 = RandomT<T>::scalar(T(-0.5), T(0.5));
				pinholeCamera.setRadialDistortion(typename PinholeCameraT<T>::DistortionPair(k1, k2));
			}

			if ((distortionType & DT_FULL_DISTORTION) == DT_FULL_DISTORTION)
			{
				const T p1 = RandomT<T>::scalar(T(-0.01), T(0.01));
				const T p2 = RandomT<T>::scalar(T(-0.01), T(0.01));
				pinholeCamera.setTangentialDistortion(typename PinholeCameraT<T>::DistortionPair(p1, p2));
			}

			const AnyCameraPinholeT<T> camera(pinholeCamera);

			VectorsT3<T> objectPoints;
			objectPoints.reserve(numberPoints);
			while (objectPoints.size() < numberPoints)
			{
				const VectorT2<T> distortedImagePoint = RandomT<T>::vector2(randomGenerator, cameraBorder, T(camera.width()) - cameraBorder, cameraBorder, T(camera.height()) - cameraBorder);

				const VectorT2<T> undistortedImagePoint = pinholeCamera.template undistort<true>(distortedImagePoint);

				if (!camera.isInside(undistortedImagePoint, cameraBorder))
				{
					// we don't use image points which can be close to the camera border
					continue;
				}

				const LineT3<T> ray(camera.ray(distortedImagePoint, world_T_camera));
				const VectorT3<T> objectPoint(ray.point(RandomT<T>::scalar(randomGenerator, 1, 5)));

				objectPoints.push_back(objectPoint);
			}

			/**
			 * jacobian for one point
			 * jacobian x: | dfx / dwx, dfx / dwy, dfx / dwz |
			 * jacobian y: | dfy / dwx, dfy / dwy, dfy / dwz |
			 */

			{
				MatrixT<T> naiveJacobian(2 * objectPoints.size(), 3);

				const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

				for (size_t n = 0; n < objectPoints.size(); ++n)
				{
					const VectorT3<T> objectPoint = objectPoints[n];
					const VectorT2<T> imagePoint(camera.projectToImageIF(flippedCamera_T_world, objectPoint));

					for (unsigned int i = 0u; i < 3u; ++i)
					{
						ExponentialMapT<T> rotationDelta(flippedCamera_R_translation);
						rotationDelta[i] += NumericT<T>::weakEps();

						const VectorT2<T> imagePointDelta(camera.projectToImageIF(HomogenousMatrixT4<T>(rotationDelta.quaternion()) * HomogenousMatrixT4<T>(translation_T_world), objectPoint));
						const VectorT2<T> derivative = (imagePointDelta - imagePoint) / NumericT<T>::weakEps();

						naiveJacobian[n * 2 + 0][i] = derivative.x();
						naiveJacobian[n * 2 + 1][i] = derivative.y();
					}
				}
			}

			SquareMatrixT3<T> dwx, dwy, dwz;
			Geometry::Jacobian::calculateRotationRodriguesDerivative(ExponentialMapT<T>(flippedCamera_R_translation), dwx, dwy, dwz);

			MatrixT<T> jacobian(2 * objectPoints.size(), 3);

			performanceOptimized.start();
				for (size_t n = 0; n < objectPoints.size(); ++n)
				{
					const VectorT3<T> objectPoint = objectPoints[n];

					T* jacobianX = jacobian[2 * n + 0];
					T* jacobianY = jacobian[2 * n + 1];

					Geometry::Jacobian::calculateOrientationalJacobianRodrigues2x3IF(camera, flippedCamera_R_translation, translation_T_world, objectPoint, dwx, dwy, dwz, jacobianX, jacobianY);
				}
			performanceOptimized.stop();

			const SharedAnyCameraD cameraD = camera.cloneToDouble();
			ocean_assert(cameraD);

			const QuaternionD flippedCamera_R_translationD = QuaternionD(flippedCamera_R_translation).normalized();
			const HomogenousMatrixD4 flippedCamera_T_worldD = HomogenousMatrixD4(flippedCamera_R_translationD) * HomogenousMatrixD4(VectorD3(translation_T_world));

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const VectorD3 objectPoint = VectorD3(objectPoints[n]);
				const VectorD2 imagePoint(cameraD->projectToImageIF(flippedCamera_T_worldD, objectPoint));

				const T* jacobianX = jacobian[2 * n + 0];
				const T* jacobianY = jacobian[2 * n + 1];

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dwx
						ExponentialMapD rotationWx = ExponentialMapD(flippedCamera_R_translationD);
						rotationWx[0] += epsilon;

						const VectorD2 imagePointWx(cameraD->projectToImageIF(HomogenousMatrixD4(rotationWx.quaternion()) * HomogenousMatrixD4(VectorD3(translation_T_world)), objectPoint));
						if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[0], jacobianY[0]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dwy
						ExponentialMapD rotationWy = ExponentialMapD(flippedCamera_R_translationD);
						rotationWy[1] += epsilon;

						const VectorD2 imagePointWy(cameraD->projectToImageIF(HomogenousMatrixD4(rotationWy.quaternion()) * HomogenousMatrixD4(VectorD3(translation_T_world)), objectPoint));
						if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[1], jacobianY[1]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dwz
						ExponentialMapD rotationWz = ExponentialMapD(flippedCamera_R_translationD);
						rotationWz[2] += epsilon;

						const VectorD2 imagePointWz(cameraD->projectToImageIF(HomogenousMatrixD4(rotationWz.quaternion()) * HomogenousMatrixD4(VectorD3(translation_T_world)), objectPoint));
						if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[2], jacobianY[2]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance optimized: " << performanceOptimized.averageMseconds() << "ms";
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestJacobian::testOrientationJacobian2nx3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing orientation Jacobian rodrigues 2nx3 for " << numberPoints << " points with " << sizeof(T) * 8 << "-bit precision:";

	const std::vector<double> epsilons = {NumericD::weakEps(), NumericD::weakEps() / 10.0, NumericD::weakEps() * 10.0, NumericD::weakEps() / 100.0, NumericD::weakEps() * 100.0};

	RandomGenerator randomGenerator;

	constexpr double threshold = std::is_same<float, T>::value ? 0.95 : 0.99;

	ValidationPrecision validation(threshold, randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceOptimized;

	constexpr T cameraBorder = T(50);

	enum DistortionType : uint32_t
	{
		DT_NO_DISTORTION = 0u,
		DT_RADIAL_DISTORTION = 1u << 0u | DT_NO_DISTORTION,
		DT_FULL_DISTORTION = (1u << 1u) | DT_RADIAL_DISTORTION
	};

	const Timestamp startTimestamp(true);

	do
	{
		for (const DistortionType distortionType : {DT_NO_DISTORTION, DT_RADIAL_DISTORTION, DT_FULL_DISTORTION})
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			constexpr unsigned int width = 1280u;
			constexpr unsigned int height = 720u;

			const T width_2 = T(width) * T(0.5);
			const T height_2 = T(height) * T(0.5);

			const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(40), NumericT<T>::deg2rad(70));

			const T principalX = RandomT<T>::scalar(randomGenerator, width_2 - T(50), width_2 + T(50));
			const T principalY = RandomT<T>::scalar(randomGenerator, height_2 - T(50), height_2 + T(50));

			const QuaternionT<T> flippedCamera_R_world(RandomT<T>::quaternion(randomGenerator));

			const HomogenousMatrixT4<T> flippedCamera_T_world = HomogenousMatrixT4<T>(flippedCamera_R_world);

			const HomogenousMatrixT4<T> world_T_camera(AnyCamera::invertedFlipped2Standard(flippedCamera_T_world));

			PinholeCameraT<T> pinholeCamera(width, height, fovX, principalX, principalY);

			if ((distortionType & DT_RADIAL_DISTORTION) == DT_RADIAL_DISTORTION)
			{
				const T k1 = RandomT<T>::scalar(T(-0.5), T(0.5));
				const T k2 = RandomT<T>::scalar(T(-0.5), T(0.5));
				pinholeCamera.setRadialDistortion(typename PinholeCameraT<T>::DistortionPair(k1, k2));
			}

			if ((distortionType & DT_FULL_DISTORTION) == DT_FULL_DISTORTION)
			{
				const T p1 = RandomT<T>::scalar(T(-0.01), T(0.01));
				const T p2 = RandomT<T>::scalar(T(-0.01), T(0.01));
				pinholeCamera.setTangentialDistortion(typename PinholeCameraT<T>::DistortionPair(p1, p2));
			}

			const AnyCameraPinholeT<T> camera(pinholeCamera);

			VectorsT3<T> objectPoints;
			objectPoints.reserve(numberPoints);
			while (objectPoints.size() < numberPoints)
			{
				const VectorT2<T> distortedImagePoint = RandomT<T>::vector2(randomGenerator, cameraBorder, T(camera.width()) - cameraBorder, cameraBorder, T(camera.height()) - cameraBorder);

				const VectorT2<T> undistortedImagePoint = pinholeCamera.template undistort<true>(distortedImagePoint);

				if (!camera.isInside(undistortedImagePoint, cameraBorder))
				{
					// we don't use image points which can be close to the camera border
					continue;
				}

				const LineT3<T> ray(camera.ray(distortedImagePoint, world_T_camera));
				const VectorT3<T> objectPoint(ray.point(RandomT<T>::scalar(randomGenerator, 1, 5)));

				objectPoints.push_back(objectPoint);
			}

			/**
			 * jacobian for one point
			 * jacobian x: | dfx / dwx, dfx / dwy, dfx / dwz |
			 * jacobian y: | dfy / dwx, dfy / dwy, dfy / dwz |
			 */

			{
				MatrixT<T> naiveJacobian(2 * objectPoints.size(), 3);

				const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

				for (size_t n = 0; n < objectPoints.size(); ++n)
				{
					const VectorT3<T> objectPoint = objectPoints[n];
					const VectorT2<T> imagePoint(camera.projectToImageIF(flippedCamera_T_world, objectPoint));

					for (unsigned int i = 0u; i < 3u; ++i)
					{
						ExponentialMapT<T> rotationDelta(flippedCamera_R_world);
						rotationDelta[i] += NumericT<T>::weakEps();

						const VectorT2<T> imagePointDelta(camera.projectToImageIF(HomogenousMatrixT4<T>(rotationDelta.quaternion()), objectPoint));
						const VectorT2<T> derivative = (imagePointDelta - imagePoint) / NumericT<T>::weakEps();

						naiveJacobian[n * 2 + 0][i] = derivative.x();
						naiveJacobian[n * 2 + 1][i] = derivative.y();
					}
				}
			}

			const ExponentialMapT<T> flippedCamera_E_world(flippedCamera_R_world);

			MatrixT<T> jacobian(2 * objectPoints.size(), 3);

			performanceOptimized.start();
				Geometry::Jacobian::calculateOrientationJacobianRodrigues2nx3IF(jacobian.data(), camera, flippedCamera_E_world, ConstArrayAccessor<VectorT3<T>>(objectPoints));
			performanceOptimized.stop();

			const SharedAnyCameraD cameraD = camera.cloneToDouble();
			ocean_assert(cameraD);

			const ExponentialMapD flippedCamera_E_worldD(flippedCamera_E_world);

			const HomogenousMatrixD4 flippedCamera_T_worldD = HomogenousMatrixD4(flippedCamera_E_worldD.rotation());

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const VectorD3 objectPoint = VectorD3(objectPoints[n]);
				const VectorD2 imagePoint(cameraD->projectToImageIF(flippedCamera_T_worldD, objectPoint));

				const T* jacobianX = jacobian[2 * n + 0];
				const T* jacobianY = jacobian[2 * n + 1];

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dwx
						ExponentialMapD rotationWx = flippedCamera_E_worldD;
						rotationWx[0] += epsilon;

						const VectorD2 imagePointWx(cameraD->projectToImageIF(HomogenousMatrixD4(rotationWx.quaternion()), objectPoint));
						if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[0], jacobianY[0]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dwy
						ExponentialMapD rotationWy = flippedCamera_E_worldD;
						rotationWy[1] += epsilon;

						const VectorD2 imagePointWy(cameraD->projectToImageIF(HomogenousMatrixD4(rotationWy.quaternion()), objectPoint));
						if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[1], jacobianY[1]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dwz
						ExponentialMapD rotationWz = ExponentialMapD(flippedCamera_E_world);
						rotationWz[2] += epsilon;

						const VectorD2 imagePointWz(cameraD->projectToImageIF(HomogenousMatrixD4(rotationWz.quaternion()), objectPoint));
						if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[2], jacobianY[2]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance optimized: " << performanceOptimized.averageMseconds() << "ms";
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestJacobian::testPinholeCameraPoseJacobian2nx6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing pinhole camera pose Jacobian rodrigues 2x6 and 2nx6 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performancePerfectCamera;
	HighPerformanceStatistic performanceDistortedCamera;

	const Timestamp startTimestamp(true);

	unsigned int distortionIteration = 0u;

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 translation(Random::vector3(-10, 10));
		const Quaternion quaternion(Random::quaternion());

		const Pose pose(translation, quaternion);
		const HomogenousMatrix4 transformation(translation, quaternion);

		const HomogenousMatrix4 transformationIF(PinholeCamera::standard2InvertedFlipped(transformation));
		const Pose poseIF(transformationIF);

		PinholeCamera camera(width, height, fovX, principalX, principalY);

		if (distortionIteration % 3u == 1u || distortionIteration % 3u == 2u)
		{
			const Scalar k1 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			const Scalar k2 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		}

		if (distortionIteration % 3u == 2u)
		{
			const Scalar p1 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			const Scalar p2 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			camera.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));
		}

		distortionIteration++;

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			Vector2 tmpImagePoint(Random::vector2(0, 1));
			tmpImagePoint.x() *= Scalar(camera.width());
			tmpImagePoint.y() *= Scalar(camera.height());

			const Line3 ray(camera.ray(tmpImagePoint, translation, quaternion));
			const Vector3 objectPoint(ray.point(Random::scalar(1, 5)));

			objectPoints.push_back(objectPoint);
		}

		/**
		 * jacobian for one point
		 * jacobian x: | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * jacobian y: | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 */

		Matrix jacobian(2 * objectPoints.size(), 6);

		if (camera.hasDistortionParameters())
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceDistortedCamera);
			Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6(jacobian.data(), camera, poseIF, objectPoints.data(), objectPoints.size(), true);
		}
		else
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performancePerfectCamera);
			Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6(jacobian.data(), camera, poseIF, objectPoints.data(), objectPoints.size(), false);
		}

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 6);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(transformationIF, objectPoint, camera.hasDistortionParameters()));

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					Pose poseDelta(poseIF);
					poseDelta[i] += Numeric::weakEps();

					const Vector2 imagePointDelta(camera.projectToImageIF<false>(poseDelta.transformation(), objectPoint, camera.hasDistortionParameters()));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3 objectPoint = objectPoints[n];
			const Vector2 imagePoint(camera.projectToImageIF<false>(transformationIF, objectPoint, camera.hasDistortionParameters()));

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			{
				// we also test the first implementation for one object point

				Scalar singleJacobianX[6], singleJacobianY[6];
				Geometry::Jacobian::calculatePoseJacobianRodrigues2x6(singleJacobianX, singleJacobianY, camera, poseIF, objectPoint, camera.hasDistortionParameters());

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						accurate = false;
					}
				}
			}

			{
				// we also test the second implementation for one object point

				SquareMatrix3 dwx, dwy, dwz;
				Geometry::Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(poseIF[3], poseIF[4], poseIF[5]), dwx, dwy, dwz);

				Scalar singleJacobianX[6], singleJacobianY[6];
				Geometry::Jacobian::calculatePoseJacobianRodrigues2x6(singleJacobianX, singleJacobianY, camera, poseIF.transformation(), objectPoint, camera.hasDistortionParameters(), dwx, dwy, dwz);

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						accurate = false;
					}
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwx
					Pose poseWx(poseIF);
					poseWx.rx() += epsilon;

					const Vector2 imagePointWx(camera.projectToImageIF<false>(poseWx.transformation(), objectPoint, camera.hasDistortionParameters()));
					if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[0], jacobianY[0]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwy
					Pose poseWy(poseIF);
					poseWy.ry() += epsilon;

					const Vector2 imagePointWy(camera.projectToImageIF<false>(poseWy.transformation(), objectPoint, camera.hasDistortionParameters()));
					if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[1], jacobianY[1]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwz
					Pose poseWz(poseIF);
					poseWz.rz() += epsilon;

					const Vector2 imagePointWz(camera.projectToImageIF<false>(poseWz.transformation(), objectPoint, camera.hasDistortionParameters()));
					if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[2], jacobianY[2]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtx
					Pose poseTx(poseIF);
					poseTx.x() += epsilon;

					const Vector2 imagePointTx(camera.projectToImageIF<false>(poseTx.transformation(), objectPoint, camera.hasDistortionParameters()));
					if (checkAccuracy(imagePoint, imagePointTx, epsilon, jacobianX[3], jacobianY[3]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dty
					Pose poseTy(poseIF);
					poseTy.y() += epsilon;

					const Vector2 imagePointTy(camera.projectToImageIF<false>(poseTy.transformation(), objectPoint, camera.hasDistortionParameters()));
					if (checkAccuracy(imagePoint, imagePointTy, epsilon, jacobianX[4], jacobianY[4]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtz
					Pose poseTz(poseIF);
					poseTz.z() += epsilon;

					const Vector2 imagePointTz(camera.projectToImageIF<false>(poseTz.transformation(), objectPoint, camera.hasDistortionParameters()));
					if (checkAccuracy(imagePoint, imagePointTz, epsilon, jacobianX[5], jacobianY[5]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance perfect camera: " << performancePerfectCamera.averageMseconds() << "ms";
	Log::info() << "Performance distorted camera: " << performanceDistortedCamera.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testFisheyeCameraPoseJacobian2x6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing fisheye camera pose Jacobian rodrigues 2x6 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		const FisheyeCamera fisheyeCamera = Utilities::realisticFisheyeCamera(RandomI::random(1u));

		const Vector3 translation(Random::vector3(-10, 10));
		const Quaternion quaternion(Random::quaternion());

		const Pose pose(translation, quaternion);
		const HomogenousMatrix4 transformation(translation, quaternion);

		const HomogenousMatrix4 transformationIF(PinholeCamera::standard2InvertedFlipped(transformation));
		const Pose poseIF(transformationIF);

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			const Vector2 imagePoint(Random::vector2(Scalar(5), Scalar(fisheyeCamera.width() - 5u), Scalar(5), Scalar(fisheyeCamera.height() - 5u)));

			const Line3 ray(fisheyeCamera.ray(imagePoint, transformation));
			const Vector3 objectPoint(ray.point(Random::scalar(1, 5)));

			objectPoints.push_back(objectPoint);
		}

		/**
		 * jacobian for one point
		 * jacobian x: | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * jacobian y: | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 */

		Matrix jacobian(2 * objectPoints.size(), 6);

		{
			const HighPerformanceStatistic::ScopedStatistic scope(performance);

			SquareMatrix3 dwx, dwy, dwz;
			Geometry::Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(poseIF[3], poseIF[4], poseIF[5]), dwx, dwy, dwz);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				Geometry::Jacobian::calculatePoseJacobianRodrigues2x6(jacobian[n * 2 + 0], jacobian[n * 2 + 1], fisheyeCamera, poseIF.transformation(), objectPoints[n], dwx, dwy, dwz);
			}
		}

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 6);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(fisheyeCamera.projectToImageIF(transformationIF, objectPoint));

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					Pose poseDelta(poseIF);
					poseDelta[i] += Numeric::weakEps();

					const Vector2 imagePointDelta(fisheyeCamera.projectToImageIF(poseDelta.transformation(), objectPoint));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3 objectPoint = objectPoints[n];
			const Vector2 imagePoint(fisheyeCamera.projectToImageIF(transformationIF, objectPoint));

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwx
					Pose poseWx(poseIF);
					poseWx.rx() += epsilon;

					const Vector2 imagePointWx(fisheyeCamera.projectToImageIF(poseWx.transformation(), objectPoint));
					if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[0], jacobianY[0]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwy
					Pose poseWy(poseIF);
					poseWy.ry() += epsilon;

					const Vector2 imagePointWy(fisheyeCamera.projectToImageIF(poseWy.transformation(), objectPoint));
					if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[1], jacobianY[1]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwz
					Pose poseWz(poseIF);
					poseWz.rz() += epsilon;

					const Vector2 imagePointWz(fisheyeCamera.projectToImageIF(poseWz.transformation(), objectPoint));
					if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[2], jacobianY[2]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtx
					Pose poseTx(poseIF);
					poseTx.x() += epsilon;

					const Vector2 imagePointTx(fisheyeCamera.projectToImageIF(poseTx.transformation(), objectPoint));
					if (checkAccuracy(imagePoint, imagePointTx, epsilon, jacobianX[3], jacobianY[3]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dty
					Pose poseTy(poseIF);
					poseTy.y() += epsilon;

					const Vector2 imagePointTy(fisheyeCamera.projectToImageIF(poseTy.transformation(), objectPoint));
					if (checkAccuracy(imagePoint, imagePointTy, epsilon, jacobianX[4], jacobianY[4]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtz
					Pose poseTz(poseIF);
					poseTz.z() += epsilon;

					const Vector2 imagePointTz(fisheyeCamera.projectToImageIF(poseTz.transformation(), objectPoint));
					if (checkAccuracy(imagePoint, imagePointTz, epsilon, jacobianX[5], jacobianY[5]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

template <typename T>
bool TestJacobian::testAnyCameraPoseJacobian2nx6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing any camera pose Jacobian rodrigues 2x6 for " << numberPoints << " points with " << sizeof(T) * 8 << "-bit precision:";

	const std::vector<double> epsilons = {NumericD::weakEps(), NumericD::weakEps() / 10.0, NumericD::weakEps() * 10.0, NumericD::weakEps() / 100.0, NumericD::weakEps() * 100.0};

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	bool firstCameraIteration = true;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		Log::info().newLine(!firstCameraIteration);
		firstCameraIteration = false;

		const SharedAnyCameraT<T> sharedAnyCamera = Utilities::realisticAnyCamera<T>(anyCameraType, RandomI::random(randomGenerator, 1u));
		ocean_assert(sharedAnyCamera);

		const AnyCameraT<T>& anyCamera = *sharedAnyCamera;

		Log::info() << "Camera name: " << anyCamera.name();

		constexpr double threshold = std::is_same<float, T>::value ? 0.95 : 0.99;

		ValidationPrecision validation(threshold, randomGenerator);

		HighPerformanceStatistic performanceNaive;
		HighPerformanceStatistic performance;

		const Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const VectorT3<T> translation(RandomT<T>::vector3(randomGenerator, -10, 10));
			const QuaternionT<T> quaternion(RandomT<T>::quaternion(randomGenerator));

			const HomogenousMatrixT4<T> world_T_camera(translation, quaternion);

			const HomogenousMatrixT4<T> flippedCamera_T_world(AnyCameraT<T>::standard2InvertedFlipped(world_T_camera));
			const PoseT<T> flippedCamera_P_world(flippedCamera_T_world);

			VectorsT3<T> objectPoints;
			objectPoints.reserve(numberPoints);

			while (objectPoints.size() < numberPoints)
			{
				const VectorT2<T> imagePoint(RandomT<T>::vector2(randomGenerator, T(5), T(anyCamera.width() - 5u), T(5), T(anyCamera.height() - 5u)));

				const LineT3<T> ray(anyCamera.ray(imagePoint, world_T_camera));
				const VectorT3<T> objectPoint(ray.point(RandomT<T>::scalar(randomGenerator, T(1), T(5))));

				objectPoints.push_back(objectPoint);
			}

			/**
			 * jacobian for one point
			 * jacobian x: | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
			 * jacobian y: | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
			 */

			MatrixT<T> jacobian(2 * objectPoints.size(), 6);

			{
				const HighPerformanceStatistic::ScopedStatistic scope(performance);

				Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6IF(jacobian.data(), anyCamera, flippedCamera_P_world, objectPoints.data(), objectPoints.size());
			}

			{
				MatrixT<T> naiveJacobian(2 * objectPoints.size(), 6);

				const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

				for (size_t n = 0; n < objectPoints.size(); ++n)
				{
					const VectorT3<T> objectPoint = objectPoints[n];
					const VectorT2<T> imagePoint(anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint));

					for (unsigned int i = 0u; i < 6u; ++i)
					{
						PoseT<T> poseDelta(flippedCamera_P_world);
						poseDelta[i] += NumericT<T>::weakEps();

						const VectorT2<T> imagePointDelta(anyCamera.projectToImageIF(poseDelta.transformation(), objectPoint));
						const VectorT2<T> derivative = (imagePointDelta - imagePoint) / NumericT<T>::weakEps();

						naiveJacobian[n * 2 + 0][i] = derivative.x();
						naiveJacobian[n * 2 + 1][i] = derivative.y();
					}
				}
			}

			const SharedAnyCameraD sharedAnyCameraD = AnyCameraD::convert(sharedAnyCamera);
			ocean_assert(sharedAnyCameraD);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const VectorD3 objectPoint = VectorD3(objectPoints[n]);
				const VectorD2 imagePoint(sharedAnyCameraD->projectToImageIF(PoseD(flippedCamera_P_world).transformation(), objectPoint));

				const T* jacobianX = jacobian[2 * n + 0];
				const T* jacobianY = jacobian[2 * n + 1];

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dwx
						PoseD poseWx(flippedCamera_P_world);
						poseWx.rx() += epsilon;

						const VectorD2 imagePointWx(sharedAnyCameraD->projectToImageIF(poseWx.transformation(), objectPoint));
						if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[0], jacobianY[0]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dwy
						PoseD poseWy(flippedCamera_P_world);
						poseWy.ry() += epsilon;

						const VectorD2 imagePointWy(sharedAnyCameraD->projectToImageIF(poseWy.transformation(), objectPoint));
						if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[1], jacobianY[1]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dwz
						PoseD poseWz(flippedCamera_P_world);
						poseWz.rz() += epsilon;

						const VectorD2 imagePointWz(sharedAnyCameraD->projectToImageIF(poseWz.transformation(), objectPoint));
						if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[2], jacobianY[2]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dtx
						PoseD poseTx(flippedCamera_P_world);
						poseTx.x() += epsilon;

						const VectorD2 imagePointTx(sharedAnyCameraD->projectToImageIF(poseTx.transformation(), objectPoint));
						if (checkAccuracy(imagePoint, imagePointTx, epsilon, jacobianX[3], jacobianY[3]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dty
						PoseD poseTy(flippedCamera_P_world);
						poseTy.y() += epsilon;

						const VectorD2 imagePointTy(sharedAnyCameraD->projectToImageIF(poseTy.transformation(), objectPoint));
						if (checkAccuracy(imagePoint, imagePointTy, epsilon, jacobianX[4], jacobianY[4]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}

				{
					bool localAccuracy = false;

					for (const double epsilon : epsilons)
					{
						// df / dtz
						PoseD poseTz(flippedCamera_P_world);
						poseTz.z() += epsilon;

						const VectorD2 imagePointTz(sharedAnyCameraD->projectToImageIF(poseTz.transformation(), objectPoint));
						if (checkAccuracy(imagePoint, imagePointTz, epsilon, jacobianX[5], jacobianY[5]))
						{
							localAccuracy = true;
							break;
						}
					}

					if (!localAccuracy)
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
		Log::info() << "Performance: " << performance.averageMseconds() << "ms";
		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testPoseJacobianDampedDistortion2nx6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing pose Jacobian with damped distortion rodrigues 2x6 and 2nx6 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performancePerfectCamera;
	HighPerformanceStatistic performanceDistortedCamera;

	const Timestamp startTimestamp(true);

	unsigned int distortionIteration = 0u;

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 translation(Random::vector3(-10, 10));
		const Quaternion quaternion(Random::quaternion());

		const Pose pose(translation, quaternion);
		const HomogenousMatrix4 transformation(translation, quaternion);

		const HomogenousMatrix4 transformationIF(PinholeCamera::standard2InvertedFlipped(transformation));
		const Pose poseIF(transformationIF);

		PinholeCamera camera(width, height, fovX, principalX, principalY);

		if (distortionIteration % 3u == 1u || distortionIteration % 3u == 2u)
		{
			const Scalar k1 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			const Scalar k2 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		}

		if (distortionIteration % 3u == 2u)
		{
			const Scalar p1 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			const Scalar p2 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			camera.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));
		}

		distortionIteration++;

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			Vector2 tmpImagePoint(Random::vector2(-1, 2));
			tmpImagePoint.x() *= Scalar(camera.width());
			tmpImagePoint.y() *= Scalar(camera.height());

			const Line3 ray(camera.ray(tmpImagePoint, translation, quaternion));
			const Vector3 objectPoint(ray.point(Random::scalar(1, 5)));

			objectPoints.push_back(objectPoint);
		}

		/**
		 * jacobian for one point
		 * jacobian x: | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * jacobian y: | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 */

		Matrix jacobian(2 * objectPoints.size(), 6);

		if (camera.hasDistortionParameters())
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceDistortedCamera);
			Geometry::Jacobian::calculatePoseJacobianRodriguesDampedDistortion2nx6(jacobian.data(), camera, poseIF, Scalar(1), objectPoints.data(), objectPoints.size(), true);
		}
		else
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performancePerfectCamera);
			Geometry::Jacobian::calculatePoseJacobianRodriguesDampedDistortion2nx6(jacobian.data(), camera, poseIF, Scalar(1), objectPoints.data(), objectPoints.size(), false);
		}

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 6);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageDampedIF(transformationIF, objectPoint, camera.hasDistortionParameters(), Scalar(1)));

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					Pose poseDelta(poseIF);
					poseDelta[i] += Numeric::weakEps();

					const Vector2 imagePointDelta(camera.projectToImageDampedIF(poseDelta.transformation(), objectPoint, camera.hasDistortionParameters(), Scalar(1)));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3 objectPoint = objectPoints[n];
			const Vector2 imagePoint(camera.projectToImageDampedIF(transformationIF, objectPoint, camera.hasDistortionParameters(), Scalar(1)));

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwx
					Pose poseWx(poseIF);
					poseWx.rx() += epsilon;

					const Vector2 imagePointWx(camera.projectToImageDampedIF(poseWx.transformation(), objectPoint, camera.hasDistortionParameters(), Scalar(1)));
					if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[0], jacobianY[0]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwy
					Pose poseWy(poseIF);
					poseWy.ry() += epsilon;

					const Vector2 imagePointWy(camera.projectToImageDampedIF(poseWy.transformation(), objectPoint, camera.hasDistortionParameters(), Scalar(1)));
					if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[1], jacobianY[1]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwz
					Pose poseWz(poseIF);
					poseWz.rz() += epsilon;

					const Vector2 imagePointWz(camera.projectToImageDampedIF(poseWz.transformation(), objectPoint, camera.hasDistortionParameters(), Scalar(1)));
					if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[2], jacobianY[2]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtx
					Pose poseTx(poseIF);
					poseTx.x() += epsilon;

					const Vector2 imagePointTx(camera.projectToImageDampedIF(poseTx.transformation(), objectPoint, camera.hasDistortionParameters(), Scalar(1)));
					if (checkAccuracy(imagePoint, imagePointTx, epsilon, jacobianX[3], jacobianY[3]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dty
					Pose poseTy(poseIF);
					poseTy.y() += epsilon;

					const Vector2 imagePointTy(camera.projectToImageDampedIF(poseTy.transformation(), objectPoint, camera.hasDistortionParameters(), Scalar(1)));
					if (checkAccuracy(imagePoint, imagePointTy, epsilon, jacobianX[4], jacobianY[4]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtz
					Pose poseTz(poseIF);
					poseTz.z() += epsilon;

					const Vector2 imagePointTz(camera.projectToImageDampedIF(poseTz.transformation(), objectPoint, camera.hasDistortionParameters(), Scalar(1)));
					if (checkAccuracy(imagePoint, imagePointTz, epsilon, jacobianX[5], jacobianY[5]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance perfect camera: " << performancePerfectCamera.averageMseconds() << "ms";
	Log::info() << "Performance distorted camera: " << performanceDistortedCamera.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testPoseZoomJacobian2nx7(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing pose and zoom Jacobian rodrigues 2x7 and 2nx7 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performancePerfectCamera;
	HighPerformanceStatistic performanceDistortedCamera;

	const Timestamp startTimestamp(true);

	unsigned int distortionIteration = 0u;

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Scalar zoom = Random::scalar(0.25, 20);

		const Vector3 translation(Random::vector3(-10, 10));
		const Quaternion quaternion(Random::quaternion());

		const Pose pose(translation, quaternion);
		const HomogenousMatrix4 transformation(translation, quaternion);

		const HomogenousMatrix4 transformationIF(PinholeCamera::standard2InvertedFlipped(transformation));
		const Pose poseIF(transformationIF);

		PinholeCamera camera(width, height, fovX, principalX, principalY);

		if (distortionIteration % 3u == 1u || distortionIteration % 3u == 2u)
		{
			const Scalar k1 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			const Scalar k2 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		}

		if (distortionIteration % 3u == 2u)
		{
			const Scalar p1 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			const Scalar p2 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			camera.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));
		}

		distortionIteration++;

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			Vector2 tmpImagePoint(Random::vector2(0, 1));
			tmpImagePoint.x() *= Scalar(camera.width());
			tmpImagePoint.y() *= Scalar(camera.height());

			const Line3 ray(camera.ray(tmpImagePoint, translation, quaternion, zoom));
			const Vector3 objectPoint(ray.point(Random::scalar(1, 5)));

			objectPoints.push_back(objectPoint);
		}

		/**
		 * jacobian for one point
		 * jacobian x: | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz, dfx / ds |
		 * jacobian y: | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz, dfy / ds |
		 */

		Matrix jacobian(2 * objectPoints.size(), 7);

		if (camera.hasDistortionParameters())
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceDistortedCamera);
			Geometry::Jacobian::calculatePoseZoomJacobianRodrigues2nx7(jacobian.data(), camera, poseIF, zoom, objectPoints.data(), objectPoints.size(), true);
		}
		else
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performancePerfectCamera);
			Geometry::Jacobian::calculatePoseZoomJacobianRodrigues2nx7(jacobian.data(), camera, poseIF, zoom, objectPoints.data(), objectPoints.size(), false);
		}

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 7);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(transformationIF, objectPoint, camera.hasDistortionParameters(), zoom));

				for (unsigned int i = 0u; i < 7u; ++i)
				{
					Pose poseDelta(poseIF);
					Scalar zoomDelta(zoom);

					if (i < 6u)
					{
						poseDelta[i] += Numeric::weakEps();
					}
					else
					{
						zoomDelta += Numeric::weakEps();
					}

					const Vector2 imagePointDelta(camera.projectToImageIF<false>(poseDelta.transformation(), objectPoint, camera.hasDistortionParameters(), zoomDelta));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3 objectPoint = objectPoints[n];
			const Vector2 imagePoint(camera.projectToImageIF<false>(transformationIF, objectPoint, camera.hasDistortionParameters(), zoom));

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			Scalar singleJacobianX[7], singleJacobianY[7];
			Geometry::Jacobian::calculatePoseZoomJacobianRodrigues2x7(singleJacobianX, singleJacobianY, camera, poseIF, zoom, objectPoint, camera.hasDistortionParameters());

			for (unsigned int i = 0u; i < 7u; ++i)
			{
				ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
				ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

				if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwx
					Pose poseWx(poseIF);
					poseWx.rx() += epsilon;

					const Vector2 imagePointWx(camera.projectToImageIF<false>(poseWx.transformation(), objectPoint, camera.hasDistortionParameters(), zoom));
					if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[0], jacobianY[0]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwy
					Pose poseWy(poseIF);
					poseWy.ry() += epsilon;

					const Vector2 imagePointWy(camera.projectToImageIF<false>(poseWy.transformation(), objectPoint, camera.hasDistortionParameters(), zoom));
					if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[1], jacobianY[1]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwz
					Pose poseWz(poseIF);
					poseWz.rz() += epsilon;

					const Vector2 imagePointWz(camera.projectToImageIF<false>(poseWz.transformation(), objectPoint, camera.hasDistortionParameters(), zoom));
					if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[2], jacobianY[2]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtx
					Pose poseTx(poseIF);
					poseTx.x() += epsilon;

					const Vector2 imagePointTx(camera.projectToImageIF<false>(poseTx.transformation(), objectPoint, camera.hasDistortionParameters(), zoom));
					if (checkAccuracy(imagePoint, imagePointTx, epsilon, jacobianX[3], jacobianY[3]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dty
					Pose poseTy(poseIF);
					poseTy.y() += epsilon;

					const Vector2 imagePointTy(camera.projectToImageIF<false>(poseTy.transformation(), objectPoint, camera.hasDistortionParameters(), zoom));
					if (checkAccuracy(imagePoint, imagePointTy, epsilon, jacobianX[4], jacobianY[4]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtz
					Pose poseTz(poseIF);
					poseTz.z() += epsilon;

					const Vector2 imagePointTz(camera.projectToImageIF<false>(poseTz.transformation(), objectPoint, camera.hasDistortionParameters(), zoom));
					if (checkAccuracy(imagePoint, imagePointTz, epsilon, jacobianX[5], jacobianY[5]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / ds
					const Scalar dZoom = zoom + epsilon;

					const Vector2 imagePointZoom(camera.projectToImageIF<false>(poseIF.transformation(), objectPoint, camera.hasDistortionParameters(), dZoom));
					if (checkAccuracy(imagePoint, imagePointZoom, epsilon, jacobianX[6], jacobianY[6]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance perfect camera: " << performancePerfectCamera.averageMseconds() << "ms";
	Log::info() << "Performance distorted camera: " << performanceDistortedCamera.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold() * 0.975; // making threshold slightly weaker

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testPinholeCameraObjectTransformation2nx6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing pinhole camera object transformation Jacobian 2x6 and 2nx6 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 objectTranslation(Random::vector3(-5, 5));
		const Quaternion objectQuaternion(Random::quaternion());

		const Pose objectPose(objectTranslation, objectQuaternion);
		const HomogenousMatrix4 objectTransformation(objectTranslation, objectQuaternion);
		const HomogenousMatrix4 objectTransformationI(objectTransformation.inverted());

		const Vector3 extrinsicTranslation(Random::vector3(-5, 5));
		const Quaternion extrinsicQuaternion(Random::quaternion());

		const HomogenousMatrix4 extrinsic(extrinsicTranslation, extrinsicQuaternion);

		const HomogenousMatrix4 extrinsicIF(PinholeCamera::standard2InvertedFlipped(extrinsic));

		const PinholeCamera camera(width, height, fovX, principalX, principalY);

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			const Vector2 imagePoint(Random::vector2(Scalar(0), Scalar(camera.width()), Scalar(0), Scalar(camera.height())));

			const Line3 ray(camera.ray(imagePoint, extrinsic));
			const Vector3 objectPoint = objectTransformationI * ray.point(Random::scalar(1, 5));

			ocean_assert(camera.projectToImage<false>(extrinsic, objectTransformation * objectPoint, false).isEqual(imagePoint, 1));

			objectPoints.push_back(objectPoint);
		}

		/**
		 * jacobian for one point
		 * jacobian x: | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * jacobian y: | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 */

		Matrix jacobian(2 * objectPoints.size(), 6);

		performance.start();
		Geometry::Jacobian::calculateObjectTransformation2nx6(jacobian.data(), camera, extrinsicIF, objectPose, objectPoints.data(), objectPoints.size());
		performance.stop();

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 6);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(extrinsicIF, objectTransformation * objectPoint, false));

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					Pose objectPoseDelta(objectPose);
					objectPoseDelta[i] += Numeric::weakEps();

					const Vector2 imagePointDelta(camera.projectToImageIF<false>(extrinsicIF, objectPoseDelta.transformation() * objectPoint, false));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3 objectPoint = objectPoints[n];
			const Vector2 imagePoint(camera.projectToImageIF<false>(extrinsicIF, objectTransformation * objectPoint, false));

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			{
				// we also test the first implementation for one object point

				Scalar singleJacobianX[6], singleJacobianY[6];
				Geometry::Jacobian::calculateObjectTransformation2x6(singleJacobianX, singleJacobianY, camera, extrinsicIF, objectPose, objectPoint);

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						accurate = false;
					}
				}
			}

			{
				// we also test the second implementation for one object point

				SquareMatrix3 dwx, dwy, dwz;
				Geometry::Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(objectPose[3], objectPose[4], objectPose[5]), dwx, dwy, dwz);

				Scalar singleJacobianX[6], singleJacobianY[6];
				Geometry::Jacobian::calculateObjectTransformation2x6(singleJacobianX, singleJacobianY, camera, extrinsicIF, objectPose, objectPoint, dwx, dwy, dwz);

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						accurate = false;
					}
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwx
					Pose objectPoseWx(objectPose);
					objectPoseWx.rx() += epsilon;

					const Vector2 imagePointWx(camera.projectToImageIF<false>(extrinsicIF, objectPoseWx.transformation() * objectPoint, false));
					if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[0], jacobianY[0]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwy
					Pose objectPoseWy(objectPose);
					objectPoseWy.ry() += epsilon;

					const Vector2 imagePointWy(camera.projectToImageIF<false>(extrinsicIF, objectPoseWy.transformation() * objectPoint, false));
					if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[1], jacobianY[1]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwz
					Pose objectPoseWz(objectPose);
					objectPoseWz.rz() += epsilon;

					const Vector2 imagePointWz(camera.projectToImageIF<false>(extrinsicIF, objectPoseWz.transformation() * objectPoint, false));
					if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[2], jacobianY[2]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtx
					Pose objectPoseTx(objectPose);
					objectPoseTx.x() += epsilon;

					const Vector2 imagePointTx(camera.projectToImageIF<false>(extrinsicIF, objectPoseTx.transformation() * objectPoint, false));
					if (checkAccuracy(imagePoint, imagePointTx, epsilon, jacobianX[3], jacobianY[3]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dty
					Pose objectPoseTy(objectPose);
					objectPoseTy.y() += epsilon;

					const Vector2 imagePointTy(camera.projectToImageIF<false>(extrinsicIF, objectPoseTy.transformation() * objectPoint, false));
					if (checkAccuracy(imagePoint, imagePointTy, epsilon, jacobianX[4], jacobianY[4]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtz
					Pose objectPoseTz(objectPose);
					objectPoseTz.z() += epsilon;

					const Vector2 imagePointTz(camera.projectToImageIF<false>(extrinsicIF, objectPoseTz.transformation() * objectPoint, false));
					if (checkAccuracy(imagePoint, imagePointTz, epsilon, jacobianX[5], jacobianY[5]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testFisheyeCameraObjectTransformation2nx6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing fisheye camera object transformation Jacobian 2x6 and 2nx6 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		const FisheyeCamera fisheyeCamera = Utilities::realisticFisheyeCamera(RandomI::random(1u));

		const Vector3 objectTranslation(Random::vector3(-5, 5));
		const Quaternion objectQuaternion(Random::quaternion());

		const Pose objectPose(objectTranslation, objectQuaternion);
		const HomogenousMatrix4 objectTransformation(objectTranslation, objectQuaternion);
		const HomogenousMatrix4 objectTransformationI(objectTransformation.inverted());

		const Vector3 extrinsicTranslation(Random::vector3(-5, 5));
		const Quaternion extrinsicQuaternion(Random::quaternion());

		const HomogenousMatrix4 extrinsic(extrinsicTranslation, extrinsicQuaternion);

		const HomogenousMatrix4 extrinsicIF(PinholeCamera::standard2InvertedFlipped(extrinsic));

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			const Vector2 imagePoint(Random::vector2(Scalar(5), Scalar(fisheyeCamera.width() - 5u), Scalar(5), Scalar(fisheyeCamera.height() - 5u)));

			const Line3 ray(fisheyeCamera.ray(imagePoint, extrinsic));
			const Vector3 objectPoint = objectTransformationI * ray.point(Random::scalar(1, 5));

			ocean_assert(fisheyeCamera.projectToImage(extrinsic, objectTransformation * objectPoint).isEqual(imagePoint, 1));

			objectPoints.push_back(objectPoint);
		}

		/**
		 * jacobian for one point
		 * jacobian x: | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * jacobian y: | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 */

		Matrix jacobian(2 * objectPoints.size(), 6);

		performance.start();
		Geometry::Jacobian::calculateObjectTransformation2nx6(jacobian.data(), fisheyeCamera, extrinsicIF, objectPose, objectPoints.data(), objectPoints.size());
		performance.stop();

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 6);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(fisheyeCamera.projectToImageIF(extrinsicIF, objectTransformation * objectPoint));

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					Pose objectPoseDelta(objectPose);
					objectPoseDelta[i] += Numeric::weakEps();

					const Vector2 imagePointDelta(fisheyeCamera.projectToImageIF(extrinsicIF, objectPoseDelta.transformation() * objectPoint));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3 objectPoint = objectPoints[n];
			const Vector2 imagePoint(fisheyeCamera.projectToImageIF(extrinsicIF, objectTransformation * objectPoint));

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwx
					Pose objectPoseWx(objectPose);
					objectPoseWx.rx() += epsilon;

					const Vector2 imagePointWx(fisheyeCamera.projectToImageIF(extrinsicIF, objectPoseWx.transformation() * objectPoint));
					if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[0], jacobianY[0]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwy
					Pose objectPoseWy(objectPose);
					objectPoseWy.ry() += epsilon;

					const Vector2 imagePointWy(fisheyeCamera.projectToImageIF(extrinsicIF, objectPoseWy.transformation() * objectPoint));
					if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[1], jacobianY[1]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwz
					Pose objectPoseWz(objectPose);
					objectPoseWz.rz() += epsilon;

					const Vector2 imagePointWz(fisheyeCamera.projectToImageIF(extrinsicIF, objectPoseWz.transformation() * objectPoint));
					if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[2], jacobianY[2]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtx
					Pose objectPoseTx(objectPose);
					objectPoseTx.x() += epsilon;

					const Vector2 imagePointTx(fisheyeCamera.projectToImageIF(extrinsicIF, objectPoseTx.transformation() * objectPoint));
					if (checkAccuracy(imagePoint, imagePointTx, epsilon, jacobianX[3], jacobianY[3]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dty
					Pose objectPoseTy(objectPose);
					objectPoseTy.y() += epsilon;

					const Vector2 imagePointTy(fisheyeCamera.projectToImageIF(extrinsicIF, objectPoseTy.transformation() * objectPoint));
					if (checkAccuracy(imagePoint, imagePointTy, epsilon, jacobianX[4], jacobianY[4]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtz
					Pose objectPoseTz(objectPose);
					objectPoseTz.z() += epsilon;

					const Vector2 imagePointTz(fisheyeCamera.projectToImageIF(extrinsicIF, objectPoseTz.transformation() * objectPoint));
					if (checkAccuracy(imagePoint, imagePointTz, epsilon, jacobianX[5], jacobianY[5]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testPinholeCameraPointJacobian2nx3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing pinhole camera point Jacobian 2x3 and 2nx3 for " << numberPoints << " points:";

	const Scalar eps = Numeric::weakEps();

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;
	unsigned int distortionIteration = 0u;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performancePerfectCamera;
	HighPerformanceStatistic performanceDistortedCamera;

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 translation(Random::scalar(-1, 1), Random::scalar(-1, 1), Random::scalar(-1, 1));
		const Quaternion rotation(Random::quaternion());

		const HomogenousMatrix4 transformation(translation, rotation);
		const HomogenousMatrix4 iFlippedTransformation(PinholeCamera::standard2InvertedFlipped(transformation));

		PinholeCamera camera(width, height, fovX, principalX, principalY);

		if (distortionIteration % 3u == 1u || distortionIteration % 3u == 2u)
		{
			const Scalar k1 = Random::scalar(Scalar(-0.2), Scalar(0.2));
			const Scalar k2 = Random::scalar(Scalar(-0.2), Scalar(0.2));
			camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		}

		if (distortionIteration % 3u == 2u)
		{
			const Scalar p1 = Random::scalar(Scalar(-0.1), Scalar(0.1));
			const Scalar p2 = Random::scalar(Scalar(-0.1), Scalar(0.1));
			camera.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));
		}

		distortionIteration++;

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);

		while (objectPoints.size() < numberPoints)
		{
			const Vector2 imagePoint(Random::scalar(Scalar(0u), Scalar(camera.width() - 1u)), Random::scalar(Scalar(0u), Scalar(camera.height() - 1u)));
			const Line3 ray(camera.ray(camera.undistort<true>(imagePoint), transformation));
			const Vector3 objectPoint(ray.point(Random::scalar(1, 100)));

			objectPoints.push_back(objectPoint);
		}

		Matrix jacobian(objectPoints.size() * 2, 3);

		if (camera.hasDistortionParameters())
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceDistortedCamera);
			Geometry::Jacobian::calculatePointJacobian2nx3(jacobian.data(), camera, iFlippedTransformation, objectPoints.data(), objectPoints.size(), true);
		}
		else
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performancePerfectCamera);
			Geometry::Jacobian::calculatePointJacobian2nx3(jacobian.data(), camera, iFlippedTransformation, objectPoints.data(), objectPoints.size(), false);
		}

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 3);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(iFlippedTransformation, objectPoint, camera.hasDistortionParameters()));

				for (unsigned int i = 0u; i < 3u; ++i)
				{
					Vector3 objectPointDelta(objectPoint);
					objectPointDelta[i] += eps;

					const Vector2 imagePointDelta(camera.projectToImageIF<false>(iFlippedTransformation, objectPointDelta, camera.hasDistortionParameters()));
					const Vector2 derivative = (imagePointDelta - imagePoint) / eps;

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3 objectPoint = objectPoints[n];
			const Vector2 imagePoint(camera.projectToImageIF<false>(iFlippedTransformation, objectPoint, camera.hasDistortionParameters()));

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			Scalar singleJacobianX[3], singleJacobianY[3];
			Geometry::Jacobian::calculatePointJacobian2x3(singleJacobianX, singleJacobianY, camera, iFlippedTransformation, objectPoint, camera.hasDistortionParameters());

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
				ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

				if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
				{
					accurate = false;
				}
			}

			{
				// df / dox
				const Vector3 objectPointX(objectPoint.x() + eps, objectPoint.y(), objectPoint.z());
				const Vector2 imagePointX(camera.projectToImageIF<false>(iFlippedTransformation, objectPointX, camera.hasDistortionParameters()));

				const Scalar doxx = (imagePointX.x() - imagePoint.x()) / eps;
				const Scalar doxy = (imagePointX.y() - imagePoint.y()) / eps;

				const Scalar diffOxx = Numeric::abs(jacobianX[0] - doxx);
				const Scalar maxOxx = max(Numeric::abs(jacobianX[0]), Numeric::abs(doxx));

				if (Numeric::isNotEqualEps(maxOxx) && diffOxx / maxOxx > 0.05)
				{
					accurate = false;
				}

				const Scalar diffOxy = Numeric::abs(jacobianY[0] - doxy);
				const Scalar maxOxy = max(Numeric::abs(jacobianY[0]), Numeric::abs(doxy));

				if (Numeric::isNotEqualEps(maxOxy) && diffOxy / maxOxy > 0.05)
				{
					accurate = false;
				}
			}

			{
				// df / doy
				const Vector3 objectPointY(objectPoint.x(), objectPoint.y() + eps, objectPoint.z());
				const Vector2 imagePointY(camera.projectToImageIF<false>(iFlippedTransformation, objectPointY, camera.hasDistortionParameters()));

				const Scalar doyx = (imagePointY.x() - imagePoint.x()) / eps;
				const Scalar doyy = (imagePointY.y() - imagePoint.y()) / eps;

				const Scalar diffOyx = Numeric::abs(jacobianX[1] - doyx);
				const Scalar maxOyx = max(Numeric::abs(jacobianX[1]), Numeric::abs(doyx));

				if (Numeric::isNotEqualEps(maxOyx) && diffOyx / maxOyx > 0.05)
				{
					accurate = false;
				}

				const Scalar diffOyy = Numeric::abs(jacobianY[1] - doyy);
				const Scalar maxOyy = max(Numeric::abs(jacobianY[1]), Numeric::abs(doyy));

				if (Numeric::isNotEqualEps(maxOyy) && diffOyy / maxOyy > 0.05)
				{
					accurate = false;
				}
			}

			{
				// df / doz
				const Vector3 objectPointZ(objectPoint.x(), objectPoint.y(), objectPoint.z() + eps);
				const Vector2 imagePointZ(camera.projectToImageIF<false>(iFlippedTransformation, objectPointZ, camera.hasDistortionParameters()));

				const Scalar dozx = (imagePointZ.x() - imagePoint.x()) / eps;
				const Scalar dozy = (imagePointZ.y() - imagePoint.y()) / eps;

				const Scalar diffOzx = Numeric::abs(jacobianX[2] - dozx);
				const Scalar maxOzx = max(Numeric::abs(jacobianX[2]), Numeric::abs(dozx));

				if (Numeric::isNotEqualEps(maxOzx) && diffOzx / maxOzx > 0.05)
				{
					accurate = false;
				}

				const Scalar diffOzy = Numeric::abs(jacobianY[2] - dozy);
				const Scalar maxOzy = max(Numeric::abs(jacobianY[2]), Numeric::abs(dozy));

				if (Numeric::isNotEqualEps(maxOzy) && diffOzy / maxOzy > 0.05)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance perfect camera: " << performancePerfectCamera.averageMseconds() << "ms";
	Log::info() << "Performance distorted camera: " << performanceDistortedCamera.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testFisheyeCameraPointJacobian2x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing fisheye camera point Jacobian 2x3 for " << numberPoints << " points:";

	const Scalar eps = Numeric::weakEps();

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceOptimized;

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		const FisheyeCamera fisheyeCamera = Utilities::realisticFisheyeCamera(RandomI::random(1u));

		const Vector3 translation(Random::scalar(-1, 1), Random::scalar(-1, 1), Random::scalar(-1, 1));
		const Quaternion rotation(Random::quaternion());

		const HomogenousMatrix4 world_T_camera(translation, rotation);
		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);

		while (objectPoints.size() < numberPoints)
		{
			const Vector2 imagePoint = Random::vector2(Scalar(5u), Scalar(fisheyeCamera.width() - 5u), Scalar(5u), Scalar(fisheyeCamera.height() - 5u));
			const Line3 ray(fisheyeCamera.ray(imagePoint, world_T_camera));
			const Vector3 objectPoint(ray.point(Random::scalar(1, 10)));

			objectPoints.push_back(objectPoint);
		}

		Matrix jacobian(objectPoints.size() * 2, 3);

		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceOptimized);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				Geometry::Jacobian::calculatePointJacobian2x3(jacobian[n * 2 + 0], jacobian[n * 2 + 1], fisheyeCamera, flippedCamera_T_world, objectPoints[n]);
			}
		}

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 3);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(fisheyeCamera.projectToImageIF(flippedCamera_T_world, objectPoint));

				for (unsigned int i = 0u; i < 3u; ++i)
				{
					Vector3 objectPointDelta(objectPoint);
					objectPointDelta[i] += eps;

					const Vector2 imagePointDelta(fisheyeCamera.projectToImageIF(flippedCamera_T_world, objectPointDelta));
					const Vector2 derivative = (imagePointDelta - imagePoint) / eps;

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3 objectPoint = objectPoints[n];
			const Vector2 imagePoint(fisheyeCamera.projectToImageIF(flippedCamera_T_world, objectPoint));

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			Scalar singleJacobianX[3], singleJacobianY[3];
			Geometry::Jacobian::calculatePointJacobian2x3(singleJacobianX, singleJacobianY, fisheyeCamera, flippedCamera_T_world, objectPoint);

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
				ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

				if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
				{
					accurate = false;
				}
			}

			{
				// df / dox
				const Vector3 objectPointX(objectPoint.x() + eps, objectPoint.y(), objectPoint.z());
				const Vector2 imagePointX(fisheyeCamera.projectToImageIF(flippedCamera_T_world, objectPointX));

				const Scalar doxx = (imagePointX.x() - imagePoint.x()) / eps;
				const Scalar doxy = (imagePointX.y() - imagePoint.y()) / eps;

				const Scalar diffOxx = Numeric::abs(jacobianX[0] - doxx);
				const Scalar maxOxx = max(Numeric::abs(jacobianX[0]), Numeric::abs(doxx));

				if (Numeric::isNotEqualEps(maxOxx) && diffOxx / maxOxx > 0.05)
				{
					accurate = false;
				}

				const Scalar diffOxy = Numeric::abs(jacobianY[0] - doxy);
				const Scalar maxOxy = max(Numeric::abs(jacobianY[0]), Numeric::abs(doxy));

				if (Numeric::isNotEqualEps(maxOxy) && diffOxy / maxOxy > 0.05)
				{
					accurate = false;
				}
			}

			{
				// df / doy
				const Vector3 objectPointY(objectPoint.x(), objectPoint.y() + eps, objectPoint.z());
				const Vector2 imagePointY(fisheyeCamera.projectToImageIF(flippedCamera_T_world, objectPointY));

				const Scalar doyx = (imagePointY.x() - imagePoint.x()) / eps;
				const Scalar doyy = (imagePointY.y() - imagePoint.y()) / eps;

				const Scalar diffOyx = Numeric::abs(jacobianX[1] - doyx);
				const Scalar maxOyx = max(Numeric::abs(jacobianX[1]), Numeric::abs(doyx));

				if (Numeric::isNotEqualEps(maxOyx) && diffOyx / maxOyx > 0.05)
				{
					accurate = false;
				}

				const Scalar diffOyy = Numeric::abs(jacobianY[1] - doyy);
				const Scalar maxOyy = max(Numeric::abs(jacobianY[1]), Numeric::abs(doyy));

				if (Numeric::isNotEqualEps(maxOyy) && diffOyy / maxOyy > 0.05)
				{
					accurate = false;
				}
			}

			{
				// df / doz
				const Vector3 objectPointZ(objectPoint.x(), objectPoint.y(), objectPoint.z() + eps);
				const Vector2 imagePointZ(fisheyeCamera.projectToImageIF(flippedCamera_T_world, objectPointZ));

				const Scalar dozx = (imagePointZ.x() - imagePoint.x()) / eps;
				const Scalar dozy = (imagePointZ.y() - imagePoint.y()) / eps;

				const Scalar diffOzx = Numeric::abs(jacobianX[2] - dozx);
				const Scalar maxOzx = max(Numeric::abs(jacobianX[2]), Numeric::abs(dozx));

				if (Numeric::isNotEqualEps(maxOzx) && diffOzx / maxOzx > 0.05)
				{
					accurate = false;
				}

				const Scalar diffOzy = Numeric::abs(jacobianY[2] - dozy);
				const Scalar maxOzy = max(Numeric::abs(jacobianY[2]), Numeric::abs(dozy));

				if (Numeric::isNotEqualEps(maxOzy) && diffOzy / maxOzy > 0.05)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance optimized: " << performanceOptimized.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testAnyCameraPointJacobian2x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing any camera point Jacobian 2x3 for " << numberPoints << " points:";

	const Scalar eps = Numeric::weakEps();

	bool firstCameraIteration = true;

	bool allSucceeded = true;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		Log::info().newLine(!firstCameraIteration);
		firstCameraIteration = false;

		const std::shared_ptr<AnyCamera> anyCameraShared = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		ocean_assert(anyCameraShared);

		const AnyCamera& anyCamera = *anyCameraShared;

		uint64_t succeeded = 0ull;
		uint64_t iterations = 0ull;

		HighPerformanceStatistic performanceNaive;
		HighPerformanceStatistic performanceOptimized;

		const Timestamp startTimestamp(true);

		do
		{
			bool accurate = true;

			const Vector3 translation(Random::scalar(-1, 1), Random::scalar(-1, 1), Random::scalar(-1, 1));
			const Quaternion rotation(Random::quaternion());

			const HomogenousMatrix4 world_T_camera(translation, rotation);
			const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

			Vectors3 objectPoints;
			objectPoints.reserve(numberPoints);

			while (objectPoints.size() < numberPoints)
			{
				const Vector2 imagePoint = Random::vector2(Scalar(5u), Scalar(anyCamera.width() - 5u), Scalar(5u), Scalar(anyCamera.height() - 5u));
				const Line3 ray(anyCamera.ray(imagePoint, world_T_camera));
				const Vector3 objectPoint(ray.point(Random::scalar(1, 10)));

				objectPoints.push_back(objectPoint);
			}

			Matrix jacobian(objectPoints.size() * 2, 3);

			{
				const HighPerformanceStatistic::ScopedStatistic scope(performanceOptimized);

				for (size_t n = 0; n < objectPoints.size(); ++n)
				{
					Geometry::Jacobian::calculatePointJacobian2x3IF(anyCamera, flippedCamera_T_world, objectPoints[n], jacobian[n * 2 + 0], jacobian[n * 2 + 1]);
				}
			}

			{
				Matrix naiveJacobian(2 * objectPoints.size(), 3);

				const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

				for (size_t n = 0; n < objectPoints.size(); ++n)
				{
					const Vector3 objectPoint = objectPoints[n];
					const Vector2 imagePoint(anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint));

					for (unsigned int i = 0u; i < 3u; ++i)
					{
						Vector3 objectPointDelta(objectPoint);
						objectPointDelta[i] += eps;

						const Vector2 imagePointDelta(anyCamera.projectToImageIF(flippedCamera_T_world, objectPointDelta));
						const Vector2 derivative = (imagePointDelta - imagePoint) / eps;

						naiveJacobian[n * 2 + 0][i] = derivative.x();
						naiveJacobian[n * 2 + 1][i] = derivative.y();
					}
				}
			}

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint));

				const Scalar* jacobianX = jacobian[2 * n + 0];
				const Scalar* jacobianY = jacobian[2 * n + 1];

				Scalar singleJacobianX[3], singleJacobianY[3];
				Geometry::Jacobian::calculatePointJacobian2x3IF(anyCamera, flippedCamera_T_world, objectPoint, singleJacobianX, singleJacobianY);

				for (unsigned int i = 0u; i < 3u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						accurate = false;
					}
				}

				{
					// df / dox
					const Vector3 objectPointX(objectPoint.x() + eps, objectPoint.y(), objectPoint.z());
					const Vector2 imagePointX(anyCamera.projectToImageIF(flippedCamera_T_world, objectPointX));

					const Scalar doxx = (imagePointX.x() - imagePoint.x()) / eps;
					const Scalar doxy = (imagePointX.y() - imagePoint.y()) / eps;

					const Scalar diffOxx = Numeric::abs(jacobianX[0] - doxx);
					const Scalar maxOxx = max(Numeric::abs(jacobianX[0]), Numeric::abs(doxx));

					if (Numeric::isNotEqualEps(maxOxx) && diffOxx / maxOxx > 0.05)
					{
						accurate = false;
					}

					const Scalar diffOxy = Numeric::abs(jacobianY[0] - doxy);
					const Scalar maxOxy = max(Numeric::abs(jacobianY[0]), Numeric::abs(doxy));

					if (Numeric::isNotEqualEps(maxOxy) && diffOxy / maxOxy > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / doy
					const Vector3 objectPointY(objectPoint.x(), objectPoint.y() + eps, objectPoint.z());
					const Vector2 imagePointY(anyCamera.projectToImageIF(flippedCamera_T_world, objectPointY));

					const Scalar doyx = (imagePointY.x() - imagePoint.x()) / eps;
					const Scalar doyy = (imagePointY.y() - imagePoint.y()) / eps;

					const Scalar diffOyx = Numeric::abs(jacobianX[1] - doyx);
					const Scalar maxOyx = max(Numeric::abs(jacobianX[1]), Numeric::abs(doyx));

					if (Numeric::isNotEqualEps(maxOyx) && diffOyx / maxOyx > 0.05)
					{
						accurate = false;
					}

					const Scalar diffOyy = Numeric::abs(jacobianY[1] - doyy);
					const Scalar maxOyy = max(Numeric::abs(jacobianY[1]), Numeric::abs(doyy));

					if (Numeric::isNotEqualEps(maxOyy) && diffOyy / maxOyy > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / doz
					const Vector3 objectPointZ(objectPoint.x(), objectPoint.y(), objectPoint.z() + eps);
					const Vector2 imagePointZ(anyCamera.projectToImageIF(flippedCamera_T_world, objectPointZ));

					const Scalar dozx = (imagePointZ.x() - imagePoint.x()) / eps;
					const Scalar dozy = (imagePointZ.y() - imagePoint.y()) / eps;

					const Scalar diffOzx = Numeric::abs(jacobianX[2] - dozx);
					const Scalar maxOzx = max(Numeric::abs(jacobianX[2]), Numeric::abs(dozx));

					if (Numeric::isNotEqualEps(maxOzx) && diffOzx / maxOzx > 0.05)
					{
						accurate = false;
					}

					const Scalar diffOzy = Numeric::abs(jacobianY[2] - dozy);
					const Scalar maxOzy = max(Numeric::abs(jacobianY[2]), Numeric::abs(dozy));

					if (Numeric::isNotEqualEps(maxOzy) && diffOzy / maxOzy > 0.05)
					{
						accurate = false;
					}
				}

				if (accurate)
				{
					++succeeded;
				}

				++iterations;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(succeeded) / double(iterations);

		Log::info() << "Camera name: " << anyCamera.name();
		Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
		Log::info() << "Performance optimized: " << performanceOptimized.averageMseconds() << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		if (percent < successThreshold())
		{
			allSucceeded = false;
		}
	}

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testPosesPointsJacobian2nx12(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing poses jacobian rodrigues 2nx12 for several points:";

	const Scalar eps = Numeric::weakEps();

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	const Timestamp startTimestamp(true);

	const unsigned int numberPoints = 37u;
	unsigned int distortionIteration = 0u;

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		PinholeCamera camera(width, height, fovX, principalX, principalY);

		if (distortionIteration % 3u == 1u || distortionIteration % 3u == 2u)
		{
			const Scalar k1 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			const Scalar k2 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		}

		if (distortionIteration % 3u == 2u)
		{
			const Scalar p1 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			const Scalar p2 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			camera.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));
		}

		distortionIteration++;

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			objectPoints.push_back(Vector3(Random::scalar(-5, 5), 0, Random::scalar(-5, 5)));
		}

		const HomogenousMatrix4 transformationFirst(Utilities::viewPosition(camera, objectPoints));
		const HomogenousMatrix4 transformationSecond(Utilities::viewPosition(camera, objectPoints));

		const HomogenousMatrix4 transformationFirstIF(PinholeCamera::standard2InvertedFlipped(transformationFirst));
		const Pose poseFirstIF(transformationFirstIF);

		const HomogenousMatrix4 transformationSecondIF(PinholeCamera::standard2InvertedFlipped(transformationSecond));
		const Pose poseSecondIF(transformationSecondIF);

		/**
		 * jacobian for two poses and one point
		 * jacobian x: | dfx / dwx1, dfx / dwy1, dfx / dwz1, dfx / dtx1, dfx / dty1, dfx / dtz1,    dfx / dwx2, dfx / dwy2, dfx / dwz2, dfx / dtx2, dfx / dty2, dfx / dtz2,    dfx / dpx, dfx / dpy, dfx / dpz |
		 * jacobian y: | dfy / dwx1, dfy / dwy1, dfy / dwz1, dfy / dtx1, dfy / dty1, dfy / dtz1,    dfy / dwx2, dfy / dwy2, dfy / dwz2, dfy / dtx2, dfy / dty2, dfy / dtz2,    dfy / dpx, dfy / dpy, dfy / dpz |
		 */

		Scalars pointJacobians(numberPoints * 3u * 2u * 2u);
		Scalar* pointJacobiansFirstPose = pointJacobians.data() + numberPoints * 0u;
		Scalar* pointJacobiansSecondPose = pointJacobians.data() + numberPoints * 6u;

		Scalars poseJacobians(numberPoints * 6u * 2u * 2u);
		Scalar* poseJacobiansFirst = &poseJacobians[0] + numberPoints * 0u;
		Scalar* poseJacobiansSecond = &poseJacobians[0] + numberPoints * 12u;

		Geometry::Jacobian::calculatePointJacobian2nx3(pointJacobiansFirstPose, camera, transformationFirstIF, objectPoints.data(), objectPoints.size(), camera.hasDistortionParameters());
		Geometry::Jacobian::calculatePointJacobian2nx3(pointJacobiansSecondPose, camera, transformationSecondIF, objectPoints.data(), objectPoints.size(), camera.hasDistortionParameters());

		Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6(poseJacobiansFirst, camera, Pose(transformationFirstIF), objectPoints.data(), objectPoints.size(), camera.hasDistortionParameters());
		Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6(poseJacobiansSecond, camera, Pose(transformationSecondIF), objectPoints.data(), objectPoints.size(), camera.hasDistortionParameters());

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			// first pose
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<true>(transformationFirstIF, objectPoint, camera.hasDistortionParameters()));

				const Scalar* jacobianX = poseJacobiansFirst + 12u * (unsigned int)n + 0u;
				const Scalar* jacobianY = jacobianX + 6;

				{
					// df / dwx
					Pose poseWx(poseFirstIF);
					poseWx.rx() += eps;

					const Vector2 imagePointWx(camera.projectToImageIF<true>(poseWx.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dWx_x = (imagePointWx.x() - imagePoint.x()) / eps;
					const Scalar dWx_y = (imagePointWx.y() - imagePoint.y()) / eps;

					const Scalar diffWx_x = Numeric::abs(jacobianX[0] - dWx_x);
					const Scalar maxWx_x = max(Numeric::abs(jacobianX[0]), Numeric::abs(dWx_x));

					if (Numeric::isNotEqualEps(maxWx_x) && diffWx_x / maxWx_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffWx_y = Numeric::abs(jacobianY[0] - dWx_y);
					const Scalar maxWx_y = max(Numeric::abs(jacobianY[0]), Numeric::abs(dWx_y));

					if (Numeric::isNotEqualEps(maxWx_y) && diffWx_y / maxWx_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dwy
					Pose poseWy(poseFirstIF);
					poseWy.ry() += eps;

					const Vector2 imagePointWy(camera.projectToImageIF<true>(poseWy.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dWy_x = (imagePointWy.x() - imagePoint.x()) / eps;
					const Scalar dWy_y = (imagePointWy.y() - imagePoint.y()) / eps;

					const Scalar diffWy_x = Numeric::abs(jacobianX[1] - dWy_x);
					const Scalar maxWy_x = max(Numeric::abs(jacobianX[1]), Numeric::abs(dWy_x));

					if (Numeric::isNotEqualEps(maxWy_x) && diffWy_x / maxWy_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffWy_y = Numeric::abs(jacobianY[1] - dWy_y);
					const Scalar maxWy_y = max(Numeric::abs(jacobianY[1]), Numeric::abs(dWy_y));

					if (Numeric::isNotEqualEps(maxWy_y) && diffWy_y / maxWy_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dwz
					Pose poseWz(poseFirstIF);
					poseWz.rz() += eps;

					const Vector2 imagePointWz(camera.projectToImageIF<true>(poseWz.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dWz_x = (imagePointWz.x() - imagePoint.x()) / eps;
					const Scalar dWz_y = (imagePointWz.y() - imagePoint.y()) / eps;

					const Scalar diffWz_x = Numeric::abs(jacobianX[2] - dWz_x);
					const Scalar maxWz_x = max(Numeric::abs(jacobianX[2]), Numeric::abs(dWz_x));

					if (Numeric::isNotEqualEps(maxWz_x) && diffWz_x / maxWz_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffWz_y = Numeric::abs(jacobianY[2] - dWz_y);
					const Scalar maxWz_y = max(Numeric::abs(jacobianY[2]), Numeric::abs(dWz_y));

					if (Numeric::isNotEqualEps(maxWz_y) && diffWz_y / maxWz_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dtx
					Pose poseTx(poseFirstIF);
					poseTx.x() += eps;

					const Vector2 imagePointTx(camera.projectToImageIF<true>(poseTx.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dTx_x = (imagePointTx.x() - imagePoint.x()) / eps;
					const Scalar dTx_y = (imagePointTx.y() - imagePoint.y()) / eps;

					const Scalar diffTx_x = Numeric::abs(jacobianX[3] - dTx_x);
					const Scalar maxTx_x = max(Numeric::abs(jacobianX[3]), Numeric::abs(dTx_x));

					if (Numeric::isNotEqualEps(maxTx_x) && diffTx_x / maxTx_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffTx_y = Numeric::abs(jacobianY[3] - dTx_y);
					const Scalar maxTx_y = max(Numeric::abs(jacobianY[3]), Numeric::abs(dTx_y));

					if ((jacobianY[3] != 0 && Numeric::isNotEqualEps(maxTx_y) && diffTx_y / maxTx_y > 0.05) || (jacobianY[3] == 0 && Numeric::abs(dTx_y) > 0.001))
					{
						accurate = false;
					}
				}

				{
					// df / dty
					Pose poseTy(poseFirstIF);
					poseTy.y() += eps;

					const Vector2 imagePointTy(camera.projectToImageIF<true>(poseTy.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dTy_x = (imagePointTy.x() - imagePoint.x()) / eps;
					const Scalar dTy_y = (imagePointTy.y() - imagePoint.y()) / eps;

					const Scalar diffTy_x = Numeric::abs(jacobianX[4] - dTy_x);
					const Scalar maxTy_x = max(Numeric::abs(jacobianX[4]), Numeric::abs(dTy_x));

					if ((jacobianX[4] != 0 && Numeric::isNotEqualEps(maxTy_x) && diffTy_x / maxTy_x > 0.05) || (jacobianX[4] == 0 && Numeric::abs(dTy_x) > 0.001))
					{
						accurate = false;
					}

					const Scalar diffTy_y = Numeric::abs(jacobianY[4] - dTy_y);
					const Scalar maxTy_y = max(Numeric::abs(jacobianY[4]), Numeric::abs(dTy_y));

					if (Numeric::isNotEqualEps(maxTy_y) && diffTy_y / maxTy_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dtz
					Pose poseTz(poseFirstIF);
					poseTz.z() += eps;

					const Vector2 imagePointTz(camera.projectToImageIF<true>(poseTz.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dTz_x = (imagePointTz.x() - imagePoint.x()) / eps;
					const Scalar dTz_y = (imagePointTz.y() - imagePoint.y()) / eps;

					const Scalar diffTz_x = Numeric::abs(jacobianX[5] - dTz_x);
					const Scalar maxTz_x = max(Numeric::abs(jacobianX[5]), Numeric::abs(dTz_x));

					if (Numeric::isNotEqualEps(maxTz_x) && diffTz_x / maxTz_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffTz_y = Numeric::abs(jacobianY[5] - dTz_y);
					const Scalar maxTz_y = max(Numeric::abs(jacobianY[5]), Numeric::abs(dTz_y));

					if (Numeric::isNotEqualEps(maxTz_y) && diffTz_y / maxTz_y > 0.05)
					{
						accurate = false;
					}
				}
			}



			// second pose
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<true>(transformationSecondIF, objectPoint, camera.hasDistortionParameters()));

				const Scalar* jacobianX = poseJacobiansSecond + 12u * (unsigned int)n + 0u;
				const Scalar* jacobianY = jacobianX + 6;

				{
					// df / dwx
					Pose poseWx(poseSecondIF);
					poseWx.rx() += eps;

					const Vector2 imagePointWx(camera.projectToImageIF<true>(poseWx.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dWx_x = (imagePointWx.x() - imagePoint.x()) / eps;
					const Scalar dWx_y = (imagePointWx.y() - imagePoint.y()) / eps;

					const Scalar diffWx_x = Numeric::abs(jacobianX[0] - dWx_x);
					const Scalar maxWx_x = max(Numeric::abs(jacobianX[0]), Numeric::abs(dWx_x));

					if (Numeric::isNotEqualEps(maxWx_x) && diffWx_x / maxWx_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffWx_y = Numeric::abs(jacobianY[0] - dWx_y);
					const Scalar maxWx_y = max(Numeric::abs(jacobianY[0]), Numeric::abs(dWx_y));

					if (Numeric::isNotEqualEps(maxWx_y) && diffWx_y / maxWx_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dwy
					Pose poseWy(poseSecondIF);
					poseWy.ry() += eps;

					const Vector2 imagePointWy(camera.projectToImageIF<true>(poseWy.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dWy_x = (imagePointWy.x() - imagePoint.x()) / eps;
					const Scalar dWy_y = (imagePointWy.y() - imagePoint.y()) / eps;

					const Scalar diffWy_x = Numeric::abs(jacobianX[1] - dWy_x);
					const Scalar maxWy_x = max(Numeric::abs(jacobianX[1]), Numeric::abs(dWy_x));

					if (Numeric::isNotEqualEps(maxWy_x) && diffWy_x / maxWy_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffWy_y = Numeric::abs(jacobianY[1] - dWy_y);
					const Scalar maxWy_y = max(Numeric::abs(jacobianY[1]), Numeric::abs(dWy_y));

					if (Numeric::isNotEqualEps(maxWy_y) && diffWy_y / maxWy_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dwz
					Pose poseWz(poseSecondIF);
					poseWz.rz() += eps;

					const Vector2 imagePointWz(camera.projectToImageIF<true>(poseWz.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dWz_x = (imagePointWz.x() - imagePoint.x()) / eps;
					const Scalar dWz_y = (imagePointWz.y() - imagePoint.y()) / eps;

					const Scalar diffWz_x = Numeric::abs(jacobianX[2] - dWz_x);
					const Scalar maxWz_x = max(Numeric::abs(jacobianX[2]), Numeric::abs(dWz_x));

					if (Numeric::isNotEqualEps(maxWz_x) && diffWz_x / maxWz_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffWz_y = Numeric::abs(jacobianY[2] - dWz_y);
					const Scalar maxWz_y = max(Numeric::abs(jacobianY[2]), Numeric::abs(dWz_y));

					if (Numeric::isNotEqualEps(maxWz_y) && diffWz_y / maxWz_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dtx
					Pose poseTx(poseSecondIF);
					poseTx.x() += eps;

					const Vector2 imagePointTx(camera.projectToImageIF<true>(poseTx.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dTx_x = (imagePointTx.x() - imagePoint.x()) / eps;
					const Scalar dTx_y = (imagePointTx.y() - imagePoint.y()) / eps;

					const Scalar diffTx_x = Numeric::abs(jacobianX[3] - dTx_x);
					const Scalar maxTx_x = max(Numeric::abs(jacobianX[3]), Numeric::abs(dTx_x));

					if (Numeric::isNotEqualEps(maxTx_x) && diffTx_x / maxTx_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffTx_y = Numeric::abs(jacobianY[3] - dTx_y);
					const Scalar maxTx_y = max(Numeric::abs(jacobianY[3]), Numeric::abs(dTx_y));

					if ((jacobianY[3] != 0 && Numeric::isNotEqualEps(maxTx_y) && diffTx_y / maxTx_y > 0.05) || (jacobianY[3] == 0 && Numeric::abs(dTx_y) > 0.001))
					{
						accurate = false;
					}
				}

				{
					// df / dty
					Pose poseTy(poseSecondIF);
					poseTy.y() += eps;

					const Vector2 imagePointTy(camera.projectToImageIF<true>(poseTy.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dTy_x = (imagePointTy.x() - imagePoint.x()) / eps;
					const Scalar dTy_y = (imagePointTy.y() - imagePoint.y()) / eps;

					const Scalar diffTy_x = Numeric::abs(jacobianX[4] - dTy_x);
					const Scalar maxTy_x = max(Numeric::abs(jacobianX[4]), Numeric::abs(dTy_x));

					if ((jacobianX[4] != 0 && Numeric::isNotEqualEps(maxTy_x) && diffTy_x / maxTy_x > 0.05) || (jacobianX[4] == 0 && Numeric::abs(dTy_x) > 0.001))
					{
						accurate = false;
					}

					const Scalar diffTy_y = Numeric::abs(jacobianY[4] - dTy_y);
					const Scalar maxTy_y = max(Numeric::abs(jacobianY[4]), Numeric::abs(dTy_y));

					if (Numeric::isNotEqualEps(maxTy_y) && diffTy_y / maxTy_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dtz
					Pose poseTz(poseSecondIF);
					poseTz.z() += eps;

					const Vector2 imagePointTz(camera.projectToImageIF<true>(poseTz.transformation(), objectPoint, camera.hasDistortionParameters()));

					const Scalar dTz_x = (imagePointTz.x() - imagePoint.x()) / eps;
					const Scalar dTz_y = (imagePointTz.y() - imagePoint.y()) / eps;

					const Scalar diffTz_x = Numeric::abs(jacobianX[5] - dTz_x);
					const Scalar maxTz_x = max(Numeric::abs(jacobianX[5]), Numeric::abs(dTz_x));

					if (Numeric::isNotEqualEps(maxTz_x) && diffTz_x / maxTz_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffTz_y = Numeric::abs(jacobianY[5] - dTz_y);
					const Scalar maxTz_y = max(Numeric::abs(jacobianY[5]), Numeric::abs(dTz_y));

					if (Numeric::isNotEqualEps(maxTz_y) && diffTz_y / maxTz_y > 0.05)
					{
						accurate = false;
					}
				}
			}



			// first point
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<true>(transformationFirstIF, objectPoint, camera.hasDistortionParameters()));

				const Scalar* jacobianX = pointJacobiansFirstPose + 6u * (unsigned int)n + 0u;
				const Scalar* jacobianY = jacobianX + 3;

				{
					// df / dpx
					Vector3 objectPointX(objectPoint);
					objectPointX.x() += eps;

					const Vector2 imagePointX(camera.projectToImageIF<true>(transformationFirstIF, objectPointX, camera.hasDistortionParameters()));

					const Scalar dX_x = (imagePointX.x() - imagePoint.x()) / eps;
					const Scalar dX_y = (imagePointX.y() - imagePoint.y()) / eps;

					const Scalar diffX_x = Numeric::abs(jacobianX[0] - dX_x);
					const Scalar maxX_x = max(Numeric::abs(jacobianX[0]), Numeric::abs(dX_x));

					if (Numeric::isNotEqualEps(maxX_x) && diffX_x / maxX_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffX_y = Numeric::abs(jacobianY[0] - dX_y);
					const Scalar maxX_y = max(Numeric::abs(jacobianY[0]), Numeric::abs(dX_y));

					if (Numeric::isNotEqualEps(maxX_y) && diffX_y / maxX_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dpy
					Vector3 objectPointY(objectPoint);
					objectPointY.y() += eps;

					const Vector2 imagePointY(camera.projectToImageIF<true>(transformationFirstIF, objectPointY, camera.hasDistortionParameters()));

					const Scalar dY_x = (imagePointY.x() - imagePoint.x()) / eps;
					const Scalar dY_y = (imagePointY.y() - imagePoint.y()) / eps;

					const Scalar diffY_x = Numeric::abs(jacobianX[1] - dY_x);
					const Scalar maxY_x = max(Numeric::abs(jacobianX[1]), Numeric::abs(dY_x));

					if (Numeric::isNotEqualEps(maxY_x) && diffY_x / maxY_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffY_y = Numeric::abs(jacobianY[1] - dY_y);
					const Scalar maxY_y = max(Numeric::abs(jacobianY[1]), Numeric::abs(dY_y));

					if (Numeric::isNotEqualEps(maxY_y) && diffY_y / maxY_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dpz
					Vector3 objectPointZ(objectPoint);
					objectPointZ.z() += eps;

					const Vector2 imagePointZ(camera.projectToImageIF<true>(transformationFirstIF, objectPointZ, camera.hasDistortionParameters()));

					const Scalar dZ_x = (imagePointZ.x() - imagePoint.x()) / eps;
					const Scalar dZ_y = (imagePointZ.y() - imagePoint.y()) / eps;

					const Scalar diffZ_x = Numeric::abs(jacobianX[2] - dZ_x);
					const Scalar maxZ_x = max(Numeric::abs(jacobianX[2]), Numeric::abs(dZ_x));

					if (Numeric::isNotEqualEps(maxZ_x) && diffZ_x / maxZ_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffZ_y = Numeric::abs(jacobianY[2] - dZ_y);
					const Scalar maxZ_y = max(Numeric::abs(jacobianY[2]), Numeric::abs(dZ_y));

					if (Numeric::isNotEqualEps(maxZ_y) && diffZ_y / maxZ_y > 0.05)
					{
						accurate = false;
					}
				}
			}




			// second point
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<true>(transformationSecondIF, objectPoint, camera.hasDistortionParameters()));

				const Scalar* jacobianX = pointJacobiansSecondPose + 6u * (unsigned int)n + 0u;
				const Scalar* jacobianY = jacobianX + 3;

				{
					// df / dpx
					Vector3 objectPointX(objectPoint);
					objectPointX.x() += eps;

					const Vector2 imagePointX(camera.projectToImageIF<true>(transformationSecondIF, objectPointX, camera.hasDistortionParameters()));

					const Scalar dX_x = (imagePointX.x() - imagePoint.x()) / eps;
					const Scalar dX_y = (imagePointX.y() - imagePoint.y()) / eps;

					const Scalar diffX_x = Numeric::abs(jacobianX[0] - dX_x);
					const Scalar maxX_x = max(Numeric::abs(jacobianX[0]), Numeric::abs(dX_x));

					if (Numeric::isNotEqualEps(maxX_x) && diffX_x / maxX_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffX_y = Numeric::abs(jacobianY[0] - dX_y);
					const Scalar maxX_y = max(Numeric::abs(jacobianY[0]), Numeric::abs(dX_y));

					if (Numeric::isNotEqualEps(maxX_y) && diffX_y / maxX_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dpy
					Vector3 objectPointY(objectPoint);
					objectPointY.y() += eps;

					const Vector2 imagePointY(camera.projectToImageIF<true>(transformationSecondIF, objectPointY, camera.hasDistortionParameters()));

					const Scalar dY_x = (imagePointY.x() - imagePoint.x()) / eps;
					const Scalar dY_y = (imagePointY.y() - imagePoint.y()) / eps;

					const Scalar diffY_x = Numeric::abs(jacobianX[1] - dY_x);
					const Scalar maxY_x = max(Numeric::abs(jacobianX[1]), Numeric::abs(dY_x));

					if (Numeric::isNotEqualEps(maxY_x) && diffY_x / maxY_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffY_y = Numeric::abs(jacobianY[1] - dY_y);
					const Scalar maxY_y = max(Numeric::abs(jacobianY[1]), Numeric::abs(dY_y));

					if (Numeric::isNotEqualEps(maxY_y) && diffY_y / maxY_y > 0.05)
					{
						accurate = false;
					}
				}

				{
					// df / dpz
					Vector3 objectPointZ(objectPoint);
					objectPointZ.z() += eps;

					const Vector2 imagePointZ(camera.projectToImageIF<true>(transformationSecondIF, objectPointZ, camera.hasDistortionParameters()));

					const Scalar dZ_x = (imagePointZ.x() - imagePoint.x()) / eps;
					const Scalar dZ_y = (imagePointZ.y() - imagePoint.y()) / eps;

					const Scalar diffZ_x = Numeric::abs(jacobianX[2] - dZ_x);
					const Scalar maxZ_x = max(Numeric::abs(jacobianX[2]), Numeric::abs(dZ_x));

					if (Numeric::isNotEqualEps(maxZ_x) && diffZ_x / maxZ_x > 0.05)
					{
						accurate = false;
					}

					const Scalar diffZ_y = Numeric::abs(jacobianY[2] - dZ_y);
					const Scalar maxZ_y = max(Numeric::abs(jacobianY[2]), Numeric::abs(dZ_y));

					if (Numeric::isNotEqualEps(maxZ_y) && diffZ_y / maxZ_y > 0.05)
					{
						accurate = false;
					}
				}

			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testSphericalObjectPoint3x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing spherical object point jacobian 3x3:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	const Vector3 defaultRotationDirection(0, 0, -1);

	Scalar jacobianX[3];
	Scalar jacobianY[3];
	Scalar jacobianZ[3];

	const Timestamp startTimestamp(true);
	do
	{
		bool accurate = true;

		const Scalar radius = Random::scalar(Scalar(0.001), 100);
		const Vector3 defaultObjectPoint(defaultRotationDirection * radius);

		const Vector3 rotationDirection(Random::vector3());
		ocean_assert(Numeric::isEqual(rotationDirection.length(), 1));

		const Vector3 objectPoint(rotationDirection * radius);

		const ExponentialMap sphericalObjectPoint(Rotation(defaultRotationDirection, rotationDirection));

		/**
		 * jacobian for wx and wz
		 * jacobian x: | dfx / dwx, dfx / dwz |
		 * jacobian y: | dfy / dwx, dfy / dwz |
		 * jacobian z: | dfz / dwx, dfz / dwz |
		 */

		Geometry::Jacobian::calculateSphericalObjectPointJacobian3x3(jacobianX, jacobianY, jacobianZ, sphericalObjectPoint, radius);

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dwx
				ExponentialMap rotationWx(sphericalObjectPoint);
				rotationWx += ExponentialMap(epsilon, 0, 0);

				const Vector3 objectPointWx(rotationWx.rotation() * defaultObjectPoint);

				if (checkAccuracy(objectPoint, objectPointWx, epsilon, jacobianX[0], jacobianY[0], jacobianZ[0]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dwy
				ExponentialMap rotationWy(sphericalObjectPoint);
				rotationWy += ExponentialMap(0, epsilon, 0);

				const Vector3 objectPointWy(rotationWy.rotation() * defaultObjectPoint);

				if (checkAccuracy(objectPoint, objectPointWy, epsilon, jacobianX[1], jacobianY[1], jacobianZ[1]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dwz
				ExponentialMap rotationWz(sphericalObjectPoint);
				rotationWz += ExponentialMap(0, 0, epsilon);

				const Vector3 objectPointWz(rotationWz.rotation() * defaultObjectPoint);

				if (checkAccuracy(objectPoint, objectPointWz, epsilon, jacobianX[2], jacobianY[2], jacobianZ[2]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		if (accurate)
		{
			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

template <typename T>
bool TestJacobian::testSphericalObjectPointOrientation2x3IF(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing spherical object point and camera orientation jacobian 2x3 with " << sizeof(T) * 8 << "-bit precision:";

	const std::vector<double> epsilons = {NumericD::weakEps(), NumericD::weakEps() / 10.0, NumericD::weakEps() * 10.0, NumericD::weakEps() / 100.0, NumericD::weakEps() * 100.0};

	const VectorT3<T> defaultRotationDirection(0, 0, -1);

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	bool firstCameraIteration = true;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		Log::info().newLine(!firstCameraIteration);
		firstCameraIteration = false;

		const SharedAnyCameraT<T> sharedAnyCamera = Utilities::realisticAnyCamera<T>(anyCameraType, RandomI::random(randomGenerator, 1u));
		ocean_assert(sharedAnyCamera);

		const AnyCameraT<T>& camera = *sharedAnyCamera;

		Log::info() << "Camera name: " << camera.name();

		constexpr double threshold = std::is_same<float, T>::value ? 0.95 : 0.99;

		ValidationPrecision validation(threshold, randomGenerator);

		const Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const T radius = std::is_same<T, double>::value ? RandomT<T>::scalar(T(0.001), T(100)) : RandomT<T>::scalar(T(0.1), T(10));
			const VectorT3<T> defaultObjectPoint(defaultRotationDirection * radius);

			const SquareMatrixT3<T> world_R_camera(RandomT<T>::rotation());
			const SquareMatrixT3<T> flippedCamera_R_world(AnyCamera::standard2InvertedFlipped(world_R_camera));

			const VectorT2<T> testImagePoint = RandomT<T>::vector2(randomGenerator, T(5), T(camera.width() - 5u), T(5), T(camera.height() - 5u));

			const VectorT3<T> rotationDirection(camera.ray(testImagePoint, HomogenousMatrixT4<T>(world_R_camera)).direction());
			ocean_assert(NumericT<T>::isEqual(rotationDirection.length(), 1));

			const ExponentialMapT<T> sphericalObjectPoint(RotationT<T>(defaultRotationDirection, rotationDirection));

			/**
			 * jacobian x: | dfx / dwx, dfx / dwz |
			 * jacobian y: | dfy / dwx, dfy / dwz |
			 */
			T jacobianX[3];
			T jacobianY[3];

			Geometry::Jacobian::calculateSphericalObjectPointOrientationJacobian2x3IF(jacobianX, jacobianY, camera, flippedCamera_R_world, sphericalObjectPoint, radius);

			const HomogenousMatrixD4 flippedCamera_T_worldD = HomogenousMatrixD4(SquareMatrixD3(flippedCamera_R_world));

			const SharedAnyCameraD cameraD = camera.cloneToDouble();
			ocean_assert(cameraD);

			const VectorD2 imagePoint(cameraD->projectToImageIF(flippedCamera_T_worldD, ExponentialMapD(sphericalObjectPoint).rotation() * VectorD3(defaultObjectPoint)));

			{
				bool localAccuracy = false;

				for (const double epsilon : epsilons)
				{
					// df / dwx
					ExponentialMapD rotationWx(sphericalObjectPoint);
					rotationWx[0] += epsilon;

					const VectorD3 objectPointWx(rotationWx.rotation() * VectorD3(defaultObjectPoint));

					const VectorD2 imagePointWx(cameraD->projectToImageIF(flippedCamera_T_worldD, objectPointWx));

					if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[0], jacobianY[0]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				bool localAccuracy = false;

				for (const double epsilon : epsilons)
				{
					// df / dwy
					ExponentialMapD rotationWy(sphericalObjectPoint);
					rotationWy[1] += epsilon;

					const VectorD3 objectPointWy(rotationWy.rotation() * VectorD3(defaultObjectPoint));

					const VectorD2 imagePointWy(cameraD->projectToImageIF(flippedCamera_T_worldD, objectPointWy));

					if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[1], jacobianY[1]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				bool localAccuracy = false;

				for (const double epsilon : epsilons)
				{
					// df / dwz
					ExponentialMapD rotationWz(sphericalObjectPoint);
					rotationWz[2] += epsilon;

					const VectorD3 objectPointWz(rotationWz.rotation() * VectorD3(defaultObjectPoint));

					const VectorD2 imagePointWz(cameraD->projectToImageIF(flippedCamera_T_worldD, objectPointWz));

					if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[2], jacobianY[2]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					scopedIteration.setInaccurate();
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testCameraDistortionJacobian2x4(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing camera distortion jacobian 2x4:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Scalar k1 = Random::scalar(Scalar(-0.1), Scalar(0.1));
		const Scalar k2 = Random::scalar(Scalar(-0.1), Scalar(0.1));

		const Scalar p1 = Random::scalar(Scalar(-0.1), Scalar(0.1));
		const Scalar p2 = Random::scalar(Scalar(-0.1), Scalar(0.1));

		PinholeCamera camera(width, height, fovX, principalX, principalY);
		camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		camera.setTangentialDistortion(PinholeCamera::DistortionPair(Scalar(p1), Scalar(p2)));

		/**
		 * jacobian x: | dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2 |
		 * jacobian y: | dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2 |
		 */
		Scalar jacobianX[4];
		Scalar jacobianY[4];

		const Vector2 lower(camera.imagePoint2normalizedImagePoint<true>(Vector2(0, 0), false));
		const Vector2 higher(camera.imagePoint2normalizedImagePoint<true>(Vector2(Scalar(width), Scalar(height)), false));

		const Vector2 normalizedImagePoint(Random::scalar(lower.x(), higher.x()), Random::scalar(lower.y(), higher.y()));
		Geometry::Jacobian::calculateCameraDistortionJacobian2x4(jacobianX, jacobianY, camera, normalizedImagePoint);

		const Vector2 imagePoint(camera.projectToImageIF<true>(normalizedImagePoint, true));

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dk1
				PinholeCamera cameraK1(camera);
				cameraK1.setRadialDistortion(PinholeCamera::DistortionPair(k1 + epsilon, k2));

				const Vector2 imagePointK1(cameraK1.projectToImageIF<false>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointK1, epsilon, jacobianX[0], jacobianY[0]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dk2
				PinholeCamera cameraK2(camera);
				cameraK2.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2 + epsilon));

				const Vector2 imagePointK2(cameraK2.projectToImageIF<false>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointK2, epsilon, jacobianX[1], jacobianY[1]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dp1
				PinholeCamera cameraP1(camera);
				cameraP1.setTangentialDistortion(PinholeCamera::DistortionPair(p1 + epsilon, p2));

				const Vector2 imagePointP1(cameraP1.projectToImageIF<false>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointP1, epsilon, jacobianX[2], jacobianY[2]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dp2
				PinholeCamera cameraP2(camera);
				cameraP2.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2 + epsilon));

				const Vector2 imagePointP2(cameraP2.projectToImageIF<false>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointP2, epsilon, jacobianX[3], jacobianY[3]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		if (accurate)
		{
			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testCameraJacobian2x6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing camera jacobian 2x6:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Scalar k1 = Random::scalar(Scalar(-0.1), Scalar(0.1));
		const Scalar k2 = Random::scalar(Scalar(-0.1), Scalar(0.1));

		PinholeCamera camera(width, height, fovX, principalX, principalY);
		camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		camera.setTangentialDistortion(PinholeCamera::DistortionPair(Scalar(0), Scalar(0)));

		/**
		 * jacobian x: | dfx / dk1, dfx / dk2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy |
		 * jacobian y: | dfy / dk1, dfy / dk2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy |
		 */
		Scalar jacobianX[6];
		Scalar jacobianY[6];

		const Vector2 lower(camera.imagePoint2normalizedImagePoint<true>(Vector2(0, 0), false));
		const Vector2 higher(camera.imagePoint2normalizedImagePoint<true>(Vector2(Scalar(width), Scalar(height)), false));

		const Vector2 normalizedImagePoint(Random::scalar(lower.x(), higher.x()), Random::scalar(lower.y(), higher.y()));
		Geometry::Jacobian::calculateCameraJacobian2x6(jacobianX, jacobianY, camera, normalizedImagePoint);

		const Vector2 imagePoint(camera.projectToImageIF<true>(normalizedImagePoint, true));

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dk1
				PinholeCamera cameraK1(camera);
				cameraK1.setRadialDistortion(PinholeCamera::DistortionPair(k1 + epsilon, k2));

				const Vector2 imagePointK1(cameraK1.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointK1, epsilon, jacobianX[0], jacobianY[0]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dk2
				PinholeCamera cameraK2(camera);
				cameraK2.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2 + epsilon));

				const Vector2 imagePointK2(cameraK2.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointK2, epsilon, jacobianX[1], jacobianY[1]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dFx
				PinholeCamera cameraFx(camera);
				SquareMatrix3 intrinsicFx(camera.intrinsic());
				intrinsicFx(0, 0) += epsilon;
				cameraFx.setIntrinsic(intrinsicFx);

				const Vector2 imagePointFx(cameraFx.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointFx, epsilon, jacobianX[2], jacobianY[2]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dFy
				PinholeCamera cameraFy(camera);
				SquareMatrix3 intrinsicFy(camera.intrinsic());
				intrinsicFy(1, 1) += epsilon;
				cameraFy.setIntrinsic(intrinsicFy);

				const Vector2 imagePointFy(cameraFy.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointFy, epsilon, jacobianX[3], jacobianY[3]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dmx
				PinholeCamera cameraMx(camera);
				SquareMatrix3 intrinsicMx(camera.intrinsic());
				intrinsicMx(0, 2) += epsilon;
				cameraMx.setIntrinsic(intrinsicMx);

				const Vector2 imagePointMx(cameraMx.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointMx, epsilon, jacobianX[4], jacobianY[4]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dmy
				PinholeCamera cameraMy(camera);
				SquareMatrix3 intrinsicMy(camera.intrinsic());
				intrinsicMy(1, 2) += epsilon;
				cameraMy.setIntrinsic(intrinsicMy);

				const Vector2 imagePointMy(cameraMy.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointMy, epsilon, jacobianX[5], jacobianY[5]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}

		}

		if (accurate)
		{
			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testCameraJacobian2x7(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing camera jacobian 2x7:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar k1 = Random::scalar(Scalar(-0.5), Scalar(0.5));
		const Scalar k2 = Random::scalar(Scalar(-0.5), Scalar(0.5));

		const Scalar p1 = Random::scalar(Scalar(-0.5), Scalar(0.5));
		const Scalar p2 = Random::scalar(Scalar(-0.5), Scalar(0.5));

		PinholeCamera camera(width, height, fovX);
		camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		camera.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));

		/**
		 * jacobian x: | dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dF, dfx / dmx, dfx / dmy |
		 * jacobian y: | dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dF, dfy / dmx, dfy / dmy |
		 */
		Scalar jacobianX[7];
		Scalar jacobianY[7];

		const Vector2 lower(camera.imagePoint2normalizedImagePoint<true>(Vector2(0, 0), false));
		const Vector2 higher(camera.imagePoint2normalizedImagePoint<true>(Vector2(Scalar(width), Scalar(height)), false));

		const Vector2 normalizedImagePoint(Random::scalar(lower.x(), higher.x()), Random::scalar(lower.y(), higher.y()));
		Geometry::Jacobian::calculateCameraJacobian2x7(jacobianX, jacobianY, camera, normalizedImagePoint);

		const Vector2 imagePoint(camera.projectToImageIF<true>(normalizedImagePoint, true));

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dk1
				PinholeCamera cameraK1(camera);
				cameraK1.setRadialDistortion(PinholeCamera::DistortionPair(k1 + epsilon, k2));

				const Vector2 imagePointK1(cameraK1.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointK1, epsilon, jacobianX[0], jacobianY[0]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dk2
				PinholeCamera cameraK2(camera);
				cameraK2.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2 + epsilon));

				const Vector2 imagePointK2(cameraK2.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointK2, epsilon, jacobianX[1], jacobianY[1]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dp1
				PinholeCamera cameraP1(camera);
				cameraP1.setTangentialDistortion(PinholeCamera::DistortionPair(p1 + epsilon, p2));

				const Vector2 imagePointP1(cameraP1.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointP1, epsilon, jacobianX[2], jacobianY[2]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dp2
				PinholeCamera cameraP2(camera);
				cameraP2.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2 + epsilon));

				const Vector2 imagePointP2(cameraP2.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointP2, epsilon, jacobianX[3], jacobianY[3]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dF
				PinholeCamera cameraF(camera);
				SquareMatrix3 intrinsicF(camera.intrinsic());
				intrinsicF(0, 0) += epsilon;
				intrinsicF(1, 1) += epsilon;
				cameraF.setIntrinsic(intrinsicF);

				const Vector2 imagePointF(cameraF.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointF, epsilon, jacobianX[4], jacobianY[4]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dmx
				PinholeCamera cameraMx(camera);
				SquareMatrix3 intrinsicMx(camera.intrinsic());
				intrinsicMx(0, 2) += epsilon;
				cameraMx.setIntrinsic(intrinsicMx);

				const Vector2 imagePointMx(cameraMx.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointMx, epsilon, jacobianX[5], jacobianY[5]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dmy
				PinholeCamera cameraMy(camera);
				SquareMatrix3 intrinsicMy(camera.intrinsic());
				intrinsicMy(1, 2) += epsilon;
				cameraMy.setIntrinsic(intrinsicMy);

				const Vector2 imagePointMy(cameraMy.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointMy, epsilon, jacobianX[6], jacobianY[6]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		if (accurate)
		{
			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testCameraJacobian2x8(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing camera jacobian 2x8:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Scalar k1 = Random::scalar(Scalar(-0.5), Scalar(0.5));
		const Scalar k2 = Random::scalar(Scalar(-0.5), Scalar(0.5));

		const Scalar p1 = Random::scalar(Scalar(-0.5), Scalar(0.5));
		const Scalar p2 = Random::scalar(Scalar(-0.5), Scalar(0.5));

		PinholeCamera camera(width, height, fovX, principalX, principalY);
		camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		camera.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));

		/**
		 * jacobian x: | dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy |
		 * jacobian y: | dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy |
		 */
		Scalar jacobianX[8];
		Scalar jacobianY[8];

		const Vector2 lower(camera.imagePoint2normalizedImagePoint<true>(Vector2(0, 0), false));
		const Vector2 higher(camera.imagePoint2normalizedImagePoint<true>(Vector2(Scalar(width), Scalar(height)), false));

		const Vector2 normalizedImagePoint(Random::scalar(lower.x(), higher.x()), Random::scalar(lower.y(), higher.y()));
		Geometry::Jacobian::calculateCameraJacobian2x8(jacobianX, jacobianY, camera, normalizedImagePoint);

		const Vector2 imagePoint(camera.projectToImageIF<true>(normalizedImagePoint, true));

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dk1
				PinholeCamera cameraK1(camera);
				cameraK1.setRadialDistortion(PinholeCamera::DistortionPair(k1 + epsilon, k2));

				const Vector2 imagePointK1(cameraK1.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointK1, epsilon, jacobianX[0], jacobianY[0]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dk2
				PinholeCamera cameraK2(camera);
				cameraK2.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2 + epsilon));

				const Vector2 imagePointK2(cameraK2.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointK2, epsilon, jacobianX[1], jacobianY[1]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dp1
				PinholeCamera cameraP1(camera);
				cameraP1.setTangentialDistortion(PinholeCamera::DistortionPair(p1 + epsilon, p2));

				const Vector2 imagePointP1(cameraP1.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointP1, epsilon, jacobianX[2], jacobianY[2]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dp2
				PinholeCamera cameraP2(camera);
				cameraP2.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2 + epsilon));

				const Vector2 imagePointP2(cameraP2.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointP2, epsilon, jacobianX[3], jacobianY[3]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dFx
				PinholeCamera cameraFx(camera);
				SquareMatrix3 intrinsicFx(camera.intrinsic());
				intrinsicFx(0, 0) += epsilon;
				cameraFx.setIntrinsic(intrinsicFx);

				const Vector2 imagePointFx(cameraFx.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointFx, epsilon, jacobianX[4], jacobianY[4]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dFy
				PinholeCamera cameraFy(camera);
				SquareMatrix3 intrinsicFy(camera.intrinsic());
				intrinsicFy(1, 1) += epsilon;
				cameraFy.setIntrinsic(intrinsicFy);

				const Vector2 imagePointFy(cameraFy.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointFy, epsilon, jacobianX[5], jacobianY[5]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dmx
				PinholeCamera cameraMx(camera);
				SquareMatrix3 intrinsicMx(camera.intrinsic());
				intrinsicMx(0, 2) += epsilon;
				cameraMx.setIntrinsic(intrinsicMx);

				const Vector2 imagePointMx(cameraMx.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointMx, epsilon, jacobianX[6], jacobianY[6]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		{
			bool localAccuracy = false;

			for (const Scalar epsilon : epsilons)
			{
				// df / dmy
				PinholeCamera cameraMy(camera);
				SquareMatrix3 intrinsicMy(camera.intrinsic());
				intrinsicMy(1, 2) += epsilon;
				cameraMy.setIntrinsic(intrinsicMy);

				const Vector2 imagePointMy(cameraMy.projectToImageIF<true>(normalizedImagePoint, true));

				if (checkAccuracy(imagePoint, imagePointMy, epsilon, jacobianX[7], jacobianY[7]))
				{
					localAccuracy = true;
					break;
				}
			}

			if (!localAccuracy)
			{
				accurate = false;
			}
		}

		if (accurate)
		{
			++succeeded;
		}

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testOrientationCameraJacobian2x11(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing camera orientation jacobian 2x11 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	unsigned int distortionIteration = 0u;

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 translation = Random::vector3(-1, 1);
		const Quaternion quaternion = Random::quaternion();

		const HomogenousMatrix4 extrinsicIF(translation, quaternion);
		const Pose poseIF(extrinsicIF);

		PinholeCamera camera(width, height, fovX, principalX, principalY);

		if (distortionIteration % 3u == 1u || distortionIteration % 3u == 2u)
		{
			const Scalar k1 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			const Scalar k2 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		}

		if (distortionIteration % 3u == 2u)
		{
			const Scalar p1 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			const Scalar p2 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			camera.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));
		}

		const Scalar k1 = camera.radialDistortion().first;
		const Scalar k2 = camera.radialDistortion().second;

		const Scalar p1 = camera.tangentialDistortion().first;
		const Scalar p2 = camera.tangentialDistortion().second;

		distortionIteration++;

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			objectPoints.push_back(camera.ray(Vector2(Random::scalar(40u, width - 40u), Random::scalar(40u, height - 40u)), PinholeCamera::invertedFlipped2Standard(extrinsicIF)).point(Random::scalar(1, 10)));
		}

		/**
		 * jacobian:
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy |<br>
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy |<br>
		 */

		Matrix jacobian(2 * objectPoints.size(), 11);

		performance.start();
		Geometry::Jacobian::calculateOrientationCameraJacobianRodrigues2nx11(jacobian.data(), camera, poseIF, ConstArrayAccessor<Vector3>(objectPoints));
		performance.stop();

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 11);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			const HomogenousMatrix4 transformationIF(poseIF.transformation());

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(transformationIF, objectPoint, camera.hasDistortionParameters()));

				for (unsigned int i = 0u; i < 11u; ++i)
				{
					Pose poseDelta(poseIF);

					SquareMatrix3 intrinsicDelta(camera.intrinsic());
					PinholeCamera::DistortionPair radialDistortionDelta(camera.radialDistortion());
					PinholeCamera::DistortionPair tangentialDistortionDelta(camera.tangentialDistortion());

					if (i < 3u)
					{
						poseDelta[i + 3u] += Numeric::weakEps();
					}
					else
					{
						switch (i)
						{
							case 3u:
								radialDistortionDelta.first += Numeric::weakEps();
								break;

							case 4u:
								radialDistortionDelta.second += Numeric::weakEps();
								break;

							case 5u:
								tangentialDistortionDelta.first += Numeric::weakEps();
								break;

							case 6u:
								tangentialDistortionDelta.second += Numeric::weakEps();
								break;

							case 7u:
								intrinsicDelta(0, 0) += Numeric::weakEps();
								break;

							case 8u:
								intrinsicDelta(1, 1) += Numeric::weakEps();
								break;

							case 9u:
								intrinsicDelta(2, 0) += Numeric::weakEps();
								break;

							case 10u:
								intrinsicDelta(2, 1) += Numeric::weakEps();
								break;

							default:
								ocean_assert(false && "This should never happen!");
						}
					}

					const PinholeCamera cameraDelta(intrinsicDelta, camera.width(), camera.height(), radialDistortionDelta, tangentialDistortionDelta);

					const Vector2 imagePointDelta(cameraDelta.projectToImageIF<false>(poseDelta.transformation(), objectPoint, camera.hasDistortionParameters()));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];
			const Vector2 imagePoint(camera.projectToImageIF<false>(extrinsicIF, objectPoint, true));

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			{
				// we also test the implementation for one object point

				Scalar singleJacobianX[11];
				Scalar singleJacobianY[11];
				Geometry::Jacobian::calculateOrientationCameraJacobianRodrigues2x11(singleJacobianX, singleJacobianY, camera, poseIF, objectPoint);

				for (unsigned int i = 0u; i < 11u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						accurate = false;
					}
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwx
					Pose poseWx(poseIF);
					poseWx.rx() += epsilon;

					const Vector2 imagePointWx(camera.projectToImageIF<false>(poseWx.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[0], jacobianY[0]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwy
					Pose poseWy(poseIF);
					poseWy.ry() += epsilon;

					const Vector2 imagePointWy(camera.projectToImageIF<false>(poseWy.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[1], jacobianY[1]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwz
					Pose poseWz(poseIF);
					poseWz.rz() += epsilon;

					const Vector2 imagePointWz(camera.projectToImageIF<false>(poseWz.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[2], jacobianY[2]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dk1
					PinholeCamera cameraK1(camera);
					cameraK1.setRadialDistortion(PinholeCamera::DistortionPair(k1 + epsilon, k2));

					const Vector2 imagePointK1(cameraK1.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointK1, epsilon, jacobianX[3], jacobianY[3]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dk2
					PinholeCamera cameraK2(camera);
					cameraK2.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2 + epsilon));

					const Vector2 imagePointK2(cameraK2.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointK2, epsilon, jacobianX[4], jacobianY[4]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dp1
					PinholeCamera cameraP1(camera);
					cameraP1.setTangentialDistortion(PinholeCamera::DistortionPair(p1 + epsilon, p2));

					const Vector2 imagePointP1(cameraP1.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointP1, epsilon, jacobianX[5], jacobianY[5]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dp2
					PinholeCamera cameraP2(camera);
					cameraP2.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2 + epsilon));

					const Vector2 imagePointP2(cameraP2.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointP2, epsilon, jacobianX[6], jacobianY[6]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dFx
					PinholeCamera cameraFx(camera);
					SquareMatrix3 intrinsicFx(camera.intrinsic());
					intrinsicFx(0, 0) += epsilon;
					cameraFx.setIntrinsic(intrinsicFx);

					const Vector2 imagePointFx(cameraFx.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointFx, epsilon, jacobianX[7], jacobianY[7]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dFy
					PinholeCamera cameraFy(camera);
					SquareMatrix3 intrinsicFy(camera.intrinsic());
					intrinsicFy(1, 1) += epsilon;
					cameraFy.setIntrinsic(intrinsicFy);

					const Vector2 imagePointFy(cameraFy.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointFy, epsilon, jacobianX[8], jacobianY[8]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dmx
					PinholeCamera cameraMx(camera);
					SquareMatrix3 intrinsicMx(camera.intrinsic());
					intrinsicMx(0, 2) += epsilon;
					cameraMx.setIntrinsic(intrinsicMx);

					const Vector2 imagePointMx(cameraMx.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointMx, epsilon, jacobianX[9], jacobianY[9]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dmy
					PinholeCamera cameraMy(camera);
					SquareMatrix3 intrinsicMy(camera.intrinsic());
					intrinsicMy(1, 2) += epsilon;
					cameraMy.setIntrinsic(intrinsicMy);

					const Vector2 imagePointMy(cameraMy.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointMy, epsilon, jacobianX[10], jacobianY[10]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testPoseCameraJacobian2x12(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing camera pose jacobian 2x12 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	unsigned int distortionIteration = 0u;

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 translation = Random::vector3(-1, 1);
		const Quaternion quaternion = Random::quaternion();

		const HomogenousMatrix4 extrinsicIF(translation, quaternion);
		const Pose poseIF(extrinsicIF);

		PinholeCamera camera(width, height, fovX, principalX, principalY);

		if (distortionIteration % 3u == 1u || distortionIteration % 3u == 2u)
		{
			const Scalar k1 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			const Scalar k2 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		}

		if (distortionIteration % 3u == 2u)
		{
			const Scalar p1 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			const Scalar p2 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			camera.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));
		}

		const Scalar k1 = camera.radialDistortion().first;
		const Scalar k2 = camera.radialDistortion().second;

		distortionIteration++;

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			objectPoints.push_back(camera.ray(Vector2(Random::scalar(40u, width - 40u), Random::scalar(40u, height - 40u)), PinholeCamera::invertedFlipped2Standard(extrinsicIF)).point(Random::scalar(1, 10)));
		}


		/**
		 * | dfx / dk1, dfx / dk2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |<br>
		 * | dfy / dk1, dfy / dk2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |<br>
		 */

		Matrix jacobian(2 * objectPoints.size(), 12);

		performance.start();

		const Pose iFlippedPose(extrinsicIF);

		SquareMatrix3 Rwx, Rwy, Rwz;
		Geometry::Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(Vector3(iFlippedPose.rx(), iFlippedPose.ry(), iFlippedPose.rz())), Rwx, Rwy, Rwz);

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			Geometry::Jacobian::calculateJacobianCameraPoseRodrigues2x12(jacobian[n * 2u + 0u], jacobian[n * 2u + 1], camera, extrinsicIF, iFlippedPose, objectPoints[n], Rwx, Rwy, Rwz);
		}

		performance.stop();

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 12);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			const HomogenousMatrix4 transformationIF(poseIF.transformation());

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(transformationIF, objectPoint, camera.hasDistortionParameters()));

				for (unsigned int i = 0u; i < 12u; ++i)
				{
					Pose poseDelta(poseIF);

					SquareMatrix3 intrinsicDelta(camera.intrinsic());
					PinholeCamera::DistortionPair radialDistortionDelta(camera.radialDistortion());
					PinholeCamera::DistortionPair tangentialDistortionDelta(camera.tangentialDistortion());

					if (i >= 6u && i < 6u + 6u)
					{
						if (i < 6u + 3u)
						{
							poseDelta[i - 6u + 3u] += Numeric::weakEps();
						}
						else
						{
							poseDelta[i - 6u - 3u] += Numeric::weakEps();
						}
					}
					else
					{
						switch (i)
						{
							case 0u:
								radialDistortionDelta.first += Numeric::weakEps();
								break;

							case 1u:
								radialDistortionDelta.second += Numeric::weakEps();
								break;

							case 2u:
								intrinsicDelta(0, 0) += Numeric::weakEps();
								break;

							case 3u:
								intrinsicDelta(1, 1) += Numeric::weakEps();
								break;

							case 4u:
								intrinsicDelta(2, 0) += Numeric::weakEps();
								break;

							case 5u:
								intrinsicDelta(2, 1) += Numeric::weakEps();
								break;

							default:
								ocean_assert(false && "This should never happend!");
						}
					}

					const PinholeCamera cameraDelta(intrinsicDelta, camera.width(), camera.height(), radialDistortionDelta, tangentialDistortionDelta);

					const Vector2 imagePointDelta(cameraDelta.projectToImageIF<false>(poseDelta.transformation(), objectPoint, camera.hasDistortionParameters()));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2u + 0u][i] = derivative.x();
					naiveJacobian[n * 2u + 1u][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];
			const Vector2 imagePoint(camera.projectToImageIF<false>(extrinsicIF, objectPoint, true));

			const Scalar* jacobianX = jacobian[2u * n + 0u];
			const Scalar* jacobianY = jacobian[2u * n + 1u];

			{
				// we also test the implementation for one object point

				Scalar singleJacobianX[12];
				Scalar singleJacobianY[12];
				Geometry::Jacobian::calculateJacobianCameraPoseRodrigues2x12(singleJacobianX, singleJacobianY, camera, extrinsicIF, objectPoint);

				for (unsigned int i = 0u; i < 12u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						accurate = false;
					}
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dk1
					PinholeCamera cameraK1(camera);
					cameraK1.setRadialDistortion(PinholeCamera::DistortionPair(k1 + epsilon, k2));

					const Vector2 imagePointK1(cameraK1.projectToImageIF<true>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointK1, epsilon, jacobianX[0], jacobianY[0]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dk2
					PinholeCamera cameraK2(camera);
					cameraK2.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2 + epsilon));

					const Vector2 imagePointK2(cameraK2.projectToImageIF<true>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointK2, epsilon, jacobianX[1], jacobianY[1]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dFx
					PinholeCamera cameraFx(camera);
					SquareMatrix3 intrinsicFx(camera.intrinsic());
					intrinsicFx(0, 0) += epsilon;
					cameraFx.setIntrinsic(intrinsicFx);

					const Vector2 imagePointFx(cameraFx.projectToImageIF<true>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointFx, epsilon, jacobianX[2], jacobianY[2]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dFy
					PinholeCamera cameraFy(camera);
					SquareMatrix3 intrinsicFy(camera.intrinsic());
					intrinsicFy(1, 1) += epsilon;
					cameraFy.setIntrinsic(intrinsicFy);

					const Vector2 imagePointFy(cameraFy.projectToImageIF<true>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointFy, epsilon, jacobianX[3], jacobianY[3]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dmx
					PinholeCamera cameraMx(camera);
					SquareMatrix3 intrinsicMx(camera.intrinsic());
					intrinsicMx(0, 2) += epsilon;
					cameraMx.setIntrinsic(intrinsicMx);

					const Vector2 imagePointMx(cameraMx.projectToImageIF<true>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointMx, epsilon, jacobianX[4], jacobianY[4]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dmy
					PinholeCamera cameraMy(camera);
					SquareMatrix3 intrinsicMy(camera.intrinsic());
					intrinsicMy(1, 2) += epsilon;
					cameraMy.setIntrinsic(intrinsicMy);

					const Vector2 imagePointMy(cameraMy.projectToImageIF<true>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointMy, epsilon, jacobianX[5], jacobianY[5]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwx
					Pose poseWx(poseIF);
					poseWx.rx() += epsilon;

					const Vector2 imagePointWx(camera.projectToImageIF<true>(poseWx.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[6], jacobianY[6]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwy
					Pose poseWy(poseIF);
					poseWy.ry() += epsilon;

					const Vector2 imagePointWy(camera.projectToImageIF<true>(poseWy.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[7], jacobianY[7]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwz
					Pose poseWz(poseIF);
					poseWz.rz() += epsilon;

					const Vector2 imagePointWz(camera.projectToImageIF<true>(poseWz.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[8], jacobianY[8]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtx
					Pose poseTx(poseIF);
					poseTx.x() += epsilon;

					const Vector2 imagePointTx(camera.projectToImageIF<true>(poseTx.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointTx, epsilon, jacobianX[9], jacobianY[9]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dty
					Pose poseTy(poseIF);
					poseTy.y() += epsilon;

					const Vector2 imagePointTy(camera.projectToImageIF<true>(poseTy.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointTy, epsilon, jacobianX[10], jacobianY[10]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtz
					Pose poseTz(poseIF);
					poseTz.z() += epsilon;

					const Vector2 imagePointTz(camera.projectToImageIF<true>(poseTz.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointTz, epsilon, jacobianX[11], jacobianY[11]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testPoseCameraJacobian2x14(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing camera pose jacobian 2x14 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	unsigned int distortionIteration = 0u;

	do
	{
		bool accurate = true;

		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar width2 = Scalar(width) * Scalar(0.5);
		const Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 translation = Random::vector3(-1, 1);
		const Quaternion quaternion = Random::quaternion();

		const HomogenousMatrix4 extrinsicIF(translation, quaternion);
		const Pose poseIF(extrinsicIF);

		PinholeCamera camera(width, height, fovX, principalX, principalY);

		if (distortionIteration % 3u == 1u || distortionIteration % 3u == 2u)
		{
			const Scalar k1 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			const Scalar k2 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		}

		if (distortionIteration % 3u == 2u)
		{
			const Scalar p1 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			const Scalar p2 = Random::scalar(Scalar(-0.5), Scalar(0.5));
			camera.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));
		}

		const Scalar k1 = camera.radialDistortion().first;
		const Scalar k2 = camera.radialDistortion().second;

		const Scalar p1 = camera.tangentialDistortion().first;
		const Scalar p2 = camera.tangentialDistortion().second;

		distortionIteration++;

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			objectPoints.push_back(camera.ray(Vector2(Random::scalar(40u, width - 40u), Random::scalar(40u, height - 40u)), PinholeCamera::invertedFlipped2Standard(extrinsicIF)).point(Random::scalar(1, 10)));
		}

		/**
		 * | dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |<br>
		 * | dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |<br>
		 */

		Matrix jacobian(2 * objectPoints.size(), 14);

		performance.start();
		Geometry::Jacobian::calculateJacobianCameraPoseRodrigues2nx14(jacobian.data(), camera, poseIF, ConstArrayAccessor<Vector3>(objectPoints));
		performance.stop();

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 14);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			const HomogenousMatrix4 transformationIF(poseIF.transformation());

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(transformationIF, objectPoint, camera.hasDistortionParameters()));

				for (unsigned int i = 0u; i < 14u; ++i)
				{
					Pose poseDelta(poseIF);

					SquareMatrix3 intrinsicDelta(camera.intrinsic());
					PinholeCamera::DistortionPair radialDistortionDelta(camera.radialDistortion());
					PinholeCamera::DistortionPair tangentialDistortionDelta(camera.tangentialDistortion());

					if (i >= 8u && i < 8u + 6u)
					{
						if (i < 8u + 3u)
						{
							poseDelta[i - 8u + 3u] += Numeric::weakEps();
						}
						else
						{
							poseDelta[i - 8u - 3u] += Numeric::weakEps();
						}
					}
					else
					{
						switch (i)
						{
							case 0u:
								radialDistortionDelta.first += Numeric::weakEps();
								break;

							case 1u:
								radialDistortionDelta.second += Numeric::weakEps();
								break;

							case 2u:
								tangentialDistortionDelta.first += Numeric::weakEps();
								break;

							case 3u:
								tangentialDistortionDelta.second += Numeric::weakEps();
								break;

							case 4u:
								intrinsicDelta(0, 0) += Numeric::weakEps();
								break;

							case 5u:
								intrinsicDelta(1, 1) += Numeric::weakEps();
								break;

							case 6u:
								intrinsicDelta(2, 0) += Numeric::weakEps();
								break;

							case 7u:
								intrinsicDelta(2, 1) += Numeric::weakEps();
								break;

							default:
								ocean_assert(false && "This should never happend!");
						}
					}

					const PinholeCamera cameraDelta(intrinsicDelta, camera.width(), camera.height(), radialDistortionDelta, tangentialDistortionDelta);

					const Vector2 imagePointDelta(cameraDelta.projectToImageIF<false>(poseDelta.transformation(), objectPoint, camera.hasDistortionParameters()));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2u + 0u][i] = derivative.x();
					naiveJacobian[n * 2u + 1u][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];
			const Vector2 imagePoint(camera.projectToImageIF<false>(extrinsicIF, objectPoint, true));

			const Scalar* jacobianX = jacobian[2u * n + 0u];
			const Scalar* jacobianY = jacobian[2u * n + 1u];

			{
				// we also test the implementation for one object point

				Scalar singleJacobianX[14];
				Scalar singleJacobianY[14];
				Geometry::Jacobian::calculateJacobianCameraPoseRodrigues2x14(singleJacobianX, singleJacobianY, camera, extrinsicIF, objectPoint);

				for (unsigned int i = 0u; i < 14u; ++i)
				{
					ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert((std::is_same<Scalar, float>::value) || Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						accurate = false;
					}
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dk1
					PinholeCamera cameraK1(camera);
					cameraK1.setRadialDistortion(PinholeCamera::DistortionPair(k1 + epsilon, k2));

					const Vector2 imagePointK1(cameraK1.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointK1, epsilon, jacobianX[0], jacobianY[0]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dk2
					PinholeCamera cameraK2(camera);
					cameraK2.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2 + epsilon));

					const Vector2 imagePointK2(cameraK2.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointK2, epsilon, jacobianX[1], jacobianY[1]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dp1
					PinholeCamera cameraP1(camera);
					cameraP1.setTangentialDistortion(PinholeCamera::DistortionPair(p1 + epsilon, p2));

					const Vector2 imagePointP1(cameraP1.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointP1, epsilon, jacobianX[2], jacobianY[2]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dp2
					PinholeCamera cameraP2(camera);
					cameraP2.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2 + epsilon));

					const Vector2 imagePointP2(cameraP2.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointP2, epsilon, jacobianX[3], jacobianY[3]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dFx
					PinholeCamera cameraFx(camera);
					SquareMatrix3 intrinsicFx(camera.intrinsic());
					intrinsicFx(0, 0) += epsilon;
					cameraFx.setIntrinsic(intrinsicFx);

					const Vector2 imagePointFx(cameraFx.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointFx, epsilon, jacobianX[4], jacobianY[4]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dFy
					PinholeCamera cameraFy(camera);
					SquareMatrix3 intrinsicFy(camera.intrinsic());
					intrinsicFy(1, 1) += epsilon;
					cameraFy.setIntrinsic(intrinsicFy);

					const Vector2 imagePointFy(cameraFy.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointFy, epsilon, jacobianX[5], jacobianY[5]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dmx
					PinholeCamera cameraMx(camera);
					SquareMatrix3 intrinsicMx(camera.intrinsic());
					intrinsicMx(0, 2) += epsilon;
					cameraMx.setIntrinsic(intrinsicMx);

					const Vector2 imagePointMx(cameraMx.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointMx, epsilon, jacobianX[6], jacobianY[6]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dmy
					PinholeCamera cameraMy(camera);
					SquareMatrix3 intrinsicMy(camera.intrinsic());
					intrinsicMy(1, 2) += epsilon;
					cameraMy.setIntrinsic(intrinsicMy);

					const Vector2 imagePointMy(cameraMy.projectToImageIF<false>(extrinsicIF, objectPoint, true));

					if (checkAccuracy(imagePoint, imagePointMy, epsilon, jacobianX[7], jacobianY[7]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwx
					Pose poseWx(poseIF);
					poseWx.rx() += epsilon;

					const Vector2 imagePointWx(camera.projectToImageIF<false>(poseWx.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointWx, epsilon, jacobianX[8], jacobianY[8]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwy
					Pose poseWy(poseIF);
					poseWy.ry() += epsilon;

					const Vector2 imagePointWy(camera.projectToImageIF<false>(poseWy.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointWy, epsilon, jacobianX[9], jacobianY[9]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dwz
					Pose poseWz(poseIF);
					poseWz.rz() += epsilon;

					const Vector2 imagePointWz(camera.projectToImageIF<false>(poseWz.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointWz, epsilon, jacobianX[10], jacobianY[10]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtx
					Pose poseTx(poseIF);
					poseTx.x() += epsilon;

					const Vector2 imagePointTx(camera.projectToImageIF<false>(poseTx.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointTx, epsilon, jacobianX[11], jacobianY[11]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dty
					Pose poseTy(poseIF);
					poseTy.y() += epsilon;

					const Vector2 imagePointTy(camera.projectToImageIF<false>(poseTy.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointTy, epsilon, jacobianX[12], jacobianY[12]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dtz
					Pose poseTz(poseIF);
					poseTz.z() += epsilon;

					const Vector2 imagePointTz(camera.projectToImageIF<false>(poseTz.transformation(), objectPoint, camera.hasDistortionParameters()));

					if (checkAccuracy(imagePoint, imagePointTz, epsilon, jacobianX[13], jacobianY[13]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testHomography2x8(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing homography Jacobian 2x8 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	Vectors2 points(numberPoints);

	Matrix jacobians(2 * numberPoints, 8);
	Matrix naiveJacobians(2 * numberPoints, 8);

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		unsigned int width = 1920u;
		unsigned int height = 1080u;

		SquareMatrix3 homography;

		for (unsigned int n = 0u; n < 9u; ++n)
		{
			homography[n] = Random::scalar(-10, 10);
		}

		while (Numeric::isEqualEps(homography[8]))
		{
			homography[8] = Random::scalar(-10, 10);
		}

		Geometry::Homography::normalizeHomography(homography);

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			points[n] = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
		}

		performance.start();

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			Geometry::Jacobian::calculateHomographyJacobian2x8(jacobians[2 * n + 0u], jacobians[2 * n + 1u], points[n].x(), points[n].y(), homography);
		}

		performance.stop();

		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < points.size(); ++n)
			{
				const Vector2 point = points[n];
				const Vector2 transformedPoint(homography * point);

				for (unsigned int i = 0u; i < 8u; ++i)
				{
					SquareMatrix3 homographyDelta(homography);
					homographyDelta[i] += Numeric::weakEps();

					const Vector2 pointDelta(homographyDelta * point);
					const Vector2 derivative = (pointDelta - transformedPoint) / Numeric::weakEps();

					naiveJacobians[2u * n + 0u][i] = derivative.x();
					naiveJacobians[2u * n + 1u][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < points.size(); ++n)
		{
			const Vector2 point = points[n];
			const Vector2 transformedPoint(homography * point);

			const Scalar* jacobianX = jacobians[2u * n + 0u];
			const Scalar* jacobianY = jacobians[2u * n + 1u];

			for (unsigned int h = 0u; h < 8u; ++h)
			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dh0 .. dh7
					SquareMatrix3 homographyDelta(homography);
					homographyDelta[h] += epsilon;

					const Vector2 transformedPointDelta(homographyDelta * point);

					if (checkAccuracy(transformedPoint, transformedPointDelta, epsilon, jacobianX[h], jacobianY[h]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testHomography2x9(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing homography Jacobian 2x9 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	Vectors2 points(numberPoints);

	Matrix jacobians(2 * numberPoints, 9);
	Matrix naiveJacobians(2 * numberPoints, 9);

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		unsigned int width = 1920u;
		unsigned int height = 1080u;

		SquareMatrix3 homography;

		for (unsigned int n = 0u; n < 9u; ++n)
		{
			homography[n] = Random::scalar(-10, 10);
		}

		while (Numeric::isEqualEps(homography[8]))
		{
			homography[8] = Random::scalar(-10, 10);
		}

		Geometry::Homography::normalizeHomography(homography);

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			points[n] = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
		}

		performance.start();

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			Geometry::Jacobian::calculateHomographyJacobian2x9(jacobians[2 * n + 0u], jacobians[2 * n + 1u], points[n].x(), points[n].y(), homography);
		}

		performance.stop();

		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < points.size(); ++n)
			{
				const Vector2 point = points[n];
				const Vector2 transformedPoint(homography * point);

				for (unsigned int i = 0u; i < 9u; ++i)
				{
					SquareMatrix3 homographyDelta(homography);
					homographyDelta[i] += Numeric::weakEps();

					const Vector2 pointDelta(homographyDelta * point);
					const Vector2 derivative = (pointDelta - transformedPoint) / Numeric::weakEps();

					naiveJacobians[2u * n + 0u][i] = derivative.x();
					naiveJacobians[2u * n + 1u][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < points.size(); ++n)
		{
			const Vector2 point = points[n];
			const Vector2 transformedPoint(homography * point);

			const Scalar* jacobianX = jacobians[2u * n + 0u];
			const Scalar* jacobianY = jacobians[2u * n + 1u];

			for (unsigned int h = 0u; h < 9u; ++h)
			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dh0 .. dh7
					SquareMatrix3 homographyDelta(homography);
					homographyDelta[h] += epsilon;

					const Vector2 transformedPointDelta(homographyDelta * point);

					if (checkAccuracy(transformedPoint, transformedPointDelta, epsilon, jacobianX[h], jacobianY[h]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testIdentityHomography2x8(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing identity homography Jacobian 2x8 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	Vectors2 points(numberPoints);

	Matrix jacobians(2 * numberPoints, 8);
	Matrix naiveJacobians(2 * numberPoints, 8);

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		unsigned int width = 1920u;
		unsigned int height = 1080u;

		const SquareMatrix3 homography(true);

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			points[n] = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
		}

		performance.start();

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			Geometry::Jacobian::calculateIdentityHomographyJacobian2x8(jacobians[2 * n + 0u], jacobians[2 * n + 1u], points[n].x(), points[n].y());
		}

		performance.stop();

		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < points.size(); ++n)
			{
				const Vector2 point = points[n];
				const Vector2 transformedPoint(homography * point);

				for (unsigned int i = 0u; i < 8u; ++i)
				{
					SquareMatrix3 homographyDelta(homography);
					homographyDelta[i] += Numeric::weakEps();

					const Vector2 pointDelta(homographyDelta * point);
					const Vector2 derivative = (pointDelta - transformedPoint) / Numeric::weakEps();

					naiveJacobians[2u * n + 0u][i] = derivative.x();
					naiveJacobians[2u * n + 1u][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < points.size(); ++n)
		{
			const Vector2 point = points[n];
			const Vector2 transformedPoint(homography * point);

			const Scalar* jacobianX = jacobians[2u * n + 0u];
			const Scalar* jacobianY = jacobians[2u * n + 1u];

			for (unsigned int h = 0u; h < 8u; ++h)
			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dh0 .. dh7
					SquareMatrix3 homographyDelta(homography);
					homographyDelta[h] += epsilon;

					const Vector2 transformedPointDelta(homographyDelta * point);

					if (checkAccuracy(transformedPoint, transformedPointDelta, epsilon, jacobianX[h], jacobianY[h]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testIdentityHomography2x9(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing identity homography Jacobian 2x9 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	Vectors2 points(numberPoints);

	Matrix jacobians(2 * numberPoints, 9);
	Matrix naiveJacobians(2 * numberPoints, 9);

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		unsigned int width = 1920u;
		unsigned int height = 1080u;

		const SquareMatrix3 homography(true);

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			points[n] = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
		}

		performance.start();

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			Geometry::Jacobian::calculateIdentityHomographyJacobian2x9(jacobians[2 * n + 0u], jacobians[2 * n + 1u], points[n].x(), points[n].y());
		}

		performance.stop();

		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < points.size(); ++n)
			{
				const Vector2 point = points[n];
				const Vector2 transformedPoint(homography * point);

				for (unsigned int i = 0u; i < 9u; ++i)
				{
					SquareMatrix3 homographyDelta(homography);
					homographyDelta[i] += Numeric::weakEps();

					const Vector2 pointDelta(homographyDelta * point);
					const Vector2 derivative = (pointDelta - transformedPoint) / Numeric::weakEps();

					naiveJacobians[2u * n + 0u][i] = derivative.x();
					naiveJacobians[2u * n + 1u][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < points.size(); ++n)
		{
			const Vector2 point = points[n];
			const Vector2 transformedPoint(homography * point);

			const Scalar* jacobianX = jacobians[2u * n + 0u];
			const Scalar* jacobianY = jacobians[2u * n + 1u];

			for (unsigned int h = 0u; h < 9u; ++h)
			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / dh0 .. dh8
					SquareMatrix3 homographyDelta(homography);
					homographyDelta[h] += epsilon;

					const Vector2 transformedPointDelta(homographyDelta * point);

					if (checkAccuracy(transformedPoint, transformedPointDelta, epsilon, jacobianX[h], jacobianY[h]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

bool TestJacobian::testSimilarity2x4(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	Log::info() << "Testing similarity Jacobian 2x4 for " << numberPoints << " points:";

	const Scalars epsilons = {Numeric::weakEps(), Numeric::weakEps() / Scalar(10), Numeric::weakEps() * Scalar(10), Numeric::weakEps() / Scalar(100), Numeric::weakEps() * Scalar(100)};

	uint64_t succeeded = 0ull;
	uint64_t iterations = 0ull;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	Vectors2 points(numberPoints);

	Matrix jacobians(2 * numberPoints, 4);
	Matrix naiveJacobians(2 * numberPoints, 4);

	const Timestamp startTimestamp(true);

	do
	{
		bool accurate = true;

		unsigned int width = 1920u;
		unsigned int height = 1080u;

		const Vector2 translation = Random::vector2(-10, 10);
		const Scalar rotation = Random::scalar(0, Numeric::pi2());
		const Scalar scale = Random::scalar(Scalar(0.01), 5);

		const Vector2 xAxis(Numeric::cos(rotation), Numeric::sin(rotation));

		SquareMatrix3 similarity(true);
		similarity(0, 0) = xAxis.x() * scale;
		similarity(1, 0) = xAxis.y() * scale;
		similarity(0, 1) = -xAxis.y() * scale;
		similarity(1, 1) = xAxis.x() * scale;
		similarity(0, 2) = translation.x();
		similarity(1, 2) = translation.y();

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			points[n] = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));
		}

		performance.start();

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			Geometry::Jacobian::calculateSimilarityJacobian2x4(jacobians[2 * n + 0u], jacobians[2 * n + 1u], points[n].x(), points[n].y(), similarity);
		}

		performance.stop();

		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < points.size(); ++n)
			{
				const Vector2 point = points[n];
				const Vector2 transformedPoint(similarity * point);

				for (unsigned int i = 0u; i < 4u; ++i)
				{
					SquareMatrix3 similarityDelta(similarity);

					switch (i)
					{
						case 0u:
							similarityDelta(0, 0) += Numeric::weakEps();
							similarityDelta(1, 1) += Numeric::weakEps();
							break;

						case 1u:
							similarityDelta(1, 0) += Numeric::weakEps();
							similarityDelta(0, 1) -= Numeric::weakEps();
							break;

						case 2u:
							similarityDelta(0, 2) += Numeric::weakEps();
							break;

						case 3u:
							similarityDelta(1, 2) += Numeric::weakEps();
							break;
					}

					const Vector2 pointDelta(similarityDelta * point);
					const Vector2 derivative = (pointDelta - transformedPoint) / Numeric::weakEps();

					naiveJacobians[2u * n + 0u][i] = derivative.x();
					naiveJacobians[2u * n + 1u][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < points.size(); ++n)
		{
			const Vector2 point = points[n];
			const Vector2 transformedPoint(similarity * point);

			const Scalar* jacobianX = jacobians[2u * n + 0u];
			const Scalar* jacobianY = jacobians[2u * n + 1u];

			for (unsigned int s = 0u; s < 4u; ++s)
			{
				bool localAccuracy = false;

				for (const Scalar epsilon : epsilons)
				{
					// df / ds0 .. ds3
					SquareMatrix3 similarityDelta(similarity);

					switch (s)
					{
						case 0u:
							similarityDelta(0, 0) += epsilon;
							similarityDelta(1, 1) += epsilon;
							break;

						case 1u:
							similarityDelta(1, 0) += epsilon;
							similarityDelta(0, 1) -= epsilon;
							break;

						case 2u:
							similarityDelta(0, 2) += epsilon;
							break;

						case 3u:
							similarityDelta(1, 2) += epsilon;
							break;
					}

					const Vector2 transformedPointDelta(similarityDelta * point);

					if (checkAccuracy(transformedPoint, transformedPointDelta, epsilon, jacobianX[s], jacobianY[s]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					accurate = false;
				}
			}

			if (accurate)
			{
				++succeeded;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(succeeded) / double(iterations);

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	const bool allSucceeded = percent >= successThreshold();

	if (!allSucceeded)
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return allSucceeded;
}

template <typename T>
bool TestJacobian::testCalculateFisheyeDistortNormalized2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing fisheye Jacobian 2x2 for " << numberPoints << " points with " << sizeof(T) * 8 << "-bit precision:";

	constexpr std::array<double, 5> epsilons = {NumericD::weakEps(), NumericD::weakEps() / 10.0, NumericD::weakEps() * 10.0, NumericD::weakEps() / 100.0, NumericD::weakEps() * 100.0};

	RandomGenerator randomGenerator;

	constexpr double threshold = std::is_same<float, T>::value ? 0.95 : 0.99;

	ValidationPrecision validation(threshold, randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		MatrixT<T> jacobians(2 * numberPoints, 2);
		MatrixT<T> naiveJacobians(2 * numberPoints, 2);

		VectorsT2<T> points(numberPoints);

		std::vector<T> radialDistortions(6);
		std::vector<T> tangentialDistortions(2);

		for (T& radialDistortion : radialDistortions)
		{
			radialDistortion = RandomT<T>::scalar(T(-0.001), T(0.001));
		}

		for (T& tangentialDistortion : tangentialDistortions)
		{
			tangentialDistortion = RandomT<T>::scalar(T(-0.001), T(0.001));
		}

		const FisheyeCameraT<T> fisheyeCamera(1920u, 1080u, T(1000.0), T(1000.0), T(960.0), T(540.0), radialDistortions.data(), tangentialDistortions.data());

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			points[n] = RandomT<T>::vector2(T(-1.0), T(1.0), T(-1.0), T(1.0));
		}

		performance.start();

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			Geometry::Jacobian::calculateFisheyeDistortNormalized2x2(jacobians[2 * n + 0u], jacobians[2 * n + 1u], points[n].x(), points[n].y(), radialDistortions.data(), tangentialDistortions.data());
		}

		performance.stop();

		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < points.size(); ++n)
			{
				const VectorT2<T> point = points[n];
				const VectorT2<T> transformedPoint = fisheyeCamera.distortNormalized(points[n]);

				for (unsigned int i = 0u; i < 2u; ++i)
				{
					VectorT2<T> pointDelta(point);
					pointDelta[i] += NumericT<T>::weakEps();

					const VectorT2<T> transformedPointDelta = fisheyeCamera.distortNormalized(pointDelta);

					const VectorT2<T> derivative = (transformedPointDelta - transformedPoint) / NumericT<T>::weakEps();

					naiveJacobians[2u * n + 0u][i] = derivative.x();
					naiveJacobians[2u * n + 1u][i] = derivative.y();
				}
			}
		}

		const FisheyeCameraD fisheyeCameraD(fisheyeCamera);

		for (size_t n = 0; n < points.size(); ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const VectorD2 point = VectorD2(points[n]);
			const VectorD2 transformedPoint = fisheyeCameraD.distortNormalized(point);

			const T* jacobianX = jacobians[2u * n + 0u];
			const T* jacobianY = jacobians[2u * n + 1u];

			for (unsigned int s = 0u; s < 2u; ++s)
			{
				bool localAccuracy = false;

				for (const double epsilon : epsilons)
				{
					VectorD2 pointDelta(point);
					pointDelta[s] += epsilon;

					const VectorD2 transformedPointDelta = fisheyeCameraD.distortNormalized(pointDelta);

					if (checkAccuracy(transformedPoint, transformedPointDelta, epsilon, jacobianX[s], jacobianY[s]))
					{
						localAccuracy = true;
						break;
					}
				}

				if (!localAccuracy)
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds() << "ms";
	Log::info() << "Performance: " << performance.averageMseconds() << "ms";
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
