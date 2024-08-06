/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/qrcodes/QRCodeDetector2D.h"

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
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT TestQRCodeDetector2D : protected CV::Detector::QRCodes::QRCodeDetector2D
{
	public:

		/**
		 * Invokes all test for the QR code detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker A worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Applies a stress test with random input data just ensuring that the detector does not crash.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker A worker object
		 * @return True, if succeeded
		 */
		static bool testStressTest(const double testDuration, Worker& worker);

		/**
		 * Test for the detection of QR codes using synthetic test images with small dimensions
		 * @param gaussianFilterSize Size of a Gaussian filter that is applied to the synthesized data; no filter is applied for the value 0, range: [0, infinity), must be an odd value
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker A worker object
		 * @return True, if succeeded
		 */
		static bool testDetectQRCodesSmallImageSyntheticData(const unsigned int gaussianFilterSize, const double testDuration, Worker& worker);

		/**
		 * Test for the detection of QR codes using synthetic test images with large dimensions
		 * @param gaussianFilterSize Size of a Gaussian filter that is applied to the synthesized data; no filter is applied for the value 0, range: [0, infinity), must be an odd value
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker A worker object
		 * @return True, if succeeded
		 */
		static bool testDetectQRCodesLargeImageSyntheticData(const unsigned int gaussianFilterSize, const double testDuration, Worker& worker);

	protected:

		/**
		 * Test for the detection of QR codes using synthetic test images
		 * @param gaussianFilterSize Size of a Gaussian filter that is applied to the synthesized data; no filter is applied for the value 0, range: [0, infinity), must be an odd value
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker A worker object
		 * @param testLable Label specified by caller test function, used in log messages and within generated file names.  Must be alphanumeric.
		 * @param validationPrecisionThreshold Proportion of QR code detection attempts that must succeed for the test to pass, range: [0, 1]
		 * @param moduleSizePixelsMin Minimum module size in pixels, range: [1, infinity)
		 * @param moduleSizePixelsMax Maximum module size in pixels, range: [moduleSizePixelsMin, infinity)
		 * @param imageDimPixelsMin Hard limit on how small width or height of generated image can be, in terms of pixels, range: [0, infinity)
		 * @param imageDimPixelsMax Soft limit on how large width or height of generated image can be, in terms of pixels, range: [0, infinity)
		 * @return True, if succeeded
		 * @remark Images generated may have width or height greater than `imageDimPixelsMax` if the image needs to be larger to fit the QR code generated due to other parameter values
		 */
		static bool testDetectQRCodesSyntheticData_Internal(const unsigned int gaussianFilterSize, const double testDuration, Worker& worker, const std::string& testLabel, const double validationPrecisionThreshold = 0.99, const unsigned int moduleSizePixelsMin = 6u, const unsigned int moduleSizePixelsMax = 8u, const unsigned int imageDimPixelsMin = 0u, const unsigned int imageDimPixelsMax = 720u);
};

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Test
