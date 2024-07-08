/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_H
#define META_OCEAN_TEST_TESTCV_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/cv/ZeroMeanSumSquareDifferences.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements zero-mean sum square differences (ZMSSD) tests.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestZeroMeanSumSquareDifferences
{
	protected:

		/**
		 * Definition of individual types of implementation.
		 */
		enum ImplementationType : uint32_t
		{
			/// The naive implementation.
			IT_NAIVE,
			/// The template-based implementation.
			IT_TEMPLATE,
			/// The SSE-based implementation.
			IT_SSE,
			/// The NEON-based implementation.
			IT_NEON,
			/// The default implementation (which is actually used by default).
			IT_DEFAULT
		};

	public:

		/**
		 * Tests the entire zero-mean sum square differences functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the zero-mean sum square differences function for two image patches.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatch8BitPerChannel(const double testDuration);

		/**
		 * Tests the zero-mean sum square differences function for two buffers.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBuffer8BitPerChannel(const double testDuration);

		/**
		 * Tests the zero-mean sum square differences function between an image patch and a buffer.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchBuffer8BitPerChannel(const double testDuration);

		/**
		 * Tests the zero-mean sum square differences function for two image patches which are mirrored at the image border.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchMirroredBorder8BitPerChannel(const double testDuration);

	private:

		/**
		 * Tests the zero mean sum square differences function for two image patches.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static bool testPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the zero-mean sum square differences function for two buffers.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPixels The number of pixels in the buffer, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPixels>
		static bool testBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the zero-mean sum square differences function between an image patch and a buffer.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static bool testPatchBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the zero-mean sum square differences function for two image patches which are mirrored at the image border.
		 * @param width The width of the test image, in pixel, with range [tSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static bool testPatchMirroredBorder8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_ZERO_MEAN_SUM_SQUARE_DIFFERENCES_H
