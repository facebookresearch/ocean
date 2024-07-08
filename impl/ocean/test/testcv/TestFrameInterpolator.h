/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Utilities.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameInterpolator.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a test for the FrameInterpolator class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameInterpolator
{
	public:

		/**
		 * Invokes all test of the FrameInterpolator class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the resize function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool testResize(const double testDuration, Worker& worker);

		/**
		 * Tests the resize function.
		 * @param sourceWidth The width of the source frame in pixel, with range [4, infinity)
		 * @param sourceHeight The height of the source frame in pixel, with range [4, infinity)
		 * @param targetWidth The width of the target frame in pixel, with range [4, infinity)
		 * @param targetHeight The height of the target frame in pixel, with range [4, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if succeeded
		 */
		template <unsigned int tChannels, CV::FrameInterpolator::ResizeMethod tResizeMethod>
		static bool testResize(const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration, Worker& worker);

		/**
		 * Tests the resize function with focus on production use case.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testResizeUseCase(const double testDuration);

		/**
		 * Tests the resize function with focus on production use case.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param workerThreads The number of worker threads to be used, with range [1, 64]
		 * @return True, if succeeded
		 */
		static bool testResizeUseCase(const double testDuration, const unsigned int workerThreads);

	protected:

		/**
		 * Validates a resize frame.
		 * @param source The source frame that has been resized, must be valid
		 * @param target The resized target frame, must be valid
		 * @param resizeMethod The method that has been used to resize the frame
		 * @return True, if the target frame is correct
		 */
		static bool validateResizedFrame(const Frame& source, const Frame& target, const CV::FrameInterpolator::ResizeMethod resizeMethod);

		/**
		 * Resizes a plane.
		 * @param sourcePlane The source plane to resize, must be valid
		 * @param targetPlane The resized target plane, must be valid
		 * @param resizeMethod The method that has been used to resize the frame
		 * @return True, if the target frame is correct
		 * @tparam tPlaneChannels The number of channels the plane has, with range [1, infinity)
		 */
		template <unsigned int tPlaneChannels>
		static bool resizePlane(const Frame& sourcePlane, Frame& targetPlane, const CV::FrameInterpolator::ResizeMethod resizeMethod);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_H
