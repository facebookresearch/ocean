/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_ORB_DETECTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_ORB_DETECTOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/cv/detector/ORBFeatureDescriptor.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements a ORB detector test.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestORBDetector
{
	public:

		/**
		 * Tests all ORB descriptor functions.
		 * @param frame Test frame to be used for feature detection, invalid to use a random image
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const Frame& frame, const double testDuration, Worker& worker);

		/**
		 * Tests the determination of the orientation of all given features.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param yFrameTest Optional explicit frame to be used for testing, otherwise a random image will be used
		 * @return True, if succeeded
		 */
		static bool testOrientationDetermination(const double testDuration, Worker& worker, const Frame& yFrameTest = Frame());

		/**
		 * Tests the determination of the descriptor of all given features.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param yFrameTest Optional explicit frame to be used for testing, otherwise a random image will be used
		 * @return True, if succeeded
		 */
		static bool testDescriptorDetermination(const double testDuration, Worker& worker, const Frame& yFrameTest = Frame());

		/**
		 * Tests the detection and description of reference features.
		 * This test is mainly a stress test.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetectReferenceFeaturesAndDetermineDescriptors(const double testDuration, Worker& worker);

		/**
		 * Tests the determination of the hamming distance between two randomized descriptors.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHammingDistanceDetermination(const double testDuration);

		/**
		 * Tests the matching of randomized descriptors
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDescriptorMatching(const double testDuration, Worker& worker);

	protected:

		/**
		 * Validate the calculation of the orientation.
		 * @param linedIntegralFrame Pointer to the (lined) integral frame of the actual 8 bit frame for which the feature descriptors will be calculated, the integral frame has an extra line with zero pixels on the left and top of the integral frame
		 * @param width The width of the original frame in pixel (not the width of the lined-integral frame), with range [1, infinity)
		 * @param height The height of the original frame in pixel (not the height of the lined-integral frame), with range [1, infinity)
		 * @param features The features to validate
		 * @return The success percentage, with range [0, 1]
		 */
		static double validateOrientation(const uint32_t* linedIntegralFrame, const unsigned int width, const unsigned int height, CV::Detector::ORBFeatures& features);

		/**
		 * Validate the calculation of the descriptor.
		 * @param linedIntegralFrame Pointer to the (lined) integral frame of the actual 8 bit frame for which the feature descriptors will be calculated, the integral frame has an extra line with zero pixels on the left and top of the integral frame
		 * @param width The width of the original frame in pixel (not the width of the lined-integral frame), with range [1, infinity)
		 * @param height The height of the original frame in pixel (not the height of the lined-integral frame), with range [1, infinity)
		 * @param features The features to validate
		 * @return True, if succeeded
		 */
		static bool validateDescriptors(const uint32_t* linedIntegralFrame, const unsigned int width, const unsigned int height, CV::Detector::ORBFeatures& features);

		/**
		 * Validate the calculation of the hamming distance.
		 * @param descriptors1 First descriptor to validate
		 * @param descriptors2 Second descriptor to validate
		 * @return True, if succeeded
		 */
		static bool validateHammingDistance(const std::vector<CV::Detector::ORBDescriptor>& descriptors1, const std::vector<CV::Detector::ORBDescriptor>& descriptors2);

		/**
		 * Validate the descriptor matching.
		 * @param matches Calculated indices of matches to check the validation for
		 * @param checkMatches Pre calculated correct indices of matches
		 * @return True, if succeeded
		 */
		static bool validateDescriptorMatching(const IndexPairs32& matches, const IndexPairs32& checkMatches);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_ORB_DETECTOR_H
