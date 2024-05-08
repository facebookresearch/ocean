/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestQRCodes
{

/**
 * This class implements tests for the alignment pattern detector
 * @ingroup testcvdetectorqrcodes
 */
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT TestAlignmentPatternDetector
{
	public:

		/**
		 * Tests the functions of the alignment pattern detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Test for the detection of alignment patterns using synthetic test data
		 * @param gaussianFilterSize Size of a Gaussian filter that is applied to the synthesized data; no filter is applied for the value 0, range: [0, infinity), must be an odd value
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDetectAlignmentPatternsSyntheticData(const unsigned int gaussianFilterSize, const double testDuration);

	protected:

		/**
		 * Draws a single (synthetic) alignment pattern
		 * @param yFrame The 8-bit grayscale image that the alignment pattern will be drawn into, must be valid and the origin in the upper left corner
		 * @param location The location in the input image where the alignment pattern will be draw
		 * @param patternSizeInPixels The side length of the alignment pattern in pixels with which it will be drawn, range: [5, infinity)
		 * @param rotation The angle in radian by which the alignment pattern will be rotated, range: [0, PI/2]
		 * @param foregroundColor The pixel value that will be used for foreground elements, range: [0, 255]
		 * @param backgroundColor The pixel value that will be used for background elements, range: [0, 255]
		 */
		static void drawAlignmentPattern(Frame& yFrame, const Vector2& location, const Scalar patternSizeInPixels, const Scalar rotation, const uint8_t foregroundColor, const uint8_t backgroundColor);
};

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namepace Test
