/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/cv/Bresenham.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * Definition of functions related to the detection of pixel transitions
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT TransitionDetector
{
	public:

		/**
		 * Function pointer to functions that detect a transition
		 * @sa TransitionDetector::findNextPixel<bool>()
		 */
		typedef bool (*FindNextPixelFunc)(const uint8_t* const, const unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int, CV::Bresenham&, const unsigned int, const unsigned int, unsigned int&, unsigned int&, VectorT2<unsigned int>&, VectorT2<unsigned int>&);


		/**
		 * Function pointer for pixel comparison functions
		 * @sa TransitionDetector::isLessOrEqual(), TransitionDetector::isGreater()
		 */
		typedef bool (*PixelComparisonFunc)(const uint8_t*, const unsigned int);

	public:

		/**
		 * Finds a the next dark or the next light pixel in a specified direction.
		 * @param yPointer The pointer to the location within the image at which the search will start, must be valid
		 * @param x Horizontal location within the image as input, with range [1, width - 1]
		 * @param y Vertical location within the image as input, with range [1, height - 1]
		 * @param width The width of the input image, range: [1, infinity)
		 * @param height The height of the input image, range: [1, infinity)
		 * @param yPointerPaddingElements Number of padding elements in the input image. If no stride if used, this should be 0, range: [0, infinity)
		 * @param bresenham Bresenham object that defines a scanline direction, must be valid
		 * @param maximumDistance The maximum number of rows (or columns) to travel (Manhattan distance), range: [1, infinity)
		 * @param threshold The threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param rows The resulting number of rows that have been traveled until the black or white pixel has been found
		 * @param columns The resulting number of columns that have been traveled until the black or white pixel has been found
		 * @param lastPointInside The resulting coordinates of the last point *before* the intensity switch
		 * @param firstPointOutside The resulting coordinates of the last point *after* the intensity switch
		 * @return True, if the black or white pixel have been found within the specified range of [1, maximalManhattanDistance]
		 * @tparam tFindBlackPixel True, to find the next black pixel; False, to find the next white pixel
		 */
		template <bool tFindBlackPixel>
		static bool findNextPixel(const uint8_t* const yPointer, const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const unsigned int yPointerPaddingElements, CV::Bresenham& bresenham, const unsigned int maximumDistance, const unsigned int threshold, unsigned int& columns, unsigned int& rows, VectorT2<unsigned int>& lastPointInside, VectorT2<unsigned int>& firstPointOutside);

		/**
		 * Finds either the next black or the next white pixel towards negative y direction (upwards in an image).
		 * @param yPointer The pointer to the location within the image at which the search will start, must be valid
		 * @param y The current vertical location within the image, with range [1, height - 1]
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param maximalRows The maximal number of rows (vertical pixels) to travel, with range [1, height - y]
		 * @param threshold The threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param frameStrideElements The number of horizontal stride elements in the image, which is width + paddingElements, with range [width, infinity)
		 * @param rows The resulting number of rows that have been traveled until the black or white pixel has been found
		 * @return True, if the black or white pixel have been found within the specified range of [1, maximalRows]
		 * @tparam tFindBlackPixel True, to find the next black pixel; False, to find the next white pixel
		 */
		template <bool tFindBlackPixel>
		static bool findNextUpperPixel(const uint8_t* yPointer, unsigned int y, const unsigned int height, const unsigned int maximalRows, const unsigned int threshold, const unsigned int frameStrideElements, unsigned int& rows);

		/**
		 * Finds either the next black or the next white pixel towards positive y direction (downwards in an image).
		 * @param yPointer The pointer to the location within the image at which the search will start, must be valid
		 * @param y The current vertical location within the image, with range [1, height - 1]
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param maximalRows The maximal number of rows (vertical pixels) to travel, with range [1, height - y]
		 * @param threshold The threshold separating a bright pixel from a dark pixel, with range [0, 255]
		 * @param frameStrideElements The number of horizontal stride elements in the image, which is width + paddingElements, with range [width, infinity)
		 * @param rows The resulting number of rows that have been traveled until the black or white pixel has been found
		 * @return True, if the black or white pixel have been found within the specified range of [1, maximalRows]
		 * @tparam tFindBlackPixel True, to find the next black pixel; False, to find the next white pixel
		 */
		template <bool tFindBlackPixel>
		static bool findNextLowerPixel(const uint8_t* yPointer, unsigned int y, const unsigned int height, const unsigned int maximalRows, const unsigned int threshold, const unsigned int frameStrideElements, unsigned int& rows);

		/**
		 * Determines the sub-pixel location of a fiducial marker given its approximate location
		 * @param yFrame The 8-bit grayscale frame in which the alignment patterns were detected, must be valid
		 * @param width The width of the input frame in pixels, with range [21, infinity)
		 * @param height The height of the given frame in pixel, with range [21, infinity)
		 * @param framePaddingElements Optional number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param xCenter The horizontal location of the center of the fiducial, with range [0, width - 1]
		 * @param yCenter The vertical location of the center of the fiducial, with range [0, height - 1]
		 * @param isNormalReflectance The reflectance of the fiducial, which indicates whether the center location is dark (normal reflectance) or light (inverted reflectance)
		 * @param grayThreshold The threshold separating a foreground and background pixels, with range [0, 255]
		 * @param location The resulting sub-pixel location of the center of the fiducial
		 * @return True, if the sub-pixel location could be determined
		 */
		static bool determineSubPixelLocation(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int xCenter, const unsigned int yCenter, const bool isNormalReflectance, const unsigned int grayThreshold, Vector2& location);

		/**
		 * Computes the intensity transition point with sub-pixel accuracy given the two image points around a transition point (before and after)
		 * @param yFrame Pointer to a grayscale image of size `width x height` pixels, must be valid
		 * @param width The width of the input frame, range: [1, infinity)
		 * @param height The height of the input frame, range: [1, infinity)
		 * @param yFramePaddingElements Number of padding elements of this frame, range: [0, infinity)
		 * @param pointInside The point before the intensity transition, range: x,y in [0, infinity)
		 * @param pointOutside The point after the intensity transition, range: x,y in [0, infinity)
		 * @param grayTreshold The threshold that is used to compute the location of the transition with sub-pixel accuracy
		 * @return True if the transition point was computed successfully, otherwise false
		 */
		static Vector2 computeTransitionPointSubpixelAccuracy(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const VectorT2<unsigned int>& pointInside, const VectorT2<unsigned int>& pointOutside, const unsigned int grayTreshold);

		/**
		 * Returns true if the pixel is less or equal to a threshold
		 * @param yFramePixel Pointer to a pixel in an 8-bit grayscale image, must be value
		 * @param threshold The threshold that is used for the comparison, range: [0, 256)
		 */
		static inline bool isLessOrEqual(const uint8_t* yFramePixel, unsigned int threshold);

		/**
		 * Returns true if the pixel is larger than a threshold
		 * @param yFramePixel Pointer to a pixel in an 8-bit grayscale image, must be value
		 * @param threshold The threshold that is used for the comparison, range: [0, 256)
		 */
		static inline bool isGreater(const uint8_t* yFramePixel, unsigned int threshold);
};

inline bool TransitionDetector::isLessOrEqual(const uint8_t* yFramePixel, unsigned int threshold)
{
	ocean_assert(yFramePixel != nullptr);
	ocean_assert(threshold < 256u);

	return *yFramePixel <= threshold;
}

inline bool TransitionDetector::isGreater(const uint8_t* yFramePixel, unsigned int threshold)
{
	ocean_assert(yFramePixel != nullptr);
	ocean_assert(threshold < 256u);

	return *yFramePixel > threshold;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
