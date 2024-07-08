/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MIN_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MIN_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameFilterMin.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements frame min filter tests.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterMin : protected CV::FrameFilterMin
{
	public:

		/**
		 * Tests the entire min filter using a given frame.
		 * @param width The width of the test frame in pixel, with range [51, infinity)
		 * @param height The height of the test frame in pixel, with range [51, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the media filter for arbitrary frames.
		 * @param width The width of the input frame in pixel, with range [51, infinity)
		 * @param height The height of the input frame in pixel, with range [51, infinity)
		 * @param channels The number of channels the input frame has, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the frame elements
		 */
		template <typename T>
		static bool testMin(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the media filter for arbitrary frames.
		 * @param width The width of the input frame in pixel, with range [51, infinity)
		 * @param height The height of the input frame in pixel, with range [51, infinity)
		 * @param channels The number of channels the input frame has, with range [1, infinity)
		 * @param filterSize The size of the filter edge in pixel, must be odd with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the frame elements
		 */
		template <typename T>
		static bool testMin(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int filterSize, const double testDuration, Worker& worker);

		/**
		 * Tests the media filter for arbitrary frames.
		 * @param width The width of the input frame in pixel, with range [51, infinity)
		 * @param height The height of the input frame in pixel, with range [51, infinity)
		 * @param channels The number of channels the input frame has, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the frame elements
		 */
		template <typename T>
		static bool testMinInPlace(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the media filter for arbitrary frames.
		 * @param width The width of the input frame in pixel, with range [51, infinity)
		 * @param height The height of the input frame in pixel, with range [51, infinity)
		 * @param channels The number of channels the input frame has, with range [1, infinity)
		 * @param filterSize The size of the filter edge in pixel, must be odd with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the frame elements
		 */
		template <typename T>
		static bool testMinInPlace(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int filterSize, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates the min filter for arbitrary frames.
		 * @param frame The frame to be filtered, must be valid
		 * @param result The filter result to verify, must be valid
		 * @param filterSize Size of the filter, with range [3, infinity), must be odd
		 * @return True, if succeeded
		 * @tparam T The data type of the elements
		 */
		template <typename T>
		static bool validateMin(const Frame& frame, const Frame& result, const unsigned int filterSize);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MIN_H
