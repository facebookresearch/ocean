/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/testbullseyes/TestAssignmentSolver.h"

#include "ocean/base/RandomI.h"

#include "ocean/math/Random.h"

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

using namespace CV::Detector::Bullseyes;

bool TestAssignmentSolver::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test for AssignmentSolver:   ---";
	Log::info() << " ";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	allSucceeded = testSolve(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "AssignmentSolver test succeeded.";
	}
	else
	{
		Log::info() << "AssignmentSolver test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

} // namespace TestBullseyes

TEST(TestAssignmentSolver, Solve)
{
	EXPECT_TRUE(TestDetector::TestBullseyes::TestAssignmentSolver::testSolve(GTEST_TEST_DURATION));
}

namespace TestBullseyes
{

#endif // OCEAN_USE_GTEST

bool TestAssignmentSolver::testSolve(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "AssignmentSolver::solve() test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	const Timestamp startTimestamp(true);

	do
	{
		// Test with various matrix sizes (both square and rectangular)
		const unsigned int rows = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int columns = RandomI::random(randomGenerator, 1u, 100u);

		// Create a random cost matrix
		CostMatrix costMatrix;
		if (!createRandomValidCostMatrix(randomGenerator, rows, columns, Scalar(0), Scalar(1000), costMatrix))
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		// Keep a copy for validation
		const CostMatrix originalCostMatrix = costMatrix;

		// Solve the assignment problem
		Assignments assignments;
		const bool solveResult = solve(std::move(costMatrix), assignments);

		// Verify solve returned true for valid input
		if (!solveResult)
		{
			allSucceeded = false;
			break;
		}

		// Validate the solution
		if (!validateSolve(originalCostMatrix, assignments))
		{
			allSucceeded = false;
			break;
		}

		// Test rectangular matrices
		if (rows != columns)
		{
			// The validation already checks this, but explicitly verify min assignment count
			const unsigned int expectedAssignments = std::min(rows, columns);
			if (assignments.size() != expectedAssignments)
			{
				allSucceeded = false;
				break;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestAssignmentSolver::validateSolve(const CostMatrix& costMatrix, const Assignments& assignments)
{
	const unsigned int rows = costMatrix.rows();
	const unsigned int columns = costMatrix.columns();

	if (rows == 0u || columns == 0u)
	{
		return false;
	}

	const unsigned int expectedAssignments = std::min(rows, columns);

	// Check number of assignments
	if (assignments.size() != expectedAssignments)
	{
		return false;
	}

	// Track which rows and columns have been assigned
	std::vector<bool> rowAssigned(rows, false);
	std::vector<bool> columnAssigned(columns, false);

	// Validate each assignment
	for (const IndexPair32& assignment : assignments)
	{
		const Index32 row = assignment.first;
		const Index32 column = assignment.second;

		// Check bounds
		if (row >= rows || column >= columns)
		{
			return false;
		}

		// Check for duplicates
		if (rowAssigned[row] || columnAssigned[column])
		{
			return false;
		}

		rowAssigned[row] = true;
		columnAssigned[column] = true;

		// Check that the cost is non-negative
		if (costMatrix(row, column) < Scalar(0))
		{
			return false;
		}
	}

	return true;
}

bool TestAssignmentSolver::createRandomValidCostMatrix(RandomGenerator& randomGenerator, const unsigned int rows, const unsigned int columns, const Scalar minimumCost, const Scalar maximumCost, CostMatrix& costMatrix)
{
	ocean_assert(rows != 0u && columns != 0u);
	if (rows == 0u || columns == 0u)
	{
		return false;
	}

	ocean_assert(minimumCost <= maximumCost);
	if (minimumCost > maximumCost)
	{
		return false;
	}

	ocean_assert(minimumCost >= 0);
	if (minimumCost < 0)
	{
		return false;
	}

	costMatrix.resize(rows, columns);

	for (unsigned int row = 0u; row < rows; ++row)
	{
		for (unsigned int column = 0u; column < columns; ++column)
		{
			costMatrix(row, column) = Random::scalar(randomGenerator, minimumCost, maximumCost);
		}
	}

	return true;
}

} // namespace TestBullseyes

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
