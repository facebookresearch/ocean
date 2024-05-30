/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_HOMOGENOUS_MATRIX_4_H
#define META_OCEAN_MATH_HOMOGENOUS_MATRIX_4_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector3.h"
#include "ocean/math/Vector4.h"

#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class EulerT;

// Forward declaration.
template <typename T> class RotationT;

// Forward declaration.
template <typename T> class SquareMatrixT4;

// Forward declaration.
template <typename T> class QuaternionT;

// Forward declaration.
template <typename T> class HomogenousMatrixT4;

/**
 * Definition of the HomogenousMatrix4 object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION flag either with single or double precision float data type.
 * @see HomogenousMatrixT4
 * @ingroup math
 */
typedef HomogenousMatrixT4<Scalar> HomogenousMatrix4;

/**
 * Instantiation of the HomogenousMatrixT4 template class using a double precision float data type.
 * @see HomogenousMatrixT4
 * @ingroup math
 */
typedef HomogenousMatrixT4<double> HomogenousMatrixD4;

/**
 * Instantiation of the HomogenousMatrixT4 template class using a float precision float data type.
 * @see HomogenousMatrixT4
 * @ingroup math
 */
typedef HomogenousMatrixT4<float> HomogenousMatrixF4;

/**
 * Definition of a typename alias for vectors with HomogenousMatrixT4 objects.
 * @see HomogenousMatrixT4
 * @ingroup math
 */
template <typename T>
using HomogenousMatricesT4 = std::vector<HomogenousMatrixT4<T>>;

/**
 * Definition of a vector holding HomogenousMatrix4 objects.
 * @see HomogenousMatrix4
 * @ingroup math
 */
typedef std::vector<HomogenousMatrix4> HomogenousMatrices4;

/**
 * Definition of a vector holding HomogenousMatrixD4 objects.
 * @ingroup math
 */
typedef std::vector<HomogenousMatrixD4> HomogenousMatricesD4;

/**
 * Definition of a vector holding HomogenousMatrixF4 objects.
 * @ingroup math
 */
typedef std::vector<HomogenousMatrixF4> HomogenousMatricesF4;


/**
 * This class implements a 4x4 homogeneous transformation matrix using floating point values with the precision specified by type T.
 * <pre>
 * The values are stored in a column major/aligned order with indices:
 * | 0  4  8  12 |
 * | 1  5  9  13 |
 * | 2  6  10 14 |
 * | 3  7  11 15 |
 *
 * This matrix allows homogeneous transformations only.
 * With basis vectors (rx1, ry1, rz1), (rx2, ry2, rz2), (rx3, ry3, rz3), and translation vector (tx, ty, tz):
 * | rx1 rx2 rx3 tx |
 * | ry1 ry2 ry3 ty |
 * | rz1 rz2 rz3 tz |
 * |  0   0   0   1 |
 * </pre>
 * @tparam T Data type of matrix elements
 * @see HomogenousMatrix4, HomogenousMatrixF4, HomogenousMatrixD4.
 * @ingroup math
 */
template <typename T>
class HomogenousMatrixT4
{
	template <typename U> friend class HomogenousMatrixT4;

	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates a new default HomogenousMatrixT4 object with undefined elements.
		 * Beware: This matrix is neither a zero nor an entity matrix!
		 */
		inline HomogenousMatrixT4();

		/**
		 * Copy constructor.
		 * @param matrix The matrix to copy
		 */
		inline HomogenousMatrixT4(const HomogenousMatrixT4<T>& matrix);

		/**
		 * Copy constructor for a matrix with difference element data type than T.
		 * @param matrix The matrix to copy
		 * @tparam U The element data type of the second matrix
		 */
		template <typename U>
		inline explicit HomogenousMatrixT4(const HomogenousMatrixT4<U>& matrix);

		/**
		 * Creates a new HomogenousMatrixT4.
		 * Beware: The zero matrix will hold a zero in the lower right corner which must be set explicitly later to create a valid matrix.<br>
		 * Only constructors explicitly set the lower right value to 1 like e.g., the constructor building a matrix from a rotation or translation and so on.
		 * @param setToIdentity True, to create a identity matrix; False, to set a zero matrix
		 */
		inline explicit HomogenousMatrixT4(const bool setToIdentity);

		/**
		 * Creates a new HomogenousMatrixT4 object by 16 given floating point values of type U.
		 * @param arrayValues The array with 16 matrix elements of type U
		 */
		template <typename U>
		explicit HomogenousMatrixT4(const U* arrayValues);

		/**
		 * Creates a new HomogenousMatrixT4 object by 16 given floating point values.
		 * @param arrayValues The array with 16 matrix elements
		 */
		explicit HomogenousMatrixT4(const T* arrayValues);

		/**
		 * Creates a new HomogenousMatrixT4 object by an array of at least sixteen elements of float type U.
		 * @param arrayValues The sixteen matrix elements defining the new matrix, must be valid
		 * @param valuesRowAligned True, if the given values are stored in a row aligned order; False, if the values are stored in a column aligned order (which is the default case for this matrix)
		 * @tparam U The floating point type of the given elements
		 */
		template <typename U>
		HomogenousMatrixT4(const U* arrayValues, const bool valuesRowAligned);

		/**
		 * Creates a new HomogenousMatrixT4 object by an array of at least sixteen elements.
		 * @param valuesRowAligned True, if the given values are stored in a row aligned order; False, if the values are stored in a column aligned order (which is the default case for this matrix)
		 * @param arrayValues The sixteen matrix elements defining the new matrix, must be valid
		 */
		HomogenousMatrixT4(const T* arrayValues, const bool valuesRowAligned);

		/**
		 * Creates a new HomogenousMatrixT4 object with only a translation.
		 * @param translation The translation of the resulting transformation
		 */
		explicit HomogenousMatrixT4(const VectorT3<T>& translation);

		/**
		 * Creates a new HomogenousMatrixT4 object with only a rotation.
		 * @param rotation The angle-axis rotation of the resulting transformation
		 */
		explicit HomogenousMatrixT4(const RotationT<T>& rotation);

		/**
		 * Creates a new HomogenousMatrixT4 object with only a rotation given as Euler rotation.
		 * @param euler The euler rotation of the resulting transformation
		 */
		explicit HomogenousMatrixT4(const EulerT<T>& euler);

		/**
		 * Creates a new HomogenousMatrixT4 object with only a rotation given as quaternion.
		 * @param rotation The rotation of the resulting transformation
		 */
		explicit HomogenousMatrixT4(const QuaternionT<T>& rotation);

		/**
		 * Creates a new HomogenousMatrixT4 object with only a rotation given as 3x3 rotation matrix.
		 * @param rotation 3x3 rotation matrix
		 */
		explicit HomogenousMatrixT4(const SquareMatrixT3<T>& rotation);

		/**
		 * Creates a new HomogenousMatrixT4 object from a 4x4 square matrix.
		 * @param matrix The 4x4 square matrix
		 */
		explicit HomogenousMatrixT4(const SquareMatrixT4<T>& matrix);

		/**
		 * Creates a new HomogenousMatrixT4 object with a translation and rotation.
		 * The resulting transformation can be written as the following matrix multiplication:
		 * <pre>
		 * HomogenousMatrix4(translation, rotation) == HomogenousMatrix4(translation) * HomogenousMatrix4(rotation)
		 * </pre>
		 * @param translation The translation of the resulting transformation
		 * @param rotation The rotation of the resulting transformation, must be valid
		 */
		HomogenousMatrixT4(const VectorT3<T>& translation, const RotationT<T>& rotation);

		/**
		 * Creates a new HomogenousMatrixT4 object with a translation and rotation.
		 * @param translationAndRotation The pair of translation and rotation of the resulting transformation
		 */
		explicit HomogenousMatrixT4(const std::pair<VectorT3<T>, RotationT<T>>& translationAndRotation);

		/**
		 * Creates a new HomogenousMatrixT4 object with a translation and rotation.
		 * The resulting transformation can be written as the following matrix multiplication:
		 * <pre>
		 * HomogenousMatrix4(translation, euler) == HomogenousMatrix4(translation) * HomogenousMatrix4(euler)
		 * </pre>
		 * @param translation The translation of the resulting transformation
		 * @param euler The euler rotation of the resulting transformation, must be valid
		 */
		HomogenousMatrixT4(const VectorT3<T>& translation, const EulerT<T>& euler);

		/**
		 * Creates a new HomogenousMatrixT4 object with a translation and rotation.
		 * The resulting transformation can be written as the following matrix multiplication:
		 * <pre>
		 * HomogenousMatrix4(translation, rotation) == HomogenousMatrix4(translation) * HomogenousMatrix4(rotation)
		 * </pre>
		 * @param translation The translation of the resulting transformation
		 * @param rotation The quaternion rotation of the resulting transformation, must be valid
		 */
		HomogenousMatrixT4(const VectorT3<T>& translation, const QuaternionT<T>& rotation);

		/**
		 * Creates a new HomogenousMatrixT4 object with a translation and rotation.
		 * @param translationAndRotation The pair of translation and rotation of the resulting transformation
		 */
		explicit HomogenousMatrixT4(const std::pair<VectorT3<T>, QuaternionT<T>>& translationAndRotation);

		/**
		 * Creates a new HomogenousMatrixT4 object with a translation and rotation matrix.
		 * The resulting transformation can be written as the following matrix multiplication:
		 * <pre>
		 * HomogenousMatrix4(translation, rotation) == HomogenousMatrix4(translation) * HomogenousMatrix4(rotation)
		 * </pre>
		 * @param translation The translation of the resulting transformation
		 * @param rotation The rotation matrix of the resulting transformation, must be valid
		 */
		HomogenousMatrixT4(const VectorT3<T>& translation, const SquareMatrixT3<T>& rotation);

		/**
		 * Creates a new HomogenousMatrixT4 object by a translation and a scale.
		 * @param translation The translation of the resulting transformation
		 * @param scale The scale of the resulting transformation
		 */
		HomogenousMatrixT4(const VectorT3<T>& translation, const VectorT3<T>& scale);

		/**
		 * Creates a new HomogenousMatrixT4 object by a translation, rotation and scale.
		 * @param translation The translation of the resulting transformation
		 * @param rotation The rotation of the resulting transformation, must be valid
		 * @param scale The scale of the resulting transformation
		 */
		HomogenousMatrixT4(const VectorT3<T>& translation, const RotationT<T>& rotation, const VectorT3<T>& scale);

		/**
		 * Creates a new HomogenousMatrixT4 object by a translation, rotation, scale and shear.
		 * @param translation The translation of the resulting transformation
		 * @param rotation The rotation of the resulting transformation as unit quaternion, must be valid
		 * @param scale The scale of the resulting transformation
		 * @param shear The shear of the resulting transformation with order (xy, xz, yz)
		 */
		HomogenousMatrixT4(const VectorT3<T>& translation, const QuaternionT<T>& rotation, const VectorT3<T>& scale, const VectorT3<T>& shear);

		/**
		 * Creates a new HomogenousMatrixT4 object by a translation, rotation, scale and shear.
		 * @param translation The translation of the resulting transformation
		 * @param rotation The rotation of the resulting transformation, must be valid
		 * @param scale The scale of the resulting transformation
		 * @param shear The shear of the resulting transformation with order (xy, xz, yz)
		 */
		HomogenousMatrixT4(const VectorT3<T>& translation, const RotationT<T>& rotation, const VectorT3<T>& scale, const VectorT3<T>& shear);

		/**
		 * Creates a new HomogenousMatrixT4 object by a translation, rotation and scale.
		 * @param translation The translation of the resulting transformation
		 * @param rotation The quaternion rotation of the resulting transformation, must be valid
		 * @param scale The scale of the resulting transformation
		 */
		HomogenousMatrixT4(const VectorT3<T>& translation, const QuaternionT<T>& rotation, const VectorT3<T>& scale);

		/**
		 * Creates a new HomogenousMatrixT4 object by three basis vectors.
		 * @param xAxis First basis vector
		 * @param yAxis Second basis vector
		 * @param zAxis Third basis vector
		 */
		HomogenousMatrixT4(const VectorT3<T>& xAxis, const VectorT3<T>& yAxis, const VectorT3<T>& zAxis);

		/**
		 * Creates a new HomogenousMatrixT4 object by three basis vectors and a translation vector.
		 * @param xAxis First basis vector
		 * @param yAxis Second basis vector
		 * @param zAxis Third basis vector
		 * @param translation The translation vector
		 */
		HomogenousMatrixT4(const VectorT3<T>& xAxis, const VectorT3<T>& yAxis, const VectorT3<T>& zAxis, const VectorT3<T>& translation);

		/**
		 * Returns the x-axis of the transformation which is the first vector of the upper left 3x3 rotation matrix of this homogeneous 4x4 transformation.
		 * @return The x-axis of this transformation
		 */
		inline VectorT3<T> xAxis() const;

		/**
		 * Returns the y-axis of the transformation which is the second vector of the upper left 3x3 rotation matrix of this homogeneous 4x4 transformation.
		 * @return The y-axis of this transformation
		 */
		inline VectorT3<T> yAxis() const;

		/**
		 * Returns the z-axis of the transformation which is the first vector of the upper left 3x3 rotation matrix of this homogeneous 4x4 transformation.
		 * @return The z-axis of this transformation
		 */
		inline VectorT3<T> zAxis() const;

		/**
		 * Returns the translation of the transformation.
		 * @return Translation
		 */
		inline VectorT3<T> translation() const;

		/**
		 * Returns the rotation of the transformation as quaternion.
		 * @return Rotation
		 */
		QuaternionT<T> rotation() const;

		/**
		 * Returns the scale of the transformation.
		 * @return Transformation scale
		 */
		VectorT3<T> scale() const;

		/**
		 * Decomposes the transformation matrix into translation, rotation, scale and shear parameters.
		 * @param translation The returning translation parameter
		 * @param rotation The returning rotation parameter as quaternion
		 * @param scale The returning scale parameter
		 * @param shear The returning shear parameter with order (xy, xz, yz)
		 * @return True, if succeeded (otherwise the transformation matrix has a zero-scaling axis and the translation is decomposed only)
		 */
		bool decompose(VectorT3<T>& translation, QuaternionT<T>& rotation, VectorT3<T>& scale, VectorT3<T>& shear) const;

		/**
		 * Returns the rotation matrix of the transformation.
		 * @return Rotation matrix containing scale
		 */
		inline SquareMatrixT3<T> rotationMatrix() const;

		/**
		 * Copies the 3x3 rotation matrix elements of the 4x4 transformation.
		 * @param data The buffer receiving the nine rotation matrix elements, must be valid
		 * @param transposed True, to copy the transposed rotation matrix (to copy the array into a row major buffer); False, to copy the matrix into a column major buffer
		 */
		inline void rotationMatrix(T* data, const bool transposed = false) const;

		/**
		 * Returns the 3x3 orthonormal rotation matrix of the 4x4 transformation (by forcing a orthogonal and normalized rotation matrix).
		 * All vectors of the resulting rotation matrix have unit length.
		 * @return The normalized rotation matrix
		 */
		SquareMatrixT3<T> orthonormalRotationMatrix() const;

		/**
		 * Returns the transposed of this matrix.
		 * @return Transposed matrix as square 4x4 matrix
		 */
		SquareMatrixT4<T> transposed() const;

		/**
		 * Returns the inverted of this matrix.
		 * This matrix must not be singular, ensure that the matrix is invertible before calling this function.<br>
		 * Even better: avoid the usage of this function and call invert() instead.<br>
		 * In case, this matrix is not invertible, this matrix will be returned instead.
		 * @return The inverted matrix
		 * see invert().
		 */
		HomogenousMatrixT4<T> inverted() const noexcept;

		/**
		 * Inverts the matrix.
		 * @return True, if the matrix could be inverted (because the matrix was not singular)
		 */
		bool invert();

		/**
		 * Inverts the matrix and returns the result as parameter.
		 * @param invertedMatrix The resulting inverted matrix
		 * @return True, if the matrix could be inverted because the matrix is not singular
		 * @see inverted(), solve().
		 */
		bool invert(HomogenousMatrixT4<T>& invertedMatrix) const;

		/**
		 * Returns the determinant of the matrix.
		 * @return Matrix determinant
		 */
		T determinant() const;

		/**
		 * Returns the trace of the matrix which is the sum of the diagonal elements.
		 * @return Trace of the matrix
		 */
		inline T trace() const;

		/**
		 * Sets the translation of this transformation.
		 * @param translation The translation to set
		 * @return Reference to this transformation matrix
		 */
		inline HomogenousMatrixT4<T>& setTranslation(const VectorT3<T>& translation);

		/**
		 * Sets the rotation of this transformation.
		 * @param rotation The rotation to set, must be valid
		 * @return Reference to this transformation matrix
		 */
		HomogenousMatrixT4<T>& setRotation(const RotationT<T>& rotation);

		/**
		 * Sets the rotation of this transformation.
		 * @param quaternion The quaternion rotation to set, must be valid
		 * @return Reference to this transformation matrix
		 */
		HomogenousMatrixT4<T>& setRotation(const QuaternionT<T>& quaternion);

		/**
		 * Sets the rotation of this transformation.
		 * @param matrix The 3x3 rotation matrix to set
		 * @return Reference to this transformation matrix
		 */
		HomogenousMatrixT4<T>& setRotation(const SquareMatrixT3<T>& matrix);

		/**
		 * Applies new scale values.
		 * @param scale The new scale values which are applied to the inner 3x3 rotation matrix
		 * @return Reference to this transformation matrix
		 */
		HomogenousMatrixT4<T>& applyScale(const VectorT3<T>& scale);

		/**
		 * Sets the matrix to the identity matrix.
		 * @see isIdentity().
		 */
		inline void toIdentity();

		/**
		 * Sets the matrix to a zero matrix (including the lower right element).
		 * @see isNull();
		 */
		inline void toNull();

		/**
		 * Returns whether this matrix is a valid homogeneous transformation.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns whether this matrix is an identity matrix.
		 * @return True, if so
		 * @see toIdentity().
		 */
		bool isIdentity() const;

		/**
		 * Returns whether two matrices are almost identical up to a specified epsilon.
		 * @param matrix Second matrix that will be checked
		 * @param epsilon The epsilon threshold to be used, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isEqual(const HomogenousMatrixT4<T>& matrix, const T epsilon = NumericT<T>::eps()) const;

		/**
		 * Returns whether this matrix is a zero matrix (with all elements equal to zero).
		 * @return True, if so
		 * @see toNull().
		 */
		bool isNull() const;

		/**
		 * Returns a pointer to the internal values.
		 * @return The pointer to the internal values, always valid
		 */
		inline const T* data() const;

		/**
		 * Returns a pointer to the internal values.
		 * @return The pointer to the internal values, always valid
		 */
		inline T* data();

		/**
		 * Copies the elements of this matrix to an array with floating point values of the same type T.
		 * @param arrayValues Array with 16 floating point values of type T receiving the elements of this matrix, must be valid
		 * @param valuesRowAligned True, if the target values are stored in a row aligned order; False, if the values are stored in a column aligned order (which is the default case for this matrix)
		 */
		inline void copyElements(T* arrayValues, const bool valuesRowAligned = false) const;

		/**
		 * Copies the elements of this matrix to an array with floating point values of the type U.
		 * @param arrayValues Array with 16 floating point values of type U receiving the elements of this matrix, must be valid
		 * @param valuesRowAligned True, if the target values are stored in a row aligned order; False, if the values are stored in a column aligned order (which is the default case for this matrix)
		 */
		template <typename U>
		inline void copyElements(U* arrayValues, const bool valuesRowAligned = false) const;

		/**
		 * Transforms a 3D vector by application of only the inner rotation matrix (including scale and shearing) of this transformation.
		 * @param vector The vector to be transformed
		 * @return Transformed 3D vector
		 */
		inline VectorT3<T> rotationMatrix(const VectorT3<T>& vector) const;

		/**
		 * Transforms a 3D vector by application of only the inner transposed rotation matrix (including scale and shearing) of this transformation.
		 * @param vector The vector to be transformed
		 * @return Transformed 3D vector
		 */
		inline VectorT3<T> transposedRotationMatrix(const VectorT3<T>& vector) const;

		/**
		 * Default copy assignment operator.
		 * @return Reference to this object
		 */
		HomogenousMatrixT4<T>& operator=(const HomogenousMatrixT4<T>&) = default;

		/**
		 * Returns whether two transformations are identical up to a small epsilon.
		 * @param matrix Right operand
		 * @return True, if so
		 */
		bool operator==(const HomogenousMatrixT4<T>& matrix) const;

		/**
		 * Returns whether two transformations are not identical up to a small epsilon.
		 * @param matrix Right operand
		 * @return True, if so
		 */
		inline bool operator!=(const HomogenousMatrixT4<T>& matrix) const;

		/**
		 * Combines two transformation matrices.
		 * @param matrix Right transformation matrix
		 * @return Combined transformation matrix
		 */
		HomogenousMatrixT4<T> operator*(const HomogenousMatrixT4<T>& matrix) const;

		/**
		 * Combines and assigns two transformation matrices.
		 * @param matrix Right transformation matrix
		 * @return Reference to this transformation matrix
		 */
		inline HomogenousMatrixT4<T>& operator*=(const HomogenousMatrixT4<T>& matrix);

		/**
		 * Combines a transformation with a rotation.
		 * @param rotation The rotation to combine, must be valid
		 * @return Combined transformation matrix
		 */
		HomogenousMatrixT4<T> operator*(const RotationT<T>& rotation) const;

		/**
		 * Combines and assigns a transformation with a rotation.
		 * @param rotation The rotation to combine, must be valid
		 * @return Reference to this transformation matrix
		 */
		inline HomogenousMatrixT4<T>& operator*=(const RotationT<T>& rotation);

		/**
		 * Combines a transformation with a quaternion rotation.
		 * @param rotation The quaternion rotation to combine, must be valid
		 * @return Combined transformation matrix
		 */
		HomogenousMatrixT4<T> operator*(const QuaternionT<T>& rotation) const;

		/**
		 * Combines and assigns a transformation with a quaternion rotation.
		 * @param rotation The quaternion rotation to combine, must be valid
		 * @return Reference to this transformation matrix
		 */
		inline HomogenousMatrixT4<T>& operator*=(const QuaternionT<T>& rotation);

		/**
		 * Transforms a 3D vector.
		 * The vector will be extended with a 1.0 as fourth element and de-homogenized afterwards.
		 * @param vector The vector to transform
		 * @return Resulting transformed and homogenized 3D vector
		 */
		inline VectorT3<T> operator*(const VectorT3<T>& vector) const;

		/**
		 * Transforms a 4D vector.
		 * @param vector The vector to transform
		 * @return The resulting transformed 4D vector
		 */
		inline VectorT4<T> operator*(const VectorT4<T>& vector) const;

		/**
		 * Element operator.
		 * @param index The index of the element to return [0, 15]
		 * @return Specified element
		 */
		inline T operator[](const unsigned int index) const;

		/**
		 * Element operator.
		 * @param index The index of the element to return [0, 15]
		 * @return Specified element
		 */
		inline T& operator[](const unsigned int index);

		/**
		 * Element operator.
		 * @param row The row of the element to return [0, 3]
		 * @param column The column of the element to return [0, 3]
		 * @return Specified element
		 */
		inline T operator()(const unsigned int row, const unsigned int column) const;

		/**
		 * Element operator.
		 * @param row The row of the element to return [0, 3]
		 * @param column The column of the element to return [0, 3]
		 * @return Specified element
		 */
		inline T& operator()(const unsigned int row, const unsigned int column);

		/**
		 * Element operator.
		 * @param index The index of the element to return [0, 15]
		 * @return Specified element
		 */
		inline T operator()(const unsigned int index) const;

		/**
		 * Element operator.
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
		inline size_t operator()(const HomogenousMatrixT4<T>& matrix) const;

		/**
		 * Converts matrices with specific data type to matrices with different data type.
		 * @param matrices The matrices to convert
		 * @return The converted matrices
		 * @tparam U The element data type of the matrices to convert
		 */
		template <typename U>
		static inline std::vector< HomogenousMatrixT4<T> > matrices2matrices(const std::vector< HomogenousMatrixT4<U> >& matrices);

		/**
		 * Converts matrices with specific data type to matrices with different data type.
		 * @param matrices The matrices to convert
		 * @param size The number of matrices to convert
		 * @return The converted matrices
		 * @tparam U The element data type of the matrices to convert
		 */
		template <typename U>
		static inline std::vector< HomogenousMatrixT4<T> > matrices2matrices(const HomogenousMatrixT4<U>* matrices, const size_t size);

	protected:

		/// The sixteen values of the transformation matrix.
		T values_[16];
};

template <typename T>
inline HomogenousMatrixT4<T>::HomogenousMatrixT4()
{
	// nothing to do here
}

template <typename T>
inline HomogenousMatrixT4<T>::HomogenousMatrixT4(const HomogenousMatrixT4<T>& matrix)
{
	memcpy(values_, matrix.values_, sizeof(T) * 16);
}

template <typename T>
template <typename U>
inline HomogenousMatrixT4<T>::HomogenousMatrixT4(const HomogenousMatrixT4<U>& matrix)
{
	for (unsigned int n = 0u; n < 16u; ++n)
	{
		values_[n] = T(matrix.values_[n]);
	}
}

template <typename T>
inline HomogenousMatrixT4<T>::HomogenousMatrixT4(const bool setToIdentity)
{
	if (setToIdentity)
	{
		values_[ 0] = T(1.0);
		values_[ 1] = T(0.0);
		values_[ 2] = T(0.0);
		values_[ 3] = T(0.0);

		values_[ 4] = T(0.0);
		values_[ 5] = T(1.0);
		values_[ 6] = T(0.0);
		values_[ 7] = T(0.0);

		values_[ 8] = T(0.0);
		values_[ 9] = T(0.0);
		values_[10] = T(1.0);
		values_[11] = T(0.0);

		values_[12] = T(0.0);
		values_[13] = T(0.0);
		values_[14] = T(0.0);
		values_[15] = T(1.0);

		ocean_assert(isValid());
	}
	else
	{
		for (unsigned int n = 0u; n < 16u; ++n)
		{
			values_[n] = T(0);
		}

		ocean_assert(!isValid());
	}
}

template <typename T>
template <typename U>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const U* arrayValues)
{
	for (unsigned int n = 0u; n < 16u; ++n)
	{
		values_[n] = T(arrayValues[n]);
	}
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const T* arrayValues)
{
	memcpy(values_, arrayValues, sizeof(T) * 16);
}

template <typename T>
template <typename U>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const U* arrayValues, const bool valuesRowAligned)
{
	ocean_assert(arrayValues);

	if (valuesRowAligned)
	{
		values_[ 0] = T(arrayValues[ 0]);
		values_[ 1] = T(arrayValues[ 4]);
		values_[ 2] = T(arrayValues[ 8]);
		values_[ 3] = T(arrayValues[12]);
		values_[ 4] = T(arrayValues[ 1]);
		values_[ 5] = T(arrayValues[ 5]);
		values_[ 6] = T(arrayValues[ 9]);
		values_[ 7] = T(arrayValues[13]);
		values_[ 8] = T(arrayValues[ 2]);
		values_[ 9] = T(arrayValues[ 6]);
		values_[10] = T(arrayValues[10]);
		values_[11] = T(arrayValues[14]);
		values_[12] = T(arrayValues[ 3]);
		values_[13] = T(arrayValues[ 7]);
		values_[14] = T(arrayValues[11]);
		values_[15] = T(arrayValues[15]);
	}
	else
	{
		for (unsigned int n = 0u; n < 16u; ++n)
		{
			values_[n] = T(arrayValues[n]);
		}
	}
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const T* arrayValues, const bool valuesRowAligned)
{
	ocean_assert(arrayValues);

	if (valuesRowAligned)
	{
		values_[ 0] = arrayValues[ 0];
		values_[ 1] = arrayValues[ 4];
		values_[ 2] = arrayValues[ 8];
		values_[ 3] = arrayValues[12];
		values_[ 4] = arrayValues[ 1];
		values_[ 5] = arrayValues[ 5];
		values_[ 6] = arrayValues[ 9];
		values_[ 7] = arrayValues[13];
		values_[ 8] = arrayValues[ 2];
		values_[ 9] = arrayValues[ 6];
		values_[10] = arrayValues[10];
		values_[11] = arrayValues[14];
		values_[12] = arrayValues[ 3];
		values_[13] = arrayValues[ 7];
		values_[14] = arrayValues[11];
		values_[15] = arrayValues[15];
	}
	else
	{
		memcpy(values_, arrayValues, sizeof(T) * 16);
	}
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& translation)
{
	values_[ 0] = T(1.0);
	values_[ 1] = T(0.0);
	values_[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	values_[ 4] = T(0.0);
	values_[ 5] = T(1.0);
	values_[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	values_[ 8] = T(0.0);
	values_[ 9] = T(0.0);
	values_[10] = T(1.0);
	values_[11] = T(0.0);

	values_[12] = translation[0];
	values_[13] = translation[1];
	values_[14] = translation[2];
	values_[15] = T(1.0);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const RotationT<T>& rotation)
{
	ocean_assert(rotation.isValid());

	//values[ 0] = T(1.0);
	//values[ 1] = T(0.0);
	//values[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	//values[ 4] = T(0.0);
	//values[ 5] = T(1.0);
	//values[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	//values[ 8] = T(0.0);
	//values[ 9] = T(0.0);
	//values[10] = T(1.0);
	values_[11] = T(0.0);

	values_[12] = T(0.0);
	values_[13] = T(0.0);
	values_[14] = T(0.0);
	values_[15] = T(1.0);

	setRotation(rotation);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const EulerT<T>& euler)
{
	ocean_assert(euler.isValid());

	//values[ 0] = T(1.0);
	//values[ 1] = T(0.0);
	//values[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	//values[ 4] = T(0.0);
	//values[ 5] = T(1.0);
	//values[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	//values[ 8] = T(0.0);
	//values[ 9] = T(0.0);
	//values[10] = T(1.0);
	values_[11] = T(0.0);

	values_[12] = T(0.0);
	values_[13] = T(0.0);
	values_[14] = T(0.0);
	values_[15] = T(1.0);

	setRotation(SquareMatrixT3<T>(euler));

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const QuaternionT<T>& rotation)
{
	ocean_assert(rotation.isValid());

	//values[ 0] = T(1.0);
	//values[ 1] = T(0.0);
	//values[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	//values[ 4] = T(0.0);
	//values[ 5] = T(1.0);
	//values[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	//values[ 8] = T(0.0);
	//values[ 9] = T(0.0);
	//values[10] = T(1.0);
	values_[11] = T(0.0);

	values_[12] = T(0.0);
	values_[13] = T(0.0);
	values_[14] = T(0.0);
	values_[15] = T(1.0);

	setRotation(rotation);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const SquareMatrixT3<T>& rotation)
{
	values_[ 0] = rotation()[0];
	values_[ 1] = rotation()[1];
	values_[ 2] = rotation()[2];
	values_[ 3] = T(0.0);

	values_[ 4] = rotation()[3];
	values_[ 5] = rotation()[4];
	values_[ 6] = rotation()[5];
	values_[ 7] = T(0.0);

	values_[ 8] = rotation()[6];
	values_[ 9] = rotation()[7];
	values_[10] = rotation()[8];
	values_[11] = T(0.0);

	values_[12] = T(0.0);
	values_[13] = T(0.0);
	values_[14] = T(0.0);
	values_[15] = T(1.0);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const SquareMatrixT4<T>& matrix)
{
	memcpy(values_, matrix(), sizeof(T) * 16);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& translation, const RotationT<T>& rotation)
{
	ocean_assert(rotation.isValid());

	//values[ 0] = T(1.0);
	//values[ 1] = T(0.0);
	//values[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	//values[ 4] = T(0.0);
	//values[ 5] = T(1.0);
	//values[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	//values[ 8] = T(0.0);
	//values[ 9] = T(0.0);
	//values[10] = T(1.0);
	values_[11] = T(0.0);

	values_[12] = translation[0];
	values_[13] = translation[1];
	values_[14] = translation[2];
	values_[15] = T(1.0);

	setRotation(rotation);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const std::pair< VectorT3<T>, RotationT<T> >& translationAndRotation)
{
	ocean_assert(translationAndRotation.second.isValid());

	//values[ 0] = T(1.0);
	//values[ 1] = T(0.0);
	//values[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	//values[ 4] = T(0.0);
	//values[ 5] = T(1.0);
	//values[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	//values[ 8] = T(0.0);
	//values[ 9] = T(0.0);
	//values[10] = T(1.0);
	values_[11] = T(0.0);

	values_[12] = translationAndRotation.first[0];
	values_[13] = translationAndRotation.first[1];
	values_[14] = translationAndRotation.first[2];
	values_[15] = T(1.0);

	setRotation(translationAndRotation.second);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& translation, const EulerT<T>& euler)
{
	ocean_assert(euler.isValid());

	//values[ 0] = T(1.0);
	//values[ 1] = T(0.0);
	//values[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	//values[ 4] = T(0.0);
	//values[ 5] = T(1.0);
	//values[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	//values[ 8] = T(0.0);
	//values[ 9] = T(0.0);
	//values[10] = T(1.0);
	values_[11] = T(0.0);

	values_[12] = translation[0];
	values_[13] = translation[1];
	values_[14] = translation[2];
	values_[15] = T(1.0);

	setRotation(SquareMatrixT3<T>(euler));

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& translation, const QuaternionT<T>& rotation)
{
	ocean_assert(rotation.isValid());

	//values[ 0] = T(1.0);
	//values[ 1] = T(0.0);
	//values[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	//values[ 4] = T(0.0);
	//values[ 5] = T(1.0);
	//values[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	//values[ 8] = T(0.0);
	//values[ 9] = T(0.0);
	//values[10] = T(1.0);
	values_[11] = T(0.0);

	values_[12] = translation[0];
	values_[13] = translation[1];
	values_[14] = translation[2];
	values_[15] = T(1.0);

	setRotation(rotation);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const std::pair< VectorT3<T>, QuaternionT<T> >& translationAndRotation)
{
	ocean_assert(translationAndRotation.second.isValid());

	//values[ 0] = T(1.0);
	//values[ 1] = T(0.0);
	//values[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	//values[ 4] = T(0.0);
	//values[ 5] = T(1.0);
	//values[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	//values[ 8] = T(0.0);
	//values[ 9] = T(0.0);
	//values[10] = T(1.0);
	values_[11] = T(0.0);

	values_[12] = translationAndRotation.first[0];
	values_[13] = translationAndRotation.first[1];
	values_[14] = translationAndRotation.first[2];
	values_[15] = T(1.0);

	setRotation(translationAndRotation.second);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& translation, const SquareMatrixT3<T>& rotation)
{
	values_[ 0] = rotation()[0];
	values_[ 1] = rotation()[1];
	values_[ 2] = rotation()[2];
	values_[ 3] = T(0.0);

	values_[ 4] = rotation()[3];
	values_[ 5] = rotation()[4];
	values_[ 6] = rotation()[5];
	values_[ 7] = T(0.0);

	values_[ 8] = rotation()[6];
	values_[ 9] = rotation()[7];
	values_[10] = rotation()[8];
	values_[11] = T(0.0);

	values_[12] = translation[0];
	values_[13] = translation[1];
	values_[14] = translation[2];
	values_[15] = T(1.0);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& translation, const VectorT3<T>& scale)
{
	values_[ 0] = scale.x();
	values_[ 1] = T(0.0);
	values_[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	values_[ 4] = T(0.0);
	values_[ 5] = scale.y();
	values_[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	values_[ 8] = T(0.0);
	values_[ 9] = T(0.0);
	values_[10] = scale.z();
	values_[11] = T(0.0);

	values_[12] = translation[0];
	values_[13] = translation[1];
	values_[14] = translation[2];
	values_[15] = T(1.0);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& translation, const RotationT<T>& rotation, const VectorT3<T>& scale)
{
	ocean_assert(rotation.isValid());

	//values[ 0] = T(1.0);
	//values[ 1] = T(0.0);
	//values[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	//values[ 4] = T(0.0);
	//values[ 5] = T(1.0);
	//values[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	//values[ 8] = T(0.0);
	//values[ 9] = T(0.0);
	//values[10] = T(1.0);
	values_[11] = T(0.0);

	values_[12] = translation[0];
	values_[13] = translation[1];
	values_[14] = translation[2];
	values_[15] = T(1.0);

	setRotation(rotation);
	applyScale(scale);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& translation, const QuaternionT<T>& rotation, const VectorT3<T>& scale, const VectorT3<T>& shear)
{
	values_[3] = T(0.0);
	values_[7] = T(0.0);
	values_[11] = T(0.0);
	values_[15] = T(1.0);

	setRotation(rotation);

	values_[12] = T(0.0);
	values_[13] = T(0.0);
	values_[14] = T(0.0);

	HomogenousMatrixT4<T> shearMatrix(true);
	shearMatrix(0, 1) = shear(0);
	shearMatrix(0, 2) = shear(1);
	shearMatrix(1, 2) = shear(2);
	*this = *this * shearMatrix;

	applyScale(scale);

	memcpy(values_ + 12, translation(), sizeof(T) * 3);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& translation, const RotationT<T>& rotation, const VectorT3<T>& scale, const VectorT3<T>& shear)
{
	values_[3] = T(0.0);
	values_[7] = T(0.0);
	values_[11] = T(0.0);
	values_[15] = T(1.0);

	setRotation(rotation);

	HomogenousMatrixT4<T> shearMatrix(true);
	shearMatrix(0, 1) = shear(0);
	shearMatrix(0, 2) = shear(1);
	shearMatrix(1, 2) = shear(2);
	*this = *this * shearMatrix;

	applyScale(scale);

	memcpy(values_ + 12, translation(), sizeof(T) * 3);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& translation, const QuaternionT<T>& rotation, const VectorT3<T>& scale)
{
	memcpy(values_ + 12, translation(), sizeof(T) * 3);

	values_[3] = T(0.0);
	values_[7] = T(0.0);
	values_[11] = T(0.0);
	values_[15] = T(1.0);

	setRotation(rotation);
	applyScale(scale);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& xAxis, const VectorT3<T>& yAxis, const VectorT3<T>& zAxis)
{
	values_[ 0] = xAxis[0];
	values_[ 1] = xAxis[1];
	values_[ 2] = xAxis[2];
	values_[ 3] = T(0.0);

	values_[ 4] = yAxis[0];
	values_[ 5] = yAxis[1];
	values_[ 6] = yAxis[2];
	values_[ 7] = T(0.0);

	values_[ 8] = zAxis[0];
	values_[ 9] = zAxis[1];
	values_[10] = zAxis[2];
	values_[11] = T(0.0);

	values_[12] = T(0.0);
	values_[13] = T(0.0);
	values_[14] = T(0.0);
	values_[15] = T(1.0);

	ocean_assert(isValid());
}

template <typename T>
HomogenousMatrixT4<T>::HomogenousMatrixT4(const VectorT3<T>& xAxis, const VectorT3<T>& yAxis, const VectorT3<T>& zAxis, const VectorT3<T>& translation)
{
	values_[ 0] = xAxis[0];
	values_[ 1] = xAxis[1];
	values_[ 2] = xAxis[2];
	values_[ 3] = T(0.0);

	values_[ 4] = yAxis[0];
	values_[ 5] = yAxis[1];
	values_[ 6] = yAxis[2];
	values_[ 7] = T(0.0);

	values_[ 8] = zAxis[0];
	values_[ 9] = zAxis[1];
	values_[10] = zAxis[2];
	values_[11] = T(0.0);

	values_[12] = translation[0];
	values_[13] = translation[1];
	values_[14] = translation[2];
	values_[15] = T(1.0);

	ocean_assert(isValid());
}

template <typename T>
inline VectorT3<T> HomogenousMatrixT4<T>::xAxis() const
{
	ocean_assert(isValid());
	return VectorT3<T>(values_ + 0);
}

template <typename T>
inline VectorT3<T> HomogenousMatrixT4<T>::yAxis() const
{
	ocean_assert(isValid());
	return VectorT3<T>(values_ + 4);
}

template <typename T>
inline VectorT3<T> HomogenousMatrixT4<T>::zAxis() const
{
	ocean_assert(isValid());
	return VectorT3<T>(values_ + 8);
}

template <typename T>
inline VectorT3<T> HomogenousMatrixT4<T>::translation() const
{
	ocean_assert(isValid());
	return VectorT3<T>(values_ + 12);
}

template <typename T>
QuaternionT<T> HomogenousMatrixT4<T>::rotation() const
{
	ocean_assert(isValid());
	return QuaternionT<T>(*this);
}

template <typename T>
VectorT3<T> HomogenousMatrixT4<T>::scale() const
{
	ocean_assert(isValid());

	const VectorT3<T> x(values_);
	const VectorT3<T> y(values_ + 4);
	const VectorT3<T> z(values_ + 8);

	return VectorT3<T>(x.length(), y.length(), z.length());
}

template <typename T>
bool HomogenousMatrixT4<T>::decompose(VectorT3<T>& translation, QuaternionT<T>& rotation, VectorT3<T>& scale, VectorT3<T>& shear) const
{
	ocean_assert(isValid());

	translation = VectorT3<T>(values_ + 12);

	VectorT3<T> xAxis(values_);
	VectorT3<T> yAxis(values_ + 4);
	VectorT3<T> zAxis(values_ + 8);

	// x scale factor and normalization of x-axis
	scale.x() = xAxis.length();
	if (NumericT<T>::isEqualEps(scale.x()))
	{
		return false;
	}

	xAxis /= scale.x();

	// xy shear factor
	shear(0) = xAxis * yAxis;
	// make y-axis orthogonal to x-axis
	yAxis -= xAxis * shear(0);

	// y scale factor and normalization of y-axis
	scale.y() = yAxis.length();
	if (NumericT<T>::isEqualEps(scale.y()))
	{
		return false;
	}

	const T invScaleY = T(1) / scale.y();

	yAxis *= invScaleY;

	// normalization of xy shear factor
	shear(0) *= invScaleY;

	// xz shear factor and orthogonalization of z-axis
	shear(1) = xAxis * zAxis;
	zAxis -= xAxis * shear(1);

	// yz shear factor and orthogonalization of z-axis
	shear(2) = yAxis * zAxis;
	zAxis -= yAxis * shear(2);

	// z scale factor and normalization of z-axis
	scale.z() = zAxis.length();

	if (NumericT<T>::isEqualEps(scale.z()))
	{
		return false;
	}

	const T invScaleZ = T(1) / scale.z();

	zAxis *= invScaleZ;

	// normalization of xz shear and yz shear
	shear(1) *= invScaleZ;
	shear(2) *= invScaleZ;

	// check for a coordinate system flip
	if (xAxis * yAxis.cross(zAxis) < 0)
	{
		scale = -scale;
		xAxis = -xAxis;
		yAxis = -yAxis;
		zAxis = -zAxis;
	}

	// no we have pairwise orthogonal base vectors

#ifdef OCEAN_DEBUG
	const T epsilon = std::is_same<T, float>::value ? NumericT<T>::weakEps() : NumericT<T>::eps();
	ocean_assert(NumericT<T>::isEqual(xAxis * yAxis, 0, epsilon));
	ocean_assert(NumericT<T>::isEqual(xAxis * zAxis, 0, epsilon));
	ocean_assert(NumericT<T>::isEqual(yAxis * zAxis, 0, epsilon));
#endif

	rotation = QuaternionT<T>(SquareMatrixT3<T>(xAxis, yAxis, zAxis));

	return true;
}

template <typename T>
inline SquareMatrixT3<T> HomogenousMatrixT4<T>::rotationMatrix() const
{
	ocean_assert(isValid());

	return SquareMatrixT3<T>(values_[0], values_[1], values_[2],
							values_[4], values_[5], values_[6],
							values_[8], values_[9], values_[10]);
}

template <typename T>
inline void HomogenousMatrixT4<T>::rotationMatrix(T* data, const bool transposed) const
{
	ocean_assert(data);

	if (transposed)
	{
		data[0] = values_[0];
		data[1] = values_[4];
		data[2] = values_[8];

		data[3] = values_[1];
		data[4] = values_[5];
		data[5] = values_[9];

		data[6] = values_[2];
		data[7] = values_[6];
		data[8] = values_[10];
	}
	else
	{
		data[0] = values_[0];
		data[1] = values_[1];
		data[2] = values_[2];

		data[3] = values_[4];
		data[4] = values_[5];
		data[5] = values_[6];

		data[6] = values_[8];
		data[7] = values_[9];
		data[8] = values_[10];
	}
}

template <typename T>
SquareMatrixT3<T> HomogenousMatrixT4<T>::orthonormalRotationMatrix() const
{
	ocean_assert(isValid());
	return rotationMatrix().orthonormalMatrix();
}

template <typename T>
SquareMatrixT4<T> HomogenousMatrixT4<T>::transposed() const
{
	ocean_assert(isValid());

	SquareMatrixT4<T> result(*this);

	result[1] = values_[4];
	result[4] = values_[1];

	result[2] = values_[8];
	result[8] = values_[2];

	result[3] = values_[12];
	result[12] = values_[3];

	result[7] = values_[13];
	result[13] = values_[7];

	result[11] = values_[14];
	result[14] = values_[11];

	result[6] = values_[9];
	result[9] = values_[6];

	ocean_assert(result == SquareMatrixT4<T>(*this).transposed());

	return result;
}

template <typename T>
HomogenousMatrixT4<T> HomogenousMatrixT4<T>::inverted() const noexcept
{
	ocean_assert(isValid());

	HomogenousMatrixT4<T> invertedMatrix;

	if (!invert(invertedMatrix))
	{
		ocean_assert(false && "Could not invert the matrix.");
		return *this;
	}

	return invertedMatrix;
}

template <typename T>
bool HomogenousMatrixT4<T>::invert()
{
	HomogenousMatrixT4<T> invertedMatrix;

	if (!invert(invertedMatrix))
	{
		return false;
	}

	*this = invertedMatrix;

	return true;
}

template <typename T>
bool HomogenousMatrixT4<T>::invert(HomogenousMatrixT4<T>& invertedMatrix) const
{
	ocean_assert(isValid());

	const T det = determinant();

	if (NumericT<T>::isEqualEps(det))
	{
		return false;
	}

	const T factor = T(1.0) / det;

	invertedMatrix.values_[0] = (values_[5] * values_[10] - values_[6] * values_[9]) * factor;
	invertedMatrix.values_[1] = (values_[2] * values_[9] - values_[1] * values_[10]) * factor;
	invertedMatrix.values_[2] = (values_[1] * values_[6] - values_[2] * values_[5]) * factor;
	invertedMatrix.values_[4] = (values_[8] * values_[6] - values_[4] * values_[10]) * factor;
	invertedMatrix.values_[5] = (values_[0] * values_[10] - values_[8] * values_[2]) * factor;
	invertedMatrix.values_[6] = (values_[4] * values_[2] - values_[0] * values_[6]) * factor;
	invertedMatrix.values_[8] = (values_[4] * values_[9] - values_[8] * values_[5]) * factor;
	invertedMatrix.values_[9] = (values_[8] * values_[1] - values_[0] * values_[9]) * factor;
	invertedMatrix.values_[10] = (values_[0] * values_[5] - values_[4] * values_[1]) * factor;

	invertedMatrix.values_[12] = -(invertedMatrix.values_[0] * values_[12] + invertedMatrix.values_[4] * values_[13] + invertedMatrix.values_[8] * values_[14]);
	invertedMatrix.values_[13] = -(invertedMatrix.values_[1] * values_[12] + invertedMatrix.values_[5] * values_[13] + invertedMatrix.values_[9] * values_[14]);
	invertedMatrix.values_[14] = -(invertedMatrix.values_[2] * values_[12] + invertedMatrix.values_[6] * values_[13] + invertedMatrix.values_[10] * values_[14]);

	invertedMatrix.values_[3] = T(0.0);
	invertedMatrix.values_[7] = T(0.0);
	invertedMatrix.values_[11] = T(0.0);
	invertedMatrix.values_[15] = T(1.0);

	return true;
}

template <typename T>
T HomogenousMatrixT4<T>::determinant() const
{
	return values_[0] * (values_[5] * values_[10] - values_[6] * values_[9])
			+ values_[1] * (values_[6] * values_[8] - values_[4] * values_[10])
			+ values_[2] * (values_[4] * values_[9] - values_[5] * values_[8]);
}

template <typename T>
inline T HomogenousMatrixT4<T>::trace() const
{
	ocean_assert(isValid());
	return values_[0] + values_[5] + values_[10] + T(1.0);
}

template <typename T>
inline HomogenousMatrixT4<T>& HomogenousMatrixT4<T>::setTranslation(const VectorT3<T>& translation)
{
	values_[12] = translation[0];
	values_[13] = translation[1];
	values_[14] = translation[2];

	return *this;
}

template <typename T>
HomogenousMatrixT4<T>& HomogenousMatrixT4<T>::setRotation(const RotationT<T>& rotation)
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

	values_[4] = xy - nz;
	values_[5] = yy + cosValue;
	values_[6] = yz + nx;

	values_[8] = xz + ny;
	values_[9] = yz - nx;
	values_[10] = zz + cosValue;

	ocean_assert(isValid() && NumericT<T>::isEqual(determinant(), T(1.0)));
	return *this;
}

template <typename T>
HomogenousMatrixT4<T>& HomogenousMatrixT4<T>::setRotation(const QuaternionT<T>& quaternion)
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

	values_[ 0] = T(1.0) - T(2.0) * (yy + zz);
	values_[ 1] = T(2.0) * (wz + xy);
	values_[ 2] = T(2.0) * (xz - wy);

	values_[ 4] = T(2.0) * (xy - wz);
	values_[ 5] = T(1.0) - T(2.0) * (xx + zz);
	values_[ 6] = T(2.0) * (wx + yz);

	values_[ 8] = T(2.0) * (wy + xz);
	values_[ 9] = T(2.0) * (yz - wx);
	values_[10] = T(1.0) - T(2.0) * (xx + yy);

	ocean_assert(isValid() && NumericT<T>::isWeakEqual(determinant(), T(1.0)));
	return *this;
}

template <typename T>
HomogenousMatrixT4<T>& HomogenousMatrixT4<T>::setRotation(const SquareMatrixT3<T>& matrix)
{
	memcpy(values_, matrix(), sizeof(T) * 3);
	memcpy(values_ + 4, matrix() + 3, sizeof(T) * 3);
	memcpy(values_ + 8, matrix() + 6, sizeof(T) * 3);

	return *this;
}

template <typename T>
HomogenousMatrixT4<T>& HomogenousMatrixT4<T>::applyScale(const VectorT3<T>& scale)
{
	ocean_assert(isValid());

	values_[0] *= scale(0);
	values_[1] *= scale(0);
	values_[2] *= scale(0);

	values_[4] *= scale(1);
	values_[5] *= scale(1);
	values_[6] *= scale(1);

	values_[8] *= scale(2);
	values_[9] *= scale(2);
	values_[10] *= scale(2);

	return *this;
}

template <typename T>
inline void HomogenousMatrixT4<T>::toIdentity()
{
	values_[ 0] = T(1.0);
	values_[ 1] = T(0.0);
	values_[ 2] = T(0.0);
	values_[ 3] = T(0.0);

	values_[ 4] = T(0.0);
	values_[ 5] = T(1.0);
	values_[ 6] = T(0.0);
	values_[ 7] = T(0.0);

	values_[ 8] = T(0.0);
	values_[ 9] = T(0.0);
	values_[10] = T(1.0);
	values_[11] = T(0.0);

	values_[12] = T(0.0);
	values_[13] = T(0.0);
	values_[14] = T(0.0);
	values_[15] = T(1.0);

	ocean_assert(isValid());
}

template <typename T>
inline void HomogenousMatrixT4<T>::toNull()
{
	for (unsigned int n = 0u; n < 16u; ++n)
	{
		values_[n] = T(0);
	}

	ocean_assert(!isValid());
}

template <typename T>
bool HomogenousMatrixT4<T>::isValid() const
{
	return NumericT<T>::isEqualEps(values_[3]) && NumericT<T>::isEqualEps(values_[7]) && NumericT<T>::isEqualEps(values_[11]) && NumericT<T>::isEqual(values_[15], T(1.0));
}

template <typename T>
bool HomogenousMatrixT4<T>::isIdentity() const
{
	return NumericT<T>::isEqual(values_[0], 1) && NumericT<T>::isEqualEps(values_[1]) && NumericT<T>::isEqualEps(values_[2]) && NumericT<T>::isEqualEps(values_[3])
				&& NumericT<T>::isEqualEps(values_[4]) && NumericT<T>::isEqual(values_[5], 1) && NumericT<T>::isEqualEps(values_[6]) && NumericT<T>::isEqualEps(values_[7])
				&& NumericT<T>::isEqualEps(values_[8]) && NumericT<T>::isEqualEps(values_[9]) && NumericT<T>::isEqual(values_[10], 1) && NumericT<T>::isEqualEps(values_[11])
				&& NumericT<T>::isEqualEps(values_[12]) && NumericT<T>::isEqualEps(values_[13]) && NumericT<T>::isEqualEps(values_[14]) && NumericT<T>::isEqual(values_[15], 1);
}

template <typename T>
inline bool HomogenousMatrixT4<T>::isEqual(const HomogenousMatrixT4<T>& matrix, const T epsilon) const
{
	return NumericT<T>::isEqual(values_[0], matrix.values_[0], epsilon) && NumericT<T>::isEqual(values_[1], matrix.values_[1], epsilon)
			&& NumericT<T>::isEqual(values_[2], matrix.values_[2], epsilon) && NumericT<T>::isEqual(values_[3], matrix.values_[3], epsilon)
			&& NumericT<T>::isEqual(values_[4], matrix.values_[4], epsilon) && NumericT<T>::isEqual(values_[5], matrix.values_[5], epsilon)
			&& NumericT<T>::isEqual(values_[6], matrix.values_[6], epsilon) && NumericT<T>::isEqual(values_[7], matrix.values_[7], epsilon)
			&& NumericT<T>::isEqual(values_[8], matrix.values_[8], epsilon) && NumericT<T>::isEqual(values_[9], matrix.values_[9], epsilon)
			&& NumericT<T>::isEqual(values_[10], matrix.values_[10], epsilon) && NumericT<T>::isEqual(values_[11], matrix.values_[11], epsilon)
			&& NumericT<T>::isEqual(values_[12], matrix.values_[12], epsilon) && NumericT<T>::isEqual(values_[13], matrix.values_[13], epsilon)
			&& NumericT<T>::isEqual(values_[14], matrix.values_[14], epsilon) && NumericT<T>::isEqual(values_[15], matrix.values_[15], epsilon);
}

template <typename T>
bool HomogenousMatrixT4<T>::isNull() const
{
	return NumericT<T>::isEqualEps(values_[0]) && NumericT<T>::isEqualEps(values_[1]) && NumericT<T>::isEqualEps(values_[2]) && NumericT<T>::isEqualEps(values_[3])
				&& NumericT<T>::isEqualEps(values_[4]) && NumericT<T>::isEqualEps(values_[5]) && NumericT<T>::isEqualEps(values_[6]) && NumericT<T>::isEqualEps(values_[7])
				&& NumericT<T>::isEqualEps(values_[8]) && NumericT<T>::isEqualEps(values_[9]) && NumericT<T>::isEqualEps(values_[10]) && NumericT<T>::isEqualEps(values_[11])
				&& NumericT<T>::isEqualEps(values_[12]) && NumericT<T>::isEqualEps(values_[13]) && NumericT<T>::isEqualEps(values_[14]) && NumericT<T>::isEqualEps(values_[15]);
}

template <typename T>
inline const T* HomogenousMatrixT4<T>::data() const
{
	return values_;
}

template <typename T>
inline T* HomogenousMatrixT4<T>::data()
{
	return values_;
}

template <typename T>
inline void HomogenousMatrixT4<T>::copyElements(T* arrayValues, const bool valuesRowAligned) const
{
	ocean_assert(arrayValues != nullptr);

	if (valuesRowAligned)
	{
		// this matrix and the provided array are both column aligned
		// thus, we can simply copy the data

		arrayValues[ 0] = values_[ 0];
		arrayValues[ 1] = values_[ 4];
		arrayValues[ 2] = values_[ 8];
		arrayValues[ 3] = values_[12];

		arrayValues[ 4] = values_[ 1];
		arrayValues[ 5] = values_[ 5];
		arrayValues[ 6] = values_[ 9];
		arrayValues[ 7] = values_[13];

		arrayValues[ 8] = values_[ 2];
		arrayValues[ 9] = values_[ 6];
		arrayValues[10] = values_[10];
		arrayValues[11] = values_[14];

		arrayValues[12] = values_[ 3];
		arrayValues[13] = values_[ 7];
		arrayValues[14] = values_[11];
		arrayValues[15] = values_[15];
	}
	else
	{
		// this matrix and the provided array are both column aligned
		// thus, we can simply copy the data

		memcpy(arrayValues, values_, sizeof(T) * 16);
	}
}

template <typename T>
template <typename U>
inline void HomogenousMatrixT4<T>::copyElements(U* arrayValues, const bool valuesRowAligned) const
{
	ocean_assert(arrayValues != nullptr);

	if (valuesRowAligned)
	{
		// the provided buffer is row aligned (this matrix is column aligned)
		// thus, we have to assign the values in a transposed manner

		arrayValues[ 0] = U(values_[ 0]);
		arrayValues[ 1] = U(values_[ 4]);
		arrayValues[ 2] = U(values_[ 8]);
		arrayValues[ 3] = U(values_[12]);

		arrayValues[ 4] = U(values_[ 1]);
		arrayValues[ 5] = U(values_[ 5]);
		arrayValues[ 6] = U(values_[ 9]);
		arrayValues[ 7] = U(values_[13]);

		arrayValues[ 8] = U(values_[ 2]);
		arrayValues[ 9] = U(values_[ 6]);
		arrayValues[10] = U(values_[10]);
		arrayValues[11] = U(values_[14]);

		arrayValues[12] = U(values_[ 3]);
		arrayValues[13] = U(values_[ 7]);
		arrayValues[14] = U(values_[11]);
		arrayValues[15] = U(values_[15]);
	}
	else
	{
		// this matrix and the provided array are both column aligned
		// thus, we can simply copy the data

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			arrayValues[n] =  U(values_[n]);
		}
	}
}

template <typename T>
inline VectorT3<T> HomogenousMatrixT4<T>::rotationMatrix(const VectorT3<T>& vector) const
{
	ocean_assert(isValid());

	return VectorT3<T>(values_[0] * vector[0] + values_[4] * vector[1] + values_[8] * vector[2],
					values_[1] * vector[0] + values_[5] * vector[1] + values_[9] * vector[2],
					values_[2] * vector[0] + values_[6] * vector[1] + values_[10] * vector[2]);
}

template <typename T>
inline VectorT3<T> HomogenousMatrixT4<T>::transposedRotationMatrix(const VectorT3<T>& vector) const
{
	ocean_assert(isValid());

	return VectorT3<T>(values_[0] * vector[0] + values_[1] * vector[1] + values_[2] * vector[2],
					values_[4] * vector[0] + values_[5] * vector[1] + values_[6] * vector[2],
					values_[8] * vector[0] + values_[9] * vector[1] + values_[10] * vector[2]);
}

template <typename T>
bool HomogenousMatrixT4<T>::operator==(const HomogenousMatrixT4<T>& matrix) const
{
	return isEqual(matrix);
}

template <typename T>
inline bool HomogenousMatrixT4<T>::operator!=(const HomogenousMatrixT4<T>& matrix) const
{
	return !(*this == matrix);
}

template <typename T>
HomogenousMatrixT4<T> HomogenousMatrixT4<T>::operator*(const HomogenousMatrixT4<T>& matrix) const
{
	ocean_assert(isValid() && matrix.isValid());

	HomogenousMatrixT4<T> result;

	result.values_[0] = values_[0] * matrix.values_[0] + values_[4] * matrix.values_[1] + values_[8] * matrix.values_[2] + values_[12] * matrix.values_[3];
	result.values_[1] = values_[1] * matrix.values_[0] + values_[5] * matrix.values_[1] + values_[9] * matrix.values_[2] + values_[13] * matrix.values_[3];
	result.values_[2] = values_[2] * matrix.values_[0] + values_[6] * matrix.values_[1] + values_[10] * matrix.values_[2] + values_[14] * matrix.values_[3];
	result.values_[3] = T(0.0);

	result.values_[4] = values_[0] * matrix.values_[4] + values_[4] * matrix.values_[5] + values_[8] * matrix.values_[6] + values_[12] * matrix.values_[7];
	result.values_[5] = values_[1] * matrix.values_[4] + values_[5] * matrix.values_[5] + values_[9] * matrix.values_[6] + values_[13] * matrix.values_[7];
	result.values_[6] = values_[2] * matrix.values_[4] + values_[6] * matrix.values_[5] + values_[10] * matrix.values_[6] + values_[14] * matrix.values_[7];
	result.values_[7] = T(0.0);

	result.values_[8] = values_[0] * matrix.values_[8] + values_[4] * matrix.values_[9] + values_[8] * matrix.values_[10] + values_[12] * matrix.values_[11];
	result.values_[9] = values_[1] * matrix.values_[8] + values_[5] * matrix.values_[9] + values_[9] * matrix.values_[10] + values_[13] * matrix.values_[11];
	result.values_[10] = values_[2] * matrix.values_[8] + values_[6] * matrix.values_[9] + values_[10] * matrix.values_[10] + values_[14] * matrix.values_[11];
	result.values_[11] = T(0.0);

	result.values_[12] = values_[0] * matrix.values_[12] + values_[4] * matrix.values_[13] + values_[8] * matrix.values_[14] + values_[12] * matrix.values_[15];
	result.values_[13] = values_[1] * matrix.values_[12] + values_[5] * matrix.values_[13] + values_[9] * matrix.values_[14] + values_[13] * matrix.values_[15];
	result.values_[14] = values_[2] * matrix.values_[12] + values_[6] * matrix.values_[13] + values_[10] * matrix.values_[14] + values_[14] * matrix.values_[15];
	result.values_[15] = T(1.0);

	ocean_assert(result.isValid());

	return result;
}

template <typename T>
inline HomogenousMatrixT4<T>& HomogenousMatrixT4<T>::operator*=(const HomogenousMatrixT4<T>& matrix)
{
	*this = *this * matrix;
	return *this;
}

template <typename T>
HomogenousMatrixT4<T> HomogenousMatrixT4<T>::operator*(const RotationT<T>& rotation) const
{
	return *this * HomogenousMatrixT4<T>(rotation);
}


template <typename T>
inline HomogenousMatrixT4<T>& HomogenousMatrixT4<T>::operator*=(const RotationT<T>& rotation)
{
	*this = *this * rotation;
	return *this;
}

template <typename T>
HomogenousMatrixT4<T> HomogenousMatrixT4<T>::operator*(const QuaternionT<T>& rotation) const
{
	return *this * HomogenousMatrixT4<T>(rotation);
}

template <typename T>
inline HomogenousMatrixT4<T>& HomogenousMatrixT4<T>::operator*=(const QuaternionT<T>& rotation)
{
	*this = *this * rotation;
	return *this;
}

template <typename T>
inline VectorT3<T> HomogenousMatrixT4<T>::operator*(const VectorT3<T>& vector) const
{
	ocean_assert(isValid());

	return VectorT3<T>(values_[0] * vector[0] + values_[4] * vector[1] + values_[8] * vector[2] + values_[12],
					values_[1] * vector[0] + values_[5] * vector[1] + values_[9] * vector[2] + values_[13],
					values_[2] * vector[0] + values_[6] * vector[1] + values_[10] * vector[2] + values_[14]);
}

template <typename T>
inline VectorT4<T> HomogenousMatrixT4<T>::operator*(const VectorT4<T>& vector) const
{
	ocean_assert(isValid());

	return VectorT4<T>(values_[0] * vector[0] + values_[4] * vector[1] + values_[8] * vector[2] + values_[12] * vector[3],
					values_[1] * vector[0] + values_[5] * vector[1] + values_[9] * vector[2] + values_[13] * vector[3],
					values_[2] * vector[0] + values_[6] * vector[1] + values_[10] * vector[2] + values_[14] * vector[3],
					vector[3]);
}

template <typename T>
inline T HomogenousMatrixT4<T>::operator[](const unsigned int index) const
{
	ocean_assert(index < 16);
	return values_[index];
}

template <typename T>
inline T& HomogenousMatrixT4<T>::operator[](const unsigned int index)
{
	ocean_assert(index < 16u);
	return values_[index];
}

template <typename T>
inline T HomogenousMatrixT4<T>::operator()(const unsigned int row, const unsigned int column) const
{
	ocean_assert(row < 4u && column < 4u);
	return values_[column * 4u + row];
}

template <typename T>
inline T& HomogenousMatrixT4<T>::operator()(const unsigned int row, const unsigned int column)
{
	ocean_assert(row < 4u && column < 4u);
	return values_[column * 4u + row];
}

template <typename T>
inline T HomogenousMatrixT4<T>::operator()(const unsigned int index) const
{
	ocean_assert(index < 16u);
	return values_[index];
}

template <typename T>
inline T& HomogenousMatrixT4<T>::operator()(const unsigned int index)
{
	ocean_assert(index < 16u);
	return values_[index];
}

template <typename T>
inline const T* HomogenousMatrixT4<T>::operator()() const
{
	return values_;
}

template <typename T>
inline T* HomogenousMatrixT4<T>::operator()()
{
	return values_;
}

template <typename T>
inline size_t HomogenousMatrixT4<T>::operator()(const HomogenousMatrixT4<T>& matrix) const
{
	// we skip the value of the lower matrix row, as these values are always [0, 0, 0, 1]

	size_t seed = std::hash<T>{}(matrix.values_[0]);
	seed ^= std::hash<T>{}(matrix.values_[1]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(matrix.values_[2]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	seed ^= std::hash<T>{}(matrix.values_[4]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(matrix.values_[5]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(matrix.values_[6]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	seed ^= std::hash<T>{}(matrix.values_[8]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(matrix.values_[9]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(matrix.values_[10]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	seed ^= std::hash<T>{}(matrix.values_[12]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(matrix.values_[13]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(matrix.values_[14]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

template <typename T>
template <typename U>
inline std::vector< HomogenousMatrixT4<T> > HomogenousMatrixT4<T>::matrices2matrices(const std::vector< HomogenousMatrixT4<U> >& matrices)
{
	std::vector< HomogenousMatrixT4<T> > result;
	result.reserve(matrices.size());

	for (typename std::vector< HomogenousMatrixT4<U> >::const_iterator i = matrices.begin(); i != matrices.end(); ++i)
	{
		result.push_back(HomogenousMatrixT4<T>(*i));
	}

	return result;
}

template <>
template <>
inline std::vector< HomogenousMatrixT4<float> > HomogenousMatrixT4<float>::matrices2matrices(const std::vector< HomogenousMatrixT4<float> >& matrices)
{
	return matrices;
}

template <>
template <>
inline std::vector< HomogenousMatrixT4<double> > HomogenousMatrixT4<double>::matrices2matrices(const std::vector< HomogenousMatrixT4<double> >& matrices)
{
	return matrices;
}

template <typename T>
template <typename U>
inline std::vector< HomogenousMatrixT4<T> > HomogenousMatrixT4<T>::matrices2matrices(const HomogenousMatrixT4<U>* matrices, const size_t size)
{
	std::vector< HomogenousMatrixT4<T> > result;
	result.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		result.push_back(HomogenousMatrixT4<T>(matrices[n]));
	}

	return result;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const HomogenousMatrixT4<T>& matrix)
{
	stream << "|" << matrix(0, 0) << ", " << matrix(0, 1) << ", " << matrix(0, 2) << ", " << matrix(0, 3) << "|" << std::endl;
	stream << "|" << matrix(1, 0) << ", " << matrix(1, 1) << ", " << matrix(1, 2) << ", " << matrix(1, 3) << "|" << std::endl;
	stream << "|" << matrix(2, 0) << ", " << matrix(2, 1) << ", " << matrix(2, 2) << ", " << matrix(2, 3) << "|" << std::endl;
	stream << "|" << matrix(3, 0) << ", " << matrix(3, 1) << ", " << matrix(3, 2) << ", " << matrix(3, 3) << "|";

	return stream;
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const HomogenousMatrixT4<T>& matrix)
{
	return messageObject << "|" << matrix(0, 0) << ", " << matrix(0, 1) << ", " << matrix(0, 2) << ", " << matrix(0, 3) << "|\n|"
							<< matrix(1, 0) << ", " << matrix(1, 1) << ", " << matrix(1, 2) << ", " << matrix(1, 3) << "|\n|"
							<< matrix(2, 0) << ", " << matrix(2, 1) << ", " << matrix(2, 2) << ", " << matrix(2, 3) << "|\n|"
							<< matrix(3, 0) << ", " << matrix(3, 1) << ", " << matrix(3, 2) << ", " << matrix(3, 3) << "|";
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const HomogenousMatrixT4<T>& matrix)
{
	return messageObject << "|" << matrix(0, 0) << ", " << matrix(0, 1) << ", " << matrix(0, 2) << ", " << matrix(0, 3) << "|\n|"
							<< matrix(1, 0) << ", " << matrix(1, 1) << ", " << matrix(1, 2) << ", " << matrix(1, 3) << "|\n|"
							<< matrix(2, 0) << ", " << matrix(2, 1) << ", " << matrix(2, 2) << ", " << matrix(2, 3) << "|\n|"
							<< matrix(3, 0) << ", " << matrix(3, 1) << ", " << matrix(3, 2) << ", " << matrix(3, 3) << "|";
}

}

#endif // META_OCEAN_MATH_HOMOGENOUS_MATRIX_4_H
