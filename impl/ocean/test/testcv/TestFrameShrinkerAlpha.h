/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_SHRINKER_ALPHA_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_SHRINKER_ALPHA_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the frame downsizing functions with alpha channel.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameShrinkerAlpha
{
	public:

		/**
		 * Tests the frame downsize.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the frame downsize function bisecting the frame size.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testFrameDivideByTwo(const double testDuration, Worker& worker);

	protected:

		/**
		 * Tests the frame downsize function bisecting the frame size.
		 * @param width The width of the source frame in pixel, with range [2, infinity)
		 * @param height The height of the source frame in pixel, with range [2, infinity)
		 * @param channels Number of frame channels (including the alpha channel), with range [1, infinity)
		 * @param alphaAtFront True, if the alpha channel is in front of the data channels
		 * @param transparentIs0xFF True, if 0xFF is interpreted as fully transparent pixel
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computational load
		 * @return True, if succeeded
		 */
		static bool testFrameDivideByTwo(const unsigned int width, const unsigned int height, const unsigned int channels, const bool alphaAtFront, const bool transparentIs0xFF, const double testDuration, Worker& worker);

		/**
		 * Invokes the frame downsize function bisecting the frame size.
		 * @param source The source frame, must be valid
		 * @param target The target frame with half dimension, must be valid
		 * @param alphaAtFront True, if the alpha channel is in front of the data channels
		 * @param transparentIs0xFF True, if 0xFF is interpreted as fully transparent pixel
		 * @param worker Optional worker object to distribute the computational load
		 */
		static void invokeFrameDivideByTwo(const Frame& source, Frame& target, const bool alphaAtFront, const bool transparentIs0xFF, Worker* worker);

		/**
		 * Tests the validation of the downsampling function.
		 * @param source The source frame, must be valid
		 * @param target The target frame with half dimension, must be valid
		 * @param alphaAtFront True, if the alpha channel is in front of the data channels
		 * @param transparentIs0xFF True, if 0xFF is interpreted as fully transparent pixel
		 * @return True, if succeeded
		 */
		static bool validationDivideByTwo(const Frame& source, const Frame& target, const bool alphaAtFront, const bool transparentIs0xFF);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_SHRINKER_ALPHA_H
