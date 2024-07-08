/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"

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
 * This class implements a test for the QR code detector.
 * @ingroup testcvdetectorqrcodes
 */
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT TestFinderPatternDetector : protected CV::Detector::QRCodes::FinderPatternDetector
{
	public:

		/**
		 * Invokes all test for the QR code detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		 /**
		 * Tests the detection of finder patterns in row (horizontal detection)
		 * @param filterSize the size of the Gaussian blur kernel to be applied, range: [1, infinity) must be odd, use 0 to disable blurring altogether
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetectFinderPatternSyntheticData(const unsigned int filterSize, const double testDuration, Worker& worker);

	protected:

		 /**
		  * Paint a finder pattern into a grayscale image.
		  * @param yFrame The grayscale image, with pixel format FORMAT_Y8, with resolution [29, infinity)x[29, infinity), must be valid
		  * @param location The location of the center of the finder pattern that will be drawn. Must inside the image and at least `round(length)` pixels away from frame border
		  * @param length Diameter of the finder pattern in horizontal direction, range: [7, infinity)
		  * @param rotation The rotation angle in radian, range: (-infinity, infinity)
		  * @param foregroundColor Color that is used for the finder pattern (usually black), range: [0, 255]
		  * @param backgroundColor Background color that the finder pattern is drawn on (usually white), range: [0, 255]
		  * @param worker Optional worker instance
		  */
		static void paintFinderPattern(Frame& yFrame, const Vector2& location, const Scalar& length, const Scalar& rotation, const uint8_t foregroundColor, const uint8_t backgroundColor, Worker* worker = nullptr);
};

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
