/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_POINT_TRACKING_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_POINT_TRACKING_H

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

#include <opencv2/core.hpp>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{


/**
 * This class tests the performance of the different patch tracking functions in OpenCV.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestPointTracking
{
	public:

		/**
		 * Test patch tracking in OpenCV with random image.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPointTracking(const double testDuration);

		/**
		 * Test patch tracking in OpenCV with provided image.
		 * @param frame Image that is used for the test, must be valid
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPointTracking(const cv::Mat& frame, const double testDuration);

	private:

		/**
		 * Test sparse optical flow in OpenCV with random image
		 * @param width The width of the random image to be tested
		 * @param height The height of the random image to be tested
		 * @param window Size of the search window in each direction
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSparseOpticalFlow(int width, int height, int window, const double testDuration);

		/**
		 * Test sparse optical flow in OpenCV with provided image.
		 * @param frame Image that is used for the test, must be valid
		 * @param window Size of the search window in each direction
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSparseOpticalFlow(const cv::Mat& frame, int window, const double testDuration);

		/**
		 * Test template matching in OpenCV with provided image.
		 * @param frame Image that is used for the test, must be valid
		 * @return True, if succeeded
		 */
		static bool testTemplateMatching(const cv::Mat& frame);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_POINT_TRACKING_H
