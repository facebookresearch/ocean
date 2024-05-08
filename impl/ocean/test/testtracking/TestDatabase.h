/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTRACKING_TEST_DATABASE_H
#define META_OCEAN_TEST_TESTTRACKING_TEST_DATABASE_H

#include "ocean/test/testtracking/TestTracking.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/tracking/Database.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

/**
 * This class implements tests for the Database class.
 * @ingroup testtracking
 */
class OCEAN_TEST_TRACKING_EXPORT TestDatabase
{
	public:

		/**
		 * Starts all tests for the database class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the addObjectPointFromDatabase function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testAddObjectPointFromDatabase(const double testDuration);

		/**
		 * Tests the serialization of the database.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSerialization(const double testDuration);

	protected:

		/**
		 * Creates a database with random topology.
		 * The database will not contain meaninful geometric information.
		 * @param randomGenerator The random generator object to be used
		 * @param lowerPoseId The id of the lower border defining the allowed pose range, with range [0, infinity)
		 * @param upperPoseId The id of the upper border defining the allowed pose range, with range [lowerPoseId, infinity)
		 * @param numberPoses The number of poses that will be created with the defined pose range, with range [1, upperPoseId - lowerPoseId + 1]
		 * @param numberObjectPoints The number of object points the database will store, with range [1, infinity)
		 * @param minimalNumberObservations The minimal number of observations in each pose (the minimal number of image points in each pose), with range [0, numberObjectPoints]
		 * @param maximalNumberObservations The maximal number of observations in each pose (the maximal number of image points in each pose), with range [max(1, minimalNumberObservations), numberObjectPoints]
		 */
		static Tracking::Database createDatabaseWithRandomTopology(RandomGenerator& randomGenerator, const unsigned int lowerPoseId, const unsigned int upperPoseId, const unsigned int numberPoses, const unsigned int numberObjectPoints, const unsigned int minimalNumberObservations, const unsigned int maximalNumberObservations);
};

}

}

}

#endif // META_OCEAN_TEST_TESTTRACKING_TEST_DATABASE_H
