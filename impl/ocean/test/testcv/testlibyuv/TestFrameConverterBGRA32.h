/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_CONVERTER_BGRA_32_H
#define META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_CONVERTER_BGRA_32_H

#include "ocean/test/testcv/testlibyuv/TestLibyuv.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestLibyuv
{

/**
 * This class tests/benchmarks the performance of the frame converter function of libyuv for BGRA32 frames.
 * @ingroup testcvlibyuv
 */
class OCEAN_TEST_CV_LIBYUV_EXPORT TestFrameConverterBGRA32
{
	public:

		/**
		 * Benchmarks the performance of libyuv agains Ocean for frame converter functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param skipValidation True, to skip the validation
		 */
		static void test(const double testDuration, const bool skipValidation = false);

		/**
		 * Benchmarks the conversion from BGRA32 frames to Y8 frames.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param skipValidation True, to skip the validation
		 * @return True, if succeeded
		 */
		static bool testConvertBGRA32ToY8(const double testDuration, const bool skipValidation = false);
};

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTLIBYUV_TEST_FRAME_CONVERTER_BGRA_32_H
