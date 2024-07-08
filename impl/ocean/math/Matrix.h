/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_MATRIX_H
#define META_OCEAN_MATH_MATRIX_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class MatrixT;

/**
 * Definition of the Matrix object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see MatrixT
 * @ingroup math
 */
typedef MatrixT<Scalar> Matrix;

/**
 * Definition of the MatrixT template class using a double precision float data type.
 * @see MatrixT
 * @ingroup math
 */
typedef MatrixT<double> MatrixD;

/**
 * Definition of the MatrixT template class using a single precision float data type.
 * @see MatrixT
 * @ingroup math
 */
typedef MatrixT<float> MatrixF;

/**
 * Definition of a vector holding matrix objects.
 * @see Matrix
 * @ingroup math
 */
typedef std::vector<Matrix> Matrices;

/**
 * This class implements a matrix with arbitrary size.
 * The elements of this matrix are stored in a row aligned order.<br>
 * That means that elements are stored in the following pattern:
 * <pre>
 * |  0  1   2   3   4  ...  c-1 |
 * |  c c+1 c+2 c+3 c+4 ...      |
 * | ...                         |
 * </pre>
 * @tparam T Data type of matrix elements
 * @see Matrix, MatrixF, MatrixD, StaticMatrix, SparseMatrix, SquareMatrix2, SquareMatrix3, SquareMatrix4.
 * @ingroup math
 */
template <typename T>
class MatrixT
{
	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

		/**
		 * Definition of specific properties of matrices.
		 */
		enum MatrixProperty
		{
			/// No specific property known.
			MP_UNKNOWN,
			/// The matrix is symmetric.
			MP_SYMMETRIC
		};

	public:

		/**
		 * Creates a new matrix with no size.
		 */
		MatrixT() = default;

		/**
		 * Creates a new matrix with defined rows and columns.
		 * @param rows The rows of the new matrix
		 * @param columns The columns of the new matrix
		 */
		MatrixT(const size_t rows, const size_t columns);

		/**
		 * Creates a new matrix with defined rows and columns.
		 * @param rows The rows of the new matrix
		 * @param columns The columns of the new matrix
		 * @param toIdentity Determines whether the matrix will be initialized with as entity matrix or zero matrix.
		 */
		MatrixT(const size_t rows, const size_t columns, bool toIdentity);

		/**
		 * Creates a new matrix with defined rows and columns.
		 * @param rows The rows of the new matrix
		 * @param columns The columns of the new matrix
		 * @param value The value that every matrix element will be set to
		 */
		MatrixT(const size_t rows, const size_t columns, const T value);

		/**
		 * Creates a new matrix with defined rows and columns.
		 * @param rows The rows of the new matrix
		 * @param columns The columns of the new matrix
		 * @param source Elements to be copied into this matrix
		 */
		MatrixT(const size_t rows, const size_t columns, const T* source);

		/**
		 * Creates a new matrix with defined rows and columns.
		 * @param rows The rows of the new matrix
		 * @param columns The columns of the new matrix
		 * @param source Elements to be copied into this matrix
		 * @param valuesRowAligned True, if the given values are stored in a row aligned order (which is the default case for this matrix); False, if the values are stored in a column aligned order
		 */
		MatrixT(const size_t rows, const size_t columns, const T* source, const bool valuesRowAligned);

		/**
		 * Creates a new matrix with defined rows and columns and initializes the diagonal with small sub matrices.
		 * The number of columns of the given diagonal vector matrix defines the size of the small sub matrices.<br>
		 * The number of rows of the diagonal vector matrix must be a multiple of the number of rows.<br>
		 * @param rows The rows of the new matrix
		 * @param columns The columns of the new matrix
		 * @param diagonal The diagonal vector matrix holding the new diagonal sub matrices
		 */
		MatrixT(const size_t rows, const size_t columns, const MatrixT<T>& diagonal);

		/**
		 * Creates a new matrix with defined rows and columns and a given sub-matrix.
		 * The given sub-matrix can be larger than the new matrix as elements not fitting into the new matrix will be skipped.<br>
		 * All remaining elements of the matrix will be set to the specified value.
		 * @param rows The number of rows the new matrix will have, with range [1, infinity)
		 * @param columns The number of columns the new matrix will have, with range [1, infinity)
		 * @param subMatrix The sub-matrix from which the elements will be copied into the new matrix
		 * @param row The row at which the sub-matrix will be placed in the new matrix, with range [0, rows - 1]
		 * @param column The column at which the sub-matrix will be placed in the new matrix, with range [0, columns - 1]
		 * @param value The value which will be used to fill the remaining matrix elements, with range (-infinity, infinity)
		 */
		MatrixT(const size_t rows, const size_t columns, const MatrixT<T>& subMatrix, const size_t row, const size_t column, const T value = T(0.0));

		/**
		 * Copies a matrix.
		 * @param matrix The matrix to copy
		 */
		MatrixT(const MatrixT<T>& matrix);

		/**
		 * Move constructor.
		 * @param matrix The matrix to be moved
		 */
		MatrixT(MatrixT<T>&& matrix) noexcept;

		/**
		 * Destructs a matrix and releases the elements.
		 */
		~MatrixT();

		/**
		 * Returns the count of rows.
		 * @return Rows of the matrix, with range [0, infinity)
		 */
		inline size_t rows() const;

		/**
		 * Returns the count of columns.
		 * @return Columns of the matrix, with range [0, infinity)
		 */
		inline size_t columns() const;

		/**
		 * Returns the number of entire elements, which is the product of rows and columns.
		 * @return Number of elements, with range [0, infinity)
		 */
		inline size_t elements() const;

		/**
		 * Returns the inverted of this matrix.
		 * Beware: This function does not throw an exception if the matrix cannot be inverted.<br>
		 * Thus, ensure that the matrix is invertible before calling this function.<br>
		 * This matrix is invertible e.g., if the matrix is square and has full rank (rank() == rows() && rank() == columns()).<br>
		 * Even better: avoid the usage of this function and call invert() instead.<br>
		 * In case, this matrix is not invertible, this matrix will be returned instead.
		 * @return Inverted matrix
		 * @see invert(), rank().
		 */
		inline MatrixT<T> inverted() const;

		/**
		 * Inverts this matrix.
		 * @return True, if the matrix is not singular.
		 * @see inverted().
		 */
		bool invert();

		/**
		 * Returns the transposed of this matrix.
		 * @return Transposed matrix
		 */
		MatrixT<T> transposed() const;

		/**
		 * Transposes this matrix.
		 */
		inline void transpose();

		/**
		 * Returns the matrix product of this matrix and the transposed matrix of this matrix.<br>
		 * The result will be a square matrix with size: rows() x rows().<br>
		 * Actually, the following matrix will be returned: (*this) * (*this).transposed().
		 * @return Resulting matrix
		 */
		MatrixT<T> selfSquareMatrix() const;

		/**
		 * Returns the matrix product of transposed matrix of this matrix and this matrix.<br>
		 * The result will be a square matrix with size: columns() x columns().<br>
		 * Actually, the following matrix will be returned: (*this).transposed() * (*this).
		 * @return Resulting matrix
		 */
		MatrixT<T> selfTransposedSquareMatrix() const;

		/**
		 * Returns the matrix product of transposed matrix of this matrix and this matrix.<br>
		 * The result will be a square matrix with size: columns() x columns().<br>
		 * Actually, the following matrix will be returned: (*this).transposed() * (*this).
		 * @param result Resulting matrix
		 */
		void selfTransposedSquareMatrix(MatrixT<T>& result) const;

		/**
		 * Returns the matrix product of transposed matrix of this matrix and this matrix and applies a further squared diagonal weighting matrix.<br>
		 * The result will be a square matrix with size: columns() x columns().<br>
		 * Actually, the following matrix will be returned: (*this).transposed() * diag(weightDiagonal) * (*this).
		 * @param weightDiagonal Weight vector defining the diagonal of the weighting matrix, with one column
		 * @param result Resulting matrix
		 */
		void weightedSelfTransposedSquareMatrix(const MatrixT<T>& weightDiagonal, MatrixT<T>& result) const;

		/**
		 * Interprets this matrix as diagonal matrix and multiplies a second matrix on the right of the interpreted diagonal matrix.
		 * The square sub matrices size is defined by the number of columns of this matrix.<br>
		 * Thus the number of rows must be a multiple of the number of columns of this matrix.<br>
		 * Actually, the following matrix will be returned: result = diag(*this) * right.
		 * @param right The right matrix to be multiplied with the interpreted diagonal matrix
		 * @param result Resulting matrix
		 * @return True, if succeeded
		 */
		bool selfSquareDiagonalMatrixMultiply(const MatrixT<T>& right, MatrixT<T>& result) const;

		/**
		 * Interprets this matrix as diagonal matrix and multiplies a second matrix on the right of the interpreted diagonal matrix.
		 * Further, a diagonal weighting vector is applied.<br>
		 * The square sub matrices size is defined by the number of columns of this matrix.<br>
		 * Thus the number of rows must be a multiple of the number of columns of this matrix.<br>
		 * Actually, the following matrix will be returned: result = diag(*this) * diag(weightDiagonal) * right.
		 * @param right The right matrix to be multiplied with the interpreted diagonal matrix
		 * @param weightDiagonal Weight vector defining the diagonal of the weighting matrix, with one column
		 * @param result Resulting matrix
		 * @return True, if succeeded
		 */
		bool selfSquareDiagonalMatrixMultiply(const MatrixT<T>& weightDiagonal, const MatrixT<T>& right, MatrixT<T>& result) const;

		/**
		 * Multiplies this transposed matrix with a second matrix.
		 * Actually, the following matrix will be returned: (*this).transposed() * right.<br>
		 * The resulting matrix will have the size: columns() x right.columns().
		 * @param right Matrix to multiply
		 * @return Matrix product
		 */
		MatrixT<T> transposedMultiply(const MatrixT<T>& right) const;

		/**
		 * Multiplies this transposed matrix with a second matrix.
		 * Actually, the following matrix will be returned: (*this).transposed() * right.<br>
		 * The resulting matrix will have the size: columns() x right.columns().
		 * @param right Matrix to multiply
		 * @param result Resulting matrix product
		 */
		void transposedMultiply(const MatrixT<T>& right, MatrixT<T>& result) const;

		/**
		 * Solves the given linear system.
		 * M * x = b, with M and b known and M is a square matrix<br>
		 * This matrix is M, the given vector is b and the result will be x.<br>
		 * Note: If a non-square matrix should be solved, use: (M.transposed() * M) * x =  M.transposed() * b
		 * @param b Vector defining the linear system, with size columns() x 1
		 * @param x Solution vector receiving the solution if existing, with size columns() x 1
		 * @param matrixProperty The property of the matrix allowing to improve the solving performance and stability, MP_UNKNOWN to apply a standard solving
		 * @return True, if succeeded
		 */
		inline bool solve(const MatrixT<T>& b, MatrixT<T>& x, const MatrixProperty matrixProperty = MP_UNKNOWN) const;

		/**
		 * Solves the given linear system.
		 * M * x = b, with M and b known and M is a square matrix<br>
		 * This matrix is M, the given vector is b and the result will be x.<br>
		 * Note: If a non-square matrix should be solved, use: (M.transposed() * M) * x =  M.transposed() * b
		 * @param b Vector defining the linear system, with size columns() x 1
		 * @param x Solution vector receiving the solution if existing, with size columns() x 1
		 * @return True, if succeeded
		 * @tparam tMatrixProperty The property of the matrix allowing to improve the solving performance and stability, MP_UNKNOWN to apply a standard solving
		 */
		template <MatrixProperty tMatrixProperty>
		inline bool solve(const MatrixT<T>& b, MatrixT<T>& x) const;

		/**
		 * Solves the given linear system.
		 * M * x = b, with M and b known and M is a square matrix<br>
		 * This matrix is M, the given vector is b and the result will be x..<br>
		 * Note: If a non-square matrix should be solved, use: (M.transposed() * M) * x =  M.transposed() * b
		 * @param b Pointer to the vector defining the linear system, ensure that the number of provided values is equal to columns()
		 * @param x Pointer to the solution vector receiving the solution if existing, ensure that the number of elements is equal to columns()
		 * @param matrixProperty The property of the matrix allowing to improve the solving performance and stability, MP_UNKNOWN to apply a standard solving
		 * @return True, if succeeded
		 */
		inline bool solve(const T* b, T* x, const MatrixProperty matrixProperty = MP_UNKNOWN) const;

		/**
		 * Solves the given linear system.
		 * M * x = b, with M and b known and M is a square matrix<br>
		 * This matrix is M, the given vector is b and the result will be x..<br>
		 * Note: If a non-square matrix should be solved, use: (M.transposed() * M) * x =  M.transposed() * b
		 * @param b Pointer to the vector defining the linear system, ensure that the number of provided values is equal to columns()
		 * @param x Pointer to the solution vector receiving the solution if existing, ensure that the number of elements is equal to columns()
		 * @return True, if succeeded
		 * @tparam tMatrixProperty The property of the matrix allowing to improve the solving performance and stability, MP_UNKNOWN to apply a standard solving
		 */
		template <MatrixProperty tMatrixProperty>
		bool solve(const T* b, T* x) const;

		/**
		 * Performs a non-negative matrix factorization with multiplicative update rules
		 * V = W * H, V is a matrix containing non-negative values<br>
		 * This matrix is V, and will be factorized into two matrices W (weights) and H (subcomponents).
		 * @param subcomponents Solution matrix containing base component vectors
		 * @param weights Solution matrix containing weights to the component vectors
		 * @param components Number of base component vectors (Number of components is usually much smaller than its rank). If set to 0, the rank of matrix is used  [0, rank]
		 * @param iterations Number of iterations maximal performed [1, infinity)
		 * @param convergenceThreshold Differential threshold aborting the calculation (0, infinity)
		 * @return True, if succeeded
		 */
		bool nonNegativeMatrixFactorization(MatrixT<T>& subcomponents, MatrixT<T>& weights, const size_t components = 0u, const unsigned int iterations = 100u, const T convergenceThreshold = T(0.0001)) const;

		/**
		 * Computes the eigen system of this matrix.
		 * The function determines values and vectors that: matrix * vectors = vectors * diagonal(values).<br>
		 * Beware: The eigen values are not ordered!
		 * @param values Vector with resulting eigen values
		 * @param vectors Matrix with resulting eigen vectors as columns
		 * @return True, if succeeded
		 */
		bool eigenSystem(MatrixT<T>& values, MatrixT<T>& vectors) const;

		/**
		 * Computes the singular value decomposition for this matrix.
		 * This matrix is decomposed into three matrices as follows: u * w * v.transposed().<br>
		 * The diagonal values of w are ordered in descending order already.
		 * @param u Resulting u matrix
		 * @param w Resulting w vector holding the values of the diagonal matrix
		 * @param v Resulting v matrix
		 * @return True, if succeeded
		 */
		bool singularValueDecomposition(MatrixT<T>& u, MatrixT<T>& w, MatrixT<T>& v) const;

		/**
		 * Computes the QR decomposition for this matrix [m x n] while m >= n must hold.
		 * This matrix is decomposed into two matrices as follows: q * r, where q is a orthogonal [m x m] matrix (m * m^T = I), and r is a upper triangular matrix [m x n].
		 * @param qMatrix Resulting q matrix, containing the null space in the last (m - rank) columns, the size of the matrix will be adjusted internally
		 * @param rMatrix Optional resulting r matrix, upper triangle matrix, the size of the matrix will be adjusted internally, nullptr if the matrix is not needed
		 * @return True, if succeeded
		 */
		bool qrDecomposition(MatrixT<T>& qMatrix, MatrixT<T>* rMatrix = nullptr) const;

		/**
		 * Computes the Cholesky decomposition for this square matrix [m x m].
		 * This matrix is decomposed into M = L * L^T, where L is a lower triangular matrix [m x m].
		 * @param lMatrix Resulting L matrix, the lower triangle matrix
		 * @return True, if succeeded
		 */
		bool choleskyDecomposition(MatrixT<T>& lMatrix) const;

		/**
		 * Returns the pseudo inverse of this matrix by application of the singular value decomposition.
		 * @param epsilon The tolerance value, with range [0, infinity)
		 * @return The resulting pseudo inverted matrix
		 */
		MatrixT<T> pseudoInverted(const T epsilon = NumericT<T>::eps()) const;

		/**
		 * Computes the rank of this matrix.
		 * The matrix must be valid.
		 * @return The matrix's rank with range [0, min(rows(), columns())]
		 */
		inline size_t rank() const;

		/**
		 * Returns a row of the matrix.
		 * @param index The index of the row to receive, with range [0, rows())
		 * @return Entire row
		 */
		MatrixT<T> row(const size_t index) const;

		/**
		 * Returns a column vector of the matrix.
		 * @param column The column to receive the vector from, with range [0, columns())
		 * @return Column vector
		 */
		MatrixT<T> vector(const size_t column) const;

		/**
		 * Returns a vector containing the values of the diagonal.
		 * @return Vector with diagonal values
		 */
		MatrixT<T> diagonal() const;

		/**
		 * Determines the L1 norm (sum of absolute elements) of this matrix.
		 * @return Matrix norm
		 */
		inline T norm() const;

		/**
		 * Determines the sum of all elements of this matrix.
		 * @return Matrix sum
		 */
		inline T sum() const;

		/**
		 * Returns a sub matrix of this one
		 * @param row The start row at which the sub matrix will start, with range [0, row() - 1]
		 * @param column The start column at which the sub matrix will start, with range [0, columns() - 1]
		 * @param rows The number of rows in the sub matrix, with range [1, rows() - row]
		 * @param columns The number of columns in the sub matrix, with range [1, columns() - row]
		 * @return The resulting sub matrix
		 */
		MatrixT<T> subMatrix(const size_t row, const size_t column, const size_t rows, const size_t columns);

		/**
		 * Multiplies a row with a scalar.
		 * Beware: No range check will be done!
		 * @param row The row to multiply, with range [0, rows() - 1]
		 * @param scalar The scalar to multiply, with range (-infinity, infinity)
		 */
		void multiplyRow(const size_t row, const T scalar);

		/**
		 * Multiplies a column with a scalar.
		 * Beware: No range check will be done!
		 * @param column The column to multiply, with range [0, columns() - 1]
		 * @param scalar The scalar to multiply, with range (-infinity, infinity)
		 */
		void multiplyColumn(const size_t column, const T scalar);

		/**
		 * Resizes this matrix.
		 * @param rows Number of rows of the resized matrix
		 * @param columns Number of columns of the resized matrix
		 */
		void resize(const size_t rows, const size_t columns);

		/**
		 * Returns whether two matrices are almost identical up to a specified epsilon.
		 * @param matrix Second matrix that will be checked
		 * @param eps The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isEqual(const MatrixT<T>& matrix, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether this matrix is symmetric (and whether this matrix is a square matrix).
		 * Beware: An empty matrix (without any rows or columns) is symmetric.
		 * @param eps The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isSymmetric(const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns a pointer to the internal values.
		 * @return Pointer to the internal values
		 */
		inline const T* data() const;

		/**
		 * Returns a pointer to the internal values.
		 * @return Pointer to the internal values
		 */
		inline T* data();

		/**
		 * Assigns a matrix to this one.
		 * @param matrix The matrix to assign
		 * @return Reference to this matrix
		 */
		MatrixT<T>& operator=(const MatrixT<T>& matrix);

		/**
		 * Move assign operator.
		 * @param matrix The matrix to moved
		 * @return Reference to this matrix
		 */
		MatrixT<T>& operator=(MatrixT<T>&& matrix) noexcept;

		/**
		 * Returns whether two matrices are identical up to a small epsilon.
		 * @param right The right matrix
		 * @return True, if so
		 */
		inline bool operator==(const MatrixT<T>& right) const;

		/**
		 * Returns whether two matrices are not identical up to a small epsilon.
		 * @param right The right matrix
		 * @return True, if so
		 */
		inline bool operator!=(const MatrixT<T>& right) const;

		/**
		 * Adds two matrices.
		 * @param right The right matrix
		 * @return Sum matrix
		 */
		MatrixT<T> operator+(const MatrixT<T>& right) const;

		/**
		 * Adds and assigns two matrices.
		 * @param right The right matrix
		 * @return Reference to this matrix
		 */
		MatrixT<T>& operator+=(const MatrixT<T>& right);

		/**
		 * Subtracts two matrices.
		 * @param right The right matrix
		 * @return Subtracted matrix
		 */
		MatrixT<T> operator-(const MatrixT<T>& right) const;

		/**
		 * Subtracts and assigns two matrices.
		 * @param right The right matrix
		 * @return Reference to this matrix
		 */
		MatrixT<T>& operator-=(const MatrixT<T>& right);

		/**
		 * Multiplies two matrices.
		 * @param right The right matrix
		 * @return Resulting matrix
		 */
		MatrixT<T> operator*(const MatrixT<T>& right) const;

		/**
		 * Multiplies the matrix with a scalar.
		 * @param scalar The scalar to multiply
		 * @return Resulting matrix
		 */
		MatrixT<T> operator*(const T scalar) const;

		/**
		 * Multiplies and assigns two matrices.
		 * @param right The right matrix
		 * @return Reference to this matrix
		 */
		inline MatrixT<T>& operator*=(const MatrixT<T>& right);

		/**
		 * Multiplies this matrix by a scalar.
		 * @param scalar The scalar to multiply
		 * @return Reference to this matrix
		 */
		MatrixT<T>& operator*=(const T scalar);

		/**
		 * Returns the pointer to the elements of a specified row.
		 * @param row Index of the row to return, with range [0, rows())
		 * @return Specified row
		 */
		inline const T* operator[](const size_t row) const;

		/**
		 * Element operator for the row aligned elements.
		 * @param row Index of the element to return, with range [0, rows())
		 * @return Specified element
		 */
		inline T* operator[](const size_t row);

		/**
		 * Element operator allowing to access a specific elements of this matrix.
		 * @param row The row of the element to return, with range [0, rows())
		 * @param column The column of the element to return, with range [0, columns())
		 * @return Specified element
		 */
		inline T operator()(const size_t row, const size_t column) const;

		/**
		 * Element operator allowing to access a specific elements of this matrix.
		 * @param row The row of the element to return, with range [0, rows())
		 * @param column The column of the element to return, with range [0, columns())
		 * @return Specified element
		 */
		inline T& operator()(const size_t row, const size_t column);

		/**
		 * Element operator for the row aligned elements.
		 * @param index The index of the element to return, with range [0, elements())
		 * @return Specified element
		 */
		inline T operator()(const size_t index) const;

		/**
		 * Element operator for the row aligned elements.
		 * @param index The index of the element to return, with range [0, elements())
		 * @return Specified element
		 */
		inline T& operator()(const size_t index);

		/**
		 * Returns whether the matrix holds at least one element.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Computes the rank of given matrix data.
		 * @param data The elements of the matrix for which the rank will be calculated, provided in a row aligned order, must be valid
		 * @param rows The number of rows of the provided matrix, with range [1, infinity)
		 * @param columns The number of columns of the provided matrix, with range [1, infinity)
		 * @return The matrix's rank with range [0, min(rows, columns)]
		 */
		static size_t rank(const T* data, const size_t rows, const size_t columns);

	protected:

		/**
		 * Swaps two rows.
		 * @param row0 The index of the first row to be swapped, with range [0, rows())
		 * @param row1 The index of the second row to be swapped, with range [0, rows())
		 */
		void swapRows(const size_t row0, const size_t row1);

		/**
		 * Swaps two columns.
		 * @param column0 The index of the first column to be swapped, with range [0, columns())
		 * @param column1 The index of the second column to be swapped, with range [0, columns())
		 */
		void swapColumns(const size_t column0, const size_t column1);

		/**
		 * Adds a multiple of a row to another one.
		 * @param targetRow The index of the target row to which the multiple of the source row will be added, with range [0, rows())
		 * @param sourceRow The index of the source row, with range [0, rows())
		 * @param scalar The scalar to multiply the source elements with
		 */
		void addRows(const size_t targetRow, const size_t sourceRow, const T scalar);

		/**
		 * Performs an element-wise matrix multiplication meaning that each element of this matrix will be multiplied by a corresponding element from 'multiplier'.
		 * @param multiplier The matrix providing the multiplication elements, with same size as this matrix, all elements must not be zero
		 */
		void elementwiseMultiplication(const MatrixT<T>& multiplier);

		/**
		 * Performs an element-wise matrix division meaning that each element of this matrix will be divided by a corresponding element from 'denominator'.
		 * @param denominator The matrix providing the denominator elements, with same size as this matrix
		 */
		void elementwiseDivision(const MatrixT<T>& denominator);

	protected:

		/// Number of rows.
		size_t rows_ = 0;

		/// Number of columns.
		size_t columns_ = 0;

		/// Elements of the matrix.
		T* values_ = nullptr;
};

template <typename T>
inline size_t MatrixT<T>::rows() const
{
	return rows_;
}

template <typename T>
inline size_t MatrixT<T>::columns() const
{
	return columns_;
}

template <typename T>
inline size_t MatrixT<T>::elements() const
{
	return rows_ * columns_;
}

template <typename T>
inline bool MatrixT<T>::solve(const MatrixT<T>& b, MatrixT<T>& x, const MatrixProperty matrixProperty) const
{
	ocean_assert(columns() == b.rows());
	ocean_assert(b.columns() == 1);

	x.resize(columns(), 1);
	return solve(b.data(), x.data(), matrixProperty);
}

template <typename T>
template <typename MatrixT<T>::MatrixProperty tMatrixProperty>
inline bool MatrixT<T>::solve(const MatrixT<T>& b, MatrixT<T>& x) const
{
	ocean_assert(columns() == b.rows());
	ocean_assert(b.columns() == 1);

	x.resize(columns(), 1);
	return solve<tMatrixProperty>(b.data(), x.data());
}

template <typename T>
inline bool MatrixT<T>::solve(const T* b, T* x, const MatrixProperty matrixProperty) const
{
	switch (matrixProperty)
	{
		case MP_SYMMETRIC:
			return solve<MP_SYMMETRIC>(b, x);

		default:
			ocean_assert(matrixProperty == MP_UNKNOWN);
			return solve<MP_UNKNOWN>(b, x);
	}
}

template <typename T>
inline size_t MatrixT<T>::rank() const
{
	return rank(data(), rows(), columns());
}

template <typename T>
inline MatrixT<T> MatrixT<T>::inverted() const
{
	ocean_assert(rows_ == columns_);

	MatrixT<T> result(*this);

	if (!result.invert())
	{
		ocean_assert(false && "The matrix is a singular matrix.");
	}

	return result;
}

template <typename T>
inline void MatrixT<T>::transpose()
{
	*this = transposed();
}

template <typename T>
inline T MatrixT<T>::norm() const
{
	T result = T(0.0);

	for (size_t n = 0u; n < elements(); ++n)
	{
		result += NumericT<T>::abs(values_[n]);
	}

	return result;
}

template <typename T>
inline T MatrixT<T>::sum() const
{
	T result = T(0.0);

	for (size_t n = 0u; n < elements(); ++n)
	{
		result += values_[n];
	}

	return result;
}

template <typename T>
inline const T* MatrixT<T>::data() const
{
	return values_;
}

template <typename T>
inline T* MatrixT<T>::data()
{
	return values_;
}

template <typename T>
inline bool MatrixT<T>::operator==(const MatrixT<T>& right) const
{
	return isEqual(right, NumericT<T>::eps());
}

template <typename T>
inline bool MatrixT<T>::operator!=(const MatrixT<T>& right) const
{
	return !(*this == right);
}

template <typename T>
inline MatrixT<T>& MatrixT<T>::operator*=(const MatrixT<T>& right)
{
	*this = *this * right;
	return *this;
}

template <typename T>
inline const T* MatrixT<T>::operator[](const size_t row) const
{
	ocean_assert(row < rows());
	return values_ + row * columns_;
}

template <typename T>
inline T* MatrixT<T>::operator[](const size_t row)
{
	ocean_assert(row < rows());
	return values_ + row * columns_;
}

template <typename T>
inline T MatrixT<T>::operator()(const size_t row, const size_t column) const
{
	ocean_assert(row < rows_ && column < columns_);

	return *(values_ + row * columns_ + column);
}

template <typename T>
inline T& MatrixT<T>::operator()(const size_t row, const size_t column)
{
	ocean_assert(row < rows_ && column < columns_);

	return *(values_ + row * columns_ + column);
}

template <typename T>
inline T MatrixT<T>::operator()(const size_t index) const
{
	ocean_assert(index < elements());
	return values_[index];
}

template <typename T>
inline T& MatrixT<T>::operator()(const size_t index)
{
	ocean_assert(index < elements());
	return values_[index];
}

template <typename T>
inline MatrixT<T>::operator bool() const
{
	return values_ != nullptr;
}

}

#endif // META_OCEAN_MATH_MATRIX_H
