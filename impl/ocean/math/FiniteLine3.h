/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_FINITE_LINE_3_H
#define META_OCEAN_MATH_FINITE_LINE_3_H

#include "ocean/math/Math.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Vector3.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class FiniteLineT3;

/**
 * Definition of the FiniteLine3 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see FiniteLineT3
 * @ingroup math
 */
typedef FiniteLineT3<Scalar> FiniteLine3;

/**
 * Instantiation of the LineT3 template class using a double precision float data type.
 * @see FiniteLineT3
 * @ingroup math
 */
typedef FiniteLineT3<double> FiniteLineD3;

/**
 * Instantiation of the LineT3 template class using a single precision float data type.
 * @see FiniteLineT3
 * @ingroup math
 */
typedef FiniteLineT3<float> FiniteLineF3;

/**
 * Definition of a typename alias for vectors with FiniteLineT3 objects.
 * @see FiniteLineT3
 * @ingroup math
 */
template <typename T>
using FiniteLinesT3 = std::vector<FiniteLineT3<T>>;

/**
 * Definition of a vector holding FiniteLine3 objects.
 * @see Line3
 * @ingroup math
 */
typedef std::vector<FiniteLine3> FiniteLines3;

/**
 * This class implements an finite line in 3D space.
 * The finite line object is invalid if both end points of the line object are identical.<br>
 * @tparam T Data type used to represent lines
 * @see FiniteLine3, FiniteLineF3, FiniteLineD3, Line3, Line3
 * @ingroup math
 */
template <typename T>
class FiniteLineT3
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
		FiniteLineT3();

		/**
		 * Creates a finite line defined by two end points of the line.
		 * @param point0 First end point of the line
		 * @param point1 Second end point of the line, must be different from point0 to create a valid line, otherwise the line is invalid
		 */
		FiniteLineT3(const VectorT3<T>& point0, const VectorT3<T>& point1);

		/**
		 * Copies a line with different data type than T.
		 * @param line The line to copy
		 * @tparam U The data type of the second line
		 */
		template <typename U>
		inline explicit FiniteLineT3(const FiniteLineT3<U>& line);

		/**
		 * Returns the first end point of the line.
		 * @return First end point of the line
		 */
		inline const VectorT3<T>& point0() const;

		/**
		 * Returns the second end point of the line.
		 * @return Second end point of the line
		 */
		inline const VectorT3<T>& point1() const;

		/**
		 * Returns the first or second end point of the line.
		 * @param index The index of the point to be returned, with range [0, 1]
		 * @return First or second end point of the line
		 */
		inline const VectorT3<T>& point(const unsigned int index) const;

		/**
		 * Returns the midpoint of the line.
		 * @return Midpoint point of the line
		 */
		inline VectorT3<T> midpoint() const;

		/**
		 * Returns the direction of the line: normalize(point1() - point0())
		 * @return Direction vector with unit length, a zero vector if the line is invalid
		 * @see isValid().
		 */
		inline const VectorT3<T>& direction() const;

		/**
		 * Returns whether a given point is part of the finite line.
		 * @param point Point to check
		 * @return True, if so
		 */
		inline bool isOnLine(const VectorT3<T>& point) const;

		/**
		 * Returns the distance between the line and a given point.
		 * This function needs a unit vector as direction!
		 * @param point Point to return the distance for
		 * @return Distance between point and line
		 */
		inline T distance(const VectorT3<T>& point) const;

		/**
		 * Returns the square distance between the line and a given point.
		 * @param point Point to return the distance for
		 * @return Square distance between point and line
		 */
		inline T sqrDistance(const VectorT3<T>& point) const;

		/**
		 * Returns the point on this line nearest to an arbitrary given point.
		 * This function needs a unit vector as direction!
		 * @param point Arbitrary point outside the line
		 * @return Nearest point on the line
		 */
		VectorT3<T> nearestPoint(const VectorT3<T>& point) const;

		/**
		 * Returns the intersection pointer of two finite lines.
		 * @param right Right line for intersection calculation
		 * @param point Resulting intersection pointer
		 * @return True, if both lines have a common intersection point
		 */
		inline bool intersection(const FiniteLineT3<T>& right, VectorT3<T>& point) const;

		/**
		 * Returns whether two lines are parallel up to a small epsilon.
		 * @param right Second line
		 * @return True, if so
		 */
		inline bool isParallel(const FiniteLineT3<T>& right) const;

		/**
		 * Returns whether this line has valid parameters.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether two line are identical up to a small epsilon.
		 * Two finite lines are identical if both lines have the same endpoint (independent of the order of the end points).<br>
		 * @param right Right line
		 * @return True, if so
		 */
		inline bool operator==(const FiniteLineT3<T>& right) const;

		/**
		 * Returns whether two line are identical up to a small epsilon.
		 * @param right Right line
		 * @return True, if so
		 */
		inline bool operator!=(const FiniteLineT3<T>& right) const;

		/**
		 * Returns whether this line is valid.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// First end point of the line.
		VectorT3<T> linePoint0;

		/// Second end point of the line.
		VectorT3<T> linePoint1;

		/// Direction of the line with unit length, if the object holds valid parameters.
		VectorT3<T> lineDirection;
};

template <typename T>
FiniteLineT3<T>::FiniteLineT3() :
	linePoint0(T(0), T(0), T(0)),
	linePoint1(T(0), T(0), T(0)),
	lineDirection(T(0), T(0), T(0))
{
	// nothing to do here
}

template <typename T>
FiniteLineT3<T>::FiniteLineT3(const VectorT3<T>& point0, const VectorT3<T>& point1) :
	linePoint0(point0),
	linePoint1(point1),
	lineDirection((point1 - point0).normalizedOrZero())
{
	// nothing to do here
}

template <typename T>
template <typename U>
inline FiniteLineT3<T>::FiniteLineT3(const FiniteLineT3<U>& line)
{
	linePoint0 = VectorT3<T>(line.linePoint0);
	linePoint1 = VectorT3<T>(line.linePoint1);
	lineDirection = VectorT3<T>(line.lineDirection);
}

template <typename T>
inline const VectorT3<T>& FiniteLineT3<T>::point0() const
{
	return linePoint0;
}

template <typename T>
inline const VectorT3<T>& FiniteLineT3<T>::point1() const
{
	return linePoint1;
}

template <typename T>
inline const VectorT3<T>& FiniteLineT3<T>::point(const unsigned int index) const
{
	ocean_assert(index <= 1u);

	if (index == 0u)
	{
		return linePoint0;
	}
	else
	{
		return linePoint1;
	}
}

template <typename T>
inline VectorT3<T> FiniteLineT3<T>::midpoint() const
{
  return (linePoint0 + linePoint1) * T(0.5);
}

template <typename T>
inline const VectorT3<T>& FiniteLineT3<T>::direction() const
{
	return lineDirection;
}

template <typename T>
inline bool FiniteLineT3<T>::isOnLine(const VectorT3<T>& point) const
{
	ocean_assert(isValid());

	return NumericT<T>::isEqualEps(sqrDistance(point));
}

template <typename T>
inline T FiniteLineT3<T>::distance(const VectorT3<T>& point) const
{
	ocean_assert(isValid());

	return NumericT<T>::sqrt(sqrDistance(point));
}

template <typename T>
inline T FiniteLineT3<T>::sqrDistance(const VectorT3<T>& point) const
{
	ocean_assert(isValid());

	return nearestPoint(point).sqrDistance(point);
}

template <typename T>
VectorT3<T> FiniteLineT3<T>::nearestPoint(const VectorT3<T>& point) const
{
	ocean_assert(isValid());

	const VectorT3<T> lineOffset(linePoint1 - linePoint0);
	const VectorT3<T> pointOffset(point - linePoint0);

	const Scalar dotProduct = lineOffset * pointOffset;

	// the projected point does not lie on the finite line (before the first end point)
	if (dotProduct <= 0)
	{
		return linePoint0;
	}

	// the projected point does not lie on the finite line (behind the second end point)
	if (dotProduct >= lineOffset.sqr())
	{
		return linePoint1;
	}

	// the projected point lies on the finite line
	return linePoint0 + lineDirection * (pointOffset * lineDirection);
}

template <typename T>
inline bool FiniteLineT3<T>::intersection(const FiniteLineT3<T>& right, VectorT3<T>& point) const
{
	return LineT3<T>(linePoint0, lineDirection).intersection(LineT3<T>(right.linePoint0, right.lineDirection), point) && isOnLine(point);
}

template <typename T>
inline bool FiniteLineT3<T>::isParallel(const FiniteLineT3<T>& right) const
{
	ocean_assert(isValid() && right.isValid());

	return lineDirection == right.lineDirection || lineDirection == -right.lineDirection;
}

template <typename T>
inline bool FiniteLineT3<T>::isValid() const
{
	return !lineDirection.isNull();
}

template <typename T>
bool FiniteLineT3<T>::operator==(const FiniteLineT3<T>& right) const
{
	return (linePoint0 == right.linePoint0 && linePoint1 == right.linePoint1)
				|| (linePoint0 == right.linePoint1 && linePoint1 == right.linePoint0);
}

template <typename T>
inline bool FiniteLineT3<T>::operator!=(const FiniteLineT3<T>& right) const
{
	return !(*this == right);
}

template <typename T>
inline FiniteLineT3<T>::operator bool() const
{
	return isValid();
}

}

#endif // META_OCEAN_MATH_FINITE_LINE_3_H
