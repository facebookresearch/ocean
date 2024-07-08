/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_MESSENGER_CODE_DETECTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_MESSENGER_CODE_DETECTOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/MessengerCodeDetector.h"

#include "ocean/test/TestDataCollection.h"
#include "ocean/test/TestDataManager.h"

#ifdef OCEAN_USE_LOCAL_TEST_DATA_COLLECTION
	#ifndef OCEAN_USE_TEST_DATA_COLLECTION
		#define OCEAN_USE_TEST_DATA_COLLECTION
	#endif
#endif // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

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
	public:

		/**
		 * This class implements a test data collection based on local files.
		 */
		class FileDataCollection : public TestDataCollection
		{
			public:

				/**
				 * Creates a new test data collection object.
				 * @param imageFilenames The filenames of all local images which will be part of the test collection
				 */
				explicit FileDataCollection(std::vector<std::string>&& imageFilenames);

				/**
				 * Returns the test data object associated with a specified index.
				 * @see TestDataCollection::data().
				 */
				SharedTestData data(const size_t index) override;

				/**
				 * Returns the number of data object objects this collection holds.
				 * @see TestDataCollection::size().
				 */
				size_t size() override;

			protected:

				/// The filenames of all local images belonging to this test collection.
				std::vector<std::string> filenames_;
		};

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

#ifdef OCEAN_USE_TEST_DATA_COLLECTION

		/**
		 * Tests real images in which exactly one bullseye is visible (in the center of the image).
		 * The images needs to be provided via a test data collection with name "messengercodedetector_1bullseye".
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetect1Bullseye(Worker& worker);

		/**
		 * Tests real images in which no Messenger code is visible.
		 * The images needs to be provided via a test data collection with name "messengercodedetector_0code".
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetect0Code(Worker& worker);

		/**
		 * Tests real images in which exactly one Messenger code is visible.
		 * The images needs to be provided via a test data collection with name "messengercodedetector_1code".
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetect1Code(Worker& worker);

#endif // OCEAN_USE_TEST_DATA_COLLECTION

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
};

#ifdef OCEAN_USE_TEST_DATA_COLLECTION

/**
 * Registers the data collections for the MessengerCodeDetector test.
 * @return The scoped subscriptions for the registered data collections
 */
TestDataManager::ScopedSubscriptions TestMessengerCodeDetector_registerTestDataCollections();

#endif // OCEAN_USE_LOCAL_TEST_DATA_COLLECTION

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_MESSENGER_CODE_DETECTOR_H
