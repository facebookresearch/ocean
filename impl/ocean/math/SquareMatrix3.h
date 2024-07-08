/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_SQUARE_MATRIX_3_H
#define META_OCEAN_MATH_SQUARE_MATRIX_3_H

#include "ocean/math/Math.h"
#include "ocean/math/Equation.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include "ocean/base/Utilities.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class EulerT;
// Forward declaration.
template <typename T> class HomogenousMatrixT4;
// Forward declaration.
template <typename T> class QuaternionT;
// Forward declaration.
template <typename T> class RotationT;
// Forward declaration.
template <typename T> class SquareMatrixT4;

// Forward declaration.
template <typename T> class SquareMatrixT3;

/**
 * Definition of the SquareMatrix3 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see SquareMatrixT3
 * @ingroup math
 */
typedef SquareMatrixT3<Scalar> SquareMatrix3;

/**
 * Instantiation of the SquareMatrixT3 template class using a double precision float data type.
 * @see SquareMatrixT3
 * @ingroup math
 */
typedef SquareMatrixT3<double> SquareMatrixD3;

/**
 * Instantiation of the SquareMatrixT3 template class using a single precision float data type.
 * @see SquareMatrixT3
 * @ingroup math
 */
typedef SquareMatrixT3<float> SquareMatrixF3;

/**
 * Definition of a typename alias for vectors with SquareMatrixT3 objects.
 * @see SquareMatrixT3
 * @ingroup math
 */
template <typename T>
using SquareMatricesT3 = std::vector<SquareMatrixT3<T>>;

/**
 * Definition of a vector holding SquareMatrix3 objects.
 * @see SquareMatrix3
 * @ingroup math
 */
typedef std::vector<SquareMatrix3> SquareMatrices3;

/**
 * This class implements a 3x3 square matrix.
 * The matrix can be applied as e.g., rotation matrix for 3D vectors or can represent a Homography and so on.<br>
 * The values are stored in a column aligned order with indices:
 * <pre>
 * | 0 3 6 |
 * | 1 4 7 |
 * | 2 5 8 |
 * </pre>
 * @tparam T Data type of matrix elements
 * @see SquareMatrix3, SquareMatrixF3, SquareMatrixD3, Rotation, Euler, Quaternion, ExponentialMap.
 * @ingroup math
 */
template <typename T>
class SquareMatrixT3
{
	template <typename U> friend class SquareMatrixT3;

	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates a new SquareMatrixT3 object with undefined elements.
		 * Beware: This matrix is neither a zero nor an entity matrix!
		 */
		SquareMatrixT3();

		/**
		 * Copy constructor.
		 * @param matrix The matrix to copy
		 */
		SquareMatrixT3(const SquareMatrixT3<T>& matrix) = default;

		/**
		 * Copy constructor for a matrix with difference element data type than T.
		 * @param matrix The matrix to copy
		 * @tparam U The element data type of the second matrix
		 */
		template <typename U>
		inline explicit SquareMatrixT3(const SquareMatrixT3<U>& matrix);

		/**
		 * Creates a new SquareMatrixT3 object.
		 * @param setToIdentity Determines whether a entity matrix will be created, otherwise the matrix is initialized with zeros
		 */
		explicit SquareMatrixT3(const bool setToIdentity);

		/**
		 * Creates a new SquareMatrixT3 rotation matrix by a given Euler rotation.
		 * @param euler Euler rotation to create a rotation matrix from, must be valid
		 */
		explicit SquareMatrixT3(const EulerT<T>& euler);

		/**
		 * Creates a new 3x3 matrix object by a given angle-axis rotation.
		 * @param rotation The angle-axis rotation to create a matrix from, must be valid
		 */
		explicit SquareMatrixT3(const RotationT<T>& rotation);

		/**
		 * Creates a new 3x3 matrix object by a given quaternion rotation.
		 * @param quaternion The quaternion rotation to create a matrix from, must be valid
		 */
		explicit SquareMatrixT3(const QuaternionT<T>& quaternion);

		/**
		 * Creates a new SquareMatrixT3 object by three axes.
		 * @param xAxis First column of the matrix
		 * @param yAxis Middle column of the matrix
		 * @param zAxis Last column of the matrix
		 */
		SquareMatrixT3(const VectorT3<T>& xAxis, const VectorT3<T>& yAxis, const VectorT3<T>& zAxis);

		/**
		 * Creates a new SquareMatrixT3 object by a given diagonal vector.
		 * @param diagonal The diagonal vector for the new matrix
		 */
		explicit SquareMatrixT3(const VectorT3<T>& diagonal);

		/**
		 * Creates a new SquareMatrixT3 object by nine elements of float type U.
		 * @param arrayValues The nine matrix elements defining the new matrix, must be valid
		 * @tparam U The floating point type of the given elements
		 */
		template <typename U>
		explicit SquareMatrixT3(const U* arrayValues);

		/**
		 * Creates a new SquareMatrixT3 object by nine elements.
		 * @param arrayValues The nine matrix elements defining the new matrix, must be valid
		 */
		explicit SquareMatrixT3(const T* arrayValues);

		/**
		 * Creates a new SquareMatrixT3 object by nine elements.
		 * @param arrayValues The nine matrix elements defining the new matrix, must be valid
		 * @param valuesRowAligned True, if the given values are stored in a row aligned order; False, if the values are stored in a column aligned order (which is the default case for this matrix)
		 * @tparam U The floating point type of the given elements
		 */
		template <typename U>
		SquareMatrixT3(const U* arrayValues, const bool valuesRowAligned);

		/**
		 * Creates a new SquareMatrixT3 object by nine elements.
		 * @param arrayValues The nine matrix elements defining the new matrix, must be valid
		 * @param valuesRowAligned True, if the given values are stored in a row aligned order; False, if the values are stored in a column aligned order (which is the default case for this matrix)
		 */
		SquareMatrixT3(const T* arrayValues, const bool valuesRowAligned);

		/**
		 * Creates a 3x3 rotation matrix by a given 4x4 homogeneous transformation.
		 * @param transformation The transformation to create a 3x3 rotation matrix from
		 */
		explicit SquareMatrixT3(const HomogenousMatrixT4<T>& transformation);

		/**
		 * Creates a 3x3 square matrix by a given 4x4 square transformation.
		 * @param transformation The transformation to create a 3x3 square matrix from
		 */
		explicit SquareMatrixT3(const SquareMatrixT4<T>& transformation);

		/**
		 * Creates a 3x3 rotation matrix by 9 given matrix elements.
		 * @param m00 Element of the first row and first column
		 * @param m10 Element of the second row and first column
		 * @param m20 Element of the third row and first column
		 * @param m01 Element of the first row and second column
		 * @param m11 Element of the second row and second column
		 * @param m21 Element of the third row and second column
		 * @param m02 Element of the first row and third column
		 * @param m12 Element of the second row and third column
		 * @param m22 Element of the third row and third column
		 */
		explicit SquareMatrixT3(const T& m00, const T& m10, const T& m20, const T& m01, const T& m11, const T& m21, const T& m02, const T& m12, const T& m22);

		/**
		 * Returns the transposed of this matrix.
		 * @return Transposed matrix
		 */
		SquareMatrixT3<T> transposed() const;

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
		SquareMatrixT3<T> inverted() const;

		/**
		 * Inverts this matrix in place.
		 * @return True, if the matrix is not singular and could be inverted
		 * @see inverted(), solve().
		 */
		bool invert();

		/**
		 * Inverts the matrix and returns the result as parameter.
		 * @param invertedMatrix The resulting inverted matrix
		 * @return True, if the matrix is not singular and could be inverted
		 * @see inverted(), solve().
		 */
		bool invert(SquareMatrixT3<T>& invertedMatrix) const;

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
		 * @see isIdentity().
		 */
		inline void toIdentity();

		/**
		 * Sets the matrix to a zero matrix.
		 * @see isNull();
		 */
		inline void toNull();

		/**
		 * Returns whether this matrix is an identity matrix.
		 * @return True, if so
		 * @see toIdentity().
		 */
		bool isIdentity() const;

		/**
		 * Returns whether this matrix is a zero matrix.
		 * @return True, if so
		 * @see toNull().
		 */
		bool isNull() const;

		/**
		 * Returns whether this matrix is singular (and thus cannot be inverted).
		 * A matrix is singular if the determinant of a matrix is 0.<br>
		 * @return True, if so
		 */
		inline bool isSingular() const;

		/**
		 * Returns true if this matrix is a similarity transformation.
		 * A similarity transformation has four degrees of freedom and contains a rotation, a scale, and a 2D translation and is not singular.<br>
		 * The 3x3 matrix representing the similarity transformation has the following layout:
		 * <pre>
		 * | a  -b  tx |
		 * | b   a  ty |
		 * | 0   0   1 |
		 * </pre>
		 * @return True, if this matrix is a similarity transformation, otherwise false
		 */
		inline bool isSimilarity() const;

		/**
		 * Returns true if this matrix is a affine transformation.
		 * In order to be considered affine, the matrix mustn't be singular and the last row must be equivalent to [0 0 1].
		 * @return True, if this matrix is an affine transformation, otherwise false
		 */
		inline bool isAffine() const;

		/**
		 * Returns true if this matrix is perspective transform/homography.
		 * In order to be considered a homography, the matrix mustn't be singular and the bottom-right matrix element must be nonzero.
		 * @return True, if this matrix is a homography, otherwise false
		 */
		inline bool isHomography() const;

		/**
		 * Returns whether this matrix is an orthonormal matrix.
		 * @param epsilon The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isOrthonormal(const T epsilon = NumericT<T>::eps()) const;

		/**
		 * Returns whether this matrix is symmetric.
		 * @param epsilon The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isSymmetric(const T epsilon = NumericT<T>::eps()) const;

		/**
		 * Returns whether two matrices are almost identical up to a specified epsilon.
		 * @param matrix Second matrix that will be checked
		 * @param eps The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isEqual(const SquareMatrixT3<T>& matrix, const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns the x axis which is the first column of the matrix.
		 * @return Vector with the first column
		 */
		VectorT3<T> xAxis() const;

		/**
		 * Returns the y axis which is the middle column of the matrix.
		 * @return Vector with the middle column
		 */
		VectorT3<T> yAxis() const;

		/**
		 * Returns the z axis which is the last column of the matrix.
		 * @return Vector with the last column
		 */
		VectorT3<T> zAxis() const;

		/**
		 * Returns the orthonormal matrix of this matrix by scaling the x-axis and adjusting y- and z-axis.
		 * This matrix must not be singular.
		 * @return An orthonormal version of this matrix
		 */
		SquareMatrixT3<T> orthonormalMatrix() const;

		/**
		 * Determines the eigen values of this matrix.
		 * @param eigenValues The three resulting eigen values, sorted: the highest first
		 * @return True, if succeeded
		 */
		bool eigenValues(T* eigenValues) const;

		/**
		 * Performs an eigen value analysis.
		 * @param eigenValues The three resulting eigen values, sorted: the highest first
		 * @param eigenVectors The three corresponding eigen vectors
		 * @return True, if succeeded
		 */
		bool eigenSystem(T* eigenValues,VectorT3<T>* eigenVectors) const;

		/**
		 * Returns a 3d vector with values of the matrix diagonal.
		 * @return Vector with diagonal values
		 */
		VectorT3<T> diagonal() const;

		/**
		 * Solve a simple 3x3 system of linear equations: Ax = b
		 * Beware: The system of linear equations is assumed to be fully determined.
		 * @param b The right-hand side vector
		 * @param x The resulting solution vector
		 * @return True, if the solution is valid, otherwise false
		 * @see invert(), inverted().
		 */
		inline bool solve(const VectorT3<T>& b, VectorT3<T>& x) const;

		/**
		 * Calculates the sum of absolute value of elements
		 * @return Sum of absolute value of elements
		 */
		inline T absSum() const;

		/**
		 * Calculates the sum of elements
		 * @return Sum of elements
		 */
		inline T sum() const;

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
		 * @param arrayValues Array with 9 floating point values of type U receiving the elements of this matrix
		 * @param valuesRowAligned True, if the target values are stored in a row aligned order; False, if the target values are stored in a column aligned order
		 * @tparam U Floating point type
		 */
		template <typename U>
		void copyElements(U* arrayValues, const bool valuesRowAligned = false) const;

		/**
		 * Copies the elements of this matrix to an array with floating point values of type T.
		 * @param arrayValues Array with 9 floating point values of type T receiving the elements of this matrix
		 * @param valuesRowAligned True, if the target values are stored in a row aligned order; False, if the target values are stored in a column aligned order
		 */
		void copyElements(T* arrayValues, const bool valuesRowAligned = false) const;

		/**
		 * Creates a skew symmetric 3x3 matrix for a specific vector.
		 * The skew symmetric matrix allows to calculate the cross product of the specified vector with a second vector by a matrix multiplication.<br>
		 * That means: skewSymmetricMatrix(vectorA) * vectorB == vectorA.cross(vectorB)<br>
		 * The final matrix has the following form for a vector (v0, v1, v2):
		 * <pre>
		 * |  0   -v2    v1 |
		 * | v2     0   -v0 |
		 * | -v1   v0     0 |
		 * </pre>
		 * @param vector The vector for which the skew symmetric matrix will be created
		 * @return Resulting matrix
		 */
		static inline SquareMatrixT3<T> skewSymmetricMatrix(const VectorT3<T>& vector);

		/**
		 * Multiplies a 2D vector with this matrix (from the right).
		 * The 2D vector is interpreted as a 3D vector with third component equal to 1.<br>
		 * This function is equivalent with the corresponding multiplication operator but returns False if the dot product between the augmented vector and the last row is zero.<br>
		 * The multiplication result will be de-homogenized to provide a 2D vector result, if possible.<br>
		 * Actually this function does:
		 * @param vector The vector to be multiplied/transformed
		 * @param result The de-homogenized resulting 2D vector, if this function succeeds
		 * @return True, if the dot product between the augmented vector and the last row is non-zero; False, otherwise
		 * @see operator*(const VectorT2<T>& vector).
		 */
		inline bool multiply(const VectorT2<T>& vector, VectorT2<T>& result) const;

		/**
		 * Multiplies this transposed matrix with a second matrix.
		 * Actually, the following matrix will be returned: (*this).transposed() * right.<br>
		 * @param right Matrix to multiply
		 * @return Matrix product
		 */
		SquareMatrixT3<T> transposedMultiply(const SquareMatrixT3<T>& right) const;

		/**
		 * Default copy assignment operator.
		 * @return Reference to this object
		 */
		SquareMatrixT3<T>& operator=(const SquareMatrixT3<T>&) = default;

		/**
		 * Returns whether two matrices are identical up to a small epsilon.
		 * @param matrix Right operand
		 * @return True, if so
		 */
		bool operator==(const SquareMatrixT3<T>& matrix) const;

		/**
		 * Returns whether two matrices are not identical up to a small epsilon.
		 * @param matrix Right operand
		 * @return True, if so
		 */
		inline bool operator!=(const SquareMatrixT3<T>& matrix) const;

		/**
		 * Adds two matrices.
		 * @param matrix Right operand
		 * @return Sum matrix
		 */
		SquareMatrixT3<T> operator+(const SquareMatrixT3<T>& matrix) const;

		/**
		 * Adds and assigns two matrices.
		 * @param matrix Right operand
		 * @return Reference to this object
		 */
		SquareMatrixT3<T>& operator+=(const SquareMatrixT3<T>& matrix);

		/**
		 * Subtracts two matrices.
		 * @param matrix Right operand
		 * @return Difference matrix
		 */
		SquareMatrixT3<T> operator-(const SquareMatrixT3<T>& matrix) const;

		/**
		 * Subtracts and assigns two matrices.
		 * @param matrix Right operand
		 * @return Reference to this object
		 */
		SquareMatrixT3<T>& operator-=(const SquareMatrixT3<T>& matrix);

		/**
		 * Returns the negative matrix of this matrix (all matrix elements are multiplied by -1).
		 * @return Resulting negative matrix
		 */
		inline SquareMatrixT3<T> operator-() const;

		/**
		 * Multiplies two matrices.
		 * @param matrix Right operand
		 * @return Product matrix
		 */
		OCEAN_FORCE_INLINE SquareMatrixT3<T> operator*(const SquareMatrixT3<T>& matrix) const;

		/**
		 * Multiplies and assigns two matrices.
		 * @param matrix Right operand
		 * @return Reference to this object
		 */
		OCEAN_FORCE_INLINE SquareMatrixT3<T>& operator*=(const SquareMatrixT3<T>& matrix);

		/**
		 * Multiply operator for a 2D vector.
		 * The 2D vector is interpreted as a 3D vector with third component equal to 1.<br>
		 * The final result will be de-homogenized to provide a 2D vector result.<br>
		 * Beware the dot product between the last row and the (augmented) vector must not be zero!
		 * @param vector Right operand, vector to be multiplied from the right
		 * @return Resulting 2D vector
		 * @see multiply().
		 */
		OCEAN_FORCE_INLINE VectorT2<T> operator*(const VectorT2<T>& vector) const;

		/**
		 * Multiply operator for a 3D vector.
		 * @param vector Right operand
		 * @return Resulting 3D vector
		 */
		OCEAN_FORCE_INLINE VectorT3<T> operator*(const VectorT3<T>& vector) const;

		/**
		 * Multiplies this matrix with a scalar value.
		 * @param value Right operand
		 * @return Resulting matrix
		 */
		OCEAN_FORCE_INLINE SquareMatrixT3<T> operator*(const T value) const;

		/**
		 * Multiplies and assigns this matrix with a scalar value.
		 * @param value right operand
		 * @return Reference to this object
		 */
		SquareMatrixT3<T>& operator*=(const T value);

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 8]
		 * @return Specified element
		 */
		inline T operator[](const unsigned int index) const;

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 8]
		 * @return Specified element
		 */
		inline T& operator[](const unsigned int index);

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param row The row of the element to return [0, 2]
		 * @param column The column of the element to return [0, 2]
		 * @return Specified element
		 */
		inline T operator()(const unsigned int row, const unsigned int column) const;

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param row The row of the element to return [0, 2]
		 * @param column The column of the element to return [0, 2]
		 * @return Specified element
		 */
		inline T& operator()(const unsigned int row, const unsigned int column);

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 8]
		 * @return Specified element
		 */
		inline T operator()(const unsigned int index) const;

		/**
		 * Element operator.
		 * Beware: No range check will be done!
		 * @param index The index of the element to return [0, 8]
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
		inline size_t operator()(const SquareMatrixT3<T>& matrix) const;

		/**
		 * Returns the number of elements this matrix has.
		 * @return The number of elements, always 9
		 */
		static inline size_t elements();

		/**
		 * Multiplies several 2D vectors with a given 3x3 matrix.
		 * Each 2D vector is interpreted as a 3D vector with third component equal to 1.<br>
		 * The final result will be de-homogenized to provide a 2D vector result.
		 * @param matrix The matrix to be used for multiplication
		 * @param vectors The input vectors that will be multiplied, may be nullptr if number is 0
		 * @param results The resulting output (multiplied/transformed) vectors, with same number as the provided input vectors
		 * @param number The number of provided vectors (input and output), with range [0, infinity)
		 */
		static void multiply(const SquareMatrixT3<T>& matrix, const VectorT2<T>* vectors, VectorT2<T>* results, const size_t number);

		/**
		 * Multiplies several 3D vectors with a given 3x3 matrix.
		 * @param matrix The matrix to be used for multiplication
		 * @param vectors The input vectors that will be multiplied, may be nullptr if number is 0
		 * @param results The resulting output (multiplied/transformed) vectors, with same number as the provided input vectors
		 * @param number The number of provided vectors (input and output), with range [0, infinity)
		 */
		static void multiply(const SquareMatrixT3<T>& matrix, const VectorT3<T>* vectors, VectorT3<T>* results, const size_t number);

		/**
		 * Converts matrices with specific data type to matrices with different data type.
		 * @param matrices The matrices to convert
		 * @return The converted matrices
		 * @tparam U The element data type of the matrices to convert
		 */
		template <typename U>
		static inline SquareMatricesT3<T> matrices2matrices(const SquareMatricesT3<U>& matrices);

		/**
		 * Converts matrices with specific data type to matrices with different data type.
		 * @param matrices The matrices to convert
		 * @param size The number of matrices to convert
		 * @return The converted matrices
		 * @tparam U The element data type of the matrices to convert
		 */
		template <typename U>
		static inline SquareMatricesT3<T> matrices2matrices(const SquareMatrixT3<U>* matrices, const size_t size);

	protected:

		/// The nine values of the matrix.
		T values_[9];
};

template <typename T>
inline SquareMatrixT3<T>::SquareMatrixT3()
{
	// nothing to do here
}

template <typename T>
template <typename U>
inline SquareMatrixT3<T>::SquareMatrixT3(const SquareMatrixT3<U>& matrix)
{
	values_[0] = T(matrix.values_[0]);
	values_[1] = T(matrix.values_[1]);
	values_[2] = T(matrix.values_[2]);
	values_[3] = T(matrix.values_[3]);
	values_[4] = T(matrix.values_[4]);
	values_[5] = T(matrix.values_[5]);
	values_[6] = T(matrix.values_[6]);
	values_[7] = T(matrix.values_[7]);
	values_[8] = T(matrix.values_[8]);
}

template <typename T>
SquareMatrixT3<T>::SquareMatrixT3(const bool setToIdentity)
{
	if (setToIdentity)
	{
		values_[0] = T(1.0);
		values_[1] = T(0.0);
		values_[2] = T(0.0);
		values_[3] = T(0.0);
		values_[4] = T(1.0);
		values_[5] = T(0.0);
		values_[6] = T(0.0);
		values_[7] = T(0.0);
		values_[8] = T(1.0);
	}
	else
	{
		values_[0] = T(0.0);
		values_[1] = T(0.0);
		values_[2] = T(0.0);
		values_[3] = T(0.0);
		values_[4] = T(0.0);
		values_[5] = T(0.0);
		values_[6] = T(0.0);
		values_[7] = T(0.0);
		values_[8] = T(0.0);
	}
}

template <typename T>
SquareMatrixT3<T>::SquareMatrixT3(const EulerT<T>& euler)
{
	/**
	 * Rotation matrix around x-axis R(x):
	 * [  1    0     0   ]
	 * [  0   cos  -sin  ]
	 * [  0   sin   cos  ]
	 */

	/**
	 * Rotation matrix around y-axis R(y):
	 * [   cos   0   sin  ]
	 * [    0    1    0   ]
	 * [  -sin   0   cos  ]
	 */

	/**
	 * Rotation matrix around z-axis R(z):
	 * [  cos   -sin   0  ]
	 * [  sin    cos   0  ]
	 * [   0      0    1  ]
	 */

	/**
	 * Combined rotation matrix for R(y)R(x)R(z)
	 * [  cy cz + sx sy sz     cz sx sy - cy sz      cx sy  ]
	 * [       cx sz                 cx cz            -sx   ]
	 * [  -cz sy + cy sx sz    cy cz sx + sy sz      cx cy  ]
	 */

	const T cx = NumericT<T>::cos(euler.pitch());
	const T sx = NumericT<T>::sin(euler.pitch());

	const T cy = NumericT<T>::cos(euler.yaw());
	const T sy = NumericT<T>::sin(euler.yaw());

	const T cz = NumericT<T>::cos(euler.roll());
	const T sz = NumericT<T>::sin(euler.roll());

	values_[0] = cy * cz + sx * sy * sz;
	values_[1] = cx * sz;
	values_[2] = -cz * sy + cy * sx * sz;
	values_[3] = cz * sx * sy - cy * sz;
	values_[4] = cx * cz;
	values_[5] = cy * cz * sx + sy * sz;
	values_[6] = cx * sy;
	values_[7] = -sx;
	values_[8] = cx * cy;

	ocean_assert(NumericT<T>::isEqual(determinant(), T(1.0)));
}

template <typename T>
SquareMatrixT3<T>::SquareMatrixT3(const RotationT<T>& rotation)
{
	// R(n, angle) = cos(angle) * I + (1 - cos(angle) * nn^T - sin(angle) * X(n)

	ocean_assert(rotation.isValid());

	const T cosValue = NumericT<T>::cos(rotation.angle());
	const T cosValue1 = T(1.0) - cosValue;
	const T sinValue = NumericT<T>::sin(rotation.angle());

	const VectorT3<T> axis(rotation.axis());

	const T xx = axis.x() * axis.x() * cosValue1;
	const T yy = axis.y() * axis.y() * cosValue1;
	const T zz = axis.z() * axis.z() * cosValue1;
	const T xy = axis.x() * axis.y() * cosValue1;
	const T xz = axis.x() * axis.z() * cosValue1;
	const T yz = axis.y() * axis.z() * cosValue1;

	const T nx = axis.x() * sinValue;
	const T ny = axis.y() * sinValue;
	const T nz = axis.z() * sinValue;

	values_[0] = xx + cosValue;
	values_[1] = xy + nz;
	values_[2] = xz - ny;

	values_[3] = xy - nz;
	values_[4] = yy + cosValue;
	values_[5] = yz + nx;

	values_[6] = xz + ny;
	values_[7] = yz - nx;
	values_[8] = zz + cosValue;

	ocean_assert(NumericT<T>::isEqual(determinant(), T(1.0)));
}

template <typename T>
SquareMatrixT3<T>::SquareMatrixT3(const QuaternionT<T>& quaternion)
{
	ocean_assert(quaternion.isValid());

	const T xx = quaternion.x() * quaternion.x();
	const T yy = quaternion.y() * quaternion.y();
	const T zz = quaternion.z() * quaternion.z();

	const T wx = quaternion.w() * quaternion.x();
	const T wy = quaternion.w() * quaternion.y();
	const T wz = quaternion.w() * quaternion.z();
	const T xy = quaternion.x() * quaternion.y();
	const T xz = quaternion.x() * quaternion.z();
	const T yz = quaternion.y() * quaternion.z();

	values_[0] = T(1.0) - T(2.0) * (yy + zz);
	values_[1] = T(2.0) * (wz + xy);
	values_[2] = T(2.0) * (xz - wy);

	values_[3] = T(2.0) * (xy - wz);
	values_[4] = T(1.0) - T(2.0) * (xx + zz);
	values_[5] = T(2.0) * (wx + yz);

	values_[6] = T(2.0) * (wy + xz);
	values_[7] = T(2.0) * (yz - wx);
	values_[8] = T(1.0) - T(2.0) * (xx + yy);

	ocean_assert(NumericT<T>::isWeakEqual(determinant(), T(1.0)) && "the quaternion is not normalized");
}

template <typename T>
SquareMatrixT3<T>::SquareMatrixT3(const T& m00, const T& m10, const T& m20, const T& m01, const T& m11, const T& m21, const T& m02, const T& m12, const T& m22)
{
	values_[0] = m00;
	values_[1] = m10;
	values_[2] = m20;

	values_[3] = m01;
	values_[4] = m11;
	values_[5] = m21;

	values_[6] = m02;
	values_[7] = m12;
	values_[8] = m22;
}

template <typename T>
SquareMatrixT3<T>::SquareMatrixT3(const VectorT3<T>& xAxis, const VectorT3<T>& yAxis, const VectorT3<T>& zAxis)
{
	memcpy(values_, xAxis(), sizeof(T) * 3);
	memcpy(values_ + 3, yAxis(), sizeof(T) * 3);
	memcpy(values_ + 6, zAxis(), sizeof(T) * 3);
}

template <typename T>
SquareMatrixT3<T>::SquareMatrixT3(const VectorT3<T>& diagonal)
{
	values_[0] = diagonal[0];
	values_[1] = T(0.0);
	values_[2] = T(0.0);
	values_[3] = T(0.0);
	values_[4] = diagonal[1];
	values_[5] = T(0.0);
	values_[6] = T(0.0);
	values_[7] = T(0.0);
	values_[8] = diagonal[2];
}

template <typename T>
template <typename U>
SquareMatrixT3<T>::SquareMatrixT3(const U* arrayValues)
{
	ocean_assert(arrayValues);

	for (unsigned int n = 0u; n < 9u; ++n)
	{
		values_[n] = T(arrayValues[n]);
	}
}

template <typename T>
SquareMatrixT3<T>::SquareMatrixT3(const T* arrayValues)
{
	ocean_assert(arrayValues);
	memcpy(values_, arrayValues, sizeof(T) * 9);
}

template <typename T>
template <typename U>
SquareMatrixT3<T>::SquareMatrixT3(const U* arrayValues, const bool valuesRowAligned)
{
	ocean_assert(arrayValues);

	if (valuesRowAligned)
	{
		values_[0] = T(arrayValues[0]);
		values_[1] = T(arrayValues[3]);
		values_[2] = T(arrayValues[6]);
		values_[3] = T(arrayValues[1]);
		values_[4] = T(arrayValues[4]);
		values_[5] = T(arrayValues[7]);
		values_[6] = T(arrayValues[2]);
		values_[7] = T(arrayValues[5]);
		values_[8] = T(arrayValues[8]);

	}
	else
	{
		for (unsigned int n = 0u; n < 9u; ++n)
		{
			values_[n] = T(arrayValues[n]);
		}
	}
}

template <typename T>
SquareMatrixT3<T>::SquareMatrixT3(const T* arrayValues, const bool valuesRowAligned)
{
	ocean_assert(arrayValues);

	if (valuesRowAligned)
	{
		values_[0] = arrayValues[0];
		values_[1] = arrayValues[3];
		values_[2] = arrayValues[6];
		values_[3] = arrayValues[1];
		values_[4] = arrayValues[4];
		values_[5] = arrayValues[7];
		values_[6] = arrayValues[2];
		values_[7] = arrayValues[5];
		values_[8] = arrayValues[8];
	}
	else
	{
		memcpy(values_, arrayValues, sizeof(T) * 9);
	}
}

template <typename T>
SquareMatrixT3<T>::SquareMatrixT3(const HomogenousMatrixT4<T>& transformation)
{
	memcpy(values_, transformation(), sizeof(T) * 3);
	memcpy(values_ + 3, transformation() + 4, sizeof(T) * 3);
	memcpy(values_ + 6, transformation() + 8, sizeof(T) * 3);
}

template <typename T>
SquareMatrixT3<T>::SquareMatrixT3(const SquareMatrixT4<T>& transformation)
{
	memcpy(values_, transformation(), sizeof(T) * 3);
	memcpy(values_ + 3, transformation() + 4, sizeof(T) * 3);
	memcpy(values_ + 6, transformation() + 8, sizeof(T) * 3);
}

template<typename T>
inline bool SquareMatrixT3<T>::solve(const VectorT3<T>& b, VectorT3<T>& x) const
{
	// Solve the system of linear equations, Ax=b, using Cramer's rule
	//
	//     [a0 a3 a6]      [b0]
	// A = [a1 a4 a7], b = [b1]
	//     [a2 a5 a8]      [b2]
	//
	// d = det(A)
	//
	//           [b0 a3 a6]               [a0 b0 a6]              [a0 a3 b0]
	// d0 = det( [b1 a4 a7] ),  d1 = det( [a1 b1 a7] ), d2 = det( [a1 a4 b1] )
	//           [b2 a5 a8]               [a2 b2 a8]              [a2 a5 b2]
	//
	//     [d0 / d]
	// x = [d1 / d]
	//     [d2 / d]
	const T d = determinant();

	if (NumericT<T>::isNotEqualEps(d))
	{
		const T d0 =      b[0] * (values_[4] * values_[8] - values_[5] * values_[7]) +      b[1] * (values_[5] * values_[6] - values_[3] * values_[8]) +      b[2] * (values_[3] * values_[7] - values_[4] * values_[6]);
		const T d1 = values_[0] * (     b[1] * values_[8] -      b[2] * values_[7]) + values_[1] * (     b[2] * values_[6] -      b[0] * values_[8]) + values_[2] * (     b[0] * values_[7] -      b[1] * values_[6]);
		const T d2 = values_[0] * (values_[4] *      b[2] - values_[5] *      b[1]) + values_[1] * (values_[5] *      b[0] - values_[3] *      b[2]) + values_[2] * (values_[3] *      b[1] - values_[4] *      b[0]);

		const T invD = T(1) / d;

		x[0] = d0 * invD;
		x[1] = d1 * invD;
		x[2] = d2 * invD;

		return true;
	}

	return false;
}

template<typename T>
inline T SquareMatrixT3<T>::absSum() const
{
	return (NumericT<T>::abs(values_[0]) + NumericT<T>::abs(values_[1]) + NumericT<T>::abs(values_[2]) + NumericT<T>::abs(values_[3]) + NumericT<T>::abs(values_[4]) + NumericT<T>::abs(values_[5]) + NumericT<T>::abs(values_[6]) + NumericT<T>::abs(values_[7]) + NumericT<T>::abs(values_[8]));
}

template<typename T>
inline T SquareMatrixT3<T>::sum() const
{
	return (values_[0] + values_[1] + values_[2] + values_[3] + values_[4] + values_[5] + values_[6] + values_[7] + values_[8]);
}

template <typename T>
inline const T* SquareMatrixT3<T>::data() const
{
	return values_;
}

template <typename T>
inline T* SquareMatrixT3<T>::data()
{
	return values_;
}

template <typename T>
inline bool SquareMatrixT3<T>::operator!=(const SquareMatrixT3<T>& matrix) const
{
	return !(*this == matrix);
}

template <typename T>
inline SquareMatrixT3<T>& SquareMatrixT3<T>::operator*=(const SquareMatrixT3<T>& matrix)
{
	*this = *this * matrix;
	return *this;
}

template <typename T>
inline T SquareMatrixT3<T>::operator[](const unsigned int index) const
{
	ocean_assert(index < 9u);
	return values_[index];
}

template <typename T>
inline T& SquareMatrixT3<T>::operator[](const unsigned int index)
{
	ocean_assert(index < 9u);
	return values_[index];
}

template <typename T>
inline T SquareMatrixT3<T>::operator()(const unsigned int row, const unsigned int column) const
{
	ocean_assert(row < 3u && column < 3u);
	return values_[column * 3 + row];
}

template <typename T>
inline T& SquareMatrixT3<T>::operator()(const unsigned int row, const unsigned int column)
{
	ocean_assert(row < 3u && column < 3u);
	return values_[column * 3 + row];
}

template <typename T>
inline T SquareMatrixT3<T>::operator()(const unsigned int index) const
{
	ocean_assert(index < 9u);
	return values_[index];
}

template <typename T>
inline T& SquareMatrixT3<T>::operator()(const unsigned int index)
{
	ocean_assert(index < 9u);
	return values_[index];
}

template <typename T>
inline const T* SquareMatrixT3<T>::operator()() const
{
	return values_;
}

template <typename T>
inline T* SquareMatrixT3<T>::operator()()
{
	return values_;
}

template <typename T>
inline size_t SquareMatrixT3<T>::operator()(const SquareMatrixT3<T>& matrix) const
{
	size_t seed = std::hash<T>{}(matrix.values_[0]);

	for (unsigned int n = 1u; n < 9u; ++n)
	{
		seed ^= std::hash<T>{}(matrix.values_[n]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	return seed;
}

template <typename T>
inline size_t SquareMatrixT3<T>::elements()
{
	return 9;
}

template <typename T>
SquareMatrixT3<T> SquareMatrixT3<T>::transposed() const
{
	SquareMatrixT3<T> result(*this);

	result.values_[1] = values_[3];
	result.values_[3] = values_[1];

	result.values_[2] = values_[6];
	result.values_[6] = values_[2];

	result.values_[5] = values_[7];
	result.values_[7] = values_[5];

	return result;
}

template <typename T>
void SquareMatrixT3<T>::transpose()
{
	SquareMatrixT3<T> tmp(*this);

	values_[3] = tmp.values_[1];
	values_[1] = tmp.values_[3];

	values_[6] = tmp.values_[2];
	values_[2] = tmp.values_[6];

	values_[7] = tmp.values_[5];
	values_[5] = tmp.values_[7];
}

template <typename T>
SquareMatrixT3<T> SquareMatrixT3<T>::inverted() const
{
	SquareMatrixT3<T> invertedMatrix;

	if (!invert(invertedMatrix))
	{
		ocean_assert(false && "Could not invert the matrix.");
		return *this;
	}

	return invertedMatrix;
}

template <typename T>
bool SquareMatrixT3<T>::invert()
{
	SquareMatrixT3<T> invertedMatrix;

	if (!invert(invertedMatrix))
	{
		return false;
	}

	*this = invertedMatrix;

	return true;
}

template <typename T>
bool SquareMatrixT3<T>::invert(SquareMatrixT3<T>& invertedMatrix) const
{
	// calculate determinant
	const T v48 = values_[4] * values_[8];
	const T v57 = values_[5] * values_[7];
	const T v56 = values_[5] * values_[6];
	const T v38 = values_[3] * values_[8];
	const T v37 = values_[3] * values_[7];
	const T v46 = values_[4] * values_[6];

	const T v48_57 = v48 - v57;
	const T v56_38 = v56 - v38;
	const T v37_46 = v37 - v46;

	const T det = values_[0] * v48_57 + values_[1] * v56_38 + values_[2] * v37_46;

	if (NumericT<T>::isEqualEps(det))
	{
		return false;
	}

	const T factor = T(1.0) / det;

	invertedMatrix.values_[0] = (v48_57) * factor;
	invertedMatrix.values_[1] = (values_[2] * values_[7] - values_[1] * values_[8]) * factor;
	invertedMatrix.values_[2] = (values_[1] * values_[5] - values_[2] * values_[4]) * factor;

	invertedMatrix.values_[3] = (v56_38) * factor;
	invertedMatrix.values_[4] = (values_[0] * values_[8] - values_[2] * values_[6]) * factor;
	invertedMatrix.values_[5] = (values_[2] * values_[3] - values_[0] * values_[5]) * factor;

	invertedMatrix.values_[6] = (v37_46) * factor;
	invertedMatrix.values_[7] = (values_[1] * values_[6] - values_[0] * values_[7]) * factor;
	invertedMatrix.values_[8] = (values_[0] * values_[4] - values_[1] * values_[3]) * factor;

#ifdef OCEAN_INTENSIVE_DEBUG
	if (!std::is_same<T, float>::value)
	{
		const SquareMatrixT3<T> test(*this * invertedMatrix);
		const SquareMatrixT3<T> entity(true);

		T sqrDistance = T(0);
		for (unsigned int n = 0; n < 9u; ++n)
		{
			sqrDistance += NumericT<T>::sqr(test[n] - entity[n]);
		}

		const T distance = NumericT<T>::sqrt(sqrDistance * T(0.111111111111111111)); // 1 / 9

		if (NumericT<T>::isWeakEqualEps(distance) == false)
		{
			T absolusteAverageEnergy = 0;
			for (unsigned int n = 0u; n < 9u; ++n)
			{
				absolusteAverageEnergy += NumericT<T>::abs(values[n]);
			}

			absolusteAverageEnergy *= T(0.111111111111111111); // 1 / 9

			// we expect/accept for each magnitude (larger than 1) a zero-inaccuracy of one magnitude (and we again comare it with the weak eps)

			if (absolusteAverageEnergy <= 1)
			{
				ocean_assert_accuracy(!"This should never happen!");
			}
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
T SquareMatrixT3<T>::determinant() const
{
	return values_[0] * (values_[4] * values_[8] - values_[5] * values_[7])
			+ values_[1] * (values_[5] * values_[6] - values_[3] * values_[8])
			+ values_[2] * (values_[3] * values_[7] - values_[4] * values_[6]);
}

template <typename T>
T SquareMatrixT3<T>::trace() const
{
	return values_[0] + values_[4] + values_[8];
}

template <typename T>
inline void SquareMatrixT3<T>::toIdentity()
{
	values_[0] = T(1.0);
	values_[1] = T(0.0);
	values_[2] = T(0.0);
	values_[3] = T(0.0);
	values_[4] = T(1.0);
	values_[5] = T(0.0);
	values_[6] = T(0.0);
	values_[7] = T(0.0);
	values_[8] = T(1.0);
}

template <typename T>
inline void SquareMatrixT3<T>::toNull()
{
	values_[0] = T(0.0);
	values_[1] = T(0.0);
	values_[2] = T(0.0);
	values_[3] = T(0.0);
	values_[4] = T(0.0);
	values_[5] = T(0.0);
	values_[6] = T(0.0);
	values_[7] = T(0.0);
	values_[8] = T(0.0);
}

template <typename T>
bool SquareMatrixT3<T>::isIdentity() const
{
	return NumericT<T>::isEqual(values_[0], 1) && NumericT<T>::isEqualEps(values_[1]) && NumericT<T>::isEqualEps(values_[2])
				&& NumericT<T>::isEqualEps(values_[3]) && NumericT<T>::isEqual(values_[4], 1) && NumericT<T>::isEqualEps(values_[5])
				&& NumericT<T>::isEqualEps(values_[6]) && NumericT<T>::isEqualEps(values_[7]) && NumericT<T>::isEqual(values_[8], 1);
}

template <typename T>
bool SquareMatrixT3<T>::isNull() const
{
	return NumericT<T>::isEqualEps(values_[0]) && NumericT<T>::isEqualEps(values_[1]) && NumericT<T>::isEqualEps(values_[2])
				&& NumericT<T>::isEqualEps(values_[3]) && NumericT<T>::isEqualEps(values_[4]) && NumericT<T>::isEqualEps(values_[5])
				&& NumericT<T>::isEqualEps(values_[6]) && NumericT<T>::isEqualEps(values_[7]) && NumericT<T>::isEqualEps(values_[8]);
}

template <typename T>
inline bool SquareMatrixT3<T>::isSingular() const
{
	return NumericT<T>::isEqualEps(determinant());
}

template <typename T>
inline bool SquareMatrixT3<T>::isSimilarity() const
{
	return NumericT<T>::isEqual(values_[0], values_[4]) && NumericT<T>::isEqual(values_[1], -values_[3]) && NumericT<T>::isEqualEps(values_[2]) && NumericT<T>::isEqualEps(values_[5]) && NumericT<T>::isEqual(values_[8], 1) && !isSingular();
}

template <typename T>
inline bool SquareMatrixT3<T>::isAffine() const
{
	return NumericT<T>::isEqualEps(values_[2]) && NumericT<T>::isEqualEps(values_[5]) && NumericT<T>::isEqual(values_[8], 1) && !isSingular();
}

template <typename T>
inline bool SquareMatrixT3<T>::isHomography() const
{
	return NumericT<T>::isNotEqualEps(values_[8]) && !isSingular();
}

template <typename T>
bool SquareMatrixT3<T>::isOrthonormal(const T epsilon) const
{
	ocean_assert(epsilon >= 0);

	const VectorT3<T> xAxis(values_);
	const VectorT3<T> yAxis(values_ + 3);
	const VectorT3<T> zAxis(values_ + 6);

	return NumericT<T>::isEqual(xAxis * yAxis, 0, epsilon) && NumericT<T>::isEqual(xAxis * zAxis, 0, epsilon) && NumericT<T>::isEqual(yAxis * zAxis, 0, epsilon)
			&& NumericT<T>::isEqual(xAxis.length(), 1, epsilon) && NumericT<T>::isEqual(yAxis.length(), 1, epsilon) && NumericT<T>::isEqual(zAxis.length(), 1, epsilon);
}

template <typename T>
bool SquareMatrixT3<T>::isSymmetric(const T epsilon) const
{
	ocean_assert(epsilon >= T(0));

	return NumericT<T>::isEqual(values_[1], values_[3], epsilon) && NumericT<T>::isEqual(values_[2], values_[6], epsilon) && NumericT<T>::isEqual(values_[5], values_[7], epsilon);
}

template <typename T>
inline bool SquareMatrixT3<T>::isEqual(const SquareMatrixT3<T>& matrix, const T eps) const
{
	return NumericT<T>::isEqual(values_[0], matrix.values_[0], eps) && NumericT<T>::isEqual(values_[1], matrix.values_[1], eps)
			&& NumericT<T>::isEqual(values_[2], matrix.values_[2], eps) && NumericT<T>::isEqual(values_[3], matrix.values_[3], eps)
			&& NumericT<T>::isEqual(values_[4], matrix.values_[4], eps) && NumericT<T>::isEqual(values_[5], matrix.values_[5], eps)
			&& NumericT<T>::isEqual(values_[6], matrix.values_[6], eps) && NumericT<T>::isEqual(values_[7], matrix.values_[7], eps)
			&& NumericT<T>::isEqual(values_[8], matrix.values_[8], eps);
}

template <typename T>
VectorT3<T> SquareMatrixT3<T>::xAxis() const
{
	return VectorT3<T>(values_);
}

template <typename T>
VectorT3<T> SquareMatrixT3<T>::yAxis() const
{
	return VectorT3<T>(values_ + 3);
}

template <typename T>
VectorT3<T> SquareMatrixT3<T>::zAxis() const
{
	return VectorT3<T>(values_ + 6);
}

template <typename T>
SquareMatrixT3<T> SquareMatrixT3<T>::orthonormalMatrix() const
{
	ocean_assert(!isSingular());

	VectorT3<T> xAxis(values_);
	VectorT3<T> yAxis(values_ + 3);
	VectorT3<T> zAxis(values_ + 6);

	// X scale factor
	const T xScale = xAxis.length();
	// normalize x axis
	xAxis /= xScale;

	// xy shear factor
	const T xyShear = xAxis * yAxis;

	// compute orthogonal y axis
	yAxis -= xAxis * xyShear;

	// y scale factor
	const T yScale = yAxis.length();
	// normalize y axis
	yAxis /= yScale;

	// xz shear
	const T xzShear = xAxis * zAxis;
	// compute orthogonal z axis
	zAxis -= xAxis * xzShear;

	// yz shear
	const T yzShear = yAxis * zAxis;
	// compute orthogonal y axis
	zAxis -= yAxis * yzShear;

	// z scale factor
	const T zScale = zAxis.length();
	// normalize z axis
	zAxis /= zScale;

#ifdef OCEAN_INTENSIVE_DEBUG
	if (std::is_same<T, double>::value)
	{
		ocean_assert(NumericT<T>::isEqualEps(xAxis * yAxis));
		ocean_assert(NumericT<T>::isEqualEps(xAxis * zAxis));
		ocean_assert(NumericT<T>::isEqualEps(yAxis * zAxis));
		ocean_assert(SquareMatrixT3<T>(xAxis, yAxis, zAxis).isOrthonormal());
	}
#endif

	return SquareMatrixT3<T>(xAxis, yAxis, zAxis);
}

template <typename T>
bool SquareMatrixT3<T>::eigenValues(T* eigenValues) const
{
	ocean_assert(eigenValues);

	/**
	 * Computation of the characteristic polynomial
	 * <pre>
	 *
	 *     [ a b c ]
	 * A = [ d e f ]
	 *     [ g h i ]
	 *
	 *             [ a-x   b    c  ]
	 * A - x * E = [  d   e-x   f  ]
	 *             [  g    h   i-x ]
	 *
	 * Polynomial = Det|A - x * E| = 0
	 *            = (a - x) * (e - x)          * (i - x) + bfg + cdh - g * (e - x) * c  - h * f * (a - x)  - (i - x) * d * b
	 *            = (ae - a * x - e * x + x^2) * (i - x) + bfg + cdh - g * (ec - c * x) - h * (fa - f * x) - d * (ib - x * b)
	 *            = aei - ae * x - ai * x + a * x^2 - ei * x + e * x^2 + i * x^2 - x^3 + bfg + cdh - gec + gc * x - hfa + hf * x - dib + db * x
	 *            = -x^3 + (a + e + i) * x^2 + (-ae - ai - ei + gc + hf + db) * x + aei + bfg + cdh - gec - hfa - dib
	 *            = x^3 - (a + e + i) * x^2 - (-ae - ai - ei + gc + hf + db) * x - aei - bfg - cdh + gec + hfa + dib
	 *            = a1x^3 + a2x^2 + a3x + a4 = 0
	 * </pre>
	 */

	const T a = values_[0];
	const T d = values_[1];
	const T g = values_[2];

	const T b = values_[3];
	const T e = values_[4];
	const T h = values_[5];

	const T c = values_[6];
	const T f = values_[7];
	const T i = values_[8];

	const T a1 = T(1.0);
	const T a2 = -(a + e + i);
	const T a3 = -(-a * e - a * i - e * i + g * c + h * f + d * b);
	const T a4 = -a * e * i - b * f * g - c * d * h + g * e * c + h * f * a + d * i * b;

	if (EquationT<T>::solveCubic(a1, a2, a3, a4, eigenValues[0], eigenValues[1], eigenValues[2]) != 3u)
	{
		return false;
	}

	Utilities::sortHighestToFront3(eigenValues[0], eigenValues[1], eigenValues[2]);
	return true;
}

template <typename T>
bool SquareMatrixT3<T>::eigenSystem(T* eigenValues, VectorT3<T>* eigenVectors) const
{
	ocean_assert(eigenValues != nullptr && eigenVectors != nullptr);

	/**
	 * Computation of the characteristic polynomial
	 * <pre>
	 *     [ a b c ]
	 * A = [ d e f ]
	 *     [ g h i ]
	 *
	 *             [ a-x   b    c  ]
	 * A - x * E = [  d   e-x   f  ]
	 *             [  g    h   i-x ]
	 *
	 * Polynomial = Det|A - x * E| = 0
	 *            = (a - x) * (e - x)          * (i - x) + bfg + cdh - g * (e - x) * c  - h * f * (a - x)  - (i - x) * d * b
	 *            = (ae - a * x - e * x + x^2) * (i - x) + bfg + cdh - g * (ec - c * x) - h * (fa - f * x) - d * (ib - x * b)
	 *            = aei - ae * x - ai * x + a * x^2 - ei * x + e * x^2 + i * x^2 - x^3 + bfg + cdh - gec + gc * x - hfa + hf * x - dib + db * x
	 *            = -x^3 + (a + e + i) * x^2 + (-ae - ai - ei + gc + hf + db) * x + aei + bfg + cdh - gec - hfa - dib
	 *            = x^3 - (a + e + i) * x^2 - (-ae - ai - ei + gc + hf + db) * x - aei - bfg - cdh + gec + hfa + dib
	 *            = a1x^3 + a2x^2 + a3x + a4 = 0
	 * </pre>
	 */

	const T a = values_[0];
	const T d = values_[1];
	const T g = values_[2];

	const T b = values_[3];
	const T e = values_[4];
	const T h = values_[5];

	const T c = values_[6];
	const T f = values_[7];
	const T i = values_[8];

	const T a1 = T(1.0);
	const T a2 = -(a + e + i);
	const T a3 = -(-a * e - a * i - e * i + g * c + h * f + d * b);
	const T a4 = -a * e * i - b * f * g - c * d * h + g * e * c + h * f * a + d * i * b;

	if (EquationT<T>::solveCubic(a1, a2, a3, a4, eigenValues[0], eigenValues[1], eigenValues[2]) != 3u)
	{
		return false;
	}

	/**
	 * <pre>
	 * Determination of the eigen vectors (vx, vy, vz):
	 *             [ a-x   b    c  ]   [ vx ]
	 * A - x * E = [  d   e-x   f  ] * [ vy ] = 0
	 *             [  g    h   i-x ]   [ vz ]
	 * We can apply the cross product to find a vector that is perpendicular to the two top rows of the matrix A - x * E
	 * </pre>
	 */

	Utilities::sortHighestToFront3(eigenValues[0], eigenValues[1], eigenValues[2]);

	for (unsigned int n = 0u; n < 3u; ++n)
	{
		const VectorT3<T> row0(a - eigenValues[n], b, c);
		const VectorT3<T> row1(d, e - eigenValues[n], f);
		const VectorT3<T> row2(g, h, i - eigenValues[n]);

		VectorT3<T> candidate0(row0.cross(row1));
		VectorT3<T> candidate1(row0.cross(row2));
		VectorT3<T> candidate2(row1.cross(row2));

		T sqrCandidate0(candidate0.sqr());
		T sqrCandidate1(candidate1.sqr());
		T sqrCandidate2(candidate2.sqr());

		Utilities::sortHighestToFront3(sqrCandidate0, sqrCandidate1, sqrCandidate2, candidate0, candidate1, candidate2);

		// if all rows (row0, row1 and row2) are parallel, we can decide any vector that is perpendicular to these rows
		if (sqrCandidate0 < NumericT<T>::eps() * NumericT<T>::eps())
		{
			// find one row that is not a null row

			candidate0 = row0;
			candidate1 = row1;
			candidate2 = row2;

			sqrCandidate0 = candidate0.sqr();
			sqrCandidate1 = candidate1.sqr();
			sqrCandidate2 = candidate2.sqr();

			Utilities::sortHighestToFront3(sqrCandidate0, sqrCandidate1, sqrCandidate2, candidate0, candidate1, candidate2);

			ocean_assert(NumericT<T>::isNotEqualEps(candidate0.length()));
			eigenVectors[n] = candidate0.perpendicular();
		}
		else
		{
			eigenVectors[n] = candidate0;
		}

		eigenVectors[n].normalize();
	}

	return true;
}

template <typename T>
VectorT3<T> SquareMatrixT3<T>::diagonal() const
{
	return VectorT3<T>(values_[0], values_[4], values_[8]);
}

template <typename T>
template <typename U>
void SquareMatrixT3<T>::copyElements(U* arrayValues, const bool valuesRowAligned) const
{
	ocean_assert(arrayValues != nullptr);

	if (valuesRowAligned)
	{
		// this matrix and the provided array are both column aligned
		// thus, we can simply copy the data

		arrayValues[0] = U(values_[0]);
		arrayValues[1] = U(values_[3]);
		arrayValues[2] = U(values_[6]);

		arrayValues[3] = U(values_[1]);
		arrayValues[4] = U(values_[4]);
		arrayValues[5] = U(values_[7]);

		arrayValues[6] = U(values_[2]);
		arrayValues[7] = U(values_[5]);
		arrayValues[8] = U(values_[8]);
	}
	else
	{
		// this matrix and the provided array are both column aligned
		// thus, we can simply copy the data

		arrayValues[0] = U(values_[0]);
		arrayValues[1] = U(values_[1]);
		arrayValues[2] = U(values_[2]);
		arrayValues[3] = U(values_[3]);
		arrayValues[4] = U(values_[4]);
		arrayValues[5] = U(values_[5]);
		arrayValues[6] = U(values_[6]);
		arrayValues[7] = U(values_[7]);
		arrayValues[8] = U(values_[8]);
	}
}

template <typename T>
void SquareMatrixT3<T>::copyElements(T* arrayValues, const bool valuesRowAligned) const
{
	ocean_assert(arrayValues != nullptr);

	if (valuesRowAligned)
	{
		// this matrix and the provided array are both column aligned
		// thus, we can simply copy the data

		arrayValues[0] = values_[0];
		arrayValues[1] = values_[3];
		arrayValues[2] = values_[6];

		arrayValues[3] = values_[1];
		arrayValues[4] = values_[4];
		arrayValues[5] = values_[7];

		arrayValues[6] = values_[2];
		arrayValues[7] = values_[5];
		arrayValues[8] = values_[8];
	}
	else
	{
		// this matrix and the provided array are both column aligned
		// thus, we can simply copy the data

		memcpy(arrayValues, values_, sizeof(T) * 9);
	}
}

template <typename T>
inline SquareMatrixT3<T> SquareMatrixT3<T>::skewSymmetricMatrix(const VectorT3<T>& vector)
{
	return SquareMatrixT3<T>(0, vector(2), -vector(1), -vector(2), 0, vector(0), vector(1), -vector(0), 0);
}

template <typename T>
inline bool SquareMatrixT3<T>::multiply(const VectorT2<T>& vector, VectorT2<T>& result) const
{
	/**
	 * | x' |   | 0 3 6 |   | x |
	 * | y' | = | 1 4 7 | * | y |
	 * | 1  |   | 2 5 8 |   | 1 |
	 */

	const T z = values_[2] * vector[0] + values_[5] * vector[1] + values_[8];

	if (NumericT<T>::isNotEqualEps(z))
	{
		const T factor = T(1) / z;

		result = VectorT2<T>((values_[0] * vector[0] + values_[3] * vector[1] + values_[6]) * factor,
								(values_[1] * vector[0] + values_[4] * vector[1] + values_[7]) * factor);

		return true;
	}

	return false;
}

template <typename T>
SquareMatrixT3<T> SquareMatrixT3<T>::transposedMultiply(const SquareMatrixT3<T>& matrix) const
{
	SquareMatrixT3<T> result;

	result.values_[0] = values_[0] * matrix.values_[0] + values_[1] * matrix.values_[1] + values_[2] * matrix.values_[2];
	result.values_[1] = values_[3] * matrix.values_[0] + values_[4] * matrix.values_[1] + values_[5] * matrix.values_[2];
	result.values_[2] = values_[6] * matrix.values_[0] + values_[7] * matrix.values_[1] + values_[8] * matrix.values_[2];
	result.values_[3] = values_[0] * matrix.values_[3] + values_[1] * matrix.values_[4] + values_[2] * matrix.values_[5];
	result.values_[4] = values_[3] * matrix.values_[3] + values_[4] * matrix.values_[4] + values_[5] * matrix.values_[5];
	result.values_[5] = values_[6] * matrix.values_[3] + values_[7] * matrix.values_[4] + values_[8] * matrix.values_[5];
	result.values_[6] = values_[0] * matrix.values_[6] + values_[1] * matrix.values_[7] + values_[2] * matrix.values_[8];
	result.values_[7] = values_[3] * matrix.values_[6] + values_[4] * matrix.values_[7] + values_[5] * matrix.values_[8];
	result.values_[8] = values_[6] * matrix.values_[6] + values_[7] * matrix.values_[7] + values_[8] * matrix.values_[8];

#ifdef OCEAN_INTENSIVE_DEBUG
	const SquareMatrixT3<T> debugMatrix(transposed() * matrix);
	ocean_assert(debugMatrix == result);
#endif

	return result;
}

template <typename T>
bool SquareMatrixT3<T>::operator==(const SquareMatrixT3<T>& matrix) const
{
	return isEqual(matrix);
}

template <typename T>
SquareMatrixT3<T> SquareMatrixT3<T>::operator+(const SquareMatrixT3<T>& matrix) const
{
	SquareMatrixT3<T> result(*this);

	result.values_[0] += matrix.values_[0];
	result.values_[1] += matrix.values_[1];
	result.values_[2] += matrix.values_[2];
	result.values_[3] += matrix.values_[3];
	result.values_[4] += matrix.values_[4];
	result.values_[5] += matrix.values_[5];
	result.values_[6] += matrix.values_[6];
	result.values_[7] += matrix.values_[7];
	result.values_[8] += matrix.values_[8];

	return result;
}

template <typename T>
SquareMatrixT3<T>& SquareMatrixT3<T>::operator+=(const SquareMatrixT3<T>& matrix)
{
	values_[0] += matrix.values_[0];
	values_[1] += matrix.values_[1];
	values_[2] += matrix.values_[2];
	values_[3] += matrix.values_[3];
	values_[4] += matrix.values_[4];
	values_[5] += matrix.values_[5];
	values_[6] += matrix.values_[6];
	values_[7] += matrix.values_[7];
	values_[8] += matrix.values_[8];

	return *this;
}

template <typename T>
SquareMatrixT3<T> SquareMatrixT3<T>::operator-(const SquareMatrixT3<T>& matrix) const
{
	SquareMatrixT3<T> result(*this);

	result.values_[0] -= matrix.values_[0];
	result.values_[1] -= matrix.values_[1];
	result.values_[2] -= matrix.values_[2];
	result.values_[3] -= matrix.values_[3];
	result.values_[4] -= matrix.values_[4];
	result.values_[5] -= matrix.values_[5];
	result.values_[6] -= matrix.values_[6];
	result.values_[7] -= matrix.values_[7];
	result.values_[8] -= matrix.values_[8];

	return result;
}

template <typename T>
SquareMatrixT3<T>& SquareMatrixT3<T>::operator-=(const SquareMatrixT3<T>& matrix)
{
	values_[0] -= matrix.values_[0];
	values_[1] -= matrix.values_[1];
	values_[2] -= matrix.values_[2];
	values_[3] -= matrix.values_[3];
	values_[4] -= matrix.values_[4];
	values_[5] -= matrix.values_[5];
	values_[6] -= matrix.values_[6];
	values_[7] -= matrix.values_[7];
	values_[8] -= matrix.values_[8];

	return *this;
}

template <typename T>
inline SquareMatrixT3<T> SquareMatrixT3<T>::operator-() const
{
	SquareMatrixT3<T> result;

	result.values_[0] = -values_[0];
	result.values_[1] = -values_[1];
	result.values_[2] = -values_[2];
	result.values_[3] = -values_[3];
	result.values_[4] = -values_[4];
	result.values_[5] = -values_[5];
	result.values_[6] = -values_[6];
	result.values_[7] = -values_[7];
	result.values_[8] = -values_[8];

	return result;
}

template <typename T>
OCEAN_FORCE_INLINE SquareMatrixT3<T> SquareMatrixT3<T>::operator*(const SquareMatrixT3<T>& matrix) const
{
	SquareMatrixT3<T> result;

	result.values_[0] = values_[0] * matrix.values_[0] + values_[3] * matrix.values_[1] + values_[6] * matrix.values_[2];
	result.values_[1] = values_[1] * matrix.values_[0] + values_[4] * matrix.values_[1] + values_[7] * matrix.values_[2];
	result.values_[2] = values_[2] * matrix.values_[0] + values_[5] * matrix.values_[1] + values_[8] * matrix.values_[2];
	result.values_[3] = values_[0] * matrix.values_[3] + values_[3] * matrix.values_[4] + values_[6] * matrix.values_[5];
	result.values_[4] = values_[1] * matrix.values_[3] + values_[4] * matrix.values_[4] + values_[7] * matrix.values_[5];
	result.values_[5] = values_[2] * matrix.values_[3] + values_[5] * matrix.values_[4] + values_[8] * matrix.values_[5];
	result.values_[6] = values_[0] * matrix.values_[6] + values_[3] * matrix.values_[7] + values_[6] * matrix.values_[8];
	result.values_[7] = values_[1] * matrix.values_[6] + values_[4] * matrix.values_[7] + values_[7] * matrix.values_[8];
	result.values_[8] = values_[2] * matrix.values_[6] + values_[5] * matrix.values_[7] + values_[8] * matrix.values_[8];

	return result;
}

template <typename T>
OCEAN_FORCE_INLINE VectorT2<T> SquareMatrixT3<T>::operator*(const VectorT2<T>& vector) const
{
	/**
	 * | x' |   | 0 3 6 |   | x |
	 * | y' | = | 1 4 7 | * | y |
	 * | 1  |   | 2 5 8 |   | 1 |
	 */

	const T z = values_[2] * vector[0] + values_[5] * vector[1] + values_[8];
	ocean_assert(NumericT<T>::isNotEqualEps(z) && "Division by zero!");

	const T factor = T(1) / z;

	return VectorT2<T>((values_[0] * vector[0] + values_[3] * vector[1] + values_[6]) * factor,
						(values_[1] * vector[0] + values_[4] * vector[1] + values_[7]) * factor);
}

template <typename T>
OCEAN_FORCE_INLINE VectorT3<T> SquareMatrixT3<T>::operator*(const VectorT3<T>& vector) const
{
	return VectorT3<T>(values_[0] * vector[0] + values_[3] * vector[1] + values_[6] * vector[2],
						values_[1] * vector[0] + values_[4] * vector[1] + values_[7] * vector[2],
						values_[2] * vector[0] + values_[5] * vector[1] + values_[8] * vector[2]);
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20

template <>
OCEAN_FORCE_INLINE VectorT3<double> SquareMatrixT3<double>::operator*(const VectorT3<double>& vector) const
{
	// the following code uses the following SSE instructions, and needs SSE2 or higher

	// SSE2:
	// _mm_load1_pd
	// _mm_loadu_pd
	// _mm_mul_pd
	// _mm_add_pd
	// _mm_storeu_pd


	// we use the following strategy:
	// the values of the matrix are column aligned so that we normally would need to transpose the matrix before we can apply simple SIMD instructions
	// however, we do not transpose the matrix (we avoid the shuffle instructions) and instead multiply the matrix column-wise:
	// finally we sum the four columns and have the result, compared to the transpose-based approach this approach is approx. two times faster
	//
	// A D G     a     Aa  +  Db  +  Gc
	// B E H     b     Ba  +  Eb  +  Hc
	// C F I  *  c  =  Ca  +  Fb  +  Ic

	const double* const vectorValues = vector.data();

	// first we load the first vector element in all 64bit elements of the 128 bit register, so that we receive [a, a]
	const __m128d v0 = _mm_load1_pd(vectorValues + 0);

	const __m128d c0 = _mm_loadu_pd(values_ + 0);

	// now we multiply the 128 bit register [A, B] * [a, a] = [Aa, Ba]
	const __m128d r0 = _mm_mul_pd(c0, v0);


	// now we proceed with the second column
	const __m128d v1 = _mm_load1_pd(vectorValues + 1);
	const __m128d c1 = _mm_loadu_pd(values_ + 3);
	const __m128d r1 = _mm_mul_pd(c1, v1);

	// and we sum the result of the first column with the result of the second column
	__m128d result_f64x2 = _mm_add_pd(r0, r1);


	// now we proceed with the third column
	const __m128d v2 = _mm_load1_pd(vectorValues + 2);
	const __m128d c2 = _mm_loadu_pd(values_ + 6);
	const __m128d r2 = _mm_mul_pd(c2, v2);

	// we sum the results
	result_f64x2 = _mm_add_pd(result_f64x2, r2);

	VectorT3<double> result;
	result[2] = values_[2] * vector[0] + values_[5] * vector[1] + values_[8] * vector[2];
	_mm_storeu_pd(result.data(), result_f64x2);

	return result;
}

template <>
OCEAN_FORCE_INLINE VectorT3<float> SquareMatrixT3<float>::operator*(const VectorT3<float>& vector) const
{
	// the following code uses the following SSE instructions, and needs SSE2 or higher

	// SSE1:
	// _mm_load1_ps
	// _mm_loadu_ps
	// _mm_mul_ps
	// _mm_add_ps
	// _mm_storeu_ps

	// SSE2:
	// _mm_castps_si128
	// _mm_srli_si128
	// _mm_castsi128_ps


	// we use the following strategy:
	// the values of the matrix are column aligned so that we normally would need to transpose the matrix before we can apply simple SIMD instructions
	// however, we do not transpose the matrix (we avoid the shuffle instructions) and instead multiply the matrix column-wise:
	// finally we sum the four columns and have the result, compared to the transpose-based approach this approach is approx. two times faster
	//
	// A D G     a     Aa  +  Db  +  Gc
	// B E H     b     Ba  +  Eb  +  Hc
	// C F I  *  c  =  Ca  +  Fb  +  Ic


	// first we load the first vector element in all 32bit elements of the 128 bit register, so that we receive [a, a, a, a]
	const __m128 v0 = _mm_load1_ps(vector.data() + 0);

	// now we load the first column to receive: [A, B, C, D], beware: D will be unused
	const __m128 c0 = _mm_loadu_ps(values_ + 0);

	// now we multiply the 128 bit register [A, B, C, D] * [a, a, a, a] = [Aa, Ba, Ca, Da]
	const __m128 r0 = _mm_mul_ps(c0, v0);


	// now we proceed with the second column
	const __m128 v1 = _mm_load1_ps(vector.data() + 1);
	const __m128 c1 = _mm_loadu_ps(values_ + 3);
	const __m128 r1 = _mm_mul_ps(c1, v1);

	// and we sum the result of the first column with the result of the second column
	__m128 result_f32x4 = _mm_add_ps(r0, r1);


	// now we proceed with the third column, this time we have to load [F, G, H, I] and we shift the values
	const __m128 v2 = _mm_load1_ps(vector.data() + 2);
	__m128 c2 = _mm_loadu_ps(values_ + 5);
	c2 = _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(c2), 4)); // shift the 128 bit register by 4 bytes to the right to receive [G H I 0]
	const __m128 r2 = _mm_mul_ps(c2, v2);

	// we sum the results
	result_f32x4 = _mm_add_ps(result_f32x4, r2);

	float resultValues[4];
	_mm_storeu_ps(resultValues, result_f32x4);

	// and finally we store the results back to the vector
	return VectorT3<float>(resultValues);
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 20

template <typename T>
SquareMatrixT3<T> SquareMatrixT3<T>::operator*(const T value) const
{
	SquareMatrixT3<T> result(*this);
	result.values_[0] *= value;
	result.values_[1] *= value;
	result.values_[2] *= value;
	result.values_[3] *= value;
	result.values_[4] *= value;
	result.values_[5] *= value;
	result.values_[6] *= value;
	result.values_[7] *= value;
	result.values_[8] *= value;

	return result;
}

template <typename T>
SquareMatrixT3<T>& SquareMatrixT3<T>::operator*=(const T value)
{
	values_[0] *= value;
	values_[1] *= value;
	values_[2] *= value;
	values_[3] *= value;
	values_[4] *= value;
	values_[5] *= value;
	values_[6] *= value;
	values_[7] *= value;
	values_[8] *= value;

	return *this;
}

template <typename T>
void SquareMatrixT3<T>::multiply(const SquareMatrixT3<T>& matrix, const VectorT2<T>* vectors, VectorT2<T>* results, const size_t number)
{
	ocean_assert((vectors && results) || number == 0);

	for (size_t n = 0; n < number; ++n)
	{
		results[n] = matrix * vectors[n];
	}
}

template <typename T>
void SquareMatrixT3<T>::multiply(const SquareMatrixT3<T>& matrix, const VectorT3<T>* vectors, VectorT3<T>* results, const size_t number)
{
	ocean_assert((vectors && results) || number == 0);

	for (size_t n = 0; n < number; ++n)
	{
		results[n] = matrix * vectors[n];
	}
}

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20

template <>
inline void SquareMatrixT3<float>::multiply(const SquareMatrixT3<float>& matrix, const VectorT3<float>* vectors, VectorT3<float>* results, const size_t number)
{
	// the following code uses the following SSE instructions, and needs SSE2 or higher

	// SSE1:
	// _mm_load1_ps
	// _mm_loadu_ps
	// _mm_mul_ps
	// _mm_add_ps
	// _mm_storeu_ps

	// SSE2:
	// _mm_castps_si128
	// _mm_srli_si128
	// _mm_castsi128_ps

	// now we load the three columns (and ignore the last entry)
	const __m128 c0 = _mm_loadu_ps(matrix.values_ + 0);
	const __m128 c1 = _mm_loadu_ps(matrix.values_ + 3);
	__m128 c2 = _mm_loadu_ps(matrix.values_ + 5);
	c2 = _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(c2), 4)); // shift the 128 bit register by 4 bytes to the right to receive [G H I 0]

	for (size_t n = 0u; n < number - 1; ++n)
	{
		const __m128 v0 = _mm_load1_ps(vectors[n].data() + 0);
		const __m128 v1 = _mm_load1_ps(vectors[n].data() + 1);
		const __m128 v2 = _mm_load1_ps(vectors[n].data() + 2);

		const __m128 r0 = _mm_mul_ps(c0, v0);
		const __m128 r1 = _mm_mul_ps(c1, v1);

		__m128 result_f32x4= _mm_add_ps(r0, r1);

		const __m128 r2 = _mm_mul_ps(c2, v2);

		result_f32x4 = _mm_add_ps(result_f32x4, r2);

		// now we write all four values (although the target vector holds 3 elements - we can due that as there is still one 3D vector target vector left)
		_mm_storeu_ps(results[n].data(), result_f32x4);
	}

	// now we handle the last vector explicitly

	const __m128 v0 = _mm_load1_ps(vectors[number - 1].data() + 0);
	const __m128 v1 = _mm_load1_ps(vectors[number - 1].data() + 1);
	const __m128 v2 = _mm_load1_ps(vectors[number - 1].data() + 2);

	const __m128 r0 = _mm_mul_ps(c0, v0);
	const __m128 r1 = _mm_mul_ps(c1, v1);

	__m128 result_f32x4 = _mm_add_ps(r0, r1);

	const __m128 r2 = _mm_mul_ps(c2, v2);

	result_f32x4 = _mm_add_ps(result_f32x4, r2);

	float resultValues[4];
	_mm_storeu_ps(resultValues, result_f32x4);

	results[number - 1] = VectorT3<float>(resultValues);
}

#endif // OCEAN_HARDWARE_SSE_VERSION >= 20

template <typename T>
template <typename U>
inline SquareMatricesT3<T> SquareMatrixT3<T>::matrices2matrices(const SquareMatricesT3<U>& matrices)
{
	SquareMatricesT3<T> result;
	result.reserve(matrices.size());

	for (const SquareMatrixT3<U>& matrix : matrices)
	{
		result.emplace_back(matrix);
	}

	return result;
}

template <>
template <>
inline std::vector< SquareMatrixT3<float> > SquareMatrixT3<float>::matrices2matrices(const std::vector< SquareMatrixT3<float> >& matrices)
{
	return matrices;
}

template <>
template <>
inline std::vector< SquareMatrixT3<double> > SquareMatrixT3<double>::matrices2matrices(const std::vector< SquareMatrixT3<double> >& matrices)
{
	return matrices;
}

template <typename T>
template <typename U>
inline std::vector< SquareMatrixT3<T> > SquareMatrixT3<T>::matrices2matrices(const SquareMatrixT3<U>* matrices, const size_t size)
{
	std::vector< SquareMatrixT3<T> > result;
	result.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		result.emplace_back(matrices[n]);
	}

	return result;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const SquareMatrixT3<T>& matrix)
{
	stream << "|" << matrix(0, 0) << ", " << matrix(0, 1) << ", " << matrix(0, 2) << "|" << std::endl;
	stream << "|" << matrix(1, 0) << ", " << matrix(1, 1) << ", " << matrix(1, 2) << "|" << std::endl;
	stream << "|" << matrix(2, 0) << ", " << matrix(2, 1) << ", " << matrix(2, 2) << "|";

	return stream;
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const SquareMatrixT3<T>& matrix)
{
	return messageObject << "|" << matrix(0, 0) << ", " << matrix(0, 1) << ", " << matrix(0, 2) << "|\n|"
							<< matrix(1, 0) << ", " << matrix(1, 1) << ", " << matrix(1, 2) << "|\n|"
							<< matrix(2, 0) << ", " << matrix(2, 1) << ", " << matrix(2, 2) << "|";
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const SquareMatrixT3<T>& matrix)
{
	return messageObject << "|" << matrix(0, 0) << ", " << matrix(0, 1) << ", " << matrix(0, 2) << "|\n|"
							<< matrix(1, 0) << ", " << matrix(1, 1) << ", " << matrix(1, 2) << "|\n|"
							<< matrix(2, 0) << ", " << matrix(2, 1) << ", " << matrix(2, 2) << "|";
}

}

#endif // META_OCEAN_MATH_SQUARE_MATRIX_3_H
