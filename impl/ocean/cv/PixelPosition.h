/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_PIXEL_POSITION_H
#define META_OCEAN_CV_PIXEL_POSITION_H

#include "ocean/cv/CV.h"

#include "ocean/base/Utilities.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/Vector2.h"

#include <limits>

namespace Ocean
{

namespace CV
{

// Forward declaration.
template <typename T> class PixelPositionT;

/**
 * Definition of the default PixelPosition object with a data type allowing only positive coordinate values.
 * @see PixelPositionT
 * @ingroup cv
 */
typedef PixelPositionT<unsigned int> PixelPosition;

/**
 * Definition of a PixelPosition object with a data type allowing positive and negative coordinate values.
 * @see PixelPositionT
 * @ingroup cv
 */
typedef PixelPositionT<int> PixelPositionI;

/**
 * Definition of a vector holding pixel positions (with positive coordinate values).
 * @see PixelPosition
 * @ingroup cv
 */
typedef std::vector<PixelPosition> PixelPositions;

/**
 * Definition of a vector holding pixel positions (with positive and negative coordinate values).
 * @see PixelPositionI
 * @ingroup cv
 */
typedef std::vector<PixelPositionI> PixelPositionsI;

/**
 * This class implements a 2D pixel position with pixel precision.
 * @tparam T The data type that is used to store the elements of a pixel coordinate
 * @see PixelPosition, PixelPositionI
 * @ingroup cv
 */
template <typename T>
class PixelPositionT
{
	public:

		/**
		 * Definition of individual rough directions.
		 */
		enum RoughPixelDirection
		{
			/// Invalid direction.
			RPD_INVALID,
			/// Vertical direction.
			RPD_VERTICAL,
			/// Horizontal direction.
			RPD_HORIZONTAL,
			/// Vertical direction.
			RPD_DIAGONAL
		};

	public:

		/**
		 * Creates an invalid pixel position object with invalid coordinates.
		 */
		inline PixelPositionT();

		/**
		 * Copy constructor.
		 * @param position The position to copy
		 */
		inline PixelPositionT(const PixelPositionT<T>& position);

		/**
		 * Creates a new coordinate object by two given coordinate values.
		 * @param x Horizontal position in pixel
		 * @param y Vertical position in pixel
		 */
		inline PixelPositionT(const T& x, const T& y);

		/**
		 * Returns the horizontal coordinate position of this object.
		 * @return Horizontal coordinate position in pixel
		 * @see y().
		 */
		inline T x() const;

		/**
		 * Returns the vertical coordinate position of this object.
		 * @return Vertical coordinate position in pixel
		 * @see x().
		 */
		inline T y() const;

		/**
		 * Returns the horizontal coordinate position of this object.
		 * @return Horizontal coordinate position in pixel
		 * @see y().
		 */
		inline T& x();

		/**
		 * Returns the vertical coordinate position of this object.
		 * @return Vertical coordinate position in pixel
		 * @see x().
		 */
		inline T& y();

		/**
		 * Sets the two coordinate values of this object.
		 * @param x Horizontal coordinate value to set, in pixel
		 * @param y Vertical coordinate value to be set, in pixel
		 */
		inline void setPosition(const T& x, const T& y);

		/**
		 * Returns the square difference between two pixel positions.
		 * @param position Second position to determine the difference for
		 * @return Square difference in pixel
		 */
		inline unsigned int sqrDistance(const PixelPositionT<T>& position) const;

		/**
		 * Returns the position of the pixel neighbor to this position.
		 * @param pixelDirection The direction in which the neighbor will be located, must be valid
		 * @return The position of the neighbor pixel
		 */
		CV::PixelPositionT<T> neighbor(const CV::PixelDirection pixelDirection) const;

		/**
		 * Returns the pixel position north to this position.
		 * @return North pixel position
		 */
		inline PixelPositionT<T> north() const;

		/**
		 * Returns the pixel position north west to this position.
		 * @return North west pixel position
		 */
		inline PixelPositionT<T> northWest() const;

		/**
		 * Returns the pixel position west to this position.
		 * @return West pixel position
		 */
		inline PixelPositionT<T> west() const;

		/**
		 * Returns the pixel position south west to this position.
		 * @return South west pixel position
		 */
		inline PixelPositionT<T> southWest() const;

		/**
		 * Returns the pixel position south to this position.
		 * @return South pixel position
		 */
		inline PixelPositionT<T> south() const;

		/**
		 * Returns the pixel position south east to this position.
		 * @return South east pixel position
		 */
		inline PixelPositionT<T> southEast() const;

		/**
		 * Returns the pixel position east to this position.
		 * @return East pixel position
		 */
		inline PixelPositionT<T> east() const;

		/**
		 * Returns the pixel position north east to this position.
		 * @return North east pixel position
		 */
		inline PixelPositionT<T> northEast() const;

		/**
		 * Returns this position divided by two.
		 * @return Half position
		 */
		inline PixelPositionT<T> half() const;

		/**
		 * Returns this position multiplied by two.
		 * @return Double position
		 */
		inline PixelPositionT<T> twice() const;

		/**
		 * Returns a sub-pixel accuracy vector of this pixel position.
		 * @return Vector object
		 */
		inline Vector2 vector() const;

		/**
		 * Returns whether this pixel position object holds two valid parameters.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this pixel position is equal to a second pixel position or is the direct neighbor in an 8-neighborhood.
		 * @param position Second pixel position to check
		 * @return True, if so
		 */
		inline bool inArea9(const PixelPositionT<T>& position) const;

		/**
		 * Returns whether this pixel position is the direct neighbor to a second pixel position in an 4-neighborhood.
		 * @param position Second pixel position to check
		 * @return True, if so
		 */
		inline bool isNeighbor4(const PixelPositionT<T>& position) const;

		/**
		 * Returns whether this pixel position is the direct neighbor to a second pixel position in an 8-neighborhood.
		 * @param position Second pixel position to check
		 * @return True, if so
		 */
		inline bool isNeighbor8(const PixelPositionT<T>& position) const;

		/**
		 * Returns the index of this position inside a frame with given width.
		 * The index is determined according to a frame stored in row aligned order.<br>
		 * The result is determined by y() * width + x().<br>
		 * @param width The width to be used for index determination
		 * @return Resulting index
		 */
		inline T index(const unsigned int width) const;

		/**
		 * Copy assignment operator.
		 * @param position The position to copy
		 */
		inline PixelPositionT<T>& operator=(const PixelPositionT<T>& position);

		/**
		 * Adds two pixel positions and returns the result as a new pixel position object.
		 * @param position Second pixel position to add
		 * @return Resulting pixel position sum
		 */
		inline PixelPositionT<T> operator+(const PixelPositionT<T>& position) const;

		/**
		 * Add a second pixel position to this position object.
		 * @param position Second pixel position to add
		 * @return Reference to this changed position object
		 */
		inline PixelPositionT<T>& operator+=(const PixelPositionT<T>& position);

		/**
		 * Subtracts two pixel positions and returns the result as a new pixel position object.
		 * @param position Second pixel position to subtract
		 * @return Resulting pixel position
		 */
		inline PixelPositionT<T> operator-(const PixelPositionT<T>& position) const;

		/**
		 * Subtracts a second pixel position from this position object.
		 * @param position Second pixel position to subtract
		 * @return Reference to this changed position object
		 */
		inline PixelPositionT<T>& operator-=(const PixelPositionT<T>& position);

		/**
		 * Multiplies this pixel position by a scalar and returns the new resulting position.
		 * @param factor The multiplication factor, with range (-infinity, infinity)
		 * @return The resulting new position
		 */
		inline PixelPositionT<T> operator*(const T factor) const;

		/**
		 * Multiplies this pixel position by a scalar.
		 * @param factor The multiplication factor, with range (-infinity, infinity)
		 * @return The reference to this modified position
		 */
		inline PixelPositionT<T>& operator*=(const T factor);

		/**
		 * Divides this pixel position by a scalar and returns the new resulting position.
		 * @param factor The division factor, with range (-infinity, infinity) \ {0}
		 * @return The resulting new position
		 */
		inline PixelPositionT<T> operator/(const T factor) const;

		/**
		 * Divides this pixel position by a scalar.
		 * @param factor The division factor, with range (-infinity, infinity) \ {0}
		 * @return The reference to this modified position
		 */
		inline PixelPositionT<T>& operator/=(const T factor);

		/**
		 * Compares two pixel position objects.
		 * @param position Second pixel position object to be compared
		 * @return True, if this one is lesser than the right one
		 */
		inline bool operator<(const PixelPositionT<T>& position) const;

		/**
		 * Returns whether two pixel position objects are equal.
		 * @param position Second pixel position object to be compared
		 * @return True, if so
		 */
		inline bool operator==(const PixelPositionT<T>& position) const;

		/**
		 * Returns whether two pixel position objects are not equal.
		 * @param position Second pixel position object to be compared
		 * @return True, if so
		 */
		inline bool operator!=(const PixelPositionT<T>& position) const;

		/**
		 * Returns whether this pixel position object holds two valid parameters.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Hash function.
		 * @param pixelPosition The pixel position for which the hash value will be determined
		 * @return The resulting hash value
		 */
		inline size_t operator()(const PixelPositionT<T>& pixelPosition) const;

		/**
		 * Returns the pixel direction of two successive pixels in a dense contour.
		 * @param pixel0 First pixel
		 * @param pixel1 Following pixel
		 * @return Resulting pixel direction
		 */
		static inline PixelDirection direction(const PixelPositionT<T>& pixel0, const PixelPositionT<T>& pixel1);

		/**
		 * Returns the rough pixel direction of two successive pixels in a dense contour.
		 * @param pixel0 First pixel
		 * @param pixel1 Following pixel
		 * @return Resulting rough pixel direction
		 */
		static inline RoughPixelDirection roughDirection(const PixelPositionT<T>& pixel0, const PixelPositionT<T>& pixel1);

		/**
		 * Converts a pixel position into a 2D vector.
		 * @param pixelPosition Pixel position to be converted
		 * @return Resulting 2D vector
		 */
		static inline Vector2 pixelPosition2vector(const PixelPositionT<T>& pixelPosition);

		/**
		 * Converts several pixel positions to 2D vectors.
		 * @param pixelPositions Pixel positions to be converted
		 * @return Resulting 2D vectors
		 */
		static inline Vectors2 pixelPositions2vectors(const std::vector<PixelPositionT<T>>& pixelPositions);

		/**
		 * Converts a 2D vector into a pixel position.
		 * The pixel positions are rounded.
		 * @param value The value to be converted, with range [0, infinity)x[0, infinity)
		 * @return Resulting pixel position
		 */
		static inline PixelPositionT<T> vector2pixelPosition(const Vector2& value);

		/**
		 * Converts several 2D vectors into pixel positions.
		 * The pixel positions are rounded.
		 * @param values The values to be converted, with range [0, infinity)x[0, infinity)
		 * @return Resulting pixel positions
		 */
		static inline std::vector<PixelPositionT<T>> vectors2pixelPositions(const Vectors2& values);

		/**
		 * Converts several 2D vectors into pixel positions.
		 * The pixel positions are rounded and clipped to the given frame dimension.
		 * @param values The values to be converted, with range [0, infinity)x[0, infinity)
		 * @param width The width of the clipping area, in pixel
		 * @param height The height of the clipping area, in pixel
		 * @return Resulting pixel positions
		 */
		static inline std::vector<PixelPositionT<T>> vectors2pixelPositions(const Vectors2& values, const unsigned int width, const unsigned int height);

		/**
		 * Converts pixels positions with a data type T to pixel positions with another data type.
		 * Beware: This function does not handle out-of-range issues. Thus, ensure that the target data types can covers the locations of the source positions.
		 * @param pixelPositions The pixel positions to convert
		 * @return The resulting converted pixel positions
		 * @tparam TTarget Data type of the target pixel positions
		 */
		template <typename TTarget>
		static inline std::vector<PixelPositionT<TTarget>> pixelPositions2pixelPositions(const std::vector<PixelPositionT<T>>& pixelPositions);

	protected:

		/// Horizontal coordinate value of this object, in pixel.
		T x_;

		/// Vertical coordinate value of this object, in pixel.
		T y_;
};

template <>
inline PixelPositionT<int>::PixelPositionT() :
	x_(NumericT<int>::minValue()),
	y_(NumericT<int>::minValue())
{
	// nothing to do here
}

template <typename T>
inline PixelPositionT<T>::PixelPositionT() :
	x_(T(-1)),
	y_(T(-1))
{
	// nothing to do here
}

template <typename T>
inline PixelPositionT<T>::PixelPositionT(const PixelPositionT<T>& position) :
	x_(position.x_),
	y_(position.y_)
{
	// nothing to do here
}

template <typename T>
inline PixelPositionT<T>::PixelPositionT(const T& x, const T& y) :
	x_(x),
	y_(y)
{
	// nothing to do here
}

template <typename T>
inline T PixelPositionT<T>::x() const
{
	return x_;
}

template <typename T>
inline T& PixelPositionT<T>::x()
{
	return x_;
}

template <typename T>
inline T PixelPositionT<T>::y() const
{
	return y_;
}

template <typename T>
inline T& PixelPositionT<T>::y()
{
	return y_;
}

template <typename T>
inline void PixelPositionT<T>::setPosition(const T& x, const T& y)
{
	x_ = x;
	y_ = y;
}

template <typename T>
inline unsigned int PixelPositionT<T>::sqrDistance(const PixelPositionT<T>& position) const
{
	static_assert(sizeof(T) <= sizeof(int), "Invalid template type T");

	const int xd = int(x_) - int(position.x_);
	const int yd = int(y_) - int(position.y_);

	return xd * xd + yd * yd;
}

template <typename T>
CV::PixelPositionT<T> PixelPositionT<T>::neighbor(const CV::PixelDirection pixelDirection) const
{
	/*
	* X---------------------
	* |                     |
	* |    NW    N    NE    |
	* |                     |
	* |     W    P    E     |
	* |                     |
	* |    SW    S    SE    |
	* |                     |
	*  ---------------------
	*/

	switch (pixelDirection)
	{
		case PD_NORTH:
			return CV::PixelPositionT<T>(x_, y_ - 1);

		case PD_NORTH_WEST:
			return CV::PixelPositionT<T>(x_ - 1, y_ - 1);

		case PD_WEST:
			return CV::PixelPositionT<T>(x_ - 1, y_);

		case PD_SOUTH_WEST:
			return CV::PixelPositionT<T>(x_ - 1, y_ + 1);

		case PD_SOUTH:
			return CV::PixelPositionT<T>(x_, y_ + 1);

		case PD_SOUTH_EAST:
			return CV::PixelPositionT<T>(x_ + 1, y_ + 1);

		case PD_EAST:
			return CV::PixelPositionT<T>(x_ + 1, y_);

		case PD_NORTH_EAST:
			return CV::PixelPositionT<T>(x_ + 1, y_ - 1);

		case PD_INVALID:
			break;
	}

	ocean_assert(false && "Invalid pixel direction!");
	return *this;
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::north() const
{
	return PixelPositionT<T>(x_, y_ - T(1));
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::northWest() const
{
	return PixelPositionT<T>(x_ - T(1), y_ - T(1));
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::west() const
{
	return PixelPositionT<T>(x_ - T(1), y_);
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::southWest() const
{
	return PixelPositionT<T>(x_ - T(1), y_ + T(1));
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::south() const
{
	return PixelPositionT<T>(x_, y_ + T(1));
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::southEast() const
{
	return PixelPositionT<T>(x_ + T(1), y_ + T(1));
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::east() const
{
	return PixelPositionT<T>(x_ + T(1), y_);
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::northEast() const
{
	return PixelPositionT<T>(x_ + T(1), y_ - T(1));
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::half() const
{
	return PixelPositionT<T>(x_ / T(2), y_ / T(2));
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::twice() const
{
	return PixelPositionT<T>(x_ << 1u, y_ << 1u);
}

template <typename T>
inline Vector2 PixelPositionT<T>::vector() const
{
	return Vector2(Scalar(x_), Scalar(y_));
}

template <>
inline bool PixelPositionT<uint32_t>::isValid() const
{
	return x_ != (uint32_t)(-1) && y_ != (uint32_t)(-1);
}

template <>
inline bool PixelPositionT<uint64_t>::isValid() const
{
	return x_ != (uint64_t)(-1) && y_ != (uint64_t)(-1);
}

template <>
inline bool PixelPositionT<int32_t>::isValid() const
{
	return x_ != NumericT<int32_t>::minValue() && y_ != NumericT<int32_t>::minValue();
}

template <>
inline bool PixelPositionT<int64_t>::isValid() const
{
	return x_ != NumericT<int64_t>::minValue() && y_ != NumericT<int64_t>::minValue();
}

template <typename T>
inline bool PixelPositionT<T>::inArea9(const PixelPositionT<T>& position) const
{
	ocean_assert(isValid());

	const T differenceX = x_ - position.x_;
	const T differenceY = y_ - position.y_;

	return (differenceX == T(1) || differenceX == T(0) || differenceX == T(-1))
				&& (differenceY == T(1) || differenceY == T(0) || differenceY == T(-1));
}

template <typename T>
inline bool PixelPositionT<T>::isNeighbor4(const PixelPositionT<T>& position) const
{
	ocean_assert(isValid());

	const T differenceX = x_ - position.x_;
	const T differenceY = y_ - position.y_;

	return (differenceX == T(0) && (differenceY == T(1) || differenceY == T(-1)))
				|| (differenceY == T(0) && (differenceX == T(1) || differenceX == T(-1)));
}

template <typename T>
inline bool PixelPositionT<T>::isNeighbor8(const PixelPositionT<T>& position) const
{
	ocean_assert(isValid());

	const T differenceX = x_ - position.x_;
	const T differenceY = y_ - position.y_;

	return (differenceX != T(0) || differenceY != T(0))
				&& (differenceX == T(1) || differenceX == T(0) || differenceX == T(-1))
				&& (differenceY == T(1) || differenceY == T(0) || differenceY == T(-1));
}

template <typename T>
inline T PixelPositionT<T>::index(const unsigned int width) const
{
	ocean_assert(isValid());
	return y_ * T(width) + x_;
}

template <typename T>
inline PixelPositionT<T>& PixelPositionT<T>::operator=(const PixelPositionT<T>& position)
{
	if (this == &position)
	{
		return *this;
	}

	x_ = position.x_;
	y_ = position.y_;

	return *this;
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::operator+(const PixelPositionT<T>& position) const
{
	ocean_assert(isValid() && position.isValid());

	return PixelPositionT<T>(x_ + position.x_, y_ + position.y_);
}

template <typename T>
inline PixelPositionT<T>& PixelPositionT<T>::operator+=(const PixelPositionT<T>& position)
{
	ocean_assert(isValid() && position.isValid());

	x_ += position.x_;
	y_ += position.y_;
	return *this;
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::operator-(const PixelPositionT<T>& position) const
{
	ocean_assert(isValid() && position.isValid());

	return PixelPositionT<T>(x_ - position.x_, y_ - position.y_);
}

template <typename T>
inline PixelPositionT<T>& PixelPositionT<T>::operator-=(const PixelPositionT<T>& position)
{
	ocean_assert(isValid() && position.isValid());

	x_ -= position.x_;
	y_ -= position.y_;
	return *this;
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::operator*(const T factor) const
{
	ocean_assert(isValid());

	return PixelPositionT<T>(x_ * factor, y_ * factor);
}

template <typename T>
inline PixelPositionT<T>& PixelPositionT<T>::operator*=(const T factor)
{
	ocean_assert(isValid());

	x_ *= factor;
	y_ *= factor;

	return *this;
}

template <typename T>
inline PixelPositionT<T> PixelPositionT<T>::operator/(const T factor) const
{
	ocean_assert(isValid());
	ocean_assert(factor != T(0));

	return PixelPositionT<T>(x_ / factor, y_ / factor);
}

template <typename T>
inline PixelPositionT<T>& PixelPositionT<T>::operator/=(const T factor)
{
	ocean_assert(isValid());
	ocean_assert(factor != T(0));

	x_ /= factor;
	y_ /= factor;

	return *this;
}

template <typename T>
inline bool PixelPositionT<T>::operator<(const PixelPositionT<T>& position) const
{
	return (y_ < position.y_) || (y_ == position.y_ && x_ < position.x_);
}

template <typename T>
inline bool PixelPositionT<T>::operator==(const PixelPositionT<T>& position) const
{
	return x_ == position.x_ && y_ == position.y_;
}

template <typename T>
inline bool PixelPositionT<T>::operator!=(const PixelPositionT<T>& position) const
{
	return !(*this == position);
}

template <typename T>
inline PixelPositionT<T>::operator bool() const
{
	return isValid();
}

template <typename T>
inline size_t PixelPositionT<T>::operator()(const PixelPositionT<T>& pixelPosition) const
{
	size_t seed = std::hash<T>{}(pixelPosition.x());
	seed ^= std::hash<T>{}(pixelPosition.y()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

template <typename T>
inline PixelDirection PixelPositionT<T>::direction(const PixelPositionT<T>& pixel0, const PixelPositionT<T>& pixel1)
{
	ocean_assert(pixel0.isValid() && pixel1.isValid());
	ocean_assert(pixel0.isNeighbor8(pixel1));

	const unsigned int parameter = (0x0000FFFFu & (pixel1.x() - pixel0.x())) | ((pixel1.y() - pixel0.y()) << 16u);

	// the low 16 bit may have value 0x0000 (same), 0x0001 (east) or 0xFFFF (west)
	// the high 16 bit may have value 0x0000 (same), 0x0001 (south) or 0xFFFF (north)

	switch (parameter)
	{
		// north
		case 0xFFFF0000u:
			ocean_assert(pixel0.north() == pixel1);
			return PD_NORTH;

		// north west
		case 0xFFFFFFFFu:
			ocean_assert(pixel0.northWest() == pixel1);
			return PD_NORTH_WEST;

		// west
		case 0x0000FFFFu:
			ocean_assert(pixel0.west() == pixel1);
			return PD_WEST;

		// south west
		case 0x0001FFFFu:
			ocean_assert(pixel0.southWest() == pixel1);
			return PD_SOUTH_WEST;

		// south
		case 0x00010000u:
			ocean_assert(pixel0.south() == pixel1);
			return PD_SOUTH;

		// south east
		case 0x00010001u:
			ocean_assert(pixel0.southEast() == pixel1);
			return PD_SOUTH_EAST;

		// east
		case 0x00000001u:
			ocean_assert(pixel0.east() == pixel1);
			return PD_EAST;

		// north east
		case 0xFFFF0001u:
			ocean_assert(pixel0.northEast() == pixel1);
			return PD_NORTH_EAST;
	}

	ocean_assert(false && "Invalid direction");
	return PD_INVALID;
}

template <typename T>
inline typename PixelPositionT<T>::RoughPixelDirection PixelPositionT<T>::roughDirection(const PixelPositionT<T>& pixel0, const PixelPositionT<T>& pixel1)
{
	ocean_assert(pixel0.isValid() && pixel1.isValid());
	ocean_assert(pixel0.isNeighbor8(pixel1));

	const unsigned int parameter = (0x0000FFFFu & (pixel1.x() - pixel0.x())) | ((pixel1.y() - pixel0.y()) << 16u);

	// the low 16 bit may have value 0x0000 (same), 0x0001 (east) or 0xFFFF (west)
	// the high 16 bit may have value 0x0000 (same), 0x0001 (south) or 0xFFFF (north)

	switch (parameter)
	{
		// north
		case 0xFFFF0000u:
		// south
		case 0x00010000u:
			ocean_assert(pixel0.north() == pixel1 || pixel0.south() == pixel1);
			return RPD_VERTICAL;

		// west
		case 0x0000FFFFu:
		// east
		case 0x00000001u:
			ocean_assert(pixel0.west() == pixel1 || pixel0.east() == pixel1);
			return RPD_HORIZONTAL;

		// north west
		case 0xFFFFFFFFu:
		// north east
		case 0xFFFF0001u:
		// south west
		case 0x0001FFFFu:
		// south east
		case 0x00010001u:
			ocean_assert(pixel0.northWest() == pixel1 || pixel0.northEast() == pixel1 || pixel0.southWest() == pixel1 || pixel0.southEast() == pixel1);
			return RPD_DIAGONAL;
	}

	ocean_assert(false && "Invalid direction");
	return RPD_INVALID;
}

template <typename T>
inline Vector2 PixelPositionT<T>::pixelPosition2vector(const PixelPositionT<T>& pixelPosition)
{
	ocean_assert(pixelPosition.isValid());
	return Vector2(Scalar(pixelPosition.x()), Scalar(pixelPosition.y()));
}

template <typename T>
inline Vectors2 PixelPositionT<T>::pixelPositions2vectors(const std::vector<PixelPositionT<T>>& pixelPositions)
{
	Vectors2 result;
	result.reserve(pixelPositions.size());

	for (typename std::vector<PixelPositionT<T>>::const_iterator i = pixelPositions.begin(); i != pixelPositions.end(); ++i)
	{
		ocean_assert(i->isValid());
		result.push_back(Vector2(Scalar(i->x()), Scalar(i->y())));
	}

	return result;
}

template <>
inline PixelPositionT<unsigned int> PixelPositionT<unsigned int>::vector2pixelPosition(const Vector2& value)
{
	ocean_assert(value.x() >= Scalar(0) && value.x() < Scalar(NumericT<unsigned int>::maxValue()));
	ocean_assert(value.y() >= Scalar(0) && value.y() < Scalar(NumericT<unsigned int>::maxValue()));

	return PixelPositionT<unsigned int>((unsigned int)(value.x() + Scalar(0.5)), (unsigned int)(value.y() + Scalar(0.5)));
}

template <>
inline PixelPositionT<int> PixelPositionT<int>::vector2pixelPosition(const Vector2& value)
{
	ocean_assert(value.x() > Scalar(NumericT<int>::minValue()) && value.x() <= Scalar(NumericT<int>::maxValue()));
	ocean_assert(value.y() > Scalar(NumericT<int>::minValue()) && value.y() <= Scalar(NumericT<int>::maxValue()));

	return PixelPositionT<int>(Numeric::round32(value.x()), Numeric::round32(value.y()));
}

template <typename T>
inline std::vector<PixelPositionT<T>> PixelPositionT<T>::vectors2pixelPositions(const Vectors2& values)
{
	std::vector<PixelPositionT<T>> result;
	result.reserve(values.size());

	for (Vectors2::const_iterator i = values.begin(); i != values.end(); ++i)
	{
		result.push_back(vector2pixelPosition(*i));
	}

	return result;
}

template <typename T>
inline std::vector<PixelPositionT<T>> PixelPositionT<T>::vectors2pixelPositions(const Vectors2& values, const unsigned int width, const unsigned int height)
{
	static_assert(sizeof(T) <= sizeof(int), "Invalid template type T");

	std::vector<PixelPositionT<T>> result;
	result.reserve(values.size());

	for (Vectors2::const_iterator i = values.begin(); i != values.end(); ++i)
	{
		result.emplace_back(T(minmax(0, int(i->x() + Scalar(0.5)), int(width - 1))), T(minmax(0, int(i->y() + Scalar(0.5)), int(height - 1u))));
	}

	return result;
}

template <typename T>
template <typename TTarget>
inline std::vector<PixelPositionT<TTarget>> PixelPositionT<T>::pixelPositions2pixelPositions(const std::vector<PixelPositionT<T>>& pixelPositions)
{
	std::vector<PixelPositionT<TTarget>> result;
	result.reserve(pixelPositions.size());

	for (typename std::vector<PixelPositionT<T>>::const_iterator i = pixelPositions.begin(); i != pixelPositions.end(); ++i)
	{
		result.emplace_back(TTarget(i->x()), TTarget(i->y()));
	}

	return result;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const PixelPositionT<T>& pixelPosition)
{
	stream << "[" << pixelPosition.x() << ", " << pixelPosition.y() << "]";

	return stream;
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const PixelPositionT<T>& pixelPosition)
{
	return messageObject << "[" << pixelPosition.x() << ", " << pixelPosition.y() << "]";
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const PixelPositionT<T>& pixelPosition)
{
	return messageObject << "[" << pixelPosition.x() << ", " << pixelPosition.y() << "]";
}

}

}

#endif // META_OCEAN_CV_PIXEL_POSITION_H
