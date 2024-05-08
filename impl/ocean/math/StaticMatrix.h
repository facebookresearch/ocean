/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_STATIC_MATRIX_H
#define META_OCEAN_MATH_STATIC_MATRIX_H

#include "ocean/math/Math.h"

namespace Ocean
{

/**
 * This class implements a matrix with static dimensions.
 * In contrast to a dynamic matrix the size of this matrix cannot be changed because the dimension is defined as template parameter.<br>
 * The elements inside the matrix are stored in a row aligned order.<br>
 * A StaticMatrix<double, 2, 4> would hold 2 rows and 4 columns.
 * <pre>
 * The indices of the internal elements would be:
 * | 0 1 2 3 |
 * | 4 5 6 7 |
 * </pre>
 * @tparam T Data type of the matrix elements
 * @tparam tRows Number of rows the matrix holds
 * @tparam tColumns Number of columns this matrix holds
 * @see Matrix, MatrixD, MatrixF, Matrix, SparseMatrix.
 * @ingroup math
 */
template <typename T, size_t tRows, size_t tColumns>
class StaticMatrix
{
	public:

		/// Definition of the matrix element type.
		typedef T Type;

	public:

		/**
		 * Creates a new matrix object without initializing the matrix elements.
		 */
		inline StaticMatrix();

		/**
		 * Creates a new matrix object and sets all elements to one unique value.
		 * @param value The value used to initialize each elements of the new matrix
		 */
		explicit inline StaticMatrix(const T& value);

		/**
		 * Creates a new matrix and initializes the elements of the matrix so that we receive an identity matrix or a zero matrix.
		 * @param toIdentity True, to initialize the matrix as identity matrix; False, to initialize the matrix as zero matrix
		 */
		explicit StaticMatrix(const bool toIdentity);

		/**
		 * Creates a new matrix element and initialized the matrix elements by a given data buffer with row aligned elements.
		 * Beware: The given buffer must be large enough.
		 * @param values Values to be copied to the matrix element buffer
		 */
		explicit StaticMatrix(const T* values);

		/**
		 * Creates a new matrix element and initialized the matrix elements by a given data buffer.
		 * Beware: The given buffer must be large enough.
		 * @param values Values to be copied to the matrix element buffer
		 * @param valuesRowAligned True, if the given values are stored in a row aligned order; False, if the values are stored in a column aligned order
		 */
		StaticMatrix(const T* values, const bool valuesRowAligned);

		/**
		 * Returns the number of rows this matrix holds.
		 * @return Matrix rows
		 * @see columns().
		 */
		static inline size_t rows();

		/**
		 * Returns the number of columsn this matrix holds.
		 * @return Matrix columns
		 * @see rows().
		 */
		static inline size_t columns();

		/**
		 * Returns the number of elements this matrix stores.
		 * @return Number of matrix elements
		 */
		static inline size_t elements();

		/**
		 * Returns a pointer to a specified row.
		 * @param index The index of the row, with range [0, rows())
		 * @return The pointer to the first element in the specified row of the matrix
		 */
		inline const T* row(const size_t index) const;

		/**
		 * Returns a pointer to a specified row.
		 * @param index The index of the row, with range [0, rows())
		 * @return The pointer to the first element in the specified row of the matrix
		 */
		inline T* row(const size_t index);

		/**
		 * Returns a pointer to a specified row.
		 * @return The pointer to the first element in the specified row of the matrix
		 * @tparam tIndex The index of the row, with range [0, rows())
		 */
		template <size_t tIndex>
		inline const T* row() const;

		/**
		 * Returns a pointer to a specified row.
		 * @return The pointer to the first element in the specified row of the matrix
		 * @tparam tIndex The index of the row, with range [0, rows())
		 */
		template <size_t tIndex>
		inline T* row();

		/**
		 * Returns a pointer to a specified element.
		 * @return The specified element
		 * @tparam tRow The index of the row, with range [0, rows())
		 * @tparam tColumn The index of the column, with range [0, columns())
		 */
		template <size_t tRow, size_t tColumn>
		inline const T& element() const;

		/**
		 * Returns a pointer to a specified element.
		 * @return The specified element
		 * @tparam tRow The index of the row, with range [0, rows())
		 * @tparam tColumn The index of the column, with range [0, columns())
		 */
		template <size_t tRow, size_t tColumn>
		inline T& element();

		/**
		 * Returns the pointer to the internal element buffer.
		 * @return Matrix element buffer
		 */
		inline const T* data() const;

		/**
		 * Returns the pointer to the internal element buffer.
		 * @return Matrix element buffer
		 */
		inline T* data();

		/**
		 * Returns whether all elements of this matrix are zero.
		 * @return True, if so
		 */
		bool isNull() const;

		/**
		 * Returns whether this matrix is an identity matrix.
		 * @return True, if so
		 */
		bool isIdentity() const;

		/**
		 * Returns whether two matrices are almost identical up to a specified epsilon.
		 * @param matrix Second matrix that will be checked
		 * @param eps The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isEqual(const StaticMatrix<T, tRows, tColumns>& matrix, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether this matrix is symmetric (and whether this matrix is a square matrix).
		 * Beware: An empty matrix (without any rows or colums) is symmetric.
		 * @param eps The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isSymmetric(const T eps = NumericT<T>::eps()) const;

		/**
		 * Sets the matrix to a zero matrix.
		 * @see isNull();
		 */
		inline void toNull();

		/**
		 * Sets the elements of this matrix by copying the values from a given buffer.
		 * @param values The elements to set, ensure that enough values are provided
		 * @param valuesRowAligned True, if the given values are stored in a row aligned order; False, if the values are stored in a column aligned order
		 */
		inline void setData(const T* values, const bool valuesRowAligned);

		/**
		 * Solves the given linear system by application of the cholesky distribution.
		 * M * x = b, with M and b known.<br>
		 * This matrix is M and M must be a symmetric positive-definite matrix, the given vector is b and the result will be x.
		 * @param vectorB Vector defining the linear system, with size tRows x 1
		 * @param vectorX Solution vector receiving the solution if existing, ensure that the number of elements is equal to tRows x 1
		 * @return True, if succeeded
		 * @see isSymmetric().
		 */
		bool solveCholesky(const StaticMatrix<T, tRows, 1>& vectorB, StaticMatrix<T, tRows, 1>& vectorX) const;

		/**
		 * Adds this matrix to a given matrix.
		 * Thus, this function calculates: result += thisMatrix.
		 * @param target The matrix to which this matrix will be added
		 */
		void add(StaticMatrix<T, tRows, tColumns>& target) const;

		/**
		 * Adds this matrix transposed to a given matrix.
		 * Thus, this function calculates: result += thisMatrix.transposed().
		 * @param target The matrix to which this (transposed) matrix will be added
		 */
		void addTransposed(StaticMatrix<T, tColumns, tRows>& target) const;

		/**
		 * Multiplies this matrix with a second matrix and assigns the results to a matrix.
		 * This function calculates: result = this * matrix.
		 * @param matrix Second matrix to multiply this matrix with
		 * @param result Resulting multiplication matrix
		 */
		template <size_t tColumns2>
		inline void multiply(const StaticMatrix<T, tColumns, tColumns2>& matrix, StaticMatrix<T, tRows, tColumns2>& result) const;

		/**
		 * Multiplies this matrix with a second matrix and assigns the results to a given buffer.
		 * The given buffer is interpreted as if the given pointer points to the first element of a resulting matrix object.<br>
		 * @param matrix Second matrix to multiply this matrix with
		 * @param result Target buffer receiving the matrix result
		 */
		template <size_t tColumns2>
		inline void multiply(const StaticMatrix<T, tColumns, tColumns2>& matrix, T* result) const;

		/**
		 * Multiplies this matrix with a second matrix and assigns the results to a given buffer.
		 * The given buffer is interpreted as if the given pointer points to the first element of a resulting matrix object.<br>
		 * However, after one row of the target buffer has been computed the given pointer is shifted by a given offset value.<br>
		 * Thus, the multiplication product may be assigned into a matrix even larger than the product matrix itself.<br>
		 * @param matrix Second matrix to multiply this matrix with
		 * @param rowOffset Number of element the target pointer will be shifted after one row of the multiplication matrix has been created before the next row will be assigned
		 * @param result Target buffer receiving the matrix result
		 */
		template <size_t tColumns2>
		inline void multiply(const StaticMatrix<T, tColumns, tColumns2>& matrix, const size_t rowOffset, T* result) const;

		/**
		 * Multiplies this matrix (right) with the transposed matrix (left).
		 * Thus, this function returns matrix.transposed() * matrix.
		 * @return The multiplication result
		 */
		inline StaticMatrix<T, tColumns, tColumns> multiplyWithTransposedLeft() const;

		/**
		 * Multiplies this matrix (right) with the transposed matrix (left) and stores the result in a provided matrix.
		 * Thus, this function calculates: result = matrix.transposed() * matrix.
		 * @param result The resulting matrix
		 */
		inline void multiplyWithTransposedLeft(StaticMatrix<T, tColumns, tColumns>& result) const;

		/**
		 * Multiplies this matrix (left) with the transposed matrix (right).
		 * Thus, this function returns matrix * matrix.transposed().
		 * @return The multiplication result
		 */
		inline StaticMatrix<T, tRows, tRows> multiplyWithTransposedRight() const;

		/**
		 * Multiplies this matrix (left) with the transposed matrix (right) and stores the result in a provided matrix.
		 * Thus, this function calculates: result = matrix * matrix.transposed().
		 * @param result The resulting matrix
		 */
		inline void multiplyWithTransposedRight(StaticMatrix<T, tRows, tRows>& result) const;

		/**
		 * Multiplies this matrix (right) with the transposed matrix (left) and adds the resulting matrix to a given matrix.
		 * Thus, this function calculates target += matrix.transposed() * matrix.
		 * @param target The matrix to which the result will be added
		 */
		inline void multiplyWithTransposedLeftAndAdd(StaticMatrix<T, tColumns, tColumns>& target) const;

		/**
		 * Multiplies this matrix (left) with the transposed matrix (right) and adds the resulting matrix to a given matrix.
		 * Thus, this function calculates target += matrix * matrix.transposed().
		 * @param target The matrix to which the result will be added
		 */
		inline void multiplyWithTransposedRightAndAdd(StaticMatrix<T, tRows, tRows>& target) const;

		/**
		 * Returns the transposed matrix of this matrix.
		 * @return The transposed matrix
		 */
		inline StaticMatrix<T, tColumns, tRows> transposed() const;

		/**
		 * Transposes this matrix.
		 * @param result The resulting transposed matrix
		 */
		inline void transposed(StaticMatrix<T, tColumns, tRows>& result) const;

		/**
		 * Adds a given matrix to this matrix.
		 * @param matrix Matrix to be added to this matrix
		 * @return The resulting sum matrix
		 */
		inline StaticMatrix<T, tRows, tColumns> operator+(const StaticMatrix<T, tRows, tColumns>& matrix) const;

		/**
		 * Adds a given matrix to this matrix.
		 * @param matrix Matrix to be added to this matrix
		 * @return Reference to this (modified) matrix
		 */
		inline StaticMatrix<T, tRows, tColumns>& operator+=(const StaticMatrix<T, tRows, tColumns>& matrix);

		/**
		 * Subtracts a given matrix from this matrix.
		 * @param matrix Matrix to be subtracted from this matrix
		 * @return The resulting matrix
		 */
		inline StaticMatrix<T, tRows, tColumns> operator-(const StaticMatrix<T, tRows, tColumns>& matrix) const;

		/**
		 * Subtracts a given matrix from this matrix.
		 * @param matrix Matrix to be subtracted from this matrix
		 * @return Reference to this (modified) matrix
		 */
		inline StaticMatrix<T, tRows, tColumns>& operator-=(const StaticMatrix<T, tRows, tColumns>& matrix);

		/**
		 * Multiplies this matrix with a second matrix objects and returns the result.
		 * @param matrix Matrix to be multiplied with this matrix
		 * @return New resulting product matrix
		 */
		template <size_t tColumns2>
		inline StaticMatrix<T, tRows, tColumns2> operator*(const StaticMatrix<T, tColumns, tColumns2>& matrix) const;

		/**
		 * Multiplies this matrix with a scalar value element-wise and returns the new matrix.
		 * @param value Scalar value to multiply each element with
		 * @return Resulting new matrix
		 */
		inline StaticMatrix<T, tRows, tColumns> operator*(const T& value) const;

		/**
		 * Multiplies this matrix with a scalar value element-wise.
		 * @param value Scalar value to multiply each element with
		 * @return Reference to this (multiplied) matrix
		 */
		inline StaticMatrix<T, tRows, tColumns>& operator*=(const T& value);

		/**
		 * Returns a specified element of this matrix object.<br>
		 * @param row Row of the element to be returned, with range [0, rows())
		 * @param column Column of the element to be returned, with range [0, columns())
		 * @return Specified matrix element
		 */
		inline T operator()(const size_t row, const size_t column) const;

		/**
		 * Returns a specified element of this matrix object.<br>
		 * @param row Row of the element to be returned, with range [0, rows())
		 * @param column Column of the element to be returned, with range [0, columns())
		 * @return Specified matrix element
		 */
		inline T& operator()(const size_t row, const size_t column);

		/**
		 * Returns a specified element of this matrix object.<br>
		 * The element index must be defined regarding to the row aligned element order of this matrix.
		 * @param index Index of the matrix element to return, with range [0, rows() * columns())
		 * @return Specified matrix element
		 */
		inline T operator[](const size_t index) const;

		/**
		 * Returns a specified element of this matrix object.<br>
		 * The element index must be defined regarding to the row aligned element order of this matrix.
		 * @param index Index of the matrix element to return, with range [0, rows() * columns())
		 * @return Specified matrix element
		 */
		inline T& operator[](const size_t index);

	private:

		/// Matrix elements.
		T matrixValues[tRows * tColumns];
};

template <typename T, size_t tRows, size_t tColumns>
inline StaticMatrix<T, tRows, tColumns>::StaticMatrix()
{
	// nothing to do here
}

template <typename T, size_t tRows, size_t tColumns>
inline StaticMatrix<T, tRows, tColumns>::StaticMatrix(const T& value)
{
	for (size_t n = 0; n < tRows * tColumns; ++n)
	{
		matrixValues[n] = value;
	}
}

template <typename T, size_t tRows, size_t tColumns>
StaticMatrix<T, tRows, tColumns>::StaticMatrix(const bool toIdentity)
{
	ocean_assert(matrixValues);

	memset(matrixValues, 0, sizeof(T) * elements());

#ifdef OCEAN_DEBUG

		for (size_t n = 0; n < elements(); ++n)
		{
			ocean_assert(matrixValues[n] == T());
		}
#endif

	if (toIdentity)
	{
		for (size_t n = 0; n < min(tRows, tColumns); ++n)
		{
			(*this)(n, n) = T(1);
		}
	}
}

template <typename T, size_t tRows, size_t tColumns>
StaticMatrix<T, tRows, tColumns>::StaticMatrix(const T* values)
{
	ocean_assert(matrixValues);
	memcpy(matrixValues, values, sizeof(T) * tRows * tColumns);
}

template <typename T, size_t tRows, size_t tColumns>
StaticMatrix<T, tRows, tColumns>::StaticMatrix(const T* values, const bool valuesRowAligned)
{
	setData(values, valuesRowAligned);
}

template <typename T, size_t tRows, size_t tColumns>
inline size_t StaticMatrix<T, tRows, tColumns>::rows()
{
	return tRows;
}

template <typename T, size_t tRows, size_t tColumns>
inline size_t StaticMatrix<T, tRows, tColumns>::columns()
{
	return tColumns;
}

template <typename T, size_t tRows, size_t tColumns>
inline size_t StaticMatrix<T, tRows, tColumns>::elements()
{
	return tRows * tColumns;
}

template <typename T, size_t tRows, size_t tColumns>
inline const T* StaticMatrix<T, tRows, tColumns>::row(const size_t index) const
{
	ocean_assert(index < tRows);
	return matrixValues + index * tColumns;
}

template <typename T, size_t tRows, size_t tColumns>
inline T* StaticMatrix<T, tRows, tColumns>::row(const size_t index)
{
	ocean_assert(index < tRows);
	return matrixValues + index * tColumns;
}

template <typename T, size_t tRows, size_t tColumns>
template <size_t tIndex>
inline const T* StaticMatrix<T, tRows, tColumns>::row() const
{
	static_assert(tIndex < tRows, "Invalid row index");
	return matrixValues + tIndex * tColumns;
}

template <typename T, size_t tRows, size_t tColumns>
template <size_t tIndex>
inline T* StaticMatrix<T, tRows, tColumns>::row()
{
	static_assert(tIndex < tRows, "Invalid row index");
	return matrixValues + tIndex * tColumns;
}

template <typename T, size_t tRows, size_t tColumns>
template <size_t tRow, size_t tColumn>
inline const T& StaticMatrix<T, tRows, tColumns>::element() const
{
	static_assert(tRow < tRows, "Invalid row index");
	static_assert(tColumn < tColumns, "Invalid row index");

	return matrixValues[tRow * tColumns + tColumn];
}

template <typename T, size_t tRows, size_t tColumns>
template <size_t tRow, size_t tColumn>
inline T& StaticMatrix<T, tRows, tColumns>::element()
{
	static_assert(tRow < tRows, "Invalid row index");
	static_assert(tColumn < tColumns, "Invalid row index");

	return matrixValues[tRow * tColumns + tColumn];
}

template <typename T, size_t tRows, size_t tColumns>
inline const T* StaticMatrix<T, tRows, tColumns>::data() const
{
	return matrixValues;
}

template <typename T, size_t tRows, size_t tColumns>
inline T* StaticMatrix<T, tRows, tColumns>::data()
{
	return matrixValues;
}

template <typename T, size_t tRows, size_t tColumns>
bool StaticMatrix<T, tRows, tColumns>::isNull() const
{
	for (size_t n = 0; n < elements(); ++n)
	{
		if (NumericT<T>::isNotEqualEps(matrixValues[n]))
		{
			return false;
		}
	}

	return true;
}

template <typename T, size_t tRows, size_t tColumns>
bool StaticMatrix<T, tRows, tColumns>::isIdentity() const
{
	const T* pointer = matrixValues;

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tColumns; ++c)
		{
			if (r == c)
			{
				if (NumericT<T>::isNotEqual(*pointer++, T(1)))
				{
					return false;
				}
			}
			else
			{
				if (NumericT<T>::isNotEqualEps(*pointer++))
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T, size_t tRows, size_t tColumns>
bool StaticMatrix<T, tRows, tColumns>::isEqual(const StaticMatrix<T, tRows, tColumns>& matrix, const T eps) const
{
	for (size_t n = 0u; n < elements(); ++n)
	{
		if (NumericT<T>::isNotEqual(matrixValues[n], matrix.matrixValues[n], eps))
		{
			return false;
		}
	}

	return true;
}

template <typename T, size_t tRows, size_t tColumns>
bool StaticMatrix<T, tRows, tColumns>::isSymmetric(const T eps) const
{
	if constexpr (tRows != tColumns)
	{
		return false;
	}

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tRows; ++c)
		{
			if (NumericT<T>::isNotEqual((*this)(r, c), (*this)(c, r), eps))
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T, size_t tRows, size_t tColumns>
inline void StaticMatrix<T, tRows, tColumns>::toNull()
{
	for (size_t n = 0; n < elements(); ++n)
	{
		matrixValues[n] = T(0);
	}
}

template <typename T, size_t tRows, size_t tColumns>
inline void StaticMatrix<T, tRows, tColumns>::setData(const T* values, const bool valuesRowAligned)
{
	ocean_assert(tRows == 0 || tColumns == 0 || values);

	if (valuesRowAligned)
	{
		memcpy(matrixValues, values, sizeof(T) * tRows * tColumns);
	}
	else
	{
		for (size_t c = 0u; c < tColumns; ++c)
		{
			for (size_t r = 0u; r < tRows; ++r)
			{
				matrixValues[r * tColumns + c] = *values++;
			}
		}
	}
}

template <typename T, size_t tRows, size_t tColumns>
bool StaticMatrix<T, tRows, tColumns>::solveCholesky(const StaticMatrix<T, tRows, 1>& vectorB, StaticMatrix<T, tRows, 1>& vectorX) const
{
	ocean_assert(isSymmetric());

	// decomposition
	StaticMatrix<T, tRows, tColumns>  matrixR(false);
	for (unsigned int i = 0u; i < tRows; ++i)
	{
		// determine diagonal
		matrixR(i, i) = matrixValues[i * tColumns + i];

		for (unsigned int k = 0u; k < i; ++k)
		{
			matrixR(i, i) -= matrixR(i, k) * matrixR(i, k);
		}

		// check, if matrixR(i, i) is greater then 0
		if (NumericT<T>::isBelow(matrixR(i, i), 0))
		{
			return false;
		}

		matrixR(i, i) = NumericT<T>::sqrt(matrixR(i, i));

		for (unsigned int j = i + 1u; j < tRows; ++j)
		{
			matrixR(i, j) = matrixValues[i * tColumns + j];

			for (unsigned int k = 0u; k < i; ++k)
			{
				matrixR(i, j) -= matrixR(k, i) * matrixR(k, j);
			}

			// matrixR(i, i) has been proven to be greater than 0
			ocean_assert(!NumericT<T>::isEqualEps(matrixR(i, i)));
			matrixR(i, j) /= matrixR(i, i);

			matrixR(j, i) = matrixR(i, j);
		}
	}

	// solve for y
	StaticMatrix<T, tRows, 1> matrixY;
	for (unsigned int i = 0u; i < tRows; ++i)
	{
		matrixY(i, 0) = vectorB[i];

		for (unsigned int k = 0; k < i; ++k)
		{
			matrixY(i, 0) -= matrixY(k, 0) * matrixR(k, i);
		}

		// matrixR(i, i) has been proven to be greater than 0
		ocean_assert(!NumericT<T>::isEqualEps(matrixR(i, i)));
		matrixY(i, 0) /= matrixR(i, i);
	}

	// solve for x
	for (int y = int(tRows - 1u); y >= 0; --y)
	{
		vectorX(y, 0) = matrixY(y, 0);

		for (unsigned int i = y + 1u; i < tRows; ++i)
		{
			vectorX(y, 0) -= vectorX(i, 0) * matrixR(i, y);
		}

		// matrixR(y, y) has been proven to be greater than 0
		ocean_assert(!NumericT<T>::isEqualEps(matrixR(y, y)));
		vectorX(y, 0) /= matrixR(y, y);
	}

	return true;
}

template <typename T, size_t tRows, size_t tColumns>
void StaticMatrix<T, tRows, tColumns>::add(StaticMatrix<T, tRows, tColumns>& target) const
{
	T* targetData = target.matrixValues;
	const T* thisData = matrixValues;

	const T* const thisDataEnd = thisData + tRows * tColumns;

	while (thisData != thisDataEnd)
	{
		ocean_assert(thisData < thisDataEnd);
		*targetData++ += *thisData++;
	}
}

template <typename T, size_t tRows, size_t tColumns>
void StaticMatrix<T, tRows, tColumns>::addTransposed(StaticMatrix<T, tColumns, tRows>& target) const
{
	const T* data = matrixValues;

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tColumns; ++c)
		{
			target(c, r) += *data++;
		}
	}
}

template <typename T, size_t tRows, size_t tColumns>
template <size_t tColumns2>
inline void StaticMatrix<T, tRows, tColumns>::multiply(const StaticMatrix<T, tColumns, tColumns2>& matrix, StaticMatrix<T, tRows, tColumns2>& target) const
{
	multiply(matrix, target.data());
}

template <typename T, size_t tRows, size_t tColumns>
template <size_t tColumns2>
inline void StaticMatrix<T, tRows, tColumns>::multiply(const StaticMatrix<T, tColumns, tColumns2>& matrix, T* target) const
{
	ocean_assert(target);

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tColumns2; ++c)
		{
			T value = 0;

			for (size_t n = 0; n < tColumns; ++n)
			{
				value += (*this)(r, n) * matrix(n, c);
			}

			*target++ = value;
		}
	}
}


template <typename T, size_t tRows, size_t tColumns>
template <size_t tColumns2>
inline void StaticMatrix<T, tRows, tColumns>::multiply(const StaticMatrix<T, tColumns, tColumns2>& matrix, const size_t rowOffset, T* target) const
{
	ocean_assert(target);

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tColumns2; ++c)
		{
			T value = 0;

			for (size_t n = 0; n < tColumns; ++n)
			{
				value += (*this)(r, n) * matrix(n, c);
			}

			*target++ = value;
		}

		target += rowOffset;
	}
}

template <typename T, size_t tRows, size_t tColumns>
inline StaticMatrix<T, tColumns, tColumns> StaticMatrix<T, tRows, tColumns>::multiplyWithTransposedLeft() const
{
	StaticMatrix<T, tColumns, tColumns> result;
	multiplyWithTransposedLeft(result);

	return result;
}

template <typename T, size_t tRows, size_t tColumns>
inline void StaticMatrix<T, tRows, tColumns>::multiplyWithTransposedLeft(StaticMatrix<T, tColumns, tColumns>& result) const
{
	static_assert(tRows != 0 && tColumns != 0, "Invalid matrix dimension");

	T intermediate;
	T* target = result.data();

	for (size_t r = 0; r < tColumns; ++r)
	{
		for (size_t c = 0; c < tColumns; ++c)
		{
			intermediate = (*this)(0, r) * (*this)(0, c);

			for (size_t i = 1; i < tRows; ++i)
			{
				intermediate += (*this)(i, r) * (*this)(i, c);
			}

			*target++ = intermediate;
		}
	}
}

template <typename T, size_t tRows, size_t tColumns>
inline StaticMatrix<T, tRows, tRows> StaticMatrix<T, tRows, tColumns>::multiplyWithTransposedRight() const
{
	StaticMatrix<T, tRows, tRows> result;
	multiplyWithTransposedRight(result);

	return result;
}

template <typename T, size_t tRows, size_t tColumns>
inline void StaticMatrix<T, tRows, tColumns>::multiplyWithTransposedRight(StaticMatrix<T, tRows, tRows>& result) const
{
	static_assert(tRows != 0 && tColumns != 0, "Invalid matrix dimension");

	T intermediate;
	T* target = result.data();

	for (size_t r = 0; r < tRows; ++r)
	{
		const T* const rowElements = matrixValues + r * tColumns;

		for (size_t c = 0; c < tRows; ++c)
		{
			const T* const columnElements = matrixValues + c * tColumns;

			intermediate = rowElements[0] * columnElements[0];

			for (size_t i = 1; i < tColumns; ++i)
			{
				intermediate += rowElements[i] * columnElements[i];
			}

			*target++ = intermediate;
		}
	}
}

template <typename T, size_t tRows, size_t tColumns>
inline void StaticMatrix<T, tRows, tColumns>::multiplyWithTransposedLeftAndAdd(StaticMatrix<T, tColumns, tColumns>& result) const
{
	static_assert(tRows != 0 && tColumns != 0, "Invalid matrix dimension");

	T intermediate;
	T* target = result.data();

	for (size_t r = 0; r < tColumns; ++r)
	{
		for (size_t c = 0; c < tColumns; ++c)
		{
			intermediate = (*this)(0, r) * (*this)(0, c);

			for (size_t i = 1; i < tRows; ++i)
			{
				intermediate += (*this)(i, r) * (*this)(i, c);
			}

			*target++ += intermediate;
		}
	}
}

template <typename T, size_t tRows, size_t tColumns>
inline void StaticMatrix<T, tRows, tColumns>::multiplyWithTransposedRightAndAdd(StaticMatrix<T, tRows, tRows>& result) const
{
	static_assert(tRows != 0 && tColumns != 0, "Invalid matrix dimension");

	T intermediate;
	T* target = result.data();

	for (size_t r = 0; r < tRows; ++r)
	{
		const T* const rowElements = matrixValues + r * tColumns;

		for (size_t c = 0; c < tRows; ++c)
		{
			const T* const columnElements = matrixValues + c * tColumns;

			intermediate = rowElements[0] * columnElements[0];

			for (size_t i = 1; i < tColumns; ++i)
			{
				intermediate += rowElements[i] * columnElements[i];
			}

			*target++ += intermediate;
		}
	}
}

template <typename T, size_t tRows, size_t tColumns>
inline StaticMatrix<T, tColumns, tRows> StaticMatrix<T, tRows, tColumns>::transposed() const
{
	StaticMatrix<T, tColumns, tRows> result;
	transposed(result);

	return result;
}

template <typename T, size_t tRows, size_t tColumns>
inline void StaticMatrix<T, tRows, tColumns>::transposed(StaticMatrix<T, tColumns, tRows>& result) const
{
	for (size_t r = 0u; r < tRows; ++r)
	{
		for (size_t c = 0u; c < tColumns; ++c)
		{
			result(c, r) = (*this)(r, c);
		}
	}
}

template <typename T, size_t tRows, size_t tColumns>
inline StaticMatrix<T, tRows, tColumns> StaticMatrix<T, tRows, tColumns>::operator+(const StaticMatrix<T, tRows, tColumns>& matrix) const
{
	StaticMatrix<T, tRows, tColumns> result;

	const T* thisData = matrixValues;
	const T* matrixData = matrix.matrixValues;
	T* resultData = result.matrixValues;

	const T* const thisDataEnd = thisData + tRows * tColumns;

	while (thisData != thisDataEnd)
	{
		*resultData++ = *thisData++ + *matrixData++;
	}

	return result;
}

template <typename T, size_t tRows, size_t tColumns>
inline StaticMatrix<T, tRows, tColumns>& StaticMatrix<T, tRows, tColumns>::operator+=(const StaticMatrix<T, tRows, tColumns>& matrix)
{
	matrix.add(*this);
	return *this;
}

template <typename T, size_t tRows, size_t tColumns>
inline StaticMatrix<T, tRows, tColumns> StaticMatrix<T, tRows, tColumns>::operator-(const StaticMatrix<T, tRows, tColumns>& matrix) const
{
	StaticMatrix<T, tRows, tColumns> result;

	const T* thisData = matrixValues;
	const T* matrixData = matrix.matrixValues;
	T* resultData = result.matrixValues;

	const T* const thisDataEnd = thisData + tRows * tColumns;

	while (thisData != thisDataEnd)
	{
		ocean_assert(thisData < thisDataEnd);
		*resultData++ = *thisData++ - *matrixData++;
	}

	return result;
}

template <typename T, size_t tRows, size_t tColumns>
inline StaticMatrix<T, tRows, tColumns>& StaticMatrix<T, tRows, tColumns>::operator-=(const StaticMatrix<T, tRows, tColumns>& matrix)
{
	T* thisData = matrixValues;
	const T* matrixData = matrix.matrixValues;

	T* const thisDataEnd = thisData + tRows * tColumns;

	while (thisData != thisDataEnd)
	{
		ocean_assert(thisData < thisDataEnd);
		*thisData++ -= *matrixData++;
	}

	return *this;
}

template <typename T, size_t tRows, size_t tColumns>
template <size_t tColumns2>
inline StaticMatrix<T, tRows, tColumns2> StaticMatrix<T, tRows, tColumns>::operator*(const StaticMatrix<T, tColumns, tColumns2>& matrix) const
{
	static_assert(tRows != 0 && tColumns != 0, "Invalid matrix dimension");

	StaticMatrix<T, tRows, tColumns2> result;

	for (size_t r = 0; r < tRows; ++r)
	{
		for (size_t c = 0; c < tColumns2; ++c)
		{
			T value = (*this)(r, 0) * matrix(0, c);

			for (size_t n = 1; n < tColumns; ++n)
			{
				value += (*this)(r, n) * matrix(n, c);
			}

			result(r, c) = value;
		}
	}

	return result;
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

#if defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 10

template <>
template <>
OCEAN_FORCE_INLINE StaticMatrix<double, 4, 1> StaticMatrix<double, 4, 4>::operator*(const StaticMatrix<double, 4, 1>& vector) const
{
	// the following code uses the following AVX instructions, and needs AVX1 or higher

	// AVX1:
	// _mm256_loadu_pd
	// _mm256_mul_pd
	// _mm256_hadd_pd
	// _mm256_permute2f128_pd
	// _mm256_storeu_pd

	// we load the four values of the vector
	__m256d v = _mm256_loadu_pd(vector.data());

	// we load the first row of the matrix
	__m256d r0 = _mm256_loadu_pd(matrixValues + 0);

	// we multiply the row with the vector
	r0 = _mm256_mul_pd(r0, v);

	// we load the second row of the matrix
	__m256d r1 = _mm256_loadu_pd(matrixValues + 4);
	r1 = _mm256_mul_pd(r1, v);

	// we sum both multiplication results horizontally (at least two neighboring products)
	__m256d sum_interleaved_r0_r1 = _mm256_hadd_pd(r0, r1);

#ifdef OCEAN_COMPILER_MSC
	ocean_assert(NumericD::isEqual(sum_interleaved_r0_r1.m256d_f64[0], matrixValues[0 + 0] * vector[0] + matrixValues[0 + 1] * vector[1], NumericD::eps() * 10));
	ocean_assert(NumericD::isEqual(sum_interleaved_r0_r1.m256d_f64[1], matrixValues[4 + 0] * vector[0] + matrixValues[4 + 1] * vector[1], NumericD::eps() * 10));
	ocean_assert(NumericD::isEqual(sum_interleaved_r0_r1.m256d_f64[2], matrixValues[0 + 2] * vector[2] + matrixValues[0 + 3] * vector[3], NumericD::eps() * 10));
	ocean_assert(NumericD::isEqual(sum_interleaved_r0_r1.m256d_f64[3], matrixValues[4 + 2] * vector[2] + matrixValues[4 + 3] * vector[3], NumericD::eps() * 10));
#endif

	// we load the third row
	__m256d r2 = _mm256_loadu_pd(matrixValues + 8);
	r2 = _mm256_mul_pd(r2, v);

	// we load the fourth row
	__m256d r3 = _mm256_loadu_pd(matrixValues + 12);
	r3 = _mm256_mul_pd(r3, v);

	// we sum the next multiplication results (at least two neighboring products)
	__m256d sum_interleaved_r2_r3 = _mm256_hadd_pd(r2, r3);

#ifdef OCEAN_COMPILER_MSC
	ocean_assert(NumericD::isEqual(sum_interleaved_r2_r3.m256d_f64[0], matrixValues[ 8 + 0] * vector[0] + matrixValues[ 8 + 1] * vector[1], NumericD::eps() * 10));
	ocean_assert(NumericD::isEqual(sum_interleaved_r2_r3.m256d_f64[1], matrixValues[12 + 0] * vector[0] + matrixValues[12 + 1] * vector[1], NumericD::eps() * 10));
	ocean_assert(NumericD::isEqual(sum_interleaved_r2_r3.m256d_f64[2], matrixValues[ 8 + 2] * vector[2] + matrixValues[ 8 + 3] * vector[3], NumericD::eps() * 10));
	ocean_assert(NumericD::isEqual(sum_interleaved_r2_r3.m256d_f64[3], matrixValues[12 + 2] * vector[2] + matrixValues[12 + 3] * vector[3], NumericD::eps() * 10));
#endif

	// now we reorder the interleaved sums
	__m256d sum_first = _mm256_permute2f128_pd(sum_interleaved_r0_r1, sum_interleaved_r2_r3, 0x20); //  0x20 = 0010 0000
	__m256d sum_second = _mm256_permute2f128_pd(sum_interleaved_r0_r1, sum_interleaved_r2_r3, 0x31); // 0x31 = 0011 0001

	// we finally add both reordered sums
	__m256d sum = _mm256_add_pd(sum_first, sum_second);

	StaticMatrix<double, 4, 1> result;
	_mm256_storeu_pd(result.data(), sum);

	ocean_assert(NumericD::isEqual(result[0], matrixValues[ 0] * vector[0] + matrixValues[ 1] * vector[1] + matrixValues[ 2] * vector[2] + matrixValues[ 3] * vector[3], NumericD::eps() * 100));
	ocean_assert(NumericD::isEqual(result[1], matrixValues[ 4] * vector[0] + matrixValues[ 5] * vector[1] + matrixValues[ 6] * vector[2] + matrixValues[ 7] * vector[3], NumericD::eps() * 100));
	ocean_assert(NumericD::isEqual(result[2], matrixValues[ 8] * vector[0] + matrixValues[ 9] * vector[1] + matrixValues[10] * vector[2] + matrixValues[11] * vector[3], NumericD::eps() * 100));
	ocean_assert(NumericD::isEqual(result[3], matrixValues[12] * vector[0] + matrixValues[13] * vector[1] + matrixValues[14] * vector[2] + matrixValues[15] * vector[3], NumericD::eps() * 100));

	return result;
}

#else

template <>
template <>
OCEAN_FORCE_INLINE StaticMatrix<double, 4, 1> StaticMatrix<double, 4, 4>::operator*(const StaticMatrix<double, 4, 1>& vector) const
{
	// the following code uses the following SSE instructions, and needs SSE 3 or higher

	// SSE2:
	// _mm_loadu_pd
	// _mm_add_pd
	// _mm_storeu_pd
	// _mm_mul_pd

	// SSE3:
	// _mm_hadd_pd

	// we load the vector into two individual 128 bit registers
	__m128d va = _mm_loadu_pd(vector.data());
	__m128d vb = _mm_loadu_pd(vector.data() + 2);

	// we load the first row of the matrix
	__m128d r0a = _mm_loadu_pd(matrixValues + 0);
	__m128d r0b = _mm_loadu_pd(matrixValues + 2);

	// we multiply the first row of the matrix with the vector and sum both results
	r0a = _mm_mul_pd(r0a, va);
	r0b = _mm_mul_pd(r0b, vb);
	r0a = _mm_add_pd(r0a, r0b);

	// we load the second row of the matrix
	__m128d r1a = _mm_loadu_pd(matrixValues + 4);
	__m128d r1b = _mm_loadu_pd(matrixValues + 6);

	// we multiply the second row of the matrix with the vector and sum both results
	r1a = _mm_mul_pd(r1a, va);
	r1b = _mm_mul_pd(r1b, vb);
	r1a = _mm_add_pd(r1a, r1b);

	// we sum both intermediate sums horizontally and receive the first half of the result vector
	__m128d result0 = _mm_hadd_pd(r0a, r1a);

	StaticMatrix<double, 4, 1> result;
	_mm_storeu_pd(result.data(), result0);

	__m128d r2a = _mm_loadu_pd(matrixValues + 8);
	__m128d r2b = _mm_loadu_pd(matrixValues + 10);

	r2a = _mm_mul_pd(r2a, va);
	r2b = _mm_mul_pd(r2b, vb);
	r2a = _mm_add_pd(r2a, r2b);

	__m128d r3a = _mm_loadu_pd(matrixValues + 12);
	__m128d r3b = _mm_loadu_pd(matrixValues + 14);

	r3a = _mm_mul_pd(r3a, va);
	r3b = _mm_mul_pd(r3b, vb);
	r3a = _mm_add_pd(r3a, r3b);

	__m128d result1 = _mm_hadd_pd(r2a, r3a);

	_mm_storeu_pd(result.data() + 2, result1);

	ocean_assert(NumericD::isEqual(result[0], matrixValues[0] * vector[0] + matrixValues[1] * vector[1] + matrixValues[2] * vector[2] + matrixValues[3] * vector[3], NumericD::eps() * 100));
	ocean_assert(NumericD::isEqual(result[1], matrixValues[4] * vector[0] + matrixValues[5] * vector[1] + matrixValues[6] * vector[2] + matrixValues[7] * vector[3], NumericD::eps() * 100));
	ocean_assert(NumericD::isEqual(result[2], matrixValues[8] * vector[0] + matrixValues[9] * vector[1] + matrixValues[10] * vector[2] + matrixValues[11] * vector[3], NumericD::eps() * 100));
	ocean_assert(NumericD::isEqual(result[3], matrixValues[12] * vector[0] + matrixValues[13] * vector[1] + matrixValues[14] * vector[2] + matrixValues[15] * vector[3], NumericD::eps() * 100));

	return result;
}

#endif

template <>
template <>
OCEAN_FORCE_INLINE StaticMatrix<float, 4, 1> StaticMatrix<float, 4, 4>::operator*(const StaticMatrix<float, 4, 1>& vector) const
{
	// the following code uses the following SSE instructions, and thus needs SSE 4.1 or higher

	// SSE:
	// _mm_loadu_ps
	// _mm_or_ps
	// _mm_storeu_ps

	// SSE4.1:
	// _mm_dp_ps

	// we load the four values of the vector
	__m128 v = _mm_loadu_ps(vector.data());

	// we load the first four rows of the matrix
	__m128 r0 = _mm_loadu_ps(matrixValues + 0);
	__m128 r1 = _mm_loadu_ps(matrixValues + 4);
	__m128 r2 = _mm_loadu_ps(matrixValues + 8);
	__m128 r3 = _mm_loadu_ps(matrixValues + 12);

	// we determine the dot product between the first row and the vector and store the result in the first float bin
	r0 = _mm_dp_ps(r0, v, 0xF1); // 0xF1 = 1111 0001

	// we determine the dot product between the second row and the vector and store the result in the second float bin
	r1 = _mm_dp_ps(r1, v, 0xF2); // 0xF2 = 1111 0010
	r2 = _mm_dp_ps(r2, v, 0xF4); // 0xF4 = 1111 0100
	r3 = _mm_dp_ps(r3, v, 0xF8); // 0xF8 = 1111 1000

	// now we blend the results by applying the bit-wise or operator
	__m128 result01 = _mm_or_ps(r0, r1);
	__m128 result23 = _mm_or_ps(r2, r3);
	__m128 result03 = _mm_or_ps(result01, result23);

	StaticMatrix<float, 4, 1> result;
	_mm_storeu_ps(result.data(), result03);

	return result;
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

template <typename T, size_t tRows, size_t tColumns>
inline StaticMatrix<T, tRows, tColumns> StaticMatrix<T, tRows, tColumns>::operator*(const T& value) const
{
	StaticMatrix<T, tRows, tColumns> result;

	T* target = result.data();
	const T* source = data();

	const T* const targetEnd = target + elements();

	while (target != targetEnd)
	{
		*target++ = *source++ * value;
	}

	return result;
}

template <typename T, size_t tRows, size_t tColumns>
inline StaticMatrix<T, tRows, tColumns>& StaticMatrix<T, tRows, tColumns>::operator*=(const T& value)
{
	T* data = matrixValues;
	T* const dataEnd = data + elements();

	while (data != dataEnd)
	{
		*data++ *= value;
	}

	return *this;
}

template <typename T, size_t tRows, size_t tColumns>
inline T StaticMatrix<T, tRows, tColumns>::operator()(const size_t row, const size_t column) const
{
	ocean_assert(row < tRows && column < tColumns);
	return matrixValues[row * tColumns + column];
}

template <typename T, size_t tRows, size_t tColumns>
inline T& StaticMatrix<T, tRows, tColumns>::operator()(const size_t row, const size_t column)
{
	ocean_assert(row < tRows && column < tColumns);
	return matrixValues[row * tColumns + column];
}

template <typename T, size_t tRows, size_t tColumns>
inline T StaticMatrix<T, tRows, tColumns>::operator[](const size_t index) const
{
	ocean_assert(index < tRows * tColumns);
	return matrixValues[index];
}

template <typename T, size_t tRows, size_t tColumns>
inline T& StaticMatrix<T, tRows, tColumns>::operator[](const size_t index)
{
	ocean_assert(index < tRows * tColumns);
	return matrixValues[index];
}

}

#endif // META_OCEAN_MATH_STATIC_MATRIX_H
