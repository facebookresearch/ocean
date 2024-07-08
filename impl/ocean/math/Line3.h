/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_LINE_3_H
#define META_OCEAN_MATH_LINE_3_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Vector3.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class LineT3;

/**
 * Definition of the Line3 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see LineT3
 * @ingroup math
 */
typedef LineT3<Scalar> Line3;

/**
 * Instantiation of the LineT3 template class using a double precision float data type.
 * @see LineT3
 * @ingroup math
 */
typedef LineT3<double> LineD3;

/**
 * Instantiation of the LineT3 template class using a single precision float data type.
 * @see LineT3
 * @ingroup math
 */
typedef LineT3<float> LineF3;

/**
 * Definition of a typename alias for vectors with LineT3 objects.
 * @see LineT3
 * @ingroup math
 */
template <typename T>
using LinesT3 = std::vector<LineT3<T>>;

/**
 * Definition of a vector holding Line3 objects.
 * @see Line3
 * @ingroup math
 */
typedef std::vector<Line3> Lines3;

/**
 * This class implements an infinite line in 3D space.
 * The line is defined by a point lying on the line and a direction vector.<br>
 * The direction vector must not be a zero vector.<br>
 * The length of the vector may be arbitrary, however a unit vector is necessary for most functions.<br>
 * @tparam T Data type used to represent lines
 * @see Line3, LineF3, LineD3, FiniteLine3, FiniteLine3.
 * @ingroup math
 */
template <typename T>
class LineT3
{
	template <typename U> friend class LineT3;

	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates an invalid line.
		 */
		LineT3() = default;

		/**
		 * Creates a line defined by two different intersection points.
		 * @param point Intersection point on the line
		 * @param direction Vector representing the direction of the line, a unit vector might be appropriate
		 */
		LineT3(const VectorT3<T>& point, const VectorT3<T>& direction);

		/**
		 * Copies a line with different data type than T.
		 * @param line The line to copy
		 * @tparam U The data type of the second line
		 */
		template <typename U>
		inline explicit LineT3(const LineT3<U>& line);

		/**
		 * Returns a point on the line.
		 * @return Point on line
		 */
		inline const VectorT3<T>& point() const;

		/**
		 * Returns a point on the line that is defined by a scalar.
		 * The result is determined by point() + direction() * distance;
		 * @param distance The distance to determine the line point for
		 * @return Point on line
		 */
		inline const VectorT3<T> point(const T distance) const;

		/**
		 * Returns the direction of the line.
		 * @return Unit vector
		 */
		inline const VectorT3<T>& direction() const;

		/**
		 * Sets a point of this line.
		 * @param point The point of this line
		 */
		inline void setPoint(const VectorT3<T>& point);

		/**
		 * Sets the direction of this line.
		 * @param direction Vector defining the new direction, a unit vector might be appropriate
		 */
		inline void setDirection(const VectorT3<T>& direction);

		/**
		 * Returns whether a given point is part of the line.
		 * This function needs a unit vector as direction!
		 * @param point The point to check
		 * @return True, if so
		 */
		bool isOnLine(const VectorT3<T>& point) const;

		/**
		 * Returns the distance between the line and a given point.
		 * This function needs a unit vector as direction!
		 * @param point The point to return the distance for
		 * @return Positive distance between point and line, with range [0, infinity)
		 */
		T distance(const VectorT3<T>& point) const;

		/**
		 * Returns the distance between two lines.
		 * This function needs a unit vector as direction!<br>
		 * @param line Second line to calculate the distance for
		 * @return Distance
		 */
		T distance(const LineT3<T>& line) const;

		/**
		 * Returns the square distance between the line and a given point.
		 * This function needs a unit vector as direction!
		 * @param point The point to return the distance for
		 * @return The square distance between point and line, with range [0, infinity)
		 */
		T sqrDistance(const VectorT3<T>& point) const;

		/**
		 * Returns the point on this line nearest to an arbitrary given point.
		 * This function needs a unit vector as direction!<br>
		 * @param point Arbitrary point outside the line
		 * @return Nearest point on the line
		 */
		VectorT3<T> nearestPoint(const VectorT3<T>& point) const;

		/**
		 * Returns the middle of two nearest points for two crossing lines.
		 * This function needs a unit vector as direction!<br>
		 * Both lines must not be parallel.<br>
		 * @param line Second line to calculate the point for
		 * @param middle Nearest point between two lines
		 * @return True, if succeeded
		 */
		bool nearestPoint(const LineT3<T>& line, VectorT3<T>& middle) const;

		/**
		 * Returns the two nearest points for two crossing lines.
		 * Both lines must not be parallel.<br>
		 * This function needs a unit vector as direction!<br>
		 * @param line Second line to calculate the points for
		 * @param first Nearest point on the first line
		 * @param second Nearest point on the second line
		 * @return True, if succeeded
		 */
		bool nearestPoints(const LineT3<T>& line, VectorT3<T>& first, VectorT3<T>& second) const;

		/**
		 * Returns whether two lines are parallel up to a small epsilon.
		 * This function needs a unit vector as direction!
		 * @param right Second line
		 * @return True, if so
		 */
		inline bool isParallel(const LineT3<T>& right) const;

		/**
		 * Returns whether this line and a given vector are parallel up to a small epsilon.
		 * This function needs a unit vector as direction!
		 * @param right Vector to be compared
		 * @return True, if so
		 */
		inline bool isParallel(const VectorT3<T>& right) const;

		/**
		 * Returns whether this line has valid parameters.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this line has a unit vector as direction.
		 * @return True, if so
		 */
		inline bool hasUnitDirection() const;

		/**
		 * Returns whether two line are identical up to a small epsilon.
		 * This function needs a unit vector as direction!
		 * @param right The right line
		 * @return True, if so
		 */
		inline bool operator==(const LineT3<T>& right) const;

		/**
		 * Returns whether two line are identical up to a small epsilon.
		 * This function needs a unit vector as direction!
		 * @param right The right line
		 * @return True, if so
		 */
		inline bool operator!=(const LineT3<T>& right) const;

		/**
		 * Returns whether this line is valid.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// Point on the line.
		VectorT3<T> point_ = VectorT3<T>(0, 0, 0);

		/// Direction of the line.
		VectorT3<T> direction_ = VectorT3<T>(0, 0, 0);
};

template <typename T>
LineT3<T>::LineT3(const VectorT3<T>& first, const VectorT3<T>& direction) :
	point_(first),
	direction_(direction)
{
	ocean_assert(!direction_.isNull());
}

template <typename T>
template <typename U>
inline LineT3<T>::LineT3(const LineT3<U>& line)
{
	point_ = VectorT3<T>(line.point_);
	direction_ = VectorT3<T>(line.direction_);
}

template <typename T>
inline const VectorT3<T>& LineT3<T>::point() const
{
	return point_;
}

template <typename T>
inline const VectorT3<T> LineT3<T>::point(const T distance) const
{
	ocean_assert(isValid());
	return point_ + direction_ * distance;
}

template <typename T>
inline const VectorT3<T>& LineT3<T>::direction() const
{
	return direction_;
}

template <typename T>
inline void LineT3<T>::setPoint(const VectorT3<T>& point)
{
	point_ = point;
}

template <typename T>
inline void LineT3<T>::setDirection(const VectorT3<T>& direction)
{
	ocean_assert(NumericT<T>::isEqual(direction.length(), T(1.0)));
	direction_ = direction;
}

template <typename T>
bool LineT3<T>::isValid() const
{
	return !direction_.isNull();
}

template <typename T>
inline bool LineT3<T>::hasUnitDirection() const
{
	return NumericT<T>::isEqual(direction_.length(), T(1.0));
}

template <typename T>
inline bool LineT3<T>::isParallel(const LineT3<T>& right) const
{
	ocean_assert(isValid() && right.isValid());
	ocean_assert(hasUnitDirection() && right.hasUnitDirection());

	const T scalarProduct = direction_ * right.direction_;

	return NumericT<T>::isEqual(NumericT<T>::abs(scalarProduct), T(1.0));
}

template <typename T>
inline bool LineT3<T>::isParallel(const VectorT3<T>& right) const
{
	ocean_assert(isValid());
	ocean_assert(hasUnitDirection() && NumericT<T>::isEqual(right.length(), T(1.0)));

	const T scalarProduct = direction_ * right;

	return NumericT<T>::isEqual(NumericT<T>::abs(scalarProduct), T(1.0));
}

template <typename T>
inline bool LineT3<T>::operator==(const LineT3<T>& right) const
{
	ocean_assert(isValid() && right.isValid());

	return isParallel(right) && isOnLine(right.point());
}

template <typename T>
inline bool LineT3<T>::operator!=(const LineT3<T>& right) const
{
	return !(*this == right);
}

template <typename T>
inline LineT3<T>::operator bool() const
{
	return isValid();
}

template <typename T>
bool LineT3<T>::isOnLine(const VectorT3<T>& point) const
{
	ocean_assert(isValid());
	ocean_assert(hasUnitDirection());

	const VectorT3<T> offset(point - point_);
	const T length = offset.length();

	if (NumericT<T>::isEqualEps(length))
	{
		return true;
	}

#ifdef OCEAN_DEBUG
	if (!std::is_same<T, float>::value)
	{
		ocean_assert(NumericT<T>::isEqual(NumericT<T>::abs((offset / length) * direction_), T(1.0))
					== NumericT<T>::isEqual(NumericT<T>::abs(offset * direction_), length, NumericT<T>::eps() * length));
	}
#endif

	// we explicitly adjust the epsilon by the length of the offset vector ensuring that the result is still correct for long vectors (short vectors would have been caught before)
	return NumericT<T>::isEqual(NumericT<T>::abs(offset * direction_), length, NumericT<T>::eps() * length);
}

template <>
inline bool LineT3<float>::isOnLine(const VectorT3<float>& point) const
{
	ocean_assert(isValid());
	ocean_assert(hasUnitDirection());

	const VectorT3<float> offset(point - point_);
	const float length = offset.length();

	if (NumericT<float>::isEqualEps(length))
	{
		return true;
	}

	if (length > 1.0f)
	{
		// we explicitly adjust the epsilon by the length of the offset vector ensuring that the result is still correct for long vectors (short vectors would have been caught before)
		return NumericT<float>::isEqual(NumericT<float>::abs(offset * direction_), length, NumericT<float>::eps() * length);
	}

	return NumericT<float>::isEqual(NumericT<float>::abs(offset * direction_), length, NumericT<float>::eps());
}

template <typename T>
T LineT3<T>::distance(const VectorT3<T>& point) const
{
	ocean_assert(isValid());
	ocean_assert(hasUnitDirection());

	const VectorT3<T> pointOnLine(nearestPoint(point));

	return (pointOnLine - point).length();
}

template <typename T>
T LineT3<T>::distance(const LineT3<T>& line) const
{
	// idea: creating a plane which intersect the second line and is parallel to the first line
	// the distance is the projection of the vector between the two base point onto the plane normal

	ocean_assert(isValid() && line.isValid());
	ocean_assert(hasUnitDirection() && line.hasUnitDirection());

	const VectorT3<T> offset(point_ - line.point_);

	// if the base points of the two lines are identical
	if (NumericT<T>::isEqualEps(offset.sqr()))
	{
		return T(0.0);
	}

	if (isParallel(line))
	{
		return (line.point_ - point_ + direction_ * (direction_ * offset)).length();
	}

	// plane normal
	const VectorT3<T> normal(direction_.cross(line.direction_).normalizedOrZero());

	// projection of point offset onto plane normal
	return NumericT<T>::abs(offset * normal);
}

template <typename T>
T LineT3<T>::sqrDistance(const VectorT3<T>& point) const
{
	ocean_assert(isValid());
	ocean_assert(hasUnitDirection());

	const VectorT3<T> pointOnLine(nearestPoint(point));

	return (pointOnLine - point).sqr();
}

template <typename T>
VectorT3<T> LineT3<T>::nearestPoint(const VectorT3<T>& point) const
{
	ocean_assert(isValid());
	ocean_assert(hasUnitDirection());

	const VectorT3<T> offset(point - point_);

	return point_ + direction_ * (direction_ * offset);
}

template <typename T>
bool LineT3<T>::nearestPoint(const LineT3<T>& line, VectorT3<T>& middle) const
{
	ocean_assert(isValid() && line.isValid());
	ocean_assert(hasUnitDirection() && line.hasUnitDirection());

	VectorT3<T> first, second;
	if (nearestPoints(line, first, second))
	{
		middle = (first + second) * T(0.5);
		return true;
	}

	return false;
}

template <typename T>
bool LineT3<T>::nearestPoints(const LineT3<T>& line, VectorT3<T>& first, VectorT3<T>& second) const
{
	ocean_assert(isValid() && line.isValid());
	ocean_assert(hasUnitDirection() && line.hasUnitDirection());

	if (isParallel(line))
	{
		return false;
	}

	const VectorT3<T> d = line.direction_ - direction_ * (direction_ * line.direction_);
	const VectorT3<T> p = line.point_ - point_ + direction_ * (direction_ * point_);

	const T denominator = d.sqr();

	if (NumericT<T>::isEqualEps(denominator))
	{
		return false;
	}

	const T factor = - (p * d) / denominator;

	second = line.point_ + line.direction_ * factor;
	first = nearestPoint(second);

	return true;
}

}

#endif // META_OCEAN_MATH_LINE_3_H
