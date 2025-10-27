/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/AssignmentSolver.h"

#include <algorithm>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

bool AssignmentSolver::solve(CostMatrix&& costMatrix, Assignments& assignments)
{
	ocean_assert(costMatrix.elements() != 0);
	if (costMatrix.elements() == 0)
	{
		return false;
	}

	ocean_assert(*std::min_element(costMatrix.data(), costMatrix.data() + costMatrix.elements()) >= 0 && "All values in the cost matrix must be non-negative");

	assignments.clear();

	// Original matrix dimensions required to extract the final assignments
	const size_t originalRows = costMatrix.rows();
	const size_t originalColumns = costMatrix.columns();

	// The cost matrix must be square
	if (originalRows != originalColumns)
	{
		const Scalar maxValue = *std::max_element(costMatrix.data(), costMatrix.data() + costMatrix.elements());
		ocean_assert(maxValue >= 0);

		const size_t matrixSize = std::max(originalRows, originalColumns);
		const Scalar fillValue = Scalar(matrixSize) * maxValue;

		costMatrix = CostMatrix(matrixSize, matrixSize, costMatrix, 0, 0, fillValue);
	}

	const size_t matrixSize = costMatrix.rows();
	ocean_assert(matrixSize == costMatrix.columns() && matrixSize != 0);

	subtractRowAndColumnMinima(costMatrix);

	Indices32 yAssignments;
	Indices32 xAssignments;

	// Helper variables for the algorithm
	std::vector<bool> yMarked;
	std::vector<bool> xMarked;

	std::vector<bool> yVisited;
	std::vector<bool> xVisited;
	Indices32 yParents;

	while (true)
	{
		// Try to find matches using the current zero elements in the cost matrix
		yAssignments.assign(matrixSize, invalidIndex());
		xAssignments.assign(matrixSize, invalidIndex());

		for (size_t y = 0; y < matrixSize; ++y)
		{
			findAugmentingPath(costMatrix, y, yAssignments, xAssignments, yVisited, xVisited, yParents);
		}

		// Check if we have found a valid assignment
		size_t numberAssignments = 0;
		for (size_t y = 0; y < matrixSize; ++y)
		{
			if (yAssignments[y] != invalidIndex())
			{
				++numberAssignments;
			}
		}

		if (numberAssignments < matrixSize)
		{
			// Reduce the cost matrix and try again.
			if (!reduceCostMatrix(yAssignments, costMatrix, yMarked, xMarked))
			{
				ocean_assert(false && "Failed to reduce the cost matrix - this should never happen!");
				return false;
			}
		}
		else
		{
			// A solution has been found.
			break;
		}
	}

	for (size_t y = 0; y < originalRows; ++y)
	{
		const Index32 x = yAssignments[y];

		if (x != invalidIndex() && size_t(x) < originalColumns)
		{
			ocean_assert(y <= size_t(NumericT<Index32>::maxValue()));
			assignments.emplace_back(Index32(y), x);
		}
	}

	return true;
}

void AssignmentSolver::subtractRowAndColumnMinima(CostMatrix& costMatrix)
{
	ocean_assert(costMatrix.elements() != 0);

	// Row minima
	for (size_t y = 0; y < costMatrix.rows(); ++y)
	{
		Scalar rowMinimum = Numeric::maxValue();

		for (size_t x = 0; x < costMatrix.columns(); ++x)
		{
			rowMinimum = std::min(rowMinimum, costMatrix[y][x]);
		}

		for (size_t x = 0; x < costMatrix.columns(); ++x)
		{
			costMatrix[y][x] -= rowMinimum;
		}
	}

	// Column minima
	for (size_t x = 0; x < costMatrix.columns(); ++x)
	{
		Scalar columnMinimum = Numeric::maxValue();

		for (size_t y = 0; y < costMatrix.rows(); ++y)
		{
			columnMinimum = std::min(columnMinimum, costMatrix[y][x]);
		}

		for (size_t y = 0; y < costMatrix.rows(); ++y)
		{
			costMatrix[y][x] -= columnMinimum;
		}
	}
}

bool AssignmentSolver::findAugmentingPath(const CostMatrix& costMatrix, const size_t yStart, Indices32& yAssignments, Indices32& xAssignments, std::vector<bool>& yVisited, std::vector<bool>& xVisited, Indices32& yParents)
{
	ocean_assert(costMatrix.elements() != 0);
	ocean_assert(costMatrix.rows() == costMatrix.columns());
	ocean_assert(yStart < costMatrix.rows());

	const size_t matrixSize = costMatrix.rows();

	// Track parent relationships: parentRow[col] = row that led to this column
	// This allows us to reconstruct and update the augmenting path
	yParents.assign(matrixSize, invalidIndex());

	yVisited.assign(matrixSize, false);
	xVisited.assign(matrixSize, false);

	yVisited[yStart] = true;

	ocean_assert(yStart <= size_t(NumericT<Index32>::maxValue()));
	Indices32 rowQueue{Index32(yStart)};
	Index32 xAssignmentNew = invalidIndex();
	while (!rowQueue.empty() && xAssignmentNew == invalidIndex())
	{
		const Index32 yCurrent = rowQueue.back();
		rowQueue.pop_back();

		// Try all columns with zero cost
		for (size_t x = 0; x < matrixSize; ++x)
		{
			if (xVisited[x] || costMatrix[yCurrent][x] > Numeric::weakEps())
			{
				continue;
			}

			xVisited[x] = true;
			yParents[x] = yCurrent;

			// Found an unassigned column - we have an augmenting path!
			if (xAssignments[x] == invalidIndex())
			{
				ocean_assert(x <= size_t(NumericT<Index32>::maxValue()));
				xAssignmentNew = Index32(x);
				break;
			}

			// This column is assigned to another row - continue search from that row
			const Index32 yPrevious = xAssignments[x];
			if (!yVisited[yPrevious])
			{
				yVisited[yPrevious] = true;
				rowQueue.push_back(yPrevious);
			}
		}
	}

	if (xAssignmentNew == invalidIndex())
	{
		return false;
	}

	// Found an augmenting path, update assignments along the path
	Index32 x = xAssignmentNew;
	while (x != invalidIndex())
	{
		const Index32 y = yParents[x];
		const Index32 xPrevious = yAssignments[y];

		yAssignments[y] = x;
		xAssignments[x] = y;

		x = xPrevious;
	}

	return true;
}

bool AssignmentSolver::reduceCostMatrix(const Indices32& yAssignments, CostMatrix& costMatrix, std::vector<bool>& yMarked, std::vector<bool>& xMarked)
{
	ocean_assert(yAssignments.size() == costMatrix.rows());
	ocean_assert(costMatrix.elements() != 0);
	ocean_assert(costMatrix.rows() == costMatrix.columns());

	const size_t matrixSize = costMatrix.rows();

	// Cover all zero elements in the cost matrix with a minimum number of lines

	// Mark unassigned rows
	yMarked.assign(matrixSize, false);
	xMarked.assign(matrixSize, false);

	for (size_t row = 0; row < matrixSize; ++row)
	{
		if (yAssignments[row] == invalidIndex())
		{
			yMarked[row] = true;
		}
	}

	// Continue to add rows and columns until no more changes are made
	bool changed = true;
	while (changed)
	{
		changed = false;

		// Mark columns with zeros in marked rows
		for (size_t y = 0; y < matrixSize; ++y)
		{
			if (!yMarked[y])
			{
				continue;
			}

			for (size_t x = 0; x < matrixSize; ++x)
			{
				if (!xMarked[x] && costMatrix[y][x] < Numeric::weakEps())
				{
					xMarked[x] = true;
					changed = true;
				}
			}
		}

		// Mark rows with assignments in marked columns
		for (size_t x = 0; x < matrixSize; ++x)
		{
			if (!xMarked[x])
			{
				continue;
			}

			for (size_t y = 0; y < matrixSize; ++y)
			{
				if (!yMarked[y] && yAssignments[y] == static_cast<int>(x))
				{
					yMarked[y] = true;
					changed = true;
				}
			}
		}
	}

	// Note: We cover unmarked rows and marked columns
	// Instead of creating separate yCovered/xCovered
	// arrays, we reuse yMarked/xMarked by inverting
	// the logic for rows in the following code.

	// Find smallest uncovered value
	Scalar uncoveredMinimum = Numeric::maxValue();

	for (size_t y = 0; y < matrixSize; ++y)
	{
		if (!yMarked[y])
		{
			// Ignore covered rows (unmarked rows are covered)
			continue;
		}

		for (size_t x = 0; x < matrixSize; ++x)
		{
			if (xMarked[x])
			{
				// Ignore covered columns (marked columns are covered)
				continue;
			}

			uncoveredMinimum = std::min(uncoveredMinimum, costMatrix[y][x]);
		}
	}

	// Adjust matrix:
	// - Subtract min from uncovered elements (marked rows, unmarked columns)
	// - Add min to elements covered twice (unmarked rows, marked columns)
	for (size_t y = 0; y < matrixSize; ++y)
	{
		for (size_t x = 0; x < matrixSize; ++x)
		{
			if (!yMarked[y] && xMarked[x])  // Row covered (unmarked) and column covered (marked)
			{
				costMatrix[y][x] += uncoveredMinimum;
			}
			else if (yMarked[y] && !xMarked[x])  // Row uncovered (marked) and column uncovered (unmarked)
			{
				ocean_assert(costMatrix[y][x] >= uncoveredMinimum);
				costMatrix[y][x] -= uncoveredMinimum;
			}
			// else: covered once (row or column but not both) - do nothing
		}
	}

	return true;
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
