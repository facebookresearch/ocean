/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_SUM_SQUARE_DIFFERENCES_NO_CENTER_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_SUM_SQUARE_DIFFERENCES_NO_CENTER_H

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

/**
 * This class implements sum square differences omitting the center pixel tests.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestSumSquareDifferencesNoCenter
{
	protected:

		/**
		 * Definition of individual types of implementation.
		 */
		enum ImplementationType : uint32_t
		{
			/// The naive implementation.
			IT_NAIVE,
			/// The template-based implementation.
			IT_TEMPLATE,
			/// The SSE-based implementation.
			IT_SSE,
			/// The NEON-based implementation.
			IT_NEON,
			/// The default implementation (which is actually used by default).
			IT_DEFAULT
		};

	public:

		/**
		 * Invokes all test functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the sum square differences function for two pixel accurate patches in two images.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatch8BitPerChannel(const double testDuration);

	private:

		/**
		 * Tests the sum square differences function for two pixel accurate patches in two images.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static bool testPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_SUM_SQUARE_DIFFERENCES_NO_CENTER_H
