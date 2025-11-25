/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_JACOBIAN_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_JACOBIAN_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/geometry/Jacobian.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements Jacobian tests.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestJacobian : protected Geometry::Jacobian
{
	protected:

		/**
		 * Base class for calculating numerical derivatives using central finite differences.
		 * @tparam T The data type of the derivative
		 * @tparam TScalar The scalar type for epsilon, either 'float' or 'double'
		 * @tparam TVariable The data type of a variable object which can change while while everything else is constant
		 */
		template <typename T, typename TScalar, typename TVariable>
		class DerivativeCalculatorT
		{
			public:

				/**
				 * Default constructor.
				 */
				virtual ~DerivativeCalculatorT() = default;

				/**
				 * Calculates the numerical derivative using central finite differences: f'(x) ≈ [f(x + epsilon) - f(x - epsilon)] / (2 * epsilon)
				 * @param variable The variable to calculate the derivative for
				 * @param parameterIndex The index of the parameter to perturb
				 * @param epsilon The epsilon value to use for the finite difference calculation
				 * @return The calculated derivative
				 */
				T calculateDerivative(const TVariable& variable, const size_t parameterIndex, const TScalar epsilon = NumericT<TScalar>::weakEps() * TScalar(0.05)) const;

				/**
				 * Verifies an analytical derivative by comparing it with a numerical approximation.
				 * Tests multiple epsilon values to find one that produces a match within tolerance.
				 * @param variable The variable to calculate the derivative for
				 * @param parameterIndex The index of the parameter to verify
				 * @param analyticalDerivative The analytical derivative to verify
				 * @return True if the analytical derivative matches the numerical approximation within tolerance
				 * @tparam TAnalyticalDerivative The type of the analytical derivative (e.g., Vector2, Vector3)
				 */
				template <typename TAnalyticalDerivative>
				bool verifyDerivative(const TVariable& variable, const size_t parameterIndex, const TAnalyticalDerivative& analyticalDerivative) const;

			protected:

				/**
				 * Calculates the function value with an offset applied to a specific parameter.
				 * Must be implemented by derived classes.
				 * @param variable The variable to evaluate
				 * @param parameterIndex The index of the parameter to perturb
				 * @param offset The offset to apply
				 * @return The calculated function value
				 */
				virtual T calculateValue(const TVariable& variable, const size_t parameterIndex, const TScalar offset) const = 0;
		};

		/**
		 * Base class for calculating numerical derivatives using central finite differences.
		 * This class is a specialization of DerivativeCalculatorT without a variable object.
		 * @tparam T The result type (e.g., VectorT2<Scalar>, VectorT3<Scalar>)
		 * @tparam TScalar The scalar type for epsilon, either 'float' or 'double'
		 */
		template <typename T, typename TScalar>
		class DerivativeCalculatorT<T, TScalar, void>
		{
			public:

				/**
				 * Default constructor.
				 */
				virtual ~DerivativeCalculatorT() = default;

				/**
				 * Calculates the numerical derivative using central finite differences: f'(x) ≈ [f(x + epsilon) - f(x - epsilon)] / (2 * epsilon)
				 * @param parameterIndex The index of the parameter to perturb
				 * @param epsilon The epsilon value to use for the finite difference calculation
				 * @return The calculated derivative
				 */
				T calculateDerivative(const size_t parameterIndex, const TScalar epsilon = NumericT<TScalar>::weakEps() * TScalar(0.05)) const;

				/**
				 * Verifies an analytical derivative by comparing it with a numerical approximation.
				 * Tests multiple epsilon values to find one that produces a match within tolerance.
				 * @param parameterIndex The index of the parameter to verify
				 * @param analyticalDerivative The analytical derivative to verify
				 * @return True if the analytical derivative matches the numerical approximation within tolerance
				 * @tparam TAnalyticalDerivative The type of the analytical derivative (e.g., Vector2, Vector3)
				 */
				template <typename TAnalyticalDerivative>
				bool verifyDerivative(const size_t parameterIndex, const TAnalyticalDerivative& analyticalDerivative) const;

			protected:

				/**
				 * Calculates the function value with an offset applied to a specific parameter.
				 * Must be implemented by derived classes.
				 * @param parameterIndex The index of the parameter to perturb
				 * @param offset The offset to apply
				 * @return The calculated function value
				 */
				virtual T calculateValue(const size_t parameterIndex, const TScalar offset) const = 0;
		};

		/// Forward declaration.
		template <typename T>
		class DerivativeCalculatorOrientationalJacobian2x3;

		/// Forward declaration.
		template <typename T>
		class DerivativeCalculatorOrientationJacobian2nx3;

		/// Forward declaration.
		class DerivativeCalculatorPinholeCameraPoseJacobian2nx6;

		/// Forward declaration.
		class DerivativeCalculatorFisheyeCameraPoseJacobian2x6;

		/// Forward declaration.
		template <typename T>
		class DerivativeCalculatorAnyCameraPoseJacobian2nx6;

		/// Forward declaration.
		class DerivativeCalculatorPoseJacobianDampedDistortion2nx6;

		/// Forward declaration.
		class DerivativeCalculatorPoseZoomJacobian2nx7;

		/// Forward declaration.
		class DerivativeCalculatorPinholeCameraObjectTransformation2nx6;

		/// Forward declaration.
		class DerivativeCalculatorFisheyeCameraObjectTransformation2nx6;

		/// Forward declaration.
		class DerivativeCalculatorPinholeCameraPointJacobian2nx3;

		/// Forward declaration.
		class DerivativeCalculatorFisheyeCameraPointJacobian2x3;

		/// Forward declaration.
		class DerivativeCalculatorAnyCameraPointJacobian2x3;

		/// Forward declaration.
		class DerivativeCalculatorSphericalObjectPoint3x3;

		/// Forward declaration.
		template <typename T>
		class DerivativeCalculatorSphericalObjectPointOrientation2x3IF;

		/// Forward declaration.
		class DerivativeCalculatorPinholeCameraDistortionJacobian2x4;

		/// Forward declaration.
		class DerivativeCalculatorPinholeCameraJacobian2x6;

		/// Forward declaration.
		class DerivativeCalculatorPinholeCameraJacobian2x7;

		/// Forward declaration.
		class DerivativeCalculatorPinholeCameraJacobian2x8;

		/// Forward declaration.
		class DerivativeCalculatorFisheyeCameraJacobian2x12;

		/// Forward declaration.
		class DerivativeCalculatorOrientationPinholeCameraJacobian2x11;

		/// Forward declaration.
		class DerivativeCalculatorPosePinholeCameraJacobian2x12;

		/// Forward declaration.
		template <typename T>
		class DerivativeCalculatorPosePinholeCameraJacobian2x14;

		/// Forward declaration.
		template <typename T>
		class DerivativeCalculatorPoseFisheyeCameraJacobian2x18;

		/// Forward declaration.
		class DerivativeCalculatorHomography2x8;

		/// Forward declaration.
		class DerivativeCalculatorHomography2x9;

		/// Forward declaration.
		class DerivativeCalculatorIdentityHomography2x8;

		/// Forward declaration.
		class DerivativeCalculatorIdentityHomography2x9;

		/// Forward declaration.
		class DerivativeCalculatorSimilarity2x4;

		/// Forward declaration.
		template <typename T>
		class DerivativeCalculatorFisheyeDistortNormalized2x2;

	public:

		/**
		 * Tests the entire Jacobian functionality.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param selector Selector for individual tests
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the Jacobian for the rotational part of extrinsic camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T the data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testOrientationalJacobian2x3(const double testDuration);

		/**
		 * Tests the Jacobian for the orientation of a camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T the data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testOrientationJacobian2nx3(const double testDuration);

		/**
		 * Deprecated.
		 *
		 * Tests the Jacobian for the extrinsic camera parameters and a pinhole camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPinholeCameraPoseJacobian2nx6(const double testDuration);

		/**
		 * Deprecated.
		 *
		 * Tests the Jacobian for the extrinsic camera parameters and fisheye camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFisheyeCameraPoseJacobian2x6(const double testDuration);

		/**
		 * Tests the Jacobian for the extrinsic camera parameters and any camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T the data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testAnyCameraPoseJacobian2nx6(const double testDuration);

		/**
		 * Tests the Jacobian for the extrinsic camera parameters using a damped distortion.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPoseJacobianDampedDistortion2nx6(const double testDuration);

		/**
		 * Tests the Jacobian for the extrinsic camera parameters (including the zoom parameter).
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPoseZoomJacobian2nx7(const double testDuration);

		/**
		 * Tests the 2x6 Jacobian for a 6-DOF transformation with fixed camera pose.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPinholeCameraObjectTransformation2nx6(const double testDuration);

		/**
		 * Tests the 2x6 Jacobian for a 6-DOF transformation with fixed camera pose.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFisheyeCameraObjectTransformation2nx6(const double testDuration);

		/**
		 * Tests the Jacobian for an 2D image point projection in relation to an object point using a pinhole camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPinholeCameraPointJacobian2nx3(const double testDuration);

		/**
		 * Tests the Jacobian for an 2D image point projection in relation to an object point using a fisheye camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFisheyeCameraPointJacobian2x3(const double testDuration);

		/**
		 * Tests the Jacobian for an 2D image point projection in relation to an object point using any camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAnyCameraPointJacobian2x3(const double testDuration);

		/**
		 * Tests the Jacobian for two 6DOF poses and a set of 3D object points.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPosesPointsJacobian2nx12(const double testDuration);

		/**
		 * Tests the Jacobian for an exponential map representing a 3D object point.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSphericalObjectPoint3x3(const double testDuration);

		/**
		 * Tests the Jacobian for an exponential map representing a 3D object point which is projected into the camera frame.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T the data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testSphericalObjectPointOrientation2x3IF(const double testDuration);

		/**
		 * Tests the Jacobian for the radial and tangential distortion camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPinholeCameraDistortionJacobian2x4(const double testDuration);

		/**
		 * Tests the Jacobian for the intrinsic and radial distortion of pinhole camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPinholeCameraJacobian2x6(const double testDuration);

		/**
		 * Tests the Jacobian for the intrinsic and radial and tangential distortion of pinhole camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPinholeCameraJacobian2x7(const double testDuration);

		/**
		 * Tests the Jacobian for the intrinsic, radial and tangential distortion of pinhole camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T the data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testPinholeCameraJacobian2x8(const double testDuration);

		/**
		 * Tests the Jacobian for the intrinsic, radial and tangential distortion of fisheye camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T the data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testFisheyeCameraJacobian2x12(const double testDuration);

		/**
		 * Tests the Jacobian for the rotational part of the extrinsic camera matrix, intrinsic and radial and tangential camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testOrientationPinholeCameraJacobian2x11(const double testDuration);

		/**
		 * Tests the Jacobian for the extrinsic, intrinsic and radial camera parameters for a pinhole camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPosePinholeCameraJacobian2x12(const double testDuration);

		/**
		 * Tests the Jacobian for the extrinsic, intrinsic and radial and tangential distortion camera parameters for a pinhole camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T the data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testPosePinholeCameraJacobian2x14(const double testDuration);

		/**
		 * Tests the Jacobian for the extrinsic, intrinsic and radial camera parameters and distortion parameters for a fisheye camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		template <typename T>
		static bool testPoseFisheyeCameraJacobian2x18(const double testDuration);

		/**
		 * Tests the 2x8 Jacobian for the homography.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHomography2x8(const double testDuration);

		/**
		 * Tests the 2x9 Jacobian for the homography.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHomography2x9(const double testDuration);

		/**
		 * Tests the 2x8 Jacobian for the identity homography.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIdentityHomography2x8(const double testDuration);

		/**
		 * Tests the 2x9 Jacobian for the identity homography.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIdentityHomography2x9(const double testDuration);

		/**
		 * Tests the 2x4 Jacobian of a similarity transformation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSimilarity2x4(const double testDuration);

		/**
		 * Tests the 2x2 Jacobian of the distortion of a normalized image point for a fisheye camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T the data type of the scalar to be used, 'float', or 'double'
		 */
		template <typename T>
		static bool testCalculateFisheyeDistortNormalized2x2(const double testDuration);

	private:

		/**
		 * Checks if an analytical derivative matches an approximated derivative within tolerance.
		 * @param analyticalDerivative The analytical derivative value
		 * @param approximatedDerivative The numerically approximated derivative value
		 * @return True if the derivatives match within tolerance
		 * @tparam T The data type of the derivative
		 */
		template <typename T>
		static bool checkDerivative(const T& analyticalDerivative, const T& approximatedDerivative);

		/**
		 * Checks if an analytical derivative matches an approximated derivative within tolerance.
		 * @param analyticalDerivative The analytical derivative value
		 * @param approximatedDerivative The numerically approximated derivative value
		 * @return True if the derivatives match within tolerance
		 * @tparam T The data type of the scalar, either 'float' or 'double'
		 */
		template <typename T>
		static bool checkScalarDerivative(const T analyticalDerivative, const T approximatedDerivative);

		/**
		 * Calculates the two Jacobian rows for a given pose and dynamic object point.
		 * The derivatives are calculated for the 3D object point only.<br>
		 * The resulting Jacobian rows have the following form:<br>
		 * | dfx / dX, dfx / dY, dfx / dZ |<br>
		 * | dfy / dX, dfy / dY, dfy / dZ |<br>
		 * @param jx First row position of the jacobian, with 3 column entries receiving the point derivatives
		 * @param jy Second row position of the jacobian, with 3 column entries receiving the point derivatives
		 * @param pinholeCamera The pinhole camera to determine the Jacobian values for
		 * @param flippedCamera_T_world Pose to determine the Jacobian for (inverted and flipped)
		 * @param objectPoint 3D object point to determine the Jacobian for
		 * @param distortImagePoint True, to force the distortion of the image point using the distortion parameters of this camera object
		 * @tparam tUseBorderDistortionIfOutside True, to apply the distortion from the nearest point lying on the frame border if the point lies outside the visible camera area; False to apply the distortion from the given position
		 */
		template <bool tUseBorderDistortionIfOutside>
		static void calculatePointJacobian2x3(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& objectPoint, const bool distortImagePoint);

		/**
		 * Returns the minimal threshold necessary to succeed a verification.
		 * @return The necessary success rate, in percent, with range [0, 1]
		 */
		static constexpr double successThreshold();
};

template <>
inline bool TestJacobian::checkDerivative(const float& analyticalDerivative, const float& approximatedDerivative)
{
	return checkScalarDerivative<float>(analyticalDerivative, approximatedDerivative);
}

template <>
inline bool TestJacobian::checkDerivative(const double& analyticalDerivative, const double& approximatedDerivative)
{
	return checkScalarDerivative<double>(analyticalDerivative, approximatedDerivative);
}

template <>
inline bool TestJacobian::checkDerivative(const VectorF2& analyticalDerivative, const VectorF2& approximatedDerivative)
{
	if (!checkScalarDerivative<float>(analyticalDerivative.x(), approximatedDerivative.x()))
	{
		return false;
	}

	if (!checkScalarDerivative<float>(analyticalDerivative.y(), approximatedDerivative.y()))
	{
		return false;
	}

	return true;
}

template <>
inline bool TestJacobian::checkDerivative(const VectorD2& analyticalDerivative, const VectorD2& approximatedDerivative)
{
	if (!checkScalarDerivative<double>(analyticalDerivative.x(), approximatedDerivative.x()))
	{
		return false;
	}

	if (!checkScalarDerivative<double>(analyticalDerivative.y(), approximatedDerivative.y()))
	{
		return false;
	}

	return true;
}

template <>
inline bool TestJacobian::checkDerivative(const VectorF3& analyticalDerivative, const VectorF3& approximatedDerivative)
{
	if (!checkScalarDerivative<float>(analyticalDerivative.x(), approximatedDerivative.x()))
	{
		return false;
	}

	if (!checkScalarDerivative<float>(analyticalDerivative.y(), approximatedDerivative.y()))
	{
		return false;
	}

	if (!checkScalarDerivative<float>(analyticalDerivative.z(), approximatedDerivative.z()))
	{
		return false;
	}

	return true;
}

template <>
inline bool TestJacobian::checkDerivative(const VectorD3& analyticalDerivative, const VectorD3& approximatedDerivative)
{
	if (!checkScalarDerivative<double>(analyticalDerivative.x(), approximatedDerivative.x()))
	{
		return false;
	}

	if (!checkScalarDerivative<double>(analyticalDerivative.y(), approximatedDerivative.y()))
	{
		return false;
	}

	if (!checkScalarDerivative<double>(analyticalDerivative.z(), approximatedDerivative.z()))
	{
		return false;
	}

	return true;
}

template <typename T>
inline bool TestJacobian::checkDerivative(const T& /*analyticalDerivative*/, const T& /*approximatedDerivative*/)
{
	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	return false;
}

template <typename T>
bool TestJacobian::checkScalarDerivative(const T analyticalDerivative, const T approximatedDerivative)
{
	const double maxAbsValue = std::max(std::abs(analyticalDerivative), std::abs(approximatedDerivative));
	const double absDiff = std::abs(analyticalDerivative - approximatedDerivative);

	if (NumericT<T>::isEqualEps(analyticalDerivative) || NumericT<T>::isEqualEps(approximatedDerivative))
	{
		if (absDiff > T(0.001))
		{
			return false;
		}
	}
	else
	{
		if (NumericT<T>::isNotEqualEps(T(maxAbsValue)) && T(absDiff / maxAbsValue) > T(0.05))
		{
			return false;
		}
	}

	return true;
}

template <bool tUseBorderDistortionIfOutside>
void TestJacobian::calculatePointJacobian2x3(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& objectPoint, const bool distortImagePoint)
{
	ocean_assert(jx != nullptr && jy != nullptr);
	ocean_assert(flippedCamera_T_world.isValid());

	/**
	 * | dfx / dX, dfx / dY, dfx / dZ |<br>
	 * | dfy / dX, dfy / dY, dfy / dZ |<br>
	 */

	const Scalar eps = Numeric::weakEps();

	const Vector2 imagePoint(pinholeCamera.projectToImageIF<tUseBorderDistortionIfOutside>(flippedCamera_T_world, objectPoint, distortImagePoint));

	for (unsigned int n = 0u; n < 3u; ++n)
	{
		Vector3 dObjectPoint(objectPoint);

		dObjectPoint[n] += eps;

		const Vector2 dImagePoint(pinholeCamera.projectToImageIF<tUseBorderDistortionIfOutside>(flippedCamera_T_world, dObjectPoint, distortImagePoint));

		const Scalar dx = (dImagePoint.x() - imagePoint.x()) / eps;
		const Scalar dy = (dImagePoint.y() - imagePoint.y()) / eps;

		jx[n] = dx;
		jy[n] = dy;
	}
}

constexpr double TestJacobian::successThreshold()
{
	return 0.99;
}

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_JACOBIAN_H
