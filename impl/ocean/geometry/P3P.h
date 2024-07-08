/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_P3P_H
#define META_OCEAN_GEOMETRY_P3P_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/PerspectivePose.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class provides functions determining the camera's pose by a set of three 3D object and 2D image point correspondences.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT P3P : public PerspectivePose
{
	public:

		/**
		 * Deprecated.
		 *
		 * Calculates the possible camera poses for three correspondences between 3D object points and 2D image points.
		 * The 3D object points as well as the resulting camera poses are defined in relation to a common world coordinate system.<br>
		 * Each pose is defined using a default camera pointing into the negative z-space of the coordinate system, with x-axis to the right of the camera frame, and y-axis pointing upwards (this coordinate system is often used in Computer Graphics).<br>
		 * The resulting poses can be transformed to an inverted flipped coordinate system e.g., by using PinholeCamera::standard2InvertedFlipped().<br>
		 * An inverted and flipped pose is pointing into the positive z-space of the coordinate system, with x-axis to the right of the camera frame, and y-axis pointing downwards (this coordinate system is often used in Computer Vision).<br>
		 * The provided image points should be defined in the domain of a normal image (with origin in the upper left corner, x pointing to the right, y pointing downwards).<br>
		 * The resulting pose is based on the following equation:
		 * <pre>
		 * imagePoint = K * PinholeCamera::standard2InvertedFlipped(cameraPoses_world_T_camera[n]) * objectPoint
		 * imagePoint = K * rotate_around_x(cameraPoses_world_T_camera[n], PI)^-1 * objectPoint
		 * </pre>
		 * The p3p can result in at most four different poses due to the under-determined system of equations.<br>
		 * The image points should be undistorted to improve the pose quality.
		 * @param pinholeCamera The pinhole camera profile defining the projection, must be valid
		 * @param objectPoints Three 3D objects points each corresponding to a different 2D image point, the points must not be collinear
		 * @param imagePoints Three 2D image points each corresponding to a different 3D object point
		 * @param world_T_cameras The resulting transformation matrices receiving the different poses, the buffer must be large enough to store up to four resulting poses
		 * @param minimalCollinearSqrDistance The minimal collinear square distance between all three points so that the given image points will be accepted, with range [0, infinity)
		 * @return Resulting number of different poses, with range [0, 4]
		 * @see PinholeCamera::standard2InvertedFlipped(), PinholeCamera::invertedFlipped2Standard(), PinholeCamera::undistort().
		 */
		static unsigned int poses(const PinholeCamera& pinholeCamera, const Vector3* objectPoints, const Vector2* imagePoints, HomogenousMatrix4* world_T_cameras, const Scalar minimalCollinearSqrDistance = Scalar(1 * 1));

		/**
		 * Calculates the possible camera poses for three correspondences between 3D object points and 2D image points.
		 * The 3D object points as well as the resulting camera poses are defined in relation to a common world coordinate system.<br>
		 * Each pose is defined using a default camera pointing into the negative z-space of the coordinate system, with x-axis to the right of the camera frame, and y-axis pointing upwards (this coordinate system is often used in Computer Graphics).<br>
		 * The resulting poses can be transformed to an inverted flipped coordinate system e.g., by using PinholeCamera::standard2InvertedFlipped().<br>
		 * An inverted and flipped pose is pointing into the positive z-space of the coordinate system, with x-axis to the right of the camera frame, and y-axis pointing downwards (this coordinate system is often used in Computer Vision).<br>
		 * The provided image points should be defined in the domain of a normal image (with origin in the upper left corner, x pointing to the right, y pointing downwards).<br>
		 * The p3p can result in at most four different poses due to the under-determined system of equations.
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param objectPoints Three 3D objects points each corresponding to a different 2D image point, the points must not be collinear
		 * @param imagePoints Three 2D image points each corresponding to a different 3D object point
		 * @param world_T_cameras The resulting transformation matrices receiving the different poses, the buffer must be large enough to store up to four resulting poses
		 * @return Resulting number of different poses, with range [0, 4]
		 * @tparam TCamera The data type of the camera profile, either 'float' or 'double'
		 * @tparam TPoint The data type image and object points, either 'float' or 'double'
		 * @see PinholeCamera::standard2InvertedFlipped(), PinholeCamera::invertedFlipped2Standard(), PinholeCamera::undistort().
		 */
		template <typename TCamera, typename TPoint>
		static unsigned int poses(const AnyCameraT<TCamera>& anyCamera, const VectorT3<TPoint>* objectPoints, const VectorT2<TPoint>* imagePoints, HomogenousMatrixT4<TPoint>* world_T_cameras);

		/**
		 * Calculates the possible camera poses for three correspondences between 3D object points and 3D rays starting at the camera's center of projection and pointing towards the 3D object points.
		 * The 3D object points are defined in relation to a world coordinate system, while the 3D rays are defined in relation to the coordinate system of the camera.<br>
		 * Each pose is defined using a default camera pointing into the negative z-space of the coordinate system, with x-axis to the right of the camera frame and y-axis pointing upwards.<br>
		 * The resulting poses can be transformed to an inverted flipped coordinate system e.g., by using PinholeCamera::standard2InvertedFlipped().<br>
		 * The p3p can result in at most four different poses due to the under-determined system of equations.<br>
		 * @param objectPoints Three 3D objects points each corresponding to a different 2D image point, the points must not be collinear
		 * @param imageRays Three 3D rays with unit length, defined in the coordinate system of the camera, starting at the camera's center of projection (equal to the origin), hitting the image plane at image points corresponding to the 3D object points and pointing towards the object points, each ray corresponds to one 3D object point
		 * @param cameraPoses The resulting transformation matrices receiving the different poses, the buffer must be large enough to store up to four resulting poses
		 * @return Resulting number of different poses, with range [0, 4]
		 * @tparam T Data type of e.g., the vector elements to be used, either 'float' or 'double'
		 * @see PinholeCamera::standard2InvertedFlipped(), PinholeCamera::invertedFlipped2Standard(), PinholeCamera::undistort().
		 */
		template <typename T>
		static unsigned int poses(const VectorT3<T>* objectPoints, const VectorT3<T>* imageRays, HomogenousMatrixT4<T>* cameraPoses);

	protected:

		/**
		 * Constructs the closest point on the line between two object points and the camera's projection center.
		 * @param objectPoint0 First object point intersecting the line
		 * @param objectPoint1 Second object point intersecting the line
		 * @param objectDistance01 Distance bettween first and second object point
		 * @param objectDistanceToCP0 Distance between first object point and the camera's projection center
		 * @param objectDistanceToCP1 Distance between second object point and the camera's projection center
		 * @return Resulting closest point
		 * @tparam T Data type of e.g., the vector elements to be used, either 'float' or 'double'
		 */
		template <typename T>
		static inline VectorT3<T> constructClosestPointToCP(const VectorT3<T>& objectPoint0, const VectorT3<T>& objectPoint1, const T objectDistance01, const T objectDistanceToCP0, const T objectDistanceToCP1);

		/**
		 * Constructs the closest point to the camera's projection center lying on the object surface triangle.
		 * @param objectPoint0 First object point intersecting the object surface triangle
		 * @param objectPoint1 Second object point intersecting the object surface triangle
		 * @param objectPoint2 Third object point intersecting the object surface triangle
		 * @param closestPoint01 Closest point to the camera's projection center lying on the line between first and second object point
		 * @param closestPoint02 Closest point to the camera's projection center lying on the line between first and third object point
		 * @param point Resulting closes point
		 * @return True, if succeeded
		 * @tparam T Data type of e.g., the vector elements to be used, either 'float' or 'double'
		 */
		template <typename T>
		static inline bool constructClosestPointToCPOnObjectPlane(const VectorT3<T>& objectPoint0, const VectorT3<T>& objectPoint1, const VectorT3<T>& objectPoint2, const VectorT3<T>& closestPoint01, const VectorT3<T>& closestPoint02, VectorT3<T>& point);

		/**
		 * Returns the square of a value.
		 * @param value The value to square
		 * @return The squared value
		 * @tparam T Data type of e.g., the vector elements to be used, either 'float' or 'double'
		 */
		template <typename T>
		static inline T sqr(const T value);
};

template <typename T>
inline VectorT3<T> P3P::constructClosestPointToCP(const VectorT3<T>& objectPoint0, const VectorT3<T>& objectPoint1, const T objectDistance01, const T objectDistanceToCP0, const T objectDistanceToCP1)
{
	ocean_assert(NumericT<T>::isNotEqualEps(objectDistance01));

	/**
	 * We combine the following steps:
	 * const Scalar cos_ObjectPoint0 = (objectDistanceToCP1 * objectDistanceToCP1 - objectDistanceToCP0 * objectDistanceToCP0 - objectDistance01 * objectDistance01) / (-2 * objectDistanceToCP0 * objectDistance01);
	 * const Scalar distanceToClosesPoint = cos_ObjectPoint0 * objectDistanceToCP0;
	 * const Vector3 directionObjectPoint0To1 = (objectPoint1 - objectPoint0) / objectDistance01;
	 * return objectPoint0 + directionObjectPoint0To1 * distanceToClosesPoint;
	 */

	const T factor = T(-0.5) * (objectDistanceToCP1 * objectDistanceToCP1 - objectDistanceToCP0 * objectDistanceToCP0 - objectDistance01 * objectDistance01) / (objectDistance01 * objectDistance01);

	return objectPoint0 * (T(1) - factor) + objectPoint1 * factor;
}

template <typename T>
inline bool P3P::constructClosestPointToCPOnObjectPlane(const VectorT3<T>& objectPoint0, const VectorT3<T>& objectPoint1, const VectorT3<T>& objectPoint2, const VectorT3<T>& closestPoint01, const VectorT3<T>& closestPoint02, VectorT3<T>& point)
{
	const VectorT3<T> objectDirection01 = objectPoint1 - objectPoint0;
	const VectorT3<T> objectDirection02 = objectPoint2 - objectPoint0;
	ocean_assert(NumericT<T>::isNotEqualEps(objectDirection01.length()));
	ocean_assert(NumericT<T>::isNotEqualEps(objectDirection02.length()));

	const VectorT3<T> objectPlaneNormal = objectDirection01.cross(objectDirection02); // normal not having unit length

	VectorT3<T> d01 = objectPlaneNormal.cross(objectDirection01);
	VectorT3<T> d02 = objectPlaneNormal.cross(objectDirection02);

	if (!d01.normalize() || !d02.normalize())
		return false;

	ocean_assert_accuracy(NumericT<T>::isWeakEqualEps(d01 * objectPlaneNormal.normalizedOrZero()));
	ocean_assert_accuracy(NumericT<T>::isWeakEqualEps(d02 * objectPlaneNormal.normalizedOrZero()));

	ocean_assert_accuracy(NumericT<T>::isWeakEqualEps(d01 * objectDirection01));
	ocean_assert_accuracy(NumericT<T>::isWeakEqualEps(d02 * objectDirection02));

	const LineT3<T> line01(closestPoint01, d01);
	const LineT3<T> line02(closestPoint02, d02);

#ifdef OCEAN_INTENSIVE_DEBUG
	{
		// we ensure that both lines are located in the plane (and respect that a line point may be very far away from our object points
		const PlaneT3<T> debugPlane(objectPoint0, objectPoint1, objectPoint2);

		const VectorT3<T> debugDirection0(line01.point() - objectPoint0);
		ocean_assert(NumericT<T>::isEqualEps(debugDirection0.length()) || NumericT<T>::angleIsEqual(debugPlane.normal().angle(debugDirection0), NumericT<T>::pi_2(), NumericT<T>::deg2rad(T(0.1))));

		const VectorT3<T> debugDirection1(line01.point(1) - objectPoint0);
		ocean_assert(NumericT<T>::isEqualEps(debugDirection1.length()) || NumericT<T>::angleIsEqual(debugPlane.normal().angle(debugDirection1), NumericT<T>::pi_2(), NumericT<T>::deg2rad(T(0.1))));

		const VectorT3<T> debugDirection2(line02.point(0) - objectPoint0);
		ocean_assert(NumericT<T>::isEqualEps(debugDirection2.length()) || NumericT<T>::angleIsEqual(debugPlane.normal().angle(debugDirection2), NumericT<T>::pi_2(), NumericT<T>::deg2rad(T(0.1))));

		const VectorT3<T> debugDirection3(line02.point(1) - objectPoint0);
		ocean_assert(NumericT<T>::isEqualEps(debugDirection3.length()) || NumericT<T>::angleIsEqual(debugPlane.normal().angle(debugDirection3), NumericT<T>::pi_2(), NumericT<T>::deg2rad(T(0.1))));
	}
#endif

	VectorT3<T> first, second;

	if (!line01.nearestPoints(line02, first, second))
		return false;

	point = (first + second) * T(0.5);
	return true;
}

template <typename T>
inline T P3P::sqr(const T value)
{
	return value * value;
}

}

}

#endif // META_OCEAN_GEOMETRY_P3P_H
