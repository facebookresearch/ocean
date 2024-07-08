/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_SPECTRAL_CLUSTERING_H
#define META_OCEAN_MATH_SPECTRAL_CLUSTERING_H

#include "ocean/math/Math.h"
#include "ocean/math/Matrix.h"

namespace Ocean
{

/**
 * This class implements spectral clustering.
 * @ingroup math
 */
class ClusteringSpectral
{
	public:

		/**
		 * Determines the symmetric Laplacian matrix of input given and computes the eigen system of the Laplacian matrix.
		 * @param affinityMatrix  a square weight matrix derived from similarity matrix (must be non-negative)
		 * @param numberCluster number of cluster to be returned (0, infinity)
		 */
		ClusteringSpectral(const Matrix&  affinityMatrix, const unsigned int numberCluster);

		/**
		 * Performances the actual clustering.
		 * The algorithm is implemented according to "Huang et al. Spectral rotation versus K-means in spectral clustering. AAAI'13".
		 * @param iterations Upper limit of iterations to be performed  [1, infinity)
		 * @param convergenceThreshold Differential threshold used as convergence criteria  (0, infinity)
		 * @return Indices of cluster elements (number of cluster was given in constructor)
		 */
		std::vector<Indices32> clusterRotation(const unsigned int iterations = 100u, const Scalar convergenceThreshold = Scalar(0.0001));

	protected:

		/**
		 * Determines unnormalized graph Laplacian matrix.
		 * L = D - W. W is the weight matrix and D is a diagonal matrix containing the row sum of W
		 * @param affinityMatrix  a square weight matrix derived from similarity matrix (must be non-negative)
		 * @return Laplacian matrix
		 */
		static Matrix determineLaplacianMatrix(const Matrix& affinityMatrix);

		/**
		 * Determines normalized graph Laplacian matrix closely connected to a random walk.
		 * L = I - D^{-1} * W. W is the weight matrix, I is the identity matrix and D is a diagonal matrix containing the row sum of W
		 * @tparam tSimplify indicating whether a simplified version is used, then substraction with identity matrix is obmitted. Mathematically wrong but commonly used
		 * @param affinityMatrix  a square weight matrix derived from similarity matrix (must be non-negative)
		 * @return Laplacian matrix
		 */
		template<bool tSimplify>
		static Matrix determineRandomWalkLaplacianMatrix(const Matrix& affinityMatrix);

		/**
		 * Determines symmetric (normalized) graph Laplacian matrix.
		 * L = I - D^{-1/2} * W * D^{-1/2}. W is the weight matrix, I is the identity matrix and D is a diagonal matrix containing the row sum of W
		 * @tparam tSimplify indicating whether a simplified version is used, then substraction with identity matrix is obmitted. Mathematically wrong but commonly used
		 * @param affinityMatrix a square weight matrix derived from similarity matrix (must be non-negative)
		 * @return Laplacian matrix
		 */
		template<bool tSimplify>
		static Matrix determineSymmetricLaplacianMatrix(const Matrix& affinityMatrix);

		/**
		 * Sorts a list of eigenvectors with corresponding eigenvalues in descending order
		 * @param firstElem First pair of eigenvalue and eigenvector
		 * @param secondElem Second pair of eigenvalue and eigenvector
		 * @return first eigenvalue is greater than eigenvalue of second element
		 */
		template <typename T>
		static inline bool pairSortDescending(const std::pair<T, MatrixT<T>>& firstElem, const std::pair<T, MatrixT<T>>& secondElem);

	protected:

		/// Column matrix of eigen vectors ordered by eigen values and reduced to [numberCluster] vectors
		Matrix reducedEigenvectors;
};

template <typename T>
inline bool ClusteringSpectral::pairSortDescending(const std::pair<T, MatrixT<T>>& firstElem, const std::pair<T, MatrixT<T>>& secondElem)
{
	return firstElem.first > secondElem.first;
}

}

#endif //OCEAN_MATH_SPECTRAL_CLUSTERING_H
