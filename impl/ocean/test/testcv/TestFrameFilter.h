/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_H
#define META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_H

#include "ocean/test/testcv/TestCV.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements a test for frame filter functions.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestFrameFilter
{
	public:

		/**
		 * Tests all frame filter functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the magnitude function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 */
		static bool testMagnitude(const double testDuration, Worker& worker);

		/**
		 * Tests the normalize value function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNormalizeValue(const double testDuration);

		/**
		 * Tests the normalize value function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam T The data type to be used
		 */
		template <typename T>
		static bool testNormalizeValue(const double testDuration);

	protected:

		/**
		 * Tests the magnitude function.
		 * @param magnitudeDataType The data type of the magnitude to be used for the test
		 * @param randomGenerator The random generator to be used
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of the frame to be used for the test
		 */
		template <typename T>
		static bool testMagnitude(const FrameType::DataType magnitudeDataType, RandomGenerator& randomGenerator, Worker& worker);

		/**
		 * Tests the magnitude function.
		 * @param randomGenerator The random generator to be used
		 * @param worker The worker object to distribute the CPU load
		 * @return True, if succeeded
		 * @tparam T The data type of the frame to be used for the test
		 * @tparam TMagnitude The data type of the magnitude to be used for the test
		 */
		template <typename T, typename TMagnitude>
		static bool testMagnitude(RandomGenerator& randomGenerator, Worker& worker);

		/**
		 * Verifies the value normalization.
		 * @param value The value to be normalized
		 * @tparam T The data type to be used
		 * @tparam tNormalizationDenominator The normalization factor, with range [1, infinity)
		 */
		template <typename T, T tNormalizationDenominator>
		static bool verifyValueNormalization(const T& value);

		/**
		 * Verifies the value normalization for floats.
		 * @param value The value to be normalized
		 * @tparam T The data type to be used
		 * @tparam tNormalizationDenominator The normalization factor, with range [1, infinity)
		 */
		template <typename T, T tNormalizationDenominator>
		static bool verifyValueNormalizationFloat(const T& value);

		/**
		 * Verifies the value normalization for integers.
		 * @param value The value to be normalized
		 * @tparam T The data type to be used
		 * @tparam tNormalizationDenominator The normalization factor, with range [1, infinity)
		 */
		template <typename T, T tNormalizationDenominator>
		static bool verifyValueNormalizationInteger(const T& value);

		/**
		 * Verifies the rounded value normalization for integers.
		 * @param value The value to be normalized
		 * @tparam T The data type to be used
		 * @tparam tNormalizationDenominator The normalization factor, with range [1, infinity)
		 */
		template <typename T, T tNormalizationDenominator>
		static bool verifyValueNormalizationIntegerRounded(const T& value);

		/**
		 * Verifies the not-rounded value normalization for integers.
		 * @param value The value to be normalized
		 * @tparam T The data type to be used
		 * @tparam tNormalizationDenominator The normalization factor, with range [1, infinity)
		 */
		template <typename T, T tNormalizationDenominator>
		static bool verifyValueNormalizationIntegerNotRounded(const T& value);
};

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TEST_FRAME_FILTER_H
