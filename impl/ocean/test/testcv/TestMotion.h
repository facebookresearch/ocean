/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_MOTION_H
#define META_OCEAN_TEST_TESTCV_TEST_MOTION_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class test the motion functions.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestMotion
{
	public:

		/**
		 * Tests the entire motion functionality.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to be used for computation distribution
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the simple motion calculation for two images.
		 * Pixels inside the blocks mapping outside the frames are mirrored back into the frame.<br>
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @tparam tChannels The number of data channels each frame has
		 * @return True, if succeeded
		 */
		template <unsigned int tChannels>
		static bool testMotionMirroredBorder(const double testDuration);

		/**
		 * Tests the simple motion calculation for two images.
		 * Pixels inside the blocks mapping outside the frames are mirrored back into the frame.<br>
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @tparam tChannels The number of data channels each frame has
		 * @tparam tSize The block size of the image patch that is applied for measurements, with range [1, infinity)
		 * @return True, if succeeded
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static bool testMotionMirroredBorder(const double testDuration);

	protected:

		/**
		 * Tests the simple motion calculation for two images.
		 * Pixels inside the blocks mapping outside the frames are mirrored back into the frame.<br>
		 * @param width0 Width of the first test frame in pixel
		 * @param height0 Height of the first test frame in pixel
		 * @param width1 Width of the second test frame in pixel
		 * @param height1 Height of the second test frame in pixel
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @tparam TMetric The metric that is applied for measurements
		 * @tparam tChannels The number of data channels each frame has
		 * @tparam tSize The block size of the image patch that is applied for measurements, with range [1, infinity)
		 * @return True, if succeeded
		 */
		template <typename TMetric, unsigned int tChannels, unsigned int tSize>
		static bool testMotionMirroredBorder(const unsigned int width0, const unsigned int height0, const unsigned int width1, const unsigned int height1, const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_MOTION_H
