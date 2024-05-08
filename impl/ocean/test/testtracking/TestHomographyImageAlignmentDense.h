/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTACKING_TEST_HOMOGRAPHY_IMAGE_ALIGNMENT_DENSE_H
#define META_OCEAN_TEST_TESTTACKING_TEST_HOMOGRAPHY_IMAGE_ALIGNMENT_DENSE_H

#include "ocean/test/testtracking/TestTracking.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

/**
 * This class implements a test for the dense homography image alignment.
 * @ingroup testtracking
 */
class OCEAN_TEST_TRACKING_EXPORT TestHomographyImageAlignmentDense
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the additive alignment.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testAdditive(const double testDuration, Worker& worker);

		/**
		 * Tests the additive alignment with specified number of image channels.
		 * @param channels The number of image channels to be used, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testAdditive(const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the inverse compositional alignment.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testInverseCompositional(const double testDuration, Worker& worker);

		/**
		 * Tests the inverse compositional alignment with specified number of image channels.
		 * @param channels The number of image channels to be used, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testInverseCompositional(const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the multi-resolution alignment.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testMultiResolution(const double testDuration, Worker& worker);

		/**
		 * Tests the multi-resolution alignment with specified number of image channels.
		 * @param channels The number of image channels to be used, with range [1, infinity)
		 * @param additive True, to use additive alignment; False, to use inverse compositional alignment
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testMultiResolution(const unsigned int channels, const bool additive, const double testDuration, Worker& worker);

	protected:

		/**
		 * Creates a random template image, and random homography and a random tracking image.
		 * @param frameType The frame type to be used for the template frame and the tracking frame, must be valid
		 * @param randomGenerator The random generator to be used
		 * @param templateFrame The resulting template image
		 * @param trackingFrame The resulting tracking image
		 * @param tracking_H_template The resulting homography between template frame and tracking frame
		 * @param maximalHomographyRadius The maximal distance between the ideal image corners and the random homography corners, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool createRandomData(const FrameType& frameType, RandomGenerator& randomGenerator, Frame& templateFrame, Frame& trackingFrame, SquareMatrix3& tracking_H_template, const Scalar maximalHomographyRadius);

		/**
		 * Determines the average pixel intensity differences between the template image, the tracking image, and the adjusted tracking image.
		 * @param templateFrame The template frame to be used
		 * @param trackingFrame The tracking frame to be used
		 * @param estimatedTracking_H_template The estimation transformation between the template frame and the tracking frame
		 * @param averageError The resulting average pixel intensity between the template image and the tracking image (the initial error), with range [0, infinity)
		 * @param averageErrorEstimated The resulting average pixel intensity between the warped template image and the tracking image (based on the estimated homography - the final error), with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool determineError(const Frame& templateFrame, const Frame& trackingFrame, const SquareMatrix3& estimatedTracking_H_template, double& averageError, double& averageErrorEstimated);
};

}

}

}

#endif // META_OCEAN_TEST_TESTTACKING_TEST_HOMOGRAPHY_IMAGE_ALIGNMENT_DENSE_H
