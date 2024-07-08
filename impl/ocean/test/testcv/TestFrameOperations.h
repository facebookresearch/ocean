/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_OPERATIONS_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_OPERATIONS_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a frame operations test.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameOperations
{
	public:

		/**
		 * Tests all frame operation functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the subtraction of two frames.
		 * @param width The width of the test frame in pixels, with range [1, infinity)
		 * @param height The height of the test frame in pixels, with range [1, infinity)
		 * @param channels The number of frame channels, with range [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if so
		 * @tparam T The data type of the pixel channel values, either 'unsiged char' or 'float'
		 */
		template <typename T>
		static bool testSubtraction(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validation of frame subtraction
		 * @param source0 The first source frame, must be valid
		 * @param source1 The second source frame, must be valid
		 * @param target The target frame that will be validated, must be valid
		 * @return True if the validation has been successful, otherwise false
		 * @tparam T Type of the data being validated
		 */
		template <typename T>
		static bool validateSubtraction(const Frame& source0, const Frame& source1, const Frame& target);
};

} // namespace TestCV

} // namespace Test

} // Ocean

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_OPERATIONS_H
