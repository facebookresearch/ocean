/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_CONTOUR_ANALYZER_H
#define META_OCEAN_CV_SEGMENTATION_CONTOUR_ANALYZER_H

#include "ocean/cv/segmentation/Segmentation.h"
#include "ocean/cv/segmentation/PixelContour.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Math.h"

#include <vector>

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * This class implements functions analyzing contours.
 * @ingroup cvsegmentation
 */
class OCEAN_CV_SEGMENTATION_EXPORT ContourAnalyzer
{
	public:

		/**
		 * Definition of a vector holding indices.
		 */
		typedef std::vector<unsigned int> Indices;

	protected:

		/**
		 * This class implements a simple storage for a point index with corresponding dense parameter.
		 * The class is mainly a helper class allowing to sort several dense objects according to their value.
		 */
		class DenseObject
		{
			public:

				/**
				 * Creates an default dense object.
				 */
				DenseObject() = default;

				/**
				 * Creates a new dense object.
				 * @param dense The dense value of the object
				 * @param index The index of the object
				 */
				inline DenseObject(const Scalar dense, const unsigned int index);

				/**
				 * Returns the dense value of this object.
				 * @return Dense value
				 */
				inline Scalar dense() const;

				/**
				 * Returns the index of this object.
				 * @return Object index
				 */
				inline unsigned int index() const;

				/**
				 * Returns whether the dense value of this object is smaller than that of a given second object.
				 * @param object Second object to compare the dense with
				 * @return True, if so
				 */
				inline bool operator<(const DenseObject& object) const;

			private:

				/// Object dense value.
				Scalar dense_ = Scalar(-1);

				/// Object index value.
				unsigned int index_ = (unsigned int)(-1);
		};

		/// Definition of a vector holding dense objects.
		typedef std::vector<DenseObject> DenseObjects;

	public:

		/**
		 * Determines the curvature for each contour pixel.
		 * The resulting values are the cosine value of the angles between two vectors starting at the interest point pointing several pixels forward and backward in the contour.
		 * @param contour The contour for which the curvature parameters will be determined
		 * @param offset Pixel offset inside the contour between the pixel of interest and the pixels to measure the curvature, with range [1, contour.size())
		 * @return Cosine values giving the curvature of the contour, one value for each pixel in the contour (a value of 1 determines an angle of 90 deg, a value of 0 determines a value of +/- 180 deg)
		 */
		static Scalars curvature(const PixelPositions& contour, const unsigned int offset);

		/**
		 * Creates a dense and distinct contour from any kind of given contour also ensuring that the resulting contour does not contain complex properties like loops.
		 * In contrast to e.g., PixelContour::makeDense(), this function needs significantly more computational time while is able to handle more complex input contours.
		 * @param points The points of any kind of contour which will be converted to a dense and distinct contour
		 * @return The resulting dense and distinct contour
		 * @see PixelContour::makeDense().
		 */
		static PixelContour createDenseContour(const PixelPositions& points);

		/**
		 * Creates a dense and distinct contour from any kind of given contour also ensuring that the resulting contour does not contain complex properties like loops.
		 * In contrast to e.g., PixelContour::makeDense(), this function needs significantly more computational time while is able to handle more complex input contours.
		 * @param points The points of any kind of contour which will be converted to a dense and distinct contour with sub-pixel accuracy
		 * @return The resulting dense and distinct contour
		 */
		static inline PixelContour createDenseContour(const Vectors2& points);

		/**
		 * Equalized a sparse contour according to the density of the locations of contour points.
		 * @param contour The contour to be equalized
		 * @return The resulting equalized contour
		 */
		static Vectors2 equalizeContourDensity(const Vectors2& contour);

	private:

		/**
		 * Calculates a hash value for a given pixel position.
		 * @param position Pixel position to determine the hash function for
		 * @return Resulting hash function
		 */
		static inline size_t pixelPositionHashValueFunction(const PixelPosition& position);

		/**
		 * Returns the pixel direction of two successive pixels in a dense contour.
		 * @param pixel0 First pixel
		 * @param pixel1 Following pixel
		 * @return Resulting pixel direction
		 */
		static inline PixelDirection determinePixelDirection(const PixelPosition& pixel0, const PixelPosition& pixel1);
};

inline ContourAnalyzer::DenseObject::DenseObject(const Scalar dense, const unsigned int index) :
	dense_(dense),
	index_(index)
{
	// nothing to do here
}

inline Scalar ContourAnalyzer::DenseObject::dense() const
{
	return dense_;
}

inline unsigned int ContourAnalyzer::DenseObject::index() const
{
	return index_;
}

inline bool ContourAnalyzer::DenseObject::operator<(const DenseObject& object) const
{
	return dense_ < object.dense_;
}

inline PixelContour ContourAnalyzer::createDenseContour(const Vectors2& points)
{
	return createDenseContour(PixelPosition::vectors2pixelPositions(points));
}

inline size_t ContourAnalyzer::pixelPositionHashValueFunction(const PixelPosition& position)
{
	// we simply shift the x-coordinate to the upper bits and take the y-coordinate for the lower bits

#ifdef OCEAN_DEBUG
	const size_t size_tBits_2 = sizeof(size_t) * 4u;
	const size_t maximalValue = size_t(1) << size_tBits_2;
	ocean_assert(size_t(position.x()) < maximalValue && size_t(position.y()) < maximalValue);
#endif

	return (size_t(position.x()) << sizeof(size_t) * 4) | size_t(position.y());
}

inline PixelDirection ContourAnalyzer::determinePixelDirection(const PixelPosition& pixel0, const PixelPosition& pixel1)
{
	ocean_assert(pixel0 && pixel1);
	ocean_assert(pixel0.isNeighbor8(pixel1));

	const unsigned int parameter = (0x0000FFFF & (pixel1.x() - pixel0.x())) | ((pixel1.y() - pixel0.y()) << 16);

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

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_CONTOUR_ANALYZER_H
