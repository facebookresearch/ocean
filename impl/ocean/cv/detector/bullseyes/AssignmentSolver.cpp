/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/AssignmentSolver.h"

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
	ocean_assert(costMatrix.rows() != 0u && costMatrix.columns() != 0u);
	if (costMatrix.rows() == 0u || costMatrix.columns() == 0u)
	{
		return false;
	}

#if defined(OCEAN_DEBUG)
	// All values in the cost matrix must be non-negative
	for (unsigned int y = 0u; y < costMatrix.rows(); ++y)
	{
		for (unsigned int x = 0u; x < costMatrix.columns(); ++x)
		{
			ocean_assert(costMatrix(y, x) >= 0);
		}
	}
#endif

	assignments.clear();

	const unsigned int originalRows = costMatrix.rows();
	const unsigned int originalColumns = costMatrix.columns();

	// The cost matrix must be square
	if (originalRows != originalColumns)
	{
		costMatrix = convertToSquareCostMatrix(costMatrix);
	}

	const unsigned int matrixSize = costMatrix.rows();
	ocean_assert(matrixSize == costMatrix.columns() && matrixSize != 0u);

	subtractRowAndColumnMinima(costMatrix);

	Indices32 yAssignments;
	Indices32 xAssignments;

	// Helper variables for the algorithm
	std::vector<bool> yMarked;
	std::vector<bool> xMarked;
	while (true)
	{
		// Try to find matches using the current zero elements in the cost matrix
		yAssignments = Indices32(matrixSize, invalidIndex());
		xAssignments = Indices32(matrixSize, invalidIndex());

		for (size_t y = 0; y < matrixSize; ++y)
		{
			std::vector<bool> yVisited(matrixSize, false);
			std::vector<bool> xVisited(matrixSize, false);
			Indices32 yParents(matrixSize, invalidIndex());
			findAugmentingPath(costMatrix, y, yAssignments, xAssignments, yVisited, xVisited, yParents);
		}

		// Check if we have found a valid assignment
		unsigned int numberAssignments = 0u;
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

	for (unsigned int y = 0u; y < originalRows; ++y)
	{
		const Index32 x = yAssignments[y];

		if (x != invalidIndex() && x < originalColumns)
		{
			assignments.emplace_back(y, x);
		}
	}

	return true;
}

AssignmentSolver::CostMatrix AssignmentSolver::convertToSquareCostMatrix(const CostMatrix& costMatrix, Scalar fillValue)
{
	ocean_assert(costMatrix.rows() != 0u && costMatrix.columns() != 0u);

	ocean_assert(costMatrix.rows() != costMatrix.columns());
	if (costMatrix.rows() == costMatrix.columns())
	{
		return costMatrix;
	}

	const unsigned int rows = costMatrix.rows();
	const unsigned int columns = costMatrix.columns();

	const unsigned int matrixSize = std::max(rows, columns);

	CostMatrix squareCostMatrix(matrixSize, matrixSize);
	Scalar maxCost = Numeric::minValue();

	for (unsigned int y = 0u; y < rows; ++y)
	{
		for (unsigned int x = 0u; x < columns; ++x)
		{
			const Scalar cost = costMatrix(y, x);
			ocean_assert(cost >= 0);

			maxCost = std::max(maxCost, cost);

			squareCostMatrix(y, x) = cost;
		}
	}

	ocean_assert(maxCost >= 0);

	// Fill the added part (either right of or below of original matrix)
	if (fillValue < 0)
	{
		// Some large value, exact value isn't important
		fillValue = Scalar(matrixSize) * maxCost;
	}

	if (rows < columns)
	{
		for (unsigned int y = rows; y < matrixSize; ++y)
		{
			for (unsigned int x = 0u; x < matrixSize; ++x)
			{
				squareCostMatrix(y, x) = fillValue;
			}
		}
	}
	else
	{
		for (unsigned int x = columns; x < matrixSize; ++x)
		{
			for (unsigned int y = 0u; y < matrixSize; ++y)
			{
				squareCostMatrix(y, x) = fillValue;
			}
		}
	}

	return squareCostMatrix;
}

void AssignmentSolver::subtractRowAndColumnMinima(CostMatrix& costMatrix)
{
	ocean_assert(costMatrix.rows() != 0u && costMatrix.columns() != 0u);

	// Row minima
	for (unsigned int y = 0u; y < costMatrix.rows(); ++y)
	{
		Scalar rowMinimum = Numeric::maxValue();

		for (unsigned int x = 0u; x < costMatrix.columns(); ++x)
		{
			rowMinimum = std::min(rowMinimum, costMatrix[y][x]);
		}

		for (unsigned int x = 0u; x < costMatrix.columns(); ++x)
		{
			costMatrix[y][x] -= rowMinimum;
		}
	}

	// Column minima
	for (size_t x = 0; x < costMatrix.columns(); ++x)
	{
		Scalar columnMinimum = Numeric::maxValue();

		for (unsigned int y = 0u; y < costMatrix.rows(); ++y)
		{
			columnMinimum = std::min(columnMinimum, costMatrix[y][x]);
		}

		for (unsigned int y = 0u; y < costMatrix.rows(); ++y)
		{
			costMatrix[y][x] -= columnMinimum;
		}
	}
}

bool AssignmentSolver::findAugmentingPath(const CostMatrix& costMatrix, const unsigned int yStart, Indices32& yAssignments, Indices32& xAssignments, std::vector<bool>& yVisited, std::vector<bool>& xVisited, Indices32& yParents)
{
	ocean_assert(costMatrix.rows() != 0u);
	ocean_assert(costMatrix.rows() == costMatrix.columns());

	const unsigned int matrixSize = costMatrix.rows();

	// Track parent relationships: parentRow[col] = row that led to this column
	// This allows us to reconstruct and update the augmenting path
	yParents.assign(matrixSize, invalidIndex());

	yVisited[yStart] = true;

	Indices32 rowQueue{yStart};
	Index32 xAssignmentNew = invalidIndex();
	while (!rowQueue.empty() && xAssignmentNew == invalidIndex())
	{
		const Index32 yCurrent = rowQueue.back();
		rowQueue.pop_back();

		// Try all columns with zero cost
		for (unsigned int x = 0u; x < matrixSize; ++x)
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
				xAssignmentNew = x;
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
	ocean_assert(costMatrix.rows() != 0u);
	ocean_assert(costMatrix.rows() == costMatrix.columns());

	const unsigned int matrixSize = costMatrix.rows();

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
		for (unsigned int y = 0u; y < matrixSize; ++y)
		{
			if (!yMarked[y])
			{
				continue;
			}

			for (unsigned int x = 0u; x < matrixSize; ++x)
			{
				if (!xMarked[x] && costMatrix[y][x] < Numeric::weakEps())
				{
					xMarked[x] = true;
					changed = true;
				}
			}
		}

		// Mark rows with assignments in marked columns
		for (unsigned int x = 0u; x < matrixSize; ++x)
		{
			if (!xMarked[x])
			{
				continue;
			}

			for (unsigned int y = 0u; y < matrixSize; ++y)
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

	for (unsigned int y = 0u; y < matrixSize; ++y)
	{
		if (!yMarked[y])
		{
			// Ignore covered rows (unmarked rows are covered)
			continue;
		}

		for (unsigned int x = 0u; x < matrixSize; ++x)
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
	for (unsigned int y = 0u; y < matrixSize; ++y)
	{
		for (unsigned int x = 0u; x < matrixSize; ++x)
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
