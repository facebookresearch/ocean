/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FEATURE_DETECTORS_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FEATURE_DETECTORS_H

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
 * This class tests the performance of several OpenCV feature detectors.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestFeatureDetectors
{
	public:

		/**
		 * Tests all functions of relevant feature detectors in OpenCV.
		 * @param frame The image that is used for the test, must be valid
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFeatureDetectors(const cv::Mat& frame, const double testDuration);

	private:

		/**
		 * Tests the OpenCV Harris corner detector.
		 * @param testDuration  Number of seconds for each test, with range (0, infinity)
		 * @param frame The image that is used for the test, must be valid
		 * @param cornersCount  Number of corners to extract (Harris corner threshold is determined internally) (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testHarrisCornerDetector(const cv::Mat& frame, const double testDuration, size_t cornersCount);

		/**
		 * Tests the OpenCV FAST feature detector.
		 * @param testDuration  Number of seconds for each test, with range (0, infinity)
		 * @param frame The image that is used for the test, must be valid
		 * @param cornersCount  Number of keypoints to extract (threshold is determined internally) (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFASTFeatureDectector(const cv::Mat& frame, const double testDuration, size_t cornersCount);

		/**
		 * Tests the OpenCV SURF feature detector.
		 * @param testDuration  Number of seconds for each test, with range (0, infinity)
		 * @param frame The image that is used for the test, must be valid
		 * @param cornersCount  Number of keypoints to extract (threshold is determined internally) (0, infinity)
		 * @param computeDescriptors In addition to the detection of feature points, also compute their corresponding descriptors (default: true)
		 * @return True, if succeeded
		 */
		static bool testSURFFeatureDectector(const cv::Mat& frame, const double testDuration, size_t cornersCount, bool computeDescriptors = true);
};

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FEATURE_DETECTORS_H
