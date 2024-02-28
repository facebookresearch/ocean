// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
		 * Tests the detection of finder patterns in row (horizontal detection)
		 * @param filterSize the size of the Gaussian blur kernel to be applied, range: [1, infinity) must be odd, use 0 to disable blurring altogether
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetectFinderPatternSyntheticData(const unsigned int filterSize, const double testDuration, Worker& worker);

	protected:

		 /**
		  * Paint a finder pattern into a grayscale image
		  * @param yFrame Pointer to the data of a grayscale image. Must be valid.
		  * @param width The width of the frame pointed to by `yFrame`, range: [29, infinity)
		  * @param height The height of the frame pointer to by `yFrame`, range: [29, infinity)
		  * @param location The location of the center of the finder pattern that will be drawn. Must inside the image and at least `round(length)` pixels away from frame border
		  * @param length Diameter of the finder pattern in horizontal direction, range: [7, infinity)
		  * @param rotation The rotation angle in radian, range: (-infinity, infinity)
		  * @param foregroundColor Color that is used for the finder pattern (usually black), range: [0, 255]
		  * @param backgroundColor Background color that the finder pattern is drawn on (usually white), range: [0, 255]
		  * @param paddingElements Optional padding elements of the input frame `yFrame`
		  * @param worker Optional worker instance
		  */
		static void paintFinderPattern(uint8_t* yFrame, const unsigned int width, const unsigned int height, const Vector2& location, const Scalar& length, const Scalar& rotation, const unsigned char foregroundColor, const unsigned char backgroundColor, const unsigned int paddingElements = 0u, Worker* worker = nullptr);
};

} // namespace TestQRCodes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Test
