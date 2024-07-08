/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_LINE_2_H
#define META_OCEAN_MATH_LINE_2_H

#include "ocean/math/Math.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class LineT2;

/**
 * Definition of the Line2 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see LineT2
 * @ingroup math
 */
typedef LineT2<Scalar> Line2;

/**
 * Instantiation of the LineT2 template class using a double precision float data type.
 * @see LineT2
 * @ingroup math
 */
typedef LineT2<double> LineD2;

/**
 * Instantiation of the LineT2 template class using a single precision float data type.
 * @see LineT2
 * @ingroup math
 */
typedef LineT2<float> LineF2;

/**
 * Definition of a typename alias for vectors with LineT2 objects.
 * @see LineT2
 * @ingroup math
 */
template <typename T>
using LinesT2 = std::vector<LineT2<T>>;

/**
 * Definition of a vector holding Line2 objects.
 * @see Line2
 * @ingroup math
 */
typedef std::vector<Line2> Lines2;

/**
 * Definition of a vector holding Line2 objects with single precision float data type.
 * @see LineF2
 * @ingroup math
 */
typedef std::vector<LineF2> LinesF2;

/**
 * Definition of a vector holding Line2 objects with double precision float data type.
 * @see LineD2
 * @ingroup math
 */
typedef std::vector<LineD2> LinesD2;

/**
 * This class implements an infinite line in 2D space.
 * The implementation is realized by an (explicit) parametric equation using a point on the line and the direction of the line.<br>
 * However, the line support conversion function to receive also an implicit equation.
 * @tparam T Data type used to represent lines
 * @see Line2, LineF2, LineD2, FiniteLine2, FiniteLine.
 * @ingroup math
 */
template <typename T>
class LineT2
{
	template <typename U> friend class LineT2;

	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates an invalid line.
		 */
		LineT2();

		/**
		 * Creates a line defined by a point on the line and a line direction.
		 * @param point Point on the line
		 * @param direction Vector representing the direction of the line, a unit vector might be appropriate
		 */
		LineT2(const VectorT2<T>& point, const VectorT2<T>& direction);

		/**
		 * Creates a new line object by given (implicit) three-parameter representation of the line.
		 * The representation is given by the normal and the distance parameter of the line so that for a point (x, y) lying on the plane the following holds: [nx, ny, d] * [x, y, 1] == 0.<br>
		 * The normal must be defined as unit vector.
		 * @param parameters The three parameters given as 3D vector
		 */
		explicit LineT2(const VectorT3<T>& parameters);

		/**
		 * Creates a new line object by a given angle of the line's normal and the distance of the line to the origin.
		 * The distance values is equal to the distance parameter of a (implicit) three-parameter representation of a line where the other two parameters define the normal of the plane.
		 * @param angle The angle of the line's normal, with normal = [cos(angle), sin(angle)], in radian
		 * @param distance The signed distance of the line to the origin, with range (-infinity, infinity)
		 * @see decomposeAngleDistance().
		 */
		LineT2(const T angle, const T distance);

		/**
		 * Copies a line with different data type than T.
		 * @param line The line to copy
		 * @tparam U The data type of the second line
		 */
		template <typename U>
		inline explicit LineT2(const LineT2<U>& line);

		/**
		 * Returns a point on the line.
		 * @return Point on line
		 */
		inline const VectorT2<T>& point() const;

		/**
		 * Returns a point on the line that is defined by a scalar.
		 * The result is determined by point() + direction() * distance;
		 * @param distance Distance to determine the line point for
		 * @return Point on line
		 */
		inline VectorT2<T> point(const T distance) const;

		/**
		 * Returns the direction of the line.
		 * @return Direction vector
		 */
		inline const VectorT2<T>& direction() const;

		/**
		 * Returns a normal to the direction of this line.
		 * The 2D cross product between the resulting normal and the direction of this line will be positive.
		 * @return The resulting normal
		 */
		inline VectorT2<T> normal() const;

		/**
		 * Calculates the angle of the line's normal and the corresponding distance of this line to the origin.
		 * The resulting distance parameter is equivalent to the distance parameter of a (implicit) three-parameter representation of this line where the other two parameters define the normal of the plane.<br>
		 * @param angle Resulting angle of the line's normal which is defined by atan(normal.y() / normal.x()); with normal = [cos(angle), sin(angle)], in radian
		 * @param distance Resulting signed distance between line and origin, is positive if the normal points towards the origin and negative if the normal points away from the origin
		 */
		inline void decomposeAngleDistance(T& angle, T& distance) const;

		/**
		 * Calculates the (implicit) three-parameters representation of this line composed of the line's normal and a distance parameter (nx, ny, d).
		 * For a point (x, y) lying on the plane the following holds: [nx, ny, d] * [x, y, 1] == 0.
		 * @param forcePositiveDistance True, to force a positive distance values; False, to accept positive and negative distance values
		 * @return The resulting three parameter representation
		 */
		inline VectorT3<T> decomposeNormalDistance(const bool forcePositiveDistance = false) const;

		/**
		 * Returns whether a given point is part of the line.
		 * This function needs a unit vector as direction!
		 * @param point Point to check
		 * @return True, if so
		 */
		bool isOnLine(const VectorT2<T>& point) const;

		/**
		 * Check if a point is in the left half-plane of the direction vector of a line
		 * A point @c p is located on the left side of a line, if the cross product of the direction of the line, @c d, and the vector pointing from starting point of the line, @c s, to the point @c p is positive: (d x (p - s)) > 0.
		 * It's on the line the cross product is zero and in the right half-plane it is negative.
		 * @note Keep in mind that if the point is not in the left half-plane, it doesn't necessarily mean that it's in the right half-plane because it could just as well be located on the line.
		 * @param point Point to check
		 * @return True, if the point is in the left half-plane
		 */
		bool isLeftOfLine(const VectorT2<T>& point) const;

		/**
		 * Returns the distance between the line and a given point.
		 * This function needs a unit vector as direction!
		 * @param point Point to return the distance for
		 * @return The always positive distance between point and line, with range [0, infinity)
		 */
		T distance(const VectorT2<T>& point) const;

		/**
		 * Returns the square distance between the line and a given point.
		 * @param point Point to return the distance for
		 * @return Square distance between point and line
		 */
		T sqrDistance(const VectorT2<T>& point) const;

		/**
		 * Returns the point on this line nearest to an arbitrary given point.
		 * This function needs a unit vector as direction!
		 * @param point Arbitrary point outside the line
		 * @return Nearest point on the line
		 */
		VectorT2<T> nearestPoint(const VectorT2<T>& point) const;

		/**
		 * Returns the unique intersection point of two lines.
		 * Two identical lines do not have one unique intersection point, so that this function will return 'false' in such a case.
		 * This function needs a unit vector as direction!
		 * @param right Right line for intersection calculation
		 * @param point Resulting intersection pointer
		 * @return True, if both lines are not parallel
		 */
		bool intersection(const LineT2<T>& right, VectorT2<T>& point) const;

		/**
		 * Returns whether two lines are parallel up to a small epsilon.
		 * This function needs a unit vector as direction!
		 * @param right Second line
		 * @return True, if so
		 */
		bool isParallel(const LineT2<T>& right) const;

		/**
		 * Returns whether this line has valid parameters.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns whether this line has a unit vector as direction.
		 * @return True, if so
		 */
		inline bool hasUnitDirection() const;

		/**
		 * Returns whether two line are identical up to a small epsilon.
		 * @param right Right line
		 * @return True, if so
		 */
		bool operator==(const LineT2<T>& right) const;

		/**
		 * Returns whether two line are identical up to a small epsilon.
		 * @param right Right line
		 * @return True, if so
		 */
		inline bool operator!=(const LineT2<T>& right) const;

		/**
		 * Returns whether this line is valid.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Fits a line to a set of given image points by application of the least square measure.
		 * @param points The points for which the best fitting line is requested, must be valid
		 * @param size The number of given points, with range [2, infinity)
		 * @param line The resulting line
		 * @return True, if succeeded
		 */
		static inline bool fitLineLeastSquare(const VectorT2<T>* points, const size_t size, LineT2<T>& line);

	protected:

		/// Point on the line.
		VectorT2<T> linePoint;

		/// Direction of the line.
		VectorT2<T> lineDirection;
};

template <typename T>
LineT2<T>::LineT2() :
	linePoint(T(0), T(0)),
	lineDirection(T(0), T(0))
{
	// nothing to do here
}

template <typename T>
LineT2<T>::LineT2(const VectorT2<T>& point, const VectorT2<T>& direction) :
	linePoint(point),
	lineDirection(direction)
{
	ocean_assert(isValid());
}

template <typename T>
LineT2<T>::LineT2(const VectorT3<T>& parameters)
{
	const VectorT2<T> normal(parameters.x(), parameters.y());
	ocean_assert(NumericT<T>::isEqual(normal.length(), 1));

	lineDirection = normal.perpendicular();
	ocean_assert(NumericT<T>::isEqual(lineDirection.length(), 1));

	linePoint = normal * -parameters.z();
	ocean_assert(isValid());
}

template <typename T>
LineT2<T>::LineT2(const T angle, const T distance)
{
	const VectorT2<T> normal(NumericT<T>::cos(angle), NumericT<T>::sin(angle));
	ocean_assert(NumericT<T>::isEqual(normal.length(), 1));

	lineDirection = normal.perpendicular();
	ocean_assert(NumericT<T>::isEqual(lineDirection.length(), 1));

	linePoint = normal * -distance;
	ocean_assert(isValid());
}

template <typename T>
template <typename U>
inline LineT2<T>::LineT2(const LineT2<U>& line)
{
	linePoint = VectorT2<T>(line.linePoint);
	lineDirection = VectorT2<T>(line.lineDirection);
}

template <typename T>
inline const VectorT2<T>& LineT2<T>::point() const
{
	return linePoint;
}

template <typename T>
inline VectorT2<T> LineT2<T>::point(const T distance) const
{
	ocean_assert(isValid());
	return linePoint + lineDirection * distance;
}

template <typename T>
inline const VectorT2<T>& LineT2<T>::direction() const
{
	return lineDirection;
}

template <typename T>
inline VectorT2<T> LineT2<T>::normal() const
{
	ocean_assert(isValid());
	VectorT2<T> result(-lineDirection.perpendicular());

	// as this line may have a direction which is not a unit vector we have to normalize our normal explicitly
	result.normalize();

	return result;
}

template <typename T>
inline void LineT2<T>::decomposeAngleDistance(T& angle, T& distance) const
{
	ocean_assert(isValid());

	const VectorT2<T> normalVector(normal());

	angle = NumericT<T>::atan2(normalVector.y(), normalVector.x());
	distance = -normalVector * linePoint;

	// ensure that the absolute distance is correct
	ocean_assert_accuracy((std::is_same<T, float>::value) || NumericT<T>::isEqual(NumericT<T>::abs(distance), this->distance(VectorT2<T>(0, 0))));

	// ensure that distance is valid for the three-parameter representation of our line
	ocean_assert_accuracy(NumericT<T>::isEqualEps(VectorT3<T>(normalVector.x(), normalVector.y(), distance) * VectorT3<T>(linePoint.x(), linePoint.y(), 1)));
}

template <typename T>
inline VectorT3<T> LineT2<T>::decomposeNormalDistance(const bool forcePositiveDistance) const
{
	ocean_assert(isValid());

	const VectorT2<T> normalVector(normal());
	const T distance = -normalVector * linePoint;

	// ensure that the absolute distance is correct
	ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isWeakEqual(NumericT<T>::abs(distance), this->distance(VectorT2<T>(0, 0))));

	// ensure that distance is valid for the three-parameter representation of our line
	ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isWeakEqualEps(VectorT3<T>(normalVector.x(), normalVector.y(), distance) * VectorT3<T>(linePoint.x(), linePoint.y(), 1)));

	if (forcePositiveDistance && distance < T(0))
	{
		return VectorT3<T>(-normalVector.x(), -normalVector.y(), -distance);
	}

	return VectorT3<T>(normalVector.x(), normalVector.y(), distance);
}

template <typename T>
inline bool LineT2<T>::isValid() const
{
	return !lineDirection.isNull();
}

template <typename T>
inline bool LineT2<T>::hasUnitDirection() const
{
	return NumericT<T>::isEqual(lineDirection.length(), T(1.0));
}

template <typename T>
inline bool LineT2<T>::operator!=(const LineT2<T>& right) const
{
	return !(*this == right);
}

template <typename T>
inline LineT2<T>::operator bool() const
{
	return isValid();
}

template <typename T>
bool LineT2<T>::isOnLine(const VectorT2<T>& point) const
{
	ocean_assert(hasUnitDirection());
	ocean_assert(isValid());

	const VectorT2<T> offset(point - linePoint);
	const T length = offset.length();

	if (NumericT<T>::isEqualEps(length))
	{
		return true;
	}

#ifdef OCEAN_DEBUG
	if (!std::is_same<T, float>::value)
	{
		ocean_assert(NumericT<T>::isEqual(NumericT<T>::abs((offset / length) * lineDirection), T(1.0))
					== NumericT<T>::isEqual(NumericT<T>::abs(offset * lineDirection), length, NumericT<T>::eps() * length));
	}
#endif

	// we explicitly adjust the epsilon by the length of the offset vector ensuring that the result is still correct for long vectors (short vectors would have been caught before)
	return NumericT<T>::isEqual(NumericT<T>::abs(offset * lineDirection), length, NumericT<T>::eps() * length);
}

template <typename T>
bool LineT2<T>::isLeftOfLine(const VectorT2<T>& otherPoint) const
{
	ocean_assert(isValid());

	return (direction().cross(otherPoint - point())) > NumericT<T>::weakEps();
}

template <>
inline bool LineT2<float>::isOnLine(const VectorT2<float>& point) const
{
	ocean_assert(hasUnitDirection());
	ocean_assert(isValid());

	const VectorT2<float> offset(point - linePoint);
	const float length = offset.length();

	if (NumericT<float>::isEqualEps(length))
	{
		return true;
	}

	if (length > 1.0f)
	{
		// we explicitly adjust the epsilon by the length of the offset vector ensuring that the result is still correct for long vectors (short vectors would have been caught before)
		return NumericT<float>::isEqual(NumericT<float>::abs(offset * lineDirection), length, NumericT<float>::eps() * length);
	}

	return NumericT<float>::isEqual(NumericT<float>::abs(offset * lineDirection), length, NumericT<float>::eps());
}

template <typename T>
T LineT2<T>::distance(const VectorT2<T>& point) const
{
	ocean_assert(hasUnitDirection());
	ocean_assert(isValid());

	const VectorT2<T> pointOnLine(nearestPoint(point));
	ocean_assert((!std::is_same<T, double>::value) || isOnLine(pointOnLine));

	return (pointOnLine - point).length();
}

template <typename T>
T LineT2<T>::sqrDistance(const VectorT2<T>& point) const
{
	ocean_assert(hasUnitDirection());
	ocean_assert(isValid());

	const VectorT2<T> pointOnLine(nearestPoint(point));

	return (pointOnLine - point).sqr();
}

template <typename T>
VectorT2<T> LineT2<T>::nearestPoint(const VectorT2<T>& point) const
{
	ocean_assert(hasUnitDirection());
	ocean_assert(isValid());

	const VectorT2<T> offset(point - linePoint);

	return linePoint + lineDirection * (lineDirection * offset);
}

template <typename T>
bool LineT2<T>::intersection(const LineT2<T>& right, VectorT2<T>& point) const
{
	ocean_assert(hasUnitDirection());

	if (isParallel(right))
	{
		return false;
	}

	// direction from the right line to this line
	const VectorT2<T> normal(nearestPoint(right.linePoint) - right.linePoint);

	// smallest distance from the right line to this line
	const T normalLength = normal.length();

	// if the point of the right line is already on this line
	if (NumericT<T>::isEqualEps(normalLength))
	{
		point = right.linePoint;
		return true;
	}

	const T cosValue = right.lineDirection * (normal / normalLength);

	if (NumericT<T>::isEqualEps(cosValue))
	{
		// both lines are too parallel, so that we wrote for no (unique) intersection
		return false;
	}

	const T offset = normalLength / cosValue;

	point = right.linePoint + right.lineDirection * offset;

	if constexpr (std::is_same<T, double>::value)
	{
		ocean_assert_accuracy(right.isOnLine(point));
		ocean_assert_accuracy(isOnLine(point));
	}

	return true;
}

template <typename T>
bool LineT2<T>::isParallel(const LineT2<T>& right) const
{
	ocean_assert(hasUnitDirection());
	ocean_assert(isValid() && right.isValid());

	return lineDirection == right.lineDirection || lineDirection == -right.lineDirection;
}

template <typename T>
bool LineT2<T>::operator==(const LineT2<T>& right) const
{
	ocean_assert(isValid() && right.isValid());
	return isParallel(right) && isOnLine(right.point());
}

template <typename T>
bool LineT2<T>::fitLineLeastSquare(const VectorT2<T>* points, const size_t size, LineT2<T>& line)
{
	static_assert(std::is_same<float, T>::value || std::is_same<double, T>::value, "Type T can be either float or double.");
	ocean_assert(points && size >= 2);

	// average values
	T x(0);
	T y(0);

	// variance values
	T x2(0);
	T y2(0);

	// covariance
	T xy(0);

	for (size_t i = 0; i < size; i++)
	{
		const VectorT2<T>& point = points[i];

		x += point[0];
		y += point[1];

		x2 += NumericT<T>::sqr(point[0]);
		y2 += NumericT<T>::sqr(point[1]);

		xy += point[0] * point[1];
	}

	ocean_assert(size != 0);
	const T invSize = T(1) / T(size);

	x *= invSize;
	y *= invSize;
	x2 *= invSize;
	y2 *= invSize;
	xy *= invSize;

	const T xSqr = x * x;
	const T ySqr = y * y;

	const T nominator = 2 * (xy - x * y);
	const T denominator = (x2 - xSqr) - (y2 - ySqr);

	if (NumericT<T>::isEqualEps(denominator) && NumericT<T>::isEqualEps(nominator))
	{
		return false;
	}

	const Scalar angleDirection(T(0.5) * NumericT<T>::atan2(nominator, denominator));
	const VectorT2<T> direction(NumericT<T>::cos(angleDirection), NumericT<T>::sin(angleDirection));

	const VectorT2<T> linePoint(x, y);

	line = LineT2<T>(linePoint, direction);

	return true;
}

} // namespace Ocean

#endif // META_OCEAN_MATH_LINE_2_H
