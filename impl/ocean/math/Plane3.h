/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_PLANE_3_H
#define META_OCEAN_MATH_PLANE_3_H

#include "ocean/math/Math.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector3.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class LineT3;

// Forward declaration.
template <typename T> class PlaneT3;

/**
 * Definition of the Plane3 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see PlaneT3
 * @ingroup math
 */
typedef PlaneT3<Scalar> Plane3;

/**
 * Definition of a 3D plane with double values.
 * @see PlaneT3
 * @ingroup math
 */
typedef PlaneT3<double> PlaneD3;

/**
 * Definition of a 3D plane with float values.
 * @see PlaneT3
 * @ingroup math
 */
typedef PlaneT3<float> PlaneF3;

/**
 * Definition of a typename alias for vectors with PlaneT3 objects.
 * @see PlaneT3
 * @ingroup math
 */
template <typename T>
using PlanesT3 = std::vector<PlaneT3<T>>;

/**
 * Definition of a vector holding Plane3 objects.
 * @see Plane3
 * @ingroup math
 */
typedef std::vector<Plane3> Planes3;

/**
 * This class implements a plane in 3D space.
 * The plane is defined by:<br>
 * (x - p)n = 0, xn - pn = 0, xn - d = 0,<br>
 * with Intersection point p, normal n and distance d.<br>
 * @tparam T Data type used to represent the plane
 * @see Plane3, PlaneF3, PlaneD3.
 * @ingroup math
 */
template <typename T>
class PlaneT3
{
	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates an invalid plane.
		 */
		PlaneT3() = default;

		/**
		 * Creates a plane by an intersection point and a normal.
		 * @param point Intersection point of the plane
		 * @param normal Unit vector perpendicular to the plane
		 */
		PlaneT3(const VectorT3<T>& point, const VectorT3<T>& normal);

		/**
		 * Creates a plane by three points lying in the plane.
		 * To create a valid plane three individual non-collinear points must be provided.<br>
		 * However, due to performance reasons this constructor accepts any kind of 3D object points but may create an invalid plane.<br>
		 * Thus, check whether the plane is valid after creation.
		 * @param point0 First intersection point
		 * @param point1 Second intersection point
		 * @param point2 Third intersection point
		 * @see isValid().
		 */
		PlaneT3(const VectorT3<T>& point0, const VectorT3<T>& point1, const VectorT3<T>& point2);

		/**
		 * Creates a plane by the plane's normal and the distance between origin and plane.
		 * @param normal Plane normal, must have length 1
		 * @param distance The signed distance between origin and plane, with range (-infinity, infinity)
		 */
		PlaneT3(const VectorT3<T>& normal, const T distance);

		/**
		 * Creates a plane by the plane's normal and the distance between origin and plane.
		 * The plane's normal is provided in yaw and pitch angles from an Euler rotation.<br>
		 * The default normal (with yaw and pitch zero) looks along the negative z-axis.<br>
		 * @param yaw The yaw angle of the plane's normal, in radian
		 * @param pitch The pitch angle of the plane's normal, in radian
		 * @param distance The distance between origin and plane
		 */
		PlaneT3(const T yaw, const T pitch, const T distance);

		/**
		 * Creates a plane by a transformation where the z-axis defined the plane's normal and the origin or the transformation defines a point on the plane.
		 * The z-axis will be normalized before it is assigned as normal of the plane.
		 * @param transformation The transformation from which a plane will be defined
		 */
		explicit PlaneT3(const HomogenousMatrixT4<T>& transformation);

		/**
		 * Returns whether a point is in the plane.
		 * @param point The point to check
		 * @param epsilon The epsilon value defining the accuracy threshold, with range [0, infinity)
		 * @return True, if so
		 */
		bool isInPlane(const VectorT3<T>& point, const T epsilon = NumericT<T>::eps()) const;

		/**
		 * Returns the distance between a given point and this plane.
		 * The distance will be positive for points on the side of the plane that its normal pointing to and negative on the other.
		 * @param point The point for which the distance will be determined
		 * @return Distance between plane and point, with range (-infinity, infinity)
		 */
		T signedDistance(const VectorT3<T>& point) const;

		/**
		 * Projects a given point onto the plane.
		 * @param point The point that will be projected
		 * @return The projected point
		 */
		VectorT3<T> projectOnPlane(const VectorT3<T>& point) const;

		/**
		 * Reflects a given vector in the plane.
		 * The given vector should point towards the plane.
		 * @param direction The direction vector being reflected
		 * @return The reflected vector
		 */
		VectorT3<T> reflect(const VectorT3<T>& direction);

		/**
		 * Returns the normal of the plane
		 * @return Plane normal
		 */
		inline const VectorT3<T>& normal() const;

		/**
		 * Returns the distance between plane and origin.
		 * @return Plane distance
		 */
		inline const T& distance() const;

		/**
		 * Calculates the yaw and pitch angle of the plane's normal.
		 * The angles are related to an Euler rotation while the default normal (with angles zero) looks along the negative z-axis.<br>
		 * @param yaw Resulting yaw angle, in radian
		 * @param pitch Resulting pitch angle, in radian
		 */
		inline void decomposeNormal(T& yaw, T& pitch) const;

		/**
		 * Transforms this plane so that it corresponds to a new coordinate system.
		 * @param iTransformation The inverse of a new coordinate system (defining the transformation from points defined in the coordinate system of this current plane to points defined in the new coordinate system)
		 * @return Resulting new plane for the given (inverted) coordinate system
		 */
		PlaneT3<T> transform(const HomogenousMatrixT4<T>& iTransformation) const;

		/**
		 * Determines a transformation having the origin in a given point on the plane, with z-axis corresponding to the normal of the plane and with y-axis corresponding to a given (projected) up-vector.
		 * @param pointOnPlane A 3D point on this 3D plane at which the origin of the transformation will be located
		 * @param upVector An up-vector starting at the given point on the 3D plane, the vector will be projected onto the plane and normalized to define the y-axis of the transformation, can be e.g., the viewing direction of the camera, must not be parallel to the plane's normal
		 * @param matrix The resulting matrix defining the transformation
		 * @return True, if succeeded
		 */
		bool transformation(const VectorT3<T>& pointOnPlane, const VectorT3<T>& upVector, HomogenousMatrixT4<T>& matrix) const;

		/**
		 * Sets the normal of this plane
		 * @param normal The normal to be set, must have length 1.
		 */
		inline void setNormal(const VectorT3<T>& normal);

		/**
		 * Sets the distance between plane and origin.
		 * @param distance Plane distance to be set
		 */
		inline void setDistance(const T distance);

		/**
		 * Returns a point on the plane.
		 * @return Point on plane
		 */
		inline VectorT3<T> pointOnPlane() const;

		/**
		 * Calculates the intersection between this plane and a given ray.
		 * @param ray The ray for which the intersection will be calculated, must be valid
		 * @param point Resulting intersection point
		 * @return True, if the ray has an intersection with this plane
		 */
		bool intersection(const LineT3<T>& ray, VectorT3<T>& point) const;

		/**
		 * Calculates the intersection between this plane and a second plane.
		 * @param plane The second plane for which the intersection will be calculated, must be valid
		 * @param line Resulting intersection line
		 * @return True, if the second plane has an intersection with this plane (if both planes are not parallel)
		 */
		bool intersection(const PlaneT3<T>& plane, LineT3<T>& line) const;

		/**
		 * Returns whether this plane is valid.
		 * A valid plane has a normal with length 1.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns whether two plane objects represent the same plane up to a specified epsilon.
		 * Two planes are identical if their normal vector and distance value is identical or inverted.
		 * @param plane The second plane to compare
		 * @param eps The epsilon value to be used, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isEqual(const PlaneT3<T>& plane, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether the plane has a normal with non-zero length.
		 * Beware: A plane with non-zero length normal may be invalid, however.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Returns an inverted plane of this plane having an inverted normal direction (and adjusted distance).
		 * @return The inverted plane
		 */
		inline PlaneT3<T> operator-() const;

		/**
		 * Returns whether two plane objects represent the same plane.
		 * Two planes are identical if their normal vector and distance value is identical or inverted.<br>
		 * @param right Second plane to compare
		 * @return True, if so
		 */
		inline bool operator==(const PlaneT3<T>& right) const;

		/**
		 * Returns whether two plane objects do not represent the same plane.
		 * @param right Second plane to compare
		 * @return True, if so
		 */
		inline bool operator!=(const PlaneT3<T>& right) const;

	protected:

		/// Normal of the plane.
		VectorT3<T> normal_ = VectorT3<T>(0, 0, 0);

		/// Distance of the plane.
		T distance_ = T(0);
};

template <typename T>
PlaneT3<T>::PlaneT3(const VectorT3<T>& point, const VectorT3<T>& normal) :
	normal_(normal),
	distance_(point * normal_)
{
	ocean_assert(NumericT<T>::isEqual(normal_.length(), T(1.0)));
}

template <typename T>
PlaneT3<T>::PlaneT3(const VectorT3<T>& point0, const VectorT3<T>& point1, const VectorT3<T>& point2)
{
	const VectorT3<T> direction10 = point1 - point0;
	const VectorT3<T> direction20 = point2 - point0;

	if (direction10 != direction20 && direction10 != -direction20) // due to floating point precision with ARM and 32bit, checking for different directions before determining the normal
	{
		normal_ = direction10.cross(direction20).normalizedOrZero();
		distance_ = point0 * normal_;
	}
}

template <typename T>
PlaneT3<T>::PlaneT3(const VectorT3<T>& normal, const T distance) :
	normal_(normal),
	distance_(distance)
{
	ocean_assert(isValid());
}

template <typename T>
PlaneT3<T>::PlaneT3(const T yaw, const T pitch, const T distance) :
	distance_(distance)
{
	const T hypotenuse = NumericT<T>::cos(pitch);
	normal_ = VectorT3<T>(-NumericT<T>::sin(yaw) * hypotenuse, NumericT<T>::sin(pitch), -NumericT<T>::cos(yaw) * hypotenuse);

	ocean_assert(isValid());
}

template <typename T>
PlaneT3<T>::PlaneT3(const HomogenousMatrixT4<T>& transformation) :
	normal_(transformation.zAxis()),
	distance_(0)
{
	ocean_assert(transformation.isValid());

	if (normal_.normalize())
	{
		distance_ = transformation.translation() * normal_;
	}

	ocean_assert(isValid());
}

template <typename T>
inline const VectorT3<T>& PlaneT3<T>::normal() const
{
	return normal_;
}

template <typename T>
inline const T& PlaneT3<T>::distance() const
{
	return distance_;
}

template <typename T>
inline void PlaneT3<T>::decomposeNormal(T& yaw, T& pitch) const
{
	ocean_assert(isValid());

	yaw = NumericT<T>::atan2(-normal_.x(), -normal_.z());
	pitch = NumericT<T>::asin(normal_.y());
}

template <typename T>
PlaneT3<T> PlaneT3<T>::transform(const HomogenousMatrixT4<T>& iTransformation) const
{
	ocean_assert(isValid() && iTransformation.isValid());

	const VectorT3<T> normal(iTransformation.rotationMatrix(normal_));
	ocean_assert(NumericT<T>::isEqual(normal.length(), 1));

	const VectorT3<T> pointOnNewPlane(iTransformation * pointOnPlane());
	ocean_assert((iTransformation.inverted() * pointOnNewPlane).isEqual(pointOnPlane(), NumericT<T>::weakEps()));

	const T distance = pointOnNewPlane * normal;

	const PlaneT3<T> result(normal, distance);
	ocean_assert(result.isInPlane(normal * distance, NumericT<T>::weakEps()));

	return result;
}

template <typename T>
bool PlaneT3<T>::transformation(const VectorT3<T>& pointOnPlane, const VectorT3<T>& upVector, HomogenousMatrixT4<T>& matrix) const
{
	ocean_assert(isValid());

	ocean_assert(NumericT<T>::isEqualEps(signedDistance(pointOnPlane)));
	ocean_assert(!normal_.isParallel(upVector));

	if (NumericT<T>::isNotEqualEps(signedDistance(pointOnPlane)))
	{
		return false;
	}

	VectorT3<T> yAxis = projectOnPlane(pointOnPlane + upVector) - pointOnPlane;

	if (!yAxis.normalize())
	{
		return false;
	}

	const VectorT3<T>& zAxis = normal_;

	ocean_assert(NumericT<T>::isEqualEps(yAxis * zAxis));

	const VectorT3<T> xAxis(yAxis.cross(zAxis));

	ocean_assert(NumericT<T>::isEqualEps(xAxis * yAxis));
	ocean_assert(NumericT<T>::isEqualEps(xAxis * zAxis));

	matrix = HomogenousMatrixT4<T>(xAxis, yAxis, zAxis, pointOnPlane);
	return true;
}

template <typename T>
inline void PlaneT3<T>::setNormal(const VectorT3<T>& normal)
{
	ocean_assert(NumericT<T>::isEqual(normal.length(), 1));
	normal_ = normal;
}

template <typename T>
inline void PlaneT3<T>::setDistance(const T distance)
{
	distance_ = distance;
}

template <typename T>
inline VectorT3<T> PlaneT3<T>::pointOnPlane() const
{
	ocean_assert(isInPlane(normal_ * distance_, NumericT<T>::weakEps()));
	return normal_ * distance_;
}

template <typename T>
bool PlaneT3<T>::isInPlane(const VectorT3<T>& point, const T epsilon) const
{
	ocean_assert(isValid());

	return NumericT<T>::isEqual(point * normal_ - distance_, 0, epsilon);
}

template <typename T>
T PlaneT3<T>::signedDistance(const VectorT3<T>& point) const
{
	ocean_assert(isValid());

	return point * normal_ - distance_;
}

template <typename T>
VectorT3<T> PlaneT3<T>::projectOnPlane(const VectorT3<T>& point) const
{
	ocean_assert(isValid());

	const VectorT3<T> result(point - normal_ * signedDistance(point));

	ocean_assert((std::is_same<T, float>::value) || isInPlane(result, NumericT<T>::weakEps()));
	ocean_assert((std::is_same<T, float>::value) || (isInPlane(point, NumericT<T>::weakEps()) || NumericT<T>::isWeakEqualEps(normal_ * (pointOnPlane() - result))));

	return result;
}

template <typename T>
VectorT3<T> PlaneT3<T>::reflect(const VectorT3<T>& direction)
{
	ocean_assert(isValid());

	// d' = (-dn)n + ((-dn)n + d)
	// d' = d - 2 (dn)n
	return direction - normal_ * (T(2.0) * (direction * normal_));
}

template <typename T>
bool PlaneT3<T>::intersection(const LineT3<T>& ray,VectorT3<T>& point) const
{
	ocean_assert(isValid());
	ocean_assert(ray.isValid());

	/**
	 * intersection point: ray.point() + t * ray.direction()
	 * t = (plane.distance() - plane.normal() * ray.point()) / (plane.normal() * ray.direction())
	 */

	const T denominator(normal_ * ray.direction());

	// if ray and plane are parallel
	if (NumericT<T>::isEqualEps(denominator))
	{
		return false;
	}

	const T factor = (distance_ - normal_ * ray.point()) / denominator;
	point = ray.point() + ray.direction() * factor;

	ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isWeakEqualEps((point - pointOnPlane()).normalizedOrZero() * normal_));

	return true;
}

template <typename T>
bool PlaneT3<T>::intersection(const PlaneT3<T>& plane, LineT3<T>& line) const
{
	ocean_assert(isValid());
	ocean_assert(plane.isValid());

	// the direction of the line will be perpendicular to both plane normals

	const VectorT3<T> lineDirection = normal_.cross(plane.normal());

	VectorT3<T> normalizedLineDirection = lineDirection;
	if (!normalizedLineDirection.normalize())
	{
		// both planes are parallel
		return false;
	}

	const T dotProduct = lineDirection * lineDirection;

	if (NumericT<T>::isEqualEps(dotProduct))
	{
		return false;
	}

	// find the point of the line which needs to be in both planes

	const VectorT3<T> linePoint = (plane.normal_ * distance_ - normal_ * plane.distance_).cross(lineDirection) / dotProduct;

	ocean_assert(NumericT<T>::isWeakEqualEps(signedDistance(linePoint)));
	ocean_assert(NumericT<T>::isWeakEqualEps(plane.signedDistance(linePoint)));

	ocean_assert(NumericT<T>::isWeakEqualEps(lineDirection * normal_));
	ocean_assert(NumericT<T>::isWeakEqualEps(lineDirection * plane.normal_));

	line = LineT3<T>(linePoint, normalizedLineDirection);

	return true;
}

template <typename T>
bool PlaneT3<T>::isValid() const
{
	return !normal_.isNull() && NumericT<T>::isEqual(normal_.length(), T(1.0));
}

template <typename T>
inline bool PlaneT3<T>::isEqual(const PlaneT3<T>& right, const T eps) const
{
	ocean_assert(isValid());

	return (NumericT<T>::isEqual(distance_, right.distance_, eps) && normal_.isEqual(right.normal_, eps))
				|| (NumericT<T>::isEqual(distance_, -right.distance_, eps) && normal_.isEqual(-right.normal_, eps));
}

template <typename T>
inline PlaneT3<T>::operator bool() const
{
	return !normal_.isNull();
}

template <typename T>
inline PlaneT3<T> PlaneT3<T>::operator-() const
{
	ocean_assert(isValid());

	const PlaneT3<T> result(-normal_, -distance_);

	ocean_assert(NumericT<T>::isEqualEps(result.signedDistance(pointOnPlane())));

	return result;
}

template <typename T>
inline bool PlaneT3<T>::operator==(const PlaneT3<T>& right) const
{
	ocean_assert(isValid());

	return isEqual(right);
}

template <typename T>
inline bool PlaneT3<T>::operator!=(const PlaneT3<T>& right) const
{
	return !(*this == right);
}

}

#endif // META_OCEAN_MATH_PLANE_3_H
