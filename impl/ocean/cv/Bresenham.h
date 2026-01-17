/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_BRESENHAM_H
#define META_OCEAN_CV_BRESENHAM_H

#include "ocean/cv/CV.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Line2.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements Bresenham's line algorithms.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT Bresenham
{
	public:

		/**
		 * Definition of different line orientations.
		 */
		enum Orientation : uint32_t
		{
			/// Invalid orientation.
			ORIENTATION_INVALID = 0u,
			/// Line orientation with range [0, 45] degree.
			ORIENTATION_0,
			/// Line orientation with range (45, 90] degree.
			ORIENTATION_1,
			/// Line orientation with range [90, 135) degree.
			ORIENTATION_2,
			/// Line orientation with range [135, 180] degree.
			ORIENTATION_3,
			/// Line orientation with range [180, 225] degree.
			ORIENTATION_4,
			/// Line orientation with range (225, 270] degree.
			ORIENTATION_5,
			/// Line orientation with range (270, 315] degree.
			ORIENTATION_6,
			/// Line orientation with range (315, 360] degree.
			ORIENTATION_7
		};

	public:

		/**
		 * Creates an invalid object.
		 */
		Bresenham() = default;

		/**
		 * Creates a new Bresenham line.
		 * @param x0 Horizontal start position of the line, with range (-infinity, infinity)
		 * @param y0 Vertical start position of the line, with range (-infinity, infinity)
		 * @param x1 Horizontal stop position of the line (inclusive), with range (-infinity, infinity)
		 * @param y1 Vertical stop position of the line (inclusive), with range (-infinity, infinity)
		 */
		Bresenham(const int x0, const int y0, const int x1, const int y1);

		/**
		 * Applies one Bresenham step to find the next pixel.
		 * @param x Current horizontal position which will be updated, with range (-infinity, infinity)
		 * @param y Current vertical position which will be updated, with range (-infinity, infinity)
		 */
		void findNext(int& x, int& y);

		/**
		 * Returns the current orientation of the bresenham line.
		 * @return The current orientation
		 */
		inline Orientation orientation() const;

		/**
		 * Returns whether this object holds a valid line.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Computes the pixel-precise border intersection (the begin and end position) of a sub-pixel-precise 2D line which needs to be visible within a specified pixel-precise frame only.
		 * The begin position will be at the left border, otherwise at the top border or bottom border (in this order).
		 * @param line The 2D line for which the begin position and end position will be determined, must be valid
		 * @param leftBorder Left inclusive pixel border (e.g. could be 0), with range (-infinity, infinity)
		 * @param topBorder Top inclusive pixel border (e.g. could be 0), with range (-infinity, infinity)
		 * @param rightBorder Right inclusive pixel border (e.g. could be width - 1), with range [leftBorder, infinity)
		 * @param bottomBorder Bottom inclusive pixel border (e.g. could be height - 1), with range [topBorder, infinity)
		 * @param x0 Resulting horizontal begin position in pixel, with range [leftBorder, rightBorder]
		 * @param y0 Resulting vertical begin position in pixel, with range [topBorder, bottomBorder]
		 * @param x1 Resulting horizontal end position in pixel, with range [leftBorder, rightBorder]
		 * @param y1 Resulting vertical end position in pixel, with range [topBorder, bottomBorder]
		 * @return True, if the given line lies inside the defined frame
		 */
		static bool borderIntersection(const Line2& line, const int leftBorder, const int topBorder, const int rightBorder, const int bottomBorder, int& x0, int& y0, int& x1, int& y1);

		/**
		 * Computes the sub-pixel-precise border intersection (the begin and end position) of a sub-pixel-precise 2D line which needs to be visible within a specified pixel-precise frame only.
		 * The begin position will be at the left border, otherwise at the top border or bottom border (in this order).
		 * @param line The 2D line for which the begin position and end position will be determined, must be valid
		 * @param leftBorder Left inclusive pixel border (e.g. could be 0), with range (-infinity, infinity)
		 * @param topBorder Top inclusive pixel border (e.g. could be 0), with range (-infinity, infinity)
		 * @param rightBorder Right inclusive pixel border (e.g. could be width - 1), with range [leftBorder, infinity)
		 * @param bottomBorder Bottom inclusive pixel border (e.g. could be height - 1), with range [topBorder, infinity)
		 * @param x0 Resulting horizontal begin position in pixel, with range [leftBorder, rightBorder]
		 * @param y0 Resulting vertical begin position in pixel, with range [topBorder, bottomBorder]
		 * @param x1 Resulting horizontal end position in pixel, with range [leftBorder, rightBorder]
		 * @param y1 Resulting vertical end position in pixel, with range [topBorder, bottomBorder]
		 * @return True, if the given line lies inside the defined frame
		 */
		static bool borderIntersection(const Line2& line, const Scalar leftBorder, const Scalar topBorder, const Scalar rightBorder, const Scalar bottomBorder, Scalar& x0, Scalar& y0, Scalar& x1, Scalar& y1);

		/**
		 * Determines the number of pixels necessary for a line between two given points.
		 * The number of pixels include the begin and end points.
		 * @param x0 Horizontal begin position of the line, with range (-infinity, infinity)
		 * @param y0 Vertical begin position of the line, with range (-infinity, infinity)
		 * @param x1 Horizontal end position of the line, with range (-infinity, infinity)
		 * @param y1 Vertical end position of the line, with range (-infinity, infinity)
		 * @return Number of line pixels, with range [1, infinity)
		 */
		static unsigned int numberLinePixels(const int x0, const int y0, const int x1, const int y1);

		/**
		 * Determines the intermediate pixels between two given pixels.
		 * @param position0 Begin position of the line (excluding), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param position1 End position of the line (excluding), with range (-infinity, infinity)x(-infinity, infinity)
		 * @param pixels The resulting intermediate pixels
		 */
		static void intermediatePixels(const PixelPosition& position0, const PixelPosition& position1, PixelPositions& pixels);

		/**
		 * Determines the pixels on the circumference of a circle using the midpoint circle algorithm.
		 * The pixels are computed using 8-way symmetry and are returned in no particular order.
		 * @param centerX Horizontal center position of the circle, with range (-infinity, infinity)
		 * @param centerY Vertical center position of the circle, with range (-infinity, infinity)
		 * @param radius Radius of the circle in pixels, with range [0, infinity)
		 * @param pixels The resulting circle pixels, will be appended to existing content
		 */
		static void circlePixels(const int centerX, const int centerY, const unsigned int radius, PixelPositionsI& pixels);

		/**
		 * Determines the number of pixels on the circumference of a circle.
		 * @param radius Radius of the circle in pixels, with range [0, infinity)
		 * @return Number of circle pixels, with range [1, infinity)
		 */
		static size_t numberCirclePixels(const unsigned int radius);

	protected:

		/// Orientation parameter.
		Orientation orientation_ = ORIENTATION_INVALID;

		/// Control parameter.
		int control_ = 0;

		/// Horizontal update value.
		int updateX_ = 0;

		/// Horizontal and vertical update value.
		int updateXY_ = 0;
};

inline Bresenham::Orientation Bresenham::orientation() const
{
	return orientation_;
}

inline bool Bresenham::isValid() const
{
	return orientation_ != ORIENTATION_INVALID;
}

}

}

#endif // META_OCEAN_CV_BRESENHAM_H
