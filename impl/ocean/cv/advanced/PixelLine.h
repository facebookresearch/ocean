/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_PIXEL_LINE_H
#define META_OCEAN_CV_ADVANCED_PIXEL_LINE_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/DataType.h"

#include "ocean/cv/PixelPosition.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

// Forward declaration.
template <typename T> class PixelLineT;

/**
 * Definition of the default PixelLine object with a data type allowing only positive coordinate values.
 * @see PixelLineT
 * @ingroup cvadvanced
 */
typedef PixelLineT<unsigned int> PixelLine;

/**
 * Definition of a PixelLine object with a data type allowing positive and negative coordinate values.
 * @see PixelLineT
 * @ingroup cvadvanced
 */
typedef PixelLineT<int> PixelLineI;

/**
 * Definition of a vector holding pixel lines (with positive coordinate values).
 * @see PixelLine
 * @ingroup cvadvanced
 */
typedef std::vector<PixelLine> PixelLines;

/**
 * Definition of a vector holding pixel lines (with positive and negative coordinate values).
 * @see PixelLineT
 * @ingroup cvadvanced
 */
typedef std::vector<PixelLineI> PixelLinesI;

/**
 * This class implements a 2D line with pixel precision.
 * @tparam T The data type that is used to store the elements of a pixel coordinate
 * @see PixelPosition, PixelPositionI
 * @ingroup cvadvanced
 */
template <typename T>
class PixelLineT
{
	public:

		/**
		 * Creates an invalid line object.
		 */
		inline PixelLineT();

		/**
		 * Creates a new line object by two given end points.
		 * @param p0 First line end point
		 * @param p1 Second line end point
		 */
		inline PixelLineT(const PixelPositionT<T>& p0, const PixelPositionT<T>& p1);

		/**
		 * Creates a new line object by two given end points.
		 * @param x0 First horizontal line end point
		 * @param y0 First vertical line end point
		 * @param x1 Second horizontal line end point
		 * @param y1 Second vertical line end point
		 */
		inline PixelLineT(const T& x0, const T& y0, const T& x1, const T& y1);

		/**
		 * Returns the first end point of this line.
		 * @return First end point
		 */
		inline const PixelPositionT<T>& p0() const;

		/**
		 * Returns the second end point of this line.
		 * @return Second end point
		 */
		inline const PixelPositionT<T>& p1() const;

		/**
		 * Returns whether this line is horizontal.
		 * @return True, if so
		 */
		inline bool isHorizontal() const;

		/**
		 * Returns whether this line is vertical.
		 * @return True, if so
		 */
		inline bool isVertical() const;

		/**
		 * Returns whether this line is a point.
		 * @return True, if so
		 */
		inline bool isPoint() const;

		/**
		 * Calculates the intersection between this line and a horizontal scan line.
		 * Beware: Make sure that the vertical scan line really intersects this line.
		 * @param y Vertical position of the scan line
		 * @param x Resulting horizontal intersection position
		 * @return True, if an intersection could be found
		 */
		bool horizontalIntersection(const T y, T& x) const;

		/**
		 * Returns whether this line holds two valid end points.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Multiplies two line objects and returns the scalar product.
		 * @param line Second line to multiply
		 * @return Scalar product
		 */
		inline typename SignedTyper<T>::Type operator*(const PixelLineT<T>& line) const;

		/**
		 * Returns whether two line objects are equal.
		 * @param line Second line object to compare
		 * @return True, if so
		 */
		inline bool operator==(const PixelLineT<T>& line) const;

		/**
		 * Returns whether two line objects are not equal.
		 * @param line Second line object to compare
		 * @return True, if so
		 */
		inline bool operator!=(const PixelLineT<T>& line) const;

		/**
		 * Returns whether this line holds two valid end points.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/// First line end point.
		PixelPositionT<T> lineP0;

		/// Second line end point.
		PixelPositionT<T> lineP1;

		/// Lower vertical position.
		T lineMinY;

		/// Upper vertical position.
		T lineMaxY;
};

template <typename T>
inline PixelLineT<T>::PixelLineT()
{
	// nothing to do here
}

template <typename T>
inline PixelLineT<T>::PixelLineT(const PixelPositionT<T>& p0, const PixelPositionT<T>& p1) :
	lineP0(p0),
	lineP1(p1),
	lineMinY(min(p0.y(), p1.y())),
	lineMaxY(max(p0.y(), p1.y()))
{
	// nothing to do here
}

template <typename T>
inline PixelLineT<T>::PixelLineT(const T& x0, const T& y0, const T& x1, const T& y1) :
	lineP0(x0, y0),
	lineP1(x1, y1),
	lineMinY(min(y0, y1)),
	lineMaxY(max(y0, y1))
{
	// nothing to do here
}

template <typename T>
inline const PixelPositionT<T>& PixelLineT<T>::p0() const
{
	return lineP0;
}

template <typename T>
inline const PixelPositionT<T>& PixelLineT<T>::p1() const
{
	return lineP1;
}

template <typename T>
inline bool PixelLineT<T>::isHorizontal() const
{
	ocean_assert(isValid());
	return lineP0.y() == lineP1.y();
}

template <typename T>
inline bool PixelLineT<T>::isVertical() const
{
	ocean_assert(isValid());
	return lineP0.x() == lineP1.x();
}

template <typename T>
inline bool PixelLineT<T>::isPoint() const
{
	ocean_assert(isValid());
	return lineP0 == lineP1;
}

template <typename T>
bool PixelLineT<T>::horizontalIntersection(const T y, T& x) const
{
	ocean_assert(isValid());
	ocean_assert(lineMinY <= lineMaxY);

	// y == p0.y + (p1.y - p0.y) * t
	// t = (y - p0.y) / (p1.y - p0.y)

	// x = p0.x + (p1.x - p0.x) * t

	if (y < lineMinY || y > lineMaxY)
	{
		return false;
	}

	if (lineP0.y() == lineP1.y())
	{
		x = min(lineP0.x(), lineP1.x());
	}
	else
	{
		typedef typename SignedTyper<T>::Type SignedType;

		const float t = float(int(y) - SignedType(lineP0.y())) / float(SignedType(lineP1.y()) - SignedType(lineP0.y()));
		x = lineP0.x() + T(float((SignedType(lineP1.x()) - SignedType(lineP0.x()))) * t + 0.5f);
	}

	return true;
}

template <typename T>
inline bool PixelLineT<T>::isValid() const
{
	return lineP0.isValid() && lineP1.isValid();
}

template <typename T>
inline typename SignedTyper<T>::Type PixelLineT<T>::operator*(const PixelLineT<T>& line) const
{
	ocean_assert(isValid() && line.isValid());

	typedef typename SignedTyper<T>::Type SignedType;

	const SignedType vx0 = SignedType(lineP1.x()) - SignedType(lineP0.x());
	const SignedType vy0 = SignedType(lineP1.y()) - SignedType(lineP0.y());

	const SignedType vx1 = SignedType(line.lineP1.x()) - SignedType(line.lineP0.x());
	const SignedType vy1 = SignedType(line.lineP1.y()) - SignedType(line.lineP0.y());

	return vx0 * vy1 - vx1 * vy0;
}

template <typename T>
inline bool PixelLineT<T>::operator==(const PixelLineT<T>& line) const
{
	return lineP0 == line.lineP0 && lineP1 == line.lineP1;
}

template <typename T>
inline bool PixelLineT<T>::operator!=(const PixelLineT<T>& line) const
{
	return !(*this == line);
}

template <typename T>
inline PixelLineT<T>::operator bool() const
{
	return isValid();
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_PIXEL_LINE_H
