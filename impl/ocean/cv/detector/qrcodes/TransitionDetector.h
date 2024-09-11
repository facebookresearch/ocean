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

#include <type_traits>

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
		using FindNextPixelFunc = bool(*)(const uint8_t* const, const unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int, CV::Bresenham&, const unsigned int, const unsigned int, unsigned int&, unsigned int&, VectorT2<unsigned int>&, VectorT2<unsigned int>&);

		/**
		 * Function pointer for applying a binary threshold to a pixel to determine whether it is black or white
		 * @sa TransitionDetector::isBlackPixel(), TransitionDetector::isWhitePixel()
		 */
		using PixelBinaryThresholdFunc = bool(*)(const uint8_t*, const uint8_t);

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
		 * @param grayThreshold The threshold that is used to compute the location of the transition with sub-pixel accuracy, range: [0, 255]
		 * @return The transition point
		 */
		static Vector2 computeTransitionPointSubpixelAccuracy(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const VectorT2<unsigned int>& pointInside, const VectorT2<unsigned int>& pointOutside, const unsigned int grayThreshold);

		/**
		 * Determines whether an intensity value is black according to threshold value
		 * @param intensityValue Intensity value in a grayscale image
		 * @param threshold The threshold that is used for the comparison, range: [0, 255]
		 * @return True if the intensity value is black relative to threshold value (i.e. less or equal to a threshold), false otherwise
		 * @tparam T The data type of the intensity value, `T` can be an arithmetic type (integral or floating point type) able to represent values ranging from 0 to 255
		 */
		template<typename T>
		static inline bool isBlack(const T& intensityValue, const T& threshold);

		/**
		 * Determines whether an intensity value is white according to threshold value
		 * @param intensityValue Intensity value in a grayscale image
		 * @param threshold The threshold that is used for the comparison, range: [0, 255]
		 * @return True if the intensity value is white relative to threshold value (i.e. is greater than a threshold), false otherwise
		 * @tparam T The data type of the intensity value, `T` can be an arithmetic type (integer or floating point type) able to represent values ranging from 0 to 255
		 */
		template<typename T>
		static inline bool isWhite(const T& intensityValue, const T& threshold);

		/**
		 * Determines whether a pixel is black according to threshold value
		 * @param yFramePixel Pointer to a pixel in an 8-bit grayscale image, must be valid
		 * @param threshold The threshold that is used for the comparison, range: [0, 255]
		 * @return True if the pixel value is black relative to threshold value (i.e. less or equal to a threshold), false otherwise
		 */
		static inline bool isBlackPixel(const uint8_t* yFramePixel, uint8_t threshold);

		/**
		 * Determines whether a pixel is white according to threshold value
		 * @param yFramePixel Pointer to a pixel in an 8-bit grayscale image, must be valid
		 * @param threshold The threshold that is used for the comparison, range: [0, 255]
		 * @return True if the pixel value is white relative to threshold value (i.e. is greater than a threshold), false otherwise
		 */
		static inline bool isWhitePixel(const uint8_t* yFramePixel, uint8_t threshold);

		/**
		 * Determines whether a transition occurred between pixel values of two neighboring pixels in a row and, if so, calculates the sub-pixel horizontal position of transition point
		 * @param yRow Pointer to a row in a grayscale image `width` pixels wide, must be valid
		 * @param width The width of the input frame, range: [1, infinity)
		 * @param xPointLeft Horizontal position of the left pixel, range: [0, width - 2]
		 * @param grayThreshold Threshold value that is used to check whether a transition occurred between the two neighboring pixels
		 * @param xTransitionPoint Resulting sub-pixel horizontal position of the transition point
		 * @return True if sub-pixel position can be computed (i.e. if transition across `grayThreshold` exists between left and right pixel), otherwise false
		 */
		static inline bool computeHorizontalTransitionPointSubpixelAccuracy(const std::uint8_t* yRow, const unsigned int width, const unsigned int xPointLeft, const std::uint8_t grayThreshold, Scalar& xTransitionPoint);
};

template<typename T>
inline bool TransitionDetector::isBlack(const T& intensityValue, const T& threshold)
{
	static_assert(std::is_arithmetic_v<T> && NumericT<T>::maxValue() >= 255, "`T` must be an arithmetic type able to hold values up to 255!");
	ocean_assert (threshold >= T(0) && threshold <= T(255));

	return intensityValue <= threshold;
}

template<typename T>
inline bool TransitionDetector::isWhite(const T& intensityValue, const T& threshold)
{
	static_assert(std::is_arithmetic_v<T> && NumericT<T>::maxValue() >= 255, "`T` must be an arithmetic type able to hold values up to 255!");
	ocean_assert (threshold >= T(0) && threshold <= T(255));

	return intensityValue > threshold;
}

inline bool TransitionDetector::isBlackPixel(const uint8_t* yFramePixel, uint8_t threshold)
{
	ocean_assert(yFramePixel != nullptr);

	return isBlack(*yFramePixel, threshold);
}

inline bool TransitionDetector::isWhitePixel(const uint8_t* yFramePixel, uint8_t threshold)
{
	ocean_assert(yFramePixel != nullptr);

	return isWhite(*yFramePixel, threshold);
}

inline bool TransitionDetector::computeHorizontalTransitionPointSubpixelAccuracy(const std::uint8_t* yRow, const unsigned int width, const unsigned int xPointLeft, const std::uint8_t grayThreshold, Scalar& xTransitionPoint)
{
	ocean_assert(yRow != nullptr);
	ocean_assert_and_suppress_unused(xPointLeft <= width - 2u, width);

	const std::uint8_t leftPixelValue = yRow[xPointLeft];
	const std::uint8_t rightPixelValue = yRow[xPointLeft + 1u];

	if (leftPixelValue <= grayThreshold == rightPixelValue <= grayThreshold)
	{
		return false;
	}

	xTransitionPoint = xPointLeft + Scalar((int)leftPixelValue - (int)grayThreshold) / ((int)leftPixelValue - (int)rightPixelValue);

	return true;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
