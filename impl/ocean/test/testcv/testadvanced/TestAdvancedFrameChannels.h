/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_CHANNELS_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_CHANNELS_H

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

/**
 * This class implements tests for AdvancedFrameChannels.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestAdvancedFrameChannels
{
	public:

		/**
		 * Invokes all tests for AdvancedFrameChannels.
		 * @param width The width of the test frame in pixel, with range [5, infinity)
		 * @param height The height of the test frame in pixel, with range [5, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests frame separation to one channel function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSeparateTo1Channel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests frame zip channels function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testZipChannels(const unsigned int width, const unsigned int height, const double testDuration);

	protected:

		/**
		 * Tests frame separation to one channel function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the test frame has, with range [2, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam TSource The data type of the source elements
		 * @tparam TTarget The data type of the target elements
		 * @tparam tSourceFactor The multiplication factor each source element will be multiplied with, with range (-infinity, infinity)/{0}
		 * @tparam tTargetFactor The multiplication factor each target element will be multiplied with, with range (-infinity, infinity)/{0}
		 */
		template <typename TSource, typename TTarget>
		static bool testSeparateTo1Channel(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);

		/**
		 * Tests zip channels function.
		 * @param width The width of the test frame in pixel, with range [1, infinity)
		 * @param height The height of the test frame in pixel, with range [1, infinity)
		 * @param channels The number of channels the test frame has, with range [2, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam TSource The data type of the source elements
		 * @tparam TTarget The data type of the target elements
		 */
		template <typename TSource, typename TTarget>
		static bool testZipChannels(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_CHANNELS_H
