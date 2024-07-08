/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_SQUARE_MATRIX_2_H
#define META_OCEAN_MATH_SQUARE_MATRIX_2_H

#include "ocean/math/Math.h"
#include "ocean/math/Equation.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Vector2.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class SquareMatrixT2;

/**
 * Definition of the SquareMatrix2 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see SquareMatrixT2
 * @ingroup math
 */
typedef SquareMatrixT2<Scalar> SquareMatrix2;

/**
 * Instantiation of the SquareMatrixT2 template class using a double precision float data type.
 * @see SquareMatrixT2
 * @ingroup math
 */
typedef SquareMatrixT2<double> SquareMatrixD2;

/**
 * Instantiation of the SquareMatrixT2 template class using a single precision float data type.
 * @see SquareMatrixT2
 * @ingroup math
 */
typedef SquareMatrixT2<float> SquareMatrixF2;

/**
 * Definition of a typename alias for vectors with SquareMatrixT2 objects.
 * @see SquareMatrixT2
 * @ingroup math
 */
template <typename T>
using SquareMatricesT2 = std::vector<SquareMatrixT2<T>>;

/**
 * Definition of a vector holding SquareMatrix2 objects.
 * @see SquareMatrix2
 * @ingroup math
 */
typedef std::vector<SquareMatrix2> SquareMatrices2;

/**
 * This class implements a 2x2 square matrix.
 * The four values are stored in a column aligned order with indices:
 * <pre>
 * | 0 2 |
 * | 1 3 |
 * </pre>
 * @tparam T Data type of matrix elements
 * @see SquareMatrix2, SquareMatrixF2, SquareMatrixD2.
 * @ingroup math
 */
template <typename T>
class SquareMatrixT2
{
  template <typename U> friend class SquareMatrixT2;

	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates a new SquareMatrixT2 object with undefined elements.
		 * Beware: This matrix is neither a zero nor an entity matrix!
		 */
		inline SquareMatrixT2();

		/**
		 * Copy constructor.
		 * @param matrix The matrix to copy
		 */
		SquareMatrixT2(const SquareMatrixT2<T>& matrix) = default;

		/**
		 * Copy constructor for a matrix with difference element data type than T.
		 * @param matrix The matrix to copy
		 * @tparam U The element data type of the second matrix
		 */
		template <typename U>
		inline explicit SquareMatrixT2(const SquareMatrixT2<U>& matrix);

		/**
		 * Creates a new SquareMatrixT2 object.
		 * @param setToIdentity Determines whether an identity matrix will be created, otherwise the matrix is initialized with zeros
		 */
		inline explicit SquareMatrixT2(const bool setToIdentity);

		/**
		 * Creates a new SquareMatrixT2 object by a given diagonal vector.
		 * @param diagonal Diagonal vector for the new matrix
		 */
		inline explicit SquareMatrixT2(const VectorT2<T>& diagonal);

		/**
		 * Creates a new SquareMatrixT2 object by four elements of float type U.
		 * @param arrayValues The four matrix elements defining the new matrix, must be valid
		 * @tparam U The floating point type of the given elements
		 */
		template <typename U>
		explicit SquareMatrixT2(const U* arrayValues);

		/**
		 * Creates a new SquareMatrixT2 object by four elements.
		 * @param arrayValues The four matrix elements defining the new matrix, must be valid
		 */
		explicit SquareMatrixT2(const T* arrayValues);

		/**
		 * Creates a new SquareMatrixT2 object by four elements.
		 * @param arrayValues The four matrix elements defining the new matrix, must be valid
		 * @param valuesRowAligned True, if the given values are stored in a row aligned order; False, if the values are stored in a column aligned order (which is the default case for this matrix)
		 * @tparam U The floating point type of the given elements
		 */
		template <typename U>
		SquareMatrixT2(const U* arrayValues, const bool valuesRowAligned);

		/**
		 * Creates a new SquareMatrixT2 object by four elements.
		 * @param arrayValues The four matrix elements defining the new matrix, must be valid
		 * @param valuesRowAligned True, if the given values are stored in a row aligned order; False, if the values are stored in a column aligned order (which is the default case for this matrix)
		 */
		SquareMatrixT2(const T* arrayValues, const bool valuesRowAligned);

		/**
		 * Creates a 2x2 rotation matrix by 4 given matrix elements.
		 * @param m00 Element of the first row and first column
		 * @param m10 Element of the second row and first column
		 * @param m01 Element of the first row and second column
		 * @param m11 Element of the second row and second column
		 */
		inline SquareMatrixT2(const T& m00, const T& m10, const T& m01, const T& m11);

		/**
		 * Creates a covariance matrix by two eigen values and two corresponding eigen vectors.
		 * @param eigenValue0 First eigen value
		 * @param eigenValue1 Second eigen value
		 * @param eigenVector0 First eigen vector
		 * @param eigenVector1 Second eigen vector
		 */
		SquareMatrixT2(const T eigenValue0, const T eigenValue1, const VectorT2<T>& eigenVector0, const VectorT2<T>& eigenVector1);

		/**
		 * Returns the transposed of this matrix.
		 * @return Transposed matrix
		 */
		inline SquareMatrixT2<T> transposed() const;

		/**
		 * Transposes the matrix.
		 */
		inline void transpose();

		/**
		 * Returns the inverted matrix of this matrix.
		 * This matrix must not be singular.<br>
		 * Beware: This function does not throw an exception if the matrix cannot be inverted.<br>
		 * Thus, ensure that the matrix is invertible before calling this function.<br>
		 * Even better: avoid the usage of this function and call invert() instead.<br>
		 * In case, this matrix is not invertible, this matrix will be returned instead.
		 * @return The inverted matrix
		 * @see invert(), isSingular().
		 */
		inline SquareMatrixT2<T> inverted() const;

		/**
		 * Inverts this matrix in place.
		 * @return True, if the matrix is not singular and could be inverted
		 * @see inverted(), solve().
		 */
		inline bool invert();

		/**
		 * Inverts the matrix and returns the result as parameter.
		 * @param invertedMatrix The resulting inverted matrix
		 * @return True, if the matrix is not singular and could be inverted
		 * @see inverted(), solve().
		 */
		inline bool invert(SquareMatrixT2<T>& invertedMatrix) const;

		/**
		 * Returns the determinant of the matrix.
		 * @return Matrix determinant
		 */
		inline T determinant() const;

		/**
		 * Returns the trace of the matrix which is the sum of the diagonal elements.
		 * @return Trace of the matrix
		 */
		inline T trace() const;

		/**
		 * Solve a simple 2x2 system of linear equations: Ax = b
		 * Beware: The system of linear equations is assumed to be fully determined.
		 * @param b The right-hand side vector
		 * @param x The resulting solution vector
		 * @return True, if the system could be solved
		 * @see invert(), inverted().
		 */
		inline bool solve(const VectorT2<T>& b, VectorT2<T>& x) const;

		/**
		 * Sets the matrix to the identity matrix.
		 */
		inline void toIdentity();

		/**
		 * Sets the matrix to a zero matrix.
		 * @see isNull();
		 */
		inline void toNull();

		/**
		 * Returns the x axis which is the first column of the matrix.
		 * @return Vector with the first column
		 */
		inline VectorT2<T> xAxis() const;

		/**
		 * Returns the y axis which is the middle column of the matrix.
		 * @return Vector with the middle column
		 */
		inline VectorT2<T> yAxis() const;

		/**
		 * Returns a 2D vector with values of the matrix diagonal.
		 * @return Vector with diagonal values
		 */
		inline VectorT2<T> diagonal() const;

		/**
		 * Returns the norm of this matrix that is the sum of the absolute matrix elements.
		 * @return Matrix norm
		 */
		inline T norm() const;

		/**
		 * Returns whether this matrix is a null matrix.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Returns whether this matrix is the identity matrix.
		 * @return True, if so
		 */
		inline bool isIdentity() const;

		/**
		 * Returns whether this matrix is singular (and thus cannot be inverted).
		 * A matrix is singular if the determinant of a matrix is 0.<br>
		 * @return True, if so
		 */
		inline bool isSingular() const;

		/**
		 * Returns whether this matrix is symmetric.
		 * @param epsilon The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isSymmetric(const T epsilon = NumericT<T>::eps()) const;

		/**
		 * Returns whether two matrices are almost identical up to a specified epsilon.
		 * @param matrix Second matrix that will be checked
		 * @param eps The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isEqual(const SquareMatrixT2<T>& matrix, const T eps = NumericT<T>::eps()) const;

		/**
		 * Performs an eigen value analysis.
		 * @param eigenValue0 First eigen value
		 * @param eigenValue1 Second eigen value
		 * @param eigenVector0 First eigen vector
		 * @param eigenVector1 Second eigen vector
		 * @return True, if succeeded
		 */
		bool eigenSystem(T& eigenValue0, T& eigenValue1, VectorT2<T>& eigenVector0, VectorT2<T>& eigenVector1) const;

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
		 * Copies the elements of this matrix to an array with floating point values of type U.
		 * @param arrayValues Array with 4 floating point values of type U receiving the elements of this matrix
		 * @param columnAligned True, if the target elements are column aligned, false to target row aligned elements
		 * @tparam U Floating point type
		 */
		template <typename U>
		inline void copyElements(U* arrayValues, const bool columnAligned = true) const;

		/**
		 * Copies the elements of this matrix to an array with floating point values of type T.
		 * @param arrayValues Array with 4 floating point values  of type T receiving the elements of this matrix
		 * @param columnAligned True, if the target elements are column aligned, false to target row aligned elements
		 */
		inline void copyElements(T* arrayValues, const bool columnAligned = true) const;

		/**
		 * Default copy assignment operator.
		 * @return Reference to this object
		 */
		SquareMatrixT2<T>& operator=(const SquareMatrixT2<T>&) = default;

		/**
		 * Returns whether two matrices are identical up to a small epsilon.
		 * @param matrix Right operand
		 * @return True, if so
		 */
		inline bool operator==(const SquareMatrixT2<T>& matrix) const;

		/**
		 * Returns whether two matrices are not identical up to a small epsilon.
		 * @param matrix Right operand
		 * @return True, if so
		 */
		inline bool operator!=(const SquareMatrixT2<T>& matrix) const;

		/**
		 * Adds two matrices.
		 * @param matrix Right operand
		 * @return Sum matrix
		 */
		inline SquareMatrixT2<T> operator+(const SquareMatrixT2<T>& matrix) const;

		/**
		 * Adds and assigns two matrices.
		 * @param matrix Right operand
		 * @return Reference to this object
		 */
		inline SquareMatrixT2<T>& operator+=(const SquareMatrixT2<T>& matrix);

		/**
		 * Subtracts two matrices.
		 * @param matrix Right operand
		 * @return Difference matrix
		 */
		inline SquareMatrixT2<T> operator-(const SquareMatrixT2<T>& matrix) const;

		/**
		 * Subtracts and assigns two matrices.
		 * @param matrix Right operand
		 * @return Reference to this object
		 */
		inline SquareMatrixT2<T>& operator-=(const SquareMatrixT2<T>& matrix);

		/**
		 * Returns the negative matrix of this matrix (all matrix elements are multiplied by -1).
		 * @return Resulting negative matrix
		 */
		inline SquareMatrixT2<T> operator-() const;

		/**
		 * Multiplies two matrices.
		 * @param matrix Right operand
		 * @return Product matrix
		 */
		inline SquareMatrixT2<T> operator*(const SquareMatrixT2<T>& matrix) const;

		/**
		 * Multiplies and assigns two matrices.
		 * @param matrix Right operand
		 * @return Reference to this object
		 */
		inline SquareMatrixT2<T>& operator*=(const SquareMatrixT2<T>& matrix);

		/**
		 * Multiply operator for a 2D vector.
		 * @param vector Right operand
		 * @return Resulting 2D vector
		 */
		inline VectorT2<T> operator*(const VectorT2<T>& vector) const;

		/**
		 * Multiplies this matrix with a scalar value.
		 * @param value Right operand
		 * @return Resulting matrix
		 */
		inline SquareMatrixT2<T> operator*(const T value) const;

		/**
		 * Multiplies and assigns this matrix with a scalar value.
		 * @param value right operand
		 * @return Reference to this object
		 */
		inline SquareMatrixT2<T>& operator*=(const T value);

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index Index of the element to return [0, 4]
		 * @return Specified element
		 */
		inline T operator[](const unsigned int index) const;

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index Index of the element to return [0, 4]
		 * @return Specified element
		 */
		inline T& operator[](const unsigned int index);

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param row Row of the element to return [0, 1]
		 * @param column Column of the element to return [0, 1]
		 * @return Specified element
		 */
		inline T operator()(const unsigned int row, const unsigned int column) const;

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param row Row of the element to return [0, 1]
		 * @param column Column of the element to return [0, 1]
		 * @return Specified element
		 */
		inline T& operator()(const unsigned int row, const unsigned int column);

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index Index of the element to return [0, 8]
		 * @return Specified element
		 */
		inline T operator()(const unsigned int index) const;

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index Index of the element to return [0, 8]
		 * @return Specified element
		 */
		inline T& operator()(const unsigned int index);

		/**
		 * Access operator.
		 * @return Pointer to the internal values
		 */
		inline const T* operator()() const;

		/**
		 * Access operator.
		 * @return Pointer to the internal values
		 */
		inline T* operator()();

		/**
		 * Hash function.
		 * @param matrix The matrix for which the hash value will be determined
		 * @return The resulting hash value
		 */
		inline size_t operator()(const SquareMatrixT2<T>& matrix) const;

		/**
		 * Returns the number of elements this matrix has.
		 * @return The number of elements, always 4
		 */
		static inline size_t elements();

		/**
		 * Converts matrices with specific data type to matrices with different data type.
		 * @param matrices The matrices to convert
		 * @return The converted matrices
		 * @tparam U The element data type of the matrices to convert
		 */
		template <typename U>
		static inline std::vector< SquareMatrixT2<T> > matrices2matrices(const std::vector< SquareMatrixT2<U> >& matrices);

		/**
		 * Converts matrices with specific data type to matrices with different data type.
		 * @param matrices The matrices to convert
		 * @param size The number of matrices to convert
		 * @return The converted matrices
		 * @tparam U The element data type of the matrices to convert
		 */
		template <typename U>
		static inline std::vector< SquareMatrixT2<T> > matrices2matrices(const SquareMatrixT2<U>* matrices, const size_t size);

	protected:

		/// The four values of the matrix.
		T values[4];
};

template <typename T>
inline SquareMatrixT2<T>::SquareMatrixT2()
{
	// nothing to do here
}

template <typename T>
template <typename U>
inline SquareMatrixT2<T>::SquareMatrixT2(const SquareMatrixT2<U>& matrix)
{
	values[0] = T(matrix.values[0]);
	values[1] = T(matrix.values[1]);
	values[2] = T(matrix.values[2]);
	values[3] = T(matrix.values[3]);
}

template <typename T>
inline SquareMatrixT2<T>::SquareMatrixT2(const bool setToIdentity)
{
	if (setToIdentity)
	{
		values[0] = T(1.0);
		values[1] = T(0.0);
		values[2] = T(0.0);
		values[3] = T(1.0);
	}
	else
	{
		values[0] = T(0.0);
		values[1] = T(0.0);
		values[2] = T(0.0);
		values[3] = T(0.0);
	}
}

template <typename T>
inline SquareMatrixT2<T>::SquareMatrixT2(const VectorT2<T>& diagonal)
{
	values[0] = diagonal[0];
	values[1] = T(0.0);
	values[2] = T(0.0);
	values[3] = diagonal[1];
}

template <typename T>
template <typename U>
SquareMatrixT2<T>::SquareMatrixT2(const U* arrayValues)
{
	ocean_assert(arrayValues);

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		values[n] = T(arrayValues[n]);
	}
}

template <typename T>
SquareMatrixT2<T>::SquareMatrixT2(const T* arrayValues)
{
	ocean_assert(arrayValues);
	memcpy(values, arrayValues, sizeof(T) * 4);
}

template <typename T>
template <typename U>
SquareMatrixT2<T>::SquareMatrixT2(const U* arrayValues, const bool valuesRowAligned)
{
	ocean_assert(arrayValues);

	if (valuesRowAligned)
	{
		values[0] = T(arrayValues[0]);
		values[1] = T(arrayValues[2]);
		values[2] = T(arrayValues[1]);
		values[3] = T(arrayValues[3]);

	}
	else
	{
		for (unsigned int n = 0u; n < 4u; ++n)
			values[n] = T(arrayValues[n]);
	}
}

template <typename T>
SquareMatrixT2<T>::SquareMatrixT2(const T* arrayValues, const bool valuesRowAligned)
{
	ocean_assert(arrayValues);

	if (valuesRowAligned)
	{
		values[0] = arrayValues[0];
		values[1] = arrayValues[2];
		values[2] = arrayValues[1];
		values[3] = arrayValues[3];
	}
	else
	{
		memcpy(values, arrayValues, sizeof(T) * 4);
	}
}

template <typename T>
inline SquareMatrixT2<T>::SquareMatrixT2(const T& m00, const T& m10, const T& m01, const T& m11)
{
	values[0] = m00;
	values[1] = m10;

	values[2] = m01;
	values[3] = m11;
}

template <typename T>
SquareMatrixT2<T>::SquareMatrixT2(const T eigenValue0, const T eigenValue1, const VectorT2<T>& eigenVector0, const VectorT2<T>& eigenVector1)
{
	ocean_assert(NumericT<T>::isEqual(eigenVector0.length(), T(1.0)));
	ocean_assert(NumericT<T>::isEqual(eigenVector1.length(), T(1.0)));

	const T det = eigenVector0.x() * eigenVector1.y() - eigenVector1.x() * eigenVector0.y();
	ocean_assert(NumericT<T>::isNotEqualEps(det));

	values[0] = (eigenVector0.x() * eigenValue0 * eigenVector1.y() - eigenVector0.y() * eigenValue1 * eigenVector1.x()) / det;
	values[1] = (eigenValue0 - eigenValue1) * eigenVector0.y() * eigenVector1.y() / det;
	values[2] = (eigenValue1 - eigenValue0) * eigenVector0.x() * eigenVector1.x() / det;
	values[3] = (eigenVector0.x() * eigenValue1 * eigenVector1.y() - eigenVector0.y() * eigenValue0 * eigenVector1.x()) / det;

#ifdef OCEAN_DEBUG
	if (!std::is_same<Scalar, float>::value)
	{
		T debugEigenValue0, debugEigenValue1;
		VectorT2<T> debugEigenVector0, debugEigenVector1;
		ocean_assert(eigenSystem(debugEigenValue0, debugEigenValue1, debugEigenVector0, debugEigenVector1));

		ocean_assert(NumericT<T>::isEqual(debugEigenValue0, eigenValue0, NumericT<T>::eps() * 10));
		ocean_assert(NumericT<T>::isEqual(debugEigenValue1, eigenValue1, NumericT<T>::eps() * 10));
		ocean_assert(debugEigenVector0.isEqual(eigenVector0, NumericT<T>::weakEps() * 10) || debugEigenVector0.isEqual(-eigenVector0, NumericT<T>::weakEps() * 10));
		ocean_assert(debugEigenVector1.isEqual(eigenVector1, NumericT<T>::weakEps() * 10) || debugEigenVector1.isEqual(-eigenVector1, NumericT<T>::weakEps() * 10));
	}
#endif // OCEAN_DEBUG
}

template <typename T>
inline SquareMatrixT2<T> SquareMatrixT2<T>::transposed() const
{
	return SquareMatrixT2<T>(values[0], values[2], values[1], values[3]);
}

template <typename T>
inline void SquareMatrixT2<T>::transpose()
{
	const T tmp = values[1];
	values[1] = values[2];
	values[2] = tmp;
}

template <typename T>
inline SquareMatrixT2<T> SquareMatrixT2<T>::inverted() const
{
	SquareMatrixT2<T> result;

	if (!invert(result))
	{
		ocean_assert(false && "Could not invert the matrix.");
		return *this;
	}

	return result;
}

template <typename T>
inline bool SquareMatrixT2<T>::invert()
{
	const T det = determinant();
	if (NumericT<T>::isEqualEps(det))
	{
		return false;
	}

	const T factor = T(1.0) / det;

	*this = SquareMatrixT2<T>(values[3] * factor, -values[1] * factor, -values[2] * factor, values[0] * factor);

	return true;
}

template <typename T>
inline bool SquareMatrixT2<T>::invert(SquareMatrixT2<T>& invertedMatrix) const
{
	const T det = determinant();
	if (NumericT<T>::isEqualEps(det))
	{
		return false;
	}

	const T factor = T(1.0) / det;

	invertedMatrix = SquareMatrixT2<T>(values[3] * factor, -values[1] * factor, -values[2] * factor, values[0] * factor);

#ifdef OCEAN_INTENSIVE_DEBUG
	if (!std::is_same<T, float>::value)
	{
		const SquareMatrixT2<T> test(*this * invertedMatrix);
		const SquareMatrixT2<T> entity(true);

		T sqrDistance = T(0);
		for (unsigned int n = 0; n < 4u; ++n)
			sqrDistance += NumericT<T>::sqr(test[n] - entity[n]);
		const T distance = NumericT<T>::sqrt(sqrDistance * T(0.25));

		if (NumericT<T>::isWeakEqualEps(distance) == false)
		{
			T absolusteAverageEnergy = 0;
			for (unsigned int n = 0u; n < 4u; ++n)
				absolusteAverageEnergy += NumericT<T>::abs(values[n]);
			absolusteAverageEnergy *= T(0.25);

			// we expect/accept for each magnitude (larger than 1) a zero-inaccuracy of one magnitude (and we again comare it with the weak eps)

			if (absolusteAverageEnergy <= 1)
				ocean_assert_accuracy(!"This should never happen!");
			else
			{
				const T adjustedDistance = distance / absolusteAverageEnergy;
				ocean_assert_accuracy(NumericT<T>::isWeakEqualEps(adjustedDistance));
			}
		}
	}
#endif // OCEAN_DEBUG

	return true;
}

template <typename T>
inline T SquareMatrixT2<T>::determinant() const
{
	return values[0] * values[3] - values[1] * values[2];
}

template <typename T>
inline T SquareMatrixT2<T>::trace() const
{
	return values[0] + values[3];
}

template <typename T>
inline bool SquareMatrixT2<T>::solve(const VectorT2<T>& b, VectorT2<T>& x) const
{
	// Solve this system of linear equations using the good ol' Gauss elimination
	//
	// Step 0:
	// |a0 a2|   |x0|   |b0|
	// |a1 a3| x |x1| = |b1|
	//
	//
	// Step 1:
	// |a0 a2|   |x0|   |b0|
	// |0  u0| x |x1| = |v0|  <- (row1 * a0) - (row0 * a1)
	//
	const T u0 = (values[0] * values[3]) - (values[1] * values[2]);
	const T v0 = (values[0] * b[1]) - (values[1] * b[0]);

	// x1 = v0 / u0

	if (NumericT<T>::isNotEqualEps(u0) && NumericT<T>::isNotEqualEps(values[0]))
	{
		x[1] = v0 / u0;
		x[0] = (b[0] - (values[2] * x[1])) / values[0];

		return true;
	}

	return false;
}

template <typename T>
inline void SquareMatrixT2<T>::toIdentity()
{
	values[0] = T(1.0);
	values[1] = T(0.0);
	values[2] = T(0.0);
	values[3] = T(1.0);
}

template <typename T>
inline void SquareMatrixT2<T>::toNull()
{
	values[0] = T(0.0);
	values[1] = T(0.0);
	values[2] = T(0.0);
	values[3] = T(0.0);
}

template <typename T>
inline VectorT2<T> SquareMatrixT2<T>::xAxis() const
{
	return VectorT2<T>(values[0], values[1]);
}

template <typename T>
inline VectorT2<T> SquareMatrixT2<T>::yAxis() const
{
	return VectorT2<T>(values[2], values[3]);
}

template <typename T>
inline VectorT2<T> SquareMatrixT2<T>::diagonal() const
{
	return VectorT2<T>(values[0], values[3]);
}

template <typename T>
inline T SquareMatrixT2<T>::norm() const
{
	return NumericT<T>::abs(values[0]) + NumericT<T>::abs(values[1]) + NumericT<T>::abs(values[2]) + NumericT<T>::abs(values[3]);
}

template <typename T>
inline bool SquareMatrixT2<T>::isNull() const
{
	return NumericT<T>::isEqualEps(values[0]) && NumericT<T>::isEqualEps(values[1]) && NumericT<T>::isEqualEps(values[2]) && NumericT<T>::isEqualEps(values[3]);
}

template <typename T>
inline bool SquareMatrixT2<T>::isIdentity() const
{
	return NumericT<T>::isEqual(values[0], 1) && NumericT<T>::isEqualEps(values[1]) && NumericT<T>::isEqualEps(values[2]) && NumericT<T>::isEqual(values[3], 1);
}

template <typename T>
inline bool SquareMatrixT2<T>::isSingular() const
{
	return NumericT<T>::isEqualEps(determinant());
}

template <typename T>
inline bool SquareMatrixT2<T>::isSymmetric(const T epsilon) const
{
	ocean_assert(epsilon >= T(0));

	return NumericT<T>::isEqual(values[1], values[2], epsilon);
}

template <typename T>
inline bool SquareMatrixT2<T>::isEqual(const SquareMatrixT2<T>& matrix, const T eps) const
{
	return NumericT<T>::isEqual(values[0], matrix.values[0], eps) && NumericT<T>::isEqual(values[1], matrix.values[1], eps)
			&& NumericT<T>::isEqual(values[2], matrix.values[2], eps) && NumericT<T>::isEqual(values[3], matrix.values[3], eps);
}

template <typename T>
inline const T* SquareMatrixT2<T>::data() const
{
	return values;
}

template <typename T>
inline T* SquareMatrixT2<T>::data()
{
	return values;
}

template <typename T>
template <typename U>
inline void SquareMatrixT2<T>::copyElements(U* arrayValues, const bool columnAligned) const
{
	ocean_assert(arrayValues);

	if (columnAligned)
	{
		arrayValues[0] = U(values[0]);
		arrayValues[1] = U(values[1]);
		arrayValues[2] = U(values[2]);
		arrayValues[3] = U(values[3]);
	}
	else
	{
		arrayValues[0] = U(values[0]);
		arrayValues[1] = U(values[2]);
		arrayValues[2] = U(values[1]);
		arrayValues[3] = U(values[3]);
	}
}

template <typename T>
inline void SquareMatrixT2<T>::copyElements(T* arrayValues, const bool columnAligned) const
{
	ocean_assert(arrayValues);

	if (columnAligned)
	{
		arrayValues[0] = values[0];
		arrayValues[1] = values[1];
		arrayValues[2] = values[2];
		arrayValues[3] = values[3];
	}
	else
	{
		arrayValues[0] = values[0];
		arrayValues[1] = values[2];
		arrayValues[2] = values[1];
		arrayValues[3] = values[3];
	}
}

template <typename T>
inline bool SquareMatrixT2<T>::operator==(const SquareMatrixT2<T>& matrix) const
{
	return isEqual(matrix);
}

template <typename T>
inline bool SquareMatrixT2<T>::operator!=(const SquareMatrixT2<T>& matrix) const
{
	return !(*this == matrix);
}

template <typename T>
inline SquareMatrixT2<T> SquareMatrixT2<T>::operator+(const SquareMatrixT2<T>& matrix) const
{
	return SquareMatrixT2<T>(values[0] + matrix.values[0], values[1] + matrix.values[1], values[2] + matrix.values[2], values[3] + matrix.values[3]);
}

template <typename T>
inline SquareMatrixT2<T>& SquareMatrixT2<T>::operator+=(const SquareMatrixT2<T>& matrix)
{
	values[0] += matrix.values[0];
	values[1] += matrix.values[1];
	values[2] += matrix.values[2];
	values[3] += matrix.values[3];

	return *this;
}

template <typename T>
inline SquareMatrixT2<T> SquareMatrixT2<T>::operator-(const SquareMatrixT2<T>& matrix) const
{
	return SquareMatrixT2<T>(values[0] - matrix.values[0], values[1] - matrix.values[1], values[2] - matrix.values[2], values[3] - matrix.values[3]);
}

template <typename T>
inline SquareMatrixT2<T>& SquareMatrixT2<T>::operator-=(const SquareMatrixT2<T>& matrix)
{
	values[0] -= matrix.values[0];
	values[1] -= matrix.values[1];
	values[2] -= matrix.values[2];
	values[3] -= matrix.values[3];

	return *this;
}

template <typename T>
inline SquareMatrixT2<T> SquareMatrixT2<T>::operator-() const
{
	SquareMatrixT2<T> result;

	result.values[ 0] = -values[ 0];
	result.values[ 1] = -values[ 1];
	result.values[ 2] = -values[ 2];
	result.values[ 3] = -values[ 3];

	return result;
}

template <typename T>
inline SquareMatrixT2<T> SquareMatrixT2<T>::operator*(const SquareMatrixT2<T>& matrix) const
{
	return SquareMatrixT2<T>(values[0] * matrix.values[0] + values[2] * matrix.values[1],
							values[1] * matrix.values[0] + values[3] * matrix.values[1],
							values[0] * matrix.values[2] + values[2] * matrix.values[3],
							values[1] * matrix.values[2] + values[3] * matrix.values[3]);
}

template <typename T>
inline SquareMatrixT2<T>& SquareMatrixT2<T>::operator*=(const SquareMatrixT2<T>& matrix)
{
	*this = *this * matrix;
	return *this;
}

template <typename T>
inline VectorT2<T> SquareMatrixT2<T>::operator*(const VectorT2<T>& vector) const
{
	return VectorT2<T>(values[0] * vector[0] + values[2] * vector[1],
						values[1] * vector[0] + values[3] * vector[1]);
}

template <typename T>
inline SquareMatrixT2<T> SquareMatrixT2<T>::operator*(const T value) const
{
	return SquareMatrixT2<T>(values[0] * value, values[1] * value, values[2] * value, values[3] * value);
}

template <typename T>
inline SquareMatrixT2<T>& SquareMatrixT2<T>::operator*=(const T value)
{
	values[0] *= value;
	values[1] *= value;
	values[2] *= value;
	values[3] *= value;

	return *this;
}

template <typename T>
inline T SquareMatrixT2<T>::operator[](const unsigned int index) const
{
	ocean_assert(index < 4u);
	return values[index];
}

template <typename T>
inline T& SquareMatrixT2<T>::operator[](const unsigned int index)
{
	ocean_assert(index < 4u);
	return values[index];
}

template <typename T>
inline T SquareMatrixT2<T>::operator()(const unsigned int row, const unsigned int column) const
{
	ocean_assert(row < 2u && column < 2u);
	return values[column * 2u + row];
}

template <typename T>
inline T& SquareMatrixT2<T>::operator()(const unsigned int row, const unsigned int column)
{
	ocean_assert(row < 2u && column < 2u);
	return values[column * 2u + row];
}

template <typename T>
inline T SquareMatrixT2<T>::operator()(const unsigned int index) const
{
	ocean_assert(index < 4u);
	return values[index];
}

template <typename T>
inline T& SquareMatrixT2<T>::operator()(const unsigned int index)
{
	ocean_assert(index < 4u);
	return values[index];
}

template <typename T>
inline const T* SquareMatrixT2<T>::operator()() const
{
	return values;
}

template <typename T>
inline T* SquareMatrixT2<T>::operator()()
{
	return values;
}

template <typename T>
inline size_t SquareMatrixT2<T>::operator()(const SquareMatrixT2<T>& matrix) const
{
	size_t seed = std::hash<T>{}(matrix.values[0]);

	for (unsigned int n = 1u; n < 4u; ++n)
	{
		seed ^= std::hash<T>{}(matrix.values[n]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	return seed;
}

template <typename T>
inline size_t SquareMatrixT2<T>::elements()
{
	return 4;
}

template <typename T>
bool SquareMatrixT2<T>::eigenSystem(T& eigenValue0, T& eigenValue1, VectorT2<T>& eigenVector0, VectorT2<T>& eigenVector1) const
{
	if (isNull())
	{
		return false;
	}

	const T tr = trace();
	const T det = determinant();

	if (!EquationT<T>::solveQuadratic(T(1.0), -tr, det, eigenValue0, eigenValue1))
	{
		return false;
	}

	if (eigenValue0 < eigenValue1)
	{
		std::swap(eigenValue0, eigenValue1);
	}

	if (NumericT<T>::isNotEqualEps(values[2]))
	{
		const T factor = T(1.0) / values[2];
		eigenVector0 = VectorT2<T>(1, (eigenValue0 - values[0]) * factor);
		eigenVector1 = VectorT2<T>(1, (eigenValue1 - values[0]) * factor);
	}
	else if (NumericT<T>::isNotEqualEps(values[1]))
	{
		const T factor = T(1.0) / values[1];

		eigenVector0 = VectorT2<T>((eigenValue0 - values[3]) * factor, T(1.0));
		eigenVector1 = VectorT2<T>((eigenValue1 - values[3]) * factor, T(1.0));
	}
	else
	{
		if (NumericT<T>::isNotEqual(eigenValue0, values[3]))
		{
			eigenVector0 = VectorT2<T>(1, values[1] / (eigenValue0 - values[3]));
		}
		else
		{
			if (NumericT<T>::isEqual(eigenValue0, values[0]))
			{
				eigenVector0 = VectorT2<T>(1, 0);
			}
			else
			{
				eigenVector0 = VectorT2<T>(values[2] / (eigenValue0 - values[0]), T(1.0));
			}
		}

		if (NumericT<T>::isNotEqual(values[3], eigenValue1))
		{
			eigenVector1 = VectorT2<T>(1, values[1] / (eigenValue1 - values[3]));
		}
		else
		{
			if (NumericT<T>::isEqual(eigenValue1, values[0]))
			{
				eigenVector1 = VectorT2<T>(0, 1);
			}
			else
			{
				eigenVector1 = VectorT2<T>(values[2] / (eigenValue1 - values[0]), T(1.0));
			}
		}
	}

	eigenVector0.normalize();
	eigenVector1.normalize();

	ocean_assert((std::is_same<T, float>::value) || (*this * eigenVector0).isEqual(eigenVector0 * eigenValue0, NumericT<T>::weakEps()));
	ocean_assert((std::is_same<T, float>::value) || (*this * eigenVector1).isEqual(eigenVector1 * eigenValue1, NumericT<T>::weakEps()));

	return true;
}

template <typename T>
template <typename U>
inline std::vector< SquareMatrixT2<T> > SquareMatrixT2<T>::matrices2matrices(const std::vector< SquareMatrixT2<U> >& matrices)
{
	std::vector< SquareMatrixT2<T> > result;
	result.reserve(matrices.size());

	for (typename std::vector< SquareMatrixT2<U> >::const_iterator i = matrices.begin(); i != matrices.end(); ++i)
	{
		result.push_back(SquareMatrixT2<T>(*i));
	}

	return result;
}

template <>
template <>
inline std::vector< SquareMatrixT2<float> > SquareMatrixT2<float>::matrices2matrices(const std::vector< SquareMatrixT2<float> >& matrices)
{
	return matrices;
}

template <>
template <>
inline std::vector< SquareMatrixT2<double> > SquareMatrixT2<double>::matrices2matrices(const std::vector< SquareMatrixT2<double> >& matrices)
{
	return matrices;
}

template <typename T>
template <typename U>
inline std::vector< SquareMatrixT2<T> > SquareMatrixT2<T>::matrices2matrices(const SquareMatrixT2<U>* matrices, const size_t size)
{
	std::vector< SquareMatrixT2<T> > result;
	result.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		result.push_back(SquareMatrixT2<T>(matrices[n]));
	}

	return result;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const SquareMatrixT2<T>& matrix)
{
	stream << "|" << matrix(0, 0) << ", " << matrix(0, 1) << "|" << std::endl;
	stream << "|" << matrix(1, 0) << ", " << matrix(1, 1) << "|";

	return stream;
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const SquareMatrixT2<T>& matrix)
{
	return messageObject << "|" << matrix(0, 0) << ", " << matrix(0, 1) << "|\n|" << matrix(1, 0) << ", " << matrix(1, 1) << "|";
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const SquareMatrixT2<T>& matrix)
{
	return messageObject << "|" << matrix(0, 0) << ", " << matrix(0, 1) << "|\n|" << matrix(1, 0) << ", " << matrix(1, 1) << "|";
}

}

#endif // META_OCEAN_MATH_SQUARE_MATRIX_2_H
