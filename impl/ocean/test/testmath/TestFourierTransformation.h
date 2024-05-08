/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTMATH_TEST_FOURIER_TRANSFORMATION_H
#define META_OCEAN_TEST_TESTMATH_TEST_FOURIER_TRANSFORMATION_H

#include "ocean/test/testmath/TestMath.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

/**
 * This class implements test for the Fourier transformation class.
 * @ingroup testmath
 */
class OCEAN_TEST_MATH_EXPORT TestFourierTransformation
{
	public:

		/**
		 * This functions tests all 3D line functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the forward and backward (inverse) Fourier transformation.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be tested, 'float' or 'double'
		 * @tparam tSourceIsComplex True, if the source signal is complex; False, if the source is composed of real elements only
		 */
		template <typename T, bool tSourceIsComplex>
		static bool testFourierTransform(const double testDuration);

		/**
		 * Tests the element-wise multiplication of two complex spectrums.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be tested, 'float' or 'double'
		 */
		template <typename T>
		static bool testElementwiseMultiplication2(const double testDuration);

		/**
		 * Tests the element-wise division of two complex spectrums.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be tested, 'float' or 'double'
		 */
		template <typename T>
		static bool testElementwiseDivision2(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTMATH_TEST_FOURIER_TRANSFORMATION_H
