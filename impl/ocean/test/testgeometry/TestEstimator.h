/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_ESTIMATOR_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_ESTIMATOR_H

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements the test for the Estimator class of the geometry library.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestEstimator
{
	public:

		/**
		 * Tests the entire error functionality.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the needSigma() function.
		 * @return True, if succeeded
		 */
		static bool testNeedSigma();

		/**
		 * Tests the isStandardEstimator() function.
		 * @return True, if succeeded
		 */
		static bool testIsStandardEstimator();

		/**
		 * Tests the robust error function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRobustError(const double testDuration);

		/**
		 * Tests the robust weight function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRobustWeight(const double testDuration);

		/**
		 * Tests the translate function.
		 * @return True, if succeeded
		 */
		static bool testTranslateEstimatorType();

	protected:

		/**
		 * Returns whether the absolute value of a given first value is smaller than the absolute value of a given second value.
		 * @param firstValue The first value to compare, with range (-infinity, infinity)
		 * @param secondValue The second value to compare, with range (-infinity, infinity)
		 * @return True, if so
		 */
		static inline bool compareAbsolute(const Scalar& firstValue, const Scalar& secondValue);
};

inline bool TestEstimator::compareAbsolute(const Scalar& firstValue, const Scalar& secondValue)
{
	return Numeric::abs(firstValue) < Numeric::abs(secondValue);
}

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_ESTIMATOR_H
