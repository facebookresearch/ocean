/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_PIXEL_BOUNDING_BOX_H
#define META_OCEAN_CV_PIXEL_BOUNDING_BOX_H

#include "ocean/cv/CV.h"
#include "ocean/cv/PixelPosition.h"

namespace Ocean
{

namespace CV
{

// Forward declaration.
template <typename T> class PixelBoundingBoxT;

/**
 * Definition of the default PixelBoundingBox object with data type allowing only positive coordinate values.
 * @see PixelPositionT
 * @ingroup cv
 */
typedef PixelBoundingBoxT<unsigned int> PixelBoundingBox;

/**
 * Definition of a PixelBoundingBox object with data type allowing positive and negative coordinate values.
 * @see PixelPositionT
 * @ingroup cv
 */
typedef PixelBoundingBoxT<int> PixelBoundingBoxI;

/**
 * Definition of a vector holding bounding box objects with only positive coordinate values.
 * @see PixelBoundingBox
 * @ingroup cv
 */
typedef std::vector<PixelBoundingBox> PixelBoundingBoxes;

/**
 * Definition of a vector holding bounding box objects with positive and negative coordinate values.
 * @see PixelBoundingBoxI
 * @ingroup cv
 */
typedef std::vector<PixelBoundingBoxI> PixelBoundingBoxesI;

/**
 * This class implements a 2D bounding box with pixel precision.
 * @ingroup cv
 */
template <typename T>
class PixelBoundingBoxT
{
	public:

		/**
		 * Creates a empty bounding box.
		 */
		inline PixelBoundingBoxT();

		/**
		 * Copy constructor.
		 * @param boundingBox The bounding box to copy
		 */
		inline PixelBoundingBoxT(const PixelBoundingBoxT<T>& boundingBox);

		/**
		 * Creates a bounding box covering one point only.
		 * @param point The point the box will cover
		 */
		inline explicit PixelBoundingBoxT(const PixelPositionT<T>& point);

		/**
		 * Creates a bounding box covering several points.
		 * @param points The points the box will cover
		 */
		inline explicit PixelBoundingBoxT(const std::vector<PixelPositionT<T>>& points);

		/**
		 * Creates a bounding box with specified dimension.
		 * @param left Left (including) pixel position
		 * @param top Top (including) pixel position
		 * @param right Right (including) pixel position, with range [left, infinity) to create a valid bounding box
		 * @param bottom Bottom (including) pixel position, with range [top, infinity) to create a valid bounding box
		 */
		inline PixelBoundingBoxT(const T left, const T top, const T right, const T bottom);

		/**
		 * Creates a bounding box by a given top left position and a width and height of the bounding box.
		 * @param topLeft Top left position of the box
		 * @param width The width of the bounding box in pixel with range [1, infinity)
		 * @param height The height of the bounding box in pixel, with range [1, infinity)
		 */
		inline PixelBoundingBoxT(const PixelPositionT<T>& topLeft, const unsigned int width, const unsigned int height);

		/**
		 * Creates a bounding box covering two points.
		 * @param point0 The first point that will be converted by the bounding box
		 * @param point1 The second point that will be converted by the bounding box
		 */
		inline PixelBoundingBoxT(const PixelPositionT<T>& point0, const PixelPositionT<T>& point1);

		/**
		 * Returns the left (including) pixel position of this bounding box.
		 * @return Left position
		 */
		inline T left() const;

		/**
		 * Returns the top (including) pixel position of this bounding box.
		 * @return Top position
		 */
		inline T top() const;

		/**
		 * Returns the right (including) pixel position of this bounding box.
		 * @return Right position
		 */
		inline T right() const;

		/**
		 * Returns the right (excluding) pixel position of this bounding box.
		 * @return First pixel to the right of the bounding box
		 */
		inline T rightEnd() const;

		/**
		 * Returns the bottom (including) position of this bounding box.
		 * @return Bottom position
		 */
		inline T bottom() const;

		/**
		 * Returns the bottom (excluding) pixel position of this bounding box.
		 * @return First pixel below the bounding box
		 */
		inline T bottomEnd() const;

		/**
		 * Returns the top left corner of this bounding box.
		 * @return Top left corner
		 */
		inline PixelPositionT<T> topLeft() const;

		/**
		 * Returns the top right corner of this bounding box.
		 * @return Top right corner
		 */
		inline PixelPositionT<T> topRight() const;

		/**
		 * Returns the bottom left corner of this bounding box.
		 * @return bottom left corner
		 */
		inline PixelPositionT<T> bottomLeft() const;

		/**
		 * Returns the bottom right corner of this bounding box.
		 * @return Bottom right corner
		 */
		inline PixelPositionT<T> bottomRight() const;

		/**
		 * Returns the width (the number of horizontal including pixels) of this bounding box.
		 * @return Bounding box width
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height (the number of vertical including pixels) of this bounding box.
		 * @return Bounding box height
		 */
		inline unsigned int height() const;

		/**
		 * Returns the area (the number of including pixels) this bounding box covers.
		 * If this bounding box is invalid, than zero is returned.
		 * @return Number of pixels or zero.
		 */
		inline unsigned int size() const;

		/**
		 * Returns whether a given point lies inside this bounding box.
		 * @param point The point to be checked, must be valid
		 * @return True, if so
		 */
		inline bool isInside(const PixelPositionT<T>& point) const;

		/**
		 * Returns whether a given bounding box lies entirely inside this bounding box.
		 * @param boundingBox The bounding box to be checked, must be valid
		 * @return True, if so
		 */
		inline bool isInside(const PixelBoundingBoxT<T>& boundingBox) const;

		/**
		 * Returns whether a given bounding box has an intersection with this bounding box.
		 * @param boundingBox The bounding box to be checked, must be valid
		 * @return True, if so
		 */
		inline bool hasIntersection(const PixelBoundingBoxT<T>& boundingBox) const;

		/**
		 * Returns whether a given bounding box is touching this bounding box.
		 * Two bounding boxes are touching if both boxes are intersecting or if both boxes have a touching edge.<br>
		 * When two bounding boxes are touching at the corners, the 'useNeighborhood4' property defines whether both boxes count as touching or not.
		 * @param boundingBox The bounding box to be checked, must be valid
		 * @param useNeighborhood8 True, to use a 8-neighborhood during the check; False, to use an 4-neighborhood during the check
		 * @return True, if so
		 */
		inline bool isTouching(const PixelBoundingBoxT<T>& boundingBox, const bool useNeighborhood8) const;

		/**
		 * Returns a new bounding box by extending this bounding box with a given number of pixel in each direction.
		 * @param pixels Number of pixels to be added to each bounding box boundary
		 * @param minLeft Minimal (including) left boundary, with minLeft <= left()
		 * @param minTop Minimal (including) top boundary, with minTop <= top()
		 * @param maxRight Minimal (including) right boundary, with maxRight >= right()
		 * @param maxBottom Minimal (including) bottom boundary, with maxBottom >= bottom()
		 * @return Extended bounding box
		 */
		inline PixelBoundingBoxT<T> extended(const unsigned int pixels, const T minLeft, const T minTop, const T maxRight, const T maxBottom) const;

		/**
		 * Returns whether this bounding box covers a valid pixel area.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Adds a new point to the bounding box.
		 * If the point lies inside the bounding box the box is unchanged, otherwise the box will be extended
		 * @param point The point extending the box
		 * @return Reference to this box
		 */
		inline PixelBoundingBoxT<T>& operator+=(const PixelPositionT<T>& point);

		/**
		 * Assignment operator override to match copy constructor
		 */
		inline PixelBoundingBoxT<T>& operator=(const PixelBoundingBoxT<T>& other) = default;

		/**
		 * Returns whether two bounding boxes are equal.
		 * @param box Second bounding box
		 * @return True, if so
		 */
		inline bool operator==(const PixelBoundingBoxT<T>& box) const;

		/**
		 * Returns whether two bounding boxes are not equal.
		 * @param box Second bounding box
		 * @return True, if so
		 */
		inline bool operator!=(const PixelBoundingBoxT<T>& box) const;

		/**
		 * Multiplies this bounding box by a specified scalar (multiplies left, top, right, and bottom location by the scalar).
		 * @param factor The multiplication factor, with range (-infinity, infinity)
		 * @return The new (bigger/multiplied) bounding box
		 */
		inline PixelBoundingBoxT<T> operator*(const T factor) const;

		/**
		 * Multiplies this bounding box by a specified scalar (multiplies left, top, right, and bottom location by the scalar).
		 * @param factor The multiplication factor, with range (-infinity, infinity)
		 * @return Reference to this modified bounding box
		 */
		inline PixelBoundingBoxT<T>& operator*=(const T factor);

		/**
		 * Divides this bounding box by a specified scalar (divides left, top, right, and bottom location by the scalar).
		 * @param factor The division factor, with range (-infinity, infinity) \ {0}
		 * @return The new (smaller/divided) bounding box
		 */
		inline PixelBoundingBoxT<T> operator/(const T factor) const;

		/**
		 * Divides this bounding box by a specified scalar (divides left, top, right, and bottom location by the scalar).
		 * @param factor The division factor, with range (-infinity, infinity) \ {0}
		 * @return Reference to this modified bounding box
		 */
		inline PixelBoundingBoxT<T>& operator/=(const T factor);

		/**
		 * Returns the union of two bounding boxes.
		 * @param box Second bounding box
		 * @return New union bounding box
		 */
		inline PixelBoundingBoxT<T> operator||(const PixelBoundingBoxT<T>& box) const;

		/**
		 * Returns the intersection of two bounding boxes.
		 * Beware: The intersection between a valid and an invalid bounding box is an invalid bounding box.
		 * @param box Second bounding box
		 * @return New intersection bounding box
		 */
		inline PixelBoundingBoxT<T> operator&&(const PixelBoundingBoxT<T>& box) const;

		/**
		 * Hash function.
		 * @param boundingBox The bounding box for which the hash value will be determined
		 * @return The resulting hash value
		 */
		inline size_t operator()(const PixelBoundingBoxT<T>& boundingBox) const;

		/**
		 * Returns whether this bounding box covers a valid pixel area.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/// Left (including) pixel position of this bounding box.
		T left_;

		/// Top (including) pixel position of this bounding box.
		T top_;

		/// Right (including) pixel position of this bounding box.
		T right_;

		/// Bottom (including) pixel position of this bounding box.
		T bottom_;
};

template <typename T>
inline PixelBoundingBoxT<T>::PixelBoundingBoxT() :
	left_(NumericT<T>::maxValue()),
	top_(NumericT<T>::maxValue()),
	right_(NumericT<T>::minValue()),
	bottom_(NumericT<T>::minValue())
{
	// nothing to do here
}

template <typename T>
inline PixelBoundingBoxT<T>::PixelBoundingBoxT(const PixelBoundingBoxT<T>& boundingBox) :
	left_(boundingBox.left_),
	top_(boundingBox.top_),
	right_(boundingBox.right_),
	bottom_(boundingBox.bottom_)
{
	// nothing to do here
}

template <typename T>
inline PixelBoundingBoxT<T>::PixelBoundingBoxT(const PixelPositionT<T>& point) :
	left_(point.x()),
	top_(point.y()),
	right_(point.x()),
	bottom_(point.y())
{
	// nothing to do here
}

template <typename T>
inline PixelBoundingBoxT<T>::PixelBoundingBoxT(const std::vector<PixelPositionT<T>>& points) :
	left_(NumericT<T>::maxValue()),
	top_(NumericT<T>::maxValue()),
	right_(NumericT<T>::minValue()),
	bottom_(NumericT<T>::minValue())
{
	for (typename std::vector<PixelPositionT<T>>::const_iterator i = points.begin(); i != points.end(); ++i)
	{
		*this += *i;
	}
}

template <typename T>
inline PixelBoundingBoxT<T>::PixelBoundingBoxT(const T left, const T top, const T right, const T bottom) :
	left_(left),
	top_(top),
	right_(right),
	bottom_(bottom)
{
	// nothing to do here
}

template <typename T>
inline PixelBoundingBoxT<T>::PixelBoundingBoxT(const PixelPositionT<T>& topLeft, const unsigned int width, const unsigned int height) :
	left_(topLeft.x()),
	top_(topLeft.y()),
	right_(topLeft.x() + T(width - 1u)),
	bottom_(topLeft.y() + T(height - 1u))
{
	ocean_assert(width >= 1u);
	ocean_assert(height >= 1u);
}

template <typename T>
inline PixelBoundingBoxT<T>::PixelBoundingBoxT(const PixelPositionT<T>& point0, const PixelPositionT<T>& point1) :
	left_(std::min(point0.x(), point1.x())),
	top_(std::min(point0.y(), point1.y())),
	right_(std::max(point0.x(), point1.x())),
	bottom_(std::max(point0.y(), point1.y()))
{
	ocean_assert(width() >= 1u);
	ocean_assert(height() >= 1u);

#ifdef OCEAN_DEBUG
	PixelBoundingBoxT<T> debugBoundingBox;
	debugBoundingBox += point0;
	debugBoundingBox += point1;

	ocean_assert(*this == debugBoundingBox);
#endif
}

template <typename T>
inline T PixelBoundingBoxT<T>::left() const
{
	ocean_assert(isValid());
	return left_;
}

template <typename T>
inline T PixelBoundingBoxT<T>::top() const
{
	ocean_assert(isValid());
	return top_;
}

template <typename T>
inline T PixelBoundingBoxT<T>::right() const
{
	ocean_assert(isValid());
	return right_;
}

template <typename T>
inline T PixelBoundingBoxT<T>::rightEnd() const
{
	ocean_assert(isValid());
	return right_ + T(1);
}

template <typename T>
inline T PixelBoundingBoxT<T>::bottom() const
{
	ocean_assert(isValid());
	return bottom_;
}

template <typename T>
inline T PixelBoundingBoxT<T>::bottomEnd() const
{
	ocean_assert(isValid());
	return bottom_ + T(1);
}

template <typename T>
inline PixelPositionT<T> PixelBoundingBoxT<T>::topLeft() const
{
	return PixelPositionT<T>(left_, top_);
}

template <typename T>
inline PixelPositionT<T> PixelBoundingBoxT<T>::topRight() const
{
	return PixelPositionT<T>(right_, top_);
}

template <typename T>
inline PixelPositionT<T> PixelBoundingBoxT<T>::bottomLeft() const
{
	return PixelPositionT<T>(left_, bottom_);
}

template <typename T>
inline PixelPositionT<T> PixelBoundingBoxT<T>::bottomRight() const
{
	return PixelPositionT<T>(right_, bottom_);
}

template <typename T>
inline unsigned int PixelBoundingBoxT<T>::width() const
{
	ocean_assert(isValid());
	return (unsigned int)(right_ - left_) + 1u;
}

template <typename T>
inline unsigned int PixelBoundingBoxT<T>::height() const
{
	ocean_assert(isValid());
	return (unsigned int)(bottom_ - top_) + 1u;
}

template <typename T>
inline unsigned int PixelBoundingBoxT<T>::size() const
{
	if (!isValid())
	{
		return 0u;
	}

	return width() * height();
}

template <typename T>
inline bool PixelBoundingBoxT<T>::isInside(const PixelPositionT<T>& point) const
{
	ocean_assert(isValid() && point.isValid());

	return point.x() >= left_ && point.y() >= top_ && point.x() <= right_ && point.y() <= bottom_;
}

template <typename T>
inline bool PixelBoundingBoxT<T>::isInside(const PixelBoundingBoxT<T>& boundingBox) const
{
	ocean_assert(isValid() && boundingBox.isValid());

	return boundingBox.left_ >= left_ && boundingBox.top_ >= top_ && boundingBox.right_ <= right_ && boundingBox.bottom_ <= bottom_;
}

template <typename T>
inline bool PixelBoundingBoxT<T>::hasIntersection(const PixelBoundingBoxT<T>& boundingBox) const
{
	ocean_assert(isValid() && boundingBox.isValid());

	// we have not an intersection if one box is entirely outside of the other box

	return !(left_ > boundingBox.right_ || boundingBox.left_ > right_
				|| top_ > boundingBox.bottom_ || boundingBox.top_ > bottom_);
}

template <typename T>
inline bool PixelBoundingBoxT<T>::isTouching(const PixelBoundingBoxT<T>& boundingBox, const bool useNeighborhood8) const
{
	ocean_assert(isValid() && boundingBox.isValid());

	if (left_ > boundingBox.right_ + T(1) || boundingBox.left_ > right_ + T(1)
				|| top_ > boundingBox.bottom_ + T(1) || boundingBox.top_ > bottom_ + T(1))
	{
		return false;
	}

	if (!useNeighborhood8)
	{
		// we need to check the case that both boxes touch at a corner

		if ((left_ == boundingBox.right_ + T(1) || right_ + T(1) == boundingBox.left_) && (top_ == boundingBox.bottom_ + T(1) || bottom_ + T(1) == boundingBox.top_))
		{
			return false;
		}
	}

	return true;
}

template <typename T>
inline PixelBoundingBoxT<T> PixelBoundingBoxT<T>::extended(const unsigned int pixels, const T minLeft, const T minTop, const T maxRight, const T maxBottom) const
{
	static_assert(sizeof(T) <= sizeof(int), "Invalid template data type T");

	ocean_assert(isValid());

	ocean_assert(minLeft <= left() && minTop <= top());
	ocean_assert(maxRight >= right() && maxBottom >= bottom());

	ocean_assert(minLeft <= maxRight);
	ocean_assert(minTop <= maxBottom);

	return PixelBoundingBoxT<T>(max(int(minLeft), int(left_) - int(pixels)),
								max(int(minTop), int(top_) - int(pixels)),
								min(maxRight, right_ + T(pixels)),
								min(maxBottom, bottom_ + T(pixels)));
}

template <typename T>
inline bool PixelBoundingBoxT<T>::isValid() const
{
	return right_ >= left_ && bottom_ >= top_;
}

template <typename T>
inline PixelBoundingBoxT<T>& PixelBoundingBoxT<T>::operator+=(const PixelPositionT<T>& point)
{
	ocean_assert(point.isValid());

	if (point.x() < left_)
	{
		left_ = point.x();
	}

	if (point.x() > right_)
	{
		right_ = point.x();
	}

	if (point.y() < top_)
	{
		top_ = point.y();
	}

	if (point.y() > bottom_)
	{
		bottom_ = point.y();
	}

	return *this;
}

template <typename T>
inline bool PixelBoundingBoxT<T>::operator==(const PixelBoundingBoxT<T>& box) const
{
	return left_ == box.left_ && top_ == box.top_ && right_ == box.right_ && bottom_ == box.bottom_;
}

template <typename T>
inline bool PixelBoundingBoxT<T>::operator!=(const PixelBoundingBoxT<T>& box) const
{
	return !(*this == box);
}

template <typename T>
inline PixelBoundingBoxT<T> PixelBoundingBoxT<T>::operator*(const T factor) const
{
	ocean_assert(isValid());

	return PixelBoundingBoxT<T>(left_ * factor, top_ * factor, right_ * factor, bottom_ * factor);
}

template <typename T>
inline PixelBoundingBoxT<T>& PixelBoundingBoxT<T>::operator*=(const T factor)
{
	ocean_assert(isValid());

	left_ *= factor;
	top_ *= factor;
	right_ *= factor;
	bottom_ *= factor;

	return *this;
}

template <typename T>
inline PixelBoundingBoxT<T> PixelBoundingBoxT<T>::operator/(const T factor) const
{
	ocean_assert(isValid());
	ocean_assert(factor != T(0));

	return PixelBoundingBoxT<T>(left_ / factor, top_ / factor, right_ / factor, bottom_ / factor);
}

template <typename T>
inline PixelBoundingBoxT<T>& PixelBoundingBoxT<T>::operator/=(const T factor)
{
	ocean_assert(isValid());
	ocean_assert(factor != T(0));

	left_ /= factor;
	top_ /= factor;
	right_ /= factor;
	bottom_ /= factor;

	return *this;
}

template <typename T>
inline PixelBoundingBoxT<T> PixelBoundingBoxT<T>::operator||(const PixelBoundingBoxT<T>& box) const
{
	if (isValid())
	{
		if (box.isValid())
		{
			return PixelBoundingBoxT<T>(min(left_, box.left_), min(top_, box.top_), max(right_, box.right_), max(bottom_, box.bottom_));
		}

		return *this;
	}

	return box;
}

template <typename T>
inline PixelBoundingBoxT<T> PixelBoundingBoxT<T>::operator&&(const PixelBoundingBoxT<T>& box) const
{
	if (isValid() && box.isValid())
	{
		return PixelBoundingBoxT<T>(max(left_, box.left_), max(top_, box.top_), min(right_, box.right_), min(bottom_, box.bottom_));
	}

	return PixelBoundingBoxT<T>();
}

template <typename T>
inline size_t PixelBoundingBoxT<T>::operator()(const PixelBoundingBoxT<T>& boundingBox) const
{
	size_t seed = std::hash<T>{}(boundingBox.left());
	seed ^= std::hash<T>{}(boundingBox.top()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(boundingBox.right()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(boundingBox.bottom()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

template <typename T>
inline PixelBoundingBoxT<T>::operator bool() const
{
	return isValid();
}

}

}

#endif // META_OCEAN_CV_PIXEL_BOUNDING_BOX_H
