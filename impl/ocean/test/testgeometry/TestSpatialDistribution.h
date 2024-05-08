/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_SPATIAL_DISTRIBUTION_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_SPATIAL_DISTRIBUTION_H

#include "ocean/test/testgeometry/TestGeometry.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements spatial distribution tests.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestSpatialDistribution
{
	public:

		/**
		 * Tests all spatial distribution functions.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the function calculating the ideal number of bins.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIdealBins(const double testDuration);

		/**
		 * Tests the function calculating the ideal number of bins with the 9 neighborhood guarantee.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testIdealBinsNeighborhood9(const double testDuration);

		/**
		 * Tests the determination of minimal square distances.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMinimalSqrDistances(const double testDuration);

		/**
		 * Tests the distribute function.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistribute(const double testDuration);

		/**
		 * Tests the distribute and filter function.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistributeAndFilter(const double testDuration);

		/**
		 * Tests the distribute and filter indices function.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDistributeAndFilterIndices(const double testDuration);

		/**
		 * Tests the copy constructor with copying 8-neighborhood.
		 * @param testDuration Number of seconds for each test, with range  (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCopyConstructorWithNeighborhood8(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_SPATIAL_DISTRIBUTION_H
