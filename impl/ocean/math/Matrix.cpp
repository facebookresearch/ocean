/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/Matrix.h"

#if defined(OCEAN_COMPILER_MSC)
	#pragma warning(push)
	#pragma warning(disable: 4244) // 'argument': conversion from 'const __int64' to 'int', possible loss of data
#elif defined(OCEAN_COMPILER_CLANG)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wconversion"
#endif

#include <Eigen/Dense>

#if defined(OCEAN_COMPILER_MSC)
	#pragma warning(pop)
#elif defined(OCEAN_COMPILER_CLANG)
	#pragma clang diagnostic pop
#endif

namespace Ocean
{

template <typename T>
MatrixT<T>::MatrixT(MatrixT<T>&& matrix) noexcept :
	rows_(matrix.rows_),
	columns_(matrix.columns_),
	values_(matrix.values_)
{
	matrix.rows_ = 0;
	matrix.columns_ = 0;
	matrix.values_ = nullptr;
}

template <typename T>
MatrixT<T>::MatrixT(const size_t rows, const size_t columns) :
	rows_(0),
	columns_(0),
	values_(nullptr)
{
	resize(rows, columns);
}

template <typename T>
MatrixT<T>::MatrixT(const size_t rows, const size_t columns, bool toIdentity) :
	rows_(0),
	columns_(0),
	values_(nullptr)
{
	resize(rows, columns);

	size_t size = elements();

	if (size > 0)
	{
		// set all elements to 0.0
		memset(values_, 0, sizeof(T) * size);

#ifdef OCEAN_DEBUG
		for (size_t n = 0; n < size; ++n)
		{
			ocean_assert(values_[n] == T(0.0));
		}
#endif

		if (toIdentity)
		{
			size = min(rows, columns);

			for (size_t n = 0; n < size; n++)
			{
				(*this)[n][n] = 1;
			}
		}
	}
}

template <typename T>
MatrixT<T>::MatrixT(const size_t rows, const size_t columns, const T value) :
	rows_(0),
	columns_(0),
	values_(nullptr)
{
	resize(rows, columns);

	const size_t size = elements();

	if (size > 0)
	{
		ocean_assert(values_);

		std::fill(values_, values_ + size, value); // memset doesn't exist for floats
	}
}

template <typename T>
MatrixT<T>::MatrixT(const size_t rows, const size_t columns, const T* source) :
	rows_(0),
	columns_(0),
	values_(nullptr)
{
	resize(rows, columns);

	const size_t size = elements();

	if (size > 0)
	{
		ocean_assert(source && values_);
		memcpy(values_, source, sizeof(T) * size);
	}
}

template <typename T>
MatrixT<T>::MatrixT(const size_t rows, const size_t columns, const T* source, const bool valuesRowAligned) :
	rows_(0),
	columns_(0),
	values_(nullptr)
{
	resize(rows, columns);

	const size_t size = elements();

	if (size > 0)
	{
		ocean_assert(source && values_);

		if (valuesRowAligned)
		{
			memcpy(values_, source, sizeof(T) * size);
		}
		else
		{
			for (size_t c = 0; c < columns; ++c)
			{
				T* target = values_ + c;

				for (size_t r = 0; r < rows; ++r)
				{
					*target = *source++;
					target += columns;
				}
			}
		}
	}
}

template <typename T>
MatrixT<T>::MatrixT(const size_t rows, const size_t columns, const MatrixT<T>& diagonal) :
	rows_(0),
	columns_(0),
	values_(nullptr)
{
	resize(rows, columns);

	size_t size = elements();

	if (size > 0)
	{
		memset(values_, 0, sizeof(T) * size);

#ifdef OCEAN_DEBUG
		for (size_t n = 0; n < size; ++n)
		{
			ocean_assert(values_[n] == T(0.0));
		}
#endif

		if (diagonal.columns() == 1u)
		{
			size = min(rows, min(columns, diagonal.rows()));
			for (size_t n = 0; n < size; n++)
			{
				(*this)(n, n) = diagonal[n][0];
			}
		}
		else
		{
			const size_t diagonalSubMatrixSize = diagonal.columns();
			ocean_assert(diagonal.rows() % diagonalSubMatrixSize == 0);

			const size_t numberSubMatrices = min(diagonal.rows() / diagonalSubMatrixSize, min(rows / diagonalSubMatrixSize, columns / diagonalSubMatrixSize));

			for (size_t n = 0; n < numberSubMatrices; ++n)
			{
				const size_t offset = n * diagonalSubMatrixSize;

				for (size_t sr = 0; sr < diagonalSubMatrixSize; ++sr)
				{
					for (size_t sc = 0; sc < diagonalSubMatrixSize; ++sc)
					{
						(*this)(offset + sr, offset + sc) = diagonal(offset + sr, sc);
					}
				}
			}
		}
	}
}

template <typename T>
MatrixT<T>::MatrixT(const size_t rows, const size_t columns, const MatrixT<T>& subMatrix, const size_t row, const size_t column, const T value) :
	rows_(0),
	columns_(0),
	values_(nullptr)
{
	ocean_assert(rows >= 1 && columns >= 1);
	ocean_assert(row < rows && column < columns);

	resize(rows, columns);

	size_t size = elements();

	if (size > 0)
	{
		std::fill(values_, values_ + size, value);

#ifdef OCEAN_DEBUG
		for (size_t n = 0; n < size; ++n)
		{
			ocean_assert(values_[n] == value);
		}
#endif

		const size_t columnsToCopy = std::min(columns - column, subMatrix.columns());
		const size_t rowsToCopy = std::min(rows - row, subMatrix.rows());

		for (size_t r = row; r < row + rowsToCopy; ++r)
		{
			memcpy((*this)[r] + column, subMatrix[r - row], columnsToCopy * sizeof(T));
		}
	}
}

template <typename T>
MatrixT<T>::MatrixT(const MatrixT<T>& matrix) :
	rows_(0),
	columns_(0),
	values_(nullptr)
{
	resize(matrix.rows_, matrix.columns_);

	const size_t size = elements();

	if (size > 0)
	{
		ocean_assert(values_ && matrix.values_);
		memcpy(values_, matrix.values_, sizeof(T) * size);
	}
}

template <typename T>
MatrixT<T>::~MatrixT()
{
	free(values_);
}

template <typename T>
bool MatrixT<T>::invert()
{
	// Implements the Gauss-Jordan Elimination

	if (rows_ != columns_)
	{
		return false;
	}

	MatrixT<T> source(*this);
	MatrixT<T> target(rows(), columns(), true);

	for (size_t c = 0; c < columns(); ++c)
	{
		// Find largest absolute value in the c-th column,
		// to remove zeros from the main diagonal and to provide numerical stability
		T absolute = 0;
		size_t selectedRow = 0;

		for (size_t r = c; r < rows(); ++r)
		{
			const T value = NumericT<T>::abs(source(r, c));
			if (absolute < value)
			{
				absolute = value;
				selectedRow = r;
			}
		}

		// If there was no greater absolute value than 0 this matrix is singular

		if (absolute <= Numeric::eps() * 10) // here we take a slightly larger epsilon to improve the stability (otherwise real singular matrices may not be identified)
		{
			return false;
		}

		// Exchange the two rows
		if (selectedRow != c)
		{
			source.swapRows(c, selectedRow);
			target.swapRows(c, selectedRow);
		}

		// Now the element at (c, c) will be 1
		if (NumericT<T>::isNotEqual(source(c, c), 1))
		{
			const T divisor = 1 / source(c, c);
			ocean_assert(divisor != 0);

			source.multiplyRow(c, divisor);
			target.multiplyRow(c, divisor);
		}

		// Clear each entry above and below the selected r and column to zero
		for (size_t r = 0; r < rows(); ++r)
		{
			if (r != c)
			{
				const T value = -source(r, c);

				source.addRows(r, c, value);
				target.addRows(r, c, value);
			}
		}
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	if (std::is_same<T, double>::value)
	{
		const MatrixT<T> product(*this * target);
		for (size_t r = 0; r < product.rows(); ++r)
			for (size_t c = 0; c < product.columns(); ++c)
			{
				if (r == c)
					ocean_assert(NumericT<T>::isWeakEqual(product(r, c), Scalar(1)));
				else
					ocean_assert(NumericT<T>::isWeakEqual(product(r, c), Scalar(0)));
			}
	}
#endif

	*this = std::move(target);
	return true;
}

template <typename T>
MatrixT<T> MatrixT<T>::transposed() const
{
	MatrixT<T> result(columns(), rows());

	for (size_t r = 0; r < rows(); r++)
	{
		for (size_t c = 0; c < columns(); c++)
		{
			result(c, r) = (*this)(r, c);
		}
	}

	return result;
}

template <typename T>
MatrixT<T> MatrixT<T>::selfSquareMatrix() const
{
	MatrixT<T> result(rows_, rows_);

	T* target = result.data();

	for (size_t r = 0; r < rows_; ++r)
	{
		// we copy the already known values due to the symmetric property of the matrix
		for (size_t c = 0; c < r; ++c)
		{
			*target++ = result(c, r);
		}

		for (size_t c = r; c < rows_; ++c)
		{
			T value = 0;

			const T* left = values_ + r * columns_;
			const T* right = values_ + c * columns_;

			const T* const leftEnd = left + columns_;

			while (left != leftEnd)
			{
				value += *left++ * *right++;
			}

			*target++ = value;
		}
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	const MatrixT<T> debugMatrix(*this * transposed());
	ocean_assert(debugMatrix == result);
#endif

	return result;
}

template <typename T>
MatrixT<T> MatrixT<T>::selfTransposedSquareMatrix() const
{
	MatrixT<T> result(columns_, columns_);
	selfTransposedSquareMatrix(result);

	return result;
}

template <typename T>
void MatrixT<T>::selfTransposedSquareMatrix(MatrixT<T>& result) const
{
	/**
	 * Determination of matrix.transposed() * matrix:
	 * Here we can use the fact the resulting matrix will be symmetric so that we determine the results for the upper right triangle only (and copy the results to the lower triangle)
	 */

	result.resize(columns_, columns_);
	T* target = result.data();

	for (size_t r = 0; r < columns_; ++r)
	{
		// we copy the already known values due to the symmetric property of the matrix
		for (size_t c = 0; c < r; ++c)
		{
			*target++ = result(c, r);
		}

		for (size_t c = r; c < columns_; ++c)
		{
			T value = 0;

			const T* left = values_ + r;
			const T* right = values_ + c;

			left -= columns_;
			right -= columns_;

			const T* const leftEnd = left + rows_ * columns_;

			while (left != leftEnd)
			{
				value += *(left += columns_) * *(right += columns_);
			}

			*target++ = value;
		}
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	const MatrixT<T> debugMatrix(transposed() * *this);
	ocean_assert(debugMatrix == result);
#endif
}

template <typename T>
void MatrixT<T>::weightedSelfTransposedSquareMatrix(const MatrixT<T>& weightDiagonal, MatrixT<T>& result) const
{
	ocean_assert(weightDiagonal.rows() == rows() && weightDiagonal.columns() == 1u);

	result.resize(columns_, columns_);
	T* target = result.data();

	for (size_t r = 0; r < columns_; ++r)
	{
		// we copy the already known values due to the symmetric property of the matrix
		for (size_t c = 0; c < r; ++c)
		{
			*target++ = result(c, r);
		}

		for (size_t c = r; c < columns_; ++c)
		{
			T value = 0;

			const T* left = values_ + r;
			const T* right = values_ + c;
			const T* w = weightDiagonal.data();

			left -= columns_;
			right -= columns_;

			const T* const leftEnd = left + rows_ * columns_;

			while (left != leftEnd)
			{
				value += *(left += columns_) * *(right += columns_) * *w++;
			}

			*target++ = value;
		}
	}
}

template <typename T>
bool MatrixT<T>::selfSquareDiagonalMatrixMultiply(const MatrixT<T>& right, MatrixT<T>& result) const
{
	ocean_assert(*this && right);
	ocean_assert(rows() == right.rows());

	const size_t diagonalSubMatrixSize = columns_;
	ocean_assert(rows_ >= diagonalSubMatrixSize);
	ocean_assert(rows_ % diagonalSubMatrixSize == 0);

	if (rows() != right.rows() || rows() % diagonalSubMatrixSize != 0)
	{
		return false;
	}

	result.resize(rows(), right.columns());

	for (size_t r = 0; r < result.rows(); ++r)
	{
		for (size_t c = 0; c < result.columns(); ++c)
		{
			const size_t subMatrixIndex = r / diagonalSubMatrixSize;

			const size_t rightStartRow = subMatrixIndex * diagonalSubMatrixSize;
			const size_t rightStartColum = c;

			const size_t diagonalStartRow = r;
			const size_t diagoanlStartColum = 0;

			T value = 0;

			for (size_t n = 0; n < diagonalSubMatrixSize; ++n)
			{
				value += (*this)(diagonalStartRow, diagoanlStartColum + n) * right(rightStartRow + n, rightStartColum);
			}

			result(r, c) = value;
		}
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	if (!std::is_same<T, float>::value)
	{
		const MatrixT<T> diagonalMatrix(right.rows(), right.rows(), *this);
		const MatrixT<T> multiplicationResult(diagonalMatrix * right);
		ocean_assert(multiplicationResult.isEqual(result, NumericT<T>::weakEps()));
	}
#endif

	return true;
}

template <typename T>
bool MatrixT<T>::selfSquareDiagonalMatrixMultiply(const MatrixT<T>& weightDiagonal, const MatrixT<T>& right, MatrixT<T>& result) const
{
	ocean_assert(*this && right);

	ocean_assert(rows() == right.rows());
	ocean_assert(weightDiagonal.columns() == 1u);
	ocean_assert(weightDiagonal.rows() == rows());

	const size_t diagonalSubMatrixSize = columns_;
	ocean_assert(rows_ >= diagonalSubMatrixSize);
	ocean_assert(rows_ % diagonalSubMatrixSize == 0);

	if (rows() != right.rows() || rows() % diagonalSubMatrixSize != 0 || weightDiagonal.rows() != rows() || weightDiagonal.columns() != 1u)
	{
		return false;
	}

	result.resize(rows(), right.columns());

	for (size_t r = 0; r < result.rows(); ++r)
	{
		for (size_t c = 0; c < result.columns(); ++c)
		{
			const size_t subMatrixIndex = r / diagonalSubMatrixSize;
			const size_t rightStartRow = subMatrixIndex * diagonalSubMatrixSize;

			T value = 0;

			for (size_t n = 0; n < diagonalSubMatrixSize; ++n)
			{
				value += (*this)(r, n) * weightDiagonal(r) * right(rightStartRow + n, c);
			}

			result(r, c) = value;
		}
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	if (!std::is_same<T, float>::value)
	{
		const MatrixT<T> diagonalMatrix(right.rows(), right.rows(), *this);
		const MatrixT<T> diagonalWeighting(right.rows(), right.rows(), weightDiagonal);

		const MatrixT<T> multiplicationResult(diagonalMatrix * (diagonalWeighting * right));
		ocean_assert(multiplicationResult.isEqual(result, NumericT<T>::weakEps()));
	}
#endif

	return true;
}

template <typename T>
MatrixT<T> MatrixT<T>::transposedMultiply(const MatrixT<T>& matrix) const
{
	if (rows() != matrix.rows())
	{
		ocean_assert(false && "Invalid matrix dimensions.");
		return MatrixT<T>();
	}

	MatrixT<T> result(columns(), matrix.columns());
	T* target = result.data();

	for (size_t r = 0; r < columns_; ++r)
	{
		for (size_t c = 0; c < matrix.columns_; ++c)
		{
			T value = 0;

			const T* left = values_ + r;
			const T* right = matrix.values_ + c;

			left -= columns_;
			right -= matrix.columns_;

			const T* const leftEnd = left + rows_ * columns_;

			while (left != leftEnd)
			{
				value += *(left += columns_) * *(right += matrix.columns_);
			}

			*target++ = value;
		}
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	const MatrixT<T> debugMatrix(transposed() * matrix);
	ocean_assert(debugMatrix == result);
#endif

	return result;
}

template <typename T>
void MatrixT<T>::transposedMultiply(const MatrixT<T>& matrix, MatrixT<T>& result) const
{
	if (rows() != matrix.rows())
	{
		ocean_assert(false && "Invalid matrix dimensions.");
		return;
	}

	result.resize(columns(), matrix.columns());
	T* target = result.data() - 1;

	for (size_t r = 0; r < columns_; ++r)
	{
		for (size_t c = 0; c < matrix.columns_; ++c)
		{
			T value = 0;

			const T* left = values_ + r;
			const T* right = matrix.values_ + c;

			left -= columns_;
			right -= matrix.columns_;

			const T* const leftEnd = left + rows_ * columns_;

			while (left != leftEnd)
			{
				value += *(left += columns_) * *(right += matrix.columns_);
			}

			*++target = value;
		}
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	const MatrixT<T> debugMatrix(transposed() * matrix);
	ocean_assert(debugMatrix == result);
#endif
}

template <typename T>
template <typename MatrixT<T>::MatrixProperty tMatrixProperty>
bool MatrixT<T>::solve(const T* b, T* x) const
{
	ocean_assert(b && x);
	ocean_assert(rows() == columns());
	typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenMatrix;

	Eigen::Map<const EigenMatrix> matrix(data(), rows(), columns());

	typedef Eigen::Matrix<T, Eigen::Dynamic, 1, Eigen::ColMajor> EigenVector;
	Eigen::Map<const EigenVector> bVector(b, rows());

	switch (tMatrixProperty)
	{
		case MP_SYMMETRIC:
		{
			Eigen::LDLT<EigenMatrix> LDLT(matrix);
			Eigen::Map<EigenVector> xVector(x, columns());
			xVector = LDLT.solve(bVector);

			if (LDLT.info() != Eigen::Success)
			{
				return false;
			}

			break;
		}

		default:
		{
			ocean_assert(tMatrixProperty == MP_UNKNOWN);

			Eigen::Map<EigenVector> xVector(x, columns());
			xVector = matrix.fullPivLu().solve(bVector);

			break;
		}
	}

#ifdef OCEAN_INTENSIVE_DEBUG
	static_assert(OCEAN_DEBUG != 0, "Invalid debug state!");

	if (!std::is_same<T, float>::value)
	{
		MatrixT<T> xVector(columns(), 1, x);

		const MatrixT<T> result(*this * xVector);
		ocean_assert(result.rows() == xVector.rows());
		ocean_assert(result.columns() == 1);
		ocean_assert(xVector.columns() == 1);

		std::vector<T> differences(xVector.rows());
		T sqrDistance = T(0);

		for (unsigned int n = 0; n < xVector.rows(); ++n)
		{
			differences[n] = result(n, 0) - b[n];
			sqrDistance += NumericT<T>::sqr(differences[n]);
		}

		const T distance = differences.size() == 0 ? T(0) : NumericT<T>::sqrt(sqrDistance / T(differences.size()));

		if (NumericT<T>::isWeakEqualEps(distance) == false)
		{
			// we determine the average maximal (absolute) value of the matrix (e.g., the energy of the matrix)

			T absolusteAverageEnergy = T(0);
			for (size_t n = 0; n < rows_ * columns_; ++n)
			{
				absolusteAverageEnergy += NumericT<T>::abs(values_[n]);
			}

			absolusteAverageEnergy /= T(rows_ * columns_);

			// we expect/accept for each magnitude (larger than 1) a zero-inaccuracy of one magnitude

			if (absolusteAverageEnergy <= 1)
			{
				ocean_assert(false && "This should never happen!");
			}
			else
			{
				const T adjustedDistance = distance / absolusteAverageEnergy;
				ocean_assert(NumericT<T>::isEqualEps(adjustedDistance));
			}
		}
	}
#endif // OCEAN_INTENSIVE_DEBUG

	return true;
}

template <typename T>
size_t MatrixT<T>::rank(const T* data, const size_t rows, const size_t columns)
{
	ocean_assert(data && rows != 0 && columns != 0);

	typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenMatrix;

	return Eigen::Map<const EigenMatrix>(data, rows, columns).fullPivLu().rank();
}

template <typename T>
bool MatrixT<T>::nonNegativeMatrixFactorization(MatrixT<T>& subcomponents, MatrixT<T>& weights, const size_t components, const unsigned int iterations, const T convergenceThreshold) const
{
	ocean_assert(rows() > 0 && columns() > 0);
	ocean_assert(components < std::min(rows(), columns()) + 1u);
	ocean_assert(iterations > 0u);
	ocean_assert(convergenceThreshold > Numeric::eps());

	const size_t r = (components == 0) ? rank() : components;

	// initialize matrices with positive random values
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> matrixGain(rows(), r);
	matrixGain.setRandom();
	matrixGain = matrixGain.array().abs();

	subcomponents = MatrixT<T>(rows(), r, matrixGain.data());

	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> matrixSpectra(r, columns());
	matrixSpectra.setRandom();
	matrixSpectra = matrixSpectra.array().abs();

	weights = MatrixT<T>(r, columns(), matrixSpectra.data());

	T previousSum(0);

	// Run the update rules until iteration or convergence
	for (unsigned int iteration = 0u; iteration < iterations; ++iteration)
	{
		// update subcomponents
		MatrixT<T> r1(*this);
		r1.elementwiseDivision(subcomponents * weights);

		MatrixT<T> rTranspose = r1 * weights.transposed();
		subcomponents.elementwiseMultiplication(rTranspose);

		// normalize subcomponents using weights
		MatrixT<T> invertedSum(1, r);
		for (size_t i = 0; i < r; ++i)
		{
			const T sumrow = weights.row(i).sum();
			ocean_assert(sumrow > 0);

			invertedSum[0][i] = T(1) / sumrow;
		}

		for (size_t row = 0; row < subcomponents.rows(); row++)
		{
			for (size_t col = 0; col < r; col++)
			{
				subcomponents[row][col] *= invertedSum[0][col];
			}
		}

		// update weights
		r1 = *this;
		r1.elementwiseDivision(subcomponents * weights);

		rTranspose = subcomponents.transposedMultiply(r1);
		weights.elementwiseMultiplication(rTranspose);

		// normalize weights using subcomponents
		memset(invertedSum.data(), 0, sizeof(T) * r);
		for (size_t y = 0; y < subcomponents.rows(); y++)
		{
			for (size_t x = 0; x < r; x++)
			{
				invertedSum[0][x] += subcomponents[y][x];
			}
		}

		for (size_t i = 0; i < r; ++i)
		{
			ocean_assert(invertedSum[0][i] > 0);
			invertedSum[0][i] = T(1) / invertedSum[0][i];
		}

		for (size_t row = 0; row < r; row++)
		{
			for (size_t col = 0; col < weights.columns(); col++)
			{
				weights[row][col] *= invertedSum[0][row];
			}
		}

		// convergence criteria: r1 is a one matrix, if ||(this matrix) - (subcomponents * weights)||^2 is zero
		const T sum = r1.sum();

		if (NumericT<T>::abs(sum - previousSum) < convergenceThreshold)
		{
			break;
		}

		previousSum = sum;
	}

	return true;
}

template <typename T>
bool MatrixT<T>::eigenSystem(MatrixT<T>& values, MatrixT<T>& vectors) const
{
	if (rows_ != columns_)
	{
		ocean_assert(false && "Eigensystem analysis not possible on such a matrix.");
		return false;
	}

	typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenMatrix;
	Eigen::Map<const EigenMatrix> eigenMatrix(data(), rows(), columns());

	const Eigen::EigenSolver<EigenMatrix> evd(eigenMatrix);

	vectors = MatrixT<T>(rows(), columns(), false);
	values = MatrixT<T>(rows(), 1, false);

	Eigen::Map<EigenMatrix> eigenVectors(vectors.data(), rows(), columns());
	Eigen::Map<EigenMatrix> eigenValues(values.data(), rows(), 1);
	eigenVectors = evd.pseudoEigenvectors();
	eigenValues = evd.pseudoEigenvalueMatrix().diagonal();

	return true;
}

template <typename T>
bool MatrixT<T>::singularValueDecomposition(MatrixT<T>& u, MatrixT<T>& w, MatrixT<T>& v) const
{
	typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenMatrix;
	Eigen::Map<const EigenMatrix> eigenMatrix(data(), rows(), columns());

	const Eigen::JacobiSVD<EigenMatrix, Eigen::FullPivHouseholderQRPreconditioner> svd(eigenMatrix, Eigen::ComputeFullU | Eigen::ComputeFullV);

	u = MatrixT<T>(rows(), rows());
	w = MatrixT<T>(max(rows(), columns()), 1, false);
	v = MatrixT<T>(columns(), columns());

	Eigen::Map<EigenMatrix> eigenU(u.data(), u.rows(), u.columns());
	Eigen::Map<EigenMatrix> eigenV(v.data(), v.rows(), v.columns());

	eigenU = svd.matrixU();
	eigenV = svd.matrixV();

	ocean_assert(svd.singularValues().cols() == 1);
	ocean_assert(size_t(svd.singularValues().rows()) == min(rows(), columns()));
	ocean_assert(size_t(svd.singularValues().rows()) <= w.rows());
	memcpy(w.data(), svd.singularValues().data(), sizeof(T) * svd.singularValues().rows());

	return true;
}

template <typename T>
inline bool MatrixT<T>::qrDecomposition(MatrixT<T>& qMatrix, MatrixT<T>* rMatrix) const
{
	const size_t m = rows();
	const size_t n = columns();

	typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenMatrix;

	Eigen::Map<const EigenMatrix> eigenMatrix(data(), m, n);

	Eigen::HouseholderQR< Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> > householderQR(eigenMatrix);

	qMatrix.resize(m, m);
	Eigen::Map<EigenMatrix> eigenQ(qMatrix.data(), m, m);
	eigenQ = householderQR.householderQ();

	if (rMatrix)
	{
		rMatrix->resize(m, n);

		Eigen::Map<EigenMatrix> eigenR(rMatrix->data(), m, n);
		const EigenMatrix& eigenQR = householderQR.matrixQR();
		eigenR = eigenQR.template triangularView<Eigen::Upper>();
	}

	return true;
}

template<typename T>
bool MatrixT<T>::choleskyDecomposition(MatrixT<T>& lMatrix) const
{
	ocean_assert(rows() == columns());
	if (rows() != columns())
	{
		return false;
	}

	const size_t size = rows();

	typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenMatrix;

	Eigen::Map<const EigenMatrix> eigenMatrix(data(), size, size);

	lMatrix.resize(size, size);
	Eigen::Map<EigenMatrix> eigenL(lMatrix.data(), size, size);

	const Eigen::LLT<EigenMatrix> eigenLLT = eigenMatrix.llt();

	if (eigenLLT.info() != Eigen::Success)
	{
		return false;
	}

	eigenL = eigenLLT.matrixL();

	return true;
}

template <typename T>
MatrixT<T> MatrixT<T>::pseudoInverted(const T epsilon) const
{
	typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenMatrix;

	Eigen::Map<const EigenMatrix> eigenMatrix(data(), rows(), columns());

	const Eigen::JacobiSVD<EigenMatrix> svd(eigenMatrix, Eigen::ComputeThinU | Eigen::ComputeThinV);

	const T tolerance = epsilon * svd.singularValues().array().abs()(0) * T(rows() > columns() ? rows() : columns());

	MatrixT<T> result(columns(), rows());
	Eigen::Map<EigenMatrix> eigenResult(result.data(), columns(), rows());
	eigenResult = svd.matrixV() * (svd.singularValues().array().abs() > tolerance).select(svd.singularValues().array().inverse(), 0).matrix().asDiagonal() * svd.matrixU().adjoint();

	return result;
}

template <typename T>
MatrixT<T> MatrixT<T>::row(const size_t index) const
{
	ocean_assert(index < rows());

	MatrixT<T> result(1, columns_);

	memcpy(result.data(), (*this)[index], sizeof(T) * columns_);
	return result;
}

template <typename T>
MatrixT<T> MatrixT<T>::vector(const size_t column) const
{
	ocean_assert(column < columns());

	MatrixT<T> result(rows_, 1);

	for (size_t r = 0; r < rows_; r++)
		result(r) = (*this)(r, column);

	return result;
}

template <typename T>
MatrixT<T> MatrixT<T>::diagonal() const
{
	size_t size = min(rows(), columns());

	MatrixT<T> result(size, 1);

	for (size_t n = 0; n < size; n++)
	{
		result(n, 0) = (*this)(n, n);
	}

	return result;
}

template <typename T>
MatrixT<T> MatrixT<T>::subMatrix(const size_t row, const size_t column, const size_t rows, const size_t columns)
{
	if (columns == 0 || rows == 0 || row + rows > rows_ || column + columns > columns_)
	{
		ocean_assert(false && "Invalid sub matrix.");
		return MatrixT<T>();
	}

	MatrixT<T> result(rows, columns);

	for (size_t r = 0; r < rows; r++)
	{
		memcpy(result[r], (*this)[r + row] + column, sizeof(T) * columns);
	}

	return result;
}

template <typename T>
MatrixT<T>& MatrixT<T>::operator=(const MatrixT<T>& matrix)
{
	resize(matrix.rows_, matrix.columns_);

	if (matrix.elements() > 0)
	{
		memcpy(values_, matrix.values_, sizeof(T) * elements());
	}

	return *this;
}

template <typename T>
MatrixT<T>& MatrixT<T>::operator=(MatrixT<T>&& matrix) noexcept
{
	if (this != &matrix)
	{
		if (values_ != nullptr)
		{
			free(values_);
			values_ = nullptr;
		}

		rows_ = matrix.rows_;
		columns_ = matrix.columns_;
		values_ = matrix.values_;

		matrix.rows_ = 0;
		matrix.columns_ = 0;
		matrix.values_ = nullptr;
	}

	return *this;
}

template <typename T>
MatrixT<T> MatrixT<T>::operator+(const MatrixT<T>& right) const
{
	if (rows() != right.rows() || columns() != right.columns())
	{
		ocean_assert(false && "Invalid matrix dimensions.");
		return MatrixT<T>();
	}

	MatrixT<T> result(rows(), columns());

	const T* l = values_;
	const T* lend = l + elements();
	const T* r = right.values_;
	T* t = result.values_;

	while (l != lend)
	{
		*t = *l + *r;
		++t;
		++l;
		++r;
	}

	return result;
}

template <typename T>
MatrixT<T>& MatrixT<T>::operator+=(const MatrixT<T>& right)
{
	if (rows() != right.rows() || columns() != right.columns())
	{
		ocean_assert(false && "Invalid matrix dimensions.");
		return *this;
	}

	T* t = values_;
	const T* tend = t + elements();
	const T* r = right.values_;

	while (t != tend)
	{
		*t += *r;
		++t;
		++r;
	}

	return *this;
}

template <typename T>
MatrixT<T> MatrixT<T>::operator-(const MatrixT<T>& right) const
{
	if (rows() != right.rows() || columns() != right.columns())
	{
		ocean_assert(false && "Invalid matrix dimensions.");
		return MatrixT<T>();
	}

	MatrixT<T> result(rows(), columns());

	const T* l = values_;
	const T* lend = l + elements();
	const T* r = right.values_;
	T* t = result.values_;

	while (l != lend)
	{
		*t = *l - *r;
		++t;
		++l;
		++r;
	}

	return result;
}

template <typename T>
MatrixT<T>& MatrixT<T>::operator-=(const MatrixT<T>& right)
{
	if (rows() != right.rows() || columns() != right.columns())
	{
		ocean_assert(false && "Invalid matrix dimensions.");
		return *this;
	}

	T* t = values_;
	const T* tend = t + elements();
	const T* r = right.values_;

	while (t != tend)
	{
		*t -= *r;
		++t;
		++r;
	}

	return *this;
}

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION

template <typename T>
MatrixT<T> MatrixT<T>::operator*(const MatrixT<T>& right) const
{
	if (columns() != right.rows())
	{
		ocean_assert(false && "Invalid matrix dimensions.");
		return MatrixT<T>();
	}

	MatrixT<T> result(rows(), right.columns());

	T value;
	const T* left = nullptr;

	for (size_t c = 0; c < result.columns(); c++)
	{
		for (size_t r = 0; r < result.rows(); r++)
		{
			value = 0;
			left = values_ + r * columns();

			for (size_t n = 0; n < columns(); n++)
				value += left[n] * right(n, c);

			result(r, c) = value;
		}
	}

	return result;
}

#else

template <typename T>
MatrixT<T> MatrixT<T>::operator*(const MatrixT<T>& right) const
{
	if (columns() != right.rows())
	{
		ocean_assert(false && "Invalid matrix dimensions.");
		return MatrixT<T>();
	}

	MatrixT<T> result(rows(), right.columns());

	typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenMatrix;

	Eigen::Map<const EigenMatrix> leftEigen(data(), rows(), columns());
	Eigen::Map<const EigenMatrix> rightEigen(right.data(), right.rows(), right.columns());
	Eigen::Map<EigenMatrix> resultEigen(result.data(), result.rows(), result.columns());

	resultEigen = leftEigen * rightEigen;

	return result;
}

#endif

template <typename T>
MatrixT<T> MatrixT<T>::operator*(const T scalar) const
{
	MatrixT<T> result(rows_, columns_);

	T* p = result.data() - 1;
	const T* const end = p + elements();
	const T* source = values_ - 1;

	while (p != end)
	{
		*++p = *++source * scalar;
	}

	return result;
}

template <typename T>
MatrixT<T>& MatrixT<T>::operator*=(const T scalar)
{
	T* p = values_ - 1;
	const T* const end = p + elements();

	while (p != end)
	{
		*++p *= scalar;
	}

	return *this;
}

template <typename T>
void MatrixT<T>::multiplyRow(const size_t row, const T scalar)
{
	ocean_assert(row < rows());

	T* p = values_ + row * columns();
	const T* const end = p + columns();

	while (p != end)
	{
		*p *= scalar;
		++p;
	}
}

template <typename T>
void MatrixT<T>::multiplyColumn(const size_t column, const T scalar)
{
	ocean_assert(column < columns());

	T* p = values_ + column - columns();
	const T* const end = p + columns() * rows();

	while (p != end)
	{
		*(p += columns()) *= scalar;
		ocean_assert((p - values_) % columns() == column);
	}
}

template <typename T>
void MatrixT<T>::resize(const size_t rows, const size_t columns)
{
	ocean_assert(rows >= 1u && columns >= 1u);

	if (rows == rows_ && columns == columns_)
	{
		return;
	}

	if (rows == 0 && columns == 0)
	{
		free(values_);
		values_ = nullptr;
		return;
	}

	const size_t size = rows * columns;

	values_ = static_cast<T*>(values_ ? realloc(values_, sizeof(T) * size) : malloc(sizeof(T) * size));
	ocean_assert(values_ != nullptr);

	rows_ = rows;
	columns_ = columns;
}

template <typename T>
bool MatrixT<T>::isEqual(const MatrixT<T>& matrix, const T eps) const
{
	if (rows_ != matrix.rows_ || columns_ != matrix.columns_)
	{
		return false;
	}

	for (size_t n = 0; n < elements(); ++n)
	{
		if (NumericT<T>::isNotEqual(values_[n], matrix.values_[n], eps))
		{
			return false;
		}
	}

	return true;
}

template <typename T>
bool MatrixT<T>::isSymmetric(const T eps) const
{
	if (rows_ != columns_)
	{
		return false;
	}

	for (size_t r = 0; r < rows_; ++r)
	{
		for (size_t c = 0; c < rows_; ++c)
		{
			if (NumericT<T>::isNotEqual((*this)(r, c), (*this)(c, r), eps))
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T>
void MatrixT<T>::swapRows(const size_t row0, const size_t row1)
{
	ocean_assert(row0 < rows() && row1 < rows());

	if (row0 == row1)
	{
		return;
	}

	T* p0 = (*this)[row0];
	T* p1 = (*this)[row1];

	for (size_t n = 0; n < columns_; ++n)
	{
		std::swap(p0[n], p1[n]);
	}
}

template <typename T>
void MatrixT<T>::swapColumns(const size_t column0, const size_t column1)
{
	ocean_assert(column0 < columns() && column1 < columns());

	if (column0 == column1)
	{
		return;
	}

	T tmp;

	for (size_t n = 0; n < rows(); n++)
	{
		tmp = (*this)(n, column0);
		(*this)(n, column0) = (*this)(n, column1);
		(*this)(n, column1) = tmp;
	}
}

template <typename T>
void MatrixT<T>::addRows(const size_t targetRow, const size_t sourceRow, const T scalar)
{
	ocean_assert(targetRow < rows() && sourceRow < rows());

	T *t = values_ + targetRow * columns();
	const T* const tEnd = t + columns();
	const T *s = values_ + sourceRow * columns();

	while (t != tEnd)
	{
		*t += *s * scalar;
		++t;
		++s;
	}
}

template <typename T>
void MatrixT<T>::elementwiseDivision(const MatrixT<T>& denominator)
{
	ocean_assert(rows() == denominator.rows() && columns() == denominator.columns());
	ocean_assert(elements() == denominator.elements());

	const size_t size = elements();

	for (size_t n = 0; n < size; ++n)
	{
		ocean_assert(denominator.values_[n] != T(0));
		ocean_assert_accuracy(NumericT<T>::isNotEqualEps(denominator.values_[n]));

		values_[n] /= denominator.values_[n];
	}
}

template <typename T>
void MatrixT<T>::elementwiseMultiplication(const MatrixT<T>& multiplier)
{
	ocean_assert(rows() == multiplier.rows() && columns() == multiplier.columns());
	ocean_assert(elements() == multiplier.elements());

	const size_t size = elements();

	for (size_t n = 0; n < size; ++n)
	{
		values_[n] *= multiplier.values_[n];
	}
}

template class OCEAN_MATH_EXPORT MatrixT<float>;
template bool OCEAN_MATH_EXPORT MatrixT<float>::solve<MatrixT<float>::MP_SYMMETRIC>(const float*, float*) const;
template bool OCEAN_MATH_EXPORT MatrixT<float>::solve<MatrixT<float>::MP_UNKNOWN>(const float*, float*) const;

template class OCEAN_MATH_EXPORT MatrixT<double>;
template bool OCEAN_MATH_EXPORT MatrixT<double>::solve<MatrixT<double>::MP_SYMMETRIC>(const double*, double*) const;
template bool OCEAN_MATH_EXPORT MatrixT<double>::solve<MatrixT<double>::MP_UNKNOWN>(const double*, double*) const;

}
