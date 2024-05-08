/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_SUM_ABSOLUTE_DIFFERENCES_H
#define META_OCEAN_TEST_TESTCV_SUM_ABSOLUTE_DIFFERENCES_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/cv/SumAbsoluteDifferences.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements sum absolute differences (SAD) tests.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestSumAbsoluteDifferences
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
		 * Invokes all tests testing absolute differences functionalities.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the absolute differences function between entire frames with one plane.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDifferenceBetweenFramesWithOnePlane(const double testDuration);

		/**
		 * Tests the absolute differences function between entire frames with several planes.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDifferenceBetweenFramesWithSeveralPlanes(const double testDuration);

		/**
		 * Tests the absolute differences function for two image patches.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatch8BitPerChannel(const double testDuration);

		/**
		 * Tests the absolute differences function for two buffers.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testBuffer8BitPerChannel(const double testDuration);

		/**
		 * Tests the sum absolute differences function between an image patch and a buffer.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchBuffer8BitPerChannel(const double testDuration);

		/**
		 * Tests the sum absolute differences function for two image patches which are mirrored at the image border.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testPatchMirroredBorder8BitPerChannel(const double testDuration);

	private:

		/**
		 * Applies one test of the absolute differences function between entire frames with one plane for a specific data type.
		 * @return True, if succeeded
		 * @tparam T The data type to test
		 */
		template <typename T>
		static bool testDifferenceBetweenFramesWithOnePlane();

		/**
		 * Applies one test of the absolute differences function between entire frames with one plane for a specific data type and channel number.
		 * @return True, if succeeded
		 * @tparam T The data type to test
		 * @tparam tChannels The number of frame channels, with range [1, 5]
		 */
		template <typename T, unsigned int tChannels>
		static bool testDifferenceBetweenFramesWithOnePlane();

		/**
		 * Tests the absolute differences function for two image patches.
		 * @param width The width of the test image, in pixel, with range [tPatchSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tPatchSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static bool testPatch8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the sum absolute differences function for two buffers.
		 * @param width The width of the test image, in pixel, with range [1, infinity)
		 * @param height The height of the test image, in pixel, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPixels The number of pixels in the buffer, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPixels>
		static bool testBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the sum absolute differences function between an image patch and a buffer.
		 * @param width The width of the test image, in pixel, with range [tPatchSize, infinity)
		 * @param height The height of the test image, in pixel, with range [tPatchSize, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 * @tparam tPatchSize The size of the patch, with range [1, infinity)
		 */
		template <unsigned int tChannels, unsigned int tPatchSize>
		static bool testPatchBuffer8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration);

		/**
		 * Tests the sum absolute differences function for two image patches which are mirrored at the image border.
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

#endif // META_OCEAN_TEST_TESTCV_SUM_ABSOLUTE_DIFFERENCES_H
