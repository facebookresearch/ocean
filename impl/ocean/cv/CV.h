/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CV_H
#define META_OCEAN_CV_CV_H

#include "ocean/base/Base.h"

namespace Ocean
{

namespace CV
{

/**
 * @defgroup cv Ocean Computer Vision (CV) Library
 * @{
 * The Ocean CV Library provides basic Computer Vision functionalities.
 * There exist several further specialized Computer Vision libraries, like e.g,. the Advanced Computer Vision library or the Computer Vision libraries addressing feature detection tasks.<br>
 * The library is platform independent.
 *
 * The FrameConverterABGR32, FrameConverterARGB32, ... classes implement functions to convert frame data with specific pixel format to different pixel formats (and e.g., different pixel origins).
 * The FrameConverter::Comfort::change() and FrameConverter::Comfort::convert() functions provide a simple interface for frame conversion task.
 *
 * The Bresenham and Canvas class provide drawing and painting functionalities (also with sub-pixel accuracies).
 *
 * Several individual common frame filter are implemented in e.g., FrameFilterGaussian, FrameFilterSeparable, FrameFilterDilation, ..., FrameFilterScharr, ...
 *
 * The FrameShrinker allows to reduce the size of a frame while the FrameEnlarger allows to increase the size of a frame.
 * The FrameShrinkerAlpha additionally provides specific support for frames with alpha channel.
 *
 * The SSE class provides SIMD functions for x86, x64 platforms while the NEON class provides SIMD functions for Android ARM platforms.
 *
 * The PixelPosition class stores a 2D pixel-accurate position, the PixelBoundingBox class implements a bounding box with pixel-accuracy.
 *
 * The NonMaximumSuppression class provides a multi-core-capable solution to determine peaks in filter responses e.g., for feature point detection.
 *
 * The SubRegion class can be used to determine a sub-region within a frame either by defining a mask or by defining the 2D coordinates of the contour.
 *
 * The SumAbsoluteDifferences, the SumSquareDifferences and the ZeroMeanSumSquareDifferences classes provide functions measuring distances between image regions (patches).
 * @}
 */

/**
 * @namespace Ocean::CV Namespace of the CV library.<p>
 * The Namespace Ocean::CV is used in the entire Ocean CV Library.
 */

// Defines OCEAN_CV_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_EXPORT
		#define OCEAN_CV_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_EXPORT
#endif

/**
 * Definition of individual directions with pixel accuracy.
 * The values of the individual directions are defined by the angle (in counter clockwise order) in degree.<br>
 * The default direction is the north direction (pointing upwards in an image with pixel origin in the top left corner).<br>
 * Here is a visualization of the individual pixel directions:
 * <pre>
 * Image with origin in top left corner (denoted by X) and the pixel position P:
 * X---------------------       X---------------------
 * |                     |      |                     |
 * |    NW    N    NE    |      |    45    0    315   |
 * |                     |      |                     |
 * |     W    P    E     |      |    90    P    270   |
 * |                     |      |                     |
 * |    SW    S    SE    |      |   135   180   225   |
 * |                     |      |                     |
 *  ---------------------        ---------------------
 * </pre>
 * Beware: An invalid pixel direction (PD_INVALID) has the value -1 and not 0 as often for other enums.
 * @ingroup cv
 */
enum PixelDirection : int32_t
{
	// Invalid direction.
	PD_INVALID = -1,
	// North direction.
	PD_NORTH = 0,
	// North west direction.
	PD_NORTH_WEST = 45,
	// West direction.
	PD_WEST = 90,
	// South west direction.
	PD_SOUTH_WEST = 135,
	// South direction.
	PD_SOUTH = 180,
	// South east direction.
	PD_SOUTH_EAST = 225,
	// East direction.
	PD_EAST = 270,
	// North east direction.
	PD_NORTH_EAST = 315
};

/**
 * Definition of a vector holding pixel directions.
 * @ingroup cv
 */
typedef std::vector<PixelDirection> PixelDirections;

/**
 * Definition of individual centers of pixels.
 * @ingroup cv
 */
enum PixelCenter : uint32_t
{
	/**
	 * The center of a pixel is in the upper-left corner of each pixel's square.
	 * Below an image with 2x2 pixels, the pixel center is denoted by 'X':
	 * <pre>
	 *  X-------X-------
	 *  |       |       |
	 *  |       |       |    <- first pixel row of image
	 *  |       |       |
	 *  X-------X-------
	 *  |       |       |
	 *  |       |       |    <- second pixel row of image
	 *  |       |       |
	 *   ------- -------
	 * </pre>
	 */
	PC_TOP_LEFT,

	/**
	 * The center of a pixel is located in the center of each pixel's square (with an offset of 0.5, 0.5).
	 * Below an image with 2x2 pixels, the pixel center is denoted by 'X':
	 * <pre>
	 *   ------- -------
	 *  |       |       |
	 *  |   X   |   X   |    <- first pixel row of image
	 *  |       |       |
	 *   ------- -------
	 *  |       |       |
	 *  |   X   |   X   |    <- second pixel row of image
	 *  |       |       |
	 *   ------- -------
	 * </pre>
	 */
	PC_CENTER
};

}

}

#endif // META_OCEAN_CV_CV_H
