/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_SPARSE_MATRIX_H
#define META_OCEAN_MATH_SPARSE_MATRIX_H

#include "ocean/math/Math.h"
#include "ocean/math/Matrix.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class MatrixT;

// Forward declaration.
template <typename T> class SparseMatrixT;

/**
 * Definition of the SparseMatrix object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see SparseMatrixT
 * @ingroup math
 */
typedef SparseMatrixT<Scalar> SparseMatrix;

/**
 * Instantiation of the SparseMatrixT template class using a double precision float data type.
 * @see SparseMatrixT
 * @ingroup math
 */
typedef SparseMatrixT<double> SparseMatrixD;

/**
 * Instantiation of the SparseMatrixT template class using a single precision float data type.
 * @see SparseMatrixT
 * @ingroup math
 */
 typedef SparseMatrixT<float> SparseMatrixF;

/**
 * Definition of a vector holding sparse matrix objects.
 * @see SparseMatrix
 * @ingroup math
 */
typedef std::vector<SparseMatrix> SparseMatrices;

/**
 * This class implements a sparse matrix using a float type for its elements that is specified by T.
 * @tparam T Data type of matrix elements
 * @see SparseMatrix, SparseMatrixF, SparseMatrixD, Matrix.
 * @ingroup math
 */
template <typename T>
class SparseMatrixT
{
	public:

		// Forward declaration.
		class Entry;

		/**
		 * Definition of a vector holding entries.
		 */
		typedef std::vector<Entry> Entries;

		/**
		 * This class implements a triple object for matrix entries.
		 */
		class OCEAN_MATH_EXPORT Entry
		{
			public:

				/**
				 * Creates an invalid entry object.
				 */
				inline Entry();

				/**
				 * Creates a new entry object.
				 * @param row The row of the entry
				 * @param column The column of the entry
				 * @param value The value of the entry
				 */
				inline Entry(const size_t row, const size_t column, const T value);

				/**
				 * Returns the row of this entry
				 * @return The row
				 */
				inline size_t row() const;

				/**
				 * Returns the column of this entry
				 * @return The column
				 */
				inline size_t column() const;

				/**
				 * Returns the value of this entry
				 * @return The value
				 */
				inline T value() const;

				/**
				 * Returns whether this entry is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Compares two entries and returns whether this entry is 'lesser' than the second entry.
				 * Beware: The result is composed only on the coordinate of this entry, not on the value (thus, two entries with same coordinate but different value count as identical).
				 * @param second The second entry to compare
				 * @return True, if so
				 */
				inline bool operator<(const Entry& second) const;

				/**
				 * Checks whether a set of given entries have at least one entry in each row and in each column of a matrix with defined dimension.
				 * @param rows The number of rows of the matrix, with range [1, infinity)
				 * @param columns The number of columns of the matrix, with range [1, infinity)
				 * @param entries The entries to check
				 * @return True, if so
				 */
				static bool hasOneEntry(const size_t rows, const size_t columns, const Entries& entries);

			protected:

				/// The row of this entry.
				size_t entryRow;

				/// The column of this entry.
				size_t entryColumn;

				/// The value of this entry.
				T entryValue;
		};

	public:

		/**
		 * Creates an empty sparse matrix object.
		 */
		SparseMatrixT();

		/**
		 * Copy constructor.
		 * @param matrix Sparse matrix to be copied
		 */
		SparseMatrixT(const SparseMatrixT<T>& matrix);

		/**
		 * Move constructor.
		 * @param matrix Sparse matrix to be movied
		 */
		SparseMatrixT(SparseMatrixT<T>&& matrix) noexcept;

		/**
		 * Creates a new spare matrix with given dimensions.
		 * @param rows Number of rows to create a sparse matrix for
		 * @param columns Number of columns to create a sparse matrix for
		 */
		SparseMatrixT(const size_t rows, const size_t columns);

		/**
		 * Creates a new spare matrix with given dimensions.
		 * @param rows Number of rows to create a sparse matrix for
		 * @param columns Number of columns to create a sparse matrix for
		 * @param nonZeroElements Number of expected non zero elements to reserve memory for
		 */
		SparseMatrixT(const size_t rows, const size_t columns, const size_t nonZeroElements);

		/**
		 * Creates a new spare matrix with given dimensions.
		 * @param rows Number of rows to create a sparse matrix for
		 * @param columns Number of columns to create a sparse matrix for
		 * @param entries All non-zero entries of the new matrix
		 */
		SparseMatrixT(const size_t rows, const size_t columns, const Entries& entries);

		/**
		 * Creates a new sparse matrix with defined rows and columns and initializes the diagonal with small sub matrices.
		 * The number of columns of the given diagonal vector matrix defines the size of the small sub matrices.<br>
		 * The number of rows of the diagonal vector matrix must be a multiple of the number of rows.<br>
		 * @param rows Rows of the new matrix
		 * @param columns Columns of the new matrix
		 * @param diagonal Diagonal vector matrix holding the new diagonal sub matrices
		 * @param forceNonZeros True, to force all diagonal elements to be an epsilon larger than zero (if the given element is zero)
		 */
		SparseMatrixT(const size_t rows, const size_t columns, const MatrixT<T>& diagonal, const bool forceNonZeros = false);

		/**
		 * Creates a new sparse matrix with defined rows and columns and copies all non-zero values from a dense matrix.
		 * @param denseMatrix Dense matrix from that the non-zero values are copied
		 */
		explicit SparseMatrixT(const MatrixT<T>& denseMatrix);

		/**
		 * Destructs a sparse matrix object.
		 */
		~SparseMatrixT();

		/**
		 * Returns the number of rows this matrix has
		 * @return Number of rows
		 */
		size_t rows() const;

		/**
		 * Returns the number of columns this matrix has.
		 * @return Number of columns
		 */
		size_t columns() const;

		/**
		 * Returns the number of non zero elements stored in this matrix.
		 * @return Number of non zero elements
		 */
		size_t nonZeroElements() const;

		/**
		 * Returns a vector containing the values of the diagonal.
		 * @return Vector with diagonal values with dimension n x 1
		 */
		MatrixT<T> diagonal() const;

		/**
		 * Reserves memory for a specified number of non zero elements.
		 * @param elements Number of non zero elements.
		 */
		void reserve(const size_t elements);

		/**
		 * Returns whether a specified elements is zero.
		 * @param row The row of the element to be checked
		 * @param column The column of the element to be checked
		 * @return True, if so
		 */
		bool isZero(const size_t row, const size_t column) const;

		/**
		 * Returns whether two matrices are almost identical up to a specified epsilon.
		 * @param matrix Second matrix that will be checked
		 * @param eps Epsilon to be used
		 * @return True, if so
		 */
		bool isEqual(const SparseMatrixT<T>& matrix, const T eps) const;

		/**
		 * Returns whether two matrices are almost identical up to a specified epsilon.
		 * @param matrix Second matrix that will be checked
		 * @param eps Epsilon to be used
		 * @return True, if so
		 */
		bool isEqual(const MatrixT<T>& matrix, const T eps) const;

		/**
		 * (Re-)sets the non-zero entries of this sparse matrix.
		 * All previous non-zero entries will be removed.<br>
		 * If the given set of entries contains zero-values these zero values will be skipped.
		 * @param entries The entries to set
		 */
		void setEntries(const Entries& entries);

		/**
		 * Returns a submatrix of this matrix.
		 * @param row The index of the row which will be the first row of the new submatrix, with range [0, rows())
		 * @param column The index of the column which will be the first column of the new sub-matrix, with range [0, colums())
		 * @param rows The number of rows of the new sub-matrix, with range [1, rows() - row]
		 * @param columns The number of columns of the new sub-matrix, with range [1, columns() - column]
		 * @return The specified submatrix of this matrix
		 */
		SparseMatrixT<T> submatrix(const size_t row, const size_t column, const size_t rows, const size_t columns) const;

		/**
		 * Returns the transposes matrix of this matrix.
		 * @return Transposes matrix of this matrix
		 */
		SparseMatrixT<T> transposed() const;

		/**
		 * Transposes this matrix.
		 */
		void transpose();

		/**
		 * Solves the given linear system.
		 * M * x = b, with M and b known.<br>
		 * This matrix is M, the given vector is b and the result will be x.<br>
		 * @param b Vector defining the linear system
		 * @param x Solution vector receiving the solution if existing
		 * @return True, if succeeded
		 */
		bool solve(const MatrixT<T>& b, MatrixT<T>& x) const;

		/**
		 * Computes the rank of this matrix.
		 * The matrix must be valid.
		 * @return The matrix's rank with range [0, min(rows(), columns())]
		 */
		size_t rank() const;

		/**
		 * Determines the sum of all elements of this matrix.
		 * @return Matrix sum
		 */
		inline T sum() const;

		/**
		 * Inverts this square diagonal matrix.
		 * @return True, if succeeded
		 */
		bool invertDiagonal();

		/**
		 * Inverts this square block diagonal matrix with 3x3 block size.
		 * The size of the matrix must be a multiple of three.<br>
		 * Each of the 3x3 block is inverted individually.<br>
		 * @return True, if succeeded
		 */
		bool invertBlockDiagonal3();

		/**
		 * Inverts this square block diagonal matrix with size x size block size.
		 * The size of the matrix must be a multiple of size.<br>
		 * Each of the size x size block is inverted individually.<br>
		 * @param size The size of the block (size x size) with range (2, infinity)
		 * @return True, if succeeded
		 */
		bool invertBlockDiagonal(const size_t size);

		/**
		 * Returns the dense matrix of this matrix.
		 * @return Dense matrix
		 */
		MatrixT<T> denseMatrix() const;

		/**
		 * Performs a non-negative matrix factorization with multiplicative update rules
		 * V = W * H, V is a matrix containing non-negative values<br>
		 * This matrix is V, and will be factorized into two matrices W (weights) and H (subcomponents).
		 * @param subcomponents Solution matrix containing base component vectors
		 * @param weights Solution matrix containing weights to the component vectors
		 * @param components Number of base component vectors (Number of components is usally much smaller than its rank). If set to 0, the rank of matrix is used [0, rank]
		 * @param iterations Number of iterations maximal performed [1, infinity)
		 * @param convergenceThreshold Differential threshold as convergence criterion (0, infinity)
		 * @return True, if succeeded
		 */
		bool nonNegativeMatrixFactorization(MatrixT<T>& subcomponents, MatrixT<T>& weights, const unsigned int components = 0u, const unsigned int iterations = 100u, const T convergenceThreshold = T(0.0001)) const;

		/**
		 * Assign operator.
		 * @param matrix Sparse matrix to be assigned to this matrix
		 * @return Reference to this matrix
		 */
		SparseMatrixT<T>& operator=(const SparseMatrixT<T>& matrix);

		/**
		 * Move operator.
		 * @param matrix Sparse matrix to be move to this matrix
		 * @return Reference to this matrix
		 */
		SparseMatrixT<T>& operator=(SparseMatrixT<T>&& matrix) noexcept;

		/**
		 * Multiplies two sparse matrix objects and returns the matrix result.
		 * @param matrix Right sparse matrix to multiply
		 * @return Matrix product result
		 */
		SparseMatrixT<T> operator*(const SparseMatrixT<T>& matrix) const;

		/**
		 * Multiplies a second sparse matrix object to this matrix object.
		 * @param matrix Right sparse matrix to multiply
		 * @return Reference to this (modified) sparse matrix
		 */
		inline SparseMatrixT<T>& operator*=(const SparseMatrixT<T>& matrix);

		/**
		 * Adds two sparse matrix objects and returns the matrix result.
		 * @param matrix Right sparse matrix to add
		 * @return Matrix result
		 */
		SparseMatrixT<T> operator+(const SparseMatrixT<T>& matrix) const;

		/**
		 * Adds a second sparse matrix object to this matrix.
		 * @param matrix Right sparse matrix to add
		 * @return Reference to this (modified) matrix
		 */
		SparseMatrixT<T>& operator+=(const SparseMatrixT<T>& matrix);

		/**
		 * Subtracts two sparse matrix objects and returns the matrix result.
		 * @param matrix Right sparse matrix to subtract
		 * @return Matrix result
		 */
		SparseMatrixT<T> operator-(const SparseMatrixT<T>& matrix) const;

		/**
		 * Subtracts a second sparse matrix object from this matrix.
		 * @param matrix Right sparse matrix to subtract
		 * @return Reference to this (modified) matrix
		 */
		SparseMatrixT<T>& operator-=(const SparseMatrixT<T>& matrix);

		/**
		 * Multiplies a dense matrix object on this sparse matrix object and returns the dense matrix result.
		 * @param matrix Right matrix to multiply
		 * @return Matrix product result
		 */
		MatrixT<T> operator*(const MatrixT<T>& matrix) const;

		/**
		 * Returns whether this matrix is equal to an other one up to an epsilon value.
		 * @param matrix Second matrix to compare
		 * @return True, if succeeded
		 */
		bool operator==(const SparseMatrixT<T>& matrix) const;

		/**
		 * Returns whether this matrix is equal to an other one up to an epsilon value.
		 * @param matrix Second matrix to compare
		 * @return True, if succeeded
		 */
		bool operator==(const MatrixT<T>& matrix) const;

		/**
		 * Returns a specific element of the sparse matrix.
		 * @param row Element row to be returned
		 * @param column Element column to be returned
		 * @return Matrix element
		 */
		T operator()(const size_t row, const size_t column) const;

		/**
		 * Returns the reference to a specific element of the sparse matrix.
		 * Beware: The element must be non zero, thus the element must have been inserted before!
		 * @param row Element row to be returned
		 * @param column Element column to be returned
		 * @return Matrix element
		 * @see insert().
		 */
		T& operator()(const size_t row, const size_t column);

	private:

		/**
		 * Creates a new sparse matrix object by the internal matrix object data directly.
		 * @param matrix Internal matrix object
		 */
		explicit SparseMatrixT<T>(void* matrix);

	private:

		/// Abstract pointer to the internal matrix object.
		void* internalMatrix;
};

template <typename T>
inline SparseMatrixT<T>::Entry::Entry() :
	entryRow(size_t(-1)),
	entryColumn(size_t(-1)),
	entryValue(T(0))
{
	// nothing to do here
}

template <typename T>
inline SparseMatrixT<T>::Entry::Entry(const size_t row, const size_t column, const T value) :
	entryRow(row),
	entryColumn(column),
	entryValue(value)
{
	// nothing to do here
}

template <typename T>
inline size_t SparseMatrixT<T>::Entry::row() const
{
	return entryRow;
}

template <typename T>
inline size_t SparseMatrixT<T>::Entry::column() const
{
	return entryColumn;
}

template <typename T>
inline T SparseMatrixT<T>::Entry::value() const
{
	return entryValue;
}

template <typename T>
inline bool SparseMatrixT<T>::Entry::isValid() const
{
	return entryRow != size_t(-1) && entryColumn != size_t(-1);
}

template <typename T>
inline bool SparseMatrixT<T>::Entry::operator<(const Entry& second) const
{
	return entryRow < second.entryRow || (entryRow == second.entryRow && entryColumn < second.entryColumn);
}

template <typename T>
inline SparseMatrixT<T>& SparseMatrixT<T>::operator*=(const SparseMatrixT<T>& matrix)
{
	*this = (*this) * matrix;
	return *this;
}

template <typename T>
inline bool SparseMatrixT<T>::operator==(const SparseMatrixT<T>& matrix) const
{
	return isEqual(matrix, NumericT<T>::eps());
}

template <typename T>
inline bool SparseMatrixT<T>::operator==(const MatrixT<T>& matrix) const
{
	return isEqual(matrix, NumericT<T>::eps());
}

}

#endif // META_OCEAN_MATH_SPARSE_MATRIX_H
