/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_CONE_3_H
#define META_OCEAN_MATH_CONE_3_H

#include "ocean/math/Math.h"

#include "ocean/math/Equation.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T>
class ConeT3;

/**
 * Definition of the Cone3 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see ConeT3
 * @ingroup math
 */
typedef ConeT3<Scalar> Cone3;

/**
 * Definition of a 3D cone with double values.
 * @see ConeT3
 * @ingroup math
 */
typedef ConeT3<double> ConeD3;

/**
 * Definition of a 3D cone with float values.
 * @see ConeT3
 * @ingroup math
 */
typedef ConeT3<float> ConeF3;

/**
 * Definition of a vector holding Cone3 objects.
 * @see Cone3
 * @ingroup math
 */
typedef std::vector<Cone3> Cones3;

/**
 * This class implements a (possibly truncated) 3D cone.
 * @ingroup math
 */
template <typename T>
class ConeT3
{
	public:

		/**
		 * Creates an invalid cone.
		 */
		ConeT3() = default;

		/**
		 * Defines a new cone.
		 * @param apex 3D coordinate for the tip of the cone
		 * @param axis Direction vector of the cone's axis, expected to already be normalized to unit length
		 * @param apexAngle Angle in radians formed at the apex by any vertical slice of the cone through its apex, must be in (0, pi)
		 * @param minSignedDistanceAlongAxis For a truncated cone, the minimum signed distance along the cone's axis at which the cones stops, defaults to the apex of the cone (distance = 0); for an infinite cone, set the minimum radius to -infinity
		 * @param maxSignedDistanceAlongAxis For a truncated cone, the maximum signed distance along the cone's axis at which the cone stops; note that only horizontal slices (perpendicular to its axis) through the cone are supported
		 */
		inline ConeT3(const VectorT3<T>& apex, const VectorT3<T>& axis, const T apexAngle, const T minSignedDistanceAlongAxis = T(0.0), const T maxSignedDistanceAlongAxis = std::numeric_limits<T>::infinity());

		/**
		 * Returns the tip of the cone.
		 * @return Cone apex
		 */
		inline const VectorT3<T>& apex() const;

		/**
		 * Returns the unit-length axis of the cone.
		 * @return Cone axis
		 */
		inline const VectorT3<T>& axis() const;

		/**
		 * Returns the angle made between diametrically opposite points on the cone and the apex.
		 * @return Cone's apex angle in radians, in range (0, pi)
		 */
		inline T apexAngle() const;

		/**
		 * Returns the minimum signed truncation distance along the cone's axis. If the cone ends at its apex, the value is zero; otherwise, the value is simply less than maxSignedDistanceAlongAxis.
		 * @return Distance value, with range (-infinity, maxSignedDistanceAlongAxis)
		 */
		inline const T& minSignedDistanceAlongAxis() const;

		/**
		 * Returns the maximum signed truncation distance along the cone's axis.
		 * @return Distance value, with range (minSignedDistanceAlongAxis, infinity)
		 */
		inline const T& maxSignedDistanceAlongAxis() const;

		/**
		 * Returns the closest point of intersection of a ray with the *outer surface* of the cone, ignoring intersections with the cone's base and intersections that 1) exit the cone or 2) are a negative signed distance along the ray.
		 * @param ray Ray for which to find the intersection, must be valid
		 * @param point Output 3D point of intersection
		 * @return True if the computed intersection point is valid, otherwise false
		 */
		bool nearestIntersection(const LineT3<T>& ray, VectorT3<T>& point) const;

		/**
		 * Returns whether this cone is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

	protected:

		/// Cone tip.
		VectorT3<T> apex_ = VectorT3<T>(T(0), T(0), T(0));

		/// Cone axis, a unit vector.
		VectorT3<T> axis_ = VectorT3<T>(T(0), T(0), T(0));

		/// Cosine-squared of half of the cone's apex angle.
		T cosSquaredHalfApexAngle_ = T(0);

		/// Minimum signed truncation distance along the cone's axis.
		T minSignedDistanceAlongAxis_ = T(0);

		/// Maximum signed truncation distance along the cone's axis.
		T maxSignedDistanceAlongAxis_ = T(0);
};

template <typename T>
inline ConeT3<T>::ConeT3(const VectorT3<T>& apex, const VectorT3<T>& axis, const T apexAngle, const T minSignedDistanceAlongAxis, const T maxSignedDistanceAlongAxis) :
	apex_(apex),
	axis_(axis),
	cosSquaredHalfApexAngle_(NumericT<T>::sqr(NumericT<T>::cos(T(0.5) * apexAngle))),
	minSignedDistanceAlongAxis_(minSignedDistanceAlongAxis),
	maxSignedDistanceAlongAxis_(maxSignedDistanceAlongAxis)
{
	ocean_assert(NumericT<T>::isEqual(axis.length(), 1));
	ocean_assert(apexAngle > NumericT<T>::eps() && apexAngle < NumericT<T>::pi() - NumericT<T>::eps());
	ocean_assert(minSignedDistanceAlongAxis < maxSignedDistanceAlongAxis);
}

template <typename T>
inline const VectorT3<T>& ConeT3<T>::apex() const
{
	return apex_;
}

template <typename T>
inline const VectorT3<T>& ConeT3<T>::axis() const
{
	return axis_;
}

template <typename T>
inline T ConeT3<T>::apexAngle() const
{
	ocean_assert(cosSquaredHalfApexAngle_ >= Scalar(0.0));
	return T(2.0) * NumericT<T>::acos(NumericT<T>::sqrt(cosSquaredHalfApexAngle_));
}

template <typename T>
inline const T& ConeT3<T>::maxSignedDistanceAlongAxis() const
{
	return maxSignedDistanceAlongAxis_;
}

template <typename T>
inline const T& ConeT3<T>::minSignedDistanceAlongAxis() const
{
	return minSignedDistanceAlongAxis_;
}

template <typename T>
bool ConeT3<T>::nearestIntersection(const LineT3<T>& ray, VectorT3<T>& point) const
{
	ocean_assert(isValid() && ray.isValid());

	// Denote the cone apex as Q and its axis as q. Let the ray origin be C and its direction be d.
	// The surface of the cone is defined as S = { X = \in R^3 | (1 / ||X - Q||) * (X - Q).q) = cos(theta/2), where theta is the apex angle of the cone.
	// An intersection point P \in S satisfies P = C + t * d, where t is the signed distance from the ray origin. We find values of t, accordingly.
	// Denoting V = C - Q and m = cos^2(theta/2), and then squaring the equation for the domain of S, we have
	//     (1 / ||V + t * d||^2) * [ (V + t * d).q ]^2 = m
	//   (V.q)^2 + 2 * t * V.q * d.q) + t^2 * (d.q)^2 = m * (V.V + t * 2 * V.d + t^2 * d.d)    <= multiplied by ||.|| term and expanded
	//   => t^2 * ((d.q)^2 - m * d.d) + t * 2 * (V.q * d.q - m * V.d) + (V.q)^2 - m * V.V = 0, <= subtracted right side
	// which we can then solve using the quadratic equation.

	const VectorT3<T> V = ray.point() - apex_;
	const VectorT3<T>& d = ray.direction();
	const VectorT3<T>& q = axis_;
	const T& m = cosSquaredHalfApexAngle_;

	const T d_dot_q = d * q;
	const T d_dot_d = d.sqr();
	const T V_dot_q = V * q;
	const T V_dot_d = V * d;
	const T V_dot_V = V.sqr();

	T a = d_dot_q * d_dot_q - m * d_dot_d;
	T b = T(2.0) * (V_dot_q * d_dot_q - m * V_dot_d);
	T c = V_dot_q * V_dot_q - m * V_dot_V;

	// Normalize the quadratic before solving.
	const T magnitude = std::max(std::max(NumericT<T>::abs(a), NumericT<T>::abs(b)), NumericT<T>::abs(c));
	if (NumericT<T>::isNotEqualEps(magnitude))
	{
		a /= magnitude;
		b /= magnitude;
		c /= magnitude;
	}

	T minDistance = T(-1.0);
	T maxDistance = T(-1.0);

	// Check the corner case of a linear equation.
	if (NumericT<T>::isEqualEps(a))
	{
		if (NumericT<T>::isNotEqualEps(b))
		{
			minDistance = -c / b;
			maxDistance = minDistance;
		}
	}
	else if (EquationT<T>::solveQuadratic(a, b, c, minDistance, maxDistance))
	{
		if (minDistance > maxDistance)
		{
			std::swap(minDistance, maxDistance);
		}
	}

	// If the minimum distance is negative, either the ray origin is inside the cone, or the ray does
	// not intersect with the cone.
	if (minDistance < T(0.0)) // use <= to disallow points exactly on the surface
	{
		return false;
	}

	point = ray.point(minDistance);

	const T minIntersectionDistanceAlongAxis = (point - apex_) * axis_; // signed distance of point projected onto cone's axis

	bool intersectionIsValid = minIntersectionDistanceAlongAxis >= minSignedDistanceAlongAxis_ && minIntersectionDistanceAlongAxis <= maxSignedDistanceAlongAxis_;

	// We also have to check the case that the first intersection point actually exits the cone.
	// In this case, the ray origin is inside the infinite cone and may or may not be inside the
	// truncated cone, itself.
	// \        /
	//  \ O    /  <- ray origin
	//   \|   /
	//    A  /    <- first intersection (exit)
	//    |\/
	//    |/\
	//    B  \    <- second intersection (entrance)
	//   /|   \
	//  / |    \
	// /  V     \
	//
	// In this case, we'll return the second intersection point; this can be detected by checking
	// whether there is a sign flip of the projected signed distance along the cone's axis between
	// points A and B in the figure above. Note that we'll only return true, however, if point A is
	// not also on the surface of the truncated cone (otherwise, B is "occluded").

	if (maxDistance > minDistance)
	{
		const VectorT3<T> maxPoint = ray.point(maxDistance);
		const T maxIntersectionDistanceAlongAxis = (maxPoint - apex_) * axis_;

		if (NumericT<T>::sign(minIntersectionDistanceAlongAxis) != NumericT<T>::sign(maxIntersectionDistanceAlongAxis))
		{
			point = maxPoint;
			intersectionIsValid = !intersectionIsValid && maxIntersectionDistanceAlongAxis >= minSignedDistanceAlongAxis_ && maxIntersectionDistanceAlongAxis <= maxSignedDistanceAlongAxis_;
		}
	}

	return intersectionIsValid;
}

template <typename T>
inline bool ConeT3<T>::isValid() const
{
	return cosSquaredHalfApexAngle_ > T(0.0) && maxSignedDistanceAlongAxis_ > minSignedDistanceAlongAxis_ && NumericT<T>::isEqual(axis_.sqr(), T(1.0));
}

} // namespace Ocean

#endif // META_OCEAN_MATH_CONE_3_H
