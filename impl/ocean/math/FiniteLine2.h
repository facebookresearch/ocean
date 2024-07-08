/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_FINITE_LINE_2_H
#define META_OCEAN_MATH_FINITE_LINE_2_H

#include "ocean/math/Math.h"
#include "ocean/math/Line2.h"
#include "ocean/math/Vector2.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class FiniteLineT2;

/**
 * Definition of the FiniteLine2 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see FiniteLineT2
 * @ingroup math
 */
typedef FiniteLineT2<Scalar> FiniteLine2;

/**
 * Instantiation of the LineT2 template class using a double precision float data type.
 * @see FiniteLineT2
 * @ingroup math
 */
typedef FiniteLineT2<double> FiniteLineD2;

/**
 * Instantiation of the LineT2 template class using a single precision float data type.
 * @see FiniteLineT2
 * @ingroup math
 */
typedef FiniteLineT2<float> FiniteLineF2;

/**
 * Definition of a typename alias for vectors with FiniteLineT2 objects.
 * @see FiniteLineT2
 * @ingroup math
 */
template <typename T>
using FiniteLinesT2 = std::vector<FiniteLineT2<T>>;

/**
 * Definition of a vector holding FiniteLine2 objects.
 * @see FiniteLineF2, Line2
 * @ingroup math
 */
typedef std::vector<FiniteLine2> FiniteLines2;

/**
 * Definition of a vector holding FiniteLineD2 objects.
 * @see FiniteLineD2, Line2
 * @ingroup math
 */
typedef std::vector<FiniteLineD2> FiniteLinesD2;

/**
 * Definition of a vector holding FiniteLineF2 objects.
 * @see FiniteLineF2, Line2
 * @ingroup math
 */
typedef std::vector<FiniteLineF2> FiniteLinesF2;

/**
 * This class implements an finite line in 2D space.
 * The finite line object is invalid if both end points of the line object are identical.<br>
 * @tparam T Data type used to represent lines
 * @see FiniteLine2, FiniteLineF2, FiniteLineD2, LineT2, Line2
 * @ingroup math
 */
template <typename T>
class FiniteLineT2
{
	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates a finite line with default parameters.
		 */
		FiniteLineT2() = default;

		/**
		 * Creates a finite line defined by two end points of the line.
		 * @param point0 First end point of the line
		 * @param point1 Second end point of the line, must be different from point0 to create a valid line, otherwise the line is invalid
		 */
		FiniteLineT2(const VectorT2<T>& point0, const VectorT2<T>& point1);

		/**
		 * Copies a line with different data type than T.
		 * @param line The line to copy
		 * @tparam U The data type of the second line
		 */
		template <typename U>
		inline explicit FiniteLineT2(const FiniteLineT2<U>& line);

		/**
		 * Returns the first end point of the line.
		 * @return First end point of the line
		 */
		inline const VectorT2<T>& point0() const;

		/**
		 * Returns the second end point of the line.
		 * @return Second end point of the line
		 */
		inline const VectorT2<T>& point1() const;

		/**
		 * Returns the first or second end point of the line.
		 * @param index The index of the point to be returned, with range [0, 1]
		 * @return First or second end point of the line
		 */
		inline const VectorT2<T>& point(const unsigned int index) const;

		/**
		 * Returns the midpoint of the line.
		 * @return Midpoint point of the line
		 */
		inline VectorT2<T> midpoint() const;

		/**
		 * Returns the direction of the line: normalized(point1() - point0())
		 * @return Direction vector with unit length, a zero vector if the line is invalid
		 * @see isValid().
		 */
		inline const VectorT2<T>& direction() const;

		/**
		 * Returns the normal of the line: -direction().perpendicular()
		 * The 2D cross product between the resulting normal and the direction of this line will be positive
		 * @return Normal vector with unit length
		 * @see isValid().
		 */
		inline const VectorT2<T> normal() const;

		/**
		 * Returns the squared length of the finite line.
		 * @return Squared distance between the end points of the line
		 */
		inline T sqrLength() const;

		/**
		 * Returns the length of the finite line.
		 * @return Distance between the end points of the line
		 */
		inline T length() const;

		/**
		 * Returns whether a given point is part of the finite line.
		 * @param point The point to check
		 * @return True, if so
		 * @see isOnInfiniteLine().
		 */
		inline bool isOnLine(const VectorT2<T>& point) const;

		/**
		 * Returns whether a given point lies on the infinite line defined by this finite line.
		 * @param point The point to check
		 * @return True, if so
		 * @see isOnLine().
		 */
		inline bool isOnInfiniteLine(const VectorT2<T>& point) const;

		/**
		 * Check if a point is in the left half-plane of the direction vector of a line
		 * A point @c p is located on the left side of a line, if the cross product of the direction of the line, @c d, and the vector pointing from starting point of the line, @c s, to the point @c p is positive: (d x (p - s)) > 0.
		 * It's on the line the cross product is zero and in the right half-plane it is negative.
		 * @note Keep in mind that if the point is not in the left half-plane, it doesn't necessarily mean that it's in the right half-plane because it could just as well be located on the line.
		 * @sa direction()
		 * @param point The point to check
		 * @return True, if the point is in the left half-plane
		 */
		inline bool isLeftOfLine(const VectorT2<T>& point) const;

		/**
		 * Returns the distance between the line and a given point.
		 * This function needs a unit vector as direction!
		 * @param point The point to return the distance for
		 * @return Distance between point and line
		 */
		inline T distance(const VectorT2<T>& point) const;

		/**
		 * Returns the square distance between the line and a given point.
		 * @param point The point to return the distance for
		 * @return The resulting square distance between point and line, with range [0, infinity)
		 */
		inline T sqrDistance(const VectorT2<T>& point) const;

		/**
		 * Returns the point lying on this finite line nearest to an arbitrary given point.
		 * This function needs a unit vector as direction!
		 * @param point Arbitrary point outside the line
		 * @return Nearest point on the finite line
		 * @see nearestPointOnInfiniteLine().
		 */
		VectorT2<T> nearestPoint(const VectorT2<T>& point) const;

		/**
		 * Returns the point on the infinite line (defined by this finite line) to an arbitrary given point.
		 * In addition to providing the projected point on the infinite line, this function can return the distance to the closest end point of the finite line when lying outside of the finite line's boundaries.
		 * A negative distance indicates a projected point close to point0(), a positive distance indicates a projected point close to point1().
		 * This function needs a unit vector as direction!
		 * @param point Arbitrary point outside the line
		 * @param outOfBoundaryDistance Optional resulting distance between the projected point on the infinite line and the closest end point of the finite line, with range (-infinity, infinity)
		 * @param finiteLineLocation Optional resulting location of the projected point in relation to the finite line so that the following holds point() + direction() * finiteLineLocation, with range (-infinity, infinity)
		 * @return Nearest point on the infinite line
		 * @see nearestPoint().
		 */
		VectorT2<T> nearestPointOnInfiniteLine(const VectorT2<T>& point, T* outOfBoundaryDistance = nullptr, T* finiteLineLocation = nullptr) const;

		/**
		 * Returns the unique intersection point of two finite lines.
		 * Two aligned lines do not have one common intersection point, so that the function will return 'false' in such a case.
		 * @param second The second line for intersection calculation
		 * @param point Resulting intersection point
		 * @return True, if both lines have a common intersection point
		 * @see intersects().
		 */
		inline bool intersection(const FiniteLineT2<T>& second, VectorT2<T>& point) const;

		/**
		 * Returns the unique intersection point of this finite line with an infinite line.
		 * Two aligned lines do not have one common intersection point, so that the function will return 'false' in such a case.
		 * @param second The second line for intersection calculation
		 * @param point Resulting intersection point
		 * @return True, if both lines have a common intersection point
		 * @see intersects().
		 */
		inline bool intersection(const LineT2<T>& second, VectorT2<T>& point) const;

		/**
		 * Returns whether two finite lines have a unique intersection point.
		 * @param second The second line to check
		 * @return True, if both lines intersect
		 * @see intersection().
		 */
		inline bool intersects(const FiniteLineT2<T>& second) const;

		/**
		 * Returns whether two finite lies have an intersection.
		 * @param second The second line to check
		 * @return True, if both lines intersect
		 * @see intersection().
		 */
		inline bool intersects(const LineT2<T>& second) const;

		/**
		 * Returns whether two lines are parallel up to a small epsilon.
		 * @param right Second line
		 * @return True, if so
		 */
		inline bool isParallel(const FiniteLineT2<T>& right) const;

		/**
		 * Check for collinearity with other line segment
		 * @param right Line to check for collinearity
		 * @param distanceEpsilon Acceptable distance of the endpoints of one line segment from the infinite line corresponding to the other line, default value: `Numeric::weakEps()`
		 * @param cosAngleEpsilon Cosine of the maximum angle that is allowed in order for the two segments to be considered parallel; default: cos(weakEps()), i.e., approx. one, range: [0, 1]
		 * @return True if the this line and the other one are collinear, otherwise false
		 */
		inline bool isCollinear(const FiniteLineT2<T>& right, const T& distanceEpsilon = NumericT<T>::weakEps(), const T& cosAngleEpsilon = NumericT<T>::cos(NumericT<T>::weakEps())) const;

		/**
		 * Returns whether this line has valid parameters.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether two lines are equal up to a specified epsilon.
		 * Two lines are equal if both lines have the same end points (while the order of the points is not important).
		 * @param line The second line to be used for comparison, must be valid
		 * @param epsilon The maximal distance between two equal end points, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isEqual(const FiniteLineT2<T>& line, const T& epsilon) const;

		/**
		 * Returns whether two line are identical up to a small epsilon.
		 * Two finite lines are identical if both lines have the same endpoint (independent of the order of the end points).
		 * @param right The right line
		 * @return True, if so
		 * @see isEqual().
		 */
		inline bool operator==(const FiniteLineT2<T>& right) const;

		/**
		 * Returns whether two line are identical up to a small epsilon.
		 * @param right The right line
		 * @return True, if so
		 */
		inline bool operator!=(const FiniteLineT2<T>& right) const;

		/**
		 * Returns whether this line is valid.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// First end point of the line.
		VectorT2<T> point0_ = VectorT2<T>(T(0), T(0));

		/// Second end point of the line.
		VectorT2<T> point1_ = VectorT2<T>(T(0), T(0));

		/// Direction of the line with unit length, if the object holds valid parameters.
		VectorT2<T> direction_ = VectorT2<T>(T(0), T(0));
};

template <typename T>
FiniteLineT2<T>::FiniteLineT2(const VectorT2<T>& point0, const VectorT2<T>& point1) :
	point0_(point0),
	point1_(point1),
	direction_((point1 - point0).normalizedOrZero())
{
	// nothing to do here
}

template <typename T>
template <typename U>
inline FiniteLineT2<T>::FiniteLineT2(const FiniteLineT2<U>& line)
{
	point0_ = VectorT2<T>(line.point0());
	point1_ = VectorT2<T>(line.point1());
	direction_ = VectorT2<T>(line.direction());
}

template <typename T>
inline const VectorT2<T>& FiniteLineT2<T>::point0() const
{
	return point0_;
}

template <typename T>
inline const VectorT2<T>& FiniteLineT2<T>::point1() const
{
	return point1_;
}

template <typename T>
inline const VectorT2<T>& FiniteLineT2<T>::point(const unsigned int index) const
{
	ocean_assert(index <= 1u);

	if (index == 0u)
	{
		return point0_;
	}
	else
	{
		return point1_;
	}
}

template <typename T>
inline VectorT2<T> FiniteLineT2<T>::midpoint() const
{
  return (point0_ + point1_) * T(0.5);
}

template <typename T>
inline const VectorT2<T>& FiniteLineT2<T>::direction() const
{
	return direction_;
}

template <typename T>
inline const VectorT2<T> FiniteLineT2<T>::normal() const
{
	ocean_assert(isValid());
	const VectorT2<T> result = -direction_.perpendicular();
	ocean_assert(Ocean::NumericT<T>::isEqual(result.length(), T(1)));
	ocean_assert(result.cross(direction_) > 0);
	return result;
}

template <typename T>
inline T FiniteLineT2<T>::sqrLength() const
{
	return (point1_ - point0_).sqr();
}

template <typename T>
inline T FiniteLineT2<T>::length() const
{
	return (point1_ - point0_).length();
}

template <typename T>
inline bool FiniteLineT2<T>::isOnLine(const VectorT2<T>& point) const
{
	ocean_assert(isValid());

	return NumericT<T>::isEqualEps(sqrDistance(point));
}

template <typename T>
inline bool FiniteLineT2<T>::isOnInfiniteLine(const VectorT2<T>& point) const
{
	ocean_assert(isValid());

	return NumericT<T>::isEqualEps(nearestPointOnInfiniteLine(point).sqrDistance(point));
}

template <typename T>
inline bool FiniteLineT2<T>::isLeftOfLine(const VectorT2<T>& point) const
{
	ocean_assert(isValid());

	return LineT2<T>(point0_, direction_).isLeftOfLine(point);
}

template <typename T>
inline T FiniteLineT2<T>::distance(const VectorT2<T>& point) const
{
	ocean_assert(isValid());

	return NumericT<T>::sqrt(sqrDistance(point));
}

template <typename T>
inline T FiniteLineT2<T>::sqrDistance(const VectorT2<T>& point) const
{
	ocean_assert(isValid());

	return nearestPoint(point).sqrDistance(point);
}

template <typename T>
VectorT2<T> FiniteLineT2<T>::nearestPoint(const VectorT2<T>& point) const
{
	ocean_assert(isValid());

	const VectorT2<T> lineOffset(point1_ - point0_);
	const VectorT2<T> pointOffset(point - point0_);

	const T dotProduct = lineOffset * pointOffset;

	// the projected point does not lie on the finite line (before the first end point)
	if (dotProduct <= 0)
	{
		return point0_;
	}

	// the projected point does not lie on the finite line (behind the second end point)
	if (dotProduct >= lineOffset.sqr())
	{
		return point1_;
	}

	// the projected point lies on the finite line
	return point0_ + direction_ * (pointOffset * direction_);
}

template <typename T>
VectorT2<T> FiniteLineT2<T>::nearestPointOnInfiniteLine(const VectorT2<T>& point, T* outOfBoundaryDistance, T* finiteLineLocation) const
{
	ocean_assert(isValid());

	const VectorT2<T> lineOffset(point1_ - point0_);
	const VectorT2<T> pointOffset(point - point0_);

	const T dotProduct = lineOffset * pointOffset;

	if (dotProduct <= 0)
	{
		// the projected point does not lie on the finite line (before the first end point)

		if (outOfBoundaryDistance)
		{
			// we have a negative distance

			*outOfBoundaryDistance = std::min(pointOffset * direction_, T(0));
			ocean_assert(*outOfBoundaryDistance <= T(0));
		}
	}
	else if (dotProduct >= lineOffset.sqr())
	{
		// the projected point does not lie on the finite line (behind the second end point)

		if (outOfBoundaryDistance)
		{
			// we have a positive distance

			*outOfBoundaryDistance = std::max(T(0), pointOffset * direction_ - length());
			ocean_assert(*outOfBoundaryDistance >= T(0));
		}
	}
	else
	{
		// the projected point lies within the finite line

		if (outOfBoundaryDistance)
		{
			*outOfBoundaryDistance = T(0);
		}
	}

	const T length = pointOffset * direction_;

	if (finiteLineLocation)
	{
		// the location in relation to the finite line is just the 1D position
		// on the line starting at point0 with positive values towards point1

		*finiteLineLocation = length;
	}

	// the projected point lies on the finite line
	return point0_ + direction_ * length;
}

template <typename T>
inline bool FiniteLineT2<T>::intersection(const FiniteLineT2<T>& second, VectorT2<T>& point) const
{
	ocean_assert(isValid() && second.isValid());
	if (!LineT2<T>(point0_, direction_).intersection(LineT2<T>(second.point0_, second.direction_), point))
	{
		// we do not have an intersection on the infinite lines
		return false;
	}

	// now we check whether the intersection points lies within the ranges [point0, point1]

	const T lengthOnThisLine = direction_ * (point - point0_);
	const T lengthOnSecondLine = second.direction_ * (point - second.point0_);

	if (lengthOnThisLine < T(0) || lengthOnSecondLine < T(0) || NumericT<T>::sqr(lengthOnThisLine) > sqrLength() || NumericT<T>::sqr(lengthOnSecondLine) > second.sqrLength())
	{
		return false;
	}

	return true;
}

template <typename T>
inline bool FiniteLineT2<T>::intersection(const LineT2<T>& second, VectorT2<T>& point) const
{
	ocean_assert(isValid() && second.isValid());
	if (!LineT2<T>(point0_, direction_).intersection(second, point))
	{
		// we do not have an intersection on the infinite lines
		return false;
	}

	// now we check whether the intersection points lies within the ranges [point0, point1]

	const T lengthOnThisLine = direction_ * (point - point0_);

	if (lengthOnThisLine < T(0) || NumericT<T>::sqr(lengthOnThisLine) > sqrLength())
	{
		return false;
	}

	return true;
}

template <typename T>
inline bool FiniteLineT2<T>::intersects(const FiniteLineT2<T>& second) const
{
	ocean_assert(isValid() && second.isValid());

	VectorT2<T> dummyPoint;
	return intersection(second, dummyPoint);
}

template <typename T>
inline bool FiniteLineT2<T>::intersects(const LineT2<T>& second) const
{
	ocean_assert(isValid() && second.isValid());

	VectorT2<T> dummyPoint;
	return intersection(second, dummyPoint);
}

template <typename T>
inline bool FiniteLineT2<T>::isParallel(const FiniteLineT2<T>& right) const
{
	ocean_assert(isValid() && right.isValid());

	return direction_ == right.direction_ || direction_ == -right.direction_;
}

template <typename T>
inline bool FiniteLineT2<T>::isCollinear(const FiniteLineT2<T>& right, const T& distanceEpsilon, const T& cosAngleEpsilon) const
{
	ocean_assert(isValid() && right.isValid());
	ocean_assert(NumericT<T>::isInsideRange((T)0, cosAngleEpsilon, (T)1));

	// Distances of the end-points of one line to the other should be within the band defined by +/- epsilon.
	const T squareDistanceEpsilon = distanceEpsilon * distanceEpsilon;
	const bool validDistanceRightToThis = (nearestPointOnInfiniteLine(right.point0()) - right.point0()).sqr() <= squareDistanceEpsilon && (nearestPointOnInfiniteLine(right.point1()) - right.point1()).sqr() <= squareDistanceEpsilon;
	const bool validDistanceThisToRight = (right.nearestPointOnInfiniteLine(point0()) - point0()).sqr() <= squareDistanceEpsilon && (right.nearestPointOnInfiniteLine(point1()) - point1()).sqr() <= squareDistanceEpsilon;

	// Both normalized lines directions should be parallel and pointing into the same or opposing direction
	const T cosAngle = normal() * right.normal();
	const bool validNormalOrientations = NumericT<T>::abs(cosAngle) >= cosAngleEpsilon;

	return validDistanceRightToThis && validDistanceThisToRight && validNormalOrientations;
}

template <typename T>
inline bool FiniteLineT2<T>::isValid() const
{
	return !direction_.isNull();
}

template <typename T>
inline bool FiniteLineT2<T>::isEqual(const FiniteLineT2<T>& line, const T& epsilon) const
{
	ocean_assert(isValid() && line.isValid());

	const T sqrDistance = NumericT<T>::sqr(epsilon);

	return (point0_.sqrDistance(line.point0_) <= sqrDistance && point1_.sqrDistance(line.point1_) <= sqrDistance)
				|| (point0_.sqrDistance(line.point1_) <= sqrDistance && point1_.sqrDistance(line.point0_) <= sqrDistance);
}

template <typename T>
bool FiniteLineT2<T>::operator==(const FiniteLineT2<T>& right) const
{
	return (point0_ == right.point0_ && point1_ == right.point1_)
				|| (point0_ == right.point1_ && point1_ == right.point0_);
}

template <typename T>
inline bool FiniteLineT2<T>::operator!=(const FiniteLineT2<T>& right) const
{
	return !(*this == right);
}

template <typename T>
inline FiniteLineT2<T>::operator bool() const
{
	return isValid();
}

}

#endif // META_OCEAN_MATH_FINITE_LINE_2_H
