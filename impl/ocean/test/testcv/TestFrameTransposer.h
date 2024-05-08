/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_TRANSPOSER_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_TRANSPOSER_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a test for the frame transposer class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameTransposer
{
	public:

		/**
		 * Tests the frame transposer functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the frame transposer function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 */
		static bool testTransposer(const double testDuration, Worker& worker);

		/**
		 * Tests the frame transposer function.
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char', 'float'
		 */
		template <typename T>
		static bool testTransposer(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the frame transposer function.
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char', 'float'
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool testTransposer(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the rotation by 90 degrees function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 */
		static bool testRotate90(const double testDuration, Worker& worker);

		/**
		 * Tests the rotation by 180 degrees function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 */
		static bool testRotate180(const double testDuration, Worker& worker);

		/**
		 * Tests the rotation by +/- 90 degree steps function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 */
		static bool testRotate(const double testDuration, Worker& worker);

	protected:

		/**
		 * Validates the frame transposer function.
		 * @param frame The frame to be transposed, must be valid
		 * @param transposed The transposed frame, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each row in the frame, with range [0, infinity)
		 * @param transposedPaddingElements The number of padding elements at the end of each row in the transposed frame, with range [0, infinity)
		 * @return True, if this test succeeded
		 * @tparam T The data type of each pixel element, e.g., 'unsigned char', 'float'
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static bool validateTransposer(const T* frame, const T* transposed, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int transposedPaddingElements);

		/**
		 * Validates the 90 degree rotation function.
		 * @param sourceFrame The source frame, must be valid
		 * @param targetFrame The by 90 degree rotated frame, must be valid
		 * @param clockwise True, if the image was rotate clockwise; False, if the image was rotated counter clockwise
		 * @return True, if succeeded
		 */
		static bool validateRotate90(const Frame& sourceFrame, const Frame& targetFrame, const bool clockwise);

		/**
		 * Validates the 180 degree rotation function.
		 * @param sourceFrame The source frame, must be valid
		 * @param targetFrame The by 180 degree rotated frame, must be valid
		 * @return True, if succeeded
		 */
		static bool validateRotate180(const Frame& sourceFrame, const Frame& targetFrame);

		/**
		 * Validates the rotation function.
		 * @param sourceFrame The source frame, must be valid
		 * @param targetFrame The by 180 degree rotated frame, must be valid
		 * @param angle The clockwise rotation angle, in degrees, with range (-infinity, infinity), must be a multiple of 90
		 * @return True, if succeeded
		 */
		static bool validateRotate(const Frame& sourceFrame, const Frame& targetFrame, const int angle);

		/**
		 * Invokes the template-based rotate function.
		 * @param sourceFrame The source frame to rotate, must be valid
		 * @param targetFrame The target frame receiving the rotated image content, must be valid
		 * @param angle The rotation angle to be used, must be multiple of +/- 90
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool rotate(const Frame& sourceFrame, Frame& targetFrame, const int angle, Worker* worker);

		/**
		 * Invokes the template-based rotate function.
		 * @param sourceFrame The source frame to rotate, must be valid
		 * @param targetFrame The target frame receiving the rotated image content, must be valid
		 * @param angle The rotation angle to be used, must be multiple of +/- 90
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 * @tparam T The data type of each element
		 */
		template <typename T>
		static bool rotate(const Frame& sourceFrame, Frame& targetFrame, const int angle, Worker* worker);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_TRANSPOSER_H
