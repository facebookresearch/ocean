/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_CREATOR_I_1_H
#define META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_CREATOR_I_1_H

#include "ocean/test/testcv/testsynthesis/TestCVSynthesis.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSynthesis
{

/**
 * This class implements a test for creators with one frame.
 * @ingroup testcvsynthesis
 */
class OCEAN_TEST_CV_SYNTHESIS_EXPORT TestCreatorI1
{
	public:

		/**
		 * Invokes all test functions.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the inpainting content creator.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testInpaintingContent(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the inpainting content creator with a specified channel number.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param channels The number of frame channels which will be used during the test, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testInpaintingContent(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration, Worker& worker);

		/**
		 * Tests the spatial cost information creator.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testInformationSpatialCost(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood cost information creator.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testInformationCost4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the 4-neighborhood cost information creator with a specified channel number.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels which will be used during the test, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testInformationCost4Neighborhood(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTSYNTHESIS_TEST_CREATOR_I_1_H
