/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_BOX_2_H
#define META_OCEAN_MATH_BOX_2_H

#include "ocean/math/Math.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Triangle2.h"
#include "ocean/math/Vector2.h"

#include "ocean/base/Utilities.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class BoxT2;

/**
 * Definition of the Box2 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see BoxT2
 * @ingroup math
 */
typedef BoxT2<Scalar> Box2;

/**
 * Instantiation of the BoxT2 template class using a double precision float data type.
 * @see BoxT2
 * @ingroup math
 */
typedef BoxT2<double> BoxD2;

/**
 * Instantiation of the BoxT2 template class using a single precision float data type.
 * @see BoxT2
 * @ingroup math
 */
typedef BoxT2<float> BoxF2;

/**
 * Definition of a typename alias for vectors with BoxT2 objects.
 * @see BoxT2
 * @ingroup math
 */
template <typename T>
using BoxesT2 = std::vector<BoxT2<T>>;

/**
 * Definition of a vector holding Box2 objects.
 * @see Box2
 * @ingroup math
 */
typedef std::vector<Box2> Boxes2;

/**
 * This class implements an axis aligned 2D box object.
 * @tparam T Data type used to represent coordinates
 * @see Box2, BoxF2, BoxD2.
 * @ingroup math
 */
template <typename T>
class BoxT2
{
	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates an invalid box object.
		 */
		BoxT2();

		/**
		 * Creates a new box object by two opposite corner positions.
		 * @param first The first 2D corner position, (-infinity, infinity)x(-infinity, infinity)
		 * @param second The second 2D corner position (opposite to the first position), (-infinity, infinity)x(-infinity, infinity)
		 */
		BoxT2(const VectorT2<T>& first, const VectorT2<T>& second);

		/**
		 * Creates a new box object by the center position and the box's dimension.
		 * @param center The center position of the box to create
		 * @param width The width or horizontal dimension of the box, with range [0, infinity)
		 * @param height The height or vertical dimension of the box, with range [0, infinity)
		 */
		BoxT2(const VectorT2<T>& center, const T width, const T height);

		/**
		 * Creates a new box object by the center position and the box's dimension.
		 * @param width The width or horizontal dimension of the box, with range [0, infinity)
		 * @param height The height or vertical dimension of the box, with range [0, infinity)
		 * @param topLeft The top left location of this bounding box, with range (-infinity, infinity)x(-infinity, infinity)
		 */
		BoxT2(const T width, const T height, const VectorT2<T>& topLeft);

		/**
		 * Creates a new box object by the four borders.
		 * @param left Left box border position, with range (-infinity, infinity)
		 * @param top Top box border position, with range (-infinity, infinity)
		 * @param right Right box border position, with range [left, infinity)
		 * @param bottom Bottom box border position, with range [top, infinity)
		 */
		BoxT2(const T left, const T top, const T right, const T bottom);

		/**
		 * Creates a new box object by a set of points.
		 * The resulting box will cover all given points.<br>
		 * If no point is provided, the resulting box will be invalid.
		 * @param points Points to create the box from, can be empty
		 */
		explicit BoxT2(const std::vector<VectorT2<T>>& points);

		/**
		 * Creates a new box object that covers a given triangle.
		 * @param triangle Triangle to create the box from
		 */
		inline explicit BoxT2(const TriangleT2<T>& triangle);

		/**
		 * Creates a new box object by a set of triangles.
		 * The resulting box will cover all given triangles.<br>
		 * If no triangle is provided, the resulting box will be invalid.
		 * @param triangles Triangles to create the box from, can be empty
		 */
		explicit BoxT2(const std::vector<TriangleT2<T>>& triangles);

		/**
		 * Creates a new box object by a set of points.
		 * The resulting box will cover all given points.<br>
		 * If no point is provided, the resulting box will be invalid.
		 * @param points The points to create the box from, can be nullptr if `number == 0`
		 * @param number The number of given points, with range [0, infinity)
		 */
		BoxT2(const VectorT2<T>* points, const size_t number);

		/**
		 * Returns the lower corner of this box.
		 * @return Lower corner
		 */
		inline const VectorT2<T>& lower() const;

		/**
		 * Returns the higher corner of this box.
		 * @return Higher corner
		 */
		inline const VectorT2<T>& higher() const;

		/**
		 * Returns the horizontal position of the lower corner of this box.
		 * @return Left position
		 */
		inline const T& left() const;

		/**
		 * Returns the vertical position of the lower corner of this box.
		 * @return Top position
		 */
		inline const T& top() const;

		/**
		 * Returns the horizontal position of the higher corner of this box.
		 * @return Left position
		 */
		inline const T& right() const;

		/**
		 * Returns the vertical position of the higher corner of this box.
		 * @return Top position
		 */
		inline const T& bottom() const;

		/**
		 * Returns the width of the box.
		 * @return Horizontal box dimension
		 */
		inline T width() const;

		/**
		 * Returns the height of the box.
		 * @return Vertical box dimension
		 */
		inline T height() const;

		/**
		 * Returns the center of this box.
		 * @return Box center
		 */
		inline VectorT2<T> center() const;

		/**
		 * Returns the dimension in x axis, which could e.g. be the width of this box.
		 * Beware: The result is undefined for an invalid box.
		 * @return Dimension in x axis
		 */
		inline T xDimension() const;

		/**
		 * Returns the dimension in x axis, which could e.g. be the height of this box.
		 * Beware: The result is undefined for an invalid box.
		 * @return Dimension in x axis
		 */
		inline T yDimension() const;

		/**
		 * Returns the area covered by this box.
		 * The result is undefined for invalid boxes.
		 * @return Box area
		 */
		inline T area() const;

		/**
		 * Returns whether a given point is inside the box.
		 * @param point The point to check
		 * @return True, if so
		 */
		inline bool isInside(const VectorT2<T>& point) const;

		/**
		 * Returns whether a given point is inside the box including a thin epsilon boundary.
		 * @param point The point to check
		 * @param epsilon The accuracy epsilon, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isInside(const VectorT2<T>& point, const T& epsilon) const;

		/**
		 * Returns whether a given box is entirely inside this box.
		 * @param box The box to check
		 * @return True, if so
		 */
		inline bool isInside(const BoxT2<T>& box) const;

		/**
		 * Returns whether a given box intersects this one.
		 * @param box The box to check
		 * @return True, if so
		 */
		inline bool intersects(const BoxT2<T>& box) const;

		/**
		 * Returns the intersection of two boxes.
		 * @param box Second box for the intersection
		 * @return Box defining the intersection
		 */
		BoxT2<T> intersection(const BoxT2<T>& box) const;

		/**
		 * Returns this box with corners rounded to integer locations.
		 * Beware: Ensure that this box is valid before calling the function.
		 * @return The box with rounded corners
		 */
		inline BoxT2<T> rounded() const;

		/**
		 * Returns one of the four corners of this 2D box.
		 * The corners have a counter-clockwise order: (left, top), (left, bottom), (right, bottom), (right top).
		 * @param index The index of the corner to return, with range [0, 3]
		 * @return The requested corner
		 */
		inline VectorT2<T> corner(const unsigned int index) const;

		/**
		 * Returns whether the box holds exactly one point.
		 * @return True, if so
		 */
		inline bool isPoint() const;

		/**
		 * Returns whether two box objects are equal up to an epsilon.
		 * @param box The box to compare with, can be invalid
		 * @param epsilon The accuracy epsilon, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isEqual(const BoxT2<T>& box, const T epsilon = NumericT<T>::eps()) const;

		/**
		 * Returns whether the box holds valid parameters.
		 * A valid box can have a dimension of zero.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns the combined box given by this one and a second box.
		 * @param right Second box
		 * @return Combined box
		 */
		BoxT2<T> operator+(const BoxT2<T>& right) const;

		/**
		 * Extends this box by a triangle (by the three points of the triangle).
		 * @param triangle The triangle that extends this box
		 * @return Extended box
		 */
		BoxT2<T> operator+(const TriangleT2<T>& triangle) const;

		/**
		 * Extends this box by another 2D point and returns the result.
		 * If the 2D point fits into this box the box is untouched, otherwise the dimension of the box will be extended.
		 * @param point The point to be added
		 * @return Extended box
		 */
		BoxT2<T> operator+(const VectorT2<T>& point) const;

		/**
		 * Extends this box by adding a border around the box.
		 * @param signedBorder The size of the signed border to extend the box, a positive border increases the box; a negative border decreases the box, with range (-infinity, infinity)
		 * @return The modified box
		 */
		BoxT2<T> operator+(const T signedBorder) const;

		/**
		 * Adds a box to this box.
		 * @param right Right box to add
		 * @return Reference to this enlarged box
		 */
		BoxT2<T>& operator+=(const BoxT2<T>& right);

		/**
		 * Adds a triangle (the three points of the triangle) to this box.
		 * @param triangle Triangle to add
		 * @return Reference to this enlarged box
		 */
		BoxT2<T>& operator+=(const TriangleT2<T>& triangle);

		/**
		 * Extends this box by another 2D point.
		 * If the 2D point fits into this box the box is untouched, otherwise the dimension of the box will be extended.
		 * @param point Point to be added
		 * @return Reference to this enlarged box
		 */
		BoxT2<T>& operator+=(const VectorT2<T>& point);

		/**
		 * Extends this box by adding a border around the box.
		 * This box needs to be valid.
		 * @param signedBorder The size of the signed border to extend the box, a positive border increases the box; a negative border decreases the box, with range (-infinity, infinity)
		 * @return Reference to this modified box
		 */
		BoxT2<T>& operator+=(const T signedBorder);

		/**
		 * Returns a new bounding box with scaled dimensions.
		 * The corners of this bounding box will be scaled with a scalar defining new scaled corners for the new bounding box object.<br>
		 * This box needs to be valid.
		 * @param factor Factor that is applied to each bounding box corner, with range (-infinity, infinity)
		 * @return Scaled bounding box
		 */
		inline BoxT2<T> operator*(const T factor) const;

		/**
		 * Scales this bounding box with a scalar factor.
		 * The corners of this bounding box will be scaled with a scalar.<br>
		 * This box needs to be valid.
		 * @param factor Factor that is applied to each bounding box corner, with range (-infinity, infinity)
		 * @return Reference to this scaled bounding box
		 */
		inline BoxT2<T>& operator*=(const T factor);

		/**
		 * Returns a new bounding box enclosing the transformed corners of this bounding box.
		 * The corners of this bounding box will be transformed with a given 3x3 transformation matrix (a homography).
		 * @param transformation The transformation to be applied to each bounding box corner, must be valid
		 * @return The transformed bounding box
		 */
		inline BoxT2<T> operator*(const SquareMatrixT3<T>& transformation) const;

		/**
		 * Transforms this bounding box with a transformation matrix.
		 * The corners of the bounding box will be transformed with a given 3x3 transformation matrix (a homography).
		 * @param transformation The transformation to be applied to each bounding box corner, must be valid
		 * @return Reference to this transformed bounding box
		 */
		inline BoxT2<T>& operator*=(const SquareMatrixT3<T>& transformation);

		/**
		 * Returns whether two boxes are identical.
		 * @param right Second box object
		 * @return True, if so
		 */
		inline bool operator==(const BoxT2<T>& right) const;

		/**
		 * Returns whether two boxes are not identical.
		 * @param right Second box object
		 * @return True, if so
		 */
		inline bool operator!=(const BoxT2<T>& right) const;

		/**
		 * Returns whether this box is not a default box.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Calculates the intersection of this bounding box (with floating point accuracy) and a second bounding box (with integer accuracy).
		 * The resulting intersection (with integer accuracy) will entirely enclose this floating point bounding box - unless not possible due to the given constraints e.g., 'constraintLeft', 'constraintTop', ...<br>
		 * If this box is invalid, than the provided integer bounding box will be returned.
		 * @param constraintLeft Most left (including) position of the resulting intersection, with range (-infinity, infinity)
		 * @param constraintTop Most top (including) position of the resulting intersection, with range (-infinity, infinity)
		 * @param constraintRight Most right (including) position of the resulting intersection, with range [left, infinity)
		 * @param constraintBottom Most bottom (including) position of the resulting intersection, with range [top, infinity)
		 * @param intersectionLeft Resulting horizontal start position of the intersection, with range [constraintLeft, constraintRight]
		 * @param intersectionTop Resulting vertical start position of the intersection, with range [constraintTop, constraintBottom]
		 * @param intersectionWidth Resulting width of the sub-region, with range [0, constraintRight - constraintLeft + 1]
		 * @param intersectionHeight Resulting height of the sub-region, with range [0, constraintBottom - constraintTop + 1]
		 * @return True, if the resulting intersection has an area larger than zero (if the intersection is valid)
		 */
		bool box2integer(const int constraintLeft, const int constraintTop, const int constraintRight, const int constraintBottom, int& intersectionLeft, int& intersectionTop, unsigned int& intersectionWidth, unsigned int& intersectionHeight) const;

		/**
		 * Calculates the intersection of this bounding box (with floating point accuracy) and a second bounding box (with integer accuracy).
		 * The resulting intersection (with integer accuracy) will entirely enclose this floating point bounding box - unless not possible due to the given constraints e.g., 'maximalWidth', 'maximalHeight', ...<br>
		 * If this box is invalid, than the provided integer bounding box will be returned.<br>
		 * Actually, this function returns box2integer(0, 0, maximalWidth - 1, maximalHeight - 1, intersectionLeft, intersectionTop, intersectionWidth, intersectionHeight)
		 * @param maximalWidth Maximal width of the resulting intersection, with range [0, infinity)
		 * @param maximalHeight Maximal height of the resulting intersection, with range [0, infinity)
		 * @param intersectionLeft Resulting horizontal start position of the intersection, with range [0, maximalWidth - 1]
		 * @param intersectionTop Resulting vertical start position of the intersection, with range [0, maximalHeight - 1]
		 * @param intersectionWidth Resulting width of the sub-region, with range [0, maximalWidth]
		 * @param intersectionHeight Resulting height of the sub-region, with range [0, maximalHeight]
		 * @return True, if the resulting intersection has an area larger than zero (if the intersection is valid)
		 */
		bool box2integer(const unsigned int maximalWidth, const unsigned int maximalHeight, unsigned int& intersectionLeft, unsigned int& intersectionTop, unsigned int& intersectionWidth, unsigned int& intersectionHeight) const;

		/**
		 * Calculates the intersection of this bounding box (with floating point accuracy) and a second bounding box (with integer accuracy).
		 * The resulting intersection (with integer accuracy) will entirely enclose this floating point bounding box - unless not possible due to the given constraints e.g., 'maximalWidth', 'maximalHeight', ...<br>
		 * If this box is invalid, than the provided integer bounding box will be returned.
		 * @param maximalWidth Maximal width of the sub-region, with range [0, infinity)
		 * @param maximalHeight Maximal height of the sub-region, with range [0, infinity)
		 * @param extraBorder Explicit border that will be added to the left, top, right and bottom borders of the resulting intersection (as long as the specified constraints 'maximalWidth' and 'maximalHeight' parameters are not violated)
		 * @param intersectionLeft Resulting horizontal start position of the intersection, with range [0, maximalWidth - 1]
		 * @param intersectionTop Resulting vertical start position of the intersection, with range [0, maximalHeight - 1]
		 * @param intersectionWidth Resulting width of the sub-region, with range [0, maximalWidth]
		 * @param intersectionHeight Resulting height of the sub-region, with range [0, maximalHeight]
		 * @return True, if the resulting intersection has an area larger than zero (if the intersection is valid)
		 */
		bool box2integer(const unsigned int maximalWidth, const unsigned int maximalHeight, const unsigned int extraBorder, unsigned int& intersectionLeft, unsigned int& intersectionTop, unsigned int& intersectionWidth, unsigned int& intersectionHeight) const;

	protected:

		/// Lower box corner.
		VectorT2<T> lower_;

		/// Higher box corner.
		VectorT2<T> higher_;
};

template <typename T>
BoxT2<T>::BoxT2() :
	lower_(NumericT<T>::maxValue(), NumericT<T>::maxValue()),
	higher_(NumericT<T>::minValue(), NumericT<T>::minValue())
{
	ocean_assert(!isValid());
}

template <typename T>
BoxT2<T>::BoxT2(const VectorT2<T>& first, const VectorT2<T>& second) :
	lower_(min(first.x(), second.x()), min(first.y(), second.y())),
	higher_(max(first.x(), second.x()), max(first.y(), second.y()))
{
	ocean_assert(isValid());
}

template <typename T>
BoxT2<T>::BoxT2(const VectorT2<T>& center, const T width, const T height) :
	lower_(center.x() - width * T(0.5), center.y() - height * T(0.5)),
	higher_(center.x() + width * T(0.5), center.y() + height * T(0.5))
{
	ocean_assert(isValid());
}

template <typename T>
BoxT2<T>::BoxT2(const T width, const T height, const VectorT2<T>& topLeft) :
	lower_(topLeft),
	higher_(topLeft.x() + width, topLeft.y() + height)
{
	ocean_assert(isValid());
}

template <typename T>
BoxT2<T>::BoxT2(const T left, const T top, const T right, const T bottom) :
	lower_(min(left, right), min(top, bottom)),
	higher_(max(left, right), max(top, bottom))
{
	// nothing to do here
}

template <typename T>
BoxT2<T>::BoxT2(const std::vector<VectorT2<T>>& points) :
	lower_(NumericT<T>::maxValue(), NumericT<T>::maxValue()),
	higher_(NumericT<T>::minValue(), NumericT<T>::minValue())
{
	for (typename std::vector<VectorT2<T>>::const_iterator i = points.begin(); i != points.end(); ++i)
	{
		if (i->x() < lower_.x())
		{
			lower_.x() = i->x();
		}

		if (i->x() > higher_.x())
		{
			higher_.x() = i->x();
		}

		if (i->y() < lower_.y())
		{
			lower_.y() = i->y();
		}

		if (i->y() > higher_.y())
		{
			higher_.y() = i->y();
		}
	}
}

template <typename T>
BoxT2<T>::BoxT2(const std::vector<TriangleT2<T>>& triangles) :
	lower_(NumericT<T>::maxValue(), NumericT<T>::maxValue()),
	higher_(NumericT<T>::minValue(), NumericT<T>::minValue())
{
	for (const TriangleT2<T>& triangle : triangles)
	{
		ocean_assert(triangle.isValid());

		*this += triangle.point0();
		*this += triangle.point1();
		*this += triangle.point2();
	}
}

template <typename T>
BoxT2<T>::BoxT2(const VectorT2<T>* points, const size_t number) :
	lower_(NumericT<T>::maxValue(), NumericT<T>::maxValue()),
	higher_(NumericT<T>::minValue(), NumericT<T>::minValue())
{
	const VectorT2<T>* const pointsEnd = points + number;

	while (points != pointsEnd)
	{
		if (points->x() < lower_.x())
		{
			lower_.x() = points->x();
		}

		if (points->x() > higher_.x())
		{
			higher_.x() = points->x();
		}

		if (points->y() < lower_.y())
		{
			lower_.y() = points->y();
		}

		if (points->y() > higher_.y())
		{
			higher_.y() = points->y();
		}

		++points;
	}
}

template <typename T>
inline BoxT2<T>::BoxT2(const TriangleT2<T>& triangle) :
	lower_(NumericT<T>::maxValue(), NumericT<T>::maxValue()),
	higher_(NumericT<T>::minValue(), NumericT<T>::minValue())
{
	*this += triangle.point0();
	*this += triangle.point1();
	*this += triangle.point2();
}

template <typename T>
inline const VectorT2<T>& BoxT2<T>::lower() const
{
	ocean_assert(isValid());
	return lower_;
}

template <typename T>
inline const VectorT2<T>& BoxT2<T>::higher() const
{
	ocean_assert(isValid());
	return higher_;
}

template <typename T>
inline const T& BoxT2<T>::left() const
{
	ocean_assert(isValid());
	return lower_.x();
}

template <typename T>
inline const T& BoxT2<T>::top() const
{
	ocean_assert(isValid());
	return lower_.y();
}

template <typename T>
inline const T& BoxT2<T>::right() const
{
	ocean_assert(isValid());
	return higher_.x();
}

template <typename T>
inline const T& BoxT2<T>::bottom() const
{
	ocean_assert(isValid());
	return higher_.y();
}

template <typename T>
inline T BoxT2<T>::width() const
{
	ocean_assert(isValid());
	return higher_.x() - lower_.x();
}

template <typename T>
inline T BoxT2<T>::height() const
{
	ocean_assert(isValid());
	return higher_.y() - lower_.y();
}

template <typename T>
inline VectorT2<T> BoxT2<T>::center() const
{
	ocean_assert(isValid());
	return VectorT2<T>((higher_.x() + lower_.x()) * T(0.5), (higher_.y() + lower_.y()) * T(0.5));
}

template <typename T>
inline T BoxT2<T>::xDimension() const
{
	ocean_assert(isValid());
	return higher_.x() - lower_.x();
}

template <typename T>
inline T BoxT2<T>::yDimension() const
{
	ocean_assert(isValid());
	return higher_.y() - lower_.y();
}

template <typename T>
inline T BoxT2<T>::area() const
{
	ocean_assert(isValid());
	return xDimension() * yDimension();
}

template <typename T>
inline bool BoxT2<T>::isInside(const VectorT2<T>& point) const
{
	return point.x() >= lower_.x() && point.x() <= higher_.x() && point.y() >= lower_.y() && point.y() <= higher_.y();
}

template <typename T>
inline bool BoxT2<T>::isInside(const VectorT2<T>& point, const T& epsilon) const
{
	ocean_assert(epsilon >= 0);

	return point.x() + epsilon >= lower_.x()
				&& point.x() - epsilon <= higher_.x()
				&& point.y() + epsilon >= lower_.y()
				&& point.y() - epsilon <= higher_.y();
}

template <typename T>
inline bool BoxT2<T>::isInside(const BoxT2<T>& box) const
{
	return isInside(box.lower()) && isInside(box.higher());
}

template <typename T>
inline bool BoxT2<T>::intersects(const BoxT2<T>& box) const
{
	return !(lower_.x() > box.higher_.x() || box.lower_.x() > higher_.x()
				|| lower_.y() > box.higher_.y() || box.lower_.y() > higher_.y());
}

template <typename T>
BoxT2<T> BoxT2<T>::intersection(const BoxT2<T>& box) const
{
	const T xLower = max(lower_.x(), box.lower_.x());
	const T yLower = max(lower_.y(), box.lower_.y());
	const T xHigher = min(higher_.x(), box.higher_.x());
	const T yHigher = min(higher_.y(), box.higher_.y());

	if (xLower > xHigher || yLower > yHigher)
	{
		return BoxT2<T>();
	}

	return BoxT2<T>(VectorT2<T>(xLower, yLower), VectorT2<T>(xHigher, yHigher));
}

template <typename T>
inline BoxT2<T> BoxT2<T>::rounded() const
{
	ocean_assert(isValid());

	const T roundedLeft = T(NumericT<T>::round32(left()));
	const T roundedTop = T(NumericT<T>::round32(top()));

	const T roundedRight = T(NumericT<T>::round32(right()));
	const T roundedBottom = T(NumericT<T>::round32(bottom()));

	return BoxT2<T>(roundedLeft, roundedTop, roundedRight, roundedBottom);
}

template <typename T>
inline VectorT2<T> BoxT2<T>::corner(const unsigned int index) const
{
	ocean_assert(index <= 3u);

	switch (index)
	{
		case 1u:
			return VectorT2<T>(left(), bottom());

		case 2u:
			return VectorT2<T>(right(), bottom());

		case 3u:
			return VectorT2<T>(right(), top());
	}

	ocean_assert(index == 0u);
	return VectorT2<T>(left(), top());
}

template <typename T>
inline bool BoxT2<T>::isPoint() const
{
	return lower_.x() == higher_.x() && lower_.y() == higher_.y();
}

template <typename T>
inline bool BoxT2<T>::isEqual(const BoxT2<T>& box, const T epsilon) const
{
	return lower_.isEqual(box.lower_, epsilon) && higher_.isEqual(box.higher_, epsilon);
}

template <typename T>
inline bool BoxT2<T>::isValid() const
{
	return lower_.x() <= higher_.x() && lower_.y() <= higher_.y();
}

template <typename T>
BoxT2<T> BoxT2<T>::operator+(const BoxT2<T>& right) const
{
	BoxT2<T> result(*this);
	result += right;
	return result;
}

template <typename T>
BoxT2<T> BoxT2<T>::operator+(const TriangleT2<T>& triangle) const
{
	BoxT2<T> result(*this);

	result += triangle.point0();
	result += triangle.point1();
	result += triangle.point2();

	return result;
}

template <typename T>
BoxT2<T> BoxT2<T>::operator+(const VectorT2<T>& point) const
{
	BoxT2<T> result(*this);
	result += point;

	return result;
}

template <typename T>
BoxT2<T> BoxT2<T>::operator+(const T signedBorder) const
{
	BoxT2<T> result(*this);
	result += signedBorder;

	return result;
}

template <typename T>
BoxT2<T>& BoxT2<T>::operator+=(const BoxT2<T>& right)
{
	ocean_assert(right.isValid());

	lower_.x() = min(lower_.x(), right.lower_.x());
	lower_.y() = min(lower_.y(), right.lower_.y());
	higher_.x() = max(higher_.x(), right.higher_.x());
	higher_.y() = max(higher_.y(), right.higher_.y());

	return *this;
}

template <typename T>
BoxT2<T>& BoxT2<T>::operator+=(const TriangleT2<T>& triangle)
{
	*this += triangle.point0();
	*this += triangle.point1();
	*this += triangle.point2();

	return *this;
}

template <typename T>
BoxT2<T>& BoxT2<T>::operator+=(const VectorT2<T>& point)
{
	lower_.x() = min(lower_.x(), point.x());
	lower_.y() = min(lower_.y(), point.y());
	higher_.x() = max(higher_.x(), point.x());
	higher_.y() = max(higher_.y(), point.y());

	return *this;
}

template <typename T>
BoxT2<T>& BoxT2<T>::operator+=(const T signedBorder)
{
	ocean_assert(isValid());

	if (signedBorder >= 0)
	{
		lower_.x() -= signedBorder;
		lower_.y() -= signedBorder;

		higher_.x() += signedBorder;
		higher_.y() += signedBorder;
	}
	else
	{
		const VectorT2<T> currentCenter = center();

		lower_.x() = std::min(lower_.x() - signedBorder, currentCenter.x());
		lower_.y() = std::min(lower_.y() - signedBorder, currentCenter.y());

		higher_.x() = std::max(currentCenter.x(), higher_.x() + signedBorder);
		higher_.y() = std::max(currentCenter.y(), higher_.y() + signedBorder);
	}

	return *this;
}

template <typename T>
inline BoxT2<T> BoxT2<T>::operator*(const T factor) const
{
	ocean_assert(isValid());

	return BoxT2<T>(lower_ * factor, higher_ * factor);
}

template <typename T>
inline BoxT2<T>& BoxT2<T>::operator*=(const T factor)
{
	ocean_assert(isValid());

	*this = *this * factor;
	return *this;
}

template <typename T>
inline BoxT2<T> BoxT2<T>::operator*(const SquareMatrixT3<T>& transformation) const
{
	ocean_assert(isValid() && !transformation.isSingular());

	BoxT2<T> result;

	result += transformation * VectorT2<T>(left(), bottom());
	result += transformation * VectorT2<T>(right(), bottom());
	result += transformation * VectorT2<T>(right(), top());
	result += transformation * VectorT2<T>(left(), top());

	return result;
}

template <typename T>
inline BoxT2<T>& BoxT2<T>::operator*=(const SquareMatrixT3<T>& transformation)
{
	*this = *this * transformation;
	return *this;
}

template <typename T>
inline bool BoxT2<T>::operator==(const BoxT2<T>& right) const
{
	return isEqual(right, NumericT<T>::eps());
}

template <typename T>
inline bool BoxT2<T>::operator!=(const BoxT2<T>& right) const
{
	return !(*this == right);
}

template <typename T>
inline BoxT2<T>::operator bool() const
{
	return isValid();
}

template <typename T>
bool BoxT2<T>::box2integer(const int constraintLeft, const int constraintTop, const int constraintRight, const int constraintBottom, int& intersectionLeft, int& intersectionTop, unsigned int& intersectionWidth, unsigned int& intersectionHeight) const
{
	ocean_assert(isValid());
	ocean_assert(constraintLeft <= constraintRight && constraintTop <= constraintBottom);

	if (!isValid())
	{
		intersectionLeft = constraintLeft;
		intersectionTop = constraintTop;

		intersectionWidth = (unsigned int)(constraintRight - constraintLeft) + 1u;
		intersectionHeight = (unsigned int)(constraintBottom - constraintTop) + 1u;

		return true;
	}

	if (left() > T(constraintRight) || top() > T(constraintBottom) || right() < T(constraintLeft) || bottom() < T(constraintTop))
	{
		return false;
	}

	intersectionLeft = minmax(constraintLeft, int(NumericT<T>::floor(left())), constraintRight);
	intersectionTop = minmax(constraintTop, int(NumericT<T>::floor(top())), constraintBottom);

	const int subRegionRight = minmax(intersectionLeft, int(NumericT<T>::floor(right())), constraintRight);
	const int subRegionBottom = minmax(intersectionTop, int(NumericT<T>::floor(bottom())), constraintBottom);

	ocean_assert(intersectionLeft <= subRegionRight);
	ocean_assert(intersectionTop <= subRegionBottom);

	intersectionWidth = (unsigned int)(subRegionRight - intersectionLeft) + 1u;
	intersectionHeight = (unsigned int)(subRegionBottom - intersectionTop) + 1u;

	ocean_assert(T(intersectionLeft) <= max(T(constraintLeft), left()));
	ocean_assert(T(intersectionTop) <= max(T(constraintTop), top()));
	ocean_assert(T(intersectionLeft) + T(intersectionWidth) >= min(T(constraintRight), right()));
	ocean_assert(T(intersectionTop) + T(intersectionHeight) >= min(T(constraintBottom), bottom()));

	return true;
}

template <typename T>
bool BoxT2<T>::box2integer(const unsigned int maximalWidth, const unsigned int maximalHeight, unsigned int& intersectionLeft, unsigned int& intersectionTop, unsigned int& intersectionWidth, unsigned int& intersectionHeight) const
{
	ocean_assert(isValid());

	if (maximalWidth == 0u || maximalHeight == 0u)
	{
		return false;
	}

	if (!isValid())
	{
		intersectionLeft = 0u;
		intersectionTop = 0u;
		intersectionWidth = maximalWidth;
		intersectionHeight = maximalHeight;
	}

	if (left() >= T(maximalWidth) || top() >= T(maximalHeight) || right() < 0 || bottom() < 0)
	{
		return false;
	}

	intersectionLeft = minmax(0, int(left()), int(maximalWidth - 1u));
	intersectionTop = minmax(0, int(top()), int(maximalHeight - 1u));
	const unsigned int intersectionRight = minmax(int(intersectionLeft), int(right()), int(maximalWidth - 1u));
	const unsigned int intersectionBottom = minmax(int(intersectionTop), int(bottom()), int(maximalHeight - 1u));

	intersectionWidth = intersectionRight - intersectionLeft + 1u;
	intersectionHeight = intersectionBottom - intersectionTop + 1u;

	return true;
}

template <typename T>
bool BoxT2<T>::box2integer(const unsigned int maximalWidth, const unsigned int maximalHeight, const unsigned int extraBorder, unsigned int& intersectionLeft, unsigned int& intersectionTop, unsigned int& intersectionWidth, unsigned int& intersectionHeight) const
{
	if (!box2integer(maximalWidth, maximalHeight, intersectionLeft, intersectionTop, intersectionWidth, intersectionHeight))
	{
		return false;
	}

	if (extraBorder == 0u)
	{
		return true;
	}

	ocean_assert(intersectionWidth >= 0u);
	ocean_assert(intersectionHeight >= 0u);

	unsigned int intersectionRight = intersectionLeft + intersectionWidth - 1u;
	unsigned int intersectionBottom = intersectionTop + intersectionHeight - 1u;

	ocean_assert(intersectionRight < maximalWidth);
	ocean_assert(intersectionTop < maximalHeight);

	intersectionRight = min(intersectionRight + extraBorder, maximalWidth - 1u);
	intersectionBottom = min(intersectionBottom + extraBorder, maximalHeight - 1u);

	intersectionLeft = max(0, int(intersectionLeft) - int(extraBorder));
	intersectionTop = max(0, int(intersectionTop) - int(extraBorder));

	intersectionWidth = intersectionRight - intersectionLeft + 1u;
	intersectionHeight = intersectionBottom - intersectionTop + 1u;

	return true;
}

}

#endif // META_OCEAN_MATH_BOX_2_H
