/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_CV_DETECTOR_BULLSEYES_ASSIGNMENTSOLVER_H
#define OCEAN_CV_DETECTOR_BULLSEYES_ASSIGNMENTSOLVER_H

#include "ocean/cv/detector/bullseyes/Bullseyes.h"

#include "ocean/math/Matrix.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

/**
 * This class solves assignment problems using the Hungarian/Kuhn-Munkres algorithm.
 *
 * The assignment problem seeks to find an optimal matching between two sets (e.g., workers and jobs)
 * where each element in one set is assigned to exactly one element in the other set to minimize total cost.
 *
 * The solver accepts rectangular cost matrices and handles them by padding to square matrices internally.
 * All cost values must be non-negative. The algorithm guarantees finding an optimal solution in polynomial time.
 *
 * Example usage:
 * @code
 * AssignmentSolver::CostMatrix costs(5, 3);
 * // ... fill costs with non-negative values ...
 * AssignmentSolver::Assignments assignments;
 * if (AssignmentSolver::solve(std::move(costs), assignments))
 * {
 *     // assignments contains optimal (row, column) pairs
 * }
 * @endcode
 *
 * @ingroup cvdetectorbullseyes
 */
class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT AssignmentSolver
{
	public:

		/// The type of the cost matrix.
		using CostMatrix = Matrix;

		/// An alias for a vector of index pairs.
		using Assignments = IndexPairs32;

	public:

		/**
		 * Solves the assignment problem for the given cost matrix using the Hungarian/Kuhn-Munkres algorithm.
		 * The algorithm finds an optimal assignment that minimizes the total cost. The cost matrix can be rectangular.
		 * For an NxM matrix, the function will assign min(N,M) pairs such that the sum of their costs is minimized.
		 * @param costMatrix The cost matrix with non-negative values, can be rectangular, will be moved/modified during solving
		 * @param assignments The resulting optimal assignments as pairs of (row, column) indices
		 * @return True if the algorithm succeeded; False if the input matrix is empty or invalid
		 */
		static bool solve(CostMatrix&& costMatrix, Assignments& assignments);

	protected:

		/**
		 * Converts a rectangular cost matrix to a square matrix by padding with a large fill value.
		 * This is necessary because the Hungarian algorithm requires a square matrix.
		 * The padding ensures that padded rows/columns won't be selected in the optimal assignment.
		 * @param costMatrix The original cost matrix, can be rectangular, must be valid
		 * @param fillValue The value to use for padding, with range (-infinity, infinity); negative values trigger automatic selection of an appropriate large value
		 * @return The square cost matrix with size max(rows, columns)
		 */
		static CostMatrix convertToSquareCostMatrix(const CostMatrix& costMatrix, Scalar fillValue = Scalar(-1));

		/**
		 * Performs the initial cost reduction by subtracting row and column minima from the cost matrix.
		 * This step of the Hungarian algorithm creates zeros in the matrix which represent potential assignments.
		 * After this operation, each row and column will have at least one zero.
		 * @param costMatrix The cost matrix to be reduced, will be modified in-place
		 */
		static void subtractRowAndColumnMinima(CostMatrix& costMatrix);

		/**
		 * Attempts to find an augmenting path starting from a given row using breadth-first search.
		 * An augmenting path alternates between unassigned edges (zeros in the matrix) and assigned edges,
		 * starting and ending at unassigned rows/columns. Finding such a path allows increasing the matching size by one.
		 * @param costMatrix The square cost matrix, must be valid
		 * @param yStart The row index to start searching from, with range [0, matrixSize)
		 * @param yAssignments Row-to-column assignment mapping, will be updated if an augmenting path is found, with invalidIndex() for unassigned rows
		 * @param xAssignments Column-to-row assignment mapping, will be updated if an augmenting path is found, with invalidIndex() for unassigned columns
		 * @param yVisited Tracks visited rows during the search, will be modified, must have size matrixSize
		 * @param xVisited Tracks visited columns during the search, will be modified, must have size matrixSize
		 * @param yParents Tracks parent relationships for path reconstruction, will be modified, must have size matrixSize
		 * @return True if an augmenting path was found and assignments were updated; False otherwise
		 */
		static bool findAugmentingPath(const CostMatrix& costMatrix, const unsigned int yStart, Indices32& yAssignments, Indices32& xAssignments, std::vector<bool>& yVisited, std::vector<bool>& xVisited, Indices32& yParents);

		/**
		 * Reduces the cost matrix when no complete assignment can be found with current zeros.
		 * This function implements the matrix adjustment step of the Hungarian algorithm:
		 * it identifies the minimum uncovered element and adjusts the matrix to create new zeros
		 * in positions that may lead to a better assignment in the next iteration.
		 * @param yAssignments Current row-to-column assignment mapping, with invalidIndex() for unassigned rows
		 * @param costMatrix The square cost matrix to be reduced, will be modified in-place
		 * @param yMarked Working array for row marking, will be reinitialized and modified, must have size matrixSize
		 * @param xMarked Working array for column marking, will be reinitialized and modified, must have size matrixSize
		 * @return True on success; False if reduction fails (which should never happen for valid inputs)
		 */
		static bool reduceCostMatrix(const Indices32& yAssignments, CostMatrix& costMatrix, std::vector<bool>& yMarked, std::vector<bool>& xMarked);

		/**
		 * Returns an invalid index value used as a sentinel for unassigned rows/columns.
		 * @return The invalid index value (Index32(-1))
		 */
		constexpr static Index32 invalidIndex();
};

constexpr Index32 AssignmentSolver::invalidIndex()
{
	return Index32(-1);
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_ASSIGNMENTSOLVER_H
