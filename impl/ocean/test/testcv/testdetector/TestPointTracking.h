/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_POINT_TRACKING_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_POINT_TRACKING_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements a point tracking test.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestPointTracking
{
	public:

		/**
		 * Tests entire point tracking functionality.
		 * @param frame Test frame to be used for point detection and tracking, with frame dimension [80, infinity)x[80, infinity), must be valid
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const Frame& frame, const double testDuration, Worker& worker);

	private:

		/**
		 * Test point tracking based on Harris corner detection and point motion.
		 * @param frame Original frame to be used for feature detection
		 * @param channels The number of frame channels to be used for testing, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testMotion(const Frame& frame, const unsigned int channels, const double testDuration, Worker & worker);

		/**
		 * Determines the accuracy of point correspondences based on the ground truth transformation.
		 * @param points0 The first set of image points, at least one
		 * @param points1 The second set of image points, one for each image point in the first set
		 * @param frame0_H_frame1 The transformation transforming points from the second frame to points int he first frame, must be valid
		 * @param maxDistance Maximal distance between the determined point and the correct ground truth point so that a correspondence counts as valid, in pixel, with range [0, infinity)
		 * @return The percentage of valid correspondences, with range [0, 1]
		 */
		static double validateAccuracy(const Vectors2& points0, const Vectors2& points1, const SquareMatrix3& frame0_H_frame1, const Scalar maxDistance = Scalar(2));
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_POINT_TRACKING_H
