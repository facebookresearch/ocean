/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointSubPixelMirroredBorder().
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testTrackPointSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointSubPixelMirroredBorder() with specified metric type.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the motion class to be tested, either 'AdvancedMotionSSD' or 'AdvancedMotionZeroMeanSSD'
		 */
		template <typename T>
		static bool testTrackPointSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointSubPixelMirroredBorder() with specified metric type and channel number.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the motion class to be tested, either 'AdvancedMotionSSD' or 'AdvancedMotionZeroMeanSSD'
		 * @tparam tChannels The number of channels the frame has, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool testTrackPointSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointSubPixelMirroredBorder() with specified metric type, channel number, and patch size.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the motion class to be tested, either 'AdvancedMotionSSD' or 'AdvancedMotionZeroMeanSSD'
		 * @tparam tChannels The number of channels the frame has, with range [1, infinity)
		 * @tparam tPatchSize The size of the image patch to be used, with range [1, infinity), must be odd
		 */
		template <typename T, unsigned int tChannels, unsigned int tPatchSize>
		static bool testTrackPointSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointsSubPixelMirroredBorder().
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testTrackPointsSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointsSubPixelMirroredBorder() with specified metric type.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the motion class to be tested, either 'AdvancedMotionSSD' or 'AdvancedMotionZeroMeanSSD'
		 */
		template <typename T>
		static bool testTrackPointsSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointsSubPixelMirroredBorder() with specified metric type and channel number.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the motion class to be tested, either 'AdvancedMotionSSD' or 'AdvancedMotionZeroMeanSSD'
		 * @tparam tChannels The number of channels the frame has, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool testTrackPointsSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointsSubPixelMirroredBorder() with specified metric type, channel number, and patch size.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the motion class to be tested, either 'AdvancedMotionSSD' or 'AdvancedMotionZeroMeanSSD'
		 * @tparam tChannels The number of channels the frame has, with range [1, infinity)
		 * @tparam tPatchSize The size of the image patch to be used, with range [1, infinity), must be odd
		 */
		template <typename T, unsigned int tChannels, unsigned int tPatchSize>
		static bool testTrackPointsSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointsBidirectionalSubPixelMirroredBorder().
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testTrackPointsBidirectionalSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointsBidirectionalSubPixelMirroredBorder() with specified metric type.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the motion class to be tested, either 'AdvancedMotionSSD' or 'AdvancedMotionZeroMeanSSD'
		 */
		template <typename T>
		static bool testTrackPointsBidirectionalSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointsBidirectionalSubPixelMirroredBorder() with specified metric type and channel number.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the motion class to be tested, either 'AdvancedMotionSSD' or 'AdvancedMotionZeroMeanSSD'
		 * @tparam tChannels The number of channels the frame has, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool testTrackPointsBidirectionalSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Invokes the test of trackPointsBidirectionalSubPixelMirroredBorder() with specified metric type, channel number, and patch size.
		 * @param width The width of the test frame in pixel, with range [32, infinity)
		 * @param height The height of the test frame in pixel, with range [32, infinity)
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam T The data type of the motion class to be tested, either 'AdvancedMotionSSD' or 'AdvancedMotionZeroMeanSSD'
		 * @tparam tChannels The number of channels the frame has, with range [1, infinity)
		 * @tparam tPatchSize The size of the image patch to be used, with range [1, infinity), must be odd
		 */
		template <typename T, unsigned int tChannels, unsigned int tPatchSize>
		static bool testTrackPointsBidirectionalSubPixelMirroredBorder(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

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

	protected:

		/**
		 * Creates a random image which is suitable for patch tracking.
		 * @param frameType The frame type of the resulting image, must be valid
		 * @param randomGenerator The random generator to be used
		 * @return The resulting frame
		 */
		static Frame createRandomTrackableFrame(const FrameType& frameType, RandomGenerator& randomGenerator);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_TEST_ADVANCED_MOTION_H
