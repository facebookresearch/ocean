/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_SQUARE_MATRIX_4_H
#define META_OCEAN_MATH_SQUARE_MATRIX_4_H

#include "ocean/math/Math.h"
#include "ocean/math/Equation.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"
#include "ocean/math/Vector4.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class AnyCameraT;

// Forward declaration.
template <typename T> class HomogenousMatrixT4;

// Forward declaration.
template <typename T> class SquareMatrixT3;

// Forward declaration.
template <typename T> class SquareMatrixT4;

/**
 * Definition of the SquareMatrix4 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see SquareMatrixT4
 * @ingroup math
 */
typedef SquareMatrixT4<Scalar> SquareMatrix4;

/**
 * Instantiation of the SquareMatrixT4 template class using a double precision float data type.
 * @see SquareMatrixT4
 * @ingroup math
 */
typedef SquareMatrixT4<double> SquareMatrixD4;

/**
 * Instantiation of the SquareMatrixT4 template class using a double precision float data type.
 * @see SquareMatrixT4
 * @ingroup math
 */
typedef SquareMatrixT4<float> SquareMatrixF4;

/**
 * Definition of a typename alias for vectors with SquareMatrixT4 objects.
 * @see SquareMatrixT4
 * @ingroup math
 */
template <typename T>
using SquareMatricesT4 = std::vector<SquareMatrixT4<T>>;

/**
 * Definition of a vector holding SquareMatrix4 objects.
 * @see SquareMatrix4
 * @ingroup math
 */
typedef std::vector<SquareMatrix4> SquareMatrices4;

/**
 * This class implements a 4x4 square matrix.
 * The values are stored in a column aligned order with indices:
 * <pre>
 * | 0  4  8  12 |
 * | 1  5  9  13 |
 * | 2  6  10 14 |
 * | 3  7  11 15 |
 * </pre>
 * @tparam T Data type of matrix elements
 * @see SquareMatrix4, SquareMatrixF4, SquareMatrixD4.
 * @ingroup math
 */
template <typename T>
class SquareMatrixT4
{
	template <typename U> friend class SquareMatrixT4;

	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates a new SquareMatrixT4 object with undefined elements.
		 * Beware: This matrix is neither a zero nor an entity matrix!
		 */
		inline SquareMatrixT4();

		/**
		 * Copy constructor.
		 * @param matrix The matrix to copy
		 */
		SquareMatrixT4(const SquareMatrixT4<T>& matrix) = default;

		/**
		 * Copy constructor for a matrix with difference element data type than T.
		 * @param matrix The matrix to copy
		 * @tparam U The element data type of the second matrix
		 */
		template <typename U>
		inline explicit SquareMatrixT4(const SquareMatrixT4<U>& matrix);

		/**
		 * Creates a new SquareMatrixT4 object.
		 * @param setToIdentity Determines whether a entity matrix will be created
		 */
		explicit SquareMatrixT4(const bool setToIdentity);

		/**
		 * Creates a new SquareMatrixT4 object by an array of at least sixteen elements of float type U.
		 * @param arrayValues The sixteen matrix elements defining the new matrix, must be valid
		 * @tparam U The floating point type of the given elements
		 */
		template <typename U>
		explicit SquareMatrixT4(const U* arrayValues);

		/**
		 * Creates a new SquareMatrixT4 object by an array of at least sixteen elements.
		 * @param arrayValues The sixteen matrix elements defining the new matrix, must be valid
		 */
		explicit SquareMatrixT4(const T* arrayValues);

		/**
		 * Creates a new SquareMatrixT4 object by an array of at least sixteen elements of float type U.
		 * @param arrayValues The sixteen matrix elements defining the new matrix, must be valid
		 * @param valuesRowAligned True, if the given values are stored in a row aligned order; False, if the values are stored in a column aligned order (which is the default case for this matrix)
		 * @tparam U The floating point type of the given elements
		 */
		template <typename U>
		SquareMatrixT4(const U* arrayValues, const bool valuesRowAligned);

		/**
		 * Creates a new SquareMatrixT4 object by an array of at least sixteen elements.
		 * @param valuesRowAligned True, if the given values are stored in a row aligned order; False, if the values are stored in a column aligned order (which is the default case for this matrix)
		 * @param arrayValues The sixteen matrix elements defining the new matrix, must be valid
		 */
		SquareMatrixT4(const T* arrayValues, const bool valuesRowAligned);

		/**
		 * Creates a new SquareMatrixT4 object by given transformation matrix.
		 * @param transformation The transformation matrix to copy
		 */
		explicit SquareMatrixT4(const HomogenousMatrixT4<T>& transformation);

		/**
		 * Creates a new SquareMatrixT4 object by given 3x3 sub matrix.
		 * The lower and right elements of the 4x4 square matrix are set to zero.
		 * @param subMatrix 3x3 subMatrix defining the upper left elements of the new matrix
		 */
		explicit SquareMatrixT4(const SquareMatrixT3<T>& subMatrix);

		/**
		 * Creates a new SquareMatrixT4 object by a given diagonal vector.
		 * @param diagonal The diagonal vector for the new matrix
		 */
		explicit SquareMatrixT4(const VectorT4<T>& diagonal);

		/**
		 * Returns the transposed of this matrix.
		 * @return Transposed matrix
		 */
		SquareMatrixT4<T> transposed() const;

		/**
		 * Transposes the matrix.
		 */
		void transpose();

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
		SquareMatrixT4<T> inverted() const;

		/**
		 * Inverts this matrix in place.
		 * @return True, if the matrix is not singular and could be inverted
		 * @see inverted().
		 */
		bool invert();

		/**
		 * Inverts the matrix and returns the result.
		 * @param invertedMatrix The resulting inverted matrix
		 * @return True, if the matrix is not singular and could be inverted
		 * @see inverted().
		 */
		bool invert(SquareMatrixT4<T>& invertedMatrix) const;

		/**
		 * Returns the determinant of the matrix.
		 * @return Matrix determinant
		 */
		T determinant() const;

		/**
		 * Returns the trace of the matrix which is the sum of the diagonal elements.
		 * @return Trace of the matrix
		 */
		T trace() const;

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
		 * Returns whether this matrix is a null matrix.
		 * @return True, if so
		 */
		bool isNull() const;

		/**
		 * Returns whether this matrix is the identity matrix.
		 * @return True, if so
		 */
		bool isIdentity() const;

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
		inline bool isEqual(const SquareMatrixT4<T>& matrix, const T eps = NumericT<T>::eps()) const;

		/**
		 * Performs an eigen value analysis.
		 * @param eigenValues Vector holding the three eigen values
		 * @param eigenVectors Matrix holding the three corresponding eigen vectors
		 * @return True, if succeeded
		 */
		bool eigenSystem(VectorT4<T>& eigenValues, SquareMatrixT4<T>& eigenVectors);

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
		 * @param arrayValues Array with 16 floating point values of type U receiving the elements of this matrix
		 * @tparam U Floating point type
		 */
		template <typename U>
		void copyElements(U* arrayValues) const;

		/**
		 * Copies the elements of this matrix to an array with floating point values.
		 * @param arrayValues Array with floating point values receiving the elements of this matrix
		 */
		void copyElements(T* arrayValues) const;

		/**
		 * Default assign operator.
		 * @return Reference to this object
		 */
		SquareMatrixT4<T>& operator=(const SquareMatrixT4<T>&) = default;

		/**
		 * Returns whether two matrices are identical up to a small epsilon.
		 * @param matrix Right operand
		 * @return True, if so
		 */
		bool operator==(const SquareMatrixT4<T>& matrix) const;

		/**
		 * Returns whether two matrices are not identical up to a small epsilon.
		 * @param matrix Right operand
		 * @return True, if so
		 */
		inline bool operator!=(const SquareMatrixT4<T>& matrix) const;

		/**
		 * Adds two matrices.
		 * @param matrix Right operand
		 * @return Sum matrix
		 */
		SquareMatrixT4<T> operator+(const SquareMatrixT4<T>& matrix) const;

		/**
		 * Adds and assigns two matrices.
		 * @param matrix Right operand
		 * @return Reference to this object
		 */
		SquareMatrixT4<T>& operator+=(const SquareMatrixT4<T>& matrix);

		/**
		 * Subtracts two matrices.
		 * @param matrix Right operand
		 * @return Difference matrix
		 */
		SquareMatrixT4<T> operator-(const SquareMatrixT4<T>& matrix) const;

		/**
		 * Subtracts and assigns two matrices.
		 * @param matrix Right operand
		 * @return Reference to this object
		 */
		SquareMatrixT4<T>& operator-=(const SquareMatrixT4<T>& matrix);

		/**
		 * Returns the negative matrix of this matrix (all matrix elements are multiplied by -1).
		 * @return Resulting negative matrix
		 */
		inline SquareMatrixT4<T> operator-() const;

		/**
		 * Multiplies two matrices.
		 * @param matrix Right operand
		 * @return Product matrix
		 */
		OCEAN_FORCE_INLINE SquareMatrixT4<T> operator*(const SquareMatrixT4<T>& matrix) const;

		/**
		 * Multiplies two matrices.
		 * @param matrix Right operand
		 * @return Product matrix
		 */
		OCEAN_FORCE_INLINE SquareMatrixT4<T> operator*(const HomogenousMatrixT4<T>& matrix) const;

		/**
		 * Multiplies and assigns two matrices.
		 * @param matrix Right operand
		 * @return Reference to this object
		 */
		OCEAN_FORCE_INLINE SquareMatrixT4<T>& operator*=(const SquareMatrixT4<T>& matrix);

		/**
		 * Multiplies and assigns two matrices.
		 * @param matrix Right operand
		 * @return Reference to this object
		 */
		OCEAN_FORCE_INLINE SquareMatrixT4<T>& operator*=(const HomogenousMatrixT4<T>& matrix);

		/**
		 * Multiply operator for a 3D vector.
		 * The 3D vector is interpreted as a 4D vector with fourth component equal to 1.<br>
		 * The final result will be de-homogenizated to provide a 3D vector result.<br>
		 * Beware the dot product of the last row with the vector must not be zero!
		 * @param vector Right operand
		 * @return Resulting 3D vector
		 */
		OCEAN_FORCE_INLINE VectorT3<T> operator*(const VectorT3<T>& vector) const;

		/**
		 * Multiply operator for a 4D vector.
		 * @param vector Right operand
		 * @return Resulting 4D vector
		 */
		OCEAN_FORCE_INLINE VectorT4<T> operator*(const VectorT4<T>& vector) const;

		/**
		 * Multiplies this matrix with a scalar value.
		 * @param value Right operand
		 * @return Resulting matrix
		 */
		SquareMatrixT4<T> operator*(const T value) const;

		/**
		 * Multiplies and assigns this matrix with a scalar value.
		 * @param value right operand
		 * @return Reference to this object
		 */
		SquareMatrixT4<T>& operator*=(const T value);

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 15]
		 * @return Specified element
		 */
		inline T operator[](const unsigned int index) const;

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 15]
		 * @return Specified element
		 */
		inline T& operator[](const unsigned int index);

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param row The row of the element to return [0, 3]
		 * @param column The column of the element to return [0, 3]
		 * @return Specified element
		 */
		inline T operator()(const unsigned int row, const unsigned int column) const;

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param row The row of the element to return [0, 3]
		 * @param column The column of the element to return [0, 3]
		 * @return Specified element
		 */
		inline T& operator()(const unsigned int row, const unsigned int column);

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 15]
		 * @return Specified element
		 */
		inline T operator()(const unsigned int index) const;

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 15]
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
		inline size_t operator()(const SquareMatrixT4<T>& matrix) const;

		/**
		 * Returns the number of elements this matrix has.
		 * @return The number of elements, always 16
		 */
		static inline size_t elements();

		/**
		 * Multiplies several 4D vectors with a given matrix.
		 * @param matrix The matrix to be used for multiplication, may be nullptr if number is 0
		 * @param vectors The input vectors that will be multiplied, may be nullptr if number is 0
		 * @param results The resulting output (multiplied/transformed) vectors, with same number as the provided input vectors
		 * @param number The number of provided vectors (input and output), with range [0, infinity)
		 */
		static void multiply(const SquareMatrixT4<T>& matrix, const VectorT4<T>* vectors, VectorT4<T>* results, const size_t number);

		/**
		 * Creates a projection matrix defined by the horizontal field of view, the aspect ratio and the near and far clipping plane.
		 * @param fovX Horizontal field of view, in radian and a range of [0, Pi]
		 * @param aspectRatio View aspect ratio which is width divided by the height of the projection window
		 * @param nearDistance Positive distance to the near clipping plane
		 * @param farDistance Positive distance to the far clipping lane
		 * @return Specified projection matrix
		 */
		static SquareMatrixT4<T> projectionMatrix(const T fovX, const T aspectRatio, const T nearDistance, const T farDistance);

		/**
		 * Creates a projection matrix defined by a camera profile of a pinhole camera and the near and far clipping plane.
		 * @param anyCamera The camera profile of a pinhole camera, without distortion parameters, must be valid
		 * @param nearDistance Positive distance to the near clipping plane
		 * @param farDistance Positive distance to the far clipping lane
		 * @return Specified projection matrix
		 */
		static SquareMatrixT4<T> projectionMatrix(const AnyCameraT<T>& anyCamera, const T nearDistance, const T farDistance);

		/**
		 * Creates a projection matrix defined by an asymmetric viewing frustum.
		 * The shape of the frustum is defined by the rectangle on the near plane.<br>
		 * Afterwards, the field of view is defined by the (positive) distance to the near clipping plane.<br>
		 * Followed by the (positive) far clipping plane to determine the entire frustum.
		 * @param left Position of the left border of the rectangle on the near plane
		 * @param right Position of the right border of the rectangle on the near plane
		 * @param top Position of the top border of the rectangle on the near plane
		 * @param bottom Position of the bottom border of the rectangle on the near plane
		 * @param nearDistance Positive distance to the near clipping plane
		 * @param farDistance Positive distance to the far clipping plane
		 * @return Specified frustum projection matrix
		 */
		static SquareMatrixT4<T> frustumMatrix(const T left, const T right, const T top, const T bottom, const T nearDistance, const T farDistance);

		/**
		 * Creates a project matrix defined by an asymmetric viewing frustum.
		 * The shape of the frustum is defined by the rectangle on the near plane.<br>
		 * The viewing position is defined by the given view matrix while the near plane is expected to lie in the origin of the coordinate system.
		 * @param width The width of the near plane, with range (0, infinity)
		 * @param height The height of the near plane, with range (0, infinity)
		 * @param viewingMatrix Viewing matrix transforming point defined in the camera coordinate system into points defined in the world coordinate system, must be invertible
		 * @param nearDistance Positive distance to the near clipping plane, with range (0, infinity)
		 * @param farDistance Positive distance to the far clipping plane, with range (nearDistance, infinity)
		 * @return Specified frustum projection matrix
		 */
		static SquareMatrixT4<T> frustumMatrix(const T width, const T height, const HomogenousMatrixT4<T>& viewingMatrix, const T nearDistance, const T farDistance);

		/**
		 * Converts matrices with specific data type to matrices with different data type.
		 * @param matrices The matrices to convert
		 * @return The converted matrices
		 * @tparam U The element data type of the matrices to convert
		 */
		template <typename U>
		static inline std::vector< SquareMatrixT4<T> > matrices2matrices(const std::vector< SquareMatrixT4<U> >& matrices);

		/**
		 * Converts matrices with specific data type to matrices with different data type.
		 * @param matrices The matrices to convert
		 * @param size The number of matrices to convert
		 * @return The converted matrices
		 * @tparam U The element data type of the matrices to convert
		 */
		template <typename U>
		static inline std::vector< SquareMatrixT4<T> > matrices2matrices(const SquareMatrixT4<U>* matrices, const size_t size);

	protected:

		/**
		 * Swaps two rows of this matrix.
		 * @param row0 The index of the first row ,with range [0, 3]
		 * @param row1 The index of the second row, with range [0, 3]
		 */
		void swapRows(const unsigned int row0, const unsigned int row1);

		/**
		 * Multiplies a row with a scalar value.
		 * @param row The index of the row to multiply, with range [0, 3]
		 * @param scalar The scalar to multiply
		 */
		void multiplyRow(const unsigned int row, const T scalar);

		/**
		 * Multiplies elements from a specific row with a scalar and adds them to another row.
		 * @param targetRow The index of the target row, with range [0, 3]
		 * @param sourceRow The index of the source row, with range [0, 3]
		 * @param scalar The scalar to multiply the source elements with
		 */
		void addRows(const unsigned int targetRow, const unsigned int sourceRow, const T scalar);

	protected:

		/// The sixteen values of the matrix.
		T values[16];
};

template <typename T>
inline SquareMatrixT4<T>::SquareMatrixT4()
{
	// nothing to do here
}

template <typename T>
template <typename U>
inline SquareMatrixT4<T>::SquareMatrixT4(const SquareMatrixT4<U>& matrix)
{
	values[ 0] = T(matrix.values[ 0]);
	values[ 1] = T(matrix.values[ 1]);
	values[ 2] = T(matrix.values[ 2]);
	values[ 3] = T(matrix.values[ 3]);
	values[ 4] = T(matrix.values[ 4]);
	values[ 5] = T(matrix.values[ 5]);
	values[ 6] = T(matrix.values[ 6]);
	values[ 7] = T(matrix.values[ 7]);
	values[ 8] = T(matrix.values[ 8]);
	values[ 9] = T(matrix.values[ 9]);
	values[10] = T(matrix.values[10]);
	values[11] = T(matrix.values[11]);
	values[12] = T(matrix.values[12]);
	values[13] = T(matrix.values[13]);
	values[14] = T(matrix.values[14]);
	values[15] = T(matrix.values[15]);
}

template <typename T>
SquareMatrixT4<T>::SquareMatrixT4(const bool setToIdentity)
{
	if (setToIdentity)
	{
		values[ 0] = T(1.0);
		values[ 1] = T(0.0);
		values[ 2] = T(0.0);
		values[ 3] = T(0.0);
		values[ 4] = T(0.0);
		values[ 5] = T(1.0);
		values[ 6] = T(0.0);
		values[ 7] = T(0.0);
		values[ 8] = T(0.0);
		values[ 9] = T(0.0);
		values[10] = T(1.0);
		values[11] = T(0.0);
		values[12] = T(0.0);
		values[13] = T(0.0);
		values[14] = T(0.0);
		values[15] = T(1.0);
	}
	else
	{
		values[ 0] = T(0.0);
		values[ 1] = T(0.0);
		values[ 2] = T(0.0);
		values[ 3] = T(0.0);
		values[ 4] = T(0.0);
		values[ 5] = T(0.0);
		values[ 6] = T(0.0);
		values[ 7] = T(0.0);
		values[ 8] = T(0.0);
		values[ 9] = T(0.0);
		values[10] = T(0.0);
		values[11] = T(0.0);
		values[12] = T(0.0);
		values[13] = T(0.0);
		values[14] = T(0.0);
		values[15] = T(0.0);
	}
}

template <typename T>
template <typename U>
SquareMatrixT4<T>::SquareMatrixT4(const U* arrayValues)
{
	ocean_assert(arrayValues);

	for (unsigned int n = 0u; n < 16u; ++n)
	{
		values[n] = T(arrayValues[n]);
	}
}

template <typename T>
SquareMatrixT4<T>::SquareMatrixT4(const T* arrayValues)
{
	ocean_assert(arrayValues);
	memcpy(values, arrayValues, sizeof(T) * 16);
}

template <typename T>
template <typename U>
SquareMatrixT4<T>::SquareMatrixT4(const U* arrayValues, const bool valuesRowAligned)
{
	ocean_assert(arrayValues);

	if (valuesRowAligned)
	{
		values[ 0] = T(arrayValues[ 0]);
		values[ 1] = T(arrayValues[ 4]);
		values[ 2] = T(arrayValues[ 8]);
		values[ 3] = T(arrayValues[12]);
		values[ 4] = T(arrayValues[ 1]);
		values[ 5] = T(arrayValues[ 5]);
		values[ 6] = T(arrayValues[ 9]);
		values[ 7] = T(arrayValues[13]);
		values[ 8] = T(arrayValues[ 2]);
		values[ 9] = T(arrayValues[ 6]);
		values[10] = T(arrayValues[10]);
		values[11] = T(arrayValues[14]);
		values[12] = T(arrayValues[ 3]);
		values[13] = T(arrayValues[ 7]);
		values[14] = T(arrayValues[11]);
		values[15] = T(arrayValues[15]);
	}
	else
	{
		for (unsigned int n = 0u; n < 16u; ++n)
			values[n] = T(arrayValues[n]);
	}
}

template <typename T>
SquareMatrixT4<T>::SquareMatrixT4(const T* arrayValues, const bool valuesRowAligned)
{
	ocean_assert(arrayValues);

	if (valuesRowAligned)
	{
		values[ 0] = arrayValues[ 0];
		values[ 1] = arrayValues[ 4];
		values[ 2] = arrayValues[ 8];
		values[ 3] = arrayValues[12];
		values[ 4] = arrayValues[ 1];
		values[ 5] = arrayValues[ 5];
		values[ 6] = arrayValues[ 9];
		values[ 7] = arrayValues[13];
		values[ 8] = arrayValues[ 2];
		values[ 9] = arrayValues[ 6];
		values[10] = arrayValues[10];
		values[11] = arrayValues[14];
		values[12] = arrayValues[ 3];
		values[13] = arrayValues[ 7];
		values[14] = arrayValues[11];
		values[15] = arrayValues[15];
	}
	else
	{
		memcpy(values, arrayValues, sizeof(T) * 16);
	}
}

template <typename T>
SquareMatrixT4<T>::SquareMatrixT4(const HomogenousMatrixT4<T>& transformation)
{
	memcpy(values, transformation(), sizeof(T) * 16);
}

template <typename T>
SquareMatrixT4<T>::SquareMatrixT4(const SquareMatrixT3<T>& subMatrix)
{
	values[ 3] = T(0.0);
	values[ 7] = T(0.0);
	values[11] = T(0.0);
	values[12] = T(0.0);
	values[13] = T(0.0);
	values[14] = T(0.0);
	values[15] = T(0.0);

	memcpy(values, subMatrix(), sizeof(T) * 3); // Set values[0] - values[2]
	memcpy(values + 4, subMatrix() + 3, sizeof(T) * 3); // Set values[4] - values[6]
	memcpy(values + 8, subMatrix() + 6, sizeof(T) * 3); // Set values[8] - values[10]
}

template <typename T>
SquareMatrixT4<T>::SquareMatrixT4(const VectorT4<T>& diagonal)
{
	values[ 0] = diagonal[0];
	values[ 1] = T(0.0);
	values[ 2] = T(0.0);
	values[ 3] = T(0.0);
	values[ 4] = T(0.0);
	values[ 5] = diagonal[1];
	values[ 6] = T(0.0);
	values[ 7] = T(0.0);
	values[ 8] = T(0.0);
	values[ 9] = T(0.0);
	values[10] = diagonal[2];
	values[11] = T(0.0);
	values[12] = T(0.0);
	values[13] = T(0.0);
	values[14] = T(0.0);
	values[15] = diagonal[3];
}

template <typename T>
inline const T* SquareMatrixT4<T>::data() const
{
	return values;
}

template <typename T>
inline T* SquareMatrixT4<T>::data()
{
	return values;
}

template <typename T>
SquareMatrixT4<T> SquareMatrixT4<T>::transposed() const
{
	SquareMatrixT4<T> result(*this);

	result.values[1] = values[4];
	result.values[4] = values[1];

	result.values[2] = values[8];
	result.values[8] = values[2];

	result.values[3] = values[12];
	result.values[12] = values[3];

	result.values[6] = values[9];
	result.values[9] = values[6];

	result.values[7] = values[13];
	result.values[13] = values[7];

	result.values[11] = values[14];
	result.values[14] = values[11];

	return result;
}

template <typename T>
void SquareMatrixT4<T>::transpose()
{
	SquareMatrixT4<T> tmp(*this);

	values[4] = tmp.values[1];
	values[1] = tmp.values[4];

	values[8] = tmp.values[2];
	values[2] = tmp.values[8];

	values[12] = tmp.values[3];
	values[3] = tmp.values[12];

	values[9] = tmp.values[6];
	values[6] = tmp.values[9];

	values[13] = tmp.values[7];
	values[7] = tmp.values[13];

	values[14] = tmp.values[11];
	values[11] = tmp.values[14];
}

template <typename T>
SquareMatrixT4<T> SquareMatrixT4<T>::inverted() const
{
	SquareMatrixT4 invertedMatrix;

	if (!invert(invertedMatrix))
	{
		ocean_assert(false && "Could not invert matrix.");
		return *this;
	}

	return invertedMatrix;
}

template <typename T>
bool SquareMatrixT4<T>::invert()
{
	SquareMatrixT4<T> invertedMatrix;

	if (!invert(invertedMatrix))
	{
		return false;
	}

	*this = invertedMatrix;

	return true;
}

template <typename T>
bool SquareMatrixT4<T>::invert(SquareMatrixT4<T>& invertedMatrix) const
{
	// implements the Gauss-Jordon Elimination

	SquareMatrixT4<T> source(*this);
	invertedMatrix.toIdentity();

	for (unsigned int col = 0; col < 4u; ++col)
	{
		// find largest absolute value in the col-th column,
		// to remove zeros from the main diagonal and to provide numerical stability
		T absolute = 0.0;
		unsigned int selectedRow = 0u;

		for (unsigned int row = col; row < 4u; ++row)
		{
			const T value = NumericT<T>::abs(source(row, col));
			if (absolute < value)
			{
				absolute = value;
				selectedRow = row;
			}
		}

		// if there was no greater absolute value than 0 this matrix is singular

		if (NumericT<T>::isEqualEps(absolute))
		{
			return false;
		}

		// exchange the two rows
		if (selectedRow != col)
		{
			source.swapRows(col, selectedRow);
			invertedMatrix.swapRows(col, selectedRow);
		}

		// now the element at (col, col) will be 1.0
		if (NumericT<T>::isNotEqual(source(col, col), T(1.0)))
		{
			const T divisor = T(1.0) / source(col, col);
			ocean_assert(divisor != T(0.0));

			source.multiplyRow(col, divisor);
			invertedMatrix.multiplyRow(col, divisor);
		}

		// clear each entry above and below the selected row and column to zero
		for (unsigned int row = 0; row < 4u; ++row)
		{
			if (row != col)
			{
				const T value = -source(row, col);

				source.addRows(row, col, value);
				invertedMatrix.addRows(row, col, value);
			}
		}
	}

	return true;
}

template <typename T>
T SquareMatrixT4<T>::determinant() const
{
	const T v6_15 = values[6] * values[15];
	const T v10_15 = values[10] * values[15];
	const T v11_14 = values[11] * values[14];
	const T v7_10 = values[7] * values[10];
	const T v9_14 = values[9] * values[14];
	const T v6_13 = values[6] * values[13];
	const T v2_13 = values[2] * values[13];
	const T v2_9 = values[2] * values[9];
	const T v3_10 = values[3] * values[10];
	const T v2_5 = values[2] * values[5];

	return values[0] * (values[5] * v10_15 - values[13] * v7_10
						+ v9_14 * values[7] - values[5] * v11_14
						+ v6_13 * values[11] - values[9] * v6_15)
		- values[4] * (v9_14 * values[3] - values[1] * v11_14
						+ v2_13 * values[11] - v2_9 * values[15]
						+ values[1] * v10_15 - values[13] * v3_10)
		+ values[8] * (values[1] * v6_15 - v6_13 * values[3]
						+ values[5] * values[14] * values[3] - values[1] * values[14] * values[7]
						+ v2_13 * values[7] - v2_5 * values[15])
		- values[12] * (values[1] * values[6] * values[11] - values[9] * values[6] * values[3]
						+ values[5] * v3_10 - values[1] * v7_10
						+ v2_9 * values[7] - v2_5 * values[11]);
}

template <typename T>
T SquareMatrixT4<T>::trace() const
{
	return values[0] + values[5] + values[10] + values[15];
}

template <typename T>
inline void SquareMatrixT4<T>::toIdentity()
{
	values[ 0] = T(1.0);
	values[ 1] = T(0.0);
	values[ 2] = T(0.0);
	values[ 3] = T(0.0);
	values[ 4] = T(0.0);
	values[ 5] = T(1.0);
	values[ 6] = T(0.0);
	values[ 7] = T(0.0);
	values[ 8] = T(0.0);
	values[ 9] = T(0.0);
	values[10] = T(1.0);
	values[11] = T(0.0);
	values[12] = T(0.0);
	values[13] = T(0.0);
	values[14] = T(0.0);
	values[15] = T(1.0);
}

template <typename T>
inline void SquareMatrixT4<T>::toNull()
{
	values[ 0] = T(0.0);
	values[ 1] = T(0.0);
	values[ 2] = T(0.0);
	values[ 3] = T(0.0);
	values[ 4] = T(0.0);
	values[ 5] = T(0.0);
	values[ 6] = T(0.0);
	values[ 7] = T(0.0);
	values[ 8] = T(0.0);
	values[ 9] = T(0.0);
	values[10] = T(0.0);
	values[11] = T(0.0);
	values[12] = T(0.0);
	values[13] = T(0.0);
	values[14] = T(0.0);
	values[15] = T(0.0);
}

template <typename T>
bool SquareMatrixT4<T>::isIdentity() const
{
	return NumericT<T>::isEqual(values[0], 1) && NumericT<T>::isEqualEps(values[1]) && NumericT<T>::isEqualEps(values[2]) && NumericT<T>::isEqualEps(values[3])
				&& NumericT<T>::isEqualEps(values[4]) && NumericT<T>::isEqual(values[5], 1) && NumericT<T>::isEqualEps(values[6]) && NumericT<T>::isEqualEps(values[7])
				&& NumericT<T>::isEqualEps(values[8]) && NumericT<T>::isEqualEps(values[9]) && NumericT<T>::isEqual(values[10], 1) && NumericT<T>::isEqualEps(values[11])
				&& NumericT<T>::isEqualEps(values[12]) && NumericT<T>::isEqualEps(values[13]) && NumericT<T>::isEqualEps(values[14]) && NumericT<T>::isEqual(values[15], 1);
}

template <typename T>
bool SquareMatrixT4<T>::isNull() const
{
	return NumericT<T>::isEqualEps(values[0]) && NumericT<T>::isEqualEps(values[1]) && NumericT<T>::isEqualEps(values[2]) && NumericT<T>::isEqualEps(values[3])
				&& NumericT<T>::isEqualEps(values[4]) && NumericT<T>::isEqualEps(values[5]) && NumericT<T>::isEqualEps(values[6]) && NumericT<T>::isEqualEps(values[7])
				&& NumericT<T>::isEqualEps(values[8]) && NumericT<T>::isEqualEps(values[9]) && NumericT<T>::isEqualEps(values[10]) && NumericT<T>::isEqualEps(values[11])
				&& NumericT<T>::isEqualEps(values[12]) && NumericT<T>::isEqualEps(values[13]) && NumericT<T>::isEqualEps(values[14]) && NumericT<T>::isEqualEps(values[15]);
}

template <typename T>
inline bool SquareMatrixT4<T>::isSingular() const
{
	return NumericT<T>::isEqualEps(determinant());
}

template <typename T>
bool SquareMatrixT4<T>::isSymmetric(const T epsilon) const
{
	ocean_assert(epsilon >= T(0));

	return NumericT<T>::isEqual(values[1], values[4], epsilon) && NumericT<T>::isEqual(values[2], values[8], epsilon) && NumericT<T>::isEqual(values[3], values[12], epsilon)
			&& NumericT<T>::isEqual(values[6], values[9], epsilon) && NumericT<T>::isEqual(values[7], values[13], epsilon) && NumericT<T>::isEqual(values[11], values[14], epsilon);
}

template <typename T>
inline bool SquareMatrixT4<T>::isEqual(const SquareMatrixT4<T>& matrix, const T eps) const
{
	return NumericT<T>::isEqual(values[0], matrix.values[0], eps) && NumericT<T>::isEqual(values[1], matrix.values[1], eps)
			&& NumericT<T>::isEqual(values[2], matrix.values[2], eps) && NumericT<T>::isEqual(values[3], matrix.values[3], eps)
			&& NumericT<T>::isEqual(values[4], matrix.values[4], eps) && NumericT<T>::isEqual(values[5], matrix.values[5], eps)
			&& NumericT<T>::isEqual(values[6], matrix.values[6], eps) && NumericT<T>::isEqual(values[7], matrix.values[7], eps)
			&& NumericT<T>::isEqual(values[8], matrix.values[8], eps) && NumericT<T>::isEqual(values[9], matrix.values[9], eps)
			&& NumericT<T>::isEqual(values[10], matrix.values[10], eps) && NumericT<T>::isEqual(values[11], matrix.values[11], eps)
			&& NumericT<T>::isEqual(values[12], matrix.values[12], eps) && NumericT<T>::isEqual(values[13], matrix.values[13], eps)
			&& NumericT<T>::isEqual(values[14], matrix.values[14], eps) && NumericT<T>::isEqual(values[15], matrix.values[15], eps);
}

template <typename T>
bool SquareMatrixT4<T>::eigenSystem(VectorT4<T>& eigenValues, SquareMatrixT4<T>& eigenVectors)
{
	/**
	 * <pre>
	 * Computation of the characteristic polynomial
	 *
	 *     [ a b c d ]
	 * A = [ e f g h ]
	 *     [ i j k l ]
	 *     [ m n o p ]
	 *
	 *             [ a-x   b    c    d  ]
	 * A - x * E = [  e   f-x   g    h  ]
	 *             [  i    j   k-x   l  ]
	 *             [  m    n    o   p-x ]
	 *
	 * Polynomial = Det|A - x * E| = 0
	 *            = x^4 + (-a - f - k - p)x^3 + (-be + af - ci - gj + ak + fk - dm - hn - lo + ap + fp + kp)x^2
	 *               + (cfi - bgi - cej + agj + bek - afk + dfm - bhm + dkm - clm - den + ahn + hkn - gln - dio - hjo + alo + flo + bep - afp + cip + gjp - akp - fkp)x
	 *               + (dgjm - chjm - dfkm + bhkm + cflm - bglm)
	 *               + (-dgin + chin + dekn - ahkn - celn + agln)
	 *               + (dfio - bhio - dejo + ahjo + belo - aflo)
	 *               + (-cfip + bgip + cejp - agjp - bekp + afkp)
	 *            = a1x^4 + a2x^3 + a3x^2 + a4x + a5 = 0
	 * </pre>
	 */

	const T a = values[0];
	const T b = values[1];
	const T c = values[2];
	const T d = values[3];
	const T e = values[4];
	const T f = values[5];
	const T g = values[6];
	const T h = values[7];
	const T i = values[8];
	const T j = values[9];
	const T k = values[10];
	const T l = values[11];
	const T m = values[12];
	const T n = values[13];
	const T o = values[14];
	const T p = values[15];

	const T a1 = 1.0;
	const T a2 = -a - f - k - p;
	const T a3 = -b * e + a * f - c * i - g * j + a * k + f * k - d * m - h * n - l * o + a * p + f * p + k * p;
	const T a4 = c * f * i - b * g * i - c * e * j + a * g * j + b * e * k - a * f * k + d * f * m - b * h * m
						+ d * k * m - c * l * m - d * e * n + a * h * n + h * k * n - g * l * n - d * i * o - h * j * o
						+ a * l * o + f * l * o + b * e * p - a * f * p + c * i * p + g * j * p - a * k * p - f * k * p;
	const T a5 = d * g * j * m - c * h * j * m - d * f * k * m + b * h * k * m + c * f * l * m - b * g * l * m
						- d * g * i * n + c * h * i * n + d * e * k * n - a * h * k * n - c * e * l * n + a * g * l * n
						+ d * f * i * o - b * h * i * o - d * e * j * o + a * h * j * o + b * e * l * o - a * f * l * o
						- c * f * i * p + b * g * i * p + c * e * j * p - a * g * j * p - b * e * k * p + a * f * k * p;

#if defined(SQUARE_MATRIX_DISABLED_MISSING_IMPLEMENTATION)
	T x[4];
	unsigned int solutions = EquationT<T>::solveQuartic(a1, a2, a3, a4, a5, x);
#endif // SQUARE_MATRIX_DISABLED_MISSING_IMPLEMENTATION

	ocean_assert(false && "Missing implementation, calculate eigen vectors");
	OCEAN_SUPPRESS_UNUSED_WARNING(eigenValues);
	OCEAN_SUPPRESS_UNUSED_WARNING(eigenVectors);

	return false;
}

template <typename T>
template <typename U>
void SquareMatrixT4<T>::copyElements(U* arrayValues) const
{
	ocean_assert(arrayValues);

	arrayValues[ 0] = U(values[ 0]);
	arrayValues[ 1] = U(values[ 1]);
	arrayValues[ 2] = U(values[ 2]);
	arrayValues[ 3] = U(values[ 3]);
	arrayValues[ 4] = U(values[ 4]);
	arrayValues[ 5] = U(values[ 5]);
	arrayValues[ 6] = U(values[ 6]);
	arrayValues[ 7] = U(values[ 7]);
	arrayValues[ 8] = U(values[ 8]);
	arrayValues[ 9] = U(values[ 9]);
	arrayValues[10] = U(values[10]);
	arrayValues[11] = U(values[11]);
	arrayValues[12] = U(values[12]);
	arrayValues[13] = U(values[13]);
	arrayValues[14] = U(values[14]);
	arrayValues[15] = U(values[15]);
}

template <typename T>
void SquareMatrixT4<T>::copyElements(T* arrayValues) const
{
	ocean_assert(arrayValues);

	memcpy(arrayValues, values, sizeof(T) * 16);
}

template <typename T>
inline bool SquareMatrixT4<T>::operator!=(const SquareMatrixT4<T>& matrix) const
{
	return !(*this == matrix);
}

template <typename T>
inline SquareMatrixT4<T>& SquareMatrixT4<T>::operator*=(const SquareMatrixT4<T>& matrix)
{
	*this = *this * matrix;
	return *this;
}

template <typename T>
inline SquareMatrixT4<T>& SquareMatrixT4<T>::operator*=(const HomogenousMatrixT4<T>& matrix)
{
	*this = *this * matrix;
	return *this;
}

template <typename T>
inline T SquareMatrixT4<T>::operator[](const unsigned int index) const
{
	ocean_assert(index < 16u);
	return values[index];
}

template <typename T>
inline T& SquareMatrixT4<T>::operator[](const unsigned int index)
{
	ocean_assert(index < 16u);
	return values[index];
}

template <typename T>
inline T SquareMatrixT4<T>::operator()(const unsigned int row, const unsigned int column) const
{
	ocean_assert(row < 4u && column < 4u);
	return values[(column << 2) + row]; // values[(column * 4) + row];
}

template <typename T>
inline T& SquareMatrixT4<T>::operator()(const unsigned int row, const unsigned int column)
{
	ocean_assert(row < 4u && column < 4u);
	return values[(column << 2) + row]; // values[(column * 4) + row];
}

template <typename T>
inline T SquareMatrixT4<T>::operator()(const unsigned int index) const
{
	ocean_assert(index < 16u);
	return values[index];
}

template <typename T>
inline T& SquareMatrixT4<T>::operator()(const unsigned int index)
{
	ocean_assert(index < 16u);
	return values[index];
}

template <typename T>
inline const T* SquareMatrixT4<T>::operator()() const
{
	return values;
}

template <typename T>
inline T* SquareMatrixT4<T>::operator()()
{
	return values;
}

template <typename T>
inline size_t SquareMatrixT4<T>::operator()(const SquareMatrixT4<T>& matrix) const
{
	size_t seed = std::hash<T>{}(matrix.values[0]);

	for (unsigned int n = 1u; n < 16u; ++n)
	{
		seed ^= std::hash<T>{}(matrix.values[n]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	return seed;
}

template <typename T>
inline size_t SquareMatrixT4<T>::elements()
{
	return 16;
}

template <typename T>
bool SquareMatrixT4<T>::operator==(const SquareMatrixT4<T>& matrix) const
{
	return isEqual(matrix);
}

template <typename T>
SquareMatrixT4<T> SquareMatrixT4<T>::operator+(const SquareMatrixT4<T>& matrix) const
{
	SquareMatrixT4<T> result(*this);

	result.values[0] += matrix.values[0];
	result.values[1] += matrix.values[1];
	result.values[2] += matrix.values[2];
	result.values[3] += matrix.values[3];
	result.values[4] += matrix.values[4];
	result.values[5] += matrix.values[5];
	result.values[6] += matrix.values[6];
	result.values[7] += matrix.values[7];
	result.values[8] += matrix.values[8];
	result.values[9] += matrix.values[9];
	result.values[10] += matrix.values[10];
	result.values[11] += matrix.values[11];
	result.values[12] += matrix.values[12];
	result.values[13] += matrix.values[13];
	result.values[14] += matrix.values[14];
	result.values[15] += matrix.values[15];

	return result;
}

template <typename T>
SquareMatrixT4<T>& SquareMatrixT4<T>::operator+=(const SquareMatrixT4<T>& matrix)
{
	values[0] += matrix.values[0];
	values[1] += matrix.values[1];
	values[2] += matrix.values[2];
	values[3] += matrix.values[3];
	values[4] += matrix.values[4];
	values[5] += matrix.values[5];
	values[6] += matrix.values[6];
	values[7] += matrix.values[7];
	values[8] += matrix.values[8];
	values[9] += matrix.values[9];
	values[10] += matrix.values[10];
	values[11] += matrix.values[11];
	values[12] += matrix.values[12];
	values[13] += matrix.values[13];
	values[14] += matrix.values[14];
	values[15] += matrix.values[15];

	return *this;
}

template <typename T>
SquareMatrixT4<T> SquareMatrixT4<T>::operator-(const SquareMatrixT4<T>& matrix) const
{
	SquareMatrixT4<T> result(*this);

	result.values[0] -= matrix.values[0];
	result.values[1] -= matrix.values[1];
	result.values[2] -= matrix.values[2];
	result.values[3] -= matrix.values[3];
	result.values[4] -= matrix.values[4];
	result.values[5] -= matrix.values[5];
	result.values[6] -= matrix.values[6];
	result.values[7] -= matrix.values[7];
	result.values[8] -= matrix.values[8];
	result.values[9] -= matrix.values[9];
	result.values[10] -= matrix.values[10];
	result.values[11] -= matrix.values[11];
	result.values[12] -= matrix.values[12];
	result.values[13] -= matrix.values[13];
	result.values[14] -= matrix.values[14];
	result.values[15] -= matrix.values[15];

	return result;
}

template <typename T>
SquareMatrixT4<T>& SquareMatrixT4<T>::operator-=(const SquareMatrixT4<T>& matrix)
{
	values[0] -= matrix.values[0];
	values[1] -= matrix.values[1];
	values[2] -= matrix.values[2];
	values[3] -= matrix.values[3];
	values[4] -= matrix.values[4];
	values[5] -= matrix.values[5];
	values[6] -= matrix.values[6];
	values[7] -= matrix.values[7];
	values[8] -= matrix.values[8];
	values[9] -= matrix.values[9];
	values[10] -= matrix.values[10];
	values[11] -= matrix.values[11];
	values[12] -= matrix.values[12];
	values[13] -= matrix.values[13];
	values[14] -= matrix.values[14];
	values[15] -= matrix.values[15];

	return *this;
}

template <typename T>
inline SquareMatrixT4<T> SquareMatrixT4<T>::operator-() const
{
	SquareMatrixT4<T> result;

	result.values[ 0] = -values[ 0];
	result.values[ 1] = -values[ 1];
	result.values[ 2] = -values[ 2];
	result.values[ 3] = -values[ 3];
	result.values[ 4] = -values[ 4];
	result.values[ 5] = -values[ 5];
	result.values[ 6] = -values[ 6];
	result.values[ 7] = -values[ 7];
	result.values[ 8] = -values[ 8];
	result.values[ 9] = -values[ 9];
	result.values[10] = -values[10];
	result.values[11] = -values[11];
	result.values[12] = -values[12];
	result.values[13] = -values[13];
	result.values[14] = -values[14];

	return result;
}

template <typename T>
OCEAN_FORCE_INLINE SquareMatrixT4<T> SquareMatrixT4<T>::operator*(const SquareMatrixT4<T>& matrix) const
{
	SquareMatrixT4<T> result;

	result.values[0] = values[0] * matrix.values[0] + values[4] * matrix.values[1] + values[8] * matrix.values[2] + values[12] * matrix.values[3];
	result.values[1] = values[1] * matrix.values[0] + values[5] * matrix.values[1] + values[9] * matrix.values[2] + values[13] * matrix.values[3];
	result.values[2] = values[2] * matrix.values[0] + values[6] * matrix.values[1] + values[10] * matrix.values[2] + values[14] * matrix.values[3];
	result.values[3] = values[3] * matrix.values[0] + values[7] * matrix.values[1] + values[11] * matrix.values[2] + values[15] * matrix.values[3];

	result.values[4] = values[0] * matrix.values[4] + values[4] * matrix.values[5] + values[8] * matrix.values[6] + values[12] * matrix.values[7];
	result.values[5] = values[1] * matrix.values[4] + values[5] * matrix.values[5] + values[9] * matrix.values[6] + values[13] * matrix.values[7];
	result.values[6] = values[2] * matrix.values[4] + values[6] * matrix.values[5] + values[10] * matrix.values[6] + values[14] * matrix.values[7];
	result.values[7] = values[3] * matrix.values[4] + values[7] * matrix.values[5] + values[11] * matrix.values[6] + values[15] * matrix.values[7];

	result.values[8] = values[0] * matrix.values[8] + values[4] * matrix.values[9] + values[8] * matrix.values[10] + values[12] * matrix.values[11];
	result.values[9] = values[1] * matrix.values[8] + values[5] * matrix.values[9] + values[9] * matrix.values[10] + values[13] * matrix.values[11];
	result.values[10] = values[2] * matrix.values[8] + values[6] * matrix.values[9] + values[10] * matrix.values[10] + values[14] * matrix.values[11];
	result.values[11] = values[3] * matrix.values[8] + values[7] * matrix.values[9] + values[11] * matrix.values[10] + values[15] * matrix.values[11];

	result.values[12] = values[0] * matrix.values[12] + values[4] * matrix.values[13] + values[8] * matrix.values[14] + values[12] * matrix.values[15];
	result.values[13] = values[1] * matrix.values[12] + values[5] * matrix.values[13] + values[9] * matrix.values[14] + values[13] * matrix.values[15];
	result.values[14] = values[2] * matrix.values[12] + values[6] * matrix.values[13] + values[10] * matrix.values[14] + values[14] * matrix.values[15];
	result.values[15] = values[3] * matrix.values[12] + values[7] * matrix.values[13] + values[11] * matrix.values[14] + values[15] * matrix.values[15];

	return result;
}

#if defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 10

template <>
OCEAN_FORCE_INLINE SquareMatrixT4<double> SquareMatrixT4<double>::operator*(const SquareMatrixT4<double>& matrix) const
{
	// the following code uses the following AVX instructions, and needs AVX1 or higher

	// AVX1:
	// _mm256_broadcast_sd
	// _mm256_loadu_pd
	// _mm256_mul_pd
	// _mm256_add_pd
	// _mm256_storeu_pd

	// we use the same strategy as we apply for matrix-vector multiplication
	// further, here we interpret the right matrix as 4 vectors

	// we load the columns of the left matrix
	__m256d c0 = _mm256_loadu_pd(values +  0);
	__m256d c1 = _mm256_loadu_pd(values +  4);
	__m256d c2 = _mm256_loadu_pd(values +  8);
	__m256d c3 = _mm256_loadu_pd(values + 12);


	// we determine the first vector of the resulting matrix
	__m256d v0 = _mm256_broadcast_sd(matrix.data() + 0);
	__m256d r0 = _mm256_mul_pd(c0, v0);

	__m256d v1 = _mm256_broadcast_sd(matrix.data() + 1);
	__m256d r1 = _mm256_mul_pd(c1, v1);

	r0 = _mm256_add_pd(r0, r1);

	__m256d v2 = _mm256_broadcast_sd(matrix.data() + 2);
	__m256d r2 = _mm256_mul_pd(c2, v2);

	r0 = _mm256_add_pd(r0, r2);

	__m256d v3 = _mm256_broadcast_sd(matrix.data() + 3);
	__m256d r3 = _mm256_mul_pd(c3, v3);

	r0 = _mm256_add_pd(r0, r3);

	SquareMatrixT4<double> result;

	_mm256_storeu_pd(result.data(), r0);


	// we determine the second vector of the resulting matrix
	__m256d v4 = _mm256_broadcast_sd(matrix.data() + 4);
	__m256d r4 = _mm256_mul_pd(c0, v4);

	__m256d v5 = _mm256_broadcast_sd(matrix.data() + 5);
	__m256d r5 = _mm256_mul_pd(c1, v5);

	r4 = _mm256_add_pd(r4, r5);

	__m256d v6 = _mm256_broadcast_sd(matrix.data() + 6);
	__m256d r6 = _mm256_mul_pd(c2, v6);

	r4 = _mm256_add_pd(r4, r6);

	__m256d v7 = _mm256_broadcast_sd(matrix.data() + 7);
	__m256d r7 = _mm256_mul_pd(c3, v7);

	r4 = _mm256_add_pd(r4, r7);

	_mm256_storeu_pd(result.data() + 4, r4);


	// we determine the third vector of the resulting matrix
	__m256d v8 = _mm256_broadcast_sd(matrix.data() + 8);
	__m256d r8 = _mm256_mul_pd(c0, v8);

	__m256d v9 = _mm256_broadcast_sd(matrix.data() + 9);
	__m256d r9 = _mm256_mul_pd(c1, v9);

	r8 = _mm256_add_pd(r8, r9);

	__m256d v10 = _mm256_broadcast_sd(matrix.data() + 10);
	__m256d r10 = _mm256_mul_pd(c2, v10);

	r8 = _mm256_add_pd(r8, r10);

	__m256d v11 = _mm256_broadcast_sd(matrix.data() + 11);
	__m256d r11 = _mm256_mul_pd(c3, v11);

	r8 = _mm256_add_pd(r8, r11);

	_mm256_storeu_pd(result.data() + 8, r8);


	// we determine the forth vector of the resulting matrix
	__m256d v12 = _mm256_broadcast_sd(matrix.data() + 12);
	__m256d r12 = _mm256_mul_pd(c0, v12);

	__m256d v13 = _mm256_broadcast_sd(matrix.data() + 13);
	__m256d r13 = _mm256_mul_pd(c1, v13);

	r12 = _mm256_add_pd(r12, r13);

	__m256d v14 = _mm256_broadcast_sd(matrix.data() + 14);
	__m256d r14 = _mm256_mul_pd(c2, v14);

	r12 = _mm256_add_pd(r12, r14);

	__m256d v15 = _mm256_broadcast_sd(matrix.data() + 15);
	__m256d r15 = _mm256_mul_pd(c3, v15);

	r12 = _mm256_add_pd(r12, r15);

	_mm256_storeu_pd(result.data() + 12, r12);

	return result;
}

template <>
OCEAN_FORCE_INLINE SquareMatrixT4<float> SquareMatrixT4<float>::operator*(const SquareMatrixT4<float>& matrix) const
{
	// the following code uses the following AVX instructions, and needs AVX1 or higher

	// we use the same strategy as we apply for matrix-vector multiplication
	// further, here we interpret the right matrix as 4 vectors, and combine two vectors into one 256 bit register

	// we load the four columns of the left matrix
	__m256 c0 = _mm256_broadcast_ps((const __m128*)values + 0);
	__m256 c1 = _mm256_broadcast_ps((const __m128*)values + 1);
	__m256 c2 = _mm256_broadcast_ps((const __m128*)values + 2);
	__m256 c3 = _mm256_broadcast_ps((const __m128*)values + 3);

	__m256 m01 = _mm256_loadu_ps(matrix.data() + 0);
	__m256 m23 = _mm256_loadu_ps(matrix.data() + 8);


	// we determine the first two vectors of the resulting matrix
	__m256 v0_4 = _mm256_permute_ps(m01, 0x00);
	__m256 r0 = _mm256_mul_ps(c0, v0_4);

#ifdef OCEAN_COMPILER_MSC
	ocean_assert(NumericF::isEqual(r0.m256_f32[0], values[0] * matrix[0]));
	ocean_assert(NumericF::isEqual(r0.m256_f32[1], values[1] * matrix[0]));
	ocean_assert(NumericF::isEqual(r0.m256_f32[2], values[2] * matrix[0]));
	ocean_assert(NumericF::isEqual(r0.m256_f32[3], values[3] * matrix[0]));

	ocean_assert(NumericF::isEqual(r0.m256_f32[4], values[0] * matrix[4]));
	ocean_assert(NumericF::isEqual(r0.m256_f32[5], values[1] * matrix[4]));
	ocean_assert(NumericF::isEqual(r0.m256_f32[6], values[2] * matrix[4]));
	ocean_assert(NumericF::isEqual(r0.m256_f32[7], values[3] * matrix[4]));
#endif

	__m256 v1_5 = _mm256_permute_ps(m01, 0x55);
	__m256 r1 = _mm256_mul_ps(c1, v1_5);

#ifdef OCEAN_COMPILER_MSC
	ocean_assert(NumericF::isEqual(r1.m256_f32[0], values[ 4] * matrix[1]));
	ocean_assert(NumericF::isEqual(r1.m256_f32[1], values[ 5] * matrix[1]));
	ocean_assert(NumericF::isEqual(r1.m256_f32[2], values[ 6] * matrix[1]));
	ocean_assert(NumericF::isEqual(r1.m256_f32[3], values[ 7] * matrix[1]));

	ocean_assert(NumericF::isEqual(r1.m256_f32[4], values[4] * matrix[5]));
	ocean_assert(NumericF::isEqual(r1.m256_f32[5], values[5] * matrix[5]));
	ocean_assert(NumericF::isEqual(r1.m256_f32[6], values[6] * matrix[5]));
	ocean_assert(NumericF::isEqual(r1.m256_f32[7], values[7] * matrix[5]));
#endif

	r0 = _mm256_add_ps(r0, r1);


	__m256 v2_6 = _mm256_permute_ps(m01, 0xAA);
	__m256 r2 = _mm256_mul_ps(c2, v2_6);

	r0 = _mm256_add_ps(r0, r2);


	__m256 v3_7 = _mm256_permute_ps(m01, 0xFF);
	__m256 r3 = _mm256_mul_ps(c3, v3_7);

	r0 = _mm256_add_ps(r0, r3);

	SquareMatrixT4<float> result;

	_mm256_storeu_ps(result.data(), r0);



	// we determine the last two vectors of the resulting matrix
	__m256 v8_12 = _mm256_permute_ps(m23, 0x00);
	__m256 r8 = _mm256_mul_ps(c0, v8_12);


	__m256 v9_13 = _mm256_permute_ps(m23, 0x55);
	__m256 r9 = _mm256_mul_ps(c1, v9_13);

	r8 = _mm256_add_ps(r8, r9);


	__m256 v10_14 = _mm256_permute_ps(m23, 0xAA);
	__m256 r10 = _mm256_mul_ps(c2, v10_14);

	r8 = _mm256_add_ps(r8, r10);


	__m256 v11_15 = _mm256_permute_ps(m23, 0xFF);
	__m256 r11 = _mm256_mul_ps(c3, v11_15);

	r8 = _mm256_add_ps(r8, r11);

	_mm256_storeu_ps(result.data() + 8, r8);

	return result;
}

#else // OCEAN_HARDWARE_AVX_VERSION < 10

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 10

template <>
OCEAN_FORCE_INLINE SquareMatrixT4<float> SquareMatrixT4<float>::operator*(const SquareMatrixT4<float>& matrix) const
{
	// the following code uses the following SSE instructions, and needs SSE1 or higher

	// SSE1:
	// _mm_load1_ps
	// _mm_loadu_ps
	// _mm_mul_ps
	// _mm_add_ps
	// _mm_storeu_ps

	// we use the same strategy as we apply for matrix-vector multiplication
	// further, here we interpret the right matrix as 4 vectors

	// we load the columns of the left matrix
	__m128 c0 = _mm_loadu_ps(values +  0);
	__m128 c1 = _mm_loadu_ps(values +  4);
	__m128 c2 = _mm_loadu_ps(values +  8);
	__m128 c3 = _mm_loadu_ps(values + 12);


	// we determine the first vector of the resulting matrix
	__m128 v0 = _mm_load1_ps(matrix.data() + 0);
	__m128 r0 = _mm_mul_ps(c0, v0);

	__m128 v1 = _mm_load1_ps(matrix.data() + 1);
	__m128 r1 = _mm_mul_ps(c1, v1);

	r0 = _mm_add_ps(r0, r1);

	__m128 v2 = _mm_load1_ps(matrix.data() + 2);
	__m128 r2 = _mm_mul_ps(c2, v2);

	r0 = _mm_add_ps(r0, r2);

	__m128 v3 = _mm_load1_ps(matrix.data() + 3);
	__m128 r3 = _mm_mul_ps(c3, v3);

	r0 = _mm_add_ps(r0, r3);

	SquareMatrixT4<float> result;

	_mm_storeu_ps(result.data(), r0);


	// we determine the second vector of the resulting matrix
	__m128 v4 = _mm_load1_ps(matrix.data() + 4);
	__m128 r4 = _mm_mul_ps(c0, v4);

	__m128 v5 = _mm_load1_ps(matrix.data() + 5);
	__m128 r5 = _mm_mul_ps(c1, v5);

	r4 = _mm_add_ps(r4, r5);

	__m128 v6 = _mm_load1_ps(matrix.data() + 6);
	__m128 r6 = _mm_mul_ps(c2, v6);

	r4 = _mm_add_ps(r4, r6);

	__m128 v7 = _mm_load1_ps(matrix.data() + 7);
	__m128 r7 = _mm_mul_ps(c3, v7);

	r4 = _mm_add_ps(r4, r7);

	_mm_storeu_ps(result.data() + 4, r4);


	// we determine the third vector of the resulting matrix
	__m128 v8 = _mm_load1_ps(matrix.data() + 8);
	__m128 r8 = _mm_mul_ps(c0, v8);

	__m128 v9 = _mm_load1_ps(matrix.data() + 9);
	__m128 r9 = _mm_mul_ps(c1, v9);

	r8 = _mm_add_ps(r8, r9);

	__m128 v10 = _mm_load1_ps(matrix.data() + 10);
	__m128 r10 = _mm_mul_ps(c2, v10);

	r8 = _mm_add_ps(r8, r10);

	__m128 v11 = _mm_load1_ps(matrix.data() + 11);
	__m128 r11 = _mm_mul_ps(c3, v11);

	r8 = _mm_add_ps(r8, r11);

	_mm_storeu_ps(result.data() + 8, r8);


	// we determine the forth vector of the resulting matrix
	__m128 v12 = _mm_load1_ps(matrix.data() + 12);
	__m128 r12 = _mm_mul_ps(c0, v12);

	__m128 v13 = _mm_load1_ps(matrix.data() + 13);
	__m128 r13 = _mm_mul_ps(c1, v13);

	r12 = _mm_add_ps(r12, r13);

	__m128 v14 = _mm_load1_ps(matrix.data() + 14);
	__m128 r14 = _mm_mul_ps(c2, v14);

	r12 = _mm_add_ps(r12, r14);

	__m128 v15 = _mm_load1_ps(matrix.data() + 15);
	__m128 r15 = _mm_mul_ps(c3, v15);

	r12 = _mm_add_ps(r12, r15);

	_mm_storeu_ps(result.data() + 12, r12);

	return result;
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 10

#endif // OCEAN_HARDWARE_AVX_VERSION >= 10

template <typename T>
OCEAN_FORCE_INLINE SquareMatrixT4<T> SquareMatrixT4<T>::operator*(const HomogenousMatrixT4<T>& matrix) const
{
	SquareMatrixT4<T> result;

	result.values[0] = values[0] * matrix[0] + values[4] * matrix[1] + values[8] * matrix[2]; // + values[12] * matrix[3];
	result.values[1] = values[1] * matrix[0] + values[5] * matrix[1] + values[9] * matrix[2]; // + values[13] * matrix[3];
	result.values[2] = values[2] * matrix[0] + values[6] * matrix[1] + values[10] * matrix[2]; // + values[14] * matrix[3];
	result.values[3] = values[3] * matrix[0] + values[7] * matrix[1] + values[11] * matrix[2]; // + values[15] * matrix[3];

	result.values[4] = values[0] * matrix[4] + values[4] * matrix[5] + values[8] * matrix[6]; // + values[12] * matrix[7];
	result.values[5] = values[1] * matrix[4] + values[5] * matrix[5] + values[9] * matrix[6]; // + values[13] * matrix[7];
	result.values[6] = values[2] * matrix[4] + values[6] * matrix[5] + values[10] * matrix[6]; // + values[14] * matrix[7];
	result.values[7] = values[3] * matrix[4] + values[7] * matrix[5] + values[11] * matrix[6]; // + values[15] * matrix[7];

	result.values[8] = values[0] * matrix[8] + values[4] * matrix[9] + values[8] * matrix[10]; // + values[12] * matrix[11];
	result.values[9] = values[1] * matrix[8] + values[5] * matrix[9] + values[9] * matrix[10]; // + values[13] * matrix[11];
	result.values[10] = values[2] * matrix[8] + values[6] * matrix[9] + values[10] * matrix[10]; // + values[14] * matrix[11];
	result.values[11] = values[3] * matrix[8] + values[7] * matrix[9] + values[11] * matrix[10]; // + values[15] * matrix[11];

	result.values[12] = values[0] * matrix[12] + values[4] * matrix[13] + values[8] * matrix[14] + values[12]; // * matrix[15];
	result.values[13] = values[1] * matrix[12] + values[5] * matrix[13] + values[9] * matrix[14] + values[13]; // * matrix[15];
	result.values[14] = values[2] * matrix[12] + values[6] * matrix[13] + values[10] * matrix[14] + values[14]; // * matrix[15];
	result.values[15] = values[3] * matrix[12] + values[7] * matrix[13] + values[11] * matrix[14] + values[15]; // * matrix[15];

	return result;
}

template <typename T>
OCEAN_FORCE_INLINE VectorT3<T> SquareMatrixT4<T>::operator*(const VectorT3<T>& vector) const
{
	const T w = values[3] * vector[0] + values[7] * vector[1] + values[11] * vector[2] + values[15];
	ocean_assert(NumericT<T>::isNotEqualEps(w) && "Division by zero!");

	const T factor = 1 / w;

	return VectorT3<T>((values[0] * vector[0] + values[4] * vector[1] + values[8] * vector[2] + values[12]) * factor,
					(values[1] * vector[0] + values[5] * vector[1] + values[9] * vector[2] + values[13]) * factor,
					(values[2] * vector[0] + values[6] * vector[1] + values[10] * vector[2] + values[14]) * factor);
}

template <typename T>
OCEAN_FORCE_INLINE VectorT4<T> SquareMatrixT4<T>::operator*(const VectorT4<T>& vector) const
{
	return VectorT4<T>(values[0] * vector[0] + values[4] * vector[1] + values[8] * vector[2] + values[12] * vector[3],
						values[1] * vector[0] + values[5] * vector[1] + values[9] * vector[2] + values[13] * vector[3],
						values[2] * vector[0] + values[6] * vector[1] + values[10] * vector[2] + values[14] * vector[3],
						values[3] * vector[0] + values[7] * vector[1] + values[11] * vector[2] + values[15] * vector[3]);
}

#if defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 10

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION

// we keep following implementation inside 'OCEAN_USE_SLOWER_IMPLEMENTATION' showing an alternative (which is slower)

template <>
OCEAN_FORCE_INLINE VectorT4<double> SquareMatrixT4<double>::operator*(const VectorT4<double>& vector) const
{
	// the following code uses the following AVX instructions, and needs AVX1 or higher

	// AVX1:
	// _mm256_loadu_pd
	// _mm256_mul_pd
	// _mm256_shuffle_pd
	// _mm256_hadd_pd
	// _mm256_permute2f128_pd
	// _mm256_storeu_pd

	// first we load the first four rows of the matrix
	__m256d row0 = _mm256_loadu_pd(values + 0);
	__m256d row1 = _mm256_loadu_pd(values + 4);
	__m256d row2 = _mm256_loadu_pd(values + 8);
	__m256d row3 = _mm256_loadu_pd(values + 12);

	// we load the four values of the vector
	__m256d v = _mm256_loadu_pd(vector.data());

	// first, we transpose the 4x4 matrix

	// A E I M     A B C D
	// B F J N     E F G H
	// C G K O     I J K L
	// D H L P     M N O P

	// A B C D,     E F G H     ->    A E C G
	__m256d temp0 = _mm256_shuffle_pd(row0, row1, 0x00); // 0x00 = 0000 0000
	// A B C D,     E F G H     ->    B F D H
	__m256d temp2 = _mm256_shuffle_pd(row0, row1, 0x0F); // 0x0F = 0000 1111
	// I J K L,     M N O P     ->    I M K O
	__m256d temp1 = _mm256_shuffle_pd(row2, row3, 0x00);
	// I J K L,     M N O P     ->    J N L P
	__m256d temp3 = _mm256_shuffle_pd(row2, row3, 0x0F);

	// A E C G      I M K O     ->    A E I M
	row0 = _mm256_permute2f128_pd(temp0, temp1, 0x20); // 0x20 = 0010 0000
	// B F D H      J N L P     ->    B F J N
	row1 = _mm256_permute2f128_pd(temp2, temp3, 0x20);
	// A E C G      I M K O     ->    C G K O
	row2 = _mm256_permute2f128_pd(temp0, temp1, 0x31); // 0x31 = 0011 0001
	// B F D H      J N L P     ->    D H L P
	row3 = _mm256_permute2f128_pd(temp2, temp3, 0x31);

#ifdef OCEAN_COMPILER_MSC
	ocean_assert(row0.m256d_f64[0] == values[0] && row0.m256d_f64[1] == values[4] && row0.m256d_f64[2] == values[ 8] && row0.m256d_f64[3] == values[12]);
	ocean_assert(row1.m256d_f64[0] == values[1] && row1.m256d_f64[1] == values[5] && row1.m256d_f64[2] == values[ 9] && row1.m256d_f64[3] == values[13]);
	ocean_assert(row2.m256d_f64[0] == values[2] && row2.m256d_f64[1] == values[6] && row2.m256d_f64[2] == values[10] && row2.m256d_f64[3] == values[14]);
	ocean_assert(row3.m256d_f64[0] == values[3] && row3.m256d_f64[1] == values[7] && row3.m256d_f64[2] == values[11] && row3.m256d_f64[3] == values[15]);
#endif

	// unfortunately the AVX does not offer _mm256_dp_pd (the determination of the dot product) so we have to do it on our own

	__m256d r0v = _mm256_mul_pd(row0, v);
	__m256d r1v = _mm256_mul_pd(row1, v);
	__m256d r2v = _mm256_mul_pd(row2, v);
	__m256d r3v = _mm256_mul_pd(row3, v);

	// we sum both multiplication results horizontally (at least two neighboring products)
	__m256d sum_interleaved_r0_r1 = _mm256_hadd_pd(r0v, r1v);
	__m256d sum_interleaved_r2_r3 = _mm256_hadd_pd(r2v, r3v);

	// now we reorder the interleaved sums
	__m256d sum_first = _mm256_permute2f128_pd(sum_interleaved_r0_r1, sum_interleaved_r2_r3, 0x20); //  0x20 = 0010 0000
	__m256d sum_second = _mm256_permute2f128_pd(sum_interleaved_r0_r1, sum_interleaved_r2_r3, 0x31); // 0x31 = 0011 0001

	// we finally add both reordered sums
	__m256d sum = _mm256_add_pd(sum_first, sum_second);

	VectorT4<double> result;
	_mm256_storeu_pd(result.data(), sum);

	ocean_assert(NumericD::isEqual(result[0], values[0] * vector[0] + values[4] * vector[1] + values[ 8] * vector[2] + values[12] * vector[3], NumericD::eps() * 100));
	ocean_assert(NumericD::isEqual(result[1], values[1] * vector[0] + values[5] * vector[1] + values[ 9] * vector[2] + values[13] * vector[3], NumericD::eps() * 100));
	ocean_assert(NumericD::isEqual(result[2], values[2] * vector[0] + values[6] * vector[1] + values[10] * vector[2] + values[14] * vector[3], NumericD::eps() * 100));
	ocean_assert(NumericD::isEqual(result[3], values[3] * vector[0] + values[7] * vector[1] + values[11] * vector[2] + values[15] * vector[3], NumericD::eps() * 100));

	return result;
}

#else // OCEAN_USE_SLOWER_IMPLEMENTATION

template <>
OCEAN_FORCE_INLINE VectorT4<double> SquareMatrixT4<double>::operator*(const VectorT4<double>& vector) const
{
	// the following code uses the following AVX instructions, and needs AVX1 or higher

	// AVX1:
	// _mm256_broadcast_sd
	// _mm256_loadu_pd
	// _mm256_mul_pd
	// _mm256_add_pd
	// _mm256_storeu_pd

	// we use the same strategy as for the 32 bit float values

	// first we load the first vector element in all 64bit elements of the 256 bit register, so that we receive [a, a, a, a]
	__m256d v0 = _mm256_broadcast_sd(vector.data() + 0);

	// now we load the first column to receive: [A, B, C, D]
	__m256d c0 = _mm256_loadu_pd(values + 0);

	// now we multiply the 256 bit register [A, B, C, D] * [a, a, a, a] = [Aa, Ba, Ca, Da]
	__m256d r0 = _mm256_mul_pd(c0, v0);


	// now we proceed with the second column
	__m256d v1 = _mm256_broadcast_sd(vector.data() + 1);
	__m256d c1 = _mm256_loadu_pd(values + 4);
	__m256d r1 = _mm256_mul_pd(c1, v1);

	// and we sum the result of the first column with the result of the second column
	r0 = _mm256_add_pd(r0, r1);


	// now we proceed with the third column
	__m256d v2 = _mm256_broadcast_sd(vector.data() + 2);
	__m256d c2 = _mm256_loadu_pd(values + 8);
	__m256d r2 = _mm256_mul_pd(c2, v2);

	// we sum the results
	r0 = _mm256_add_pd(r0, r2);


	// now we proceed with the fourth column
	__m256d v3 = _mm256_broadcast_sd(vector.data() + 3);
	__m256d c3 = _mm256_loadu_pd(values + 12);
	__m256d r3 = _mm256_mul_pd(c3, v3);

	// we sum the results
	r0 = _mm256_add_pd(r0, r3);

	// and finally we store the results back to the vector
	VectorT4<double> result;

	_mm256_storeu_pd(result.data(), r0);

	return result;
}

#endif // OCEAN_USE_SLOWER_IMPLEMENTATION

#else // OCEAN_HARDWARE_AVX_VERSION

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20

template <>
OCEAN_FORCE_INLINE VectorT4<double> SquareMatrixT4<double>::operator*(const VectorT4<double>& vector) const
{
	// the following code uses the following SSE instructions, and needs SSE2 or higher

	// SSE2:
	// _mm_load1_pd
	// _mm_loadu_pd
	// _mm_add_pd
	// _mm_storeu_pd
	// _mm_mul_pd

	// we use the following strategy:
	// the values of the matrix are column aligned so that we normally would need to transpose the matrix before we can apply simple SIMD instructions
	// however, we do not transpose the matrix (we avoid the shuffle instructions) and instead multiply the matrix column-wise:
	// finally we sum the four columns and have the result, compared to the transpose-based approach this approach is approx. two times faster
	//
	// A E I M     a     Aa  +  Eb  +  Ic  +  Md
	// B F J N     b     Ba  +  Fb  +  Jc  +  Nd
	// C G K O  *  c  =  Ca  +  Gb  +  Kc  +  Od
	// D H L P     d     Da  +  Hb  +  Lc  +  Pd

	// first we load the first vector element in both 64 elements of the 128 bit register, so that we receive [a, a]
	__m128d v0 = _mm_load1_pd(vector.data() + 0);

	// now we load the first column to receive: [A, B]  and  [C, D]
	__m128d c0a = _mm_loadu_pd(values + 0);
	__m128d c0b = _mm_loadu_pd(values + 2);

	// now we multiply both 128 bit registers by: [A, B] * [a, a] = [Aa, Ba]  and  [C, D] * [a, a] = [Ca, Da]
	__m128d r0a = _mm_mul_pd(c0a, v0);
	__m128d r0b = _mm_mul_pd(c0b, v0);


	// now we proceed with the second column
	__m128d v1 = _mm_load1_pd(vector.data() + 1);

	__m128d c1a = _mm_loadu_pd(values + 4);
	__m128d c1b = _mm_loadu_pd(values + 6);

	__m128d r1a = _mm_mul_pd(c1a, v1);
	__m128d r1b = _mm_mul_pd(c1b, v1);

	// and we sum the result of the first column with the result of the second column
	r0a = _mm_add_pd(r0a, r1a);
	r0b = _mm_add_pd(r0b, r1b);


	// now we proceed with the third column
	__m128d v2 = _mm_load1_pd(vector.data() + 2);

	__m128d c2a = _mm_loadu_pd(values + 8);
	__m128d c2b = _mm_loadu_pd(values + 10);

	__m128d r2a = _mm_mul_pd(c2a, v2);
	__m128d r2b = _mm_mul_pd(c2b, v2);

	// we sum the results
	r0a = _mm_add_pd(r0a, r2a);
	r0b = _mm_add_pd(r0b, r2b);


	// now we proceed with the fourth column
	__m128d v3 = _mm_load1_pd(vector.data() + 3);

	__m128d c3a = _mm_loadu_pd(values + 12);
	__m128d c3b = _mm_loadu_pd(values + 14);

	__m128d r3a = _mm_mul_pd(c3a, v3);
	__m128d r3b = _mm_mul_pd(c3b, v3);

	// we sum the results
	r0a = _mm_add_pd(r0a, r3a);
	r0b = _mm_add_pd(r0b, r3b);

	// and finally we store the results back to the vector
	VectorT4<double> result;

	_mm_storeu_pd(result.data() + 0, r0a);
	_mm_storeu_pd(result.data() + 2, r0b);

	return result;
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 20

#endif // OCEAN_HARDWARE_AVX_VERSION >= 10

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION

// we keep following implementation inside 'OCEAN_USE_SLOWER_IMPLEMENTATION' showing an alternative (which is slower)

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

template <>
OCEAN_FORCE_INLINE VectorT4<float> SquareMatrixT4<float>::operator*(const VectorT4<float>& vector) const
{
	// the following code uses the following SSE instructions, and needs SSE 4.1 or higher

	// SSE:
	// _mm_loadu_ps
	// _mm_shuffle_ps
	// _mm_or_ps
	// _mm_storeu_ps

	// SSE4.1:
	// _mm_dp_ps

	// first we load the first four rows of the matrix
	__m128 row0 = _mm_loadu_ps(values + 0);
	__m128 row1 = _mm_loadu_ps(values + 4);
	__m128 row2 = _mm_loadu_ps(values + 8);
	__m128 row3 = _mm_loadu_ps(values + 12);

	// we load the four values of the vector
	__m128 v = _mm_loadu_ps(vector.data());

	// first, we transpose the 4x4 matrix

	// A E I M     A B C D
	// B F J N     E F G H
	// C G K O     I J K L
	// D H L P     M N O P

	// A B C D,     E F G H     ->    A B E F
	__m128 temp0 = _mm_shuffle_ps(row0, row1, 0x44); // 0x44 = 0100 0100
	// A B C D,     E F G H     ->    C D G H
	__m128 temp2 = _mm_shuffle_ps(row0, row1, 0xEE); // 0xEE = 1110 1110
	// I J K L,     M N O P     ->    I J M N
	__m128 temp1 = _mm_shuffle_ps(row2, row3, 0x44);
	// I J K L,     M N O P     ->    K L O P
	__m128 temp3 = _mm_shuffle_ps(row2, row3, 0xEE);

	// A B E F,     I J M N     ->    A E I M
	row0 = _mm_shuffle_ps(temp0, temp1, 0x88); // 0x88 = 1000 1000
	// A B E F,     I J M N     ->    B F J N
	row1 = _mm_shuffle_ps(temp0, temp1, 0xDD); // 0xDD = 1101 1101
	// C D G H,     K L O P     ->    C G K O
	row2 = _mm_shuffle_ps(temp2, temp3, 0x88);
	// C D G H,     K L O P     ->    D H L P
	row3 = _mm_shuffle_ps(temp2, temp3, 0xDD);

#ifdef OCEAN_COMPILER_MSC
	ocean_assert(row0.m128_f32[0] == values[0] && row0.m128_f32[1] == values[4] && row0.m128_f32[2] == values[ 8] && row0.m128_f32[3] == values[12]);
	ocean_assert(row1.m128_f32[0] == values[1] && row1.m128_f32[1] == values[5] && row1.m128_f32[2] == values[ 9] && row1.m128_f32[3] == values[13]);
	ocean_assert(row2.m128_f32[0] == values[2] && row2.m128_f32[1] == values[6] && row2.m128_f32[2] == values[10] && row2.m128_f32[3] == values[14]);
	ocean_assert(row3.m128_f32[0] == values[3] && row3.m128_f32[1] == values[7] && row3.m128_f32[2] == values[11] && row3.m128_f32[3] == values[15]);
#endif

	// we determine the dot product between the first row and the vector and store the result in the first float bin
	row0 = _mm_dp_ps(row0, v, 0xF1); // 0xF1 = 1111 0001

	// we determine the dot product between the second row and the vector and store the result in the second float bin
	row1 = _mm_dp_ps(row1, v, 0xF2); // 0xF2 = 1111 0010
	row2 = _mm_dp_ps(row2, v, 0xF4); // 0xF4 = 1111 0100
	row3 = _mm_dp_ps(row3, v, 0xF8); // 0xF8 = 1111 1000

	// now we blend the results by applying the bit-wise or operator
	__m128 result01 = _mm_or_ps(row0, row1);
	__m128 result23 = _mm_or_ps(row2, row3);
	__m128 result03 = _mm_or_ps(result01, result23);

	VectorT4<float> result;
	_mm_storeu_ps(result.data(), result03);

	return result;
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#else // OCEAN_USE_SLOWER_IMPLEMENTATION

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 10

template <>
OCEAN_FORCE_INLINE VectorT4<float> SquareMatrixT4<float>::operator*(const VectorT4<float>& vector) const
{
	// the following code uses the following SSE instructions, and needs SSE1 or higher

	// SSE1:
	// _mm_load1_ps
	// _mm_loadu_ps
	// _mm_mul_ps
	// _mm_add_ps
	// _mm_storeu_ps


	// we use the following strategy:
	// the values of the matrix are column aligned so that we normally would need to transpose the matrix before we can apply simple SIMD instructions
	// however, we do not transpose the matrix (we avoid the shuffle instructions) and instead multiply the matrix column-wise:
	// finally we sum the four columns and have the result, compared to the transpose-based approach this approach is approx. two times faster
	//
	// A E I M     a     Aa  +  Eb  +  Ic  +  Md
	// B F J N     b     Ba  +  Fb  +  Jc  +  Nd
	// C G K O  *  c  =  Ca  +  Gb  +  Kc  +  Od
	// D H L P     d     Da  +  Hb  +  Lc  +  Pd

	// first we load the first vector element in all 32bit elements of the 128 bit register, so that we receive [a, a, a, a]
	__m128 v0 = _mm_load1_ps(vector.data() + 0);

	// now we load the first column to receive: [A, B, C, D]
	__m128 c0 = _mm_loadu_ps(values + 0);

	// now we multiply the 128 bit register [A, B, C, D] * [a, a, a, a] = [Aa, Ba, Ca, Da]
	__m128 r0 = _mm_mul_ps(c0, v0);


	// now we proceed with the second column
	__m128 v1 = _mm_load1_ps(vector.data() + 1);
	__m128 c1 = _mm_loadu_ps(values + 4);
	__m128 r1 = _mm_mul_ps(c1, v1);

	// and we sum the result of the first column with the result of the second column
	r0 = _mm_add_ps(r0, r1);


	// now we proceed with the third column
	__m128 v2 = _mm_load1_ps(vector.data() + 2);
	__m128 c2 = _mm_loadu_ps(values + 8);
	__m128 r2 = _mm_mul_ps(c2, v2);

	// we sum the results
	r0 = _mm_add_ps(r0, r2);


	// now we proceed with the fourth column
	__m128 v3 = _mm_load1_ps(vector.data() + 3);
	__m128 c3 = _mm_loadu_ps(values + 12);
	__m128 r3 = _mm_mul_ps(c3, v3);

	// we sum the results
	r0 = _mm_add_ps(r0, r3);

	// and finally we store the results back to the vector
	VectorT4<float> result;

	_mm_storeu_ps(result.data(), r0);

	return result;
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 10

#endif // OCEAN_USE_SLOWER_IMPLEMENTATION

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#ifdef __aarch64__

template <>
OCEAN_FORCE_INLINE VectorT4<double> SquareMatrixT4<double>::operator*(const VectorT4<double>& vector) const
{
	// the following NEON code is almost identical to the SSE implementation

	// we use the following strategy:
	// the values of the matrix are column aligned so that we normally would need to transpose the matrix before we can apply simple SIMD instructions
	// however, we do not transpose the matrix (we avoid the shuffle instructions) and instead multiply the matrix column-wise:
	// finally we sum the four columns and have the result, compared to the transpose-based approach this approach is approx. two times faster
	//
	// A E I M     a     Aa  +  Eb  +  Ic  +  Md
	// B F J N     b     Ba  +  Fb  +  Jc  +  Nd
	// C G K O  *  c  =  Ca  +  Gb  +  Kc  +  Od
	// D H L P     d     Da  +  Hb  +  Lc  +  Pd

	// first we load the first vector element in both 64 elements of the 128 bit register, so that we receive [a, a]
	float64x2_t v0 = vld1q_dup_f64(vector.data() + 0);

	// now we load the first column to receive: [A, B]  and  [C, D]
	float64x2_t c0a = vld1q_f64(values + 0);
	float64x2_t c0b = vld1q_f64(values + 2);

	// now we multiply both 128 bit registers by: [A, B] * [a, a] = [Aa, Ba]  and  [C, D] * [a, a] = [Ca, Da]
	float64x2_t r0a = vmulq_f64(c0a, v0);
	float64x2_t r0b = vmulq_f64(c0b, v0);


	// now we proceed with the second column
	float64x2_t v1 = vld1q_dup_f64(vector.data() + 1);

	float64x2_t c1a = vld1q_f64(values + 4);
	float64x2_t c1b = vld1q_f64(values + 6);

	float64x2_t r1a = vmulq_f64(c1a, v1);
	float64x2_t r1b = vmulq_f64(c1b, v1);

	// and we sum the result of the first column with the result of the second column
	r0a = vaddq_f64(r0a, r1a);
	r0b = vaddq_f64(r0b, r1b);


	// now we proceed with the third column
	float64x2_t v2 = vld1q_dup_f64(vector.data() + 2);

	float64x2_t c2a = vld1q_f64(values + 8);
	float64x2_t c2b = vld1q_f64(values + 10);

	float64x2_t r2a = vmulq_f64(c2a, v2);
	float64x2_t r2b = vmulq_f64(c2b, v2);

	// we sum the results
	r0a = vaddq_f64(r0a, r2a);
	r0b = vaddq_f64(r0b, r2b);


	// now we proceed with the fourth column
	float64x2_t v3 = vld1q_dup_f64(vector.data() + 3);

	float64x2_t c3a = vld1q_f64(values + 12);
	float64x2_t c3b = vld1q_f64(values + 14);

	float64x2_t r3a = vmulq_f64(c3a, v3);
	float64x2_t r3b = vmulq_f64(c3b, v3);

	// we sum the results
	r0a = vaddq_f64(r0a, r3a);
	r0b = vaddq_f64(r0b, r3b);

	// and finally we store the results back to the vector
	VectorT4<double> result;

	vst1q_f64(result.data() + 0, r0a);
	vst1q_f64(result.data() + 2, r0b);

	return result;
}

#endif // __aarch64__

template <>
OCEAN_FORCE_INLINE VectorT4<float> SquareMatrixT4<float>::operator*(const VectorT4<float>& vector) const
{
	// the following NEON code is almost identical to the SSE implementation

	// we use the following strategy:
	// the values of the matrix are column aligned so that we normally would need to transpose the matrix before we can apply simple SIMD instructions
	// however, we do not transpose the matrix (we avoid the shuffle instructions) and instead multiply the matrix column-wise:
	// finally we sum the four columns and have the result, compared to the transpose-based approach this approach is approx. two times faster
	//
	// A E I M     a     Aa  +  Eb  +  Ic  +  Md
	// B F J N     b     Ba  +  Fb  +  Jc  +  Nd
	// C G K O  *  c  =  Ca  +  Gb  +  Kc  +  Od
	// D H L P     d     Da  +  Hb  +  Lc  +  Pd

	// first we load the first vector element in all 32bit elements of the 128 bit register, so that we receive [a, a, a, a]
	float32x4_t v0 = vld1q_dup_f32(vector.data() + 0);

	// now we load the first column to receive: [A, B, C, D]
	float32x4_t c0 = vld1q_f32(values + 0);

	// now we multiply the 128 bit register [A, B, C, D] * [a, a, a, a] = [Aa, Ba, Ca, Da]
	float32x4_t r0 = vmulq_f32(c0, v0);


	// now we proceed with the second column
	float32x4_t v1 = vld1q_dup_f32(vector.data() + 1);
	float32x4_t c1 = vld1q_f32(values + 4);
	float32x4_t r1 = vmulq_f32(c1, v1);

	// and we sum the result of the first column with the result of the second column
	r0 = vaddq_f32(r0, r1);


	// now we proceed with the third column
	float32x4_t v2 = vld1q_dup_f32(vector.data() + 2);
	float32x4_t c2 = vld1q_f32(values + 8);
	float32x4_t r2 = vmulq_f32(c2, v2);

	// we sum the results
	r0 = vaddq_f32(r0, r2);


	// now we proceed with the fourth column
	float32x4_t v3 = vld1q_dup_f32(vector.data() + 3);
	float32x4_t c3 = vld1q_f32(values + 12);
	float32x4_t r3 = vmulq_f32(c3, v3);

	// we sum the results
	r0 = vaddq_f32(r0, r3);

	// and finally we store the results back to the vector
	VectorT4<float> result;

	vst1q_f32(result.data(), r0);

	return result;
}

#endif

template <typename T>
SquareMatrixT4<T> SquareMatrixT4<T>::operator*(const T value) const
{
	SquareMatrixT4<T> result(*this);

	result.values[0] *= value;
	result.values[1] *= value;
	result.values[2] *= value;
	result.values[3] *= value;
	result.values[4] *= value;
	result.values[5] *= value;
	result.values[6] *= value;
	result.values[7] *= value;
	result.values[8] *= value;
	result.values[9] *= value;
	result.values[10] *= value;
	result.values[11] *= value;
	result.values[12] *= value;
	result.values[13] *= value;
	result.values[14] *= value;
	result.values[15] *= value;

	return result;
}

template <typename T>
SquareMatrixT4<T>& SquareMatrixT4<T>::operator*=(const T value)
{
	values[0] *= value;
	values[1] *= value;
	values[2] *= value;
	values[3] *= value;
	values[4] *= value;
	values[5] *= value;
	values[6] *= value;
	values[7] *= value;
	values[8] *= value;
	values[9] *= value;
	values[10] *= value;
	values[11] *= value;
	values[12] *= value;
	values[13] *= value;
	values[14] *= value;
	values[15] *= value;

	return *this;
}

template <typename T>
SquareMatrixT4<T> SquareMatrixT4<T>::projectionMatrix(const T fovX, const T aspectRatio, const T nearDistance, const T farDistance)
{
	/*
	 * <pre>
	 * Creates the following frustum projection matrix.
	 *
	 *  --------------------------------------------------
	 * |   t/a          0             0              0     |
	 * |    0           t             0              0     |
	 * |    0           0        (f+n)/(n-f)    -2fn/(n-f) |
	 * |    0           0            -1              0     |
	 *  --------------------------------------------------
	 *
	 * With: t = 1 / tan (fovY / 2), a = aspectRatio, n = nearDistance, f = farDistance
	 * </pre>
	 */

	ocean_assert(fovX > 0 && fovX < NumericT<T>::pi());
	ocean_assert(aspectRatio > 0);
	ocean_assert(nearDistance > 0);
	ocean_assert(nearDistance < farDistance);

	const T fovY = T(2.0) * NumericT<T>::atan(NumericT<T>::tan(T(0.5) * fovX) / aspectRatio);

	SquareMatrixT4<T> matrix(false);
	ocean_assert(matrix(1, 0) == 0 && matrix(2, 0) == 0 && matrix(3, 0) == 0);
	ocean_assert(matrix(0, 1) == 0 && matrix(2, 1) == 0 && matrix(3, 1) == 0);
	ocean_assert(matrix(0, 2) == 0 && matrix(1, 2) == 0);
	ocean_assert(matrix(0, 3) == 0 && matrix(1, 3) == 0 && matrix(3, 3) == 0);

	ocean_assert(NumericT<T>::isNotEqual(farDistance, nearDistance));

	const T f = T(1.0) / NumericT<T>::tan(fovY * T(0.5));
	const T factor = T(1.0) / (nearDistance - farDistance);

	matrix(0, 0) = f / aspectRatio;
	matrix(1, 1) = f;
	matrix(2, 2) = (farDistance + nearDistance) * factor;
	matrix(3, 2) = -T(1.0);
	matrix(2, 3) = (T(2.0) * farDistance * nearDistance) * factor;

	return matrix;
}

template <typename T>
SquareMatrixT4<T> SquareMatrixT4<T>::projectionMatrix(const AnyCameraT<T>& anyCamera, const T nearDistance, const T farDistance)
{
	/*
	 * <pre>
	 * Creates the following frustum projection matrix.
	 *
	 *  --------------------------------------------------
	 * |   Fx           0             mx              0    |
	 * |    0          Fy             my              0    |
	 * |    0           0        (f+n)/(n-f)    -2fn/(n-f) |
	 * |    0           0            -1              0     |
	 *  --------------------------------------------------
	 *
	 * n = nearDistance, f = farDistance
	 * </pre>
	 */

	ocean_assert(anyCamera.isValid());

	ocean_assert(nearDistance > 0);
	ocean_assert(nearDistance < farDistance);

	const T fxPixel = anyCamera.focalLengthX();
	const T fyPixel = anyCamera.focalLengthY();
	ocean_assert(fxPixel > T(1) && fyPixel > T(1));

	const T mxPixel = anyCamera.principalPointX();
	const T myPixel = anyCamera.principalPointY();

	const T width_2 = T(anyCamera.width()) / T(2);
	const T height_2 = T(anyCamera.height()) / T(2);

	ocean_assert(NumericT<T>::isNotEqualEps(width_2));
	ocean_assert(NumericT<T>::isNotEqualEps(height_2));

	const T fx = fxPixel / width_2;
	const T fy = fyPixel / height_2;

	const T mx = (mxPixel - width_2) / width_2; // principal point with range [-1, 1]
	const T my = (myPixel - height_2) / height_2;

	const T factor = T(1.0) / (nearDistance - farDistance);

	SquareMatrixT4<T> matrix(false);

	matrix(0, 0) = fx;
	matrix(1, 1) = fy;
	matrix(0, 2) = -mx;
	matrix(1, 2) = my;
	matrix(2, 2) = (farDistance + nearDistance) * factor;
	matrix(3, 2) = -T(1);
	matrix(2, 3) = (T(2) * farDistance * nearDistance) * factor;

	return matrix;
}

template <typename T>
SquareMatrixT4<T> SquareMatrixT4<T>::frustumMatrix(const T left, const T right, const T top, const T bottom, const T nearDistance, const T farDistance)
{
	/**
	 * <pre>
	 * Creates the following frustum projection matrix:
	 *
	 *  --------------------------------------------------
	 * | 2n/(r-l)       0       (r+l)/(r-l)         0     |
	 * |    0        2n/(t-b)   (t+b)/(t-b)         0     |
	 * |    0           0      -(f+n)/(f-n)    -2fn/(f-n) |
	 * |    0           0           -1              0     |
	 *  --------------------------------------------------
	 * </pre>
	 */

	ocean_assert(NumericT<T>::isNotEqual(left, right));
	ocean_assert(NumericT<T>::isNotEqual(top, bottom));
	ocean_assert(NumericT<T>::isNotEqual(nearDistance, farDistance));

	SquareMatrixT4<T> matrix(false);
	ocean_assert(matrix(1, 0) == 0 && matrix(2, 0) == 0 && matrix(3, 0) == 0);
	ocean_assert(matrix(0, 1) == 0 && matrix(2, 1) == 0 && matrix(3, 1) == 0);
	ocean_assert(matrix(0, 3) == 0 && matrix(1, 3) == 0 && matrix(3, 3) == 0);

	const T rightLeft = T(1.0) / (right - left);
	const T near2 = nearDistance * T(2.0);

	matrix(0, 0) = near2 * rightLeft;
	matrix(0, 2) = (right + left) * rightLeft;

	const T topBottom = T(1.0) / (top - bottom);

	matrix(1, 1) = near2 * topBottom;
	matrix(1, 2) = (top + bottom) * topBottom;

	const T farNear = T(1.0) / (farDistance - nearDistance);

	matrix(2, 2) = -(farDistance + nearDistance) * farNear;
	matrix(2, 3) = -T(2.0) * farDistance * nearDistance * farNear;

	matrix(3, 2) = -T(1.0);

	return matrix;
}

template <typename T>
SquareMatrixT4<T> SquareMatrixT4<T>::frustumMatrix(const T width, const T height, const HomogenousMatrixT4<T>& viewingMatrix, const T nearDistance, const T farDistance)
{
	ocean_assert(width > NumericT<T>::eps() && height > NumericT<T>::eps());
	ocean_assert(nearDistance >= NumericT<T>::eps() && farDistance > nearDistance);

	const T planeDistance = NumericT<T>::abs(viewingMatrix.translation().z());
	ocean_assert(viewingMatrix.isValid() && NumericT<T>::isNotEqualEps(planeDistance));

	const HomogenousMatrixT4<T> inversedViewingMatrix(viewingMatrix.inverted());

	const VectorT3<T> leftTop(width * -T(0.5), height * T(0.5), 0);
	const VectorT3<T> rightBottom(width * T(0.5), height * -T(0.5), 0);

	const VectorT3<T> leftTopInCamera(inversedViewingMatrix * leftTop);
	const VectorT3<T> rightBottomInCamera(inversedViewingMatrix * rightBottom);

	const T factor = nearDistance / planeDistance;

	return frustumMatrix(factor * leftTopInCamera.x(), factor * rightBottomInCamera.x(), factor * leftTopInCamera.y(), factor * rightBottomInCamera.y(), nearDistance, farDistance);
}

template <typename T>
void SquareMatrixT4<T>::multiply(const SquareMatrixT4<T>& matrix, const VectorT4<T>* vectors, VectorT4<T>* results, const size_t number)
{
	ocean_assert((vectors && results) || number == 0);

	for (size_t n = 0; n < number; ++n)
	{
		results[n] = matrix * vectors[n];
	}
}

#if defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 10

template <>
inline void SquareMatrixT4<double>::multiply(const SquareMatrixT4<double>& matrix, const VectorT4<double>* vectors, VectorT4<double>* results, const size_t number)
{
	// the following code uses the following AVX instructions, and needs AVX1 or higher

	// AVX1:
	// _mm256_broadcast_sd
	// _mm256_loadu_pd
	// _mm256_mul_pd
	// _mm256_add_pd
	// _mm256_storeu_pd

	// we use the same strategy as for the 32 bit float values

	__m256d c0 = _mm256_loadu_pd(matrix.values + 0);
	__m256d c1 = _mm256_loadu_pd(matrix.values + 4);
	__m256d c2 = _mm256_loadu_pd(matrix.values + 8);
	__m256d c3 = _mm256_loadu_pd(matrix.values + 12);

	for (size_t n = 0; n < number; ++n)
	{
		__m256d v0 = _mm256_broadcast_sd(vectors[n].data() + 0);
		__m256d r0 = _mm256_mul_pd(c0, v0);

		__m256d v1 = _mm256_broadcast_sd(vectors[n].data() + 1);
		__m256d r1 = _mm256_mul_pd(c1, v1);

		r0 = _mm256_add_pd(r0, r1);

		__m256d v2 = _mm256_broadcast_sd(vectors[n].data() + 2);
		__m256d r2 = _mm256_mul_pd(c2, v2);

		r0 = _mm256_add_pd(r0, r2);

		__m256d v3 = _mm256_broadcast_sd(vectors[n].data() + 3);
		__m256d r3 = _mm256_mul_pd(c3, v3);

		r0 = _mm256_add_pd(r0, r3);

		_mm256_storeu_pd(results[n].data(), r0);
	}
}

#else // OCEAN_HARDWARE_AVX_VERSION

#ifdef OCEAN_USE_SLOWER_IMPLEMENTATION

// we keep following implementation inside 'OCEAN_USE_SLOWER_IMPLEMENTATION' showing an alternative (which is slower)

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

template <>
inline void SquareMatrixT4<float>::multiply(const SquareMatrixT4<float>& matrix, const VectorT4<float>* vectors, VectorT4<float>* results, const size_t number)
{
	// the following code uses the following SSE instructions, and needs SSE 4.1 or higher

	// SSE:
	// _mm_loadu_ps
	// _mm_shuffle_ps
	// _mm_or_ps
	// _mm_storeu_ps

	// SSE4.1:
	// _mm_dp_ps

	// first we load the four rows of the matrix
	__m128 row0 = _mm_loadu_ps(matrix.values + 0);
	__m128 row1 = _mm_loadu_ps(matrix.values + 4);
	__m128 row2 = _mm_loadu_ps(matrix.values + 8);
	__m128 row3 = _mm_loadu_ps(matrix.values + 12);

	// first, we transpose the 4x4 matrix

	// A E I M     A B C D
	// B F J N     E F G H
	// C G K O     I J K L
	// D H L P     M N O P

	// A B C D,     E F G H     ->    A B E F
	__m128 temp0 = _mm_shuffle_ps(row0, row1, 0x44); // 0x44 = 0100 0100
	// A B C D,     E F G H     ->    C D G H
	__m128 temp2 = _mm_shuffle_ps(row0, row1, 0xEE); // 0xEE = 1110 1110
	// I J K L,     M N O P     ->    I J M N
	__m128 temp1 = _mm_shuffle_ps(row2, row3, 0x44);
	// I J K L,     M N O P     ->    K L O P
	__m128 temp3 = _mm_shuffle_ps(row2, row3, 0xEE);

	// A B E F,     I J M N     ->    A E I M
	row0 = _mm_shuffle_ps(temp0, temp1, 0x88); // 0x88 = 1000 1000
	// A B E F,     I J M N     ->    B F J N
	row1 = _mm_shuffle_ps(temp0, temp1, 0xDD); // 0xDD = 1101 1101
	// C D G H,     K L O P     ->    C G K O
	row2 = _mm_shuffle_ps(temp2, temp3, 0x88);
	// C D G H,     K L O P     ->    D H L P
	row3 = _mm_shuffle_ps(temp2, temp3, 0xDD);

#ifdef OCEAN_COMPILER_MSC
	ocean_assert(row0.m128_f32[0] == matrix.values[0] && row0.m128_f32[1] == matrix.values[4] && row0.m128_f32[2] == matrix.values[ 8] && row0.m128_f32[3] == matrix.values[12]);
	ocean_assert(row1.m128_f32[0] == matrix.values[1] && row1.m128_f32[1] == matrix.values[5] && row1.m128_f32[2] == matrix.values[ 9] && row1.m128_f32[3] == matrix.values[13]);
	ocean_assert(row2.m128_f32[0] == matrix.values[2] && row2.m128_f32[1] == matrix.values[6] && row2.m128_f32[2] == matrix.values[10] && row2.m128_f32[3] == matrix.values[14]);
	ocean_assert(row3.m128_f32[0] == matrix.values[3] && row3.m128_f32[1] == matrix.values[7] && row3.m128_f32[2] == matrix.values[11] && row3.m128_f32[3] == matrix.values[15]);
#endif

	for (size_t n = 0u; n < number; ++n)
	{
		// we load the four values of the vector
		__m128 v = _mm_loadu_ps(vectors[n].data());

		// we determine the dot product between the first row and the vector and store the result in the first float bin
		__m128 dot0 = _mm_dp_ps(row0, v, 0xF1); // 0xF1 = 1111 0001

		// we determine the dot product between the second row and the vector and store the result in the second float bin
		__m128 dot1 = _mm_dp_ps(row1, v, 0xF2); // 0xF2 = 1111 0010
		__m128 dot2 = _mm_dp_ps(row2, v, 0xF4); // 0xF4 = 1111 0100
		__m128 dot3 = _mm_dp_ps(row3, v, 0xF8); // 0xF8 = 1111 1000

		// now we blend the results by applying the bit-wise or operator
		__m128 result01 = _mm_or_ps(dot0, dot1);
		__m128 result23 = _mm_or_ps(dot2, dot3);
		__m128 result03 = _mm_or_ps(result01, result23);

		_mm_storeu_ps(results[n].data(), result03);
	}
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#else // OCEAN_USE_SLOWER_IMPLEMENTATION

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20

template <>
inline void SquareMatrixT4<double>::multiply(const SquareMatrixT4<double>& matrix, const VectorT4<double>* vectors, VectorT4<double>* results, const size_t number)
{
	// the following code uses the following SSE instructions, and needs SSE2 or higher

	// SSE2:
	// _mm_load1_pd
	// _mm_loadu_pd
	// _mm_add_pd
	// _mm_storeu_pd
	// _mm_mul_pd

	// now we load the four columns
	__m128d c0a = _mm_loadu_pd(matrix.values + 0);
	__m128d c0b = _mm_loadu_pd(matrix.values + 2);
	__m128d c1a = _mm_loadu_pd(matrix.values + 4);
	__m128d c1b = _mm_loadu_pd(matrix.values + 6);
	__m128d c2a = _mm_loadu_pd(matrix.values + 8);
	__m128d c2b = _mm_loadu_pd(matrix.values + 10);
	__m128d c3a = _mm_loadu_pd(matrix.values + 12);
	__m128d c3b = _mm_loadu_pd(matrix.values + 14);

	for (size_t n = 0u; n < number; ++n)
	{
		__m128d v0 = _mm_load1_pd(vectors[n].data() + 0);
		__m128d v1 = _mm_load1_pd(vectors[n].data() + 1);
		__m128d v2 = _mm_load1_pd(vectors[n].data() + 2);
		__m128d v3 = _mm_load1_pd(vectors[n].data() + 3);

		// first column
		__m128d r0a = _mm_mul_pd(c0a, v0);
		__m128d r0b = _mm_mul_pd(c0b, v0);

		// second column
		__m128d r1a = _mm_mul_pd(c1a, v1);
		__m128d r1b = _mm_mul_pd(c1b, v1);

		r0a = _mm_add_pd(r0a, r1a);
		r0b = _mm_add_pd(r0b, r1b);

		// third column
		__m128d r2a = _mm_mul_pd(c2a, v2);
		__m128d r2b = _mm_mul_pd(c2b, v2);
		r0a = _mm_add_pd(r0a, r2a);
		r0b = _mm_add_pd(r0b, r2b);

		// fourth column
		__m128d r3a = _mm_mul_pd(c3a, v3);
		__m128d r3b = _mm_mul_pd(c3b, v3);
		r0a = _mm_add_pd(r0a, r3a);
		r0b = _mm_add_pd(r0b, r3b);

		_mm_storeu_pd(results[n].data() + 0, r0a);
		_mm_storeu_pd(results[n].data() + 2, r0b);
	}
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 20

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 10

template <>
inline void SquareMatrixT4<float>::multiply(const SquareMatrixT4<float>& matrix, const VectorT4<float>* vectors, VectorT4<float>* results, const size_t number)
{
	// the following code uses the following SSE instructions, and needs SSE1 or higher

	// SSE1:
	// _mm_load1_ps
	// _mm_loadu_ps
	// _mm_mul_ps
	// _mm_add_ps
	// _mm_storeu_ps

	// now we load the four columns
	__m128 c0 = _mm_loadu_ps(matrix.values + 0);
	__m128 c1 = _mm_loadu_ps(matrix.values + 4);
	__m128 c2 = _mm_loadu_ps(matrix.values + 8);
	__m128 c3 = _mm_loadu_ps(matrix.values + 12);

	for (size_t n = 0u; n < number; ++n)
	{
		__m128 v0 = _mm_load1_ps(vectors[n].data() + 0);
		__m128 v1 = _mm_load1_ps(vectors[n].data() + 1);
		__m128 v2 = _mm_load1_ps(vectors[n].data() + 2);
		__m128 v3 = _mm_load1_ps(vectors[n].data() + 3);

		// first column
		__m128 r0 = _mm_mul_ps(c0, v0);

		// second column
		__m128 r1 = _mm_mul_ps(c1, v1);

		r0 = _mm_add_ps(r0, r1);

		// third column
		__m128 r2 = _mm_mul_ps(c2, v2);
		r0 = _mm_add_ps(r0, r2);

		// fourth column
		__m128 r3 = _mm_mul_ps(c3, v3);
		r0 = _mm_add_ps(r0, r3);

		_mm_storeu_ps(results[n].data(), r0);
	}
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 10

#endif // OCEAN_USE_SLOWER_IMPLEMENTATION

#endif // OCEAN_HARDWARE_AVX_VERSION

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

#ifdef __aarch64__

template <>
inline void SquareMatrixT4<double>::multiply(const SquareMatrixT4<double>& matrix, const VectorT4<double>* vectors, VectorT4<double>* results, const size_t number)
{
	// the following NEON code is almost identical to the SSE implementation

	// now we load the four columns
	float64x2_t c0a = vld1q_f64(matrix.values + 0);
	float64x2_t c0b = vld1q_f64(matrix.values + 2);
	float64x2_t c1a = vld1q_f64(matrix.values + 4);
	float64x2_t c1b = vld1q_f64(matrix.values + 6);
	float64x2_t c2a = vld1q_f64(matrix.values + 8);
	float64x2_t c2b = vld1q_f64(matrix.values + 10);
	float64x2_t c3a = vld1q_f64(matrix.values + 12);
	float64x2_t c3b = vld1q_f64(matrix.values + 14);

	for (size_t n = 0u; n < number; ++n)
	{
		float64x2_t v0 = vld1q_dup_f64(vectors[n].data() + 0);
		float64x2_t v1 = vld1q_dup_f64(vectors[n].data() + 1);
		float64x2_t v2 = vld1q_dup_f64(vectors[n].data() + 2);
		float64x2_t v3 = vld1q_dup_f64(vectors[n].data() + 3);

		float64x2_t r0a = vmulq_f64(c0a, v0);
		float64x2_t r0b = vmulq_f64(c0b, v0);

		float64x2_t r1a = vmulq_f64(c1a, v1);
		float64x2_t r1b = vmulq_f64(c1b, v1);

		r0a = vaddq_f64(r0a, r1a);
		r0b = vaddq_f64(r0b, r1b);

		float64x2_t r2a = vmulq_f64(c2a, v2);
		float64x2_t r2b = vmulq_f64(c2b, v2);

		r0a = vaddq_f64(r0a, r2a);
		r0b = vaddq_f64(r0b, r2b);

		float64x2_t r3a = vmulq_f64(c3a, v3);
		float64x2_t r3b = vmulq_f64(c3b, v3);

		r0a = vaddq_f64(r0a, r3a);
		r0b = vaddq_f64(r0b, r3b);

		vst1q_f64(results[n].data() + 0, r0a);
		vst1q_f64(results[n].data() + 2, r0b);
	}
}

#endif // __aarch64__

template <>
inline void SquareMatrixT4<float>::multiply(const SquareMatrixT4<float>& matrix, const VectorT4<float>* vectors, VectorT4<float>* results, const size_t number)
{
	// the following NEON code is almost identical to the SSE implementation

	// now we load the four columns
	float32x4_t c0 = vld1q_f32(matrix.values + 0);
	float32x4_t c1 = vld1q_f32(matrix.values + 4);
	float32x4_t c2 = vld1q_f32(matrix.values + 8);
	float32x4_t c3 = vld1q_f32(matrix.values + 12);

	for (size_t n = 0u; n < number; ++n)
	{
		float32x4_t v0 = vld1q_dup_f32(vectors[n].data() + 0);
		float32x4_t v1 = vld1q_dup_f32(vectors[n].data() + 1);
		float32x4_t v2 = vld1q_dup_f32(vectors[n].data() + 2);
		float32x4_t v3 = vld1q_dup_f32(vectors[n].data() + 3);

		// first column
		float32x4_t r0 = vmulq_f32(c0, v0);

		// second column
		float32x4_t r1 = vmulq_f32(c1, v1);

		r0 = vaddq_f32(r0, r1);

		// third column
		float32x4_t r2 = vmulq_f32(c2, v2);

		r0 = vaddq_f32(r0, r2);

		// fourth column
		float32x4_t r3 = vmulq_f32(c3, v3);

		r0 = vaddq_f32(r0, r3);

		vst1q_f32(results[n].data(), r0);
	}
}

#endif

template <typename T>
template <typename U>
inline std::vector< SquareMatrixT4<T> > SquareMatrixT4<T>::matrices2matrices(const std::vector< SquareMatrixT4<U> >& matrices)
{
	std::vector< SquareMatrixT4<T> > result;
	result.reserve(matrices.size());

	for (typename std::vector< SquareMatrixT4<U> >::const_iterator i = matrices.begin(); i != matrices.end(); ++i)
	{
		result.push_back(SquareMatrixT4<T>(*i));
	}

	return result;
}

template <>
template <>
inline std::vector< SquareMatrixT4<float> > SquareMatrixT4<float>::matrices2matrices(const std::vector< SquareMatrixT4<float> >& matrices)
{
	return matrices;
}

template <>
template <>
inline std::vector< SquareMatrixT4<double> > SquareMatrixT4<double>::matrices2matrices(const std::vector< SquareMatrixT4<double> >& matrices)
{
	return matrices;
}

template <typename T>
template <typename U>
inline std::vector< SquareMatrixT4<T> > SquareMatrixT4<T>::matrices2matrices(const SquareMatrixT4<U>* matrices, const size_t size)
{
	std::vector< SquareMatrixT4<T> > result;
	result.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		result.push_back(SquareMatrixT4<T>(matrices[n]));
	}

	return result;
}

template <typename T>
void SquareMatrixT4<T>::swapRows(const unsigned int row0, const unsigned int row1)
{
	ocean_assert(row0 < 4u && row1 < 4u);

	if (row0 == row1)
	{
		return;
	}

	T* first = values + row0;
	T* second = values + row1;

	T tmp = *first;
	*first = *second;
	*second = tmp;

	first += 4;
	second += 4;
	tmp = *first;
	*first = *second;
	*second = tmp;

	first += 4;
	second += 4;
	tmp = *first;
	*first = *second;
	*second = tmp;

	first += 4;
	second += 4;
	tmp = *first;
	*first = *second;
	*second = tmp;
}

template <typename T>
void SquareMatrixT4<T>::multiplyRow(const unsigned int row, const T scalar)
{
	ocean_assert(row < 4u);

	T* element = values + row;

	*element *= scalar;
	element += 4;
	*element *= scalar;
	element += 4;
	*element *= scalar;
	element += 4;
	*element *= scalar;
}

template <typename T>
void SquareMatrixT4<T>::addRows(const unsigned int targetRow, unsigned int const sourceRow, const T scalar)
{
	ocean_assert(targetRow < 4u && sourceRow < 4u);
	ocean_assert(targetRow != sourceRow);

	T* target = values + targetRow;
	T* source = values + sourceRow;

	*target += *source * scalar;

	target += 4;
	source += 4;
	*target += *source * scalar;

	target += 4;
	source += 4;
	*target += *source * scalar;

	target += 4;
	source += 4;
	*target += *source * scalar;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const SquareMatrixT4<T>& matrix)
{
	stream << "|" << matrix(0, 0) << ", " << matrix(0, 1) << ", " << matrix(0, 2) << ", " << matrix(0, 3) << "|" << std::endl;
	stream << "|" << matrix(1, 0) << ", " << matrix(1, 1) << ", " << matrix(1, 2) << ", " << matrix(1, 3) << "|" << std::endl;
	stream << "|" << matrix(2, 0) << ", " << matrix(2, 1) << ", " << matrix(2, 2) << ", " << matrix(2, 3) << "|" << std::endl;
	stream << "|" << matrix(3, 0) << ", " << matrix(3, 1) << ", " << matrix(3, 2) << ", " << matrix(3, 3) << "|";

	return stream;
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const SquareMatrixT4<T>& matrix)
{
	return messageObject << "|" << matrix(0, 0) << ", " << matrix(0, 1) << ", " << matrix(0, 2) << ", " << matrix(0, 3) << "|\n|"
							<< matrix(1, 0) << ", " << matrix(1, 1) << ", " << matrix(1, 2) << ", " << matrix(1, 3) << "|\n|"
							<< matrix(2, 0) << ", " << matrix(2, 1) << ", " << matrix(2, 2) << ", " << matrix(2, 3) << "|\n|"
							<< matrix(3, 0) << ", " << matrix(3, 1) << ", " << matrix(3, 2) << ", " << matrix(3, 3) << "|";
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const SquareMatrixT4<T>& matrix)
{
	return messageObject << "|" << matrix(0, 0) << ", " << matrix(0, 1) << ", " << matrix(0, 2) << ", " << matrix(0, 3) << "|\n|"
							<< matrix(1, 0) << ", " << matrix(1, 1) << ", " << matrix(1, 2) << ", " << matrix(1, 3) << "|\n|"
							<< matrix(2, 0) << ", " << matrix(2, 1) << ", " << matrix(2, 2) << ", " << matrix(2, 3) << "|\n|"
							<< matrix(3, 0) << ", " << matrix(3, 1) << ", " << matrix(3, 2) << ", " << matrix(3, 3) << "|";
}

}

#endif // META_OCEAN_MATH_SQUARE_MATRIX_4_H
