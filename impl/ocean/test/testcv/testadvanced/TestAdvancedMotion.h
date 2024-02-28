// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_MOTION_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_MOTION_H

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

#include "ocean/base/RandomGenerator.h"
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
 * This class implements tests for AdvancedMotion.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestAdvancedMotion
{
	public:

		/**
		 * Invokes all tests for AdvancedMotion.
		 * @param width The width of the test frame in pixel, with range [5, infinity)
		 * @param height The height of the test frame in pixel, with range [5, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the stress test of trackPointsBidirectionalSubPixelMirroredBorder().
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool stressTestTrackPointsBidirectionalSubPixelMirroredBorder(const double testDuration, Worker& worker);

		/**
		 * Invokes the stress test of trackPointsBidirectionalSubPixelMirroredBorder().
		 * @param randomGenerator Random number generator to use for test
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the motion class to be tested, either 'AdvancedMotionSSD' or 'AdvancedMotionZeroMeanSSD'
		 */
		template <typename T>
		static bool stressTestTrackPointsBidirectionalSubPixelMirroredBorder(RandomGenerator& randomGenerator, Worker& worker);

		/**
		 * Invokes the stress test of trackPointsBidirectionalSubPixelMirroredBorder().
		 * @param randomGenerator Random number generator to use for test
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the motion class to be tested, either 'AdvancedMotionSSD' or 'AdvancedMotionZeroMeanSSD'
		 * @tparam tPatchSize The size of the image patch to be used, with range [1, infinity), must be odd
		 */
		template <typename T, unsigned int tPatchSize>
		static bool stressTestTrackPointsBidirectionalSubPixelMirroredBorder(RandomGenerator& randomGenerator, Worker& worker);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_MOTION_H
