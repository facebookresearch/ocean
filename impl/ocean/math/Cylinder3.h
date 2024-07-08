/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_CYLINDER_3_H
#define META_OCEAN_MATH_CYLINDER_3_H

#include "ocean/math/Math.h"

#include "ocean/math/Equation.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T>
class CylinderT3;

/**
 * Definition of the Cylinder3 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see CylinderT3
 * @ingroup math
 */
typedef CylinderT3<Scalar> Cylinder3;

/**
 * Definition of a 3D cylinder with double values.
 * @see CylinderT3
 * @ingroup math
 */
typedef CylinderT3<double> CylinderD3;

/**
 * Definition of a 3D cylinder with float values.
 * @see CylinderT3
 * @ingroup math
 */
typedef CylinderT3<float> CylinderF3;

/**
 * Definition of a vector holding Cylinder3 objects.
 * @see Cylinder3
 * @ingroup math
 */
typedef std::vector<Cylinder3> Cylinders3;

/**
 * This class implements a 3D cylinder defined by its origin, axis, radius, and (signed) starting and stopping points along its axis.
 *
 * Consider a finite right cylinder, consisting of (1) two circular endcaps that lie in parallel planes and (2) the curved surface connecting the endcaps.
 * Let one of these endcaps be the "bottom base" and the other endcap be the "top base" of the cylinder.
 * The cylinder axis is the unit vector pointing from the center of the bottom base to the center of
 * the top base.
 * Since we are considering a right (i.e., non-oblique) cylinder, each cylinder base is perpendicular to the axis.
 *
 * Each cylinder has a 4DOF coordinate frame consisting of the line through its axis in 3D space.
 * The cylinder origin can be defined using any 3D point lying on this line.
 * The 3D center point of each cylinder base also lies on this line, at a fixed 1D distance along
 * the line from the cylinder origin:
 *   bottom = origin + minSignedDistance * axis,
 *   top = origin + maxSignedDistance * axis.
 *
 * The cylinder radius, r, is defined as the radius of the two circular endcaps.
 * For any 3D point X lying on the cylinder surface between the endcaps, the distance to the nearest point on the cylinder center-line is r.
 *
 * Note that the origin does not need to lie within the cylinder.
 * Infinite cylinders (without endcaps) and half-infinite cylinders (with only one endcap) are also allowed.
 *
 * Currently, cylinder-ray intersection checking is only supported for portion of the cylinder between its endcaps. Ray intersections with cylinder endcaps are not computed.
 * @ingroup math
 */
template <typename T>
class CylinderT3
{
	public:

		/**
		 * Creates an invalid cylinder.
		 */
		inline CylinderT3();

		/**
		 * Defines a new cylinder with a base along its axis at distance = 0 and with the specified height (positive distance along the axis).
		 * @param origin 3D coordinate for the center of the base of the cylinder
		 * @param axis Direction vector of the cylinder's axis, expected to already be normalized to unit length
		 * @param radius Radius of the cylinder, with range (0, infinity)
		 * @param height The height of the cylinder, with range [0, infinity]
		 */
		inline CylinderT3(const VectorT3<T>& origin, const VectorT3<T>& axis, const T radius, const T height);

		/**
		 * Defines a new cylinder.
		 * @param origin 3D coordinate for the center of the base of the cylinder
		 * @param axis Direction vector of the cylinder's axis, expected to already be normalized to unit length
		 * @param radius Radius of the cylinder, with range (0, infinity)
		 * @param minSignedDistanceAlongAxis Signed distance along the axis for the bottom of the cylinder, with range [-infinity, maxSignedDistanceAlongAxis]
		 * @param maxSignedDistanceAlongAxis Signed distance along the axis for the top of the cylinder, with range [minSignedDistanceAlongAxis, infinity]
		 */
		inline CylinderT3(const VectorT3<T>& origin, const VectorT3<T>& axis, const T radius, const T minSignedDistanceAlongAxis, const T maxSignedDistanceAlongAxis);

		/**
		 * Returns the center of the cylinder's base
		 * @return Cylinder origin
		 */
		inline const VectorT3<T>& origin() const;

		/**
		 * Returns the unit-length axis of the cylinder.
		 * @return Cylinder axis
		 */
		inline const VectorT3<T>& axis() const;

		/**
		 * Returns the radius of the cylinder
		 * @return Cylinder radius, with range (0, infinity)
		 */
		inline const T& radius() const;

		/**
		 * Returns the minimum signed truncation distance along the cylinder's axis.
		 * @return Distance value, with range [-infinity, maxSignedDistanceAlongAxis]
		 */
		inline const T& minSignedDistanceAlongAxis() const;

		/**
		 * Returns the maximum signed truncation distance along the cylinder's axis.
		 * @return Distance value, with range [minSignedDistanceAlongAxis, infinity]
		 */
		inline const T& maxSignedDistanceAlongAxis() const;

		/**
		 * Returns the length of the cylinder along its axis
		 * @return Cylinder's height, with range [0, infinity]
		 */
		inline T height() const;

		/**
		 * Returns the closest point of intersection of a ray with the *outer surface* of the cylinder, ignoring intersections with the cylinder's base and intersections that 1) exit the cylinder or 2) are a negative signed distance along the ray.
		 * @param ray Ray for which to find the intersection, must be valid
		 * @param point Output 3D point of intersection
		 * @return True if the computed intersection point is valid, otherwie false
		 */
		bool nearestIntersection(const LineT3<T>& ray, VectorT3<T>& point) const;

		/**
		 * Returns whether this cylinder is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

	protected:

		/// Center of the cylinder's base
		VectorT3<T> origin_;

		/// Cylinder axis, a unit vector.
		VectorT3<T> axis_;

		/// Radius of the cylinder.
		T radius_;

		/// Minimum signed truncation distance along the cone's axis.
		T minSignedDistanceAlongAxis_;

		/// Maximum signed truncation distance along the cone's axis.
		T maxSignedDistanceAlongAxis_;
};

template <typename T>
inline CylinderT3<T>::CylinderT3() :
	origin_(0.0, 0.0, 0.0),
	axis_(0.0, 0.0, 0.0),
	radius_(0.0),
	minSignedDistanceAlongAxis_(0.0),
	maxSignedDistanceAlongAxis_(0.0)
{
	// nothing to do here
}

template <typename T>
inline CylinderT3<T>::CylinderT3(const VectorT3<T>& origin, const VectorT3<T>& axis, const T radius, const T height) :
	CylinderT3(origin, axis, radius, T(0.0), height)
{
	// nothing to do here
}

template <typename T>
CylinderT3<T>::CylinderT3(const VectorT3<T>& origin, const VectorT3<T>& axis, const T radius, const T minSignedDistanceAlongAxis, const T maxSignedDistanceAlongAxis) :
	origin_(origin),
	axis_(axis),
	radius_(radius),
	minSignedDistanceAlongAxis_(minSignedDistanceAlongAxis),
	maxSignedDistanceAlongAxis_(maxSignedDistanceAlongAxis)
{
	ocean_assert(NumericT<T>::isEqual(axis.length(), 1));
	ocean_assert(minSignedDistanceAlongAxis_ <= maxSignedDistanceAlongAxis_);
}

template <typename T>
inline const VectorT3<T>& CylinderT3<T>::origin() const
{
	return origin_;
}

template <typename T>
inline const VectorT3<T>& CylinderT3<T>::axis() const
{
	return axis_;
}

template <typename T>
inline const T& CylinderT3<T>::radius() const
{
	return radius_;
}

template <typename T>
inline const T& CylinderT3<T>::minSignedDistanceAlongAxis() const
{
	return minSignedDistanceAlongAxis_;
}

template <typename T>
inline const T& CylinderT3<T>::maxSignedDistanceAlongAxis() const
{
	return maxSignedDistanceAlongAxis_;
}

template <typename T>
inline T CylinderT3<T>::height() const
{
	return maxSignedDistanceAlongAxis_ - minSignedDistanceAlongAxis_;
}

template <typename T>
bool CylinderT3<T>::nearestIntersection(const LineT3<T>& ray, VectorT3<T>& intersection) const
{
	ocean_assert(isValid() && ray.isValid());

	// First, we'll compute the intersection of the ray with an infinite cylinder -- i.e., we'll ignore the end caps of the cylinder.
	// We'll then check if the intersection point falls between the cylinder caps (if applicable).
	//
	// Denote the cylinder origin as Q, its axis by unit vector q, and its radius as r.
	// When Q is projected onto the 2D plane perpendicular to q, the projected point lies at the origin.
	// The surface of the (infinite) cylinder is defined as
	//
	//   S = { X \in R^3  |  || (X - Q) - ((X - Q)^T * q) * q || = r },
	//
	// i.e., project 3D point X onto the 2D plane perpendicular to q, and check that this projected point lies on the circle with radius r.
	//
	// Denote the ray origin as C and its unit direction vector as d.
	// An intersection point P \in S satisfies P = C + t * d, where t is the signed distance from the ray origin. We find values of t, accordingly.
	//
	// Denoting V = C - Q and then squaring the equation for the domain of S, we have
	//
	//     [ V + t * d - ((V + t * d)^T * q) * q ]^2 = r^2,
	//
	// which reduces to
	//
	//       [ d^T * d - (d^T * q)^2 ] * t^2 + 2 * [ d^T * V - (d^T * q) * (V^T * q) ] * t + [ (V^T * V) - (V^T * q)^2 - r^2] = 0,
	//
	// which we can then solve using the quadratic equation.

	const VectorT3<T> V = ray.point() - origin_;
	const VectorT3<T>& d = ray.direction();
	const VectorT3<T>& q = axis_;

	const T d_dot_q = d * q;
	const T d_dot_d = d.sqr();
	const T V_dot_q = V * q;
	const T V_dot_d = V * d;
	const T V_dot_V = V.sqr();

	const T a = d_dot_d - d_dot_q * d_dot_q;
	const T b = T(2.) * (V_dot_d - d_dot_q * V_dot_q);
	const T c = V_dot_V - V_dot_q * V_dot_q - radius_ * radius_;

	T minDistance = T(-1.), maxDistance = T(-1.);

	if (EquationT<T>::solveQuadratic(a, b, c, minDistance, maxDistance))
	{
		minDistance = min(minDistance, maxDistance);
	}
	else
	{
		// Check the corner case of a linear equation (the axis and direction are parallel, and the
		// point might be on the surface).
		if (NumericT<T>::isEqualEps(a) && !NumericT<T>::isEqualEps(b))
		{
			minDistance = -c / b;
		}
	}

	if (minDistance < T(0.)) // use <= to disallow points exactly on the surface
	{
		return false;
	}

	intersection = ray.point(minDistance);

	const T distanceAlongAxis = (intersection - origin_) * axis_; // signed distance of the intersection point projected onto the cylinder's axis
	return (distanceAlongAxis >= minSignedDistanceAlongAxis_ && distanceAlongAxis <= maxSignedDistanceAlongAxis_);
}

template <typename T>
inline bool CylinderT3<T>::isValid() const
{
	return radius_ > T(0.) && maxSignedDistanceAlongAxis_ >= minSignedDistanceAlongAxis_ && NumericT<T>::isEqual(axis_.sqr(), T(1.));
}

} // namespace Ocean

#endif // META_OCEAN_MATH_CYLINDER_3_H

