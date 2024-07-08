/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_SHRINKER_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_SHRINKER_H

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
 * This class implements an advanced frame shrinker test.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestAdvancedFrameShrinker
{
	public:

		/**
		 * Tests all advanced frame shrinker functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the advanced frame shrinker.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testDivideByTwo(const double testDuration, Worker& worker);

		/**
		 * Tests the advanced frame shrinker for a specific frame type.
		 * @param width The width of the test frame in pixel
		 * @param height The height of the test frame in pixel
		 * @param handleMask True, to handle also frame pixels corresponding to four mask pixels
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testDivideByTwo(const unsigned int width, const unsigned int height, const bool handleMask, const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates the advanced frame shrinker for a given frame.
		 * @param sourceFrame The source frame, must be valid
		 * @param sourceMask The mask associated with the source frame, must be valid
		 * @param targetFrame The resulting target frame, must be valid
		 * @param targetMask The resulting target mask, must be valid
		 * @param handleMaskPixels True, to handle also pixels entirely associated with mask pixels; False, to skip these pixels
		 * @param maskValue The pixel value associated with a mask pixel belonging to the mask, with range [0, 255]
		 * @param nonMaskValue The pixel value associated with a mask pixel not belonging to the mask, with range [0, 255]
		 * @return True, if succeeded
		 */
		static bool validateDivideByTwo(const Frame& sourceFrame, const Frame& sourceMask, const Frame& targetFrame, const Frame& targetMask, const bool handleMaskPixels, const uint8_t maskValue = 0x00u, const uint8_t nonMaskValue = 0xFFu);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_FRAME_SHRINKER_H
