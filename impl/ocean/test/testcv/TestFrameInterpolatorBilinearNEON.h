/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BILINEAR_NEON_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BILINEAR_NEON_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the NEON-accelerated bilinear interpolation functions.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameInterpolatorBilinearNEON
{
	public:

		/**
		 * Invokes all NEON interpolation tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the correctness of interpolate4Pixels8BitPerChannelNEON<4u> against a scalar reference.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolate4Pixels8BitPerChannel4NEON(const double testDuration);

		/**
		 * Tests the correctness of interpolate4Pixels4Channel8BitPerChannelNEON against a scalar reference.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolate4Pixels4Channel8BitPerChannelNEON(const double testDuration);

		/**
		 * Benchmarks and compares the speed of the two 4-channel NEON interpolation functions.
		 * @param testDuration Number of seconds for the benchmark, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testInterpolate4Pixels4ChannelNEONSpeedComparison(const double testDuration);

		/**
		 * Tests that lookup8BitPerChannelSubsetNEON produces identical results with useOptimizedNEON=true and useOptimizedNEON=false for 4 channels.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testLookup4ChannelOptimizedNEONConsistency(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_INTERPOLATOR_BILINEAR_NEON_H
