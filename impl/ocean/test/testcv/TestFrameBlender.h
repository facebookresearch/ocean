/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_BLENDER_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_BLENDER_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a test for the frame blender class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameBlender
{
	public:

		/**
		 * Tests the frame blender functions.
		 * @param testDuration Test duration in seconds, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the blending two images with constant alpha value.
		 * @param testDuration Test duration in seconds, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 */
		static bool testConstantAlpha(const double testDuration, Worker& worker);

		/**
		 * Tests the blending two images with a separate alpha channel.
		 * @param testDuration Test duration in seconds, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value; False, if 0x00 is interpreted as fully transparent
		 */
		template <bool tTransparentIs0xFF>
		static bool testSeparateAlphaChannel(const double testDuration, Worker& worker);

		/**
		 * Tests the blend function with alpha channel at the front or at the back.
		 * @param testDuration Test duration in seconds, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value; False, if 0x00 is interpreted as fully transparent
		 */
		template <bool tTransparentIs0xFF>
		static bool testBlend(const double testDuration, Worker& worker);

		/**
		 * Tests the blend function with constant value.
		 * @param testDuration Test duration in seconds, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value; False, if 0x00 is interpreted as fully transparent
		 */
		template <bool tTransparentIs0xFF>
		static bool testBlendWithConstantValue(const double testDuration, Worker& worker);

	protected:

		/**
		 * Tests the blending two images with a separate alpha channel.
		 * @param testDuration Test duration in seconds, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value; False, if 0x00 is interpreted as fully transparent
		 */
		template <bool tTransparentIs0xFF>
		static bool testSeparateAlphaChannelSubFrame(const double testDuration, Worker& worker);

		/**
		 * Tests the blending two images with a separate alpha channel.
		 * @param testDuration Test duration in seconds, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value; False, if 0x00 is interpreted as fully transparent
		 */
		template <bool tTransparentIs0xFF>
		static bool testSeparateAlphaChannelFullFrame(const double testDuration, Worker& worker);

		/**
		 * Tests the sub-region transparent blend function.
		 * @param sourcePixelFormat Source pixel format
		 * @param targetPixelFormat Target pixel format
		 * @param targetWidth Width of the target test frame in pixel
		 * @param targetHeight Height of the target test frame in pixel
		 * @param testDuration Test duration in seconds, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value; False, if 0x00 is interpreted as fully transparent
		 */
		template <bool tTransparentIs0xFF>
		static bool testSubFrame(const FrameType::PixelFormat sourcePixelFormat, const FrameType::PixelFormat targetPixelFormat, const unsigned int targetWidth, const unsigned int targetHeight, const double testDuration, Worker& worker);

		/**
		 * Tests the transparent blend function.
		 * @param sourcePixelFormat Source pixel format
		 * @param targetPixelFormat Target pixel format
		 * @param width The width of the test frame in pixel
		 * @param height The height of the test frame in pixel
		 * @param testDuration Test duration in seconds, with range (0, infinity)
		 * @param worker The worker object to distribute the computation
		 * @return True, if this test succeeded
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value; False, if 0x00 is interpreted as fully transparent
		 */
		template <bool tTransparentIs0xFF>
		static bool testFullFrame(const FrameType::PixelFormat sourcePixelFormat, const FrameType::PixelFormat targetPixelFormat, const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Validates the blend function with alpha channel.
		 * @param sourceWithAlpha The source frame with alpha channel, must be valid
		 * @param target The target frame, must be valid
		 * @param blendResult The blend result, must be valid
		 * @param sourceLeft Horizontal start position inside the source frame, with range [0, sourceWithAlpha.width() - 1]
		 * @param sourceTop Vertical start position inside the source frame, with range [0, sourceWithAlpha.height() - 1]
		 * @param targetLeft Horizontal start position inside the target frame, with range [0, target.width() - 1]
		 * @param targetTop Vertical start position inside the target frame, with range [0, target.height() - 1]
		 * @param width The width of the blending sub-region in pixel, with range [1, min(sourceWithAlpha.width() - sourceLeft, target.width() - targetLeft)]
		 * @param height The height of the blending sub-region in pixel, with range [1, min(sourceWithAlpha.height() - sourceTop, target.height() - targetTop)]
		 * @return True, if succeeded
		 * @tparam tTransparentIs0xFF True, if 0xFF is interpreted as fully transparent value; False, if 0x00 is interpreted as fully transparent
		 */
		template <bool tTransparentIs0xFF>
		static bool validateBlendResult(const Frame& sourceWithAlpha, const Frame& target, const Frame& blendResult, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_BLENDER_H
