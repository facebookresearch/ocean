/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FOURIER_TRANSFORM_H
#define META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FOURIER_TRANSFORM_H

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

/**
 * This class tests the performance of Fourier transform algorithms.
 * @ingroup testcvopencv
 */
class OCEAN_TEST_CV_OPENCV_EXPORT TestFourierTransform
{
	public:

		/**
		 * Runs the test, validates the code and benchmarks the performance.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True if all tests passed, otherwise false
		 */
		static bool test(const double testDuration);

		/**
		 * Benchmarks the FFT performance and validate the results, i.e., identical results with OpenCV
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True if the validation was successul, otherwise false
		 * @tparam TDataType Data type used to test this function, valid values: `float` or `double`
		 */
		template <typename TDataType>
		static bool testFFT(const double testDuration);

		/**
		 * Uses random data with padding in order to benchmark the FFT performance and validate the results, i.e., identical results with OpenCV
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True if the validation was successul, otherwise false
		 * @tparam TDataType Data type used to test this function, valid values: `float` or `double`
		 */
		 template <typename TDataType>
		static bool testFFTPadding(const double testDuration);

		/**
		 * Benchmarks and validate the FFT performance using the CCS-packed format.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True if the validation was successful, otherwise false
		 */
		static bool testElementwiseMultiplicationCCS(const double testDuration);

		/**
		 * Benchmarks and validate the FFT performance using the CCS-packed format.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True if the validation was successful, otherwise false
		 * @tparam TElementType Type used for the test data, can be either float or double
		 * @tparam tConjugateB If true, multiply the first value with the conjugate of the second
		 */
		template <typename TElementType, bool tConjugateB>
		static bool testElementwiseMultiplicationCCS(const double testDuration);
};

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TESTOPENCV_TEST_FOURIER_TRANSFORM_H
