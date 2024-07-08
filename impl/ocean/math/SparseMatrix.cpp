/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/SparseMatrix.h"
#include "ocean/math/Numeric.h"

#include "ocean/math/Matrix.h"
#include "ocean/math/SquareMatrix3.h"

#if defined(OCEAN_COMPILER_MSC)
	#pragma warning(disable:4244)
#elif defined(OCEAN_COMPILER_CLANG)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wconversion"
#endif

#include <Eigen/Sparse>
#include <Eigen/Dense>

#if defined(OCEAN_COMPILER_MSC)
	#pragma warning(default:4244)
#elif defined(OCEAN_COMPILER_CLANG)
	#pragma clang diagnostic pop
#endif

#include <set>
#include <vector>

namespace Ocean
{

namespace
{

/**
 * Performs a element-wise matrix division
 * matrixA ./ (denominatorA * denominatorB) where denominatorA.row(.) * denominatorB.col(.) is only calculated for elements present in sparse matrix
 * a helper for non-negative matrix factorization
 * @param matrixA sparse input matrix as numerator (Eigen::SparseMatrix)
 * @param denominatorA dense input matrix as denominator
 * @param denominatorB dense input matrix as denominator
 * @return a sparse matrix: matrixA ./ (denominatorA * denominatorB)
 */
template <typename T>
SparseMatrixT<T> elementDivision(const Eigen::SparseMatrix<T, Eigen::ColMajor>& matrixA, const MatrixT<T>& denominatorA, const MatrixT<T>& denominatorB)
{
	ocean_assert(matrixA.rows() == denominatorA.rows() && matrixA.cols() == denominatorB.columns());

	const MatrixT<T> bTranspose = denominatorB.transposed(); //we want to access single columns from denominatorB, so it will be transposed and accessed by row(.)

	SparseMatrixT<T> result(matrixA.rows(), matrixA.rows());

	// visit only non-zero elements
	for (auto c = 0; c < matrixA.outerSize(); ++c)
	{
		for (typename Eigen::SparseMatrix<T>::InnerIterator it(matrixA, c); it; ++it)
		{
			ocean_assert(c == it.col());

			const auto r = it.row();
			const T v = it.value();

			result(r, c) = NumericT<T>::ratio(v, denominatorA.row(r) * bTranspose.row(c));
		}
	}

	return result;
}

/**
 * Performs a element-wise matrix division
 * matrixA ./ (denominatorA * denominatorB) where denominatorA.row(.) * denominatorB.col(.) is only calculated for elements present in sparse matrix
 * a helper for non-negative matrix factorization
 * @param matrixA sparse input matrix as numerator (Eigen::SparseMatrix)
 * @param denominatorA dense input matrix as denominator (Eigen::Matrix)
 * @param denominatorB dense input matrix as denominator (Eigen::Matrix)
 * @return a sparse matrix: matrixA ./ (denominatorA * denominatorB)
 */
template <typename T>
SparseMatrixT<T> elementDivision(const Eigen::SparseMatrix<T, Eigen::ColMajor>& matrixA, const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& denominatorA, const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& denominatorB)
{
	ocean_assert(matrixA.rows() == denominatorA.rows() && matrixA.cols() == denominatorB.cols());

	SparseMatrixT<T> result(matrixA.rows(), matrixA.cols());

	// visit only non-zero elements
	for (auto c = 0; c < matrixA.outerSize(); ++c)
	{
		for (typename Eigen::SparseMatrix<T>::InnerIterator it(matrixA, c); it; ++it)
		{
			ocean_assert(c == it.col());

			const auto r = it.row();
			const T v = it.value();

			result(r, c) = NumericT<T>::ratio(v, denominatorA.row(r) * denominatorB.col(c));
		}
	}

	return result;
}

/**
 * Multiplies a sparse matrix with a second transposed dense matrix.
 * Beware: missing elements in sparse matrix will be interpreted as 1.<br>
 * The resulting matrix will have the size: sparse.rows() x dense.rows().
 * a helper for non-negative matrix factorization
 * @param sparseMatrix Sparse matrix (Eigen::SparseMatrix)
 * @param denseMatrix Dense matrix to multiply (Eigen::Matrix)
 * @return Matrix product (Eigen::Matrix), but non-present elements will be interpreted as 1
 */
template <typename T>
Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> matrixMultiplicationTranspose(const Eigen::SparseMatrix<T, Eigen::ColMajor>& sparseMatrix, const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& denseMatrix)
{
	ocean_assert(sparseMatrix.cols() == denseMatrix.cols());

	// **TODO** performance improvement

	typedef typename Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> DenseMatrix;
	typedef typename Eigen::SparseMatrix<T, Eigen::ColMajor> InternalSparseMatrix;

	DenseMatrix result(sparseMatrix.rows(), denseMatrix.rows());
	memset(result.data(), 0, sizeof(T) * result.rows() * result.cols());

	const typename InternalSparseMatrix::Index rEnd = sparseMatrix.rows();

	const typename DenseMatrix::Index cResultEnd = result.cols();
	ocean_assert(cResultEnd == denseMatrix.rows() && cResultEnd == denseMatrix.transpose().cols());

	const typename InternalSparseMatrix::Index cSparseEnd = sparseMatrix.outerSize();
	ocean_assert(cSparseEnd == sparseMatrix.cols() && cSparseEnd == denseMatrix.cols() && cSparseEnd == denseMatrix.transpose().rows());

	for (typename DenseMatrix::Index cResult = 0; cResult < cResultEnd; ++cResult)
	{
		// visit only non-zero elements, missing data is interpreted as 1
		for (typename InternalSparseMatrix::Index cSparse = 0; cSparse < cSparseEnd; ++cSparse)
		{
			typename InternalSparseMatrix::Index rStart = 0;

			for (typename InternalSparseMatrix::InnerIterator it(sparseMatrix, cSparse); it; ++it)
			{
				typename InternalSparseMatrix::Index r = it.row();

				ocean_assert(r < result.rows());
				ocean_assert(cSparse == it.col());

				while (rStart < r)
				{
					result(rStart++, cResult) += denseMatrix(cResult, cSparse); // missing data is interpreted as 1
				} //== result(rStart++, cResult) *= const unsigned int(r - rStart) * denseMatrix(cResult, cSparse);


				T value = it.value();
				result(r, cResult) += value * denseMatrix(cResult, cSparse);
				rStart = r + 1;
			}

			while (rStart < rEnd)
			{
				result(rStart++, cResult) += denseMatrix(cResult, cSparse); // missing data is interpreted as 1
			}
		}
	}

	return result;
}

/**
 * Multiplies a dense matrix with a second transposed sparse matrix.
 * Beware: missing elements in sparse matrix will be interpreted as 1.<br>
 * The resulting matrix will have the size: dense.columns() x sparse.columns().
 * a helper for non-negative matrix factorization
 * @param sparseMatrix Dense matrix (Eigen::Matrix)
 * @param denseMatrix Sparse matrix to multiply (Eigen::SparseMatrix)
 * @return Matrix product (Eigen::Matrix), but non-present elements will be interpreted as 1
 */
template <typename T>
Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> matrixMultiplicationTranspose(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& denseMatrix, const Eigen::SparseMatrix<T, Eigen::ColMajor>& sparseMatrix)
{
	ocean_assert(denseMatrix.rows() == sparseMatrix.rows());

	// **TODO** performance improvement

	typedef typename Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> DenseMatrix;
	typedef typename Eigen::SparseMatrix<T, Eigen::ColMajor> InternalSparseMatrix;

	DenseMatrix result(denseMatrix.cols(), sparseMatrix.cols());
	memset(result.data(), 0, sizeof(T) * result.rows() * result.cols());

	typename Eigen::SparseMatrix<T>::Index rEnd = sparseMatrix.rows();
	T value;

	typename InternalSparseMatrix::Index cSparseEnd = sparseMatrix.outerSize();
	ocean_assert(cSparseEnd == result.cols());

	const typename DenseMatrix::Index rDenseEnd = result.rows();
	ocean_assert(rDenseEnd == denseMatrix.cols()); // == denseMatrix.transpose().rows());

	// visit only non-zero elements, missing data is interpreted as 1
	for (typename InternalSparseMatrix::Index cSparse = 0; cSparse < cSparseEnd; ++cSparse)
	{
		for (typename DenseMatrix::Index rDense = 0; rDense < rDenseEnd; rDense++)
		{
			value = 0;
			typename InternalSparseMatrix::Index rStart = 0;

			for (typename Eigen::SparseMatrix<T>::InnerIterator it(sparseMatrix, cSparse); it; ++it)
			{
				typename Eigen::SparseMatrix<T>::Index rSparse = it.row();

				ocean_assert(rSparse < denseMatrix.rows()); // < denseMatrix.transpose().cols()
				ocean_assert(cSparse == it.col());

				T valueSparse = it.value();

				while (rStart < rSparse)
				{
					value += denseMatrix(rStart++, rDense); // missing data is interpreted as 1
				}

				value += denseMatrix(rSparse, rDense) * valueSparse;
				rStart = rSparse + 1;
			}

			while (rStart < rEnd)
			{
				value += denseMatrix(rStart++, rDense); // missing data is interpreted as 1
			}

			result(rDense, cSparse) = value;
		}
	}

	return result;
}

}

template <typename T>
bool SparseMatrixT<T>::Entry::hasOneEntry(const size_t rows, const size_t columns, const Entries& entries)
{
	ocean_assert(rows >= 1 && columns >= 1);

	std::set<size_t> coveredRows, coveredColumns;

	for (typename Entries::const_iterator i = entries.begin(); i != entries.end(); ++i)
	{
		coveredRows.insert(i->row());
		coveredColumns.insert(i->column());
	}

	return coveredRows.size() == rows && coveredColumns.size() == columns;
}

/**
 * This class defines the sparse Eigen matrix based on the data type to be used.
 * @tparam T The data type of each element of the matrix
 */
template <typename T>
struct InternalMatrix
{
	/**
	 * Definition of the sparse Eigen matrix.
	 */
	typedef Eigen::SparseMatrix<T, Eigen::ColMajor> Type;
};

template <typename T>
SparseMatrixT<T>::SparseMatrixT() :
	internalMatrix(nullptr)
{
	// nothing to do here
}

template <typename T>
SparseMatrixT<T>::SparseMatrixT(const SparseMatrixT<T>& matrix) :
	internalMatrix(nullptr)
{
	if (matrix.internalMatrix)
	{
		internalMatrix = new typename InternalMatrix<T>::Type(*static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix));
		ocean_assert(internalMatrix != nullptr);
	}
}

template <typename T>
SparseMatrixT<T>::SparseMatrixT(SparseMatrixT<T>&& matrix) noexcept :
	internalMatrix(matrix.internalMatrix)
{
	matrix.internalMatrix = nullptr;
}

template <typename T>
SparseMatrixT<T>::SparseMatrixT(const size_t rows, const size_t columns) :
	internalMatrix(nullptr)
{
	internalMatrix = new typename InternalMatrix<T>::Type(int(rows), int(columns));
	ocean_assert(internalMatrix != nullptr);
}

template <typename T>
SparseMatrixT<T>::SparseMatrixT(const size_t rows, const size_t columns, const size_t nonZeroElements) :
	internalMatrix(nullptr)
{
	internalMatrix = new typename InternalMatrix<T>::Type(int(rows), int(columns));
	ocean_assert(internalMatrix != nullptr);

	static_cast<typename InternalMatrix<T>::Type*>(internalMatrix)->reserve(int(nonZeroElements));
}

template <typename T>
SparseMatrixT<T>::SparseMatrixT(const size_t rows, const size_t columns, const Entries& entries) :
	internalMatrix(nullptr)
{
	internalMatrix = new typename InternalMatrix<T>::Type(int(rows), int(columns));
	ocean_assert(internalMatrix != nullptr);

	setEntries(entries);
}

template <typename T>
SparseMatrixT<T>::SparseMatrixT(const size_t rows, const size_t columns, const MatrixT<T>& diagonal, const bool forceNonZeros) :
	internalMatrix(nullptr)
{
	internalMatrix = new typename InternalMatrix<T>::Type(int(rows), int(columns));
	ocean_assert(internalMatrix != nullptr);

	typename InternalMatrix<T>::Type& matrix = *static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);
	matrix.reserve(int(diagonal.elements()));

	typedef typename InternalMatrix<T>::Type::Index Index;

	if (diagonal.columns() == 1u)
	{
		const size_t size = min(rows, min(columns, diagonal.rows()));

		for (size_t n = 0; n < size; n++)
		{
			if (NumericT<T>::isNotEqualEps(diagonal[n][0]))
			{
				matrix.insert(Index(n), Index(n)) = diagonal[n][0];
			}
			else if (forceNonZeros)
			{
				matrix.insert(Index(n), Index(n)) = NumericT<T>::eps() * 10;
			}
		}
	}
	else
	{
		const size_t diagonalSubMatrixSize = diagonal.columns();
		ocean_assert(diagonal.rows() % diagonalSubMatrixSize == 0u);

		const size_t numberSubMatrices = min(diagonal.rows() / diagonalSubMatrixSize, min(rows / diagonalSubMatrixSize, columns / diagonalSubMatrixSize));

		for (size_t n = 0; n < numberSubMatrices; ++n)
		{
			const size_t offset = n * diagonalSubMatrixSize;

			for (size_t sr = 0; sr < diagonalSubMatrixSize; ++sr)
			{
				for (size_t sc = 0; sc < diagonalSubMatrixSize; ++sc)
				{
					if (NumericT<T>::isNotEqualEps(diagonal(offset + sr, sc)))
					{
						matrix.insert(Index(offset + sr), Index(offset + sc)) = diagonal(offset + sr, sc);
					}
					else if (forceNonZeros)
					{
						matrix.insert(Index(offset + sr), Index(offset + sc)) = NumericT<T>::eps() * 10;
					}
				}
			}
		}
	}

	matrix.finalize();
}

template <typename T>
SparseMatrixT<T>::SparseMatrixT(const MatrixT<T>& denseMatrix) :
	internalMatrix(nullptr)
{
	typedef typename InternalMatrix<T>::Type::Index Index;

	size_t nonZeros = 0;

	for (size_t n = 0; n < denseMatrix.elements(); ++n)
	{
		if (denseMatrix(n) != T(0))
		{
			nonZeros++;
		}
	}

	internalMatrix = new typename InternalMatrix<T>::Type(int(denseMatrix.rows()), int(denseMatrix.columns()));
	ocean_assert(internalMatrix != nullptr);

	typename InternalMatrix<T>::Type& matrix = *static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);
	matrix.reserve(int(nonZeros));

	for (size_t c = 0; c < denseMatrix.columns(); ++c)
	{
		matrix.startVec(Index(c));

		for (size_t r = 0; r < denseMatrix.rows(); ++r)
		{
			if (denseMatrix(r, c) != T(0))
			{
				matrix.insertBack(Index(r), Index(c)) = denseMatrix(r, c);
			}
		}
	}

	matrix.finalize();
}

template <typename T>
SparseMatrixT<T>::SparseMatrixT(void* matrix) :
	internalMatrix(matrix)
{
	// nothing to do here
}

template <typename T>
SparseMatrixT<T>::~SparseMatrixT()
{
	delete static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);
}

template <typename T>
size_t SparseMatrixT<T>::rows() const
{
	ocean_assert(internalMatrix);
	return (size_t)(static_cast<typename InternalMatrix<T>::Type*>(internalMatrix)->rows());
}

template <typename T>
size_t SparseMatrixT<T>::columns() const
{
	ocean_assert(internalMatrix);
	return (size_t)(static_cast<typename InternalMatrix<T>::Type*>(internalMatrix)->cols());
}

template <typename T>
size_t SparseMatrixT<T>::nonZeroElements() const
{
	ocean_assert(internalMatrix);
	return static_cast<typename InternalMatrix<T>::Type*>(internalMatrix)->nonZeros();
}

template <typename T>
MatrixT<T> SparseMatrixT<T>::diagonal() const
{
	ocean_assert(internalMatrix);

	const typename InternalMatrix<T>::Type& matrix = *static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);

	size_t size = min(matrix.rows(), matrix.cols());

	MatrixT<T> result(size, 1);

	for (size_t n = 0; n < size; n++)
	{
		result(n, 0) = matrix.coeff(typename InternalMatrix<T>::Type::Index(n), typename InternalMatrix<T>::Type::Index(n));
	}

	return result;
}

template <typename T>
void SparseMatrixT<T>::reserve(const size_t elements)
{
	ocean_assert(internalMatrix);
	static_cast<typename InternalMatrix<T>::Type*>(internalMatrix)->reserve(int(elements));
}

template <typename T>
bool SparseMatrixT<T>::isZero(const size_t row, const size_t column) const
{
	ocean_assert(internalMatrix);
	ocean_assert(row < this->rows());
	ocean_assert(column < this->columns());

	return static_cast<typename InternalMatrix<T>::Type*>(internalMatrix)->coeff(typename InternalMatrix<T>::Type::Index(row), typename InternalMatrix<T>::Type::Index(column)) == T(0);
}

template <typename T>
bool SparseMatrixT<T>::isEqual(const SparseMatrixT<T>& matrix, const T eps) const
{
	if (internalMatrix == matrix.internalMatrix)
		return true;

	if (internalMatrix && matrix.internalMatrix)
	{
		const typename InternalMatrix<T>::Type& matrix0 = *static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);
		const typename InternalMatrix<T>::Type& matrix1 = *static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix);

		if (matrix0.rows() != matrix1.rows() || matrix0.cols() != matrix1.cols())
			return false;

		for (typename InternalMatrix<T>::Type::Index r = 0; r < matrix0.rows(); ++r)
		{
			for (typename InternalMatrix<T>::Type::Index c = 0; c < matrix0.cols(); ++c)
			{
				if (NumericT<T>::isNotEqual(matrix0.coeff(r, c), matrix1.coeff(r, c), eps))
				{
					return false;
				}
			}
		}

		return true;
	}

	if (internalMatrix)
	{
		const typename InternalMatrix<T>::Type& matrix0 = *static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);

		return matrix0.rows() == 0 || matrix0.cols() == 0;
	}

	if (matrix.internalMatrix)
	{
		const typename InternalMatrix<T>::Type& matrix1 = *static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix);

		return matrix1.rows() == 0 || matrix1.cols() == 0;
	}

	return true;
}

template <typename T>
bool SparseMatrixT<T>::isEqual(const MatrixT<T>& matrix, const T eps) const
{
	ocean_assert(internalMatrix);
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> sparseAsDense(*static_cast<typename InternalMatrix<T>::Type*>(internalMatrix));

	ocean_assert(size_t(sparseAsDense.rows()) == matrix.rows());
	ocean_assert(size_t(sparseAsDense.cols()) == matrix.columns());

	for (size_t r = 0; r < size_t(sparseAsDense.rows()); ++r)
		for (size_t c = 0; c < size_t(sparseAsDense.cols()); ++c)
			if (NumericT<T>::isNotEqual(sparseAsDense(r, c), matrix(r, c), eps))
				return false;

	return true;
}

template <typename T>
void SparseMatrixT<T>::setEntries(const Entries& entries)
{
	ocean_assert(internalMatrix);

	typedef typename InternalMatrix<T>::Type::Index Index;
	typedef Eigen::Triplet<T, Index> Triplet;
	typedef std::vector<Triplet> Triplets;

	ocean_assert(std::set<Entry>(entries.begin(), entries.end()).size() == entries.size());

	Triplets triplets;
	triplets.reserve(entries.size());
	for (typename Entries::const_iterator i = entries.begin(); i != entries.end(); ++i)
	{
		ocean_assert(i->isValid() && i->row() < rows() && i->column() < columns());

		// we only add non-zero values
		if (NumericT<T>::isNotEqualEps(i->value()))
			triplets.push_back(Triplet(Index(i->row()), Index(i->column()), i->value()));
	}

	typename InternalMatrix<T>::Type& matrix = *static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);
	matrix.setFromTriplets(triplets.begin(), triplets.end());

	// the result is a sorted and compressed sparse matrix
}

template <typename T>
SparseMatrixT<T> SparseMatrixT<T>::submatrix(const size_t row, const size_t column, const size_t rows, const size_t columns) const
{
	ocean_assert(internalMatrix);
	ocean_assert(row < this->rows());
	ocean_assert(column < this->columns());
	ocean_assert(row + rows <= this->rows());
	ocean_assert(column + columns <= this->columns());

	typedef typename InternalMatrix<T>::Type::Index Index;

	return SparseMatrixT<T>(new typename InternalMatrix<T>::Type(static_cast<typename InternalMatrix<T>::Type*>(internalMatrix)->block(Index(row), Index(column), Index(rows), Index(columns))));
}

template <typename T>
SparseMatrixT<T> SparseMatrixT<T>::transposed() const
{
	ocean_assert(internalMatrix);

	return SparseMatrixT<T>(new typename InternalMatrix<T>::Type(static_cast<typename InternalMatrix<T>::Type*>(internalMatrix)->transpose()));
}

template <typename T>
void SparseMatrixT<T>::transpose()
{
	*this = transposed();
}

template <typename T>
bool SparseMatrixT<T>::solve(const MatrixT<T>& b, MatrixT<T>& x) const
{
	ocean_assert(b.rows() > 0 && b.columns() == 1);
	ocean_assert(internalMatrix);

	const typename InternalMatrix<T>::Type& sparseMatrix = *static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);

	Eigen::SparseLU<typename InternalMatrix<T>::Type> solver;
	solver.compute(sparseMatrix);

	if (solver.info() != Eigen::Success)
		return false;

	Eigen::Matrix<T, Eigen::Dynamic, 1, Eigen::ColMajor> bVector(b.rows());
	memcpy(bVector.data(), b.data(), sizeof(T) * b.elements());

	const Eigen::Matrix<T, Eigen::Dynamic, 1, Eigen::ColMajor> xVector(solver.solve(bVector));

	// check whether the solving failed
	if (solver.info() != Eigen::Success)
		return false;

	x = MatrixT<T>((size_t)xVector.rows(), 1, xVector.data());

#ifdef OCEAN_INTENSIVE_DEBUG

	const MatrixT<T> result(*this * x);
	ocean_assert(result.rows() == x.rows());
	ocean_assert(result.columns() == 1);
	ocean_assert(x.columns() == 1);

	std::vector<T> difference(x.rows());

	for (unsigned int n = 0u; n < x.rows(); ++n)
		difference[n] = result(n, 0) - b(n, 0);

#endif // OCEAN_DEBUG

	return true;
}

template <typename T>
size_t SparseMatrixT<T>::rank() const
{
	ocean_assert(rows() > 0 && columns() > 0);
	ocean_assert(internalMatrix);

	static_cast<typename InternalMatrix<T>::Type*>(internalMatrix)->makeCompressed();
	const typename InternalMatrix<T>::Type& sparseMatrix = *(static_cast<typename InternalMatrix<T>::Type*>(internalMatrix));

	Eigen::SparseQR<typename InternalMatrix<T>::Type, Eigen::COLAMDOrdering<int>> solver;
	solver.compute(sparseMatrix);

	if (solver.info() != Eigen::Success)
	{
		ocean_assert(false && "Rank determination failed!");
		return 0;
	}

	return size_t(solver.rank());
}

template <typename T>
inline T SparseMatrixT<T>::sum() const
{
	const typename InternalMatrix<T>::Type& sparseMatrix = *(static_cast<typename InternalMatrix<T>::Type*>(internalMatrix));

	return T(sparseMatrix.sum());
}

template <typename T>
bool SparseMatrixT<T>::invertDiagonal()
{
	/**
	 * We have the following matrix form:
	 *
	 * | x             |
	 * |   x           |
	 * |     x         |
	 * |       .       |
	 * |         .     |
	 * |           x   |
	 * |             x |
	 *
	 * Thus we can invert each diagonal element individually.
	 */

	typename InternalMatrix<T>::Type& sparseMatrix = *static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);

	ocean_assert(sparseMatrix.rows() == sparseMatrix.cols());

	if (sparseMatrix.rows() != sparseMatrix.cols())
		return false;

#ifdef OCEAN_INTENSIVE_DEBUG
	for (int r = 0; r < sparseMatrix.rows(); ++r)
		for (int c = 0; c < sparseMatrix.cols(); ++c)
		{
			const T value = sparseMatrix.coeff(r, c);

			if (r != c)
				ocean_assert(NumericT<T>::isEqualEps(value));
		}

	const typename InternalMatrix<T>::Type copyMatrix(sparseMatrix);
	typename InternalMatrix<T>::Type identityMatrix(sparseMatrix.rows(), sparseMatrix.cols());
	identityMatrix.setIdentity();

#endif

	for (typename InternalMatrix<T>::Type::Index r = 0; r < sparseMatrix.rows(); ++r)
		for (typename InternalMatrix<T>::Type::Index c = 0; c < sparseMatrix.cols(); ++c)
		{
			T& value = sparseMatrix.coeffRef(r, c);

			if (NumericT<T>::isEqualEps(value))
				return false;

			value = 1 / value;
		}

#ifdef OCEAN_INTENSIVE_DEBUG
	ocean_assert(SparseMatrixT<T>((void*)&copyMatrix) * SparseMatrixT<T>((void*)&sparseMatrix) == SparseMatrixT<T>((void*)&identityMatrix));
#endif

	return true;
}

template <typename T>
bool SparseMatrixT<T>::invertBlockDiagonal3()
{
	/**
	 * We have the following matrix form:
	 *
	 * | x x x               |
	 * | x x x               |
	 * | x x x               |
	 * |       x x x         |
	 * |       x x x         |
	 * |       x x x         |
	 * |             .       |
	 * |               x x x |
	 * |               x x x |
	 * |               x x x |
	 *
	 * Thus we can invert each 3x3 block matrix individually.
	 */

	typename InternalMatrix<T>::Type& sparseMatrix = *static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);

	ocean_assert(sparseMatrix.rows() == sparseMatrix.cols());
	ocean_assert(sparseMatrix.rows() % 3 == 0);

	if (sparseMatrix.rows() != sparseMatrix.cols() || sparseMatrix.rows() % 3 != 0)
		return false;

#ifdef OCEAN_INTENSIVE_DEBUG

	for (int r = 0; r < sparseMatrix.rows(); ++r)
	{
		const int rBlock = (r / 3) * 3;

		for (int c = 0; c < sparseMatrix.cols(); ++c)
		{
			const T value = sparseMatrix.coeff(int(r), int(c));

			if (c < rBlock || c > rBlock + 2)
				ocean_assert(NumericT<T>::isEqualEps(value));
		}
	}

	const SparseMatrixT<T> copyMatrix(new typename InternalMatrix<T>::Type(sparseMatrix));

#endif

	SquareMatrixT3<T> blockMatrix;

	for (typename InternalMatrix<T>::Type::Index n = 0; n < sparseMatrix.rows(); n += 3)
	{
		blockMatrix(0, 0) = sparseMatrix.coeff(n + 0, n + 0);
		blockMatrix(0, 1) = sparseMatrix.coeff(n + 0, n + 1);
		blockMatrix(0, 2) = sparseMatrix.coeff(n + 0, n + 2);

		blockMatrix(1, 0) = sparseMatrix.coeff(n + 1, n + 0);
		blockMatrix(1, 1) = sparseMatrix.coeff(n + 1, n + 1);
		blockMatrix(1, 2) = sparseMatrix.coeff(n + 1, n + 2);

		blockMatrix(2, 0) = sparseMatrix.coeff(n + 2, n + 0);
		blockMatrix(2, 1) = sparseMatrix.coeff(n + 2, n + 1);
		blockMatrix(2, 2) = sparseMatrix.coeff(n + 2, n + 2);

		if (!blockMatrix.invert())
		{
			// we could now apply a matrix invert function not using the determinant to try whether we can find an inverse
			// however, due to stability reasons of the entire system we will not do this (as it seems to be quite instable)
			return false;
		}

		sparseMatrix.coeffRef(n + 0, n + 0) = blockMatrix(0, 0);
		sparseMatrix.coeffRef(n + 0, n + 1) = blockMatrix(0, 1);
		sparseMatrix.coeffRef(n + 0, n + 2) = blockMatrix(0, 2);

		sparseMatrix.coeffRef(n + 1, n + 0) = blockMatrix(1, 0);
		sparseMatrix.coeffRef(n + 1, n + 1) = blockMatrix(1, 1);
		sparseMatrix.coeffRef(n + 1, n + 2) = blockMatrix(1, 2);

		sparseMatrix.coeffRef(n + 2, n + 0) = blockMatrix(2, 0);
		sparseMatrix.coeffRef(n + 2, n + 1) = blockMatrix(2, 1);
		sparseMatrix.coeffRef(n + 2, n + 2) = blockMatrix(2, 2);
	}

#ifdef OCEAN_INTENSIVE_DEBUG

	const SparseMatrixT<T> result(new typename InternalMatrix<T>::Type(sparseMatrix));
	const SparseMatrixT<T> test(copyMatrix * result);

	T totalDifference = T(0);
	size_t totalDifferenceElements = 0;

	for (size_t r = 0u; r < test.rows(); ++r)
		for (size_t c = 0u; c < test.columns(); ++c)
		{
			if (c == r)
			{
				totalDifference += NumericT<T>::abs(test(r, c) - T(1));
				totalDifferenceElements++;
			}
			else if (!test.isZero(r, c))
			{
				totalDifference += NumericT<T>::abs(test(r, c));
				totalDifferenceElements++;
			}
		}

	if (totalDifferenceElements != 0)
		totalDifference /= T(totalDifferenceElements);

	if (NumericT<T>::isNotWeakEqualEps(totalDifference))
	{
		// we determine the average maximal (absolute) value of the matrix (e.g., the energy of the matrix)

		T absolusteAverageEnergy = 0;
		size_t absolusteAverageEnergyElements = 0;

		for (size_t r = 0u; r < test.rows(); ++r)
			for (size_t c = 0u; c < test.columns(); ++c)
			{
				if (!copyMatrix.isZero(r, c))
				{
					absolusteAverageEnergy += NumericT<T>::abs(copyMatrix(r, c));
					absolusteAverageEnergyElements++;
				}
			}

		if (absolusteAverageEnergyElements != 0)
			absolusteAverageEnergy /= T(absolusteAverageEnergyElements);

		// we expect/accept for each magnitude (larger than 1) a zero-inaccuracy of one magnitude and we take the weak eps

		if (absolusteAverageEnergy <= 1)
			ocean_assert(false && "This should never happen!");
		else
		{
			const T adjustedDistance = totalDifference / absolusteAverageEnergy;
			ocean_assert(NumericT<T>::isWeakEqualEps(adjustedDistance));
		}
	}

#endif

	return true;
}

template <typename T>
bool SparseMatrixT<T>::invertBlockDiagonal(const size_t size)
{
	ocean_assert(size >= 2);

	typedef typename InternalMatrix<T>::Type::Index Index;

	typename InternalMatrix<T>::Type& sparseMatrix = *static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);

	ocean_assert(sparseMatrix.rows() == sparseMatrix.cols());
	ocean_assert(sparseMatrix.rows() % size == 0);

	if (sparseMatrix.rows() != sparseMatrix.cols() || sparseMatrix.rows() % size != 0)
		return false;

#ifdef OCEAN_INTENSIVE_DEBUG
	for (int r = 0; r < sparseMatrix.rows(); ++r)
	{
		const int rBlock = (r / int(size)) * int(size);

		for (int c = 0; c < sparseMatrix.cols(); ++c)
		{
			const T value = sparseMatrix.coeff(int(r), int(c));

			if (c < rBlock || c > rBlock + int(size) - 1)
				ocean_assert(NumericT<T>::isEqualEps(value));
		}
	}

	const SparseMatrixT<T> copyMatrix(new typename InternalMatrix<T>::Type(sparseMatrix));

	typename InternalMatrix<T>::Type iMatrix(sparseMatrix.rows(), sparseMatrix.cols());
	iMatrix.setIdentity();

	const SparseMatrixT<T> identityMatrix(new typename InternalMatrix<T>::Type(iMatrix));
#endif

	MatrixT<T> blockMatrix(size, size);

	for (Index n = 0; n < sparseMatrix.rows(); n += Index(size))
	{
		for (Index r = 0; r < Index(size); ++r)
		{
			for (Index c = 0; c < Index(size); ++c)
			{
				blockMatrix(r, c) = sparseMatrix.coeff(n + r, n + c);
			}
		}

		if (!blockMatrix.invert())
		{
			return false;
		}

		for (Index r = 0; r < Index(size); ++r)
		{
			for (Index c = 0; c < Index(size); ++c)
			{
				sparseMatrix.coeffRef(n + r, n + c) = blockMatrix(r, c);
			}
		}
	}

#ifdef OCEAN_INTENSIVE_DEBUG

	const SparseMatrixT<T> result(new typename InternalMatrix<T>::Type(sparseMatrix));
	ocean_assert((copyMatrix * result).isEqual(identityMatrix, NumericT<T>::weakEps()));

#endif

	return true;
}

template <typename T>
MatrixT<T> SparseMatrixT<T>::denseMatrix() const
{
	MatrixT<T> result(rows(), columns());

	ocean_assert(internalMatrix);

	const typename InternalMatrix<T>::Type* matrix = static_cast<const typename InternalMatrix<T>::Type*>(internalMatrix);

	for (size_t r = 0u; r < rows(); ++r)
	{
		for (size_t c = 0u; c < columns(); ++c)
		{
			result(r, c) = matrix->coeff(typename InternalMatrix<T>::Type::Index(r), typename InternalMatrix<T>::Type::Index(c));
		}
	}

	return result;
}

template <typename T>
bool SparseMatrixT<T>::nonNegativeMatrixFactorization(MatrixT<T>& subcomponents, MatrixT<T>& weights, const unsigned int components, const unsigned int iterations, const T convergenceThreshold) const
{
	ocean_assert(rows() > 0u && columns() > 0u);
	ocean_assert(components < std::min(rows(), columns()) + 1u);
	ocean_assert(iterations > 0u);
	ocean_assert(convergenceThreshold > Numeric::eps());

	const size_t r = (components == 0) ? rank() : components;

	// initialize matrices with positive random values
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> matrixGain(rows(), r);
	matrixGain.setRandom();
	matrixGain = matrixGain.array().abs();

	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> matrixSpectra(r, columns());
	matrixSpectra.setRandom();
	matrixSpectra = matrixSpectra.array().abs();

	const Eigen::SparseMatrix<T, Eigen::ColMajor>& sparseMatrix = *(static_cast<typename InternalMatrix<T>::Type*>(internalMatrix));

	T previousSum(0);

	// Run the update rules until iteration or convergence
	for (unsigned int iteration = 0u; iteration < iterations; ++iteration)
	{
		// update subcomponents
		Eigen::SparseMatrix<T, Eigen::ColMajor> r1(*(static_cast<typename InternalMatrix<T>::Type*>(elementDivision(sparseMatrix, matrixGain, matrixSpectra).internalMatrix)));
		Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> rTranspose = matrixMultiplicationTranspose(r1, matrixSpectra); // missing data in r1 should be interpreted as 1

#ifdef OCEAN_DEBUG
		MatrixT<T> r1TransposedDebug(rTranspose.rows(), rTranspose.cols(), rTranspose.data());
#endif
		matrixGain = matrixGain.cwiseProduct(rTranspose);

		// normalize matrixGain using matrixSpectra
		MatrixT<T> invertedSum(1, r);
		for (size_t i = 0; i < r; ++i)
		{
			const T sumrow = matrixSpectra.row(i).sum();
			ocean_assert(sumrow > 0);

			invertedSum[0][i] = T(1) / sumrow;
		}

		size_t rows = matrixGain.rows();
		for (size_t row = 0; row < rows; row++)
		{
			for (size_t col = 0; col < r; col++)
			{
				matrixGain(row, col) *= invertedSum[0][col];
			}
		}

		// update weights
		Eigen::SparseMatrix<T, Eigen::ColMajor> r2(*(static_cast<typename InternalMatrix<T>::Type*>(elementDivision(sparseMatrix, matrixGain, matrixSpectra).internalMatrix)));
		Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> r2Transpose = matrixMultiplicationTranspose(matrixGain, r2); // missing data in r2 should be interpreted as 1

#ifdef OCEAN_DEBUG
		MatrixT<T> r2TransposedDebug(r2Transpose.rows(), r2Transpose.cols(), r2Transpose.data());
#endif
		matrixSpectra = matrixSpectra.cwiseProduct(r2Transpose);

		// normalize matrixSpectra using matrixGain
		for (size_t i = 0; i < r; ++i)
		{
			const T sumcol = matrixGain.col(i).sum();
			ocean_assert(sumcol > 0);

			invertedSum[0][i] = T(1) / sumcol;
		}

		size_t columns = matrixSpectra.cols();
		for (size_t row = 0; row < r; row++)
		{
			for (size_t col = 0; col < columns; col++)
			{
				matrixSpectra(row, col) *= invertedSum[0][row];
			}
		}

		// convergence criteria: r1 is a one matrix, if ||(this matrix) - (subcomponents * weights)||^2 is zero
		const T sum = r2.sum();

		if (NumericT<T>::abs(sum - previousSum) < convergenceThreshold)
			break;

		previousSum = sum;
	}

	// pass to output matrices
	subcomponents = MatrixT<T>(rows(), r, matrixGain.data());
	weights = MatrixT<T>(r, columns(), matrixSpectra.data());

	return true;
}

template <typename T>
SparseMatrixT<T>& SparseMatrixT<T>::operator=(const SparseMatrixT<T>& matrix)
{
	ocean_assert(this != &matrix);

	delete static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);
	internalMatrix = nullptr;

	if (matrix.internalMatrix)
	{
		internalMatrix = new typename InternalMatrix<T>::Type(*static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix));
		ocean_assert(internalMatrix != nullptr);
	}

	return *this;
}

template <typename T>
SparseMatrixT<T>& SparseMatrixT<T>::operator=(SparseMatrixT<T>&& matrix) noexcept
{
	if (this != &matrix)
	{
		if (internalMatrix)
		{
			delete static_cast<typename InternalMatrix<T>::Type*>(internalMatrix);
			internalMatrix = nullptr;
		}

		internalMatrix = matrix.internalMatrix;
		matrix.internalMatrix = nullptr;
	}

	return *this;
}

template <typename T>
SparseMatrixT<T> SparseMatrixT<T>::operator*(const SparseMatrixT<T>& matrix) const
{
	ocean_assert(internalMatrix);
	ocean_assert(matrix.internalMatrix);

	ocean_assert(columns() == size_t((*static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)).rows()));

	return SparseMatrixT<T>(new typename InternalMatrix<T>::Type(*static_cast<typename InternalMatrix<T>::Type*>(internalMatrix) * *static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)));
}

template <typename T>
SparseMatrixT<T> SparseMatrixT<T>::operator+(const SparseMatrixT<T>& matrix) const
{
	ocean_assert(internalMatrix);
	ocean_assert(matrix.internalMatrix);

	ocean_assert(rows() == size_t((*static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)).rows()));
	ocean_assert(columns() == size_t((*static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)).cols()));

	return SparseMatrixT<T>(new typename InternalMatrix<T>::Type(*static_cast<typename InternalMatrix<T>::Type*>(internalMatrix) + *static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)));
}

template <typename T>
SparseMatrixT<T>& SparseMatrixT<T>::operator+=(const SparseMatrixT<T>& matrix)
{
	ocean_assert(internalMatrix);
	ocean_assert(matrix.internalMatrix);

	ocean_assert(rows() == size_t((*static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)).rows()));
	ocean_assert(columns() == size_t((*static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)).cols()));

	*static_cast<typename InternalMatrix<T>::Type*>(internalMatrix) += *static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix);

	return *this;
}

template <typename T>
SparseMatrixT<T> SparseMatrixT<T>::operator-(const SparseMatrixT<T>& matrix) const
{
	ocean_assert(internalMatrix);
	ocean_assert(matrix.internalMatrix);

	ocean_assert(rows() == size_t((*static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)).rows()));
	ocean_assert(columns() == size_t((*static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)).cols()));

	return SparseMatrixT<T>(new typename InternalMatrix<T>::Type(*static_cast<typename InternalMatrix<T>::Type*>(internalMatrix) - *static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)));
}

template <typename T>
SparseMatrixT<T>& SparseMatrixT<T>::operator-=(const SparseMatrixT<T>& matrix)
{
	ocean_assert(internalMatrix);
	ocean_assert(matrix.internalMatrix);

	ocean_assert(rows() == size_t((*static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)).rows()));
	ocean_assert(columns() == size_t((*static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix)).cols()));

	*static_cast<typename InternalMatrix<T>::Type*>(internalMatrix) -= *static_cast<typename InternalMatrix<T>::Type*>(matrix.internalMatrix);

	return *this;
}

template <typename T>
MatrixT<T> SparseMatrixT<T>::operator*(const MatrixT<T>& matrix) const
{
	ocean_assert(internalMatrix);

	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> denseMatrix(matrix.rows(), matrix.columns());
	memcpy(denseMatrix.data(), matrix.data(), sizeof(T) * matrix.elements());

	const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> resultMatrix(*static_cast<typename InternalMatrix<T>::Type*>(internalMatrix) * denseMatrix);
	return MatrixT<T>((size_t)resultMatrix.rows(), (size_t)resultMatrix.cols(), resultMatrix.data());
}

template <typename T>
T SparseMatrixT<T>::operator()(const size_t row, const size_t column) const
{
	ocean_assert(internalMatrix);
	ocean_assert(row < this->rows());
	ocean_assert(column < this->columns());

	return static_cast<typename InternalMatrix<T>::Type*>(internalMatrix)->coeff(typename InternalMatrix<T>::Type::Index(row), typename InternalMatrix<T>::Type::Index(column));
}

template <typename T>
T& SparseMatrixT<T>::operator()(const size_t row, const size_t column)
{
	ocean_assert(internalMatrix);
	ocean_assert(row < this->rows());
	ocean_assert(column < this->columns());

	return static_cast<typename InternalMatrix<T>::Type*>(internalMatrix)->coeffRef(typename InternalMatrix<T>::Type::Index(row), typename InternalMatrix<T>::Type::Index(column));
}

template class OCEAN_MATH_EXPORT SparseMatrixT<float>;

template class OCEAN_MATH_EXPORT SparseMatrixT<double>;

}
