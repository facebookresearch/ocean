/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/ClusteringSpectral.h"
#include "ocean/math/Numeric.h"

#include "ocean/base/RandomI.h"

namespace Ocean
{

ClusteringSpectral::ClusteringSpectral(const Matrix& affinityMatrix, const unsigned int numberCluster)
{
	ocean_assert(affinityMatrix.rows() == affinityMatrix.columns());
	ocean_assert(numberCluster > 1u && numberCluster <= affinityMatrix.rows());

	size_t size = affinityMatrix.rows();

	const Matrix laplacianMatrix = determineSymmetricLaplacianMatrix<false>(affinityMatrix);

	Matrix vectors, values;
	if (!laplacianMatrix.eigenSystem(values, vectors))
	{
		ocean_assert(false && "Invalid system!");
		return;
	}

	// sort eigenvalues and get the {numberCluster} first eigenvalues
	vectors.transpose();

	std::vector<std::pair<Scalar, Matrix>> listToSort;

	for (unsigned int i = 0; i < values.rows(); i++)
	{
		listToSort.push_back(std::make_pair(values(i), vectors.row(i)));
	}
	std::sort(listToSort.begin(), listToSort.end(), pairSortDescending<Scalar>);

	reducedEigenvectors = Matrix(numberCluster, size);

	Scalar* data = reducedEigenvectors.data();
	for (unsigned int i = 0; i < numberCluster; i++)
	{
		memcpy(data, listToSort[i].second.data(), size * sizeof(Scalar));
		data += size;
	}
	reducedEigenvectors.transpose();

	// normalize rows of reducedEigenVectors
	for (size_t r = 0; r < size; r++)
	{
		Scalar squaredsum(0);
		for (size_t c = 0; c < numberCluster; c++)
		{
			squaredsum += Numeric::sqr(reducedEigenvectors[r][c]);
		}

		ocean_assert(squaredsum > Numeric::eps());
		// squaredsum is always greater 0

		squaredsum = Scalar(1) / Numeric::sqrt(squaredsum);

		for (size_t c = 0; c < numberCluster; c++)
		{
			reducedEigenvectors[r][c] *= squaredsum;
		}
	}
}

std::vector<Indices32> ClusteringSpectral::clusterRotation(const unsigned int iterations, const Scalar convergenceThreshold)
{
	std::vector<Indices32> clusters;
	size_t numberCluster = reducedEigenvectors.columns();
	size_t size = reducedEigenvectors.rows();

	Matrix matrixGtranspose(numberCluster, size);
	memset(matrixGtranspose.data(), 0, sizeof(Scalar) * matrixGtranspose.elements());

	// random itialize, but unique 1 in each column
	for (size_t c = 0; c < size; c++)
	{
		unsigned int oneIndex = RandomI::random((unsigned int)(matrixGtranspose.rows()) - 1u);
		matrixGtranspose[oneIndex][c] = Scalar(1);
	}

	Matrix matrixR;
	Scalar previousSum(0);
	for (unsigned int iteration = 0; iteration < iterations; iteration++)
	{
		// G is fixed, update R
		Matrix indicatedEigenvektors = matrixGtranspose * reducedEigenvectors;

		Matrix u, v, w;
		if (!indicatedEigenvektors.singularValueDecomposition(u, w, v))
		{
			break;
		}
		matrixR = u * v.transposed();

		ocean_assert(matrixGtranspose.rows() == matrixR.rows()); // G*R ~= reducedEigenvectors

	   // convergence

#ifdef OCEAN_DEBUG
		Matrix debugQ = matrixGtranspose.transposedMultiply(matrixR);
#endif
		Scalar sum = matrixR.sum();

		// R is fixed, update G
		memset(matrixGtranspose.data(), 0, sizeof(Scalar) * matrixGtranspose.elements());

		for (size_t i = 0; i < size; i++)
		{
			size_t minIndex = 0;
			Scalar minValue(Numeric::maxValue());

			for (size_t k = 0; k < numberCluster; k++)
			{
				Scalar diffNorm(0);

				for (size_t n = 0; n < numberCluster; n++)
				{
					diffNorm += Numeric::sqr(reducedEigenvectors[i][n] - matrixR[k][n]);
				}

				if (diffNorm < minValue)
				{
					minValue = diffNorm;
					minIndex = k;
				}
			}

			matrixGtranspose[minIndex][i] = Scalar(1);
		}

		if (Numeric::abs(sum - previousSum) < convergenceThreshold)
		{
			break;
		}

		previousSum = sum;
	}

	clusters.resize(numberCluster);

	for (size_t r = 0; r < numberCluster; r++)
	{
		for (unsigned int c = 0; c < (unsigned int)(size); c++)
		{
			if (matrixGtranspose[r][c] > 0.9)
			{
				clusters[r].push_back(c);
			}
		}
	}

	return clusters;
}

Matrix ClusteringSpectral::determineLaplacianMatrix(const Matrix & affinityMatrix)
{
	// L = D - A
	ocean_assert(affinityMatrix.rows() == affinityMatrix.columns());

	size_t size = affinityMatrix.rows();
	Matrix diagonalVectorInvert(1, size);
	for (size_t i = 0u; i < size; ++i)
	{
		Scalar sum(0);
		for (size_t n = 0; n < size; n++)
		{
			sum += affinityMatrix[i][n];
		}
		diagonalVectorInvert[0][i] = sum;
	}

	Matrix laplacian(size, size);
	for (size_t r = 0; r < size; r++)
	{
		for (size_t c = 0; c < size; c++)
		{
			if (r == c)
			{
				laplacian[r][c] = diagonalVectorInvert[0][r] - affinityMatrix[r][c];
			}
			else
			{
				laplacian[r][c] = -(affinityMatrix[r][c]);
			}
		}
	}

#ifdef OCEAN_DEBUG
	Matrix debugMatrixInvert(size, size);
	memset(debugMatrixInvert.data(), 0, sizeof(Scalar) * debugMatrixInvert.elements());

	for (size_t i = 0u; i < size; ++i)
	{
		Scalar sum(0);
		for (size_t n = 0; n < size; n++)
		{
			sum += affinityMatrix[i][n];
		}
		debugMatrixInvert[i][i] = sum;
	}

	const Matrix laplacianDebug = debugMatrixInvert - affinityMatrix;

	ocean_assert(laplacianDebug == laplacian);
#endif

	return laplacian;
}

template<bool tSimplify>
Matrix ClusteringSpectral::determineRandomWalkLaplacianMatrix(const Matrix & affinityMatrix)
{
	// L = I - D^{-1} * A
	ocean_assert(affinityMatrix.rows() == affinityMatrix.columns());

	size_t size = affinityMatrix.rows();
	Matrix diagonalVectorInvert(1, size);

	for (size_t i = 0u; i < size; ++i)
	{
		Scalar sum(0);
		for (size_t n = 0; n < size; n++)
		{
			sum += affinityMatrix[i][n];
		}

		ocean_assert(sum > Numeric::eps());
		// sum is always greater 0

		diagonalVectorInvert[0][i] = Scalar(1) / sum;
	}

	Matrix laplacian(size, size);
	for (size_t r = 0; r < size; r++)
	{
		for (size_t c = 0; c < size; c++)
		{
			if constexpr (tSimplify)
			{
				laplacian[r][c] = (affinityMatrix[r][c] * diagonalVectorInvert[0][r]);
			}
			else
			{
				if (r == c)
				{
					laplacian[r][c] = 1 - (affinityMatrix[r][c] * diagonalVectorInvert[0][r]);
				}
				else
				{
					laplacian[r][c] = -(affinityMatrix[r][c] * diagonalVectorInvert[0][r]);
				}
			}
		}
	}

#ifdef OCEAN_DEBUG
	Matrix debugMatrixInvert(size, size);
	memset(debugMatrixInvert.data(), 0, sizeof(Scalar) * debugMatrixInvert.elements());

	for (size_t i = 0u; i < size; ++i)
	{
		Scalar sum(0);
		for (size_t n = 0; n < size; n++)
		{
			sum += affinityMatrix[i][n];
		}

		ocean_assert(sum > 0);
		// sum is always greater 0

		debugMatrixInvert[i][i] = Scalar(1) / sum;
	}

	Matrix laplacianDebug = (debugMatrixInvert * affinityMatrix);
	if (!tSimplify)
	{
		const Matrix identyMatrix(size, size, true);

		laplacianDebug = identyMatrix - laplacianDebug;
	}
	ocean_assert(laplacianDebug == laplacian);
#endif

	return laplacian;
}

template<bool tSimplify>
Matrix ClusteringSpectral::determineSymmetricLaplacianMatrix(const Matrix& affinityMatrix)
{
	// L = I - D^{-0.5} * A * D^{-0.5}
	ocean_assert(affinityMatrix.rows() == affinityMatrix.columns());

	size_t size = affinityMatrix.rows();

	Matrix diagonalVectorInvert(1, size);
	for (size_t i = 0u; i < size; ++i)
	{
		Scalar sum(0);
		for (size_t n = 0; n < size; n++)
		{
			sum += affinityMatrix[i][n];
		}

		ocean_assert(sum > Numeric::eps());
		// sum is always greater 0

		diagonalVectorInvert[0][i] = Scalar(1) / Numeric::sqrt(sum);
	}

	Matrix laplacian(size, size);
	for (size_t r = 0; r < size; r++)
	{
		for (size_t c = 0; c < size; c++)
		{
			if constexpr (tSimplify)
			{
				laplacian[r][c] = (affinityMatrix[r][c] * diagonalVectorInvert[0][r] * diagonalVectorInvert[0][c]);
			}
			else
			{
				if (r == c)
				{
					laplacian[r][c] = 1 - (affinityMatrix[r][c] * diagonalVectorInvert[0][r] * diagonalVectorInvert[0][c]);
				}
				else
				{
					laplacian[r][c] = -(affinityMatrix[r][c] * diagonalVectorInvert[0][r] * diagonalVectorInvert[0][c]);
				}
			}
		}
	}

#ifdef OCEAN_DEBUG
	Matrix denseMatrixInvert(size, size);
	memset(denseMatrixInvert.data(), 0, sizeof(Scalar) * denseMatrixInvert.elements());

	for (size_t i = 0u; i < size; ++i)
	{
		Scalar sum(0);
		for (size_t n = 0; n < size; n++)
		{
			sum += affinityMatrix[i][n];
		}

		ocean_assert(sum > Numeric::eps());
		// sum is always greater 0

		denseMatrixInvert[i][i] = Scalar(1) / Numeric::sqrt(sum);
	}

	Matrix laplacianDebug = (denseMatrixInvert * affinityMatrix * denseMatrixInvert);

	if (!tSimplify)
	{
		const Matrix identyMatrix(size, size, true);

		laplacianDebug = identyMatrix - laplacianDebug;
	}
	ocean_assert(laplacianDebug == laplacian);
#endif

	return laplacian;
}

}
