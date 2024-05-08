/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_CAMERA_H
#define META_OCEAN_MATH_CAMERA_H

#include "ocean/math/Math.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class CameraT;

/**
 * Definition of an Camera object with Scalar precision.
 * @see CameraT
 * @ingroup math
 */
typedef CameraT<Scalar> Camera;

/**
 * Definition of an Camera object with double precision.
 * @see CameraT
 * @ingroup math
 */
typedef CameraT<double> CameraD;

/**
 * Definition of an Camera object with float precision.
 * @see CameraT
 * @ingroup math
 */
typedef CameraT<float> CameraF;

/**
 * This class implements the base class for all cameras.
 * The base class provides model independent functionalities.<br>
 * Use AnyCamera in case an entire camera model is needed.
 * @tparam T The data type of a scalar, 'float' or 'double'
 * @ingroup math
 */
template <typename T>
class CameraT
{
	public:

		/**
		 * Calculates the vertical FOV from the horizontal FOV and the aspect ratio of the camera image.
		 * @param fovX The given field of view in x direction (in radian), with range (0, PI)
		 * @param aspectRatio The aspect ratio of the camera (width / height)
		 * @return The field of view in y direction (in radian)
		 */
		static T fovX2Y(const T fovX, const T aspectRatio);

		/**
		 * Calculates the horizontal FOV from the vertical FOV and the aspect ratio of the camera image.
		 * @param fovY The given field of view in y direction (in radian), with range (0, PI)
		 * @param aspectRatio The aspect ratio of the camera (width / height)
		 * @return The field of view in x direction (in radian)
		 */
		static T fovY2X(const T fovY, const T aspectRatio);

		/**
		 * Converts field of view (and width) to the corresponding focal length.
		 * @param width The width of the camera in pixel, with range [1, infinity)
		 * @param fovX The horizontal field of view of the camera, in radian, with range (0, PI)
		 * @return The focal length of the camera, with range (0, infinity)
		 */
		static T fieldOfViewToFocalLength(const unsigned int width, const T fovX);

		/**
		 * Calculates the normalized image point (the normalized projected object point) for a of given object point with corresponding extrinsic camera matrix.
		 * The extrinsic matrix transforms a 3D point given in camera coordinates into 3D world coordinates.<br>
		 * The viewing direction of the camera is along the negative z-axis.<br>
		 * The extrinsic matrix will be flipped and inverted internally.
		 * @param extrinsic The extrinsic camera matrix
		 * @param objectPoint The 3D object point for that the normalized image point will be calculated
		 * @return The resulting normalized image point
		 * @see objectPoints2normalizedImagePoints().
		 */
		static inline VectorT2<T> objectPoint2normalizedImagePoint(const HomogenousMatrixT4<T>& extrinsic, const VectorT3<T>& objectPoint);

		/**
		 * Calculates the normalized image point (the normalized projected object point) for a given object point with corresponding inverse and flipped extrinsic camera matrix.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.
		 * @param iFlippedExtrinsic The inverted and flipped extrinsic camera matrix
		 * @param objectPoint The 3D object point for that the normalized image point will be calculated
		 * @return The resulting normalized image point
		 * @see objectPoint2normalizedImagePoint().
		 */
		static inline VectorT2<T> objectPoint2normalizedImagePointIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const VectorT3<T>& objectPoint);

		/**
		 * Calculates the normalized image points (the normalized projected object points) for a set of given object points with corresponding extrinsic camera matrix.
		 * The extrinsic matrix transforms a 3D point given in camera coordinates into 3D world coordinates.<br>
		 * The viewing direction of the camera is along the negative z-axis.<br>
		 * The extrinsic matrix will be flipped and inverted internally.
		 * @param extrinsic The extrinsic camera matrix
		 * @param objectPoints The set of 3D object points for that the normalized image points will be calculated
		 * @param numberObjectPoints The number of object points to project
		 * @param normalizedImagePoints The resulting normalized image points, make sure that enough memory is provided
		 * @see objectPoint2normalizedImagePoint(), objectPoints2normalizedImagePointsIF().
		 */
		static inline void objectPoints2normalizedImagePoints(const HomogenousMatrixT4<T>& extrinsic, const VectorT3<T>* objectPoints, const size_t numberObjectPoints, VectorT2<T>* normalizedImagePoints);

		/**
		 * Calculates the normalized image points (the normalized projected object points) for a set of given object points with corresponding inverse and flipped extrinsic camera matrix.
		 * The inverse extrinsic matrix transforms a 3D point given in world coordinates into 3D camera coordinates.<br>
		 * The coordinate system of the camera is flipped meaning that the viewing direction is along the positive z-axis.<br>
		 * The flipped coordinate system can be received by a rotation around the x-axis by 180 degree.
		 * @param iFlippedExtrinsic The inverted and flipped extrinsic camera matrix
		 * @param objectPoints The set of 3D object points for that the normalized image points will be calculated
		 * @param numberObjectPoints The number of object points to project
		 * @param normalizedImagePoints The resulting normalized image points, make sure that enough memory is provided
		 * @see objectPoints2normalizedImagePoints().
		 */
		static void objectPoints2normalizedImagePointsIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const VectorT3<T>* objectPoints, const size_t numberObjectPoints, VectorT2<T>* normalizedImagePoints);

		/**
		 * Returns the 3x3 transformation matrix flipping a transformation around the x-axis by 180 deg.
		 * The matrix is defined by an angle-axis rotation: Rotation(1, 0, 0, PI).
		 * @return The transformation matrix
		 * @tparam U The data type of the elements of the matrix
		 * @see flipMatrix4(), flipQuaternion().
		 */
		template <typename U = T>
		static inline SquareMatrixT3<U> flipMatrix3();

		/**
		 * Returns the 4x4 transformation matrix flipping a transformation around the x-axis by 180 deg.
		 * The matrix is defined by an angle-axis rotation: Rotation(1, 0, 0, PI).
		 * @return The transformation matrix
		 * @tparam U The data type of the elements of the matrix
		 * @see flipMatrix3(), flipQuaternion().
		 */
		template <typename U = T>
		static inline HomogenousMatrixT4<U> flipMatrix4();

		/**
		 * Returns the quaternion flipping a rotation around the x-axis by 180 deg.
		 * The quaternion is defined by an angle-axis rotation: Rotation(1, 0, 0, PI).
		 * @return The transformation quaternion
		 * @tparam U The data type of the elements of the quaternion
		 * @see flipMatrix3(), flipMatrix4().
		 */
		template <typename U = T>
		static inline QuaternionT<U> flipQuaternion();

		/**
		 * Flips a transformation matrix around the x-axis by 180 degree.
		 * The flip transformation is applied at the left side of the given matrix.
		 * @param left_T_right The transformation matrix to flip, must be valid
		 * @return The flipped transformation matrix, will be flippedLeft_T_right
		 * @tparam U The data type of the elements of the matrix
		 */
		template <typename U>
		static inline HomogenousMatrixT4<U> flippedTransformationLeftSide(const HomogenousMatrixT4<U>& left_T_right);

		/**
		 * Flips a transformation matrix around the x-axis by 180 degree.
		 * The flip transformation is applied at the right side of the given matrix.
		 * @param left_T_right The transformation matrix to flip, must be valid
		 * @return The flipped transformation matrix, will be left_T_flippedRight
		 * @tparam U The data type of the elements of the matrix
		 */
		template <typename U>
		static inline HomogenousMatrixT4<U> flippedTransformationRightSide(const HomogenousMatrixT4<U>& left_T_right);

		/**
		 * Flips a transformation matrix around the x-axis by 180 degree.
		 * The flip transformation is applied at the left and right side of the original matrix.
		 * @param left_T_right The transformation matrix to flip, must be valid
		 * @return The flipped transformation matrix, will be flippedLeft_T_flippedRight
		 * @tparam U The data type of the elements of the matrix
		 */
		template <typename U>
		static inline HomogenousMatrixT4<U> flippedTransformationLeftAndRightSide(const HomogenousMatrixT4<U>& left_T_right);

		/**
		 * Flips a 3x3 rotation matrix around the x-axis by 180 degree.
		 * The flip transformation is applied at the left side of the original rotation.
		 * @param left_R_right The rotation matrix to flip, must be valid
		 * @return The flipped rotation, will be flippedLeft_R_right
		 * @tparam U The data type of the elements of the rotation
		 */
		template <typename U>
		static inline SquareMatrixT3<U> flippedTransformationLeftSide(const SquareMatrixT3<U>& left_R_right);

		/**
		 * Flips a 3x3 rotation matrix around the x-axis by 180 degree.
		 * The flip transformation is applied at the right side of the original rotation.
		 * @param left_R_right The rotation matrix to flip, must be valid
		 * @return The flipped rotation, will be left_R_flippedRight
		 * @tparam U The data type of the elements of the rotation
		 */
		template <typename U>
		static inline SquareMatrixT3<U> flippedTransformationRightSide(const SquareMatrixT3<U>& left_R_right);

		/**
		 * Flips a 3x3 rotation matrix around the x-axis by 180 degree.
		 * The flip transformation is applied at the left and right side of the original rotation.
		 * @param left_R_right The rotation matrix to flip, must be valid
		 * @return The flipped rotation, will be flippedLeft_R_flippedRight
		 * @tparam U The data type of the elements of the rotation
		 */
		template <typename U>
		static inline SquareMatrixT3<U> flippedTransformationLeftAndRightSide(const SquareMatrixT3<U>& left_R_right);

		/**
		 * Flips a quaternion around the x-axis by 180 degree.
		 * The flip transformation is applied at the left side of the original rotation.
		 * @param left_Q_right The quaternion to flip, must be valid
		 * @return The flipped quaternion, will be flippedLeft_Q_right
		 * @tparam U The data type of the elements of the quaternion
		 */
		template <typename U>
		static inline QuaternionT<U> flippedTransformationLeftSide(const QuaternionT<U>& left_Q_right);

		/**
		 * Flips a quaternion around the x-axis by 180 degree.
		 * The flip transformation is applied at the right side of the original rotation.
		 * @param left_Q_right The quaternion to flip, must be valid
		 * @return The flipped quaternion, will be left_Q_flippedRight
		 * @tparam U The data type of the elements of the quaternion
		 */
		template <typename U>
		static inline QuaternionT<U> flippedTransformationRightSide(const QuaternionT<U>& left_Q_right);

		/**
		 * Flips a quaternion around the x-axis by 180 degree.
		 * The flip transformation is applied at the left and right side of the original rotation.
		 * @param left_Q_right The quaternion to flip, must be valid
		 * @return The flipped quaternion, will be flippedLeft_Q_flippedRight
		 * @tparam U The data type of the elements of the quaternion
		 */
		template <typename U>
		static inline QuaternionT<U> flippedTransformationLeftAndRightSide(const QuaternionT<U>& left_Q_right);

		/**
		 * Transforms a standard homogenous 4x4 viewing (extrinsic camera) matrix into an inverted and flipped camera pose.
		 * The standard matrix defines a coordinate system with negative Z-axis as viewing direction in relation to the world coordinate system.<br>
		 * The inverted and flipped camera pose defines a coordinate system with positive Z-axis as viewing direction and transforms the world in relation to the camera coordinate system.
		 * @param world_T_camera The standard camera pose to transform, must be valid
		 * @return The inverted and flipped camera pose, flippedCamera_T_world
		 * @tparam U The data type of the elements of the matrix
		 * @see invertedFlipped2Standard().
		 */
		template <typename U>
		static HomogenousMatrixT4<U> standard2InvertedFlipped(const HomogenousMatrixT4<U>& world_T_camera);

		/**
		 * Transforms standard homogenous 4x4 viewing (extrinsic camera) matrices into an inverted and flipped camera matrices.
		 * The standard matrix defines a coordinate system with negative Z-axis as viewing direction in relation to the world coordinate system.<br>
		 * The inverted and flipped camera pose defines a coordinate system with positive Z-axis as viewing direction and transforms the world in relation to the camera coordinate system.
		 * @param world_T_cameras The standard camera poses to transform, can be nullptr if number is 0u
		 * @param number The number of provided poses, with range [0, infinity)
		 * @return The inverted and flipped camera poses, flippedCameras_T_world
		 * @tparam U The data type of the elements of the matrix
		 * @see invertedFlipped2Standard().
		 */
		template <typename U>
		static inline HomogenousMatricesT4<U> standard2InvertedFlipped(const HomogenousMatrixT4<U>* world_T_cameras, const size_t number);

		/**
		 * Transforms standard homogenous 4x4 viewing (extrinsic camera) matrices into an inverted and flipped camera matrices.
		 * The standard matrix defines a coordinate system with negative Z-axis as viewing direction in relation to the world coordinate system.<br>
		 * The inverted and flipped camera pose defines a coordinate system with positive Z-axis as viewing direction and transforms the world in relation to the camera coordinate system.
		 * @param world_T_cameras The standard camera poses to transform, can be nullptr if number is 0u
		 * @param flippedCameras_T_world The resulting inverted and flipped camera poses
		 * @param number The number of provided poses, with range [0, infinity)
		 * @tparam U The data type of the elements of the matrix
		 * @see invertedFlipped2Standard().
		 */
		template <typename U>
		static inline void standard2InvertedFlipped(const HomogenousMatrixT4<U>* world_T_cameras, HomogenousMatrixT4<U>* flippedCameras_T_world, const size_t number);

		/**
		 * Transforms standard homogenous 4x4 viewing (extrinsic camera) matrices into an inverted and flipped camera matrices.
		 * The standard matrix defines a coordinate system with negative Z-axis as viewing direction in relation to the world coordinate system.<br>
		 * The inverted and flipped camera pose defines a coordinate system with positive Z-axis as viewing direction and transforms the world in relation to the camera coordinate system.
		 * @param world_T_cameras The standard camera poses to transform
		 * @return The inverted and flipped camera poses, flippedCameras_T_world
		 * @tparam U The data type of the elements of the matrix
		 * @see invertedFlipped2Standard().
		 */
		template <typename U>
		static inline HomogenousMatricesT4<U> standard2InvertedFlipped(const HomogenousMatricesT4<U>& world_T_cameras);

		/**
		 * Transforms a standard 3x3 rotation matrix into an inverted and flipped rotation matrix.
		 * The standard rotation defines a coordinate system with negative Z-axis as viewing direction in relation to the world coordinate system.<br>
		 * The inverted and flipped rotation matrix defines a coordinate system with positive Z-axis as viewing direction and transforms the world in relation to the camera coordinate system.
		 * @param world_R_camera The standard rotation matrix of the camera pose to transform, must be valid
		 * @return The inverted and flipped rotation matrix of camera pose, flippedCamera_R_world
		 * @tparam U The data type of the elements of the matrix
		 * @see invertedFlipped2Standard().
		 */
		template <typename U>
		static SquareMatrixT3<U> standard2InvertedFlipped(const SquareMatrixT3<U>& world_R_camera);

		/**
		 * Transforms a standard rotation quaternion into an inverted and flipped rotation quaternion.
		 * The standard rotation defines a coordinate system with negative Z-axis as viewing direction in relation to the world coordinate system.<br>
		 * The inverted and flipped rotation quaternion defines a coordinate system with positive Z-axis as viewing direction and transforms the world in relation to the camera coordinate system.
		 * @param world_Q_camera The standard rotation quaternion of the camera pose to transform, must be valid
		 * @return The inverted and flipped rotation quaternion of camera pose, flippedCamera_Q_world
		 * @tparam U The data type of the elements of the quaternion
		 * @see invertedFlipped2Standard().
		 */
		template <typename U>
		static QuaternionT<U> standard2InvertedFlipped(const QuaternionT<U>& world_Q_camera);

		/**
		 * Transforms an inverted and flipped camera pose into a standard camera pose.
		 * @param flippedCamera_T_world The inverted and flipped camera pose to transform, must be valid
		 * @return The standard camera pose, world_T_camera
		 * @tparam U The data type of the elements of the matrix
		 * @see standard2InvertedFlipped().
		 */
		template <typename U>
		static inline HomogenousMatrixT4<U> invertedFlipped2Standard(const HomogenousMatrixT4<U>& flippedCamera_T_world);

		/**
		 * Transforms inverted and flipped camera matrices into standard viewing (extrinsic camera) matrices.
		 * @param flippedCameras_T_world The inverted and flipped camera poses to transform, can be nullptr if number == 0
		 * @param number The number of poses, with range [0, infinity)
		 * @return The standard viewing poses, world_T_cameras
		 * @tparam U The data type of the elements of the matrix
		 * @see standard2InvertedFlipped().
		 */
		template <typename U>
		static inline HomogenousMatricesT4<U> invertedFlipped2Standard(const HomogenousMatrixT4<U>* flippedCameras_T_world, const size_t number);

		/**
		 * Transforms inverted and flipped camera matrices into standard viewing (extrinsic camera) matrices.
		 * @param flippedCameras_T_world The inverted and flipped camera poses to transform, can be nullptr if number == 0
		 * @param world_T_cameras The resulting standard camera poses, can be nullptr if number == 0
		 * @param number The number of poses, with range [0, infinity)
		 * @tparam U The data type of the elements of the matrix
		 * @see standard2InvertedFlipped().
		 */
		template <typename U>
		static inline void invertedFlipped2Standard(const HomogenousMatrixT4<U>* flippedCameras_T_world, HomogenousMatrixT4<U>* world_T_cameras, const size_t number);

		/**
		 * Transforms inverted and flipped camera matrices into standard viewing (extrinsic camera) matrices.
		 * @param flippedCameras_T_world The inverted and flipped camera poses to transform
		 * @return The standard camera poses, world_T_cameras
		 * @tparam U The data type of the elements of the matrix
		 * @see standard2InvertedFlipped().
		 */
		template <typename U>
		static inline HomogenousMatricesT4<U> invertedFlipped2Standard(const HomogenousMatricesT4<U>& flippedCameras_T_world);

		/**
		 * Transforms an inverted and flipped rotation matrix into a standard viewing rotation matrix.
		 * @param flippedCamera_R_world The inverted and flipped rotation matrix of camera to transform, must be valid
		 * @return The rotation matrix of standard camera pose, world_R_camera
		 * @tparam U The data type of the elements of the matrix
		 * @see standard2InvertedFlipped().
		 */
		template <typename U>
		static inline SquareMatrixT3<U> invertedFlipped2Standard(const SquareMatrixT3<U>& flippedCamera_R_world);

		/**
		 * Transforms an inverted and flipped rotation quaternion into a standard viewing rotation quaternion.
		 * @param flippedCamera_Q_world The inverted and flipped rotation quaternion of camera pose to transform
		 * @return The rotation quaternion of standard camera pose, world_Q_camera
		 * @tparam U The data type of the elements of the quaternion
		 * @see standard2InvertedFlipped().
		 */
		template <typename U>
		static inline QuaternionT<U> invertedFlipped2Standard(const QuaternionT<U>& flippedCamera_Q_world);

		/**
		 * Determines whether a given 3D object point lies in front of a camera while the location of the camera is defined by a 6-DOF pose.
		 * This function actually determined whether (iFlippedPose * objectPoint).z() > epsilon.
		 * @param flippedCamera_T_world The inverted and flipped pose of the camera, must be valid
		 * @param objectPoint The object point to check
		 * @param epsilon The minimal distance between camera and object point on the z-axis so that the object point counts as lying in front, with range [0, infinity)
		 * @return True, if so
		 */
		static inline bool isObjectPointInFrontIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint, const T epsilon = NumericT<T>::eps());

		/**
		 * Determines whether a given 3D object point lies in front of a camera while the location of the camera is defined by a 3-DOF orientation.
		 * This function actually determined whether (iFlippedPose * objectPoint).z() > epsilon.
		 * @param flippedCamera_R_world The inverted and flipped orientation of the camera, must be valid
		 * @param objectPoint The object point to check
		 * @param epsilon The minimal distance between camera and object point on the z-axis so that the object point counts as lying in front, with range [0, infinity)
		 * @return True, if so
		 */
		static inline bool isObjectPointInFrontIF(const SquareMatrixT3<T>& flippedCamera_R_world, const VectorT3<T>& objectPoint, const T epsilon = NumericT<T>::eps());
};

template <typename T>
T CameraT<T>::fovX2Y(const T fovX, const T aspectRatio)
{
	ocean_assert(aspectRatio != 0);

	return 2 * NumericT<T>::atan(NumericT<T>::tan(T(0.5) * fovX) / aspectRatio);
}

template <typename T>
T CameraT<T>::fovY2X(const T fovY, const T aspectRatio)
{
	ocean_assert(aspectRatio != 0);

	return 2 * NumericT<T>::atan(NumericT<T>::tan(T(0.5) * fovY) * aspectRatio);
}

template <typename T>
T CameraT<T>::fieldOfViewToFocalLength(const unsigned int width, const T fovX)
{
	ocean_assert(width > 0u);
	ocean_assert(fovX > NumericT<T>::eps() && fovX < NumericT<T>::pi());

	const T width_2 = T(width) * T(0.5);

	return width_2 / NumericT<T>::tan(fovX * T(0.5));
}

template <typename T>
inline VectorT2<T> CameraT<T>::objectPoint2normalizedImagePoint(const HomogenousMatrixT4<T>& extrinsic, const VectorT3<T>& objectPoint)
{
	ocean_assert(extrinsic.isValid());

	return objectPoint2normalizedImagePointIF(standard2InvertedFlipped(extrinsic), objectPoint);
}

template <typename T>
inline VectorT2<T> CameraT<T>::objectPoint2normalizedImagePointIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const VectorT3<T>& objectPoint)
{
	ocean_assert(iFlippedExtrinsic.isValid());

	const VectorT3<T> transformedObjectPoint(iFlippedExtrinsic * objectPoint);

	ocean_assert(NumericT<T>::isNotEqualEps(transformedObjectPoint.z()));
	if (NumericT<T>::isEqualEps(transformedObjectPoint.z()))
	{
		return VectorT2<T>(0, 0);
	}

	const T factor = 1 / transformedObjectPoint.z();
	return VectorT2<T>(transformedObjectPoint.x() * factor, transformedObjectPoint.y() * factor);
}

template <typename T>
inline void CameraT<T>::objectPoints2normalizedImagePoints(const HomogenousMatrixT4<T>& extrinsic, const VectorT3<T>* objectPoints, const size_t numberObjectPoints, VectorT2<T>* normalizedImagePoints)
{
	ocean_assert(extrinsic.isValid());
	ocean_assert(numberObjectPoints == 0u || (objectPoints && normalizedImagePoints));

	objectPoints2normalizedImagePointsIF(standard2InvertedFlipped(extrinsic), objectPoints, numberObjectPoints, normalizedImagePoints);
}

template <typename T>
void CameraT<T>::objectPoints2normalizedImagePointsIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const VectorT3<T>* objectPoints, const size_t numberObjectPoints, VectorT2<T>* normalizedImagePoints)
{
	ocean_assert(iFlippedExtrinsic.isValid());
	ocean_assert(numberObjectPoints == 0u || (objectPoints && normalizedImagePoints));

	for (unsigned int n = 0u; n < numberObjectPoints; ++n)
	{
		const VectorT3<T> transformedObjectPoint(iFlippedExtrinsic * objectPoints[n]);

		ocean_assert(NumericT<T>::isNotEqualEps(transformedObjectPoint.z()));

		if (NumericT<T>::isNotEqualEps(transformedObjectPoint.z()))
		{
			const T factor = 1 / transformedObjectPoint.z();
			normalizedImagePoints[n].x() = transformedObjectPoint.x() * factor;
			normalizedImagePoints[n].y() = transformedObjectPoint.y() * factor;
		}
		else
		{
			normalizedImagePoints[n].x() = 0;
			normalizedImagePoints[n].y() = 0;
		}
	}
}

template <typename T>
template <typename U>
inline SquareMatrixT3<U> CameraT<T>::flipMatrix3()
{
	ocean_assert(SquareMatrixT3<U>(1, 0, 0, 0, -1, 0, 0, 0, -1) == SquareMatrixT3<U>(RotationT<U>(1, 0, 0, NumericT<U>::pi())));

	return SquareMatrixT3<U>(1, 0, 0, 0, -1, 0, 0, 0, -1);
}

template <typename T>
template <typename U>
inline HomogenousMatrixT4<U> CameraT<T>::flipMatrix4()
{
	const U flipValues[16] =
	{
		1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1
	};

	ocean_assert(HomogenousMatrixT4<U>(flipValues) == HomogenousMatrixT4<U>(RotationT<U>(1, 0, 0, NumericT<U>::pi())));

	return HomogenousMatrixT4<U>(flipValues);
}

template <typename T>
template <typename U>
inline QuaternionT<U> CameraT<T>::flipQuaternion()
{
	ocean_assert(QuaternionT<U>(0, 1, 0, 0) == QuaternionT<U>(VectorT3<U>(1, 0, 0), NumericT<U>::pi()));

	return QuaternionT<U>(0, 1, 0, 0);
}

template <typename T>
template <typename U>
inline HomogenousMatrixT4<U> CameraT<T>::flippedTransformationLeftSide(const HomogenousMatrixT4<U>& left_T_right)
{
	/*
	* | rx1 rx2 rx3 tx |      |  rx1  rx2  rx3  tx |
	* | ry1 ry2 ry3 ty |  ->  | -ry1 -ry2 -ry3 -ty |
	* | rz1 rz2 rz3 tz |      | -rz1 -rz2 -rz3 -tz |
	* |  0   0   0   1 |      |   0    0    0    1 |
	*/

	HomogenousMatrixT4<U> result(left_T_right);

	result[1] = -result[1];
	result[2] = -result[2];

	result[5] = -result[5];
	result[6] = -result[6];

	result[9] = -result[9];
	result[10] = -result[10];

	result[13] = -result[13];
	result[14] = -result[14];

	ocean_assert(result == flipMatrix4<U>() * left_T_right);

	return result;
}

template <typename T>
template <typename U>
inline HomogenousMatrixT4<U> CameraT<T>::flippedTransformationRightSide(const HomogenousMatrixT4<U>& left_T_right)
{
	/*
	 * | rx1 rx2 rx3 tx |      | rx1 -rx2 -rx3 tx |
	 * | ry1 ry2 ry3 ty |  ->  | ry1 -ry2 -ry3 ty |
	 * | rz1 rz2 rz3 tz |      | rz1 -rz2 -rz3 tz |
	 * |  0   0   0   1 |      |  0    0    0   1 |
	 */

	HomogenousMatrixT4<U> result(left_T_right);

	result[4] = -result[4];
	result[5] = -result[5];
	result[6] = -result[6];

	result[8] = -result[8];
	result[9] = -result[9];
	result[10] = -result[10];

	ocean_assert(result == left_T_right * flipMatrix4<U>());

	return result;
}

template <typename T>
template <typename U>
inline HomogenousMatrixT4<U> CameraT<T>::flippedTransformationLeftAndRightSide(const HomogenousMatrixT4<U>& left_T_right)
{
	/*
	* | rx1 rx2 rx3 tx |      |  rx1 -rx2 -rx3  tx |
	* | ry1 ry2 ry3 ty |  ->  | -ry1  ry2  ry3 -ty |
	* | rz1 rz2 rz3 tz |      | -rz1  rz2  rz3 -tz |
	* |  0   0   0   1 |      |   0    0    0    1 |
	*/

	HomogenousMatrixT4<U> result(left_T_right);

	result[1] = -result[1];
	result[2] = -result[2];

	result[4] = -result[4];

	result[8] = -result[8];

	result[13] = -result[13];
	result[14] = -result[14];

	ocean_assert(result == flipMatrix4<U>() * left_T_right * flipMatrix4<U>());

	return result;
}

template <typename T>
template <typename U>
inline SquareMatrixT3<U> CameraT<T>::flippedTransformationLeftSide(const SquareMatrixT3<U>& left_R_right)
{
	/*
	* | rx1 rx2 rx3 |      |  rx1  rx2  rx3 |
	* | ry1 ry2 ry3 |  ->  | -ry1 -ry2 -ry3 |
	* | rz1 rz2 rz3 |      | -rz1 -rz2 -rz3 |
	*/

	SquareMatrixT3<U> result(left_R_right);

	result[1] = -result[1];
	result[2] = -result[2];

	result[4] = -result[4];
	result[5] = -result[5];

	result[7] = -result[7];
	result[8] = -result[8];

	ocean_assert(result == left_R_right * flipMatrix3<U>());

	return result;
}

template <typename T>
template <typename U>
inline SquareMatrixT3<U> CameraT<T>::flippedTransformationRightSide(const SquareMatrixT3<U>& left_R_right)
{
	/*
	* | rx1 rx2 rx3 |      | rx1 -rx2 -rx3 |
	* | ry1 ry2 ry3 |  ->  | ry1 -ry2 -ry3 |
	* | rz1 rz2 rz3 |      | rz1 -rz2 -rz3 |
	*/

	ocean_assert(SquareMatrixT3<U>(left_R_right.xAxis(), -left_R_right.yAxis(), -left_R_right.zAxis()) == left_R_right * flipMatrix3<U>());

	return SquareMatrixT3<U>(left_R_right.xAxis(), -left_R_right.yAxis(), -left_R_right.zAxis());
}

template <typename T>
template <typename U>
inline SquareMatrixT3<U> CameraT<T>::flippedTransformationLeftAndRightSide(const SquareMatrixT3<U>& left_R_right)
{
	/*
	* | rx1 rx2 rx3 |      |  rx1 -rx2 -rx3 |
	* | ry1 ry2 ry3 |  ->  | -ry1  ry2  ry3 |
	* | rz1 rz2 rz3 |      | -rz1  rz2  rz3 |
	*/

	SquareMatrixT3<U> result(left_R_right);

	result[1] = -result[1];
	result[2] = -result[2];

	result[3] = -result[4];

	result[6] = -result[5];

	ocean_assert(result == flipMatrix3<U>() * left_R_right * flipMatrix3<U>());

	return result;
}

template <typename T>
template <typename U>
inline QuaternionT<U> CameraT<T>::flippedTransformationLeftSide(const QuaternionT<U>& left_Q_right)
{
	return flipQuaternion<U>() * left_Q_right;
}

template <typename T>
template <typename U>
inline QuaternionT<U> CameraT<T>::flippedTransformationRightSide(const QuaternionT<U>& left_Q_right)
{
	return left_Q_right * flipQuaternion<U>();
}

template <typename T>
template <typename U>
inline QuaternionT<U> CameraT<T>::flippedTransformationLeftAndRightSide(const QuaternionT<U>& left_Q_right)
{
	return flipQuaternion<U>() * left_Q_right * flipQuaternion<U>();
}

template <typename T>
template <typename U>
inline HomogenousMatricesT4<U> CameraT<T>::standard2InvertedFlipped(const HomogenousMatrixT4<U>* world_T_cameras, const size_t number)
{
	HomogenousMatricesT4<U> flippedCameras_T_world;
	flippedCameras_T_world.reserve(number);

	for (size_t n = 0; n < number; ++n)
	{
		flippedCameras_T_world.emplace_back(standard2InvertedFlipped<U>(world_T_cameras[n]));
	}

	return flippedCameras_T_world;
}

template <typename T>
template <typename U>
inline void CameraT<T>::standard2InvertedFlipped(const HomogenousMatrixT4<U>* world_T_cameras, HomogenousMatrixT4<U>* flippedCameras_T_world, const size_t number)
{
	for (size_t n = 0; n < number; ++n)
	{
		flippedCameras_T_world[n] = standard2InvertedFlipped<U>(world_T_cameras[n]);
	}
}

template <typename T>
template <typename U>
inline HomogenousMatricesT4<U> CameraT<T>::standard2InvertedFlipped(const HomogenousMatricesT4<U>& world_T_cameras)
{
	return standard2InvertedFlipped<U>(world_T_cameras.data(), world_T_cameras.size());
}

template <typename T>
template <typename U>
inline HomogenousMatrixT4<U> CameraT<T>::standard2InvertedFlipped(const HomogenousMatrixT4<U>& world_T_camera)
{
	ocean_assert(world_T_camera.isValid());

	return flippedTransformationRightSide<U>(world_T_camera).inverted();
}

template <typename T>
template <typename U>
inline SquareMatrixT3<U> CameraT<T>::standard2InvertedFlipped(const SquareMatrixT3<U>& world_R_camera)
{
	return flippedTransformationRightSide<U>(world_R_camera).inverted();
}

template <typename T>
template <typename U>
inline QuaternionT<U> CameraT<T>::standard2InvertedFlipped(const QuaternionT<U>& world_Q_camera)
{
	return flippedTransformationRightSide<U>(world_Q_camera).inverted();
}

template <typename T>
template <typename U>
inline HomogenousMatrixT4<U> CameraT<T>::invertedFlipped2Standard(const HomogenousMatrixT4<U>& flippedCamera_T_world)
{
	ocean_assert(flippedCamera_T_world.isValid());

	return flippedTransformationRightSide<U>(flippedCamera_T_world.inverted());
}

template <typename T>
template <typename U>
inline SquareMatrixT3<U> CameraT<T>::invertedFlipped2Standard(const SquareMatrixT3<U>& flippedCamera_R_world)
{
	return flippedTransformationRightSide<U>(flippedCamera_R_world.inverted());
}

template <typename T>
template <typename U>
inline QuaternionT<U> CameraT<T>::invertedFlipped2Standard(const QuaternionT<U>& flippedCamera_Q_world)
{
	return flippedTransformationRightSide<U>(flippedCamera_Q_world.inverted());
}

template <typename T>
template <typename U>
inline HomogenousMatricesT4<U> CameraT<T>::invertedFlipped2Standard(const HomogenousMatrixT4<U>* flippedCameras_T_world, const size_t number)
{
	HomogenousMatricesT4<U> cameras_T_world;
	cameras_T_world.reserve(number);

	for (size_t n = 0; n < number; ++n)
	{
		cameras_T_world.emplace_back(invertedFlipped2Standard<U>(flippedCameras_T_world[n]));
	}

	return cameras_T_world;
}

template <typename T>
template <typename U>
inline void CameraT<T>::invertedFlipped2Standard(const HomogenousMatrixT4<U>* flippedCameras_T_world, HomogenousMatrixT4<U>* world_T_camera, const size_t number)
{
	for (size_t n = 0; n < number; ++n)
	{
		world_T_camera[n] = invertedFlipped2Standard<U>(flippedCameras_T_world[n]);
	}
}

template <typename T>
template <typename U>
inline HomogenousMatricesT4<U> CameraT<T>::invertedFlipped2Standard(const HomogenousMatricesT4<U>& flippedCameras_T_world)
{
	return invertedFlipped2Standard<U>(flippedCameras_T_world.data(), flippedCameras_T_world.size());
}

template <typename T>
inline bool CameraT<T>::isObjectPointInFrontIF(const HomogenousMatrixT4<T>& flippedCamera_T_world, const VectorT3<T>& objectPoint, const T epsilon)
{
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(epsilon >= 0);

	// the inverted flipped pose looks towards the positive z-axis, so that object points lying in front of the camera must have a positive z-value

	ocean_assert((flippedCamera_T_world[2] * objectPoint.x() + flippedCamera_T_world[6] * objectPoint.y() + flippedCamera_T_world[10] * objectPoint.z() + flippedCamera_T_world[14] > epsilon) == ((flippedCamera_T_world * objectPoint).z() > epsilon));
	return flippedCamera_T_world[2] * objectPoint.x() + flippedCamera_T_world[6] * objectPoint.y() + flippedCamera_T_world[10] * objectPoint.z() + flippedCamera_T_world[14] > epsilon;
}

template <typename T>
inline bool CameraT<T>::isObjectPointInFrontIF(const SquareMatrixT3<T>& flippedCamera_R_world, const VectorT3<T>& objectPoint, const T epsilon)
{
	ocean_assert(!flippedCamera_R_world.isNull());
	ocean_assert(epsilon >= 0);

	// the inverted flipped orientation looks towards the positive z-axis, so that object points lying in front of the camera must have a positive z-value

	ocean_assert((flippedCamera_R_world[2] * objectPoint.x() + flippedCamera_R_world[5] * objectPoint.y() + flippedCamera_R_world[8] * objectPoint.z() > epsilon) == ((flippedCamera_R_world * objectPoint).z() > epsilon));
	return flippedCamera_R_world[2] * objectPoint.x() + flippedCamera_R_world[5] * objectPoint.y() + flippedCamera_R_world[8] * objectPoint.z() > epsilon;
}

}

#endif // META_OCEAN_MATH_CAMERA_H
