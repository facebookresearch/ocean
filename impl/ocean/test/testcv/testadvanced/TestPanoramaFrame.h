/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTADVANCED_PANORAMA_FRAME_H
#define META_OCEAN_TEST_TESTCV_TESTADVANCED_PANORAMA_FRAME_H

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

#include "ocean/cv/advanced/PanoramaFrame.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

/**
 * This class implements a test for the panorama frame.
 * @ingroup testcvadvanced
 */
class OCEAN_TEST_CV_ADVANCED_EXPORT TestPanoramaFrame : protected CV::Advanced::PanoramaFrame
{
	public:

		/**
		 * Tests the panorama frame functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the conversion between frames with different orientations with all possible function parameters.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testCameraFrame2cameraFrame(const double testDuration, Worker& worker);

		/**
		 * Tests the conversion between frames with different orientations with specific function parameters.
		 * @param performanceWidth The image width that should be used to measure run-time performance, range: [1, infinity)
		 * @param performanceHeight The image height that should be used to measure run-time performance, range: [1, infinity)
		 * @param useApproximation True, will run tests on the approximated version of the test function, otherwise it will the pixel-exact one
		 * @param useSourceMask True, it will run tests with masks for the source images, otherwise it will run them without them
		 * @param numberChannels The number of channels that the source images that are used for testing will have, range: [1, 4]
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testCameraFrame2cameraFrame(const unsigned int performanceWidth, const unsigned int performanceHeight, const bool useApproximation, const bool useSourceMask, const unsigned int numberChannels, const double testDuration, Worker& worker);

		/**
		 * Tests the cameraFrame2panoramaSubFrame function and the reverse operation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testCameraFrame2panoramaSubFrame(const double testDuration, Worker& worker);

		/**
		 * Tests the re-creation of a panorama frame.
		 * @param worker Optional Worker object
		 * @return True, if succeeded
		 */
		static bool testRecreation(Worker& worker);

	protected:

		/**
		 * Tests the re-creation of a panorama frame.
		 * @param channels The number of frame channels to be used, with range [1, infinity)
		 * @param approximate True, to apply an approximation using a lookup table
		 * @param worker Optional Worker object
		 * @return True, if succeeded
		 */
		static bool testRecreation(const unsigned int channels, const bool approximate, Worker& worker);

		/**
		 * Determines the error between two frames.
		 * @param frameA The first frame to be used, must be valid
		 * @param frameB The second frame to be used, with same frame type as frameA, must be valid
		 * @return The average intensity error
		 */
		static double averageFrameError(const Frame& frameA, const Frame& frameB);

		/**
		 * Validates conversion between frames with different orientations for specific function parameters.
		 * @param sourceCamera The camera profile of the given source frame, must be valid,
		 * @param world_R_source The orientation of the given source frame and camera. Must be orthonormal.
		 * @param source The source frame that will be converted, the frame dimension must match with the given source camera profile, must have a single plane with no more than 4 channels (8 bit), must be valid
		 * @param sourceMask An optional mask defining valid and invalid pixels in the source frame, passing an invalid frame indicates that all pixels in the source frame are valid, otherwise it must be of the same size the source frame and have 1 channel (8 bit) and 1 plane
		 * @param targetCamera The camera profile of the resulting target frame
		 * @param world_R_target The orientation of the resulting target frame, must be orthonormal
		 * @param testTarget The target frame that will be validated, the frame dimension must match with the given source camera profile, must have a single plane with no more than 4 channels, must be valid
		 * @param testTargetMask The target mask of the test target frame that will be validated, defines valid and invalid pixels in the target frame, must be of the same size the target frame and have 1 channel (8 bit) and 1 plane
		 * @param maskValue The mask value defining valid pixels in the mask frames, valid values: {0, 255}
		 * @param approximationBinSize The approximated bin size to be used, with range [1, infinity)
		 * @return True, if succeeded
		 */
		static bool validateCameraFrame2cameraFrame(const PinholeCamera& sourceCamera, const SquareMatrix3& world_R_source, const Frame& source, const Frame& sourceMask, const PinholeCamera& targetCamera, const SquareMatrix3& world_R_target, const Frame& testTarget, const Frame& testTargetMask, const uint8_t maskValue, const unsigned int approximationBinSize);

};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTADVANCED_PANORAMA_FRAME_H
