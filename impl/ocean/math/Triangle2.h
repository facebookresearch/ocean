/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_TRIANGLE_2_H
#define META_OCEAN_MATH_TRIANGLE_2_H

#include "ocean/math/Math.h"
#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Triangle.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class TriangleT2;

/**
 * Definition of the Triangle2 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see TriangleT2
 * @ingroup math
 */
typedef TriangleT2<Scalar> Triangle2;

/**
 * Instantiation of the TriangleT2 template class using a double precision float data type.
 * @see TriangleT2
 * @ingroup math
 */
typedef TriangleT2<double> TriangleD2;

/**
 * Instantiation of the TriangleT2 template class using a single precision float data type.
 * @see TriangleT2
 * @ingroup math
 */
typedef TriangleT2<float> TriangleF2;

/**
 * Definition of a typename alias for vectors with TriangleT2 objects.
 * @see TriangleT2
 * @ingroup math
 */
template <typename T>
using TrianglesT2 = std::vector<TriangleT2<T>>;

/**
 * Definition of a vector holding 2D triangles.
 * @see Triangle2
 * @ingroup math
 */
typedef std::vector<Triangle2> Triangles2;

/**
 * Definition of a vector holding 2D triangles with single precision float data type.
 * @see Triangle2
 * @ingroup math
 */
typedef std::vector<TriangleF2> TrianglesF2;

/**
 * Definition of a vector holding 2D triangles with double precision float data type.
 * @see Triangle2
 * @ingroup math
 */
typedef std::vector<TriangleD2> TrianglesD2;

/**
 * This class implements a 2D triangle with Cartesian coordinates.
 * @tparam T Data type used to represent coordinates
 * @see TriangleF2, TriangleD2, TriangleT3.
 * @ingroup math
 */
template <typename T>
class TriangleT2 : public TriangleT<T>
{
	public:

		/**
		 * Creates a new 2D triangle object with default parameters.
		 */
		TriangleT2() = default;

		/**
		 * Creates a new 2D triangle object by three corner positions.
		 * @param point0 First corner position
		 * @param point1 Second corner position
		 * @param point2 Third corner position
		 */
		inline TriangleT2(const VectorT2<T>& point0, const VectorT2<T>& point1, const VectorT2<T>& point2);

		/**
		 * Returns the first triangle corner.
		 * @return First triangle corner
		 */
		inline const VectorT2<T>& point0() const;

		/**
		 * Returns the second triangle corner.
		 * @return Second triangle corner
		 */
		inline const VectorT2<T>& point1() const;

		/**
		 * Returns the third triangle corner.
		 * @return Third triangle corner
		 */
		inline const VectorT2<T>& point2() const;

		/**
		 * Returns the square distance between point0 and point1.
		 * @return Square distance
		 */
		inline T sqrDistance01() const;

		/**
		 * Returns the square distance between point0 and point2.
		 * @return Square distance
		 */
		inline T sqrDistance02() const;

		/**
		 * Returns the square distance between point1 and point2.
		 * @return Square distance
		 */
		inline T sqrDistance12() const;

		/**
		 * Returns the most left position of this triangle.
		 * @return Left position
		 */
		inline T left() const;

		/**
		 * Returns the most top position of this triangle.
		 * @return Top position
		 */
		inline T top() const;

		/**
		 * Returns the most right position of this triangle.
		 * @return Right position
		 */
		inline T right() const;

		/**
		 * Returns the most bottom position of this triangle.
		 * @return Bottom position
		 */
		inline T bottom() const;

		/**
		 * Returns the area of this triangle.
		 * @return Triangle area
		 * @see area2().
		 */
		inline T area() const;

		/**
		 * Returns the square area of this triangle.
		 * @return Triangle area
		 * @see area().
		 */
		inline T area2() const;

		/**
		 * Calculates the three angle cosine values of the three triangle corners.
		 * Beware: Make sure that this triangle is valid before!
		 * @param cosine0 Resulting angle corresponding to point0
		 * @param cosine1 Resulting angle corresponding to point1
		 * @param cosine2 Resulting angle corresponding to point2
		 */
		void cosines(T& cosine0, T& cosine1, T& cosine2) const;

		/**
		 * Calculates the three angles of the three triangle corners.
		 * Beware: Make sure that this triangle is valid before!
		 * @param angle0 Resulting angle corresponding to point0 in radian
		 * @param angle1 Resulting angle corresponding to point1 in radian
		 * @param angle2 Resulting angle corresponding to point2 in radian
		 */
		inline void angles(T& angle0, T& angle1, T& angle2) const;

		/**
		 * Returns the minimal angle of this triangle.
		 * Beware: Make sure that this triangle is valid before!
		 * @return Minimal angle in radian
		 */
		inline T minAngle() const;

		/**
		 * Returns whether all cosine values of the three triangle corners are below or equal to a given threshold.
		 * Thus, to test whether the minimal corner angle is equal to PI/8, then allCosineBelow(cos(PI/8)) has to be checked.
		 * @param cosValue Cosine threshold value
		 * @return True, if so
		 */
		bool allCosineBelow(const T cosValue) const;

		/**
		 * Returns the maximal square side length of this triangle.
		 * @return Maximal square side length
		 */
		inline T maxSqrLength() const;

		/**
		 * Returns the maximal side length of this triangle.
		 * @return Maximal side length
		 */
		inline T maxLength() const;

		/**
		 * Returns the minimal square side length of this triangle.
		 * @return Minimal square side length
		 */
		inline T minSqrLength() const;

		/**
		 * Returns the minimal side length of this triangle.
		 * @return Minimal side length
		 */
		inline T minLength() const;

		/**
		 * Returns whether a given point lies inside this triangle.
		 * @param point The point to be checked
		 * @return True, if so
		 */
		inline bool isInside(const VectorT2<T>& point) const;

		/**
		 * Returns whether a given point lies inside at least one of the given triangles.
		 * @param triangles The triangles that are tested
		 * @param point The point to be checked
		 * @return True, if so
		 */
		static inline bool isInside(const std::vector<TriangleT2<T>>& triangles, const VectorT2<T>& point);

		/**
		 * Returns whether this triangles is defined in a counter clockwise manner.
		 * The result of the function depends on the coordinate system in which the points are defined:
		 * <pre>
		 * First coordinate system,     Second coordinate system
		 *
		 *  ------> x-axis              ^
		 * |                            | y-axis
		 * |                            |
		 * | y-axis                     |
		 * V                             ------> x-axis
		 *
		 * </pre>
		 * @param yAxisDownwards True, if the y-axis points downwards and x-axis points to the right; False, if the y-axis points upwards and the x-axis points to the right
		 * @return True, if so
		 */
		inline bool isCounterClockwise(const bool yAxisDownwards = true) const;

		/**
		 * Returns the 2D Cartesian coordinate of a given barycentric coordinate defined in relation to this triangle.
		 * @param barycentric The Barycentric coordinate to convert to a Cartesian coordinate
		 * @return Cartesian coordinate
		 */
		inline VectorT2<T> barycentric2cartesian(const VectorT3<T>& barycentric) const;

		/**
		 * Returns the barycentric coordinate of a given 2D Cartesian coordinate defined in relation to this triangle.
		 * @param cartesian The Cartesian coordinate to convert to a barycentric coordinate
		 * @return Barycentric coordinate
		 */
		inline VectorT3<T> cartesian2barycentric(const VectorT2<T>& cartesian) const;

		/**
		 * Returns the circumcenter for this triangle in barycentric coordinates.
		 * @return Barycentric coordinates of the circumcenter
		 */
		VectorT3<T> barycentricCircumcenter() const;

		/**
		 * Returns the circumcenter for this triangle in Cartesian coordinates.
		 * @return Cartesian coordinates of the circumcenter
		 */
		inline VectorT2<T> cartesianCircumcenter() const;

		/**
		 * Returns the incenter for this triangle in barycentric coordinates.
		 * @return Barycentric coordinates of the incenter
		 */
		VectorT3<T> barycentricIncenter() const;

		/**
		 * Returns the incenter for this triangle in Cartesian coordinates.
		 * @return Cartesian coordinates of the circumcenter
		 */
		inline VectorT2<T> cartesianIncenter() const;

		/**
		 * Returns whether this triangle has an intersection with a second triangle.
		 * @param triangle The second triangle to test
		 * @return True, if so
		 */
		bool intersects(const TriangleT2<T>& triangle) const;

		/**
		 * Pad a given 2D triangle along each edge by a fixed value. For a positive pad width, each side of the resulting triangle is shifted away from the triangle circumcenter along the perpendicular.
		 * @param padWidth Absolute amount to shift the triangle edges out from the triangle circumcenter, with range (-infinity, infinity); note that, in the case where the padding is negative with an absolute value smaller than the shortest distance from the circumcenter to an edge, then the triangle will flip its orientation
		 * @return Padded 2D triangle
		 */
		TriangleT2<T> padded(const T padWidth) const;

		/**
		 * Returns whether this triangle can provide valid barycentric coordinates (for 64 bit floating point values).
		 * For 32 bit floating point values we simply check whether all three corners of the triangle are different.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns individual triangle corners.
		 * @param index Index of the corner that is requested, with range [0, 2]
		 * @return Resulting triangle corner
		 */
		inline const VectorT2<T>& operator[](const unsigned int index) const;

		/**
		 * Shifts the triangle by a given 2D vector (by adding the vector to all three corners of the triangle).
		 * @param offset The offset vector to shift the triangle
		 * @return The new shifted triangle
		 */
		inline TriangleT2<T> operator+(const VectorT2<T>& offset) const;

		/**
		 * Shifts the triangle by a given 2D vector (by adding the vector to all three corners of the triangle).
		 * @param offset The offset vector to shift the triangle
		 * @return The reference to this triangle
		 */
		inline TriangleT2<T>& operator+=(const VectorT2<T>& offset);

		/**
		 * Shifts the triangle by a given 2D vector (by subtracting the vector from all three corners of the triangle).
		 * @param offset The offset vector to shift the triangle
		 * @return The new shifted triangle
		 */
		inline TriangleT2<T> operator-(const VectorT2<T>& offset) const;

		/**
		 * Shifts the triangle by a given 2D vector (by subtracting the vector from all three corners of the triangle).
		 * @param offset The offset vector to shift the triangle
		 * @return The reference to this triangle
		 */
		inline TriangleT2<T>& operator-=(const VectorT2<T>& offset);

		/**
		 * Analyses the layout of three 2D points forming either a triangle or a line.
		 * The result of the function depends on the coordinate system in which the points are defined:
		 * <pre>
		 * First coordinate system,     Second coordinate system
		 *
		 *  ------> x-axis              ^
		 * |                            | y-axis
		 * |                            |
		 * | y-axis                     |
		 * V                             ------> x-axis
		 *
		 * </pre>
		 * @param point0 The first point to be analyzed
		 * @param point1 The first point to be analyzed
		 * @param point2 The first point to be analyzed
		 * @param yAxisDownwards True, if the y-axis points downwards and x-axis points to the right; False, if the y-axis points upwards and the x-axis points to the right
		 * @return A negative value if the three points define a counter clockwise triangle, a positive value for a clockwise triangle, zero is the tree points are located on a line, with range (-infinity, infinity)
		*/
		static T analyzePoints(const VectorT2<T>& point0, const VectorT2<T>& point1, const VectorT2<T>& point2, const bool yAxisDownwards);

	private:

		/// The corner positions.
		VectorT2<T> points_[3] = {VectorT2<T>(T(0), T(0)), VectorT2<T>(T(0), T(0)), VectorT2<T>(T(0), T(0))};

		/// Convert factor for barycentric coordinates.
		T barycentricFactor_ = T(0);
};

template <typename T>
inline TriangleT2<T>::TriangleT2(const VectorT2<T>& point0, const VectorT2<T>& point1, const VectorT2<T>& point2) :
	barycentricFactor_(T(0))
{
	points_[0] = point0;
	points_[1] = point1;
	points_[2] = point2;

	const T factor = (points_[1].y() - points_[2].y()) * (points_[0].x() - points_[2].x())
								+ (points_[2].x() - points_[1].x()) * (points_[0].y() - points_[2].y());

	if (NumericT<T>::isNotEqualEps(factor))
	{
		barycentricFactor_ = T(1) / factor;
	}
}

template <typename T>
inline const VectorT2<T>& TriangleT2<T>::point0() const
{
	return points_[0];
}

template <typename T>
inline const VectorT2<T>& TriangleT2<T>::point1() const
{
	return points_[1];
}

template <typename T>
inline const VectorT2<T>& TriangleT2<T>::point2() const
{
	return points_[2];
}

template <typename T>
inline T TriangleT2<T>::sqrDistance01() const
{
	return points_[0].sqrDistance(points_[1]);
}

template <typename T>
inline T TriangleT2<T>::sqrDistance02() const
{
	return points_[0].sqrDistance(points_[2]);
}

template <typename T>
inline T TriangleT2<T>::sqrDistance12() const
{
	return points_[1].sqrDistance(points_[2]);
}

template <typename T>
inline T TriangleT2<T>::left() const
{
	ocean_assert(isValid());
	return min(points_[0].x(), min(points_[1].x(), points_[2].x()));
}

template <typename T>
inline T TriangleT2<T>::top() const
{
	ocean_assert(isValid());
	return min(points_[0].y(), min(points_[1].y(), points_[2].y()));
}

template <typename T>
inline T TriangleT2<T>::right() const
{
	ocean_assert(isValid());
	return max(points_[0].x(), max(points_[1].x(), points_[2].x()));
}

template <typename T>
inline T TriangleT2<T>::bottom() const
{
	ocean_assert(isValid());
	return max(points_[0].y(), max(points_[1].y(), points_[2].y()));
}

template <typename T>
inline T TriangleT2<T>::area() const
{
	const T squaredArea = area2();

	if (std::is_same<T, float>::value)
	{
		if (squaredArea <= T(0))
		{
			return T(0);
		}
	}

	return NumericT<T>::sqrt(area2());
}

template <typename T>
inline T TriangleT2<T>::area2() const
{
	const T a2 = points_[0].sqrDistance(points_[1]);
	const T b2 = points_[0].sqrDistance(points_[2]);
	const T c2 = points_[1].sqrDistance(points_[2]);

	return (T(4) * a2 * c2 - NumericT<T>::sqr(a2 + c2 - b2)) * T(0.0625);
}

template <typename T>
void TriangleT2<T>::cosines(T& cosine0, T& cosine1, T& cosine2) const
{
	ocean_assert(isValid());

	const T sqrDistance01(points_[1].sqrDistance(points_[0]));
	const T sqrDistance02(points_[2].sqrDistance(points_[0]));
	const T sqrDistance12(points_[2].sqrDistance(points_[1]));

	const T factorDistance01(T(1) / NumericT<T>::sqrt(sqrDistance01));
	const T factorDistance02(T(1) / NumericT<T>::sqrt(sqrDistance02));
	const T factorDistance12(T(1) / NumericT<T>::sqrt(sqrDistance12));

	// c^2 = a^2 + b^2 - 2abcos
	// cos = (a^2 + b^2 - c^2) / (2ab)

	cosine0 = (sqrDistance01 + sqrDistance02 - sqrDistance12) * T(0.5) * factorDistance01 * factorDistance02;
	cosine1 = (sqrDistance01 + sqrDistance12 - sqrDistance02) * T(0.5) * factorDistance01 * factorDistance12;
	cosine2 = (sqrDistance02 + sqrDistance12 - sqrDistance01) * T(0.5) * factorDistance02 * factorDistance12;

	ocean_assert(NumericT<T>::isWeakEqual(NumericT<T>::acos(cosine0) + NumericT<T>::acos(cosine1) + NumericT<T>::acos(cosine2), NumericT<T>::pi()));
}

template <typename T>
bool TriangleT2<T>::allCosineBelow(const T cosValue) const
{
	ocean_assert(isValid());

	const T sqrDistance01(points_[1].sqrDistance(points_[0]));
	const T sqrDistance02(points_[2].sqrDistance(points_[0]));
	const T sqrDistance12(points_[2].sqrDistance(points_[1]));

	const T factorDistance01(T(1) / NumericT<T>::sqrt(sqrDistance01));
	const T factorDistance02(T(1) / NumericT<T>::sqrt(sqrDistance02));
	const T factorDistance12(T(1) / NumericT<T>::sqrt(sqrDistance12));

	// c^2 = a^2 + b^2 - 2abcos
	// cos = (a^2 + b^2 - c^2) / (2ab)

	return (sqrDistance01 + sqrDistance02 - sqrDistance12) * T(0.5) * factorDistance01 * factorDistance02 <= cosValue
			&& (sqrDistance01 + sqrDistance12 - sqrDistance02) * T(0.5) * factorDistance01 * factorDistance12 <= cosValue
			&& (sqrDistance02 + sqrDistance12 - sqrDistance01) * T(0.5) * factorDistance02 * factorDistance12 <= cosValue;
}

template <typename T>
VectorT3<T> TriangleT2<T>::barycentricCircumcenter() const
{
	ocean_assert(isValid());

	const T a2 = points_[1].sqrDistance(points_[2]);
	const T b2 = points_[0].sqrDistance(points_[2]);
	const T c2 = points_[0].sqrDistance(points_[1]);

	const T coord0 = a2 * (-a2 + b2 + c2);
	const T coord1 = b2 * (a2 - b2 + c2);
	const T coord2 = c2 * (a2 + b2 - c2);

	const T total = coord0 + coord1 + coord2;
	ocean_assert(NumericT<T>::isNotEqualEps(total));

	const T factor = T(1) / total;

	return VectorT3<T>(coord0 * factor, coord1 * factor, coord2 * factor);
}

template <typename T>
VectorT3<T> TriangleT2<T>::barycentricIncenter() const
{
	ocean_assert(isValid());

	const T a = points_[1].distance(points_[2]);
	const T b = points_[0].distance(points_[2]);
	const T c = points_[0].distance(points_[1]);

	const T total = a + b + c;
	ocean_assert(NumericT<T>::isNotEqualEps(total));

	const T factor = T(1) / total;

	return VectorT3<T>(a * factor, b * factor, c * factor);
}

template <typename T>
bool TriangleT2<T>::intersects(const TriangleT2<T>& triangle) const
{
	ocean_assert(isValid() && triangle.isValid());

	// check whether bounding boxes do not intersect
	if (left() > triangle.right() || triangle.left() > right() || top() > triangle.bottom() || triangle.top() > bottom())
	{
		return false;
	}

	if (isInside(triangle.point0()) || isInside(triangle.point1()) || isInside(triangle.point2()) || triangle.isInside(point0()) || triangle.isInside(point1()) || triangle.isInside(point2()))
	{
		return true;
	}

	const FiniteLineT2<T> thisLines[3] =
	{
		FiniteLineT2<T>(point0(), point1()),
		FiniteLineT2<T>(point1(), point2()),
		FiniteLineT2<T>(point2(), point0())
	};

	const FiniteLineT2<T> triangleLines[3] =
	{
		FiniteLineT2<T>(triangle.point0(), triangle.point1()),
		FiniteLineT2<T>(triangle.point1(), triangle.point2()),
		FiniteLineT2<T>(triangle.point2(), triangle.point0())
	};

	return thisLines[0].intersects(triangleLines[0]) || thisLines[0].intersects(triangleLines[1]) || thisLines[0].intersects(triangleLines[2])
			|| thisLines[1].intersects(triangleLines[0]) || thisLines[1].intersects(triangleLines[1]) || thisLines[1].intersects(triangleLines[2])
			|| thisLines[2].intersects(triangleLines[0]) || thisLines[2].intersects(triangleLines[1]) || thisLines[2].intersects(triangleLines[2]);
}

template <typename T>
TriangleT2<T> TriangleT2<T>::padded(const T padWidth) const
{
	ocean_assert(isValid());
	ocean_assert(NumericT<T>::isNotEqualEps(padWidth));

	if (!isValid())
	{
		return TriangleT2<T>();
	}

	if (NumericT<T>::isEqualEps(padWidth))
	{
		return *this;
	}

	// Create homogeneous 2D points.
	const VectorT3<T> hPoint0(points_[0], T(1.0));
	const VectorT3<T> hPoint1(points_[1], T(1.0));
	const VectorT3<T> hPoint2(points_[2], T(1.0));

	VectorT3<T> line01 = hPoint0.cross(hPoint1);
	VectorT3<T> line12 = hPoint1.cross(hPoint2);
	VectorT3<T> line20 = hPoint2.cross(hPoint0);

	ocean_assert(NumericT<T>::isNotEqualEps(line01.xy().length()));
	ocean_assert(NumericT<T>::isNotEqualEps(line12.xy().length()));
	ocean_assert(NumericT<T>::isNotEqualEps(line20.xy().length()));

	// Put each line in n.x + d = 0 form, where n is the unit-length normal pointing away from the
	// opposite triangle vertex, and d is the signed distance from the origin.
	line01 /= line01.xy().length();
	line12 /= line12.xy().length();
	line20 /= line20.xy().length();

	if (line01 * hPoint2 > T(0.0))
	{
		line01 = -line01;
		line12 = -line12;
		line20 = -line20;
	}

	// Shift the lines the specified distance away from the origin.
	line01.z() -= padWidth;
	line12.z() -= padWidth;
	line20.z() -= padWidth;

	// Compute the homogeneous 2D padded triangle vertices as the cross product of the shifted 2D lines.
	const VectorT3<T> hNewPoint0 = line20.cross(line01);
	const VectorT3<T> hNewPoint1 = line01.cross(line12);
	const VectorT3<T> hNewPoint2 = line12.cross(line20);

	// Since the input triangle was valid and the lines stay parallel, it can never be the case that the line intersections are at infinity.
	ocean_assert(NumericT<T>::isNotEqualEps(hNewPoint0.z()));
	ocean_assert(NumericT<T>::isNotEqualEps(hNewPoint1.z()));
	ocean_assert(NumericT<T>::isNotEqualEps(hNewPoint2.z()));

	// De-homogenize.
	return TriangleT2<T>(
		VectorT2<T>(hNewPoint0.xy() / hNewPoint0.z()),
		VectorT2<T>(hNewPoint1.xy() / hNewPoint1.z()),
		VectorT2<T>(hNewPoint2.xy() / hNewPoint2.z()));
}

template <typename T>
inline void TriangleT2<T>::angles(T& angle0, T& angle1, T& angle2) const
{
	T cosine0, cosine1, cosine2;
	cosines(cosine0, cosine1, cosine2);

	angle0 = NumericT<T>::acos(cosine0);
	angle1 = NumericT<T>::acos(cosine1);
	angle2 = NumericT<T>::acos(cosine2);
}

template <typename T>
inline T TriangleT2<T>::minAngle() const
{
	T cosine0, cosine1, cosine2;
	cosines(cosine0, cosine1, cosine2);

	ocean_assert(NumericT<T>::isInsideRange(T(-1), cosine0, T(1)));
	ocean_assert(NumericT<T>::isInsideRange(T(-1), cosine1, T(1)));
	ocean_assert(NumericT<T>::isInsideRange(T(-1), cosine2, T(1)));

	return NumericT<T>::acos(max(NumericT<T>::abs(cosine0), max(NumericT<T>::abs(cosine1), NumericT<T>::abs(cosine2))));
}

template <typename T>
inline T TriangleT2<T>::maxSqrLength() const
{
	const T sqrLength01(points_[0].sqrDistance(points_[1]));
	const T sqrLength02(points_[0].sqrDistance(points_[2]));
	const T sqrLength12(points_[1].sqrDistance(points_[2]));

	return max(sqrLength01, max(sqrLength02, sqrLength12));
}

template <typename T>
inline T TriangleT2<T>::maxLength() const
{
	return NumericT<T>::sqrt(maxSqrLength());
}

template <typename T>
inline T TriangleT2<T>::minSqrLength() const
{
	const T sqrLength01(points_[0].sqrDistance(points_[1]));
	const T sqrLength02(points_[0].sqrDistance(points_[2]));
	const T sqrLength12(points_[1].sqrDistance(points_[2]));

	return min(sqrLength01, min(sqrLength02, sqrLength12));
}

template <typename T>
inline T TriangleT2<T>::minLength() const
{
	return NumericT<T>::sqrt(minSqrLength());
}

template <typename T>
inline bool TriangleT2<T>::isInside(const VectorT2<T>& point) const
{
	return TriangleT<T>::isBarycentricInside(cartesian2barycentric(point));
}

template <typename T>
inline bool TriangleT2<T>::isInside(const std::vector<TriangleT2<T>>& triangles, const VectorT2<T>& point)
{
	for (const TriangleT2<T>& triangle : triangles)
	{
		if (triangle.isInside(point))
		{
			return true;
		}
	}

	return false;
}

template <typename T>
inline bool TriangleT2<T>::isCounterClockwise(const bool yAxisDownwards) const
{
	ocean_assert(isValid());

	return analyzePoints(points_[0], points_[1], points_[2], yAxisDownwards) < T(0);
}

template <typename T>
inline VectorT2<T> TriangleT2<T>::barycentric2cartesian(const VectorT3<T>& barycentric) const
{
	ocean_assert_accuracy((std::is_same<float, T>::value || TriangleT<T>::isValidBarycentric(barycentric, NumericT<T>::weakEps())));

	return VectorT2<T>((points_[0].x() * barycentric[0] + points_[1].x() * barycentric[1] + points_[2].x() * barycentric[2]),
						(points_[0].y() * barycentric[0] + points_[1].y() * barycentric[1] + points_[2].y() * barycentric[2]));
}

template <typename T>
inline VectorT3<T> TriangleT2<T>::cartesian2barycentric(const VectorT2<T>& cartesian) const
{
	ocean_assert(isValid());

	const T barycentric0 = ((points_[1].y() - points_[2].y()) * (cartesian.x() - points_[2].x())
							+ (points_[2].x() - points_[1].x()) * (cartesian.y() -points_[2].y())) * barycentricFactor_;

	const T barycentric1 = ((points_[2].y() - points_[0].y()) * (cartesian.x() - points_[2].x())
							+ (points_[0].x() - points_[2].x()) * (cartesian.y() - points_[2].y())) * barycentricFactor_;

#ifdef OCEAN_DEBUG
	if (std::is_same<float, Scalar>::value)
	{
		ocean_assert_accuracy(TriangleT<T>::isValidBarycentric(VectorT3<T>(barycentric0, barycentric1, 1 - barycentric0 - barycentric1), NumericT<T>::weakEps()));
	}
	else
	{
		ocean_assert(TriangleT<T>::isValidBarycentric(VectorT3<T>(barycentric0, barycentric1, 1 - barycentric0 - barycentric1), NumericT<T>::weakEps()));
	}
#endif

	return VectorT3<T>(barycentric0, barycentric1, T(1) - barycentric0 - barycentric1);
}

template <typename T>
inline VectorT2<T> TriangleT2<T>::cartesianCircumcenter() const
{
	ocean_assert(isValid());
	return barycentric2cartesian(barycentricCircumcenter());
}

template <typename T>
inline VectorT2<T> TriangleT2<T>::cartesianIncenter() const
{
	ocean_assert(isValid());
	return barycentric2cartesian(barycentricIncenter());
}

template <typename T>
inline bool TriangleT2<T>::isValid() const
{
	if (std::is_same<T, double>::value)
	{
		return NumericT<T>::isNotEqualEps(barycentricFactor_);
	}
	else
	{
		return points_[0] != points_[1] && points_[0] != points_[2] && points_[1] != points_[2];
	}
}

template <typename T>
inline const VectorT2<T>& TriangleT2<T>::operator[](const unsigned int index) const
{
	ocean_assert(index <= 2u);
	return points_[index];
}

template <typename T>
inline TriangleT2<T> TriangleT2<T>::operator+(const VectorT2<T>& offset) const
{
	return TriangleT2<T>(points_[0] + offset, points_[1] + offset, points_[2] + offset);
}

template <typename T>
inline TriangleT2<T>& TriangleT2<T>::operator+=(const VectorT2<T>& offset)
{
	points_[0] += offset;
	points_[1] += offset;
	points_[2] += offset;

	return *this;
}

template <typename T>
inline TriangleT2<T> TriangleT2<T>::operator-(const VectorT2<T>& offset) const
{
	return TriangleT2<T>(points_[0] - offset, points_[1] - offset, points_[2] - offset);
}

template <typename T>
inline TriangleT2<T>& TriangleT2<T>::operator-=(const VectorT2<T>& offset)
{
	points_[0] -= offset;
	points_[1] -= offset;
	points_[2] -= offset;

	return *this;
}

template <typename T>
T TriangleT2<T>::analyzePoints(const VectorT2<T>& point0, const VectorT2<T>& point1, const VectorT2<T>& point2, const bool yAxisDownwards)
{
	const VectorT2<T> vector01(point1 - point0);
	const VectorT2<T> vector02(point2 - point0);

	if (yAxisDownwards)
	{
		return vector01.cross(vector02);
	}
	else
	{
		return -vector01.cross(vector02);
	}
}

}

#endif // META_OCEAN_MATH_TRIANGLE_H
