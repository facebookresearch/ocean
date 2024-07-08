/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_PIXEL_TRIANGLE_H
#define META_OCEAN_CV_ADVANCED_PIXEL_TRIANGLE_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/Utilities.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/Triangle2.h"

#include <vector>

namespace Ocean
{

namespace CV
{

namespace Advanced
{

// Forward declaration.
template <typename T> class PixelTriangleT;

/**
 * Definition of the default PixelTriangle object with a data type allowing only positive coordinate values.
 * @see PixelTriangleT
 * @ingroup cvadvanced
 */
typedef PixelTriangleT<unsigned int> PixelTriangle;

/**
 * Definition of a PixelTriangle object with a data type allowing positive and negative coordinate values.
 * @see PixelTriangleT
 * @ingroup cvadvanced
 */
typedef PixelTriangleT<int> PixelTriangleI;

/**
 * Definition of a vector holding pixel triangles (with positive coordinate values).
 * @see PixelTriangle
 * @ingroup cvadvanced
 */
typedef std::vector<PixelTriangle> PixelTriangles;

/**
 * Definition of a vector holding pixel triangles (with positive and negative coordinate values).
 * @see PixelTriangleI
 * @ingroup cvadvanced
 */
typedef std::vector<PixelTriangleI> PixelTrianglesI;

/**
 * This class implements a 2D triangle with pixel precision.
 * @tparam T The data type that is used to store the elements of a pixel coordinate
 * @see PixelPosition, PixelPositionI
 * @ingroup cvadvanced
 */
template <typename T>
class PixelTriangleT
{
	public:

		/**
		 * Creates an invalid triangle.
		 */
		inline PixelTriangleT();

		/**
		 * Creates a triangle by three corners.
		 * @param point0 First corner point
		 * @param point1 Second corner point
		 * @param point2 Third corner point
		 */
		inline PixelTriangleT(const PixelPositionT<T>& point0, const PixelPositionT<T>& point1, const PixelPositionT<T>& point2);

		/**
		 * Creates a triangle by a given triangle with subpixel accuracy.
		 * The corners of the given triangle are rounded to the next matching pixel within a specified area with upper left corner at (0, 0).
		 * @param triangle Subpixel accuracy triangle to be converted to a pixel accuracy triangle
		 * @param width The width of the specified area in which the resulting triangle will be located, with range [1, infinity)
		 * @param height The height of the specified area in which the resulting triangle will be located, with range [1, infinity)
		 */
		inline PixelTriangleT(const Triangle2& triangle, const unsigned int width, const unsigned int height);

		/**
		 * Returns the first corner point of this triangle.
		 * @return First corner point
		 */
		inline const PixelPositionT<T>& point0() const;

		/**
		 * Returns the second corner point of this triangle.
		 * @return Second corner point
		 */
		inline const PixelPositionT<T>& point1() const;

		/**
		 * Returns the third corner point of this triangle.
		 * @return Third corner point
		 */
		inline const PixelPositionT<T>& point2() const;

		/**
		 * Returns the most left (including) position of this triangle.
		 * @return Left position
		 */
		inline T left() const;

		/**
		 * Returns the most top (including) position of this triangle.
		 * @return Top position
		 */
		inline T top() const;

		/**
		 * Returns the most right (including) position of this triangle.
		 * @return Right position
		 */
		inline T right() const;

		/**
		 * Returns the most bottom (including) position of this triangle.
		 * @return Bottom position
		 */
		inline T bottom() const;

		/**
		 * Returns whether this triangle holds three valid corner points.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this triangle holds three valid corner points.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Shifts the corners of the triangle by a given offset (by adding the offset to each corner) and returns the shifted triangle.
		 * This triangle object must be valid.
		 * @param offset The offset to be added to each corner, must be valid
		 * @return The new shifted triangle
		 */
		inline PixelTriangleT<T> operator+(const CV::PixelPositionT<T>& offset) const;

		/**
		 * Shifts the corners of this triangle by a given offset (by adding the offset to each corner).
		 * This triangle object must be valid.
		 * @param offset The offset to be added to each corner, must be valid
		 * @return The reference to this shifted triangle
		 */
		inline PixelTriangleT<T>& operator+=(const CV::PixelPositionT<T>& offset);

		/**
		 * Shifts the corners of the triangle by a given offset (by subtracting the offset from each corner) and returns the shifted triangle.
		 * This triangle object must be valid.
		 * @param offset The offset to be subtracted from each corner, must be valid
		 * @return The new shifted triangle
		 */
		inline PixelTriangleT<T> operator-(const CV::PixelPositionT<T>& offset) const;

		/**
		 * Shifts the corners of this triangle by a given offset (by subtracting the offset from each corner).
		 * This triangle object must be valid.
		 * @param offset The offset to be subtracted from each corner, must be valid
		 * @return The reference to this shifted triangle
		 */
		inline PixelTriangleT<T>& operator-=(const CV::PixelPositionT<T>& offset);

		/**
		 * Returns individual triangle corners.
		 * @param index Index of the corner that is requested, with range [0, 2]
		 * @return Resulting triangle corner
		 */
		inline const PixelPositionT<T>& operator[](const unsigned int index) const;

		/**
		 * Returns individual triangle corners.
		 * @param index Index of the corner that is requested, with range [0, 2]
		 * @return Resulting triangle corner
		 */
		inline PixelPositionT<T>& operator[](const unsigned int index);

	private:

		/// Three triangle corners.
		PixelPositionT<T> trianglePoints[3];
};

template <typename T>
inline PixelTriangleT<T>::PixelTriangleT()
{
	// nothing to do here
}

template <typename T>
inline PixelTriangleT<T>::PixelTriangleT(const PixelPositionT<T>& point0, const PixelPositionT<T>& point1, const PixelPositionT<T>& point2)
{
	trianglePoints[0] = point0;
	trianglePoints[1] = point1;
	trianglePoints[2] = point2;
}

template <typename T>
inline PixelTriangleT<T>::PixelTriangleT(const Triangle2& triangle, const unsigned int width, const unsigned int height)
{
	ocean_assert(triangle.isValid());
	ocean_assert(width >= 1u && height >= 1u);

	trianglePoints[0] = PixelPosition(minmax(0, Numeric::round32(triangle.point0().x()), int(width) - 1), minmax(0, Numeric::round32(triangle.point0().y()), int(height) - 1));
	trianglePoints[1] = PixelPosition(minmax(0, Numeric::round32(triangle.point1().x()), int(width) - 1), minmax(0, Numeric::round32(triangle.point1().y()), int(height) - 1));
	trianglePoints[2] = PixelPosition(minmax(0, Numeric::round32(triangle.point2().x()), int(width) - 1), minmax(0, Numeric::round32(triangle.point2().y()), int(height) - 1));
}

template <typename T>
inline const PixelPositionT<T>& PixelTriangleT<T>::point0() const
{
	return trianglePoints[0];
}

template <typename T>
inline const PixelPositionT<T>& PixelTriangleT<T>::point1() const
{
	return trianglePoints[1];
}

template <typename T>
inline const PixelPositionT<T>& PixelTriangleT<T>::point2() const
{
	return trianglePoints[2];
}

template <typename T>
inline T PixelTriangleT<T>::left() const
{
	ocean_assert(isValid());
	return min(trianglePoints[0].x(), min(trianglePoints[1].x(), trianglePoints[2].x()));
}

template <typename T>
inline T PixelTriangleT<T>::top() const
{
	ocean_assert(isValid());
	return min(trianglePoints[0].y(), min(trianglePoints[1].y(), trianglePoints[2].y()));
}

template <typename T>
inline T PixelTriangleT<T>::right() const
{
	ocean_assert(isValid());
	return max(trianglePoints[0].x(), max(trianglePoints[1].x(), trianglePoints[2].x()));
}

template <typename T>
inline T PixelTriangleT<T>::bottom() const
{
	ocean_assert(isValid());
	return max(trianglePoints[0].y(), max(trianglePoints[1].y(), trianglePoints[2].y()));
}

template <typename T>
inline bool PixelTriangleT<T>::isValid() const
{
	return trianglePoints[0].isValid() && trianglePoints[1].isValid() && trianglePoints[2].isValid();
}

template <typename T>
inline PixelTriangleT<T>::operator bool() const
{
	return isValid();
}

template <typename T>
inline PixelTriangleT<T> PixelTriangleT<T>::operator+(const CV::PixelPositionT<T>& offset) const
{
	ocean_assert(isValid() && offset.isValid());
	return PixelTriangleT<T>(trianglePoints[0] + offset, trianglePoints[1] + offset, trianglePoints[2] + offset);
}

template <typename T>
inline PixelTriangleT<T>& PixelTriangleT<T>::operator+=(const CV::PixelPositionT<T>& offset)
{
	ocean_assert(isValid() && offset.isValid());

	trianglePoints[0] += offset;
	trianglePoints[1] += offset;
	trianglePoints[2] += offset;

	return *this;
}

template <typename T>
inline PixelTriangleT<T> PixelTriangleT<T>::operator-(const CV::PixelPositionT<T>& offset) const
{
	ocean_assert(isValid() && offset.isValid());
	return PixelTriangleT<T>(trianglePoints[0] - offset, trianglePoints[1] - offset, trianglePoints[2] - offset);
}

template <typename T>
inline PixelTriangleT<T>& PixelTriangleT<T>::operator-=(const CV::PixelPositionT<T>& offset)
{
	ocean_assert(isValid() && offset.isValid());

	trianglePoints[0] -= offset;
	trianglePoints[1] -= offset;
	trianglePoints[2] -= offset;

	return *this;
}

template <typename T>
inline const PixelPositionT<T>& PixelTriangleT<T>::operator[](const unsigned int index) const
{
	ocean_assert(index <= 2u);
	return trianglePoints[index];
}

template <typename T>
inline PixelPositionT<T>& PixelTriangleT<T>::operator[](const unsigned int index)
{
	ocean_assert(index <= 2u);
	return trianglePoints[index];
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_PIXEL_TRIANGLE_H
