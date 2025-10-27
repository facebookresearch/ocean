/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/test/testcv/testdetector/testbullseyes/TestCVDetectorBullseyes.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/bullseyes/AssignmentSolver.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

namespace TestBullseyes
{

/**
 * This class implements tests for the AssignmentSolver class
 * @ingroup testcvdetectorbullseyes
 */
class OCEAN_TEST_CV_DETECTOR_BULLSEYES_EXPORT TestAssignmentSolver : protected CV::Detector::Bullseyes::AssignmentSolver
{
	public:

		/**
		 * Tests the AssignmentSolver functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Test for AssignmentSolver::solve()
		 * @param testDuration The duration in seconds for which this test will be run, must be > 0.0
		 * @return True, if succeeded
		 */
		static bool testSolve(const double testDuration);

	protected:

		/**
		 * Validates the result of solve() by checking:
		 * - All assignment pairs are within bounds
		 * - No duplicate row or column assignments
		 * - Number of assignments equals min(rows, columns)
		 * - All assigned costs are non-negative
		 * @param costMatrix The original cost matrix used for solving
		 * @param assignments The resulting assignments from solve()
		 * @return True if validation succeeds, False otherwise
		 */
		static bool validateSolve(const CostMatrix& costMatrix, const Assignments& assignments);

		/**
		 * Creates a random valid cost matrix with specified dimensions and cost range.
		 * All generated costs will be non-negative values within the specified range.
		 * @param randomGenerator A random generator to use for generating random values
		 * @param rows The number of rows in the cost matrix, must be > 0
		 * @param columns The number of columns in the cost matrix, must be > 0
		 * @param minimumCost The minimum cost value, must be >= 0
		 * @param maximumCost The maximum cost value, must be >= minimumCost
		 * @param costMatrix The resulting cost matrix, will be resized and filled with random values
		 * @return True if the cost matrix was created successfully, False otherwise
		 */
		static bool createRandomValidCostMatrix(RandomGenerator& randomGenerator, const size_t rows, const size_t columns, const Scalar minimumCost, const Scalar maximumCost, CostMatrix& costMatrix);
	};

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
