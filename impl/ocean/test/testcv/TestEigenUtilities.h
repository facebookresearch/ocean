/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTCV_TEST_EIGEN_UTILITIES_H
#define META_OCEAN_TEST_TESTCV_TEST_EIGEN_UTILITIES_H

#include "ocean/test/testcv/TestCV.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * This class implements tests for the EigenUtilities class.
 * @ingroup testcv
 */
class OCEAN_TEST_CV_EXPORT TestEigenUtilities
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the frame to matrix conversion function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testFrame2Matrix(const double testDuration);

		/**
		 * Tests the conversion of Ocean VectorT2 to Eigen::Vector2T
		 * @param testDuration Number of seconds that this test is rung, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testToEigenVector(const double testDuration);

		/**
		 * Tests the conversion of Eigen::Vector2T to Ocean VectorT2
		 * @param testDuration Number of seconds that this test is rung, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testToOceanVector(const double testDuration);

		/**
		 * Tests the conversion of Ocean::QuaternionT<T> to Eigen::Quaternion<T>
		 * @param testDuration Number of seconds that this test is rung, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testToEigenQuaternion(const double testDuration);

		/**
		 * Tests the conversion of Eigen::Quaternion<T> to Ocean::QuaternionT<T>
		 * @param testDuration Number of seconds that this test is rung, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testToOceanQuaternion(const double testDuration);

	protected:

		/**
		 * Tests the frame to matrix conversion function for a specific source type and target type.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam TSource The data type of each source element
		 * @tparam TTarget The data type of each target element
		 */
		template <typename TSource, typename TTarget>
		static bool testFrame2Matrix(const double testDuration);

		/**
		 * Tests the conversion of Ocean VectorT{2,3,4}T to Eigen::Vector{2,3,4}T
		 * @param testDuration Number of seconds that this test will be run, range: (0, infinity)
		 * @return True if the test passed, otherwise false
		 * @tparam TType The type of the vector elements, must be either `int`, `float`, or `double`
		 * @tparam tElements Number of vector elements, range: [2, 4]
		 * @tparam TOceanVector The type of the Ocean vector that has `tElements` elements of type `TType`, e.g., `TType=int` and `tElements=3` then `TOceanVector=VectorI3`
		 */
		template <typename TType, size_t tElements, typename TOceanVector>
		static bool testToEigenVector(const double testDuration);

		/**
		 * Tests the conversion of Eigen::Vector{2,3,4}T to Ocean VectorT{2,3,4}T
		 * @param testDuration Number of seconds that this test will be run, range: (0, infinity)
		 * @return True if the test passed, otherwise false
		 * @tparam TType The type of the vector elements, must be either `int`, `float`, or `double`
		 * @tparam tElements Number of vector elements, range: [2, 4]
		 * @tparam TOceanVector The type of the Ocean vector that has `tElements` elements of type `TType`, e.g., `TType=int` and `tElements=3` then `TOceanVector=VectorI3`
		 */
		template <typename TType, size_t tElements, typename TOceanVector>
		static bool testToOceanVector(const double testDuration);

		/**
		 * Tests the conversion of Ocean::QuaternionT<T> to Eigen::Quaternion<T>
		 * @param testDuration Number of seconds that this test is rung, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam TType Type of the quaternion elements, must be either {float, double}
		 */
		template <typename TType>
		static bool testToEigenQuaternion(const double testDuration);

		/**
		 * Tests the conversion of Eigen::Quaternion<T> to Ocean::QuaternionT<T>
		 * @param testDuration Number of seconds that this test is rung, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam TType Type of the quaternion elements, must be either {float, double}
		 */
		template <typename TType>
		static bool testToOceanQuaternion(const double testDuration);
};

} // namespace CV

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTCV_TEST_EIGEN_UTILITIES_H
