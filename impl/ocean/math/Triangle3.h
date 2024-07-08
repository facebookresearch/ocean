/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_TRIANGLE_3_H
#define META_OCEAN_MATH_TRIANGLE_3_H

#include "ocean/math/Math.h"
#include "ocean/math/Line3.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Triangle.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class TriangleT3;

/**
 * Definition of the Triangle3 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see TriangleT3
 * @ingroup math
 */
typedef TriangleT3<Scalar> Triangle3;

/**
 * Instantiation of the TriangleT3 template class using a double precision float data type.
 * @see TriangleT3
 * @ingroup math
 */
typedef TriangleT3<double> TriangleD3;

/**
 * Instantiation of the TriangleT3 template class using a single precision float data type.
 * @see TriangleT3
 * @ingroup math
 */
typedef TriangleT3<float> TriangleF3;

/**
 * Definition of a typename alias for vectors with TriangleT3 objects.
 * @see TriangleT3
 * @ingroup math
 */
template <typename T>
using TrianglesT3 = std::vector<TriangleT3<T>>;

/**
 * Definition of a vector holding 3D triangles.
 * @see Triangle3
 * @ingroup math
 */
typedef std::vector<Triangle3> Triangles3;

/**
 * Definition of a vector holding 3D triangles with single precision float data type.
 * @see Triangle3
 * @ingroup math
 */
typedef std::vector<TriangleF3> TrianglesF3;

/**
 * Definition of a vector holding 3D triangles with double precision float data type.
 * @see Triangle3
 * @ingroup math
 */
typedef std::vector<TriangleD3> TrianglesD3;

/**
 * This class implements a 3D triangle.
 * @see Triangle2.
 * @ingroup math
 */
template <typename T>
class TriangleT3 : public TriangleT<T>
{
	public:

		/**
		 * Creates a new triangle object with default parameters.
		 */
		inline TriangleT3();

		/**
		 * Creates a new triangle object by three given corners.
		 * @param point0 First triangle point
		 * @param point1 Second triangle point
		 * @param point2 Third triangle point
		 */
		inline TriangleT3(const VectorT3<T>& point0, const VectorT3<T>& point1, const VectorT3<T>& point2);

		/**
		 * Returns the first point of this triangle.
		 * @return Triangle point
		 */
		inline const VectorT3<T>& point0() const;

		/**
		 * Returns the second point of this triangle.
		 * @return Triangle point
		 */
		inline const VectorT3<T>& point1() const;

		/**
		 * Returns the third point of this triangle.
		 * @return Triangle point
		 */
		inline const VectorT3<T>& point2() const;

		/**
		 * Returns the 3D cartesian coordinate of a given barycentric coordinate defined in relation to this triangle.
		 * @param barycentric Barycentric coordinate to convert to a cartesian coordinate
		 * @return Cartesian coordinate
		 */
		inline VectorT3<T> barycentric2cartesian(const VectorT3<T>& barycentric) const;

		/**
		 * Returns whether a given ray has an intersection with this triangle.
		 * @param ray Ray to determine whether an intersection occurs
		 * @return True, if so
		 */
		inline bool hasIntersection(const LineT3<T>& ray) const;

		/**
		 * Returns whether a given ray has an intersection with this triangle and determines the intersection point.
		 * @param ray Ray to determine whether an intersection occurs
		 * @param point Resulting intersection point, if an intersection occurs
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		bool intersection(const LineT3<T>& ray, VectorT3<T>& point, T& distance) const;

		/**
		 * Returns whether a given ray has an intersection with this triangle and determines the intersection point.
		 * @param ray Ray to determine whether an intersection occurs
		 * @param point Resulting intersection point, if an intersection occurs
		 * @param barycentric Resulting Barycentric intersection point
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		bool intersection(const LineT3<T>& ray, VectorT3<T>& point, VectorT3<T>& barycentric, T& distance) const;

		/**
		 * Returns whether a given ray has an intersection with this triangle and determines the reflective ray starting at the intersection point.
		 * The reflection normal is determined by the triangle.<br>
		 * @param ray Ray to determine whether an intersection occurs
		 * @param reflection Resulting reflection ray, if an intersection occurs
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		bool intersection(const LineT3<T>& ray, LineT3<T>& reflection, T& distance) const;

		/**
		 * Returns whether a given ray has an intersection with this triangle and determines the reflective ray starting at the intersection point.
		 * The reflection normal is determined by the triangle.<br>
		 * @param ray Ray to determine whether an intersection occurs
		 * @param reflection Resulting reflection ray, if an intersection occurs
		 * @param distance Resulting intersection distance
		 * @param normal Resulting normal
		 * @return True, if so
		 */
		bool intersection(const LineT3<T>& ray, LineT3<T>& reflection, T& distance, VectorT3<T>& normal) const;

		/**
		 * Returns whether a given ray has a front intersection with this triangle and determines the reflective ray starting at the intersection point.
		 * An explicit reflection normal is used.<br>
		 * @param ray Ray to determine whether an intersection occurs
		 * @param normal Reflection normal
		 * @param reflection Resulting reflection ray, if an intersection occurs
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		inline bool frontIntersection(const LineT3<T>& ray, const VectorT3<T>& normal, LineT3<T>& reflection, T& distance) const;

		/**
		 * Returns whether a given ray has a front intersection with this triangle and determines the reflective ray starting at the intersection point.
		 * The reflection normal is determined by interpolation of the given three point normals.
		 * @param ray Ray to determine whether an intersection occurs
		 * @param normal0 Normal of the first triangle point
		 * @param normal1 Normal of the second triangle point
		 * @param normal2 Normal of the third triangle point
		 * @param reflection Resulting reflection ray, if an intersection occurs
		 * @param distance Resulting intersection distance
		 * @return True, if so
		 */
		inline bool frontIntersection(const LineT3<T>& ray, const VectorT3<T>& normal0, const VectorT3<T>& normal1, const VectorT3<T>& normal2, LineT3<T>& reflection, T& distance) const;

		/**
		 * Returns whether a given ray has a front intersection with this triangle and determines the reflective ray starting at the intersection point.
		 * The reflection normal is determined by interpolation of the given three point normals.
		 * @param ray Ray to determine whether an intersection occurs
		 * @param normal0 Normal of the first triangle point
		 * @param normal1 Normal of the second triangle point
		 * @param normal2 Normal of the third triangle point
		 * @param reflection Resulting reflection ray, if an intersection occurs
		 * @param distance Resulting intersection distance
		 * @param normal Resulting normal
		 * @return True, if so
		 */
		inline bool frontIntersection(const LineT3<T>& ray, const VectorT3<T>& normal0, const VectorT3<T>& normal1, const VectorT3<T>& normal2, LineT3<T>& reflection, T& distance, VectorT3<T>& normal) const;

		/**
		 * Returns the intersection point with a given ray and a plane that is spanned by this triangle.
		 * @param ray Ray to determine whether an intersection occurs
		 * @param barycentric Resulting Barycentric intersection point
		 * @return True, if the ray intersects the plane that is spanned by the triangle (the return value does not indicate whether the triangle itself has been intersected)
		 */
		bool planeIntersection(const LineT3<T>& ray, VectorT3<T>& barycentric) const;

		/**
		 * Returns whether this triangle is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns individual triangle corners.
		 * @param index Index of the corner that is requested, with range [0, 2]
		 * @return Resulting triangle corner
		 */
		inline const VectorT3<T>& operator[](const unsigned int index) const;

		/**
		 * Shifts the triangle by a given 3D vector (by adding the vector to all three corners of the triangle).
		 * @param offset The offset vector to shift the triangle
		 * @return The new shifted triangle
		 */
		inline TriangleT3<T> operator+(const VectorT3<T>& offset) const;

		/**
		 * Shifts the triangle by a given 3D vector (by adding the vector to all three corners of the triangle).
		 * @param offset The offset vector to shift the triangle
		 * @return The reference to this triangle
		 */
		inline TriangleT3<T>& operator+=(const VectorT3<T>& offset);

		/**
		 * Shifts the triangle by a given 3D vector (by subtracting the vector from all three corners of the triangle).
		 * @param offset The offset vector to shift the triangle
		 * @return The new shifted triangle
		 */
		inline TriangleT3<T> operator-(const VectorT3<T>& offset) const;

		/**
		 * Shifts the triangle by a given 3D vector (by subtracting the vector from all three corners of the triangle).
		 * @param offset The offset vector to shift the triangle
		 * @return The reference to this triangle
		 */
		inline TriangleT3<T>& operator-=(const VectorT3<T>& offset);

	private:

		/// Three triangle points.
		VectorT3<T> trianglePoints[3];
};

template <typename T>
inline TriangleT3<T>::TriangleT3()
{
	trianglePoints[0] = VectorT3<T>(0, 0, 0);
	trianglePoints[1] = VectorT3<T>(0, 0, 0);
	trianglePoints[2] = VectorT3<T>(0, 0, 0);
}

template <typename T>
inline TriangleT3<T>::TriangleT3(const VectorT3<T>& point0, const VectorT3<T>& point1, const VectorT3<T>& point2)
{
	trianglePoints[0] = point0;
	trianglePoints[1] = point1;
	trianglePoints[2] = point2;
}

template <typename T>
inline const VectorT3<T>& TriangleT3<T>::point0() const
{
	return trianglePoints[0];
}

template <typename T>
inline const VectorT3<T>& TriangleT3<T>::point1() const
{
	return trianglePoints[1];
}

template <typename T>
inline const VectorT3<T>& TriangleT3<T>::point2() const
{
	return trianglePoints[2];
}

template <typename T>
inline VectorT3<T> TriangleT3<T>::barycentric2cartesian(const VectorT3<T>& barycentric) const
{
	ocean_assert(TriangleT<T>::isValidBarycentric(barycentric, NumericT<T>::weakEps()));

	return VectorT3<T>((trianglePoints[0].x() * barycentric[0] + trianglePoints[1].x() * barycentric[1] + trianglePoints[2].x() * barycentric[2]),
						(trianglePoints[0].y() * barycentric[0] + trianglePoints[1].y() * barycentric[1] + trianglePoints[2].y() * barycentric[2]),
						(trianglePoints[0].z() * barycentric[0] + trianglePoints[1].z() * barycentric[1] + trianglePoints[2].z() * barycentric[2]));
}

template <typename T>
inline bool TriangleT3<T>::hasIntersection(const LineT3<T>& ray) const
{
	ocean_assert(ray.isValid());

	SquareMatrixT3<T> matrix(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0], -ray.direction());

	if (!matrix.invert())
	{
		return false;
	}

	const VectorT3<T> result(matrix * (ray.point() - trianglePoints[0]));

	return result.x() >= -NumericT<T>::eps() && result.y() >= -NumericT<T>::eps() && result.x() + result.y() <= T(1) + NumericT<T>::eps();
}

template <typename T>
bool TriangleT3<T>::intersection(const LineT3<T>& ray, VectorT3<T>& point, T& distance) const
{
	ocean_assert(ray.isValid());

	// following matrix multiplication determines the intersection of triangle plane P (with V0, V1, V2) and ray line L:
	// L(r) = P(s,t)
	// [V1-V0 V2-V0 -Ld] * [s t r] = [Lp-V0]
	// L(r) is in triangle, when s>=0, t>=0 and s+t <= 1

	SquareMatrixT3<T> matrix(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0], -ray.direction());

	if (!matrix.invert())
	{
		return false;
	}

	const VectorT3<T> result(matrix * (ray.point() - trianglePoints[0]));

	if (result.x() < 0 || result.y() < 0 || result.x() + result.y() > 1)
	{
		return false;
	}

	distance = result.z();
	point = ray.point() + ray.direction() * result.z();

	return true;
}

template <typename T>
bool TriangleT3<T>::intersection(const LineT3<T>& ray, VectorT3<T>& point, VectorT3<T>& barycentric, T& distance) const
{
	ocean_assert(ray.isValid());

	SquareMatrixT3<T> matrix(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0], -ray.direction());

	if (!matrix.invert())
	{
		return false;
	}

	const VectorT3<T> result(matrix * (ray.point() - trianglePoints[0]));

	if (result.x() < 0 || result.y() < 0 || result.x() + result.y() > 1)
	{
		return false;
	}

	distance = result.z();
	point = ray.point() + ray.direction() * result.z();
	barycentric = VectorT3<T>(1 - result.x() - result.y(), result.x(), result.y());

	return true;
}

template <typename T>
bool TriangleT3<T>::intersection(const LineT3<T>& ray, LineT3<T>& reflection, T& distance) const
{
	ocean_assert(ray.isValid());

	SquareMatrixT3<T> matrix(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0], -ray.direction());

	if (!matrix.invert())
	{
		return false;
	}

	const VectorT3<T> result(matrix * (ray.point() - trianglePoints[0]));

	if (result.x() < 0 || result.y() < 0 || result.x() + result.y() > 1)
	{
		return false;
	}

	distance = result.z();

	const VectorT3<T> normal((trianglePoints[1] - trianglePoints[0]).cross(trianglePoints[2] - trianglePoints[0]));
	ocean_assert(NumericT<T>::isNotEqualEps(normal.length()));

	ocean_assert(NumericT<T>::isEqual(ray.direction().length(), 1));

	reflection = LineT3<T>(ray.point() + ray.direction() * result.z(), (-ray.direction()).reflect(normal));
	ocean_assert(reflection.isValid());

	return true;
}

template <typename T>
bool TriangleT3<T>::intersection(const LineT3<T>& ray, LineT3<T>& reflection, T& distance, VectorT3<T>& normal) const
{
	ocean_assert(ray.isValid());

	SquareMatrixT3<T> matrix(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0], -ray.direction());

	if (!matrix.invert())
	{
		return false;
	}

	const VectorT3<T> result(matrix * (ray.point() - trianglePoints[0]));

	if (result.x() < 0 || result.y() < 0 || result.x() + result.y() > 1)
	{
		return false;
	}

	distance = result.z();

	normal = VectorT3<T>((trianglePoints[1] - trianglePoints[0]).cross(trianglePoints[2] - trianglePoints[0]));
	ocean_assert(NumericT<T>::isNotEqualEps(normal.length()));

	ocean_assert(NumericT<T>::isEqual(ray.direction().length(), 1));

	reflection = LineT3<T>(ray.point() + ray.direction() * result.z(), (-ray.direction()).reflect(normal));
	ocean_assert(reflection.isValid());

	return true;
}

template <typename T>
inline bool TriangleT3<T>::frontIntersection(const LineT3<T>& ray, const VectorT3<T>& normal, LineT3<T>& reflection, T& distance) const
{
	if (normal * ray.direction() > 0)
	{
		return false;
	}

	ocean_assert(ray.isValid());
	ocean_assert(NumericT<T>::isNotEqualEps(normal.length()));

	SquareMatrixT3<T> matrix(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0], -ray.direction());

	if (!matrix.invert())
	{
		return false;
	}

	const VectorT3<T> result(matrix * (ray.point() - trianglePoints[0]));

	if (result.x() < 0 || result.y() < 0 || result.x() + result.y() > 1)
	{
		return false;
	}

	distance = result.z();

	ocean_assert(NumericT<T>::isEqual(ray.direction().length(), 1));

	reflection = LineT3<T>(ray.point() + ray.direction() * result.z(), (-ray.direction()).reflect(normal));
	ocean_assert(reflection.isValid());

	return true;
}

template <typename T>
inline bool TriangleT3<T>::frontIntersection(const LineT3<T>& ray, const VectorT3<T>& normal0, const VectorT3<T>& normal1, const VectorT3<T>& normal2, LineT3<T>& reflection, T& distance) const
{
	ocean_assert(ray.isValid());

	ocean_assert(NumericT<T>::isNotEqualEps(normal0.length()));
	ocean_assert(NumericT<T>::isNotEqualEps(normal1.length()));
	ocean_assert(NumericT<T>::isNotEqualEps(normal2.length()));

	SquareMatrixT3<T> matrix(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0], -ray.direction());

	if (!matrix.invert())
	{
		return false;
	}

	const VectorT3<T> result(matrix * (ray.point() - trianglePoints[0]));

	if (result.x() < 0 || result.y() < 0 || result.x() + result.y() > 1)
	{
		return false;
	}

	distance = result.z();

	const VectorT3<T> normal(normal0 * (1 - result.x() - result.y()) + normal1 * result.x() + normal2 * result.y());
	ocean_assert(NumericT<T>::isNotEqualEps(normal.length()));

	ocean_assert(NumericT<T>::isEqual(ray.direction().length(), 1));

	if (normal * ray.direction() > 0)
	{
		return false;
	}

	reflection = LineT3<T>(ray.point() + ray.direction() * result.z(), (-ray.direction()).reflect(normal));
	ocean_assert(reflection.isValid());

	return true;
}

template <typename T>
inline bool TriangleT3<T>::frontIntersection(const LineT3<T>& ray, const VectorT3<T>& normal0, const VectorT3<T>& normal1, const VectorT3<T>& normal2, LineT3<T>& reflection, T& distance, VectorT3<T>& normal) const
{
	ocean_assert(ray.isValid());

	ocean_assert(NumericT<T>::isNotEqualEps(normal0.length()));
	ocean_assert(NumericT<T>::isNotEqualEps(normal1.length()));
	ocean_assert(NumericT<T>::isNotEqualEps(normal2.length()));

	SquareMatrixT3<T> matrix(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0], -ray.direction());

	if (!matrix.invert())
	{
		return false;
	}

	const VectorT3<T> result(matrix * (ray.point() - trianglePoints[0]));

	if (result.x() < 0 || result.y() < 0 || result.x() + result.y() > 1)
	{
		return false;
	}

	distance = result.z();

	normal = VectorT3<T>(normal0 * (1 - result.x() - result.y()) + normal1 * result.x() + normal2 * result.y());
	ocean_assert(NumericT<T>::isNotEqualEps(normal.length()));
	normal.normalize();

	ocean_assert(NumericT<T>::isEqual(ray.direction().length(), 1));

	if (normal * ray.direction() > 0)
	{
		return false;
	}

	reflection = LineT3<T>(ray.point() + ray.direction() * result.z(), (-ray.direction()).reflect(normal));
	ocean_assert(reflection.isValid());

	return true;
}

template <typename T>
bool TriangleT3<T>::planeIntersection(const LineT3<T>& ray, VectorT3<T>& barycentric) const
{
	ocean_assert(ray.isValid());

	SquareMatrixT3<T> matrix(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0], -ray.direction());

	if (!matrix.invert())
	{
		return false;
	}

	const VectorT3<T> result(matrix * (ray.point() - trianglePoints[0]));

	barycentric = VectorT3<T>(1 - result.x() - result.y(), result.x(), result.y());

	return true;
}

template <typename T>
inline bool TriangleT3<T>::isValid() const
{
	return trianglePoints[0] != trianglePoints[1] && trianglePoints[0] != trianglePoints[2] && trianglePoints[1] != trianglePoints[2];
}

template <typename T>
inline const VectorT3<T>& TriangleT3<T>::operator[](const unsigned int index) const
{
	ocean_assert(index <= 2u);
	return trianglePoints[index];
}

template <typename T>
inline TriangleT3<T> TriangleT3<T>::operator+(const VectorT3<T>& offset) const
{
	return TriangleT3<T>(trianglePoints[0] + offset, trianglePoints[1] + offset, trianglePoints[2] + offset);
}

template <typename T>
inline TriangleT3<T>& TriangleT3<T>::operator+=(const VectorT3<T>& offset)
{
	trianglePoints[0] += offset;
	trianglePoints[1] += offset;
	trianglePoints[2] += offset;

	return *this;
}

template <typename T>
inline TriangleT3<T> TriangleT3<T>::operator-(const VectorT3<T>& offset) const
{
	return TriangleT3<T>(trianglePoints[0] - offset, trianglePoints[1] - offset, trianglePoints[2] - offset);
}

template <typename T>
inline TriangleT3<T>& TriangleT3<T>::operator-=(const VectorT3<T>& offset)
{
	trianglePoints[0] -= offset;
	trianglePoints[1] -= offset;
	trianglePoints[2] -= offset;

	return *this;
}

}

#endif // META_OCEAN_MATH_TRIANGLE_3_H
