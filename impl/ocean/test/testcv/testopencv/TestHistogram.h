/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_HISTOGRAM_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_HISTOGRAM_H

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

/**
 * This class implements performance and validation tests for image histogram, histogram equalization, etc.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestHistogram
{
	public:

		/**
		 * Test histogram functions
		 * @param testDuration Number of seconds for each subtest, range: (0, infinity)
		 * @return True if all subtests finished successfully, otherwise false
		 */
		static bool test(const double testDuration);

		/**
		 * Test Ocean's implementation of Contrast-Limited Adaptive Histogram Equalization (CLAHE)
		 * @param testDuration Number of of seconds for each subtest, range: (0, infinity)
		 * @return True if all subtests finished successfully, otherwise false
		 */
		static bool testCLAHE(const double testDuration);
};

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_HISTOGRAM_H
