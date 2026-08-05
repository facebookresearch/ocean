/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_PIXEL_CONTOUR_H
#define META_OCEAN_CV_SEGMENTATION_PIXEL_CONTOUR_H

#include "ocean/cv/segmentation/Segmentation.h"

#include "ocean/cv/Bresenham.h"
#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

// Forward declaration.
template <typename T> class PixelContourT;

/**
 * Definition of the default PixelContour object with a data type allowing only positive coordinate values.
 * @see PixelCountourT
 * @ingroup cvsegmentation
 */
using PixelContour = PixelContourT<unsigned int>;

/**
 * Definition of a PixelContour object with a data type allowing positive and negative coordinate values.
 * @see PixelCountourT
 * @ingroup cvsegmentation
 */
using PixelContourI = PixelContourT<int>;

/**
 * Definition of a vector holding pixel contours (with positive coordinate values).
 * @see PixelContour
 * @ingroup cvsegmentation
 */
using PixelContours = std::vector<PixelContour>;

/**
 * Definition of a vector holding pixel contours (with positive and negative coordinate values).
 * @see PixelContourI
 * @ingroup cvsegmentation
 */
using PixelContoursI = std::vector<PixelContourI>;

/**
 * This class implements a contour with pixel accuracy.
 * A valid contour is composed of several consecutive contour locations.<br>
 * A contour is dense if all consecutive contour pixel locations are connected via a 8-neighborhood.<br>
 * Otherwise, a contour is sparse.
 * @tparam T The data type of the elements of the PixelPositions
 * @see PixelContour, PixelContourI
 * @ingroup cvsegmentation
 */
template <typename T>
class PixelContourT
{
	public:

		/**
		 * Definition of a pixel position.
		 */
		using PixelPosition = PixelPositionT<T>;

		/**
		 * Definition of a vector holding pixel positions.
		 */
		using PixelPositions = std::vector<PixelPosition>;

		/**
		 * Definition of a pixel bounding box.
		 */
		using PixelBoundingBox = PixelBoundingBoxT<T>;

	public:

		/**
		 * Creates a new pixel contour object.
		 */
		PixelContourT() = default;

		/**
		 * Copy constructor.
		 * @param contour Contour to be copied
		 */
		inline PixelContourT(const PixelContourT<T>& contour);

		/**
		 * Move constructor.
		 * @param contour Contour to be moved
		 */
		inline PixelContourT(PixelContourT<T>&& contour) noexcept;

		/**
		 * Creates a new pixel contour object by a given set of pixel positions that represent the pixel locations of the contour in a ring order.
		 * @param pixelPositions Pixel positions defining the new contour
		 * @param pixelBoundingBox Optional bounding box of the provided pixel position, must be correct if provided
		 */
		inline explicit PixelContourT(const PixelPositions& pixelPositions, const PixelBoundingBox& pixelBoundingBox = PixelBoundingBox());

		/**
		 * Creates a new pixel contour object by moving set of pixel positions that represent the pixel locations of the contour in a ring order.
		 * @param pixelPositions Pixel positions defining the new contour that will be moved
		 * @param pixelBoundingBox Optional bounding box of the provided pixel position, must be correct if provided
		 */
		inline explicit PixelContourT(PixelPositions&& pixelPositions, const PixelBoundingBox& pixelBoundingBox = PixelBoundingBox());

		/**
		 * Creates a new pixel contour object by a given set of pixel positions that represent the pixel locations of the contour in a ring order.
		 * @param pixelPositions Pixel positions defining the new contour
		 * @param indexMostLeftPosition Index of the left most pixel position
		 * @param isCounterClockwise True, if the given pixel positions define a contour in counter clockwise order
		 * @param pixelBoundingBox Optional bounding box of the provided pixel position, must be correct if provided
		 */
		inline PixelContourT(const PixelPositions& pixelPositions, const size_t indexMostLeftPosition, const bool isCounterClockwise, const PixelBoundingBox& pixelBoundingBox = PixelBoundingBox());

		/**
		 * Creates a new pixel contour object by moving a set of pixel positions that represent the pixel locations of the contour in a ring order.
		 * @param pixelPositions Pixel positions defining the new contour that will be moved
		 * @param indexMostLeftPosition Index of the left most pixel position
		 * @param isCounterClockwise True, if the given pixel positions define a contour in counter clockwise order
		 * @param pixelBoundingBox Optional bounding box of the provided pixel position, must be correct if provided
		 */
		inline PixelContourT(PixelPositions&& pixelPositions, const size_t indexMostLeftPosition, const bool isCounterClockwise, const PixelBoundingBox& pixelBoundingBox = PixelBoundingBox());

		/**
		 * Creates a new pixel contour object by a given set of pixel positions that represent the pixel locations of the contour in a ring order and ensures that the contour will have specific properties afterwards.
		 * @param createDistinct True, to create a distinct pixel contour; False, to ignore this property
		 * @param createSimplified True, to create a simplified pixel contour; False, to ignore this property; a simplified contour is always distinct
		 * @param pixelPositions Pixel positions defining the new contour
		 * @param pixelBoundingBox Optional bounding box of the provided pixel position, must be correct if provided
		 */
		inline PixelContourT(const bool createDistinct, const bool createSimplified, const PixelPositions& pixelPositions, const PixelBoundingBox& pixelBoundingBox = PixelBoundingBox());

		/**
		 * Creates a new sparse pixel contour object by a given set of pixel positions that represent the pixel locations of the contour in a ring order.
		 * In addition, the new contour will be a sparse contour by ensuring that the minimal distance between consecutive contour pixels has at least a specific distance.
		 * @param pixelPositions Pixel positions out of which the new contour will be created (by skipping pixels too close to eachother), at least one
		 * @param minimalSqrDistance The minimal square distance between consecutive locations of the new contour, with range [1, infinity)
		 * @param startIndex The index of the pixel which will be preserved in any case, with range [0, size())
		 */
		PixelContourT(const PixelPositions& pixelPositions, const unsigned int minimalSqrDistance, const size_t startIndex = 0);

		/**
		 * Returns the pixels of this contour.
		 * @return Pixel positions
		 */
		inline const PixelPositions& pixels() const;

		/**
		 * Returns the bounding box of this contour.
		 * @return Bounding box
		 */
		const PixelBoundingBox& boundingBox() const;

		/**
		 * Computes the area of a contour
		 * Uses the Shoelace formula to determine the area of a contour. The contour must not self-intersect.
		 * @return The area of the contour, range: [0, infinity)
		 * @sa Geometry::Utilities::computePolygonArea()
		 */
		inline unsigned int area() const;

		/**
		 * Computes the signed area of a contour
		 * Uses the Shoelace formula to determine the area of a contour. The contour must not self-intersect.
		 * @return The signed  area of the contour; this value will be positive if the contour pixels are in counter-clockwise order and negative if they are in clock-wise order, range: (-infinity, infinity)
		 * @sa Geometry::Utilities::computePolygonAreaSigned()
		 */
		int areaSigned() const;

		/**
		 * Returns the index of a left most position of this contour with following pixel right to this position.
		 * @return Index of the left most position, -1 if invalid
		 */
		size_t indexLeftPosition() const;

		/**
		 * Returns whether this contour is defined in a counter clockwise order, clockwise otherwise.<br>
		 * A contour with contour clockwise order has a negative 2D edge cross product at the most left position.<br>
		 * If this contour is degenerated the result is arbitrary.
		 * @return True, if so
		 */
		bool isCounterClockwise() const;

		/**
		 * Returns whether all consecutive pixels of this contour are different.
		 * @return True, if so
		 * @see makeDistinct().
		 */
		bool isDistinct() const;

		/**
		 * Returns whether this contour is dense.
		 * Successive pixel positions in a dense contour are part of a direct 8-neighborhood.<br>
		 * An empty set of pixel positions is dense.
		 * @return True, if so
		 */
		bool isDense() const;

		/**
		 * Returns whether this contour is dense according to a 4-neighborhood.
		 * Successive pixel positions in a dense contour are part of a direct 4-neighborhood.<br>
		 * An empty set of pixel positions is dense.
		 * @return True, if so
		 */
		bool isDense4() const;

		/**
		 * Returns whether this contour is simplified.
		 * A simplified contour is the sparsest contour possible. e.g., the contour does not have successive pixels building a line.<br>
		 * An empty contour is always simplified.
		 * @return True, if so
		 * @see simplify().
		 */
		bool isSimplified() const;

		/**
		 * Removes non distinct pixels from this contour.
		 * The resulting contour will not have identical consecutive pixels.
		 * @see isDistinct().
		 */
		void makeDistinct();

		/**
		 * Makes this pixel contour dense.<br>
		 */
		void makeDense();

		/**
		 * Returns the simplified contour of this contour which will be a sparse but identical contour.
		 * @return The simplified contour
		 * @see simplify(), isSimplified().
		 */
		PixelContourT<T> simplified() const;

		/**
		 * Simplifies this (dense) contour to a sparse but identical contour.
		 * @see simplified(), isSimplified().
		 */
		void simplify();

		/**
		 * Creates a sparse contour out of this contour by ensuring that the minimal distance between consecutive contour pixels has at least a specific distance.
		 * @param minimalSqrDistance The minimal square distance between consecutive locations of the resulting sparse contour, with range [1, infinity)
		 * @param startIndex The index of the contour pixel which will be preserved in any case, with range [0, size())
		 * @return The sparse contour
		 */
		PixelContourT<T> sparseContour(const unsigned int minimalSqrDistance, const size_t startIndex = 0) const;

		/**
		 * Returns the smallest square distance between consecutive contour pixels.
		 * This contour must not be empty.
		 * @return The smallest square distance, with range [0, infinity)
		 */
		unsigned int smallestSqrDistanceBetweenPixels() const;

		/**
		 * Returns the largest square distance between consecutive contour pixels.
		 * This contour must not be empty.
		 * @return The largest square distance, with range [0, infinity)
		 */
		unsigned int largestSqrDistanceBetweenPixels() const;

		/**
		 * Returns the number of pixel positions of this contour.
		 * @return Number of pixel positions
		 */
		inline size_t size() const;

		/**
		 * Returns whether this contour does not hold any pixel position.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns the pixel position of this pixel contour.
		 * @param index Index of the pixel position that is requested, with range [0, size())
		 * @return Requested pixel position
		 */
		inline const PixelPosition& operator[](const size_t index) const;

		/**
		 * Assign operator.
		 * @param contour Contour object to be assigned
		 * @return Reference to this object
		 */
		inline PixelContourT<T>& operator=(const PixelContourT<T>& contour);

		/**
		 * Move operator.
		 * @param contour Contour object to be moved
		 * @return Reference to this object
		 */
		inline PixelContourT<T>& operator=(PixelContourT<T>&& contour) noexcept;

		/**
		 * Returns whether this contour holds at least one pixel position.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/**
		 * Returns whether two given vectors are parallel and point into the same direction.
		 * @param first First vector
		 * @param second Second vector
		 * @return True, if so
		 */
		static inline bool similar(const VectorI2& first, const VectorI2& second);

	protected:

		/// Pixel positions of the contour.
		PixelPositions pixels_;

		/// Index of the most left pixel.
		mutable size_t mostLeftIndex_ = size_t(-1);

		/// State whether this contour is counter clockwise: -1 undefined, 0 false, 1 true.
		mutable unsigned int counterClockwise_ = (unsigned int)(-1);

		/// Bounding box of the contour.
		mutable PixelBoundingBox boundingBox_;
};

template <typename T>
inline PixelContourT<T>::PixelContourT(const PixelContourT<T>& contour) :
	pixels_(contour.pixels_),
	mostLeftIndex_(contour.mostLeftIndex_),
	counterClockwise_(contour.counterClockwise_),
	boundingBox_(contour.boundingBox_)
{
	// nothing to do here
}

template <typename T>
inline PixelContourT<T>::PixelContourT(PixelContourT<T>&& contour) noexcept :
	pixels_(std::move(contour.pixels_)),
	mostLeftIndex_(contour.mostLeftIndex_),
	counterClockwise_(contour.counterClockwise_),
	boundingBox_(contour.boundingBox_)
{
	contour.mostLeftIndex_ = (size_t)(-1);
	contour.counterClockwise_ = (unsigned int)(-1);
	contour.boundingBox_ = PixelBoundingBox();
}

template <typename T>
inline PixelContourT<T>::PixelContourT(const PixelPositions& pixelPositions, const PixelBoundingBox& pixelBoundingBox) :
	pixels_(pixelPositions),
	mostLeftIndex_((size_t)(-1)),
	counterClockwise_((unsigned int)(-1)),
	boundingBox_(pixelBoundingBox)
{
	ocean_assert(!boundingBox_ || boundingBox_ == PixelBoundingBoxT<T>(pixels_));
}

template <typename T>
inline PixelContourT<T>::PixelContourT(PixelPositions&& pixelPositions, const PixelBoundingBox& pixelBoundingBox) :
	pixels_(std::move(pixelPositions)),
	mostLeftIndex_((size_t)(-1)),
	counterClockwise_((unsigned int)(-1)),
	boundingBox_(pixelBoundingBox)
{
	ocean_assert(!boundingBox_ || boundingBox_ == PixelBoundingBoxT<T>(pixels_));
}

template <typename T>
inline PixelContourT<T>::PixelContourT(const PixelPositions& pixelPositions, const size_t indexMostLeftPosition, const bool isCounterClockwise, const PixelBoundingBox& pixelBoundingBox) :
	pixels_(pixelPositions),
	mostLeftIndex_(indexMostLeftPosition),
	counterClockwise_(isCounterClockwise),
	boundingBox_(pixelBoundingBox)
{
	ocean_assert(!boundingBox_ || boundingBox_ == PixelBoundingBoxT<T>(pixels_));
}

template <typename T>
inline PixelContourT<T>::PixelContourT(PixelPositions&& pixelPositions, const size_t indexMostLeftPosition, const bool isCounterClockwise, const PixelBoundingBox& pixelBoundingBox) :
	pixels_(std::move(pixelPositions)),
	mostLeftIndex_(indexMostLeftPosition),
	counterClockwise_(isCounterClockwise),
	boundingBox_(pixelBoundingBox)
{
	ocean_assert(!boundingBox_ || boundingBox_ == PixelBoundingBoxT<T>(pixels_));
}

template <typename T>
inline PixelContourT<T>::PixelContourT(const bool createDistinct, const bool createSimplified, const PixelPositions& pixelPositions, const PixelBoundingBox& pixelBoundingBox) :
	pixels_(pixelPositions),
	mostLeftIndex_((size_t)(-1)),
	counterClockwise_((unsigned int)(-1)),
	boundingBox_(pixelBoundingBox)
{
	if (createSimplified)
	{
		simplify();
	}
	else if (createDistinct)
	{
		makeDistinct();
	}

	ocean_assert(!boundingBox_ || boundingBox_ == PixelBoundingBoxT<T>(pixels_));
}

template <typename T>
PixelContourT<T>::PixelContourT(const PixelPositions& pixelPositions, const unsigned int minimalSqrDistance, const size_t startIndex) :
	mostLeftIndex_((size_t)(-1)),
	counterClockwise_((unsigned int)(-1))
{
	ocean_assert(pixelPositions.size() >= 1);
	ocean_assert(minimalSqrDistance >= 1u);
	ocean_assert(startIndex < pixelPositions.size());

	ocean_assert(pixels_.empty());
	pixels_.reserve(pixelPositions.size());

	// our first sparse
	pixels_.push_back(pixelPositions[startIndex]);

	for (size_t n = startIndex + 1; n <= startIndex + pixelPositions.size(); ++n)
	{
		const size_t nModulo = (size_t)modulo((int)n, (int)pixelPositions.size());

		if (pixels_.back().sqrDistance(pixelPositions[nModulo]) >= minimalSqrDistance)
		{
			pixels_.push_back(pixelPositions[nModulo]);
		}
	}

	if (pixels_.size() > 1 && pixels_.front().sqrDistance(pixels_.back()) < minimalSqrDistance)
	{
		pixels_.pop_back();
	}

	ocean_assert(pixels_.size() == 1 || PixelContourT<T>(pixels_).smallestSqrDistanceBetweenPixels() >= minimalSqrDistance);
}

template <typename T>
inline const typename PixelContourT<T>::PixelPositions& PixelContourT<T>::pixels() const
{
	return pixels_;
}

template <typename T>
inline size_t PixelContourT<T>::size() const
{
	return pixels_.size();
}

template <typename T>
inline bool PixelContourT<T>::isEmpty() const
{
	return pixels_.empty();
}

template <typename T>
inline const typename PixelContourT<T>::PixelPosition& PixelContourT<T>::operator[](const size_t index) const
{
	ocean_assert(index < pixels_.size());
	return pixels_[index];
}

template <typename T>
inline PixelContourT<T>& PixelContourT<T>::operator=(const PixelContourT<T>& contour)
{
	pixels_ = contour.pixels_;
	mostLeftIndex_ = contour.mostLeftIndex_;
	counterClockwise_ = contour.counterClockwise_;
	boundingBox_ = contour.boundingBox_;

	return *this;
}

template <typename T>
inline PixelContourT<T>& PixelContourT<T>::operator=(PixelContourT<T>&& contour) noexcept
{
	if (this != &contour)
	{
		pixels_ = std::move(contour.pixels_);
		mostLeftIndex_ = contour.mostLeftIndex_;
		counterClockwise_ = contour.counterClockwise_;
		boundingBox_ = contour.boundingBox_;

		contour.mostLeftIndex_ = (size_t)(-1);
		contour.counterClockwise_ = (unsigned int)(-1);
		contour.boundingBox_ = PixelBoundingBox();
	}

	return *this;
}

template <typename T>
const typename PixelContourT<T>::PixelBoundingBox& PixelContourT<T>::boundingBox() const
{
	if (!boundingBox_)
	{
		boundingBox_ = PixelBoundingBox(pixels_);
	}

	return boundingBox_;
}

template <typename T>
inline unsigned int PixelContourT<T>::area() const
{
	return (unsigned int)std::abs(areaSigned());
}

template <typename T>
int PixelContourT<T>::areaSigned() const
{
	if (pixels_.size() < 3)
	{
		return 0;
	}

	int area = 0;

	for (size_t i = 0; i < (pixels_.size() - 1); ++i)
	{
		const int partialArea = pixels_[i].x() * pixels_[i + 1].y() - pixels_[i].y() * pixels_[i + 1].x();
		ocean_assert(partialArea <= 0 || area <= NumericT<int>::maxValue() - partialArea && "Integer overflow");
		ocean_assert(partialArea >= 0 || area >= NumericT<int>::minValue() - partialArea && "Integer underflow");
		area += partialArea;
	}

	return (area + NumericT<int>::copySign(1, area)) / 2;
}

template <typename T>
size_t PixelContourT<T>::indexLeftPosition() const
{
	if (mostLeftIndex_ != (size_t)(-1))
	{
		return mostLeftIndex_;
	}

	if (pixels_.empty())
	{
		return (size_t)(-1);
	}

	if (pixels_.size() == 1)
	{
		return 0;
	}

	// finding the most left pixel with following pixel right to this position

	T left = NumericT<T>::maxValue();
	T bottom = NumericT<T>::minValue();

	size_t index = size_t(-1);

	for (size_t n = 0u; n < pixels_.size(); ++n)
	{
		if (pixels_[n].x() < left || (pixels_[n].x() == left && pixels_[n].y() > bottom))
		{
			left = pixels_[n].x();
			bottom = pixels_[n].y();
			index = n;
		}
	}

	ocean_assert(index != size_t(-1));
	ocean_assert(!boundingBox_ || left == boundingBox_.left());

	mostLeftIndex_ = index;
	return index;
}

template <typename T>
bool PixelContourT<T>::isCounterClockwise() const
{
	if (counterClockwise_ != (unsigned int)(-1))
	{
		return counterClockwise_ == 1u;
	}

	const size_t index0 = size_t(indexLeftPosition());
	ocean_assert(index0 != size_t(-1));

	const size_t index2 = modulo(int(index0) - 1, int(pixels_.size()));

	const PixelPosition& position0 = pixels_[index0];
	const PixelPosition& position2 = pixels_[index2];

	const int dx02 = int(position2.x()) - int(position0.x());
	const int dy02 = int(position2.y()) - int(position0.y());

	size_t index1 = size_t(-1);
	size_t offset = 1;

	while (true)
	{
		index1 = modulo(int(index0 + offset), int(pixels_.size()));

		// the contour is degenerated and thus the result is arbitrary
		if (index1 == index2 || index1 == index0)
		{
			return true;
		}

		const PixelPosition& position1 = pixels_[index1];

		const int dx01 = int(position1.x()) - int(position0.x());
		const int dy01 = int(position1.y()) - int(position0.y());

		// cross (dx01, dy01) x (dx02, dy02) = dx01 * dy02 - dx02 * dy01
		const int crossProduct = dx01 * dy02 - dx02 * dy01;

		if (crossProduct != 0)
		{
			counterClockwise_ = crossProduct < 0;
			return counterClockwise_ == 1u;
		}

		offset++;
	}

	ocean_assert(false && "This should never happen!");
	counterClockwise_ = 1u;
	return true;
}

template <typename T>
bool PixelContourT<T>::isDistinct() const
{
	if (pixels_.size() <= 1)
	{
		return true;
	}

	for (size_t n = 0; n < pixels_.size() - 1; ++n)
	{
		if (pixels_[n] == pixels_[n + 1u])
		{
			return false;
		}
	}

	return pixels_.front() != pixels_.back();
}

template <typename T>
bool PixelContourT<T>::isDense() const
{
	if (pixels_.size() <= 1)
	{
		return true;
	}

	for (size_t n = 1; n < pixels_.size(); ++n)
	{
		if (!pixels_[n - 1].isNeighbor8(pixels_[n]))
		{
			return false;
		}
	}

	return pixels_.back().isNeighbor8(pixels_.front());
}

template <typename T>
bool PixelContourT<T>::isDense4() const
{
	if (pixels_.size() <= 1)
	{
		return true;
	}

	for (size_t n = 1; n < pixels_.size(); ++n)
	{
		if (!pixels_[n - 1].isNeighbor4(pixels_[n]))
		{
			return false;
		}
	}

	return pixels_.back().isNeighbor4(pixels_.front());
}

template <typename T>
bool PixelContourT<T>::isSimplified() const
{
	if (pixels_.size() <= 2)
	{
		return true;
	}

	PixelPosition previousOffset(pixels_[1] - pixels_[0]);

	for (size_t n = 2; n < pixels_.size(); ++n)
	{
		const PixelPosition currentOffset(pixels_[n] - pixels_[n - 1]);

		if (currentOffset == previousOffset)
		{
			return false;
		}

		previousOffset = currentOffset;
	}

	// now the remaining two pixels
	PixelPosition currentOffset(pixels_[0] - pixels_[pixels_.size() - 1]);
	if (previousOffset == currentOffset)
	{
		return false;
	}

	// we avoid: previousOffset = currentOffset;
	if (PixelPosition(pixels_[1] - pixels_[0]) == currentOffset)
	{
		return false;
	}

	return true;
}

template <typename T>
void PixelContourT<T>::makeDistinct()
{
	if (pixels_.size() > 1)
	{
		PixelPositions distinctPixels;
		distinctPixels.reserve(pixels_.size());

		distinctPixels.push_back(pixels_.front());

		for (size_t n = 1; n < pixels_.size(); ++n)
		{
			if (pixels_[n - 1] != pixels_[n])
			{
				distinctPixels.push_back(pixels_[n]);
			}
		}

		if (distinctPixels.size() > 1 && distinctPixels.front() == distinctPixels.back())
		{
			distinctPixels.pop_back();
		}

		ocean_assert(distinctPixels.size() <= 1 || distinctPixels.front() != distinctPixels.back());

		// the bounding box should not have changed
		ocean_assert(!boundingBox_ || boundingBox_ == PixelBoundingBox(distinctPixels));

		mostLeftIndex_ = size_t(-1);
		counterClockwise_ = (unsigned int)(-1);

		pixels_ = std::move(distinctPixels);
	}
}

template <typename T>
void PixelContourT<T>::makeDense()
{
	if (pixels_.size() > 1)
	{
		PixelPositions newPositions;
		newPositions.reserve(pixels_.size() * 20);

		for (size_t n = 0; n < pixels_.size(); ++n)
		{
			const PixelPosition& start = pixels_[n];
			const PixelPosition& end = pixels_[modulo(int(n + 1), int(pixels_.size()))];

			int x = int(start.x());
			int y = int(start.y());
			const int xEnd = int(end.x());
			const int yEnd = int(end.y());

			CV::Bresenham bresenham(x, y, xEnd, yEnd);

			while (x != xEnd || y != yEnd)
			{
				newPositions.push_back(PixelPosition(T(x), T(y)));
				bresenham.findNext(x, y);
			}
		}

		pixels_ = std::move(newPositions);
		ocean_assert(!boundingBox_ || boundingBox_ == PixelBoundingBox(pixels_));

		mostLeftIndex_ = size_t(-1);
		counterClockwise_ = (unsigned int)(-1);
	}
}

template <typename T>
PixelContourT<T> PixelContourT<T>::simplified() const
{
	if (pixels_.size() <= 1)
	{
		return PixelContourT<T>(*this);
	}

	PixelPositions newPixelPositions;
	newPixelPositions.reserve(pixels_.size());

	VectorI2 currentDirection = VectorI2(int(pixels_.front().x() - pixels_.back().x()), int(pixels_.front().y() - pixels_.back().y()));

	for (size_t n = 1; n < pixels_.size(); ++n)
	{
		const VectorI2 newDirection = VectorI2(int(pixels_[n].x() - pixels_[n - 1].x()), int(pixels_[n].y() - pixels_[n - 1].y()));

		if (!newDirection.isNull())
		{
			if (!similar(currentDirection, newDirection))
			{
				currentDirection = newDirection;
				newPixelPositions.push_back(pixels_[n - 1]);
			}
		}
	}

	const VectorI2 newDirection = VectorI2(int(pixels_.front().x() - pixels_.back().x()), int(pixels_.front().y() - pixels_.back().y()));

	if (currentDirection != newDirection)
	{
		newPixelPositions.push_back(pixels_.back());
	}

#ifdef OCEAN_DEBUG
	{
		ocean_assert(!newPixelPositions.empty());

		const PixelContourT<T> debugContour(newPixelPositions);
		ocean_assert(debugContour.boundingBox() == boundingBox());
		ocean_assert(debugContour.isCounterClockwise() == isCounterClockwise());

		ocean_assert(debugContour.isSimplified());
		ocean_assert(debugContour.isDistinct());
	}
#endif

	return PixelContourT<T>(std::move(newPixelPositions), boundingBox_);
}

template <typename T>
void PixelContourT<T>::simplify()
{
	*this = simplified();
}

template <typename T>
PixelContourT<T> PixelContourT<T>::sparseContour(const unsigned int minimalSqrDistance, const size_t startIndex) const
{
	ocean_assert(pixels_.size() >= 1);

	if (pixels_.empty())
	{
		return PixelContourT<T>();
	}

	ocean_assert(minimalSqrDistance >= 1u);
	ocean_assert(startIndex < pixels_.size());

	return PixelContourT<T>(pixels_, minimalSqrDistance, startIndex);
}

template <typename T>
unsigned int PixelContourT<T>::smallestSqrDistanceBetweenPixels() const
{
	ocean_assert(!pixels_.empty());

	unsigned int sqrDistance = pixels_.front().sqrDistance(pixels_.back());

	for (size_t n = 1; n < pixels_.size(); ++n)
	{
		const unsigned int localSqrDistance = pixels_[n - 1].sqrDistance(pixels_[n]);

		if (localSqrDistance < sqrDistance)
		{
			sqrDistance = localSqrDistance;
		}
	}

	return sqrDistance;
}

template <typename T>
unsigned int PixelContourT<T>::largestSqrDistanceBetweenPixels() const
{
	ocean_assert(!pixels_.empty());

	unsigned int sqrDistance = pixels_.front().sqrDistance(pixels_.back());

	for (size_t n = 1; n < pixels_.size(); ++n)
	{
		const unsigned int localSqrDistance = pixels_[n - 1].sqrDistance(pixels_[n]);

		if (localSqrDistance > sqrDistance)
		{
			sqrDistance = localSqrDistance;
		}
	}

	return sqrDistance;
}

template <typename T>
inline PixelContourT<T>::operator bool() const
{
	return !pixels_.empty();
}

template <typename T>
inline bool PixelContourT<T>::similar(const VectorI2& first, const VectorI2& second)
{
	ocean_assert(first.x() != 0 || first.y() != 0);
	ocean_assert(second.x() != 0 || second.y() != 0);

#ifdef OCEAN_DEBUG

	{
		const bool fastResult = first.x() * second.y() == second.x() * first.y()
			&& (0x80000000 & first.x()) == (0x80000000 & second.x())
			&& (0x80000000 & first.y()) == (0x80000000 & second.y());

		Vector2 vf(Scalar(first.x()), Scalar(first.y()));
		Vector2 vs(Scalar(second.x()), Scalar(second.y()));

		vf.normalize();
		vs.normalize();

		ocean_assert(fastResult == (vf == vs));
	}

#endif

	return first.x() * second.y() == second.x() * first.y()
		&& (0x80000000 & first.x()) == (0x80000000 & second.x())
		&& (0x80000000 & first.y()) == (0x80000000 & second.y());
}

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_PIXEL_CONTOUR_H
