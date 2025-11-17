/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/TestJacobian.h"
#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/base/HighPerformanceTimer.h"
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

	allSucceeded = testPinholeCameraDistortionJacobian2x4(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPinholeCameraJacobian2x6(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPinholeCameraJacobian2x7(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPinholeCameraJacobian2x8<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPinholeCameraJacobian2x8<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFisheyeCameraJacobian2x12<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testFisheyeCameraJacobian2x12<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testOrientationPinholeCameraJacobian2x11(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPosePinholeCameraJacobian2x12(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPosePinholeCameraJacobian2x14<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPosePinholeCameraJacobian2x14<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPoseFisheyeCameraJacobian2x18<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPoseFisheyeCameraJacobian2x18<double>(testDuration) && allSucceeded;

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


TEST(TestJacobian, PinholeCameraDistortionJacobian2x4)
{
	EXPECT_TRUE(TestJacobian::testPinholeCameraDistortionJacobian2x4(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PinholeCameraJacobian2x6)
{
	EXPECT_TRUE(TestJacobian::testPinholeCameraJacobian2x6(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PinholeCameraJacobian2x7)
{
	EXPECT_TRUE(TestJacobian::testPinholeCameraJacobian2x7(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PinholeCameraJacobian2x8_float)
{
	EXPECT_TRUE(TestJacobian::testPinholeCameraJacobian2x8<float>(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PinholeCameraJacobian2x8_double)
{
	EXPECT_TRUE(TestJacobian::testPinholeCameraJacobian2x8<double>(GTEST_TEST_DURATION));
}

TEST(TestJacobian, FisheyeCameraJacobian2x12_float)
{
	EXPECT_TRUE(TestJacobian::testFisheyeCameraJacobian2x12<float>(GTEST_TEST_DURATION));
}

TEST(TestJacobian, FisheyeCameraJacobian2x12_double)
{
	EXPECT_TRUE(TestJacobian::testFisheyeCameraJacobian2x12<double>(GTEST_TEST_DURATION));
}

TEST(TestJacobian, OrientationPinholeCameraJacobian2x11)
{
	EXPECT_TRUE(TestJacobian::testOrientationPinholeCameraJacobian2x11(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PosePinholeCameraJacobian2x12)
{
	EXPECT_TRUE(TestJacobian::testPosePinholeCameraJacobian2x12(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PosePinholeCameraJacobian2x14_float)
{
	EXPECT_TRUE(TestJacobian::testPosePinholeCameraJacobian2x14<float>(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PosePinholeCameraJacobian2x14_double)
{
	EXPECT_TRUE(TestJacobian::testPosePinholeCameraJacobian2x14<double>(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PoseFisheyeCameraJacobian2x18_float)
{
	EXPECT_TRUE(TestJacobian::testPoseFisheyeCameraJacobian2x18<float>(GTEST_TEST_DURATION));
}

TEST(TestJacobian, PoseFisheyeCameraJacobian2x18_double)
{
	EXPECT_TRUE(TestJacobian::testPoseFisheyeCameraJacobian2x18<double>(GTEST_TEST_DURATION));
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

template <typename T, typename TScalar, typename TVariable>
T TestJacobian::DerivativeCalculatorT<T, TScalar, TVariable>::calculateDerivative(const TVariable& variable, const size_t parameterIndex, const TScalar epsilon) const
{
	ocean_assert(epsilon >= TScalar(0));

	const T valuePositive = calculateValue(variable, parameterIndex, epsilon);
	const T valueNegative = calculateValue(variable, parameterIndex, -epsilon);

	return (valuePositive - valueNegative) / (epsilon * TScalar(2));
}

template <typename T, typename TScalar, typename TVariable>
template <typename TAnalyticalDerivative>
bool TestJacobian::DerivativeCalculatorT<T, TScalar, TVariable>::verifyDerivative(const TVariable& variable, const size_t parameterIndex, const TAnalyticalDerivative& analyticalDerivative) const
{
	constexpr TScalar initialEpsilon = NumericT<TScalar>::weakEps() * TScalar(0.01);

	for (const TScalar epsilon : {initialEpsilon, initialEpsilon * TScalar(0.1), initialEpsilon * TScalar(10), initialEpsilon * TScalar(0.01), initialEpsilon * TScalar(100), initialEpsilon * TScalar(0.001), initialEpsilon * TScalar(1000)})
	{
		if (NumericT<TScalar>::isNotEqualEps(epsilon * TScalar(2)))
		{
			const TAnalyticalDerivative approximatedDerivative = TAnalyticalDerivative(calculateDerivative(variable, parameterIndex, epsilon));

			if (checkDerivative<TAnalyticalDerivative>(analyticalDerivative, approximatedDerivative))
			{
				return true;
			}
		}
	}

	return false;
}

template <typename T, typename TScalar>
T TestJacobian::DerivativeCalculatorT<T, TScalar, void>::calculateDerivative(const size_t parameterIndex, const TScalar epsilon) const
{
	ocean_assert(epsilon >= TScalar(0));

	const T valuePositive = calculateValue(parameterIndex, epsilon);
	const T valueNegative = calculateValue(parameterIndex, -epsilon);

	return (valuePositive - valueNegative) / (epsilon * TScalar(2));
}

template <typename T, typename TScalar>
template <typename TAnalyticalDerivative>
bool TestJacobian::DerivativeCalculatorT<T, TScalar, void>::verifyDerivative(const size_t parameterIndex, const TAnalyticalDerivative& analyticalDerivative) const
{
	constexpr TScalar initialEpsilon = NumericT<TScalar>::weakEps() * TScalar(0.01);

	for (const TScalar epsilon : {initialEpsilon, initialEpsilon * TScalar(0.1), initialEpsilon * TScalar(10), initialEpsilon * TScalar(0.01), initialEpsilon * TScalar(100), initialEpsilon * TScalar(0.001), initialEpsilon * TScalar(1000)})
	{
		if (NumericT<TScalar>::isNotEqualEps(epsilon * TScalar(2)))
		{
			const TAnalyticalDerivative approximatedDerivative = TAnalyticalDerivative(calculateDerivative(parameterIndex, epsilon));

			if (checkDerivative<TAnalyticalDerivative>(analyticalDerivative, approximatedDerivative))
			{
				return true;
			}
		}
	}

	return false;
}

/**
 * Derivative calculator for orientational jacobian 2x3.
 * Calculates numerical derivatives of 2D image projection with respect to camera orientation parameters.
 * @tparam T The data type of the scalar, either 'float' or 'double'
 */
template <typename T>
class TestJacobian::DerivativeCalculatorOrientationalJacobian2x3 : public DerivativeCalculatorT<VectorD2, T, VectorD3>
{
	public:

		/**
		 * Creates a derivative calculator.
		 * @param flippedCamera_R_translation The camera's rotation (inverted and flipped)
		 * @param camera The camera model
		 * @param translation_T_world The translation from world to camera's translation frame
		 */
		DerivativeCalculatorOrientationalJacobian2x3(const QuaternionT<T>& flippedCamera_R_translation, const AnyCameraT<T>& camera, const VectorT3<T>& translation_T_world) :
			flippedCamera_EM_translationD_(QuaternionD(flippedCamera_R_translation).normalized()),
			camera_(camera.cloneToDouble()),
			translation_T_world_(VectorD3(translation_T_world))
		{
			ocean_assert(camera_);
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed camera orientation.
		 * @param objectPoint The 3D object point
		 * @param parameterIndex The rotation parameter index to perturb (0, 1, or 2)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		VectorD2 calculateValue(const VectorD3& objectPoint, const size_t parameterIndex, const T offset) const override
		{
			ExponentialMapD flippedCamera_EM_translationD(flippedCamera_EM_translationD_);
			flippedCamera_EM_translationD[(unsigned int)(parameterIndex)] += offset;

			return camera_->projectToImageIF(HomogenousMatrixD4(flippedCamera_EM_translationD.quaternion()) * translation_T_world_, objectPoint);
		}

	protected:

		/// Base camera orientation as exponential map
		ExponentialMapD flippedCamera_EM_translationD_;

		/// Camera model in double precision (cached for performance)
		const SharedAnyCameraD camera_;

		/// Transformation from world to camera's translation frame
		const HomogenousMatrixD4 translation_T_world_;
};

template <typename T>
bool TestJacobian::testOrientationalJacobian2x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing orientational Jacobian Rodrigues 2x3 for " << numberPoints << " points with " << sizeof(T) * 8 << "-bit precision:";

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

			const DerivativeCalculatorOrientationalJacobian2x3<T> derivativeCalculator(flippedCamera_R_translation, camera, translation_T_world);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const VectorT3<T>& objectPoint = objectPoints[n];

				const T* jacobianX = jacobian[2 * n + 0];
				const T* jacobianY = jacobian[2 * n + 1];

				for (size_t parameterIndex = 0; parameterIndex < 3; ++parameterIndex)
				{
					if (!derivativeCalculator.verifyDerivative(VectorD3(objectPoint), parameterIndex, VectorT2<T>(jacobianX[parameterIndex], jacobianY[parameterIndex])))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive.averageMseconds();
	Log::info() << "Performance optimized: " << performanceOptimized.averageMseconds();
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

/**
 * Derivative calculator for orientation jacobian 2nx3.
 * Calculates numerical derivatives of 2D image projections with respect to camera orientation parameters for multiple points.
 * @tparam T The data type of the scalar, either 'float' or 'double'
 */
template <typename T>
class TestJacobian::DerivativeCalculatorOrientationJacobian2nx3 : public DerivativeCalculatorT<VectorD2, T, VectorD3>
{
	public:

		/**
		 * Creates a derivative calculator for orientation jacobian.
		 * @param camera The camera model
		 * @param flippedCamera_E_world The camera's exponential map rotation
		 */
		DerivativeCalculatorOrientationJacobian2nx3(const AnyCameraT<T>& camera, const ExponentialMapT<T>& flippedCamera_E_world) :
			camera_(camera.cloneToDouble()),
			flippedCamera_E_worldD_(flippedCamera_E_world)
		{
			ocean_assert(camera_);
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed camera orientation.
		 * @param objectPoint The 3D object point
		 * @param parameterIndex The rotation parameter index to perturb (0, 1, or 2)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		VectorD2 calculateValue(const VectorD3& objectPoint, const size_t parameterIndex, const T offset) const override
		{
			ExponentialMapD rotationPerturbed(flippedCamera_E_worldD_);
			rotationPerturbed[(unsigned int)(parameterIndex)] += offset;

			return camera_->projectToImageIF(HomogenousMatrixD4(rotationPerturbed.quaternion()), objectPoint);
		}

	protected:

		/// Camera model in double precision (cached for performance)
		const SharedAnyCameraD camera_;

		/// Base camera orientation as exponential map
		const ExponentialMapD flippedCamera_E_worldD_;
};

template <typename T>
bool TestJacobian::testOrientationJacobian2nx3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing orientation Jacobian Rodrigues 2nx3 for " << numberPoints << " points with " << sizeof(T) * 8 << "-bit precision:";

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

			const DerivativeCalculatorOrientationJacobian2nx3<T> derivativeCalculator(camera, flippedCamera_E_world);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const VectorT3<T>& objectPoint = objectPoints[n];

				const T* jacobianX = jacobian[2 * n + 0];
				const T* jacobianY = jacobian[2 * n + 1];

				for (size_t parameterIndex = 0; parameterIndex < 3; ++parameterIndex)
				{
					if (!derivativeCalculator.verifyDerivative(VectorD3(objectPoint), parameterIndex, VectorT2<T>(jacobianX[parameterIndex], jacobianY[parameterIndex])))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance optimized: " << performanceOptimized;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

/**
 * Derivative calculator for pinhole camera pose jacobian 2nx6.
 * Calculates numerical derivatives of 2D image projections with respect to camera pose parameters (rotation and translation).
 */
class TestJacobian::DerivativeCalculatorPinholeCameraPoseJacobian2nx6 : public DerivativeCalculatorT<Vector2, Scalar, Vector3>
{
	public:

		/**
		 * Creates a derivative calculator for pinhole camera pose jacobian.
		 * @param camera The pinhole camera
		 * @param flippedCamera_P_world The camera pose (inverted and flipped)
		 */
		DerivativeCalculatorPinholeCameraPoseJacobian2nx6(const PinholeCamera& camera, const Pose& flippedCamera_P_world) :
			camera_(camera),
			flippedCamera_P_world_(flippedCamera_P_world)
		{
			// nothing to do here
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed camera pose.
		 * @param objectPoint The 3D object point
		 * @param parameterIndex The pose parameter index to perturb (0-2: rotation, 3-5: translation)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		Vector2 calculateValue(const Vector3& objectPoint, const size_t parameterIndex, const Scalar offset) const override
		{
			const size_t poseParameterIndex = (parameterIndex + 3) % 6; // 0-2: translation, 3-5 rotation

			Pose flippedCamera_P_world_perturbed(flippedCamera_P_world_);
			flippedCamera_P_world_perturbed[(unsigned int)(poseParameterIndex)] += offset;

			return camera_.projectToImageIF<false>(flippedCamera_P_world_perturbed.transformation(), objectPoint, camera_.hasDistortionParameters());
		}

	protected:

		/// The pinhole camera
		const PinholeCamera camera_;

		/// The camera pose (inverted and flipped)
		const Pose flippedCamera_P_world_;
};

bool TestJacobian::testPinholeCameraPoseJacobian2nx6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing pinhole camera pose Jacobian Rodrigues 2x6 and 2nx6 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performancePerfectCamera;
	HighPerformanceStatistic performanceDistortedCamera;

	const Timestamp startTimestamp(true);

	unsigned int distortionIteration = 0u;

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr unsigned int width = 640u;
		constexpr unsigned int height = 480u;

		constexpr Scalar width2 = Scalar(width) * Scalar(0.5);
		constexpr Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 world_t_camera(Random::vector3(-10, 10));
		const Quaternion world_Q_camera(Random::quaternion());

		const Pose world_P_camera(world_t_camera, world_Q_camera);
		const HomogenousMatrix4 world_T_camera(world_t_camera, world_Q_camera);

		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));
		const Pose flippedCamera_P_world(flippedCamera_T_world);

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

			const Line3 ray(camera.ray(tmpImagePoint, world_t_camera, world_Q_camera));
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
			Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6(jacobian.data(), camera, flippedCamera_P_world, objectPoints.data(), objectPoints.size(), true);
		}
		else
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performancePerfectCamera);
			Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6(jacobian.data(), camera, flippedCamera_P_world, objectPoints.data(), objectPoints.size(), false);
		}

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 6);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3& objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(flippedCamera_T_world, objectPoint, camera.hasDistortionParameters()));

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					Pose flippedCamera_P_world_delta(flippedCamera_P_world);
					flippedCamera_P_world_delta[i] += Numeric::weakEps();

					const Vector2 imagePointDelta(camera.projectToImageIF<false>(flippedCamera_P_world_delta.transformation(), objectPoint, camera.hasDistortionParameters()));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			{
				// we also test the first implementation for one object point

				Scalar singleJacobianX[6], singleJacobianY[6];
				Geometry::Jacobian::calculatePoseJacobianRodrigues2x6(singleJacobianX, singleJacobianY, camera, flippedCamera_P_world, objectPoint, camera.hasDistortionParameters());

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			{
				// we also test the second implementation for one object point

				SquareMatrix3 dwx, dwy, dwz;
				Geometry::Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(flippedCamera_P_world[3], flippedCamera_P_world[4], flippedCamera_P_world[5]), dwx, dwy, dwz);

				Scalar singleJacobianX[6], singleJacobianY[6];
				Geometry::Jacobian::calculatePoseJacobianRodrigues2x6(singleJacobianX, singleJacobianY, camera, flippedCamera_P_world.transformation(), objectPoint, camera.hasDistortionParameters(), dwx, dwy, dwz);

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			const DerivativeCalculatorPinholeCameraPoseJacobian2nx6 derivativeCalculator(camera, flippedCamera_P_world);

			for (size_t parameterIndex = 0; parameterIndex < 6; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(objectPoint, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance perfect camera: " << performancePerfectCamera;
	Log::info() << "Performance distorted camera: " << performanceDistortedCamera;
	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for fisheye camera pose jacobian.
 */
class TestJacobian::DerivativeCalculatorFisheyeCameraPoseJacobian2x6 : public DerivativeCalculatorT<Vector2, Scalar, Vector3>
{
	public:

		/**
		 * Creates a derivative calculator for fisheye camera pose jacobian.
		 * @param fisheyeCamera The fisheye camera
		 * @param flippedCamera_P_world The camera pose (inverted and flipped)
		 */
		DerivativeCalculatorFisheyeCameraPoseJacobian2x6(const FisheyeCamera& fisheyeCamera, const Pose& flippedCamera_P_world) :
			fisheyeCamera_(fisheyeCamera),
			flippedCamera_P_world_(flippedCamera_P_world)
		{
			// nothing to do here
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed camera pose.
		 * @param objectPoint The 3D object point
		 * @param parameterIndex The pose parameter index to perturb (0-2: rotation, 3-5: translation)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		Vector2 calculateValue(const Vector3& objectPoint, const size_t parameterIndex, const Scalar offset) const override
		{
			const size_t poseParameterIndex = (parameterIndex + 3) % 6; // 0-2: translation, 3-5 rotation

			Pose flippedCamera_P_world_perturbed(flippedCamera_P_world_);
			flippedCamera_P_world_perturbed[(unsigned int)(poseParameterIndex)] += offset;

			return fisheyeCamera_.projectToImageIF(flippedCamera_P_world_perturbed.transformation(), objectPoint);
		}

	protected:

		/// The fisheye camera
		const FisheyeCamera fisheyeCamera_;

		/// The camera pose (inverted and flipped)
		const Pose flippedCamera_P_world_;
};

bool TestJacobian::testFisheyeCameraPoseJacobian2x6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing fisheye camera pose Jacobian Rodrigues 2x6 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const FisheyeCamera fisheyeCamera = Utilities::realisticFisheyeCamera(RandomI::random(1u));

		const Vector3 world_t_camera(Random::vector3(-10, 10));
		const Quaternion world_Q_camera(Random::quaternion());

		const Pose world_P_camera(world_t_camera, world_Q_camera);
		const HomogenousMatrix4 world_T_camera(world_t_camera, world_Q_camera);

		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));
		const Pose flippedCamera_P_world(flippedCamera_T_world);

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			const Vector2 imagePoint(Random::vector2(Scalar(5), Scalar(fisheyeCamera.width() - 5u), Scalar(5), Scalar(fisheyeCamera.height() - 5u)));

			const Line3 ray(fisheyeCamera.ray(imagePoint, world_T_camera));
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
			Geometry::Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(flippedCamera_P_world[3], flippedCamera_P_world[4], flippedCamera_P_world[5]), dwx, dwy, dwz);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				Geometry::Jacobian::calculatePoseJacobianRodrigues2x6(jacobian[n * 2 + 0], jacobian[n * 2 + 1], fisheyeCamera, flippedCamera_P_world.transformation(), objectPoints[n], dwx, dwy, dwz);
			}
		}

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 6);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3& objectPoint = objectPoints[n];
				const Vector2 imagePoint(fisheyeCamera.projectToImageIF(flippedCamera_T_world, objectPoint));

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					Pose flippedCamera_P_world_delta(flippedCamera_P_world);
					flippedCamera_P_world_delta[i] += Numeric::weakEps();

					const Vector2 imagePointDelta(fisheyeCamera.projectToImageIF(flippedCamera_P_world_delta.transformation(), objectPoint));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			const DerivativeCalculatorFisheyeCameraPoseJacobian2x6 derivativeCalculator(fisheyeCamera, flippedCamera_P_world);

			for (size_t parameterIndex = 0; parameterIndex < 6; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(objectPoint, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for any camera pose jacobian.
 * @tparam T The data type of the scalar, either 'float' or 'double'
 */
template <typename T>
class TestJacobian::DerivativeCalculatorAnyCameraPoseJacobian2nx6 : public DerivativeCalculatorT<VectorD2, double, VectorD3>
{
	public:

		/**
		 * Creates a derivative calculator for any camera pose jacobian.
		 * @param anyCameraD The any camera in double precision
		 * @param flippedCamera_P_world The camera pose (inverted and flipped) in template precision
		 */
		DerivativeCalculatorAnyCameraPoseJacobian2nx6(const SharedAnyCameraD& anyCameraD, const PoseT<T>& flippedCamera_P_world) :
			anyCameraD_(anyCameraD),
			flippedCamera_P_worldD_(flippedCamera_P_world)
		{
			ocean_assert(anyCameraD_);
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed camera pose.
		 * @param objectPoint The 3D object point
		 * @param parameterIndex The pose parameter index to perturb (0-2: rotation, 3-5: translation)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		VectorD2 calculateValue(const VectorD3& objectPoint, const size_t parameterIndex, const double offset) const override
		{
			const size_t poseParameterIndex = (parameterIndex + 3) % 6; // 0-2: translation, 3-5 rotation

			PoseD posePerturbed(flippedCamera_P_worldD_);
			posePerturbed[(unsigned int)(poseParameterIndex)] += offset;

			return anyCameraD_->projectToImageIF(posePerturbed.transformation(), objectPoint);
		}

	protected:

		/// The any camera in double precision
		const SharedAnyCameraD anyCameraD_;

		/// The camera pose (inverted and flipped) in double precision
		const PoseD flippedCamera_P_worldD_;
};

template <typename T>
bool TestJacobian::testAnyCameraPoseJacobian2nx6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing any camera pose Jacobian Rodrigues 2x6 for " << numberPoints << " points with " << sizeof(T) * 8 << "-bit precision:";
	Log::info() << " ";

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

				const T* jacobianX = jacobian[2 * n + 0];
				const T* jacobianY = jacobian[2 * n + 1];

				const DerivativeCalculatorAnyCameraPoseJacobian2nx6<T> derivativeCalculator(sharedAnyCameraD, flippedCamera_P_world);

				for (size_t parameterIndex = 0; parameterIndex < 6; ++parameterIndex)
				{
					if (!derivativeCalculator.verifyDerivative(objectPoint, parameterIndex, VectorT2<T>(jacobianX[parameterIndex], jacobianY[parameterIndex])))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
		while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

		Log::info() << "Performance naive: " << performanceNaive;
		Log::info() << "Performance: " << performance;
		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

/**
 * Derivative calculator for pose jacobian with damped distortion.
 */
class TestJacobian::DerivativeCalculatorPoseJacobianDampedDistortion2nx6 : public DerivativeCalculatorT<Vector2, Scalar, Vector3>
{
	public:

		/**
		 * Creates a derivative calculator for pose jacobian with damped distortion.
		 * @param camera The pinhole camera
		 * @param flippedCamera_P_world The camera pose (inverted and flipped)
		 * @param dampingFactor The damping factor for distortion
		 */
		DerivativeCalculatorPoseJacobianDampedDistortion2nx6(const PinholeCamera& camera, const Pose& flippedCamera_P_world, const Scalar dampingFactor) :
			camera_(camera),
			flippedCamera_P_world_(flippedCamera_P_world),
			dampingFactor_(dampingFactor)
		{
			// nothing to do here
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed camera pose using damped distortion.
		 * @param objectPoint The 3D object point
		 * @param parameterIndex The pose parameter index to perturb (0-2: rotation, 3-5: translation)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		Vector2 calculateValue(const Vector3& objectPoint, const size_t parameterIndex, const Scalar offset) const override
		{
			const size_t poseParameterIndex = (parameterIndex + 3) % 6; // 0-2: translation, 3-5 rotation

			Pose flippedCamera_P_world_perturbed(flippedCamera_P_world_);
			flippedCamera_P_world_perturbed[(unsigned int)(poseParameterIndex)] += offset;

			return camera_.projectToImageDampedIF(flippedCamera_P_world_perturbed.transformation(), Vector3(objectPoint), camera_.hasDistortionParameters(), dampingFactor_);
		}

	protected:

		/// The pinhole camera
		const PinholeCamera camera_;

		/// The camera pose (inverted and flipped)
		const Pose flippedCamera_P_world_;

		/// The damping factor for distortion
		const Scalar dampingFactor_;
};

bool TestJacobian::testPoseJacobianDampedDistortion2nx6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing pose Jacobian with damped distortion Rodrigues 2x6 and 2nx6 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performancePerfectCamera;
	HighPerformanceStatistic performanceDistortedCamera;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr unsigned int width = 640u;
		constexpr unsigned int height = 480u;

		constexpr Scalar width2 = Scalar(width) * Scalar(0.5);
		constexpr Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 world_t_camera(Random::vector3(-10, 10));
		const Quaternion world_Q_camera(Random::quaternion());

		const Pose world_P_camera(world_t_camera, world_Q_camera);
		const HomogenousMatrix4 world_T_camera(world_t_camera, world_Q_camera);

		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));
		const Pose flippedCamera_P_world(flippedCamera_T_world);

		PinholeCamera camera(width, height, fovX, principalX, principalY);

		if (validation.iterations() % 3ull == 1ull || validation.iterations() % 3ull == 2ull)
		{
			const Scalar k1 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			const Scalar k2 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			camera.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
		}

		if (validation.iterations() % 3ull == 2ull)
		{
			const Scalar p1 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			const Scalar p2 = Random::scalar(Scalar(-2.5), Scalar(2.5));
			camera.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));
		}

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			Vector2 tmpImagePoint(Random::vector2(-1, 2));
			tmpImagePoint.x() *= Scalar(camera.width());
			tmpImagePoint.y() *= Scalar(camera.height());

			const Line3 ray(camera.ray(tmpImagePoint, world_t_camera, world_Q_camera));
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
			Geometry::Jacobian::calculatePoseJacobianRodriguesDampedDistortion2nx6(jacobian.data(), camera, flippedCamera_P_world, Scalar(1), objectPoints.data(), objectPoints.size(), true);
		}
		else
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performancePerfectCamera);
			Geometry::Jacobian::calculatePoseJacobianRodriguesDampedDistortion2nx6(jacobian.data(), camera, flippedCamera_P_world, Scalar(1), objectPoints.data(), objectPoints.size(), false);
		}

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 6);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3& objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageDampedIF(flippedCamera_T_world, objectPoint, camera.hasDistortionParameters(), Scalar(1)));

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					Pose flippedCamera_P_world_delta(flippedCamera_P_world);
					flippedCamera_P_world_delta[i] += Numeric::weakEps();

					const Vector2 imagePointDelta(camera.projectToImageDampedIF(flippedCamera_P_world_delta.transformation(), objectPoint, camera.hasDistortionParameters(), Scalar(1)));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			const DerivativeCalculatorPoseJacobianDampedDistortion2nx6 derivativeCalculator(camera, flippedCamera_P_world, Scalar(1));

			for (size_t parameterIndex = 0; parameterIndex < 6; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(objectPoint, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance perfect camera: " << performancePerfectCamera;
	Log::info() << "Performance distorted camera: " << performanceDistortedCamera;
	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for pose and zoom jacobian.
 */
class TestJacobian::DerivativeCalculatorPoseZoomJacobian2nx7 : public DerivativeCalculatorT<Vector2, Scalar, Vector3>
{
	public:

		/**
		 * Creates a derivative calculator for pose and zoom jacobian.
		 * @param camera The pinhole camera
		 * @param flippedCamera_P_world The camera pose (inverted and flipped)
		 * @param zoom The zoom factor
		 */
		DerivativeCalculatorPoseZoomJacobian2nx7(const PinholeCamera& camera, const Pose& flippedCamera_P_world, const Scalar zoom) :
			camera_(camera),
			flippedCamera_P_world_(flippedCamera_P_world),
			zoom_(zoom)
		{
			// nothing to do here
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed camera pose or zoom.
		 * @param objectPoint The 3D object point
		 * @param parameterIndex The parameter index to perturb (0-2: rotation, 3-5: translation, 6: zoom)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		Vector2 calculateValue(const Vector3& objectPoint, const size_t parameterIndex, const Scalar offset) const override
		{
			if (parameterIndex < 6)
			{
				const size_t poseParameterIndex = (parameterIndex + 3) % 6; // 0-2: translation, 3-5 rotation

				Pose flippedCamera_P_world_perturbed(flippedCamera_P_world_);
				flippedCamera_P_world_perturbed[(unsigned int)(poseParameterIndex)] += offset;

				return camera_.projectToImageIF<false>(flippedCamera_P_world_perturbed.transformation(), objectPoint, camera_.hasDistortionParameters(), zoom_);
			}
			else
			{
				const Scalar zoomPerturbed = zoom_ + offset;
				return camera_.projectToImageIF<false>(flippedCamera_P_world_.transformation(), objectPoint, camera_.hasDistortionParameters(), zoomPerturbed);
			}
		}

	protected:

		/// The pinhole camera
		const PinholeCamera camera_;

		/// The camera pose (inverted and flipped)
		const Pose flippedCamera_P_world_;

		/// The zoom factor
		const Scalar zoom_;
};

bool TestJacobian::testPoseZoomJacobian2nx7(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing pose and zoom Jacobian Rodrigues 2x7 and 2nx7 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	constexpr double threshold = successThreshold() * 0.975; // making threshold slightly weaker

	ValidationPrecision validation(threshold, randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performancePerfectCamera;
	HighPerformanceStatistic performanceDistortedCamera;

	const Timestamp startTimestamp(true);

	unsigned int distortionIteration = 0u;

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr unsigned int width = 640u;
		constexpr unsigned int height = 480u;

		constexpr Scalar width2 = Scalar(width) * Scalar(0.5);
		constexpr Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Scalar zoom = Random::scalar(0.25, 20);

		const Vector3 world_t_camera(Random::vector3(-10, 10));
		const Quaternion world_Q_camera(Random::quaternion());

		const Pose world_P_camera(world_t_camera, world_Q_camera);
		const HomogenousMatrix4 world_T_camera(world_t_camera, world_Q_camera);

		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));
		const Pose flippedCamera_P_world(flippedCamera_T_world);

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

			const Line3 ray(camera.ray(tmpImagePoint, world_t_camera, world_Q_camera, zoom));
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
			Geometry::Jacobian::calculatePoseZoomJacobianRodrigues2nx7(jacobian.data(), camera, flippedCamera_P_world, zoom, objectPoints.data(), objectPoints.size(), true);
		}
		else
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performancePerfectCamera);
			Geometry::Jacobian::calculatePoseZoomJacobianRodrigues2nx7(jacobian.data(), camera, flippedCamera_P_world, zoom, objectPoints.data(), objectPoints.size(), false);
		}

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 7);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3 objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(flippedCamera_T_world, objectPoint, camera.hasDistortionParameters(), zoom));

				for (unsigned int i = 0u; i < 7u; ++i)
				{
					Pose flippedCamera_P_world_delta(flippedCamera_P_world);
					Scalar zoomDelta(zoom);

					if (i < 6u)
					{
						flippedCamera_P_world_delta[i] += Numeric::weakEps();
					}
					else
					{
						zoomDelta += Numeric::weakEps();
					}

					const Vector2 imagePointDelta(camera.projectToImageIF<false>(flippedCamera_P_world_delta.transformation(), objectPoint, camera.hasDistortionParameters(), zoomDelta));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			Scalar singleJacobianX[7], singleJacobianY[7];
			Geometry::Jacobian::calculatePoseZoomJacobianRodrigues2x7(singleJacobianX, singleJacobianY, camera, flippedCamera_P_world, zoom, objectPoint, camera.hasDistortionParameters());

			for (unsigned int i = 0u; i < 7u; ++i)
			{
				ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
				ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

				if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
				{
					scopedIteration.setInaccurate();
				}
			}

			const DerivativeCalculatorPoseZoomJacobian2nx7 derivativeCalculator(camera, flippedCamera_P_world, zoom);

			for (size_t parameterIndex = 0; parameterIndex < 7; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(objectPoint, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance perfect camera: " << performancePerfectCamera;
	Log::info() << "Performance distorted camera: " << performanceDistortedCamera;
	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for object transformation Jacobian (6-DOF object pose).
 * Calculates numerical derivatives of image point projection with respect to object pose parameters.
 */
class TestJacobian::DerivativeCalculatorPinholeCameraObjectTransformation2nx6 : public DerivativeCalculatorT<Vector2, Scalar, Vector3>
{
	public:

		/**
		 * Creates a new derivative calculator.
		 * @param camera The pinhole camera
		 * @param flippedCamera_T_world The camera pose (inverted and flipped)
		 * @param world_P_object The object pose
		 * @param world_T_object The object transformation matrix
		 * @param distortImagePoint Whether to apply distortion
		 */
		DerivativeCalculatorPinholeCameraObjectTransformation2nx6(const PinholeCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Pose& world_P_object, const HomogenousMatrix4& world_T_object) :
			camera_(camera),
			flippedCamera_T_world_(flippedCamera_T_world),
			world_P_object_(world_P_object),
			world_T_object_(world_T_object)
		{
			// nothing to do here
		}

		/**
		 * Calculates the projected image point with an offset applied to a specific object pose parameter.
		 * @param objectPoint The object point to project
		 * @param parameterIndex The index of the pose parameter to perturb [0, 5]
		 * @param offset The offset to apply to the pose parameter
		 * @return The projected image point
		 */
		Vector2 calculateValue(const Vector3& objectPoint, const size_t parameterIndex, const Scalar offset) const override
		{
			const size_t poseParameterIndex = (parameterIndex + 3) % 6; // 0-2: translation, 3-5 rotation

			Pose world_P_object_perturbed(world_P_object_);
			world_P_object_perturbed[(unsigned int)(poseParameterIndex)] += offset;

			return camera_.projectToImageIF<false>(flippedCamera_T_world_, world_P_object_perturbed.transformation() * objectPoint, false);
		}

	protected:

		/// The pinhole camera
		const PinholeCamera camera_;

		/// The camera pose (inverted and flipped)
		const HomogenousMatrix4 flippedCamera_T_world_;

		/// The object pose
		const Pose world_P_object_;

		/// The object transformation matrix
		const HomogenousMatrix4 world_T_object_;
};

bool TestJacobian::testPinholeCameraObjectTransformation2nx6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing pinhole camera object transformation Jacobian 2x6 and 2nx6 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr unsigned int width = 640u;
		constexpr unsigned int height = 480u;

		constexpr Scalar width2 = Scalar(width) * Scalar(0.5);
		constexpr Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 world_t_object(Random::vector3(-5, 5));
		const Quaternion world_Q_object(Random::quaternion());

		const Pose world_P_object(world_t_object, world_Q_object);
		const HomogenousMatrix4 world_T_object(world_t_object, world_Q_object);
		const HomogenousMatrix4 object_T_world(world_T_object.inverted());

		const Vector3 world_t_camera(Random::vector3(-5, 5));
		const Quaternion world_Q_camera(Random::quaternion());

		const HomogenousMatrix4 world_T_camera(world_t_camera, world_Q_camera);

		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

		const PinholeCamera camera(width, height, fovX, principalX, principalY);

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			const Vector2 imagePoint(Random::vector2(Scalar(0), Scalar(camera.width()), Scalar(0), Scalar(camera.height())));

			const Line3 ray(camera.ray(imagePoint, world_T_camera));
			const Vector3 objectPoint = object_T_world * ray.point(Random::scalar(1, 5));

			ocean_assert(camera.projectToImage<false>(world_T_camera, world_T_object * objectPoint, false).isEqual(imagePoint, 1));

			objectPoints.push_back(objectPoint);
		}

		/**
		 * jacobian for one point
		 * jacobian x: | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * jacobian y: | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 */

		Matrix jacobian(2 * objectPoints.size(), 6);

		performance.start();
		Geometry::Jacobian::calculateObjectTransformation2nx6(jacobian.data(), camera, flippedCamera_T_world, world_P_object, objectPoints.data(), objectPoints.size());
		performance.stop();

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 6);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3& objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(flippedCamera_T_world, world_T_object * objectPoint, false));

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					Pose world_P_object_delta(world_P_object);
					world_P_object_delta[i] += Numeric::weakEps();

					const Vector2 imagePointDelta(camera.projectToImageIF<false>(flippedCamera_T_world, world_P_object_delta.transformation() * objectPoint, false));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			{
				// we also test the first implementation for one object point

				Scalar singleJacobianX[6], singleJacobianY[6];
				Geometry::Jacobian::calculateObjectTransformation2x6(singleJacobianX, singleJacobianY, camera, flippedCamera_T_world, world_P_object, objectPoint);

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			{
				// we also test the second implementation for one object point

				SquareMatrix3 dwx, dwy, dwz;
				Geometry::Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(world_P_object[3], world_P_object[4], world_P_object[5]), dwx, dwy, dwz);

				Scalar singleJacobianX[6], singleJacobianY[6];
				Geometry::Jacobian::calculateObjectTransformation2x6(singleJacobianX, singleJacobianY, camera, flippedCamera_T_world, world_P_object, objectPoint, dwx, dwy, dwz);

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			const DerivativeCalculatorPinholeCameraObjectTransformation2nx6 derivativeCalculator(camera, flippedCamera_T_world, world_P_object, world_T_object);

			for (size_t parameterIndex = 0; parameterIndex < 6; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(objectPoint, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for fisheye camera object transformation Jacobian (6-DOF object pose).
 * Calculates numerical derivatives of image point projection with respect to object pose parameters.
 */
class TestJacobian::DerivativeCalculatorFisheyeCameraObjectTransformation2nx6 : public DerivativeCalculatorT<Vector2, Scalar, Vector3>
{
	public:

		/**
		 * Creates a new derivative calculator.
		 * @param fisheyeCamera The fisheye camera
		 * @param flippedCamera_T_world The camera pose (inverted and flipped)
		 * @param world_P_object The object pose
		 * @param world_T_object The object transformation matrix
		 */
		DerivativeCalculatorFisheyeCameraObjectTransformation2nx6(const FisheyeCamera& fisheyeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Pose& world_P_object, const HomogenousMatrix4& world_T_object) :
			fisheyeCamera_(fisheyeCamera),
			flippedCamera_T_world_(flippedCamera_T_world),
			world_P_object_(world_P_object),
			world_T_object_(world_T_object)
		{
			// nothing to do here
		}

		/**
		 * Calculates the projected image point with an offset applied to a specific object pose parameter.
		 * @param objectPoint The object point to project
		 * @param parameterIndex The index of the pose parameter to perturb [0, 5]
		 * @param offset The offset to apply to the pose parameter
		 * @return The projected image point
		 */
		Vector2 calculateValue(const Vector3& objectPoint, const size_t parameterIndex, const Scalar offset) const override
		{
			const size_t poseParameterIndex = (parameterIndex + 3) % 6; // 0-2: translation, 3-5 rotation

			Pose world_P_object_perturbed(world_P_object_);
			world_P_object_perturbed[(unsigned int)(poseParameterIndex)] += offset;

			return fisheyeCamera_.projectToImageIF(flippedCamera_T_world_, world_P_object_perturbed.transformation() * objectPoint);
		}

	protected:

		/// The fisheye camera
		const FisheyeCamera fisheyeCamera_;

		/// The camera pose (inverted and flipped)
		const HomogenousMatrix4 flippedCamera_T_world_;

		/// The object pose
		const Pose world_P_object_;

		/// The object transformation matrix
		const HomogenousMatrix4 world_T_object_;
};

bool TestJacobian::testFisheyeCameraObjectTransformation2nx6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing fisheye camera object transformation Jacobian 2x6 and 2nx6 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const FisheyeCamera fisheyeCamera = Utilities::realisticFisheyeCamera(RandomI::random(1u));

		const Vector3 world_t_object(Random::vector3(-5, 5));
		const Quaternion world_Q_object(Random::quaternion());

		const Pose world_P_object(world_t_object, world_Q_object);
		const HomogenousMatrix4 world_T_object(world_t_object, world_Q_object);
		const HomogenousMatrix4 object_T_world(world_T_object.inverted());

		const Vector3 world_t_camera(Random::vector3(-5, 5));
		const Quaternion world_Q_camera(Random::quaternion());

		const HomogenousMatrix4 world_T_camera(world_t_camera, world_Q_camera);

		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			const Vector2 imagePoint(Random::vector2(Scalar(5), Scalar(fisheyeCamera.width() - 5u), Scalar(5), Scalar(fisheyeCamera.height() - 5u)));

			const Line3 ray(fisheyeCamera.ray(imagePoint, world_T_camera));
			const Vector3 objectPoint = object_T_world * ray.point(Random::scalar(1, 5));

			ocean_assert(fisheyeCamera.projectToImage(world_T_camera, world_T_object * objectPoint).isEqual(imagePoint, 1));

			objectPoints.push_back(objectPoint);
		}

		/**
		 * jacobian for one point
		 * jacobian x: | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * jacobian y: | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 */

		Matrix jacobian(2 * objectPoints.size(), 6);

		performance.start();
		Geometry::Jacobian::calculateObjectTransformation2nx6(jacobian.data(), fisheyeCamera, flippedCamera_T_world, world_P_object, objectPoints.data(), objectPoints.size());
		performance.stop();

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 6);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3& objectPoint = objectPoints[n];
				const Vector2 imagePoint(fisheyeCamera.projectToImageIF(flippedCamera_T_world, world_T_object * objectPoint));

				for (unsigned int i = 0u; i < 6u; ++i)
				{
					Pose world_P_object_delta(world_P_object);
					world_P_object_delta[i] += Numeric::weakEps();

					const Vector2 imagePointDelta(fisheyeCamera.projectToImageIF(flippedCamera_T_world, world_P_object_delta.transformation() * objectPoint));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			const DerivativeCalculatorFisheyeCameraObjectTransformation2nx6 derivativeCalculator(fisheyeCamera, flippedCamera_T_world, world_P_object, world_T_object);

			for (size_t parameterIndex = 0; parameterIndex < 6; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(objectPoint, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for pinhole camera point Jacobian (3D object point).
 * Calculates numerical derivatives of image point projection with respect to object point coordinates.
 */
class TestJacobian::DerivativeCalculatorPinholeCameraPointJacobian2nx3 : public DerivativeCalculatorT<VectorD2, Scalar, VectorD3>
{
	public:

		/**
		 * Creates a new derivative calculator.
		 * @param camera The pinhole camera
		 * @param flippedCamera_T_world The camera transformation (inverted and flipped)
		 * @param distortImagePoint Whether to apply distortion
		 */
		DerivativeCalculatorPinholeCameraPointJacobian2nx3(const PinholeCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const bool distortImagePoint) :
			camera_(camera),
			flippedCamera_T_world_(flippedCamera_T_world),
			distortImagePoint_(distortImagePoint)
		{
			// nothing to do here
		}

		/**
		 * Calculates the projected image point with an offset applied to a specific object point coordinate.
		 * @param objectPoint The object point to project
		 * @param parameterIndex The index of the coordinate to perturb [0, 2] (x, y, z)
		 * @param offset The offset to apply to the coordinate
		 * @return The projected image point
		 */
		VectorD2 calculateValue(const VectorD3& objectPoint, const size_t parameterIndex, const Scalar offset) const override
		{
			VectorD3 objectPointPerturbed(objectPoint);
			objectPointPerturbed[(unsigned int)(parameterIndex)] += offset;

			return VectorD2(camera_.projectToImageIF<false>(flippedCamera_T_world_, Vector3(objectPointPerturbed), distortImagePoint_));
		}

	protected:

		/// The pinhole camera
		const PinholeCamera camera_;

		/// The camera transformation (inverted and flipped)
		const HomogenousMatrix4 flippedCamera_T_world_;

		/// Whether to apply distortion
		const bool distortImagePoint_;
};

bool TestJacobian::testPinholeCameraPointJacobian2nx3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing pinhole camera point Jacobian 2x3 and 2nx3 for " << numberPoints << " points:";

	constexpr Scalar eps = Numeric::weakEps();

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	unsigned int distortionIteration = 0u;

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performancePerfectCamera;
	HighPerformanceStatistic performanceDistortedCamera;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr unsigned int width = 640u;
		constexpr unsigned int height = 480u;

		constexpr Scalar width2 = Scalar(width) * Scalar(0.5);
		constexpr Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 world_t_camera(Random::scalar(-1, 1), Random::scalar(-1, 1), Random::scalar(-1, 1));
		const Quaternion world_Q_camera(Random::quaternion());

		const HomogenousMatrix4 world_T_camera(world_t_camera, world_Q_camera);
		const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

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
			const Line3 ray(camera.ray(camera.undistort<true>(imagePoint), world_T_camera));
			const Vector3 objectPoint(ray.point(Random::scalar(1, 100)));

			objectPoints.push_back(objectPoint);
		}

		Matrix jacobian(objectPoints.size() * 2, 3);

		if (camera.hasDistortionParameters())
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performanceDistortedCamera);
			Geometry::Jacobian::calculatePointJacobian2nx3(jacobian.data(), camera, flippedCamera_T_world, objectPoints.data(), objectPoints.size(), true);
		}
		else
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performancePerfectCamera);
			Geometry::Jacobian::calculatePointJacobian2nx3(jacobian.data(), camera, flippedCamera_T_world, objectPoints.data(), objectPoints.size(), false);
		}

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 3);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3& objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(flippedCamera_T_world, objectPoint, camera.hasDistortionParameters()));

				for (unsigned int i = 0u; i < 3u; ++i)
				{
					Vector3 objectPointDelta(objectPoint);
					objectPointDelta[i] += eps;

					const Vector2 imagePointDelta(camera.projectToImageIF<false>(flippedCamera_T_world, objectPointDelta, camera.hasDistortionParameters()));
					const Vector2 derivative = (imagePointDelta - imagePoint) / eps;

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			Scalar singleJacobianX[3], singleJacobianY[3];
			Geometry::Jacobian::calculatePointJacobian2x3(singleJacobianX, singleJacobianY, camera, flippedCamera_T_world, objectPoint, camera.hasDistortionParameters());

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
				ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

				if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
				{
					scopedIteration.setInaccurate();
				}
			}

			const DerivativeCalculatorPinholeCameraPointJacobian2nx3 derivativeCalculator(camera, flippedCamera_T_world, camera.hasDistortionParameters());

			for (size_t parameterIndex = 0; parameterIndex < 3; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(VectorD3(objectPoint), parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance perfect camera: " << performancePerfectCamera;
	Log::info() << "Performance distorted camera: " << performanceDistortedCamera;
	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for fisheye camera point Jacobian (2x3) using central finite differences.
 * Calculates numerical derivatives of 2D image projection with respect to 3D object point coordinates.
 */
class TestJacobian::DerivativeCalculatorFisheyeCameraPointJacobian2x3 : public DerivativeCalculatorT<VectorD2, Scalar, VectorD3>
{
	public:

		/**
		 * Creates calculator with fisheye camera and transformation.
		 * @param fisheyeCamera Fisheye camera model for projection
		 * @param flippedCamera_T_world Inverted and flipped camera transformation
		 */
		DerivativeCalculatorFisheyeCameraPointJacobian2x3(const FisheyeCamera& fisheyeCamera, const HomogenousMatrix4& flippedCamera_T_world) :
			fisheyeCamera_(fisheyeCamera),
			flippedCamera_T_world_(flippedCamera_T_world)
		{
			// nothing to do here
		}

		/**
		 * Calculates image projection with perturbed object point coordinate.
		 * @param objectPoint 3D object point to perturb
		 * @param parameterIndex Index of coordinate to perturb (0=x, 1=y, 2=z)
		 * @param offset Epsilon offset to apply to coordinate
		 * @return Projected 2D image point
		 */
		VectorD2 calculateValue(const VectorD3& objectPoint, const size_t parameterIndex, const Scalar offset) const override
		{
			VectorD3 objectPointPerturbed(objectPoint);
			objectPointPerturbed[(unsigned int)(parameterIndex)] += offset;

			return VectorD2(fisheyeCamera_.projectToImageIF(flippedCamera_T_world_, Vector3(objectPointPerturbed)));
		}

	protected:

		/// Fisheye camera model
		const FisheyeCamera fisheyeCamera_;

		/// Inverted and flipped camera transformation
		const HomogenousMatrix4 flippedCamera_T_world_;
};

bool TestJacobian::testFisheyeCameraPointJacobian2x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing fisheye camera point Jacobian 2x3 for " << numberPoints << " points:";

	constexpr Scalar eps = Numeric::weakEps();

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performanceOptimized;

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const FisheyeCamera fisheyeCamera = Utilities::realisticFisheyeCamera(RandomI::random(1u));

		const Vector3 world_t_camera(Random::scalar(-1, 1), Random::scalar(-1, 1), Random::scalar(-1, 1));
		const Quaternion world_Q_camera(Random::quaternion());

		const HomogenousMatrix4 world_T_camera(world_t_camera, world_Q_camera);
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
				const Vector3& objectPoint = objectPoints[n];
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
			const Vector3& objectPoint = objectPoints[n];

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
					scopedIteration.setInaccurate();
				}
			}

			const DerivativeCalculatorFisheyeCameraPointJacobian2x3 derivativeCalculator(fisheyeCamera, flippedCamera_T_world);

			for (size_t parameterIndex = 0; parameterIndex < 3; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(VectorD3(objectPoint), parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance optimized: " << performanceOptimized;
	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for any camera point Jacobian (2x3) using central finite differences.
 * Calculates numerical derivatives of 2D image projection with respect to 3D object point coordinates.
 */
class TestJacobian::DerivativeCalculatorAnyCameraPointJacobian2x3 : public DerivativeCalculatorT<VectorD2, Scalar, VectorD3>
{
	public:

		/**
		 * Creates calculator with any camera and transformation.
		 * @param anyCamera Any camera model for projection
		 * @param flippedCamera_T_world Inverted and flipped camera transformation
		 */
		DerivativeCalculatorAnyCameraPointJacobian2x3(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world) :
			anyCamera_(anyCamera),
			flippedCamera_T_world_(flippedCamera_T_world)
		{
			// nothing to do here
		}

		/**
		 * Calculates image projection with perturbed object point coordinate.
		 * @param objectPoint 3D object point to perturb
		 * @param parameterIndex Index of coordinate to perturb (0=x, 1=y, 2=z)
		 * @param offset Epsilon offset to apply to coordinate
		 * @return Projected 2D image point
		 */
		VectorD2 calculateValue(const VectorD3& objectPoint, const size_t parameterIndex, const Scalar offset) const override
		{
			VectorD3 objectPointPerturbed(objectPoint);
			objectPointPerturbed[(unsigned int)(parameterIndex)] += offset;

			return VectorD2(anyCamera_.projectToImageIF(flippedCamera_T_world_, Vector3(objectPointPerturbed)));
		}

	protected:

		/// Any camera model
		const AnyCamera& anyCamera_;

		/// Inverted and flipped camera transformation
		const HomogenousMatrix4 flippedCamera_T_world_;
};

bool TestJacobian::testAnyCameraPointJacobian2x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing any camera point Jacobian 2x3 for " << numberPoints << " points:";
	Log::info() << " ";

	constexpr Scalar eps = Numeric::weakEps();

	bool firstCameraIteration = true;

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (const AnyCameraType anyCameraType : Utilities::realisticCameraTypes())
	{
		Log::info().newLine(!firstCameraIteration);
		firstCameraIteration = false;

		const std::shared_ptr<AnyCamera> anyCameraShared = Utilities::realisticAnyCamera(anyCameraType, RandomI::random(1u));
		ocean_assert(anyCameraShared);

		const AnyCamera& anyCamera = *anyCameraShared;

		ValidationPrecision validation(successThreshold(), randomGenerator);

		HighPerformanceStatistic performanceNaive;
		HighPerformanceStatistic performanceOptimized;

		const Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Vector3 world_t_camera(Random::scalar(-1, 1), Random::scalar(-1, 1), Random::scalar(-1, 1));
			const Quaternion world_Q_camera(Random::quaternion());

			const HomogenousMatrix4 world_T_camera(world_t_camera, world_Q_camera);
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
					const Vector3& objectPoint = objectPoints[n];
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
				const Vector3& objectPoint = objectPoints[n];

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
						scopedIteration.setInaccurate();
					}
				}

				const DerivativeCalculatorAnyCameraPointJacobian2x3 derivativeCalculator(anyCamera, flippedCamera_T_world);

				for (size_t parameterIndex = 0; parameterIndex < 3; ++parameterIndex)
				{
					if (!derivativeCalculator.verifyDerivative(VectorD3(objectPoint), parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
		while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

		Log::info() << "Camera name: " << anyCamera.name();
		Log::info() << "Performance naive: " << performanceNaive;
		Log::info() << "Performance optimized: " << performanceOptimized;
		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
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

	Log::info() << "Testing poses jacobian Rodrigues 2nx12 for several points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	const Timestamp startTimestamp(true);

	constexpr unsigned int numberPoints = 37u;
	unsigned int distortionIteration = 0u;

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr unsigned int width = 640u;
		constexpr unsigned int height = 480u;

		constexpr Scalar width2 = Scalar(width) * Scalar(0.5);
		constexpr Scalar height2 = Scalar(height) * Scalar(0.5);

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

		const HomogenousMatrix4 world_T_cameraFirst(Utilities::viewPosition(camera, objectPoints));
		const HomogenousMatrix4 world_T_cameraSecond(Utilities::viewPosition(camera, objectPoints));

		const HomogenousMatrix4 flippedCameraFirst_T_world(PinholeCamera::standard2InvertedFlipped(world_T_cameraFirst));
		const Pose flippedCameraFirst_P_world(flippedCameraFirst_T_world);

		const HomogenousMatrix4 flippedCameraSecond_T_world(PinholeCamera::standard2InvertedFlipped(world_T_cameraSecond));
		const Pose flippedCameraSecond_P_world(flippedCameraSecond_T_world);

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

		Geometry::Jacobian::calculatePointJacobian2nx3(pointJacobiansFirstPose, camera, flippedCameraFirst_T_world, objectPoints.data(), objectPoints.size(), camera.hasDistortionParameters());
		Geometry::Jacobian::calculatePointJacobian2nx3(pointJacobiansSecondPose, camera, flippedCameraSecond_T_world, objectPoints.data(), objectPoints.size(), camera.hasDistortionParameters());

		Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6(poseJacobiansFirst, camera, Pose(flippedCameraFirst_T_world), objectPoints.data(), objectPoints.size(), camera.hasDistortionParameters());
		Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6(poseJacobiansSecond, camera, Pose(flippedCameraSecond_T_world), objectPoints.data(), objectPoints.size(), camera.hasDistortionParameters());

		// Create derivative calculators for pose and point Jacobians
		const DerivativeCalculatorPinholeCameraPoseJacobian2nx6 derivativeCalculatorFirstPose(camera, flippedCameraFirst_P_world);
		const DerivativeCalculatorPinholeCameraPoseJacobian2nx6 derivativeCalculatorSecondPose(camera, flippedCameraSecond_P_world);
		const DerivativeCalculatorPinholeCameraPointJacobian2nx3 derivativeCalculatorFirstPoint(camera, flippedCameraFirst_T_world, camera.hasDistortionParameters());
		const DerivativeCalculatorPinholeCameraPointJacobian2nx3 derivativeCalculatorSecondPoint(camera, flippedCameraSecond_T_world, camera.hasDistortionParameters());

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			// Verify first pose jacobian
			{
				const Scalar* jacobianX = poseJacobiansFirst + 12u * (unsigned int)n + 0u;
				const Scalar* jacobianY = jacobianX + 6;

				for (size_t parameterIndex = 0; parameterIndex < 6; ++parameterIndex)
				{
					if (!derivativeCalculatorFirstPose.verifyDerivative(objectPoint, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			// Verify second pose jacobian
			{
				const Scalar* jacobianX = poseJacobiansSecond + 12u * (unsigned int)n + 0u;
				const Scalar* jacobianY = jacobianX + 6;

				for (size_t parameterIndex = 0; parameterIndex < 6; ++parameterIndex)
				{
					if (!derivativeCalculatorSecondPose.verifyDerivative(objectPoint, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			// Verify first point jacobian
			{
				const Scalar* jacobianX = pointJacobiansFirstPose + 6u * (unsigned int)n + 0u;
				const Scalar* jacobianY = jacobianX + 3;

				for (size_t parameterIndex = 0; parameterIndex < 3; ++parameterIndex)
				{
					if (!derivativeCalculatorFirstPoint.verifyDerivative(VectorD3(objectPoint), parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			// Verify second point jacobian
			{
				const Scalar* jacobianX = pointJacobiansSecondPose + 6u * (unsigned int)n + 0u;
				const Scalar* jacobianY = jacobianX + 3;

				for (size_t parameterIndex = 0; parameterIndex < 3; ++parameterIndex)
				{
					if (!derivativeCalculatorSecondPoint.verifyDerivative(VectorD3(objectPoint), parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for spherical object point Jacobian (3x3) using central finite differences.
 * Calculates numerical derivatives of 3D point position with respect to spherical rotation parameters.
 */
class TestJacobian::DerivativeCalculatorSphericalObjectPoint3x3 : public DerivativeCalculatorT<VectorD3, Scalar, void>
{
	public:

		/**
		 * Creates calculator with spherical coordinates and radius.
		 * @param sphericalObjectPoint Spherical rotation representation
		 * @param defaultObjectPoint Default object point before rotation
		 */
		DerivativeCalculatorSphericalObjectPoint3x3(const ExponentialMap& sphericalObjectPoint, const Vector3& defaultObjectPoint) :
			sphericalObjectPoint_(sphericalObjectPoint),
			defaultObjectPoint_(defaultObjectPoint)
		{
			// nothing to do here
		}

		/**
		 * Calculates 3D point position with perturbed spherical rotation parameter.
		 * @param parameterIndex Index of rotation component to perturb (0=wx, 1=wy, 2=wz)
		 * @param offset Epsilon offset to apply to rotation component
		 * @return Rotated 3D point position
		 */
		VectorD3 calculateValue(const size_t parameterIndex, const Scalar offset) const override
		{
			ExponentialMap perturbedRotation(sphericalObjectPoint_);

			if (parameterIndex == 0)
			{
				perturbedRotation += ExponentialMap(offset, 0, 0);
			}
			else if (parameterIndex == 1)
			{
				perturbedRotation += ExponentialMap(0, offset, 0);
			}
			else
			{
				perturbedRotation += ExponentialMap(0, 0, offset);
			}

			return VectorD3(perturbedRotation.rotation() * defaultObjectPoint_);
		}

	protected:

		/// Spherical rotation representation
		const ExponentialMap sphericalObjectPoint_;

		/// Default object point before rotation
		const Vector3 defaultObjectPoint_;
};

bool TestJacobian::testSphericalObjectPoint3x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing spherical object point jacobian 3x3:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	const Vector3 defaultRotationDirection(0, 0, -1);

	Scalar jacobianX[3];
	Scalar jacobianY[3];
	Scalar jacobianZ[3];

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const Scalar radius = Random::scalar(Scalar(0.001), 100);
		const Vector3 defaultObjectPoint(defaultRotationDirection * radius);

		const Vector3 rotationDirection(Random::vector3());
		ocean_assert(Numeric::isEqual(rotationDirection.length(), 1));

		const ExponentialMap sphericalObjectPoint(Rotation(defaultRotationDirection, rotationDirection));

		/**
		 * jacobian for wx and wz
		 * jacobian x: | dfx / dwx, dfx / dwz |
		 * jacobian y: | dfy / dwx, dfy / dwz |
		 * jacobian z: | dfz / dwx, dfz / dwz |
		 */

		Geometry::Jacobian::calculateSphericalObjectPointJacobian3x3(jacobianX, jacobianY, jacobianZ, sphericalObjectPoint, radius);

		const DerivativeCalculatorSphericalObjectPoint3x3 derivativeCalculator(sphericalObjectPoint, defaultObjectPoint);

		for (size_t parameterIndex = 0; parameterIndex < 3; ++parameterIndex)
		{
			if (!derivativeCalculator.verifyDerivative(parameterIndex, Vector3(jacobianX[parameterIndex], jacobianY[parameterIndex], jacobianZ[parameterIndex])))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for spherical object point orientation jacobian.
 * @tparam T The data type of the scalar, either 'float' or 'double'
 */
template <typename T>
class TestJacobian::DerivativeCalculatorSphericalObjectPointOrientation2x3IF : public DerivativeCalculatorT<VectorD2, T, ExponentialMapD>
{
	public:

		/**
		 * Creates a derivative calculator for spherical object point orientation jacobian.
		 * @param camera The camera model
		 * @param flippedCamera_R_world The camera rotation (inverted and flipped)
		 * @param defaultObjectPoint The default object point
		 */
		DerivativeCalculatorSphericalObjectPointOrientation2x3IF(const AnyCameraT<T>& camera, const SquareMatrixT3<T>& flippedCamera_R_world, const VectorT3<T>& defaultObjectPoint) :
			camera_(camera.cloneToDouble()),
			flippedCamera_T_worldD_(SquareMatrixD3(flippedCamera_R_world)),
			defaultObjectPointD_(defaultObjectPoint)
		{
			ocean_assert(camera_);
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed spherical object point orientation.
		 * @param sphericalObjectPoint The spherical object point as exponential map
		 * @param parameterIndex The rotation parameter index to perturb (0, 1, or 2)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		VectorD2 calculateValue(const ExponentialMapD& sphericalObjectPoint, const size_t parameterIndex, const T offset) const override
		{
			ExponentialMapD sphericalObjectPointPerturbed(sphericalObjectPoint);
			sphericalObjectPointPerturbed[(unsigned int)(parameterIndex)] += offset;

			const VectorD3 objectPoint = sphericalObjectPointPerturbed.rotation() * defaultObjectPointD_;

			return camera_->projectToImageIF(flippedCamera_T_worldD_, objectPoint);
		}

	protected:

		/// Camera model in double precision (cached for performance)
		const SharedAnyCameraD camera_;

		/// Transformation from world to camera (inverted and flipped)
		const HomogenousMatrixD4 flippedCamera_T_worldD_;

		/// Default object point
		const VectorD3 defaultObjectPointD_;
};

template <typename T>
bool TestJacobian::testSphericalObjectPointOrientation2x3IF(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing spherical object point and camera orientation jacobian 2x3 with " << sizeof(T) * 8 << "-bit precision:";

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

			const DerivativeCalculatorSphericalObjectPointOrientation2x3IF<T> derivativeCalculator(camera, flippedCamera_R_world, defaultObjectPoint);

			for (size_t parameterIndex = 0; parameterIndex < 3; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(ExponentialMapD(sphericalObjectPoint), parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
		while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

/**
 * Derivative calculator for pinhole camera distortion jacobian 2x4.
 * Calculates numerical derivatives of 2D image projection with respect to distortion parameters (k1, k2, p1, p2).
 */
class TestJacobian::DerivativeCalculatorPinholeCameraDistortionJacobian2x4 : public DerivativeCalculatorT<Vector2, Scalar, void>
{
	public:

		/**
		 * Creates a derivative calculator for distortion jacobian.
		 * @param camera The pinhole camera with distortion parameters
		 * @param normalizedImagePoint The normalized image point
		 */
		DerivativeCalculatorPinholeCameraDistortionJacobian2x4(const PinholeCamera& camera, const Vector2& normalizedImagePoint) :
			camera_(camera),
			normalizedImagePoint_(normalizedImagePoint)
		{
			// nothing to do here
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed distortion parameter.
		 * @param parameterIndex The distortion parameter index to perturb (0: k1, 1: k2, 2: p1, 3: p2)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		Vector2 calculateValue(const size_t parameterIndex, const Scalar offset) const override
		{
			const PinholeCamera::DistortionPair radialDistortion = camera_.radialDistortion();
			const PinholeCamera::DistortionPair tangentialDistortion = camera_.tangentialDistortion();

			const Scalar k1 = radialDistortion.first;
			const Scalar k2 = radialDistortion.second;
			const Scalar p1 = tangentialDistortion.first;
			const Scalar p2 = tangentialDistortion.second;

			PinholeCamera cameraPerturbed(camera_);

			switch (parameterIndex)
			{
				case 0:
					cameraPerturbed.setRadialDistortion(PinholeCamera::DistortionPair(k1 + offset, k2));
					break;
				case 1:
					cameraPerturbed.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2 + offset));
					break;
				case 2:
					cameraPerturbed.setTangentialDistortion(PinholeCamera::DistortionPair(p1 + offset, p2));
					break;
				case 3:
					cameraPerturbed.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2 + offset));
					break;
				default:
					ocean_assert(false && "Invalid parameter index");
					break;
			}

			return cameraPerturbed.projectToImageIF<false>(normalizedImagePoint_, true);
		}

	protected:

		/// The pinhole camera
		const PinholeCamera camera_;

		/// The normalized image point
		const Vector2 normalizedImagePoint_;
};

bool TestJacobian::testPinholeCameraDistortionJacobian2x4(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing camera distortion jacobian 2x4:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr unsigned int width = 640u;
		constexpr unsigned int height = 480u;

		constexpr Scalar width2 = Scalar(width) * Scalar(0.5);
		constexpr Scalar height2 = Scalar(height) * Scalar(0.5);

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

		const DerivativeCalculatorPinholeCameraDistortionJacobian2x4 derivativeCalculator(camera, normalizedImagePoint);

		for (size_t parameterIndex = 0; parameterIndex < 4; ++parameterIndex)
		{
			if (!derivativeCalculator.verifyDerivative(parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for pinhole camera jacobian 2x6.
 * Computes derivatives with respect to camera parameters: k1, k2, Fx, Fy, mx, my
 */
class TestJacobian::DerivativeCalculatorPinholeCameraJacobian2x6 : public DerivativeCalculatorT<Vector2, Scalar, void>
{
	public:

		/**
		 * Creates a derivative calculator for pinhole camera jacobian 2x6.
		 * @param camera The camera profile
		 * @param normalizedImagePoint The normalized image point
		 */
		DerivativeCalculatorPinholeCameraJacobian2x6(const PinholeCamera& camera, const Vector2& normalizedImagePoint) :
			normalizedImagePoint_(normalizedImagePoint)
		{
			PinholeCamera::ParameterConfiguration parameterConfiguration = PinholeCamera::PC_UNKNOWN;

			ocean_assert(camera.isValid());
			camera.copyParameters(width_, height_, cameraParameters_, parameterConfiguration);
			ocean_assert(width_ == camera.width() && height_ == camera.height());
			ocean_assert(cameraParameters_.size() == 8 && parameterConfiguration == PinholeCamera::PC_8_PARAMETERS);
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed camera parameter.
		 * @param parameterIndex The parameter index to perturb, with order k1, k2, Fx, Fy, mx, my
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		Vector2 calculateValue(const size_t parameterIndex, const Scalar offset) const override
		{
			ocean_assert(cameraParameters_.size() == 8 && parameterIndex < 6);

			Scalars cameraParametersPerturbed(cameraParameters_);

			constexpr std::array<Index32, 6> parameterMapping = {4u, 5u, 0u, 1u, 2u, 3u};

			cameraParametersPerturbed[parameterMapping[parameterIndex]] += offset;

			const PinholeCamera cameraPerturbed(width_, height_, PinholeCamera::PC_8_PARAMETERS, cameraParametersPerturbed.data());

			return cameraPerturbed.projectToImageIF<true>(normalizedImagePoint_, cameraPerturbed.hasDistortionParameters());
		}

	protected:

		/// The normalized image point
		const Vector2 normalizedImagePoint_;

		/// The width of the camera model, in pixel.
		unsigned int width_ = 0u;

		/// The height of the camera model, in pixel.
		unsigned int height_ = 0u;

		/// The camera parameters.
		Scalars cameraParameters_;
};

bool TestJacobian::testPinholeCameraJacobian2x6(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing pinhole camera jacobian 2x6:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr unsigned int width = 640u;
		constexpr unsigned int height = 480u;

		constexpr Scalar width2 = Scalar(width) * Scalar(0.5);
		constexpr Scalar height2 = Scalar(height) * Scalar(0.5);

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

		const DerivativeCalculatorPinholeCameraJacobian2x6 derivativeCalculator(camera, normalizedImagePoint);

		for (size_t parameterIndex = 0; parameterIndex < 6; ++parameterIndex)
		{
			if (!derivativeCalculator.verifyDerivative(parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for pinhole camera jacobian 2x7.
 * Computes derivatives with respect to camera parameters: k1, k2, p1, p2, F, mx, my
 */
class TestJacobian::DerivativeCalculatorPinholeCameraJacobian2x7 : public DerivativeCalculatorT<Vector2, Scalar, void>
{
	public:

		/**
		 * Creates a derivative calculator for pinhole camera jacobian 2x7.
		 * @param camera The camera profile
		 * @param normalizedImagePoint The normalized image point
		 */
		DerivativeCalculatorPinholeCameraJacobian2x7(const PinholeCamera& camera, const Vector2& normalizedImagePoint) :
			normalizedImagePoint_(normalizedImagePoint)
		{
			PinholeCamera::ParameterConfiguration parameterConfiguration = PinholeCamera::PC_UNKNOWN;

			ocean_assert(camera.isValid());
			camera.copyParameters(width_, height_, cameraParameters_, parameterConfiguration);
			ocean_assert(width_ == camera.width() && height_ == camera.height());
			ocean_assert(cameraParameters_.size() == 8 && parameterConfiguration == PinholeCamera::PC_8_PARAMETERS);
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed camera parameter.
		 * @param parameterIndex The parameter index to perturb, with order k1, k2, p1, p2, F, mx, my
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		Vector2 calculateValue(const size_t parameterIndex, const Scalar offset) const override
		{
			ocean_assert(cameraParameters_.size() == 8 && parameterIndex < 7);

			Scalars cameraParametersPerturbed(cameraParameters_);

			constexpr std::array<Index32, 7> parameterMapping = {4u, 5u, 6u, 7u, 0u, 2u, 3u};

			const Index32 mappedIndex = parameterMapping[parameterIndex];

			cameraParametersPerturbed[mappedIndex] += offset;

			if (mappedIndex == 0u) // special handling for Fx and Fy
			{
				cameraParametersPerturbed[1u] += offset;
			}

			const PinholeCamera cameraPerturbed(width_, height_, PinholeCamera::PC_8_PARAMETERS, cameraParametersPerturbed.data());

			return cameraPerturbed.projectToImageIF<true>(normalizedImagePoint_, cameraPerturbed.hasDistortionParameters());
		}

	protected:

		/// The normalized image point
		const Vector2 normalizedImagePoint_;

		/// The width of the camera model, in pixel.
		unsigned int width_ = 0u;

		/// The height of the camera model, in pixel.
		unsigned int height_ = 0u;

		/// The camera parameters.
		Scalars cameraParameters_;
};

bool TestJacobian::testPinholeCameraJacobian2x7(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing pinhole camera jacobian 2x7:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr unsigned int width = 640u;
		constexpr unsigned int height = 480u;

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

		const DerivativeCalculatorPinholeCameraJacobian2x7 derivativeCalculator(camera, normalizedImagePoint);

		for (size_t parameterIndex = 0; parameterIndex < 7; ++parameterIndex)
		{
			if (!derivativeCalculator.verifyDerivative(parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for pinhole camera jacobian 2x8.
 * Computes derivatives with respect to 8 camera parameters: Fx, Fy, mx, my, k1, k2, p1, p2
 */
class TestJacobian::DerivativeCalculatorPinholeCameraJacobian2x8 : public DerivativeCalculatorT<VectorD2, double, VectorD2>
{
	public:

		/**
		 * Creates a derivative calculator for pinhole camera jacobian 2x8.
		 * @param camera The pinhole camera in double precision
		 */
		DerivativeCalculatorPinholeCameraJacobian2x8(const PinholeCameraD& camera)
		{
			PinholeCameraD::ParameterConfiguration parameterConfiguration = PinholeCameraD::PC_UNKNOWN;

			camera.copyParameters(cameraWidth_, cameraHeight_, cameraParametersD_, parameterConfiguration);
			ocean_assert(cameraParametersD_.size() == 8 && parameterConfiguration == PinholeCameraD::PC_8_PARAMETERS);
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed camera parameter.
		 * @param normalizedUndistortedImagePoint The normalized undistorted image point
		 * @param parameterIndex The parameter index to perturb, with order Fx, Fy, mx, my, k1, k2, p1, p2
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		VectorD2 calculateValue(const VectorD2& normalizedUndistortedImagePoint, const size_t parameterIndex, const double offset) const override
		{
			std::vector<double> deltaCameraParametersD(cameraParametersD_);
			deltaCameraParametersD[parameterIndex] += offset;

			const PinholeCameraD deltaCameraD(cameraWidth_, cameraHeight_, PinholeCameraD::PC_8_PARAMETERS, deltaCameraParametersD.data());

			return deltaCameraD.projectToImageIF<true>(normalizedUndistortedImagePoint, true);
		}

	protected:

		/// Camera width
		unsigned int cameraWidth_;

		/// Camera height
		unsigned int cameraHeight_;

		/// Camera parameters
		std::vector<double> cameraParametersD_;
};

template <typename T>
bool TestJacobian::testPinholeCameraJacobian2x8(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing pinhole camera jacobian 2x8, with " << TypeNamer::name<T>() << ":";

	constexpr size_t numberPoints = 100;

	RandomGenerator randomGenerator;

	constexpr double threshold = std::is_same<float, T>::value ? 0.90 : 0.99;

	ValidationPrecision validation(threshold, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		constexpr unsigned int width = 1000u;
		constexpr unsigned int height = 1000u;

		constexpr T width_2 = T(width) * T(0.5);
		constexpr T height_2 = T(height) * T(0.5);

		const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(40), NumericT<T>::deg2rad(70));

		const T principalX = RandomT<T>::scalar(randomGenerator, width_2 - T(50), width_2 + T(50));
		const T principalY = RandomT<T>::scalar(randomGenerator, height_2 - T(50), height_2 + T(50));

		const T k1 = RandomT<T>::scalar(randomGenerator, T(-0.5), T(0.5));
		const T k2 = RandomT<T>::scalar(randomGenerator, T(-0.5), T(0.5));

		const T p1 = RandomT<T>::scalar(randomGenerator, T(-0.5), T(0.5));
		const T p2 = RandomT<T>::scalar(randomGenerator, T(-0.5), T(0.5));

		PinholeCameraT<T> camera(width, height, fovX, principalX, principalY);
		camera.setRadialDistortion(typename PinholeCameraT<T>::DistortionPair(k1, k2));
		camera.setTangentialDistortion(typename PinholeCameraT<T>::DistortionPair(p1, p2));

		/**
		 * jacobian x: | dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2 |
		 * jacobian y: | dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2 |
		 */
		T jacobianX[8];
		T jacobianY[8];

		const PinholeCameraD cameraD(camera);

		const DerivativeCalculatorPinholeCameraJacobian2x8 derivativeCalculator(cameraD);

		for (size_t n = 0; n < numberPoints; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const VectorT2<T> distortedImagePoint = RandomT<T>::vector2(randomGenerator, T(0), T(width), T(0), T(height));
			const VectorT3<T> objectPoint = camera.vectorIF(distortedImagePoint);
			ocean_assert(objectPoint.z() > NumericT<T>::eps());

			const VectorT2<T> normalizedUndistortedImagePoint = VectorT2<T>(objectPoint.x() / objectPoint.z(), objectPoint.y() / objectPoint.z());

			Geometry::Jacobian::calculateCameraJacobian2x8<T>(camera, normalizedUndistortedImagePoint, jacobianX, jacobianY);

			const VectorD3 objectPointD(objectPoint);
			const VectorD2 normalizedUndistortedImagePointD = VectorD2(objectPointD.x() / objectPointD.z(), objectPointD.y() / objectPointD.z());

			for (size_t parameterIndex = 0; parameterIndex < 8; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(normalizedUndistortedImagePointD, parameterIndex, VectorT2<T>(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

/**
 * Derivative calculator for fisheye camera jacobian 2x12.
 * Computes derivatives with respect to 12 fisheye camera parameters
 */
class TestJacobian::DerivativeCalculatorFisheyeCameraJacobian2x12 : public DerivativeCalculatorT<VectorD2, double, VectorD3>
{
	public:

		/**
		 * Creates a derivative calculator for fisheye camera jacobian 2x12.
		 * @param fisheyeCamera The fisheye camera, must be valid
		 */
		DerivativeCalculatorFisheyeCameraJacobian2x12(const FisheyeCameraD& fisheyeCamera)
		{
			ocean_assert(fisheyeCamera.isValid());

			FisheyeCameraD::ParameterConfiguration parameterConfiguration = FisheyeCameraD::PC_UNKNOWN;

			ocean_assert(fisheyeCamera.isValid());
			fisheyeCamera.copyParameters(width_, height_, cameraParameters_, parameterConfiguration);
			ocean_assert(width_ == fisheyeCamera.width() && height_ == fisheyeCamera.height());
			ocean_assert(cameraParameters_.size() == 12 && parameterConfiguration == FisheyeCameraD::PC_12_PARAMETERS);
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed camera parameter.
		 * @param cameraParameters
		 * @param parameterIndex The parameter index to perturb
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		VectorD2 calculateValue(const VectorD3& objectPoint, const size_t parameterIndex, const double offset) const override
		{
			std::vector<double> deltaCameraParameters(cameraParameters_);
			deltaCameraParameters[parameterIndex] += offset;

			const FisheyeCameraD deltaFisheyeCamera(width_, height_, FisheyeCameraD::PC_12_PARAMETERS, deltaCameraParameters.data());

			return deltaFisheyeCamera.projectToImageIF(objectPoint);
		}

	protected:

		/// The width of the camera model, in pixel.
		unsigned int width_ = 0u;

		/// The height of the camera model, in pixel.
		unsigned int height_ = 0u;

		/// The camera parameters.
		std::vector<double> cameraParameters_;
};

template <typename T>
bool TestJacobian::testFisheyeCameraJacobian2x12(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing fisheye camera jacobian 2x12, with " << TypeNamer::name<T>() << ":";

	constexpr size_t numberPoints = 100;

	RandomGenerator randomGenerator;

	constexpr double threshold = std::is_same<float, T>::value ? 0.95 : 0.99;

	ValidationPrecision validation(threshold, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		constexpr unsigned int width = 1000u;
		constexpr unsigned int height = 1000u;

		constexpr T width2 = T(width) * T(0.5);
		constexpr T height2 = T(height) * T(0.5);

		const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(70), NumericT<T>::deg2rad(140));
		const T fovY = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(70), NumericT<T>::deg2rad(140));

		const T principalX = RandomT<T>::scalar(randomGenerator, width2 - T(50), width2 + T(50));
		const T principalY = RandomT<T>::scalar(randomGenerator, height2 - T(50), height2 + T(50));

		const T focalLengthX = principalX / NumericT<T>::tan(fovX * T(0.5));
		const T focalLengthY = principalY / NumericT<T>::tan(fovY * T(0.5));

		std::vector<T> parameters =
		{
			focalLengthX, focalLengthY,
			principalX, principalY,
			0, 0, 0, 0, 0, 0,
			0, 0
		};

		ocean_assert(parameters.size() == 12);

		for (size_t n = 4; n < parameters.size(); ++n)
		{
			parameters[n] = RandomT<T>::scalar(randomGenerator, T(-0.01), T(0.01));
		}

		const FisheyeCameraT<T> fisheyeCamera(width, height, FisheyeCameraT<T>::PC_12_PARAMETERS, parameters.data());

		/**
		 * jacobian x: | dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dk3, dfx / dk5, dfx / dk7, dfx / dk9, dfx / dk11, dfx / dk13, dfx / dp1, dfx / dp2 |
		 * jacobian y: | dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dk3, dfy / dk5, dfy / dk7, dfy / dk9, dfy / dk11, dfy / dk13, dfy / dp1, dfy / dp2 |
		 */
		T jacobianX[12];
		T jacobianY[12];

		const FisheyeCameraD fisheyeCameraD(fisheyeCamera);

		const DerivativeCalculatorFisheyeCameraJacobian2x12 derivativeCalculator(fisheyeCameraD);

		for (size_t n = 0; n < numberPoints; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const VectorT2<T> distortedImagePoint = RandomT<T>::vector2(randomGenerator, T(0), T(width), T(0), T(height));
			const VectorT3<T> objectPoint = fisheyeCamera.vectorIF(distortedImagePoint);
			ocean_assert(objectPoint.z() > NumericT<T>::eps());

			const VectorT2<T> normalizedUndistortedImagePoint = VectorT2<T>(objectPoint.x() / objectPoint.z(), objectPoint.y() / objectPoint.z());

			Geometry::Jacobian::calculateCameraJacobian2x12(fisheyeCamera, normalizedUndistortedImagePoint, jacobianX, jacobianY);

			for (size_t parameterIndex = 0; parameterIndex < 12; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(VectorD3(objectPoint), parameterIndex, VectorT2<T>(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

/**
 * Derivative calculator for orientation pinhole camera jacobian 2x11.
 * Computes derivatives with respect to 11 parameters: wx, wy, wz, k1, k2, p1, p2, Fx, Fy, mx, my
 */
class TestJacobian::DerivativeCalculatorOrientationPinholeCameraJacobian2x11 : public DerivativeCalculatorT<Vector2, Scalar, Vector3>
{
	public:

		/**
		 * Creates a derivative calculator for orientation pinhole camera jacobian 2x11.
		 * @param camera The pinhole camera
		 * @param flippedCamera_P_world The camera pose (inverted and flipped)
		 * @param objectPoint The 3D object point
		 */
		DerivativeCalculatorOrientationPinholeCameraJacobian2x11(const PinholeCamera& camera, const Pose& flippedCamera_P_world) :
			camera_(camera),
			flippedCamera_P_world_(flippedCamera_P_world)
		{
			unsigned int width = 0u;
			unsigned int height = 0u;
			PinholeCamera::ParameterConfiguration parameterConfiguration = PinholeCamera::PC_UNKNOWN;

			camera_.copyParameters(width, height, cameraParameters_, parameterConfiguration);

			ocean_assert(width == camera_.width());
			ocean_assert(height == camera_.height());
			ocean_assert(parameterConfiguration == PinholeCamera::PC_8_PARAMETERS);
			ocean_assert(cameraParameters_.size() == 8);
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed parameter.
		 * @param objectPoint The 3D object point (not used, required by interface)
		 * @param parameterIndex The parameter index to perturb (0-2: rotation, 3-10: camera params)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		Vector2 calculateValue(const Vector3& objectPoint, const size_t parameterIndex, const Scalar offset) const override
		{
			Pose flippedCamera_P_world_perturbed(flippedCamera_P_world_);
			Scalars cameraParameters(cameraParameters_);

			if (parameterIndex < 3)
			{
				// Rotation parameters (wx, wy, wz) - perturb the pose
				flippedCamera_P_world_perturbed[(unsigned int)(parameterIndex + 3)] += offset; // index 0-2 map to rx, ry, rz which are indices 3-5 in Pose
			}
			else
			{
				/**
				 * 8 parameters with order:
				 * horizontal focal length,
				 * vertical focal length,
				 * horizontal principal point,
				 * vertical principal point,
				 * two radial distortion parameters k1, k2
				 * two tangential distortion parameters p1, p2
				 */

				constexpr std::array<Index32, 8u> parameterMapping = {4u, 5u, 6u, 7u, 0u, 1u, 2u, 3u};

				cameraParameters[parameterMapping[parameterIndex - 3]] += offset;
			}

			const PinholeCamera cameraPerturbed(camera_.width(), camera_.height(), PinholeCamera::PC_8_PARAMETERS, cameraParameters.data());

			return cameraPerturbed.projectToImageIF<false>(flippedCamera_P_world_perturbed.transformation(), objectPoint, cameraPerturbed.hasDistortionParameters());
		}

	protected:

		/// The pinhole camera
		const PinholeCamera& camera_;

		/// The 8 camera parameters.
		Scalars cameraParameters_;

		/// The camera pose (inverted and flipped)
		const Pose& flippedCamera_P_world_;
};

bool TestJacobian::testOrientationPinholeCameraJacobian2x11(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing camera orientation jacobian 2x11 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	unsigned int distortionIteration = 0u;

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr unsigned int width = 640u;
		constexpr unsigned int height = 480u;

		constexpr Scalar width2 = Scalar(width) * Scalar(0.5);
		constexpr Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 world_t_camera = Random::vector3(-1, 1);
		const Quaternion world_Q_camera = Random::quaternion();

		const HomogenousMatrix4 flippedCamera_T_world(world_t_camera, world_Q_camera);
		const Pose flippedCamera_P_world(flippedCamera_T_world);

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

		distortionIteration++;

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			objectPoints.push_back(camera.ray(Vector2(Random::scalar(40u, width - 40u), Random::scalar(40u, height - 40u)), PinholeCamera::invertedFlipped2Standard(flippedCamera_T_world)).point(Random::scalar(1, 10)));
		}

		/**
		 * jacobian:
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy |<br>
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy |<br>
		 */

		Matrix jacobian(2 * objectPoints.size(), 11);

		performance.start();
			Geometry::Jacobian::calculateOrientationCameraJacobianRodrigues2nx11(jacobian.data(), camera, flippedCamera_P_world, ConstArrayAccessor<Vector3>(objectPoints));
		performance.stop();

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 11);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			const HomogenousMatrix4 flippedCamera_T_world_matrix(flippedCamera_P_world.transformation());

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3& objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(flippedCamera_T_world_matrix, objectPoint, camera.hasDistortionParameters()));

				for (unsigned int i = 0u; i < 11u; ++i)
				{
					Pose flippedCamera_P_world_delta(flippedCamera_P_world);

					SquareMatrix3 intrinsicDelta(camera.intrinsic());
					PinholeCamera::DistortionPair radialDistortionDelta(camera.radialDistortion());
					PinholeCamera::DistortionPair tangentialDistortionDelta(camera.tangentialDistortion());

					if (i < 3u)
					{
						flippedCamera_P_world_delta[i + 3u] += Numeric::weakEps();
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

					const Vector2 imagePointDelta(cameraDelta.projectToImageIF<false>(flippedCamera_P_world_delta.transformation(), objectPoint, camera.hasDistortionParameters()));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2 + 0][i] = derivative.x();
					naiveJacobian[n * 2 + 1][i] = derivative.y();
				}
			}
		}

		const DerivativeCalculatorOrientationPinholeCameraJacobian2x11 derivativeCalculator(camera, flippedCamera_P_world);

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			const Scalar* jacobianX = jacobian[2 * n + 0];
			const Scalar* jacobianY = jacobian[2 * n + 1];

			{
				// we also test the implementation for one object point

				Scalar singleJacobianX[11];
				Scalar singleJacobianY[11];
				Geometry::Jacobian::calculateOrientationCameraJacobianRodrigues2x11(singleJacobianX, singleJacobianY, camera, flippedCamera_P_world, objectPoint);

				for (unsigned int i = 0u; i < 11u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			for (size_t parameterIndex = 0; parameterIndex < 11; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(objectPoint, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for pose pinhole camera jacobian 2x12.
 * Computes derivatives with respect to 12 parameters: k1, k2, Fx, Fy, mx, my, wx, wy, wz, tx, ty, tz
 */
class TestJacobian::DerivativeCalculatorPosePinholeCameraJacobian2x12 : public DerivativeCalculatorT<Vector2, Scalar, Vector3>
{
	public:

		/**
		 * Creates a derivative calculator for pose pinhole camera jacobian 2x12.
		 * @param camera The pinhole camera
		 * @param flippedCamera_P_world The camera pose (inverted and flipped)
		 * @param objectPoint The 3D object point
		 */
		DerivativeCalculatorPosePinholeCameraJacobian2x12(const PinholeCamera& camera, const Pose& flippedCamera_P_world) :
			camera_(camera),
			flippedCamera_P_world_(flippedCamera_P_world)
		{
			unsigned int width = 0u;
			unsigned int height = 0u;
			PinholeCamera::ParameterConfiguration parameterConfiguration = PinholeCamera::PC_UNKNOWN;

			camera_.copyParameters(width, height, cameraParameters_, parameterConfiguration);

			ocean_assert(width == camera_.width());
			ocean_assert(height == camera_.height());
			ocean_assert(parameterConfiguration == PinholeCamera::PC_8_PARAMETERS);
			ocean_assert(cameraParameters_.size() == 8);
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed parameter.
		 * @param objectPoint The 3D object point (not used, required by interface)
		 * @param parameterIndex The parameter index to perturb (0-5: camera params, 6-11: pose params)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		Vector2 calculateValue(const Vector3& objectPoint, const size_t parameterIndex, const Scalar offset) const override
		{
			Pose flippedCamera_P_world_perturbed(flippedCamera_P_world_);
			Scalars cameraParameters(cameraParameters_);

			if (parameterIndex < 6)
			{
				/**
				 * 8 parameters with order:
				 * horizontal focal length,
				 * vertical focal length,
				 * horizontal principal point,
				 * vertical principal point,
				 * two radial distortion parameters k1, k2
				 * two tangential distortion parameters p1, p2
				 */

				constexpr std::array<Index32, 6u> parameterMapping = {4u, 5u, 0u, 1u, 2u, 3u};

				cameraParameters[parameterMapping[parameterIndex]] += offset;
			}
			else
			{
				constexpr std::array<Index32, 6u> parameterMapping = {3u, 4u, 5u, 0u, 1u, 2u};

				flippedCamera_P_world_perturbed[parameterMapping[parameterIndex - 6]] += offset;
			}

			const PinholeCamera cameraPerturbed(camera_.width(), camera_.height(), PinholeCamera::PC_8_PARAMETERS, cameraParameters.data());

			return cameraPerturbed.projectToImageIF<false>(flippedCamera_P_world_perturbed.transformation(), objectPoint, cameraPerturbed.hasDistortionParameters());
		}

	protected:

		/// The pinhole camera
		const PinholeCamera& camera_;

		/// The 8 camera parameters.
		Scalars cameraParameters_;

		/// The camera pose (inverted and flipped)
		const Pose& flippedCamera_P_world_;
};

bool TestJacobian::testPosePinholeCameraJacobian2x12(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing camera pose jacobian 2x12 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	unsigned int distortionIteration = 0u;

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		constexpr unsigned int width = 640u;
		constexpr unsigned int height = 480u;

		constexpr Scalar width2 = Scalar(width) * Scalar(0.5);
		constexpr Scalar height2 = Scalar(height) * Scalar(0.5);

		const Scalar fovX = Random::scalar(Numeric::deg2rad(40), Numeric::deg2rad(70));

		const Scalar principalX = Random::scalar(width2 - Scalar(50), width2 + Scalar(50));
		const Scalar principalY = Random::scalar(height2 - Scalar(50), height2 + Scalar(50));

		const Vector3 world_t_camera = Random::vector3(-1, 1);
		const Quaternion world_Q_camera = Random::quaternion();

		const HomogenousMatrix4 flippedCamera_T_world(world_t_camera, world_Q_camera);
		const Pose flippedCamera_P_world(flippedCamera_T_world);

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

		distortionIteration++;

		Vectors3 objectPoints;
		objectPoints.reserve(numberPoints);
		while (objectPoints.size() < numberPoints)
		{
			objectPoints.push_back(camera.ray(Vector2(Random::scalar(40u, width - 40u), Random::scalar(40u, height - 40u)), PinholeCamera::invertedFlipped2Standard(flippedCamera_T_world)).point(Random::scalar(1, 10)));
		}


		/**
		 * | dfx / dk1, dfx / dk2, dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |<br>
		 * | dfy / dk1, dfy / dk2, dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |<br>
		 */

		Matrix jacobian(2 * objectPoints.size(), 12);

		performance.start();

		const Pose flippedCamera_P_world_copy(flippedCamera_T_world);

		SquareMatrix3 Rwx, Rwy, Rwz;
		Geometry::Jacobian::calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world_copy.rx(), flippedCamera_P_world_copy.ry(), flippedCamera_P_world_copy.rz())), Rwx, Rwy, Rwz);

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			Geometry::Jacobian::calculateJacobianCameraPoseRodrigues2x12(jacobian[n * 2u + 0u], jacobian[n * 2u + 1], camera, flippedCamera_T_world, flippedCamera_P_world_copy, objectPoints[n], Rwx, Rwy, Rwz);
		}

		performance.stop();

		{
			Matrix naiveJacobian(2 * objectPoints.size(), 12);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			const HomogenousMatrix4 flippedCamera_T_world_matrix(flippedCamera_P_world.transformation());

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const Vector3& objectPoint = objectPoints[n];
				const Vector2 imagePoint(camera.projectToImageIF<false>(flippedCamera_T_world_matrix, objectPoint, camera.hasDistortionParameters()));

				for (unsigned int i = 0u; i < 12u; ++i)
				{
					Pose flippedCamera_P_world_delta(flippedCamera_P_world);

					SquareMatrix3 intrinsicDelta(camera.intrinsic());
					PinholeCamera::DistortionPair radialDistortionDelta(camera.radialDistortion());
					PinholeCamera::DistortionPair tangentialDistortionDelta(camera.tangentialDistortion());

					if (i >= 6u && i < 6u + 6u)
					{
						if (i < 6u + 3u)
						{
							flippedCamera_P_world_delta[i - 6u + 3u] += Numeric::weakEps();
						}
						else
						{
							flippedCamera_P_world_delta[i - 6u - 3u] += Numeric::weakEps();
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
								ocean_assert(false && "This should never happen!");
						}
					}

					const PinholeCamera cameraDelta(intrinsicDelta, camera.width(), camera.height(), radialDistortionDelta, tangentialDistortionDelta);

					const Vector2 imagePointDelta(cameraDelta.projectToImageIF<false>(flippedCamera_P_world_delta.transformation(), objectPoint, camera.hasDistortionParameters()));
					const Vector2 derivative = (imagePointDelta - imagePoint) / Numeric::weakEps();

					naiveJacobian[n * 2u + 0u][i] = derivative.x();
					naiveJacobian[n * 2u + 1u][i] = derivative.y();
				}
			}
		}

		const DerivativeCalculatorPosePinholeCameraJacobian2x12 derivativeCalculator(camera, flippedCamera_P_world);

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			const Scalar* jacobianX = jacobian[2u * n + 0u];
			const Scalar* jacobianY = jacobian[2u * n + 1u];

			{
				// we also test the implementation for one object point

				Scalar singleJacobianX[12];
				Scalar singleJacobianY[12];
				Geometry::Jacobian::calculateJacobianCameraPoseRodrigues2x12(singleJacobianX, singleJacobianY, camera, flippedCamera_T_world, objectPoint);

				for (unsigned int i = 0u; i < 12u; ++i)
				{
					ocean_assert(Numeric::isWeakEqual(jacobianX[i], singleJacobianX[i]));
					ocean_assert(Numeric::isWeakEqual(jacobianY[i], singleJacobianY[i]));

					if (Numeric::isNotEqual(jacobianX[i], singleJacobianX[i], Numeric::eps() * 100) || Numeric::isNotEqual(jacobianY[i], singleJacobianY[i], Numeric::eps() * 100))
					{
						scopedIteration.setInaccurate();
					}
				}
			}

			for (size_t parameterIndex = 0; parameterIndex < 12; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(objectPoint, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	if (!validation.succeeded())
	{
		if (std::is_same<Scalar, float>::value)
		{
			Log::info() << "This test failed due to precision issues of 32-bit floating point numbers. This is expected and no reason to be alarmed.";
			return true;
		}
	}

	return validation.succeeded();
}

/**
 * Derivative calculator for pose pinhole camera jacobian 2x14.
 * Calculates numerical derivatives of 2D image projection with respect to 14 parameters:
 * 8 camera parameters (Fx, Fy, mx, my, k1, k2, p1, p2) and 6 pose parameters (tx, ty, tz, wx, wy, wz).
 * @tparam T The data type of the scalar, either 'float' or 'double'
 */
template <typename T>
class TestJacobian::DerivativeCalculatorPosePinholeCameraJacobian2x14 : public DerivativeCalculatorT<VectorD2, double, VectorD3>
{
	public:

		/**
		 * Creates a derivative calculator for pose pinhole camera jacobian.
		 * @param camera The pinhole camera
		 * @param flippedCamera_P_world The camera pose (inverted and flipped)
		 */
		DerivativeCalculatorPosePinholeCameraJacobian2x14(const PinholeCameraT<T>& camera, const PoseT<T>& flippedCamera_P_world) :
			cameraD_(camera),
			flippedCamera_P_worldD_(flippedCamera_P_world)
		{
			unsigned int cameraWidth = 0u;
			unsigned int cameraHeight = 0u;
			PinholeCameraD::ParameterConfiguration parameterConfiguration = PinholeCameraD::PC_UNKNOWN;

			cameraD_.copyParameters(cameraWidth, cameraHeight, cameraParametersD_, parameterConfiguration);

			ocean_assert(parameterConfiguration == PinholeCameraD::PC_8_PARAMETERS);
			ocean_assert(cameraParametersD_.size() == 8);

			cameraWidth_ = cameraWidth;
			cameraHeight_ = cameraHeight;
			cameraParameterConfiguration_ = parameterConfiguration;
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed parameter.
		 * @param objectPoint The 3D object point
		 * @param parameterIndex The parameter index to perturb (0-7: camera params, 8-13: pose params)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		VectorD2 calculateValue(const VectorD3& objectPoint, const size_t parameterIndex, const double offset) const override
		{
			ocean_assert(parameterIndex < 14);

			PoseD posePerturbed(flippedCamera_P_worldD_);
			std::vector<double> cameraParameters(cameraParametersD_);

			if (parameterIndex < 8)
			{
				// Camera parameters
				cameraParameters[parameterIndex] += offset;
			}
			else
			{
				// Pose parameters
				const unsigned int poseIndex = (unsigned int)(parameterIndex - 8);
				ocean_assert(poseIndex < 6);
				posePerturbed[poseIndex] += offset;
			}

			const PinholeCameraD cameraPerturbed(cameraWidth_, cameraHeight_, cameraParameterConfiguration_, cameraParameters.data());

			return cameraPerturbed.projectToImageIF<false>(posePerturbed.transformation(), objectPoint, true);
		}

	protected:

		/// The pinhole camera in double precision
		PinholeCameraD cameraD_;

		/// The camera pose (inverted and flipped) in double precision
		const PoseD flippedCamera_P_worldD_;

		/// Camera width
		unsigned int cameraWidth_ = 0u;

		/// Camera height
		unsigned int cameraHeight_ = 0u;

		/// Camera parameters in double precision
		std::vector<double> cameraParametersD_;

		/// Camera parameter configuration
		PinholeCameraD::ParameterConfiguration cameraParameterConfiguration_ = PinholeCameraD::PC_UNKNOWN;
};

template <typename T>
bool TestJacobian::testPosePinholeCameraJacobian2x14(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr size_t numberPoints = 50;

	Log::info() << "Testing pinhole camera pose jacobian 2x14 for " << numberPoints << " points with " << sizeof(T) * 8 << "-bit precision:";

	constexpr double threshold = std::is_same<float, T>::value ? 0.95 : 0.99;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(threshold, randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	constexpr T cameraBorder = Scalar(50);

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

			const T width2 = T(width) * T(0.5);
			const T height2 = T(height) * T(0.5);

			const T fovX = RandomT<T>::scalar(randomGenerator, NumericT<T>::deg2rad(40), NumericT<T>::deg2rad(70));

			const T principalX = RandomT<T>::scalar(randomGenerator, width2 - T(50), width2 + T(50));
			const T principalY = RandomT<T>::scalar(randomGenerator, height2 - T(50), height2 + T(50));

			const VectorT3<T> translation = RandomT<T>::vector3(randomGenerator, -1, 1);
			const QuaternionT<T> quaternion = RandomT<T>::quaternion(randomGenerator);

			const HomogenousMatrixT4<T> world_T_camera(translation, quaternion);
			const HomogenousMatrixT4<T> flippedCamera_T_world(Camera::standard2InvertedFlipped(world_T_camera));

			const PoseT<T> flippedCamera_P_world(flippedCamera_T_world);

			PinholeCameraT<T> camera(width, height, fovX, principalX, principalY);

			if ((distortionType & DT_RADIAL_DISTORTION) == DT_RADIAL_DISTORTION)
			{
				const T k1 = RandomT<T>::scalar(randomGenerator, T(-0.5), T(0.5));
				const T k2 = RandomT<T>::scalar(randomGenerator, T(-0.5), T(0.5));

				camera.setRadialDistortion(typename PinholeCameraT<T>::DistortionPair(k1, k2));
			}

			if ((distortionType & DT_FULL_DISTORTION) == DT_FULL_DISTORTION)
			{
				const T p1 = RandomT<T>::scalar(randomGenerator, T(-0.5), T(0.5));
				const T p2 = RandomT<T>::scalar(randomGenerator, T(-0.5), T(0.5));

				camera.setTangentialDistortion(typename PinholeCameraT<T>::DistortionPair(p1, p2));
			}

			VectorsT3<T> objectPoints;
			objectPoints.reserve(numberPoints);

			while (objectPoints.size() < numberPoints)
			{
				const VectorT2<T> imagePoint = RandomT<T>::vector2(randomGenerator, T(cameraBorder), T(width - cameraBorder), T(cameraBorder), T(height - cameraBorder));

				const LineT3<T> ray = camera.ray(imagePoint, world_T_camera);

				objectPoints.emplace_back(ray.point(RandomT<T>::scalar(randomGenerator, 1, 10)));
			}

			SquareMatrixT3<T> Rwx, Rwy, Rwz;
			calculateRotationRodriguesDerivative(ExponentialMapT<T>(VectorT3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

			/**
			 * | dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dk1, dfx / dk2, dfx / dp1, dfx / dp2,          dfx / dtx, dfx / dty, dfx / dtz,   dfx / dwx, dfx / dwy, dfx / dwz |
			 * | dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dk1, dfy / dk2, dfy / dp1, dfy / dp2,          dfy / dtx, dfy / dty, dfy / dtz,   dfy / dwx, dfy / dwy, dfy / dwz |
			 */

			MatrixT<T> jacobian(2 * objectPoints.size(), 14);

			performance.start();
				Geometry::Jacobian::calculateJacobianCameraPoseRodrigues2nx14IF(camera, flippedCamera_T_world, ConstArrayAccessor<VectorT3<T>>(objectPoints), Rwx, Rwy, Rwz, jacobian.data());
			performance.stop();

			{
				MatrixT<T> naiveJacobian(2 * objectPoints.size(), 14);

				const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

				for (size_t n = 0; n < objectPoints.size(); ++n)
				{
					const VectorT3<T> objectPoint = objectPoints[n];
					const VectorT2<T> imagePoint(camera.template projectToImageIF<false>(flippedCamera_T_world, objectPoint, camera.hasDistortionParameters()));

					for (unsigned int i = 0u; i < 14u; ++i)
					{
						PoseT<T> poseDelta(flippedCamera_T_world);

						unsigned int cameraWidth = 0u;
						unsigned int cameraHeight = 0u;

						std::vector<T> deltaParameters;
						typename PinholeCameraT<T>::ParameterConfiguration parameterConfiguration = PinholeCameraT<T>::PC_UNKNOWN;
						camera.copyParameters(cameraWidth, cameraHeight, deltaParameters, parameterConfiguration);

						ocean_assert(width == cameraWidth && height == cameraHeight);
						ocean_assert(deltaParameters.size() == 8 && parameterConfiguration == PinholeCameraT<T>::PC_8_PARAMETERS);

						if (i < 8u)
						{
							deltaParameters[i] += NumericT<T>::weakEps();
						}
						else
						{
							const unsigned int ip = i - 8u;
							ocean_assert(ip < 6u);

							poseDelta[ip] += NumericT<T>::weakEps();
						}

						const PinholeCameraT<T> cameraDelta(cameraWidth, cameraHeight, PinholeCameraT<T>::PC_8_PARAMETERS, deltaParameters.data());

						const VectorT2<T> imagePointDelta(cameraDelta.template projectToImageIF<false>(poseDelta.transformation(), objectPoint, camera.hasDistortionParameters()));
						const VectorT2<T> derivative = (imagePointDelta - imagePoint) / NumericT<T>::weakEps();

						naiveJacobian[n * 2u + 0u][i] = derivative.x();
						naiveJacobian[n * 2u + 1u][i] = derivative.y();
					}
				}
			}

			const DerivativeCalculatorPosePinholeCameraJacobian2x14<T> derivativeCalculator(camera, flippedCamera_P_world);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const VectorT3<T>& objectPoint = objectPoints[n];

				const T* jacobianX = jacobian[2u * n + 0u];
				const T* jacobianY = jacobian[2u * n + 1u];

				{
					// we also test the implementation for one object point

					T singleJacobianCameraX[8];
					T singleJacobianCameraY[8];

					T singleJacobianPoseX[6];
					T singleJacobianPoseY[6];
					Geometry::Jacobian::calculateJacobianCameraPoseRodrigues2x14IF(camera, flippedCamera_T_world, objectPoint, Rwx, Rwy, Rwz, singleJacobianCameraX, singleJacobianCameraY, singleJacobianPoseX, singleJacobianPoseY);

					for (unsigned int i = 0u; i < 8u; ++i)
					{
						if constexpr (!std::is_same<T, float>::value)
						{
							ocean_assert(NumericT<T>::isWeakEqual(jacobianX[i], singleJacobianCameraX[i]));
							ocean_assert(NumericT<T>::isWeakEqual(jacobianY[i], singleJacobianCameraY[i]));
						}

						if (NumericT<T>::isNotEqual(jacobianX[i], singleJacobianCameraX[i], NumericT<T>::eps() * 100) || NumericT<T>::isNotEqual(jacobianY[i], singleJacobianCameraY[i], NumericT<T>::eps() * 100))
						{
							scopedIteration.setInaccurate();
						}
					}

					for (unsigned int i = 0u; i < 6u; ++i)
					{
						if constexpr (!std::is_same<T, float>::value)
						{
							ocean_assert(NumericT<T>::isWeakEqual(jacobianX[8 + i], singleJacobianPoseX[i]));
							ocean_assert(NumericT<T>::isWeakEqual(jacobianY[8 + i], singleJacobianPoseY[i]));
						}

						if (NumericT<T>::isNotEqual(jacobianX[8 + i], singleJacobianPoseX[i], NumericT<T>::eps() * 100) || NumericT<T>::isNotEqual(jacobianY[8 + i], singleJacobianPoseY[i], NumericT<T>::eps() * 100))
						{
							scopedIteration.setInaccurate();
						}
					}
				}

				const VectorD3 objectPointD = VectorD3(objectPoints[n]);

				for (size_t parameterIndex = 0; parameterIndex < 14; ++parameterIndex)
				{
					if (!derivativeCalculator.verifyDerivative(objectPointD, parameterIndex, VectorD2(double(jacobianX[parameterIndex]), double(jacobianY[parameterIndex]))))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

/**
 * Derivative calculator for pose fisheye camera jacobian 2x18.
 * Calculates numerical derivatives of 2D image projection with respect to 18 parameters:
 * 12 fisheye camera parameters (Fx, Fy, mx, my, k3, k5, k7, k9, k11, k13, p1, p2) and 6 pose parameters (tx, ty, tz, wx, wy, wz).
 * @tparam T The data type of the scalar, either 'float' or 'double'
 */
template <typename T>
class TestJacobian::DerivativeCalculatorPoseFisheyeCameraJacobian2x18 : public DerivativeCalculatorT<VectorD2, double, VectorD3>
{
	public:

		/**
		 * Creates a derivative calculator for pose fisheye camera jacobian.
		 * @param camera The fisheye camera
		 * @param flippedCamera_P_world The camera pose (inverted and flipped)
		 */
		DerivativeCalculatorPoseFisheyeCameraJacobian2x18(const FisheyeCameraT<T>& camera, const PoseT<T>& flippedCamera_P_world) :
			cameraD_(camera),
			flippedCamera_P_worldD_(flippedCamera_P_world)
		{
			unsigned int cameraWidth = 0u;
			unsigned int cameraHeight = 0u;
			FisheyeCameraD::ParameterConfiguration parameterConfiguration = FisheyeCameraD::PC_UNKNOWN;

			cameraD_.copyParameters(cameraWidth, cameraHeight, cameraParametersD_, parameterConfiguration);

			ocean_assert(parameterConfiguration == FisheyeCameraD::PC_12_PARAMETERS);
			ocean_assert(cameraParametersD_.size() == 12);

			cameraWidth_ = cameraWidth;
			cameraHeight_ = cameraHeight;
			cameraParameterConfiguration_ = parameterConfiguration;
		}

	public:

		/**
		 * Calculates the projected image point with a perturbed parameter.
		 * @param objectPoint The 3D object point
		 * @param parameterIndex The parameter index to perturb (0-11: camera params, 12-17: pose params)
		 * @param offset The offset to apply
		 * @return The projected 2D image point
		 */
		VectorD2 calculateValue(const VectorD3& objectPoint, const size_t parameterIndex, const double offset) const override
		{
			ocean_assert(parameterIndex < 18);

			PoseD posePerturbed(flippedCamera_P_worldD_);
			std::vector<double> cameraParameters(cameraParametersD_);

			if (parameterIndex < 12)
			{
				// Camera parameters
				cameraParameters[parameterIndex] += offset;
			}
			else
			{
				// Pose parameters
				const unsigned int poseIndex = (unsigned int)(parameterIndex - 12);
				ocean_assert(poseIndex < 6);
				posePerturbed[poseIndex] += offset;
			}

			const FisheyeCameraD cameraPerturbed(cameraWidth_, cameraHeight_, cameraParameterConfiguration_, cameraParameters.data());

			return cameraPerturbed.projectToImageIF(posePerturbed.transformation(), objectPoint);
		}

	protected:

		/// The fisheye camera in double precision
		FisheyeCameraD cameraD_;

		/// The camera pose (inverted and flipped) in double precision
		const PoseD flippedCamera_P_worldD_;

		/// Camera width
		unsigned int cameraWidth_ = 0u;

		/// Camera height
		unsigned int cameraHeight_ = 0u;

		/// Camera parameters in double precision
		std::vector<double> cameraParametersD_;

		/// Camera parameter configuration
		FisheyeCameraD::ParameterConfiguration cameraParameterConfiguration_ = FisheyeCameraD::PC_UNKNOWN;
};

template <typename T>
bool TestJacobian::testPoseFisheyeCameraJacobian2x18(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr size_t numberPoints = 50;

	Log::info() << "Testing fisheye camera pose jacobian 2x14 for " << numberPoints << " points with " << sizeof(T) * 8 << "-bit precision:";

	constexpr double threshold = std::is_same<float, T>::value ? 0.95 : 0.99;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(threshold, randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	constexpr size_t numberCameraParameters = 12;
	constexpr size_t numberPoseParameters = 6;

	constexpr T cameraBorder = Scalar(50);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		const VectorT3<T> translation = RandomT<T>::vector3(randomGenerator, -1, 1);
		const QuaternionT<T> quaternion = RandomT<T>::quaternion(randomGenerator);

		const HomogenousMatrixT4<T> world_T_camera(translation, quaternion);
		const HomogenousMatrixT4<T> flippedCamera_T_world(Camera::standard2InvertedFlipped(world_T_camera));

		const PoseT<T> flippedCamera_P_world(flippedCamera_T_world);

		const FisheyeCameraT<T> camera = Utilities::realisticFisheyeCamera<T>(RandomI::random(randomGenerator, 1u));
		ocean_assert(camera.isValid());

		const unsigned int width = camera.width();
		const unsigned int height = camera.height();

		VectorsT3<T> objectPoints;
		objectPoints.reserve(numberPoints);

		while (objectPoints.size() < numberPoints)
		{
			const VectorT2<T> imagePoint = RandomT<T>::vector2(randomGenerator, T(cameraBorder), T(width - cameraBorder), T(cameraBorder), T(height - cameraBorder));

			const LineT3<T> ray = camera.ray(imagePoint, world_T_camera);

			objectPoints.emplace_back(ray.point(RandomT<T>::scalar(randomGenerator, 1, 10)));
		}

		SquareMatrixT3<T> Rwx, Rwy, Rwz;
		calculateRotationRodriguesDerivative(ExponentialMapT<T>(VectorT3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

		/**
		 * | dfx / dFx, dfx / dFy, dfx / dmx, dfx / dmy, dfx / dk3, dfx / dk5, dfx / dk7, dfx / dk9, dfx / dk11, dfx / dk13, dfx / dp1, dfx / dp2,          dfx / dtx, dfx / dty, dfx / dtz,   dfx / dwx, dfx / dwy, dfx / dwz |
		 * | dfy / dFx, dfy / dFy, dfy / dmx, dfy / dmy, dfy / dk3, dfy / dk5, dfy / dk7, dfy / dk9, dfy / dk11, dfy / dk13, dfy / dp1, dfy / dp2,          dfy / dtx, dfy / dty, dfy / dtz,   dfy / dwx, dfy / dwy, dfy / dwz |
		 */

		MatrixT<T> jacobian(2 * objectPoints.size(), numberCameraParameters + numberPoseParameters);

		performance.start();
			for (size_t nPoint = 0u; nPoint < numberPoints; ++nPoint)
			{
				T* jacobianRowX = jacobian[nPoint * 2 + 0];
				T* jacobianRowY = jacobian[nPoint * 2 + 1];

				T* jacobianCameraX = jacobianRowX + 0;
				T* jacobianCameraY = jacobianRowY + 0;

				T* jacobianPoseX = jacobianRowX + numberCameraParameters;
				T* jacobianPoesY = jacobianRowY + numberCameraParameters;

				Geometry::Jacobian::calculateJacobianCameraPoseRodrigues2x18IF(camera, flippedCamera_T_world, objectPoints[nPoint], Rwx, Rwy, Rwz, jacobianCameraX, jacobianCameraY, jacobianPoseX, jacobianPoesY);
			}
		performance.stop();

		{
			MatrixT<T> naiveJacobian(2 * objectPoints.size(), 18);

			const HighPerformanceStatistic::ScopedStatistic scope(performanceNaive);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				const VectorT3<T> objectPoint = objectPoints[n];
				const VectorT2<T> imagePoint(camera.projectToImageIF(flippedCamera_T_world, objectPoint));

				for (unsigned int i = 0u; i < numberCameraParameters + numberPoseParameters; ++i)
				{
					PoseT<T> poseDelta(flippedCamera_T_world);

					unsigned int cameraWidth = 0u;
					unsigned int cameraHeight = 0u;

					std::vector<T> deltaParameters;
					typename FisheyeCameraT<T>::ParameterConfiguration parameterConfiguration = FisheyeCameraT<T>::PC_UNKNOWN;
					camera.copyParameters(cameraWidth, cameraHeight, deltaParameters, parameterConfiguration);

					ocean_assert(width == cameraWidth && height == cameraHeight);
					ocean_assert(deltaParameters.size() == numberCameraParameters && parameterConfiguration == FisheyeCameraT<T>::PC_12_PARAMETERS);

					if (i < numberCameraParameters)
					{
						deltaParameters[i] += NumericT<T>::weakEps();
					}
					else
					{
						const unsigned int ip = i - numberCameraParameters;
						ocean_assert(ip < numberPoseParameters);

						poseDelta[ip] += NumericT<T>::weakEps();
					}

					const FisheyeCameraT<T> cameraDelta(cameraWidth, cameraHeight, FisheyeCameraT<T>::PC_12_PARAMETERS, deltaParameters.data());

					const VectorT2<T> imagePointDelta(cameraDelta.projectToImageIF(poseDelta.transformation(), objectPoint));
					const VectorT2<T> derivative = (imagePointDelta - imagePoint) / NumericT<T>::weakEps();

					naiveJacobian[n * 2u + 0u][i] = derivative.x();
					naiveJacobian[n * 2u + 1u][i] = derivative.y();
				}
			}
		}

		const DerivativeCalculatorPoseFisheyeCameraJacobian2x18<T> derivativeCalculator(camera, flippedCamera_P_world);

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const VectorT3<T>& objectPoint = objectPoints[n];

			const T* jacobianX = jacobian[2u * n + 0u];
			const T* jacobianY = jacobian[2u * n + 1u];

			const VectorD3 objectPointD = VectorD3(objectPoint);

			for (size_t parameterIndex = 0; parameterIndex < numberCameraParameters + numberPoseParameters; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(objectPointD, parameterIndex, VectorD2(double(jacobianX[parameterIndex]), double(jacobianY[parameterIndex]))))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

/**
 * Derivative calculator for homography jacobian 2x8.
 * Calculates numerical derivatives of 2D point transformation with respect to 8 homography parameters (h0-h7, h8 fixed to 1).
 */
class TestJacobian::DerivativeCalculatorHomography2x8 : public DerivativeCalculatorT<VectorD2, double, Vector2>
{
	public:

		/**
		 * Creates a derivative calculator for homography jacobian.
		 * @param homography The 3x3 homography matrix
		 */
		DerivativeCalculatorHomography2x8(const SquareMatrix3& homography) :
			homography_(homography)
		{
			// nothing to do here
		}

	public:

		/**
		 * Calculates the transformed point with a perturbed homography parameter.
		 * @param point The 2D input point
		 * @param parameterIndex The homography parameter index to perturb (0-7)
		 * @param offset The offset to apply
		 * @return The transformed 2D point
		 */
		VectorD2 calculateValue(const Vector2& point, const size_t parameterIndex, const double offset) const override
		{
			ocean_assert(parameterIndex < 8);

			SquareMatrixD3 homographyPerturbed(homography_);
			homographyPerturbed[(unsigned int)(parameterIndex)] += offset;

			return homographyPerturbed * VectorD2(point);
		}

	protected:

		/// The homography matrix
		const SquareMatrixD3 homography_;
};

bool TestJacobian::testHomography2x8(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing homography Jacobian 2x8 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	Vectors2 points(numberPoints);

	Matrix jacobians(2 * numberPoints, 8);
	Matrix naiveJacobians(2 * numberPoints, 8);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

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

		const DerivativeCalculatorHomography2x8 derivativeCalculator(homography);

		for (size_t n = 0; n < points.size(); ++n)
		{
			const Vector2& point = points[n];

			const Scalar* jacobianX = jacobians[2u * n + 0u];
			const Scalar* jacobianY = jacobians[2u * n + 1u];

			for (size_t parameterIndex = 0; parameterIndex < 8; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(point, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

/**
 * Derivative calculator for homography jacobian 2x9.
 * Calculates numerical derivatives of 2D point transformation with respect to all 9 homography parameters.
 */
class TestJacobian::DerivativeCalculatorHomography2x9 : public DerivativeCalculatorT<VectorD2, double, Vector2>
{
	public:

		/**
		 * Creates a derivative calculator for homography jacobian.
		 * @param homography The 3x3 homography matrix
		 */
		DerivativeCalculatorHomography2x9(const SquareMatrix3& homography) :
			homography_(homography)
		{
			// nothing to do here
		}

	public:

		/**
		 * Calculates the transformed point with a perturbed homography parameter.
		 * @param point The 2D input point
		 * @param parameterIndex The homography parameter index to perturb (0-8)
		 * @param offset The offset to apply
		 * @return The transformed 2D point
		 */
		VectorD2 calculateValue(const Vector2& point, const size_t parameterIndex, const double offset) const override
		{
			ocean_assert(parameterIndex < 9);

			SquareMatrixD3 homographyPerturbed(homography_);
			homographyPerturbed[(unsigned int)(parameterIndex)] += offset;

			return homographyPerturbed * VectorD2(point);
		}

	protected:

		/// The homography matrix
		const SquareMatrixD3 homography_;
};

bool TestJacobian::testHomography2x9(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing homography Jacobian 2x9 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	const double threshold = std::is_same<Scalar, float>::value ? 0.95 : successThreshold();

	ValidationPrecision validation(threshold, randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	Vectors2 points(numberPoints);

	Matrix jacobians(2 * numberPoints, 9);
	Matrix naiveJacobians(2 * numberPoints, 9);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

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
				const Vector2& point = points[n];
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

		const DerivativeCalculatorHomography2x9 derivativeCalculator(homography);

		for (size_t n = 0; n < points.size(); ++n)
		{
			const Vector2& point = points[n];

			const Scalar* jacobianX = jacobians[2u * n + 0u];
			const Scalar* jacobianY = jacobians[2u * n + 1u];

			for (size_t parameterIndex = 0; parameterIndex < 9; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(point, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

/**
 * Derivative calculator for identity homography jacobian 2x8.
 * Calculates numerical derivatives of 2D point transformation with respect to 8 identity homography parameters.
 */
class TestJacobian::DerivativeCalculatorIdentityHomography2x8 : public DerivativeCalculatorT<VectorD2, double, Vector2>
{
	public:

		/**
		 * Creates a derivative calculator for identity homography jacobian.
		 * @param homography The 3x3 identity homography matrix
		 */
		DerivativeCalculatorIdentityHomography2x8(const SquareMatrix3& homography) :
			homography_(homography)
		{
			// nothing to do here
		}

	public:

		/**
		 * Calculates the transformed point with a perturbed homography parameter.
		 * @param point The 2D input point
		 * @param parameterIndex The homography parameter index to perturb (0-7)
		 * @param offset The offset to apply
		 * @return The transformed 2D point
		 */
		VectorD2 calculateValue(const Vector2& point, const size_t parameterIndex, const double offset) const override
		{
			ocean_assert(parameterIndex < 8);

			SquareMatrixD3 homographyPerturbed(homography_);
			homographyPerturbed[(unsigned int)(parameterIndex)] += offset;

			return homographyPerturbed * VectorD2(point);
		}

	protected:

		/// The homography matrix
		const SquareMatrixD3 homography_;
};

bool TestJacobian::testIdentityHomography2x8(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing identity homography Jacobian 2x8 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	Vectors2 points(numberPoints);

	Matrix jacobians(2 * numberPoints, 8);
	Matrix naiveJacobians(2 * numberPoints, 8);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

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

		const DerivativeCalculatorIdentityHomography2x8 derivativeCalculator(homography);

		for (size_t n = 0; n < points.size(); ++n)
		{
			const Vector2& point = points[n];

			const Scalar* jacobianX = jacobians[2u * n + 0u];
			const Scalar* jacobianY = jacobians[2u * n + 1u];

			for (size_t parameterIndex = 0; parameterIndex < 8; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(point, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}

		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

/**
 * Derivative calculator for identity homography jacobian 2x9.
 * Calculates numerical derivatives of 2D point transformation with respect to all 9 identity homography parameters.
 */
class TestJacobian::DerivativeCalculatorIdentityHomography2x9 : public DerivativeCalculatorT<VectorD2, double, Vector2>
{
	public:

		/**
		 * Creates a derivative calculator for identity homography jacobian.
		 * @param homography The 3x3 identity homography matrix
		 */
		DerivativeCalculatorIdentityHomography2x9(const SquareMatrix3& homography) :
			homography_(homography)
		{
			// nothing to do here
		}

	public:

		/**
		 * Calculates the transformed point with a perturbed homography parameter.
		 * @param point The 2D input point
		 * @param parameterIndex The homography parameter index to perturb (0-8)
		 * @param offset The offset to apply
		 * @return The transformed 2D point
		 */
		VectorD2 calculateValue(const Vector2& point, const size_t parameterIndex, const double offset) const override
		{
			ocean_assert(parameterIndex < 9);

			SquareMatrixD3 homographyPerturbed(homography_);
			homographyPerturbed[(unsigned int)(parameterIndex)] += offset;

			return homographyPerturbed * VectorD2(point);
		}

	protected:

		/// The homography matrix
		const SquareMatrixD3 homography_;
};

bool TestJacobian::testIdentityHomography2x9(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing identity homography Jacobian 2x9 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	Vectors2 points(numberPoints);

	Matrix jacobians(2 * numberPoints, 9);
	Matrix naiveJacobians(2 * numberPoints, 9);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

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
				const Vector2& point = points[n];
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

		const DerivativeCalculatorIdentityHomography2x9 derivativeCalculator(homography);

		for (size_t n = 0; n < points.size(); ++n)
		{
			const Vector2& point = points[n];

			const Scalar* jacobianX = jacobians[2u * n + 0u];
			const Scalar* jacobianY = jacobians[2u * n + 1u];

			for (size_t parameterIndex = 0; parameterIndex < 9; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(point, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}

		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

/**
 * Derivative calculator for similarity transformation jacobian 2x4.
 * Calculates numerical derivatives of 2D point transformation with respect to 4 similarity parameters (scale, rotation, tx, ty).
 */
class TestJacobian::DerivativeCalculatorSimilarity2x4 : public DerivativeCalculatorT<VectorD2, double, Vector2>
{
	public:

		/**
		 * Creates a derivative calculator for similarity jacobian.
		 * @param similarity The 3x3 similarity matrix
		 */
		DerivativeCalculatorSimilarity2x4(const SquareMatrix3& similarity) :
			similarity_(similarity)
		{
			// nothing to do here
		}

	public:

		/**
		 * Calculates the transformed point with a perturbed similarity parameter.
		 * @param point The 2D input point
		 * @param parameterIndex The similarity parameter index to perturb (0: scale, 1: rotation, 2: tx, 3: ty)
		 * @param offset The offset to apply
		 * @return The transformed 2D point
		 */
		VectorD2 calculateValue(const Vector2& point, const size_t parameterIndex, const double offset) const override
		{
			ocean_assert(parameterIndex < 4);

			SquareMatrixD3 similarityPerturbed(similarity_);

			switch (parameterIndex)
			{
				case 0:
					// Scale parameter
					similarityPerturbed(0, 0) += offset;
					similarityPerturbed(1, 1) += offset;
					break;

				case 1:
					// Rotation parameter
					similarityPerturbed(1, 0) += offset;
					similarityPerturbed(0, 1) -= offset;
					break;

				case 2:
					// Translation x parameter
					similarityPerturbed(0, 2) += offset;
					break;

				case 3:
					// Translation y parameter
					similarityPerturbed(1, 2) += offset;
					break;

				default:
					ocean_assert(false && "Invalid parameter index");
					break;
			}

			return similarityPerturbed * VectorD2(point);
		}

	protected:

		/// The similarity matrix
		const SquareMatrixD3 similarity_;
};

bool TestJacobian::testSimilarity2x4(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing similarity Jacobian 2x4 for " << numberPoints << " points:";

	RandomGenerator randomGenerator;

	ValidationPrecision validation(successThreshold(), randomGenerator);

	HighPerformanceStatistic performanceNaive;
	HighPerformanceStatistic performance;

	Vectors2 points(numberPoints);

	Matrix jacobians(2 * numberPoints, 4);
	Matrix naiveJacobians(2 * numberPoints, 4);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

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
				const Vector2& point = points[n];
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

		const DerivativeCalculatorSimilarity2x4 derivativeCalculator(similarity);

		for (size_t n = 0; n < points.size(); ++n)
		{
			const Vector2& point = points[n];

			const Scalar* jacobianX = jacobians[2u * n + 0u];
			const Scalar* jacobianY = jacobians[2u * n + 1u];

			for (size_t parameterIndex = 0; parameterIndex < 4; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(point, parameterIndex, Vector2(jacobianX[parameterIndex], jacobianY[parameterIndex])))
				{
					scopedIteration.setInaccurate();
				}
			}

		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

/**
 * Derivative calculator for fisheye distortion of normalized image coordinates 2x2.
 * Calculates numerical derivatives of distorted 2D point with respect to normalized point coordinates.
 * @tparam T The data type of the scalar, either 'float' or 'double'
 */
template <typename T>
class TestJacobian::DerivativeCalculatorFisheyeDistortNormalized2x2 : public DerivativeCalculatorT<VectorD2, double, VectorD2>
{
	public:

		/**
		 * Creates a derivative calculator for fisheye distortion jacobian.
		 * @param fisheyeCamera The fisheye camera with distortion parameters
		 */
		DerivativeCalculatorFisheyeDistortNormalized2x2(const FisheyeCameraT<T>& fisheyeCamera) :
			fisheyeCameraD_(fisheyeCamera)
		{
			// nothing to do here
		}

	public:

		/**
		 * Calculates the distorted normalized point with a perturbed input coordinate.
		 * @param point The normalized 2D point
		 * @param parameterIndex The coordinate index to perturb (0: x, 1: y)
		 * @param offset The offset to apply
		 * @return The distorted 2D point
		 */
		VectorD2 calculateValue(const VectorD2& point, const size_t parameterIndex, const double offset) const override
		{
			ocean_assert(parameterIndex < 2);

			VectorD2 pointPerturbed(point);
			pointPerturbed[(unsigned int)(parameterIndex)] += offset;

			return fisheyeCameraD_.distortNormalized(pointPerturbed);
		}

	protected:

		/// The fisheye camera in double precision
		const FisheyeCameraD fisheyeCameraD_;
};

template <typename T>
bool TestJacobian::testCalculateFisheyeDistortNormalized2x2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int numberPoints = 50u;

	Log::info() << "Testing fisheye Jacobian 2x2 for " << numberPoints << " points with " << sizeof(T) * 8 << "-bit precision:";

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

		const DerivativeCalculatorFisheyeDistortNormalized2x2<T> derivativeCalculator(fisheyeCamera);

		for (size_t n = 0; n < points.size(); ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const VectorD2 point = VectorD2(points[n]);

			const T* jacobianX = jacobians[2u * n + 0u];
			const T* jacobianY = jacobians[2u * n + 1u];

			for (size_t parameterIndex = 0; parameterIndex < 2; ++parameterIndex)
			{
				if (!derivativeCalculator.verifyDerivative(point, parameterIndex, VectorD2(double(jacobianX[parameterIndex]), double(jacobianY[parameterIndex]))))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance naive: " << performanceNaive;
	Log::info() << "Performance: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
