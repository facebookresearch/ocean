// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRCodes.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/qrcodes/LegacyQRCodeDetector2D.h"

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
class OCEAN_TEST_CV_DETECTOR_QRCODES_EXPORT TestLegacyQRCodeDetector2D : protected CV::Detector::QRCodes::LegacyQRCodeDetector2D
{
	protected:

		/// Handle for test images, first: filename, second: Everstore handle
		typedef std::pair<std::string, std::string> HandlePair;

		/// Vector of handle pairs
		typedef std::vector<HandlePair> HandlePairs;

	public:

		/**
		 * Invokes all test for the QR code detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Applies a stress test with random input data just ensuring that the detector does not crash.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testStressTest(const double testDuration, Worker& worker);

		/**
		 * Tests the detection of QR codes in images without QR codes (avoid false-positive detections)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param forceFullTest If true, the test will ignore the test duration and will continue until all images have been tested, otherwise test success/failure will be determined after the test duration has been reached.
		 * @return True, if succeeded
		 */
		static bool testDetectQRCodes_0_qrcodes(const double testDuration, Worker& worker, const bool forceFullTest = false);

		/**
		 * Tests the detection of QR codes in images
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param forceFullTest If true, the test will ignore the test duration and will continue until all images have been tested, otherwise test success/failure will be determined after the test duration has been reached.
		 * @return True, if succeeded
		 */
		static bool testDetectQRCodes_1_qrcode(const double testDuration, Worker& worker, const bool forceFullTest = false);

	protected:

		 /**
		  * Loads an image as grayscale image from either Everstore or a specified disk location.
		  * @param handlePair The handle defining the image to be loaded
		  * @return The resulting image grayscale image, if successful
		  */
		static Frame loadTestImage(const HandlePair& handlePair);

		 /**
		  * Returns the absolute path to the test images, if any.
		  * @return Absolute path to test images
		  */
		static std::string testImagesDirectory();

		/**
		 * Returns a list of predefined of Everstore handles (and corresponding filenames) to vetted images without QR codes
		 * @return The list of handles
		 */
		static HandlePairs getHandlePairs_0_qrcodes();

		/**
		 * Returns a list of predefined of Everstore handles (and corresponding filenames) to vetted images with a single QR code
		 * @return The list of handles
		 */
		static HandlePairs getHandlePairs_1_qrcode();
};

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Test
