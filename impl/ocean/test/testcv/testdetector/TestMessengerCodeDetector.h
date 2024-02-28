// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_MESSENGER_CODE_DETECTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_MESSENGER_CODE_DETECTOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/MessengerCodeDetector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements a test for the Messenger Code detector.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestMessengerCodeDetector : protected CV::Detector::MessengerCodeDetector
{
	protected:

		/// Handle for test images, first: filename, second: Everstore handle
		typedef std::pair<std::string, std::string> HandlePair;

		/// Vector of handle pairs
		typedef std::vector<HandlePair> HandlePairs;

	public:

		/**
		 * Invokes all test for the Messenger Code detector.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the extraction of code candidates.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testExtractCodeCandidates(const double testDuration);

		/**
		 * Tests the detection of artificial rendered bullseyes.
		 * @param filterSize The size of the Gaussian blur kernel to be applied, with range [1, infinity), must be odd, 0 to avoid blur
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBullseyeDetectionArtificial(const unsigned int filterSize, const double testDuration);

		/**
		 * Applies a stress test.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testStressTest(const double testDuration, Worker& worker);

		/**
		 * Tests real images in which exactly one bullseye is visible (in the center of the image).
		 * The images are either downloaded from Everstore or loaded locally from disk.
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetect1Bullseye(Worker& worker);

		/**
		 * Tests real images in which no Messenger code is visible.
		 * The images are either downloaded from Everstore or loaded locally from disk.
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetect0Code(Worker& worker);

		/**
		 * Tests real images in which exactly one Messenger code is visible.
		 * The images are either downloaded from Everstore or loaded locally from disk.
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetect1Code(Worker& worker);

	protected:

		/**
		 * Calculates four corners for one Messenger Code.
		 * The four corners will be rotated randomly.
		 * @param codeCenter The center of the Messenger Code
		 * @param distance The distance between neighboring corners, with range (0, infinity)
		 * @param randomGenerator Random generator object
		 * @return The four corners
		 */
		static Vectors2 createCodeCorners(const Vector2& codeCenter, const Scalar distance, RandomGenerator& randomGenerator);

		/**
		 * Paints a bullseye into a given 8 bit grayscale frame.
		 * @param frame The frame in which the bullseye will be painted, must be valid
		 * @param width The width of the given frame in pixel, with range [21, infinity)
		 * @param height The height of the given frame in pixel, with range [21, infinity)
		 * @param location The bullseye's center location within the frame
		 * @param radius The bullseye's radius, with range (3, infinity)
		 * @param foregroundColor The foreground color to be used
		 * @param backgroundColor The background color to be used
		 * @param framePaddingElements The number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 */
		static void paintBullseye(unsigned char* frame, const unsigned int width, const unsigned int height, const Vector2& location, const Scalar radius, const unsigned char foregroundColor, const unsigned char backgroundColor, const unsigned int framePaddingElements);

		/**
		 * Paints noise into a given 8 bit grayscale frame.
		 * @param yFrame The frame in which the bullseye will be painted, with pixel format FORMAT_Y8, must be valid
		 * @param location The center location of the noise within the frame
		 * @param foregroundColor The foreground color to be used
		 */
		static void paintNoise(Frame& yFrame, const Vector2& location, const uint8_t foregroundColor);

		/**
		 * Loads a test image either from Everstore or from a specified disk location.
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
		 * Returns a predefined list of handles of images containing exactly one bullseye.
		 * @return The list of handles
		 */
		static HandlePairs testImage1Bullseye();

		/**
		 * Returns a predefined list of Everstore handles of images containing no messenger code.
		 * @return The list of handles
		 */
		static HandlePairs testImages0Code();

		/**
		 * Returns a predefined list of Everstore handles of images containing exactly one messenger code.
		 * @return The list of handles
		 */
		static HandlePairs testImages1Code();
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_MESSENGER_CODE_DETECTOR_H
