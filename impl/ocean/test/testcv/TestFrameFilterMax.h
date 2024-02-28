// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MAX_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MAX_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameFilterMax.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements frame max filter tests.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilterMax : protected CV::FrameFilterMax
{
	public:

		/**
		 * Tests the entire max filter using a given frame.
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
		static bool testMax(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

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
		static bool testMax(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int filterSize, const double testDuration, Worker& worker);

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
		static bool testMaxInPlace(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

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
		static bool testMaxInPlace(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int filterSize, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates the max filter for arbitrary frames.
		 * @param frame The frame to be filtered, must be valid
		 * @param result The filter result to verify, must be valid
		 * @param filterSize Size of the filter, with range [3, infinity), must be odd
		 * @return True, if succeeded
		 * @tparam T The data type of the elements
		 */
		template <typename T>
		static bool validateMax(const Frame& frame, const Frame& result, const unsigned int filterSize);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_MAX_H
