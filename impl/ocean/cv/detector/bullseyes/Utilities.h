/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_CV_DETECTOR_BULLSEYES_UTILITIES_H
#define OCEAN_CV_DETECTOR_BULLSEYES_UTILITIES_H

#include "ocean/cv/detector/bullseyes/Bullseyes.h"

#include "ocean/base/Frame.h"

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
		 * @param quietZone The width of the quiet zone in pixels, with range [0, infinity)
		 * @param rgbFrame The resulting bullseye image
		 * @param foregroundColor Optional foreground color of the bullseye, nullptr to use the default color (black)
		 * @param backgroundColor Optional background color of the bullseye, nullptr to use the default color (white)
		 * @return True, if succeeded
		 */
		static bool createBullseyeImage(const unsigned int diameter, const unsigned int quietZone, Frame& rgbFrame, const uint8_t* foregroundColor = nullptr, const uint8_t* backgroundColor = nullptr);
};

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_UTILITIES_H
