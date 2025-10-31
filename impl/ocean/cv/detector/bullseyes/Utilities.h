/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_CV_DETECTOR_BULLSEYES_UTILITIES_H
#define OCEAN_CV_DETECTOR_BULLSEYES_UTILITIES_H

#include "ocean/cv/detector/bullseyes/Bullseyes.h"

#include "ocean/cv/detector/bullseyes/Bullseye.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/PixelPosition.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

/**
 * This class implements utility functions for the Bullseyes library.
 * @ingroup cvdetectorbullseyes
 */
class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT Utilities
{
	public:

		/**
		 * Creates a bullseye image with a given diameter and quiet zone.
		 * @param diameter The diameter of the bullseye in pixels, with range [5, infinity) and must be a multiple of 5
		 * @param emptyBorder The width of an empty border around the bullseye (quiet zone), in pixels, with range [0, infinity)
		 * @param rgbFrame The resulting bullseye image, the frame will be created internally
		 * @param foregroundColor Optional foreground color of the bullseye, nullptr to use the default color (black)
		 * @param backgroundColor Optional background color of the bullseye, nullptr to use the default color (white)
		 * @return True, if succeeded
		 */
		static bool createBullseyeImage(const unsigned int diameter, const unsigned int emptyBorder, Frame& rgbFrame, const uint8_t* foregroundColor = nullptr, const uint8_t* backgroundColor = nullptr);

		/**
		 * Draws a bullseye pattern into a given frame at a specified offset.
		 * @param rgbFrame The frame in which the bullseye will be drawn, must be valid and have a pixel format RGB24 (or compatible)
		 * @param offset The top-left position where the bullseye will be drawn, must be within frame bounds considering the bullseye size
		 * @param diameter The diameter of the bullseye in pixels, with range [5, infinity) and must be a multiple of 5
		 * @param emptyBorder The width of an empty border around the bullseye (quiet zone), in pixels, with range [0, infinity)
		 * @param foregroundColor Optional foreground color of the bullseye, nullptr to use the default color (black)
		 * @param backgroundColor Optional background color of the bullseye, nullptr to use the default color (white)
		 * @return True, if succeeded
		 */
		static bool drawBullseyeWithOffset(Frame& rgbFrame, const PixelPosition& offset, const unsigned int diameter, const unsigned int emptyBorder, const uint8_t* foregroundColor = nullptr, const uint8_t* backgroundColor = nullptr);

		/**
		 * Draws the location of a bullseye into a given frame.
		 * @param rgbFrame The frame in which the bullseye will be painted, must be valid and have a pixel format RGB24 (or compatible)
		 * @param bullseye The bullseye to be painted, must be valid
		 * @param color Optional color to be used, one value for each frame channel, nullptr to use the default color (red)
		 */
		static void drawBullseye(Frame& rgbFrame, const Bullseye& bullseye, const uint8_t* color = nullptr);

		/**
		 * Draws the locations of multiple bullseyes into a given frame.
		 * @param rgbFrame The frame in which the bullseyes will be painted, must be valid and have a pixel format RGB24 (or compatible)
		 * @param bullseyes The bullseyes to be painted, must be valid
		 * @param numberBullseyes The number of bullseyes to be painted, with range [0, infinity)
		 * @param color Optional color to be used, one value for each frame channel, nullptr to use the default color (red)
		 */
		static void drawBullseyes(Frame& rgbFrame, const Bullseye* bullseyes, const size_t numberBullseyes, const uint8_t* color = nullptr);
};

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_UTILITIES_H
