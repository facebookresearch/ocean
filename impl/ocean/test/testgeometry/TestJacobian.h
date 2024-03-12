// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_JACOBIAN_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_JACOBIAN_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/geometry/Jacobian.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"

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
	public:

		/**
		 * Tests the entire Jacobian functionality.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the Jacobian for the rotational part of extrinsic camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T the data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename T>
		static bool testAnyCameraOrientationJacobian2x3(const double testDuration);

		/**
		 * Tests the Jacobian for the extrinsic camera parameters and a pinhole camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPinholeCameraPoseJacobian2nx6(const double testDuration);

		/**
		 * Tests the Jacobian for the extrinsic camera parameters and fisheye camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFisheyeCameraPoseJacobian2x6(const double testDuration);

		/**
		 * Tests the Jacobian for the extrinsic camera parameters and any camera.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAnyCameraPoseJacobian2x6(const double testDuration);

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
		 */
		static bool testSphericalObjectPointOrientation2x3(const double testDuration);

		/**
		 * Tests the Jacobian for the radial and tangential distortion camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCameraDistortionJacobian2x4(const double testDuration);

		/**
		 * Tests the Jacobian for the intrinsic and radial distortion camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCameraJacobian2x6(const double testDuration);

		/**
		 * Tests the Jacobian for the intrinsic and radial and tangential distortion camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCameraJacobian2x7(const double testDuration);

		/**
		 * Tests the Jacobian for the intrinsic and radial and tangential distortion camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCameraJacobian2x8(const double testDuration);

		/**
		 * Tests the Jacobian for the rotational part of the extrinsic camera matrix, intrinsic and radial and tangential camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testOrientationCameraJacobian2x11(const double testDuration);

		/**
		 * Tests the Jacobian for the extrinsic, intrinsic and radial  camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPoseCameraJacobian2x12(const double testDuration);

		/**
		 * Tests the Jacobian for the extrinsic, intrinsic and radial and tangential distortion camera parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPoseCameraJacobian2x14(const double testDuration);

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
		 * Determines the derivative for a given 2D position and compares the accuracy with the precise derivatives.
		 * @param original The original position
		 * @param offset The offset position (epsilon offset)
		 * @param eps Used epsilon
		 * @param derivativeX Precise derivative
		 * @param derivativeY Precise derivative
		 * @return True, if so
		 * @tparam T The data type of the scalar to be used, 'float' or 'double'
		 */
		template <typename T>
		inline static bool checkAccuracy(const VectorT2<T>& original, const VectorT2<T>& offset, const T eps, const T derivativeX, const T derivativeY);

		/**
		 * Determines the derivative for a given 3D position and compares the accuracy with the precise derivatives.
		 * @param original The original position
		 * @param offset The offset position (epsilon offset)
		 * @param eps Used epsilon
		 * @param derivativeX Precise derivative
		 * @param derivativeY Precise derivative
		 * @param derivativeZ Precise derivative
		 * @return True, if so
		 * @tparam T The data type of the scalar to be used, 'float' or 'double'
		 */
		template <typename T>
		inline static bool checkAccuracy(const VectorT3<T>& original, const VectorT3<T>& offset, const T eps, const T derivativeX, const T derivativeY, const T derivativeZ);

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
		 * @tparam T The data type of the scalar for which the threshold will be returned
		 */
		template <typename T>
		static constexpr double successThreshold();
};

template <typename T>
bool TestJacobian::checkAccuracy(const VectorT2<T>& original, const VectorT2<T>& offset, const T eps, const T derivativeX, const T derivativeY)
{
	ocean_assert(eps > NumericT<T>::eps());

	// approximation of the derivative:
	// f'(x) = [f(x + e) - f(x)] / e

	const T calculatedDerivativeX = (offset.x() - original.x()) / eps;
	const T calculatedDerivativeY = (offset.y() - original.y()) / eps;

	const T diffX = NumericT<T>::abs(derivativeX - calculatedDerivativeX);

	const T maxX = max(NumericT<T>::abs(derivativeX), NumericT<T>::abs(calculatedDerivativeX));

	if ((derivativeX != 0 && calculatedDerivativeX != 0 && NumericT<T>::isNotEqualEps(maxX) && diffX / maxX > T(0.05)) || ((derivativeX == 0 || calculatedDerivativeX == 0) && NumericT<T>::abs(diffX) > T(0.001)))
	{
		return false;
	}

	const T diffY = NumericT<T>::abs(derivativeY - calculatedDerivativeY);

	const T maxY = max(NumericT<T>::abs(derivativeY), NumericT<T>::abs(calculatedDerivativeY));

	if ((derivativeY != 0 && calculatedDerivativeY != 0 && NumericT<T>::isNotEqualEps(maxY) && diffY / maxY > T(0.05)) || ((derivativeY == 0 || calculatedDerivativeY == 0) && NumericT<T>::abs(diffY) > T(0.001)))
	{
		return false;
	}

	return true;
}

template <typename T>
inline bool TestJacobian::checkAccuracy(const VectorT3<T>& original, const VectorT3<T>& offset, const T eps, const T derivativeX, const T derivativeY, const T derivativeZ)
{
	ocean_assert(eps > NumericT<T>::eps());

	// approximation of the derivative:
	// f'(x) = [f(x + e) - f(x)] / e

	const T calculatedDerivativeX = (offset.x() - original.x()) / eps;
	const T calculatedDerivativeY = (offset.y() - original.y()) / eps;
	const T calculatedDerivativeZ = (offset.z() - original.z()) / eps;

	const T diffX = NumericT<T>::abs(derivativeX - calculatedDerivativeX);

	const T maxX = max(NumericT<T>::abs(derivativeX), NumericT<T>::abs(calculatedDerivativeX));

	if ((derivativeX != 0 && NumericT<T>::isNotEqualEps(maxX) && diffX / maxX > T(0.05)) || (derivativeX == 0 && NumericT<T>::abs(diffX) > T(0.001)))
	{
		return false;
	}

	const T diffY = NumericT<T>::abs(derivativeY - calculatedDerivativeY);

	const T maxY = max(NumericT<T>::abs(derivativeY), NumericT<T>::abs(calculatedDerivativeY));

	if ((derivativeY != 0 && NumericT<T>::isNotEqualEps(maxY) && diffY / maxY > T(0.05)) || (derivativeY == 0 && NumericT<T>::abs(diffY) > T(0.001)))
	{
		return false;
	}

	const T diffZ = NumericT<T>::abs(derivativeZ - calculatedDerivativeZ);

	const T maxZ = max(NumericT<T>::abs(derivativeZ), NumericT<T>::abs(calculatedDerivativeZ));

	if ((derivativeZ != 0 && NumericT<T>::isNotEqualEps(maxZ) && diffZ / maxZ > T(0.05)) || (derivativeZ == 0 && NumericT<T>::abs(diffZ) > T(0.001)))
	{
		return false;
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

template <typename T>
constexpr double TestJacobian::successThreshold()
{
	if constexpr (std::is_same<T, float>::value)
	{
		return 0.1; // very generous threshold due to precision issues
	}
	else
	{
		return 0.99;
	}
}

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_JACOBIAN_H
