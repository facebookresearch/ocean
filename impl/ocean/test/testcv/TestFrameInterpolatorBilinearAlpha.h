/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BILINEAR_ALPHA_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BILINEAR_ALPHA_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Worker.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a bilinear frame interpolator test for frames holding an alpha channel.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameInterpolatorBilinearAlpha
{
	public:

		/**
		 * Tests all bilinear interpolation filter functions.
		 * @param width The width of the source frame in pixel, with range [1, infinity)
		 * @param height The height of the source frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker);

		/**
		 * Tests the bilinear pixel interpolation function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolatePixel8BitPerChannel(const double testDuration);

		/**
		 * Tests the bilinear pixel interpolation function.
		 * @param pixelCenter The pixel center to be used
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam TScalar The data type of the scalar to be used, either 'float' or 'double'
		 */
		template <typename TScalar>
		static bool testInterpolatePixel8BitPerChannel(const CV::PixelCenter pixelCenter, const double testDuration);

		/**
		 * Tests the bilinear infinite border interpolation function.
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolateInfiniteBorder8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the bilinear infinite border interpolation function.
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static bool testInterpolateInfiniteBorder8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

	protected:

		/**
		 * Determines the interpolation for one pixel.
		 * @param frame The frame that is used for interpolation, must be valid
		 * @param alphaAtFront True, if the alpha channel is at the front of the data channels
		 * @param transparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 * @param position The position for which the interpolated pixel will be determined, with ranges [0, frame.width() - 1]x[0, frame.height() - 1] for PC_TOP_LEFT, [0, frame.width()]x[0, frame.height()] for PC_CENTER
		 * @param pixelCenter The pixel center to be used during interpolation
		 * @param result The resulting interpolated pixel value, must be valid
		 * @return True, if succeeded
		 * @tparam TScalar The data type of a scalar, either 'float' or 'double'
		 */
		template <typename TScalar>
		static bool interpolatePixel8BitPerChannel(const Frame& frame, const bool alphaAtFront, const bool transparentIs0xFF, const VectorT2<TScalar>& position, const CV::PixelCenter pixelCenter, uint8_t* result);

		/**
		 * Returns the infinite border interpolation result for a given frame and interpolation position.
		 * @param frame The frame that is used for interpolation, must be valid
		 * @param alphaAtFront True, if the alpha channel is at the front of the data channels
		 * @param transparentIs0xFF True, if 0xFF is interpreted as fully transparent
		 * @param position Interpolation position, must be valid
		 * @param result Resulting interpolation value, must be valid
		 * @return True, if the interpolation position is inside the frame area
		 */
		static bool infiniteBorderInterpolation8BitPerChannel(const Frame& frame, const bool alphaAtFront, const bool transparentIs0xFF, const Vector2& position, uint8_t* result);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BILINEAR_ALPHA_H
