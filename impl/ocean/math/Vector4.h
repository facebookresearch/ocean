/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_VECTOR4_H
#define META_OCEAN_MATH_VECTOR4_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Vector3.h"

#include <type_traits>
#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class VectorT4;

/**
 * Definition of a 4D vector.
 * @see VectorT4
 * @ingroup math
 */
typedef VectorT4<Scalar> Vector4;

/**
 * Definition of a 4D vector with double values.
 * @see VectorT4
 * @ingroup math
 */
typedef VectorT4<double> VectorD4;

/**
 * Definition of a 4D vector with float values.
 * @see VectorT4
 * @ingroup math
 */
typedef VectorT4<float> VectorF4;

/**
 * Definition of a 4D vector with integer values.
 * @see VectorT4
 * @ingroup math
 */
typedef VectorT4<int> VectorI4;

/**
 * Definition of a typename alias for vectors with VectorT4 objects.
 * @see VectorT4
 * @ingroup math
 */
template <typename T>
using VectorsT4 = std::vector<VectorT4<T>>;

/**
 * Definition of a vector holding Vector4 objects.
 * @see Vector4
 * @ingroup math
 */
typedef std::vector<Vector4> Vectors4;

/**
 * Definition of a vector holding VectorD4 objects.
 * @see VectorD4
 * @ingroup math
 */
typedef std::vector<VectorD4> VectorsD4;

/**
 * Definition of a vector holding VectorF4 objects.
 * @see VectorF4
 * @ingroup math
 */
typedef std::vector<VectorF4> VectorsF4;

/**
 * Definition of a vector holding VectorI4 objects.
 * @see VectorI4
 * @ingroup math
 */
typedef std::vector<VectorI4> VectorsI4;

/**
 * This class implements a vector with four elements.
 * The element order is: (x, y, z, w).
 * @tparam T Data type of the vector elements.
 * @see Vector4, VectorF4, VectorD4.
 * @ingroup math
 */
template <typename T>
class VectorT4
{
	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates a new 2D vector with zeros as default values.
		 */
		VectorT4() = default;

		/**
		 * Creates a new 4D vector with four given elements.
		 * @param x X value
		 * @param y Y value
		 * @param z Z value
		 * @param w W value
		 */
		inline VectorT4(const T& x, const T& y, const T& z, const T& w) noexcept;

		/**
		 * Creates a new homogeneous 4D vector by a 3D vector.
		 * The fourth parameter will be 1.
		 * @param vector The vector for the first three elements
		 */
		inline explicit VectorT4(const VectorT3<T>& vector) noexcept;

		/**
		 * Creates a new 4D vector by a 3D vector and a following scalar.
		 * @param vector The vector for the first three elements
		 * @param w Scalar value for the last element
		 */
		inline VectorT4(const VectorT3<T>& vector, const T& w) noexcept;

		/**
		 * Creates a new 4D vector with a given array of elements.
		 * @param valueArray Array with at least four elements
		 */
		inline explicit VectorT4(const T* valueArray) noexcept;

		/**
		 * Copies a vector.
		 * @param vector 4D vector that is copied
		 */
		inline VectorT4(const VectorT4<T>& vector) noexcept;

		/**
		 * Copies a vector with different element data type than T.
		 * @param vector The 4D vector to copy
		 * @tparam U The element data type of the second vector
		 */
		template <typename U>
		inline explicit VectorT4(const VectorT4<U>& vector) noexcept;

		/**
		 * Returns the normalized vector.
		 * Beware: This function does not throw an exception if this vector cannot be normalized.<br>
		 * Thus, ensure that this vector is not zero before calling this function.<br>
		 * Or even better, use different normalization functions like: normalizedOrZero(), normalizedOrValue(), or normalize().<br>
		 * In case, the vector cannot be normalized, an uninitialized vector will be returned (due to performance reasons).
		 * @return This vector as unit vector (vector with length 1)
		 * @see normalizedOrZero(), normalizedOrValue(), normalize().
		 */
		inline VectorT4<T> normalized() const;

		/**
		 * Returns the normalized vector.
		 * If this vector cannot be normalized the zero vector is returned.
		 * @return Vector with length 1
		 */
		inline VectorT4<T> normalizedOrZero() const;

		/**
		 * Returns the normalized vector.
		 * If this vector cannot be normalized the given vector is returned.
		 * @param value Vector that will be returned if the vector cannot be normalized
		 * @return Vector with length 1
		 */
		inline VectorT4<T> normalizedOrValue(const VectorT4<T>& value) const;

		/**
		 * Normalizes this vector.
		 * @return True, if the vector could be normalized
		 */
		inline bool normalize();

		/**
		 * Returns the length of the vector.
		 * @return Vector length
		 */
		inline T length() const;

		/**
		 * Returns the square of the vector length.
		 * @return Square of vector length
		 */
		inline T sqr() const;

		/**
		 * Returns the angle between this vector and a second vectors.
		 * Beware: This vector must not be zero.<br>
		 * Beware: This function does not throw an exception if one or both vectors are zero.<br>
		 * In case, the angle cannot be determined -1 will be returned.
		 * @param right Second vector, must not be zero
		 * @return Angle between both vectors in radian, with range [0, PI], -1 in case of an error
		 */
		T angle(const VectorT4<T>& right) const;

		/**
		 * Returns whether two vectors are parallel.
		 * A zero vector will not be parallel.<br>
		 * @param right The right vector
		 * @return True, if so
		 */
		bool isParallel(const VectorT4<T>& right) const;

		/**
		 * Returns whether two vectors are orthogonal.
		 * A zero vector will not be orthogonal.<br>
		 * @param right The right vector
		 * @return True, if so
		 */
		bool isOrthogonal(const VectorT4<T>& right) const;

		/**
		 * Returns the x value.
		 * @return X value
		 */
		inline const T& x() const noexcept;

		/**
		 * Returns the x value.
		 * @return X value
		 */
		inline T& x() noexcept;

		/**
		 * Returns the y value.
		 * @return Y value
		 */
		inline const T& y() const noexcept;

		/**
		 * Returns the y value.
		 * @return Y value
		 */
		inline T& y() noexcept;

		/**
		 * Returns the z value.
		 * @return Z value
		 */
		inline const T& z() const noexcept;

		/**
		 * Returns the z value.
		 * @return Z value
		 */
		inline T& z() noexcept;

		/**
		 * Returns the w value.
		 * @return W value
		 */
		inline const T& w() const noexcept;

		/**
		 * Returns the w value.
		 * @return W value
		 */
		inline T& w() noexcept;

		/**
		 * Returns the x and y component of the vector as new 2D vector.
		 * @return New 2D vector
		 */
		inline VectorT2<T> xy() const noexcept;

		/**
		 * Returns the x, y and z component of the vector as new 3D vector.
		 * @return New 3D vector
		 */
		inline VectorT3<T> xyz() const noexcept;

		/**
		 * Returns an pointer to the vector elements.
		 * @return Pointer to elements
		 */
		inline const T* data() const noexcept;

		/**
		 * Returns an pointer to the vector elements.
		 * @return Pointer to elements
		 */
		inline T* data() noexcept;

		/**
		 * Returns whether this vector is a null vector up to a small epsilon.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Returns whether this vector is a unit vector (whether the vector has the length 1).
		 * @param eps Epsilon to be used, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isUnit(const T eps = NumericT<T>::eps()) const;

		/**
		 * Returns whether two vectors are equal up to a specified epsilon.
		 * @param vector Second vector
		 * @param eps Epsilon to be used, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isEqual(const VectorT4<T>& vector, const T eps) const;

		/**
		 * Copy assigns a vector
		 * @param vector 4D vector that is copied
		 */
		inline VectorT4<T>& operator=(const VectorT4<T>& vector);

		/**
		 * Returns whether two vectors are identical up to a small epsilon.
		 * @param vector Right vector
		 * @return True, if so
		 */
		inline bool operator==(const VectorT4<T>& vector) const;

		/**
		 * Returns whether two vectors are not identical up to a small epsilon.
		 * @param vector Right vector
		 * @return True, if so
		 */
		inline bool operator!=(const VectorT4<T>& vector) const;

		/**
		 * Adds two vectors.
		 * @param vector Right vector
		 * @return Sum vector
		 */
		inline VectorT4<T> operator+(const VectorT4<T>& vector) const;

		/**
		 * Adds and assigns two vectors.
		 * @param vector Right vector
		 * @return Reference to this vector
		 */
		inline VectorT4<T>& operator+=(const VectorT4<T>& vector);

		/**
		 * Subtracts two vectors.
		 * @param vector Right vector
		 * @return Difference vector
		 */
		inline VectorT4<T> operator-(const VectorT4<T>& vector) const;

		/**
		 * Subtracts and assigns two vectors.
		 * @param vector Right vector
		 * @return Reference to this vector
		 */
		inline VectorT4<T>& operator-=(const VectorT4<T>& vector);

		/**
		 * Returns the negated vector.
		 * @return Negated vector
		 */
		inline VectorT4<T> operator-() const;

		/**
		 * Returns the dot product of two vectors.
		 * @param vector Right vector
		 * @return Dot product
		 */
		inline T operator*(const VectorT4<T>& vector) const;

		/**
		 * Multiplies this vector with a scalar.
		 * @param value Scalar value
		 * @return Resulting vector
		 */
		inline VectorT4<T> operator*(const T& value) const;

		/**
		 * Multiplies and assigns this vector with a scalar.
		 * @param value Scalar value
		 * @return Reference to this vector
		 */
		inline VectorT4<T>& operator*=(const T& value);

		/**
		 * Divides this vector by a scalar.
		 * Beware: This function does not throw an exception if the given value is zero.<br>
		 * Thus, ensure that given value is not zero before calling this function.<br>
		 * In case, the given value is zero, the result is undefined.
		 * @param value Scalar value to be used as denominator, must not be zero
		 * @return Resulting vector
		 */
		inline VectorT4<T> operator/(const T& value) const;

		/**
		 * Divides and assigns this vector by a scalar.
		 * Beware: This function does not throw an exception if the given value is zero.<br>
		 * Thus, ensure that given value is not zero before calling this function.<br>
		 * In case, the given value is zero, the result is undefined.
		 * @param value Scalar value to be used as denominator, must not be zero
		 * @return Reference to this vector
		 */
		inline VectorT4<T>& operator/=(const T& value);

		/**
		 * Compares two vector objects and returns whether the left vector represents a smaller value than the right vector.
		 * First the first component of both vectors are compared, if these values are equal then the next components are compares and so on.<br>
		 * @param vector The second vector to compare
		 * @return True, if so
		 */
		inline bool operator<(const VectorT4<T>& vector) const;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 3]
		 * @return Element of the vector
		 */
		inline const T& operator[](const unsigned int index) const noexcept;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 3]
		 * @return Element of the vector
		 */
		inline T& operator[](const unsigned int index) noexcept;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 3]
		 * @return Element of the vector
		 */
		inline const T& operator()(const unsigned int index) const noexcept;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 3]
		 * @return Element of the vector
		 */
		inline T& operator()(const unsigned int index) noexcept;

		/**
		 * Access operator.
		 * @return Pointer to the elements
		 */
		inline const T* operator()() const noexcept;

		/**
		 * Access operator.
		 * @return Pointer to the elements
		 */
		inline T* operator()() noexcept;

		/**
		 * Hash function.
		 * @param vector The vector for which the hash value will be determined
		 * @return The resulting hash value
		 */
		inline size_t operator()(const VectorT4<T>& vector) const;

		/**
		 * Converts vectors with specific data type to vectors with different data type.
		 * @param vectors The vectors to convert
		 * @return The converted vectors
		 * @tparam U The element data type of the vectors to convert
		 */
		template <typename U>
		static inline std::vector<VectorT4<T>> vectors2vectors(std::vector<VectorT4<U>>&& vectors);

		/**
		 * Converts vectors with specific data type to vectors with different data type.
		 * @param vectors The vectors to convert
		 * @return The converted vectors
		 * @tparam U The element data type of the vectors to convert
		 */
		template <typename U>
		static inline std::vector<VectorT4<T>> vectors2vectors(const std::vector<VectorT4<U>>& vectors);

		/**
		 * Converts vectors with specific data type to vectors with different data type.
		 * @param vectors The vectors to convert
		 * @param size The number of vector to convert
		 * @return The converted vectors
		 * @tparam U The element data type of the vectors to convert
		 */
		template <typename U>
		static inline std::vector<VectorT4<T>> vectors2vectors(const VectorT4<U>* vectors, const size_t size);

	protected:

		/// The four values of the vector, with element order x, y, z, w.
		T values_[4] = {T(0), T(0), T(0), T(0)};
};

template <typename T>
inline VectorT4<T>::VectorT4(const T& x, const T& y, const T& z, const T& w) noexcept
{
	values_[0] = x;
	values_[1] = y;
	values_[2] = z;
	values_[3] = w;
}

template <typename T>
inline VectorT4<T>::VectorT4(const VectorT3<T>& vector) noexcept
{
	memcpy(values_, vector(), sizeof(T) * 3);
	values_[3] = T(1);
}

template <typename T>
inline VectorT4<T>::VectorT4(const VectorT3<T>& vector, const T& w) noexcept
{
	memcpy(values_, vector(), sizeof(T) * 3);
	values_[3] = w;
}

template <typename T>
inline VectorT4<T>::VectorT4(const T* valueArray) noexcept
{
	memcpy(values_, valueArray, sizeof(T) * 4);
}

template <typename T>
inline VectorT4<T>::VectorT4(const VectorT4<T>& vector) noexcept
{
	values_[0] = vector.values_[0];
	values_[1] = vector.values_[1];
	values_[2] = vector.values_[2];
	values_[3] = vector.values_[3];
}

template <typename T>
template <typename U>
inline VectorT4<T>::VectorT4(const VectorT4<U>& vector) noexcept
{
	values_[0] = T(vector[0]);
	values_[1] = T(vector[1]);
	values_[2] = T(vector[2]);
	values_[3] = T(vector[3]);
}

template <typename T>
inline VectorT4<T> VectorT4<T>::normalized() const
{
	T len = length();
	if (NumericT<T>::isEqualEps(len))
	{
		ocean_assert(false && "Division by zero!");
		return VectorT4<T>();
	}

	T factor = T(1) / len;
	return VectorT4<T>(values_[0] * factor, values_[1] * factor, values_[2] * factor, values_[3] * factor);
}

template <typename T>
inline VectorT4<T> VectorT4<T>::normalizedOrZero() const
{
	const T len = length();

	if (NumericT<T>::isEqualEps(len))
	{
		return *this;
	}

	const T factor = T(1) / len;
	return VectorT4<T>(values_[0] * factor, values_[1] * factor, values_[2] * factor, values_[3] * factor);
}

template <typename T>
inline VectorT4<T> VectorT4<T>::normalizedOrValue(const VectorT4<T>& value) const
{
	const T len = length();

	if (NumericT<T>::isEqualEps(len))
	{
		return value;
	}

	const T factor = T(1) / len;
	return VectorT4<T>(values_[0] * factor, values_[1] * factor, values_[2] * factor, values_[3] * factor);
}

template <typename T>
inline bool VectorT4<T>::normalize()
{
	const T len = length();
	if (NumericT<T>::isEqualEps(len))
	{
		return false;
	}

	const T factor = T(1) / len;
	values_[0] *= factor;
	values_[1] *= factor;
	values_[2] *= factor;
	values_[3] *= factor;
	return true;
}

template <typename T>
inline T VectorT4<T>::length() const
{
	return NumericT<T>::sqrt(values_[0] * values_[0] + values_[1] * values_[1] + values_[2] * values_[2] + values_[3] * values_[3]);
}

template <typename T>
inline T VectorT4<T>::sqr() const
{
	return values_[0] * values_[0] + values_[1] * values_[1] + values_[2] * values_[2] + values_[3] * values_[3];
}

template <typename T>
T VectorT4<T>::angle(const VectorT4<T>& right) const
{
	// a * b = cos * |a| * |b|
	// cos = (a * b) / (|a| * |b|)
	// we separate the sqrt determinations to receive a higher accuracy

	const T thisLength = length();
	const T rightLength = right.length();

	if (NumericT<T>::isEqualEps(thisLength) || NumericT<T>::isEqualEps(rightLength))
	{
		ocean_assert(false && "Invalid vector!");
		return T(-1);
	}

	const T dot = values_[0] * right.values_[0] + values_[1] * right.values_[1] + values_[2] * right.values_[2] + values_[3] * right.values_[3];

	return NumericT<T>::acos((dot / thisLength) / rightLength);
}

template <typename T>
bool VectorT4<T>::isParallel(const VectorT4<T>& right) const
{
	const VectorT4<T> normalizedThis(normalizedOrZero());
	const VectorT4<T> normalizedRight(right.normalizedOrZero());

	const T dotProduct = normalizedThis * normalizedRight;

	return NumericT<T>::isEqual(dotProduct, 1) || NumericT<T>::isEqual(dotProduct, -1);
}

template <typename T>
bool VectorT4<T>::isOrthogonal(const VectorT4<T>& right) const
{
	return NumericT<T>::isEqualEps(values_[0] * right.values_[0] + values_[1] * right.values_[1] + values_[2] * right.values_[2] + values_[3] * right.values_[3]);
}

template <typename T>
inline const T& VectorT4<T>::x() const noexcept
{
	return values_[0];
}

template <typename T>
inline T& VectorT4<T>::x() noexcept
{
	return values_[0];
}

template <typename T>
inline const T& VectorT4<T>::y() const noexcept
{
	return values_[1];
}

template <typename T>
inline T& VectorT4<T>::y() noexcept
{
	return values_[1];
}

template <typename T>
inline const T& VectorT4<T>::z() const noexcept
{
	return values_[2];
}

template <typename T>
inline T& VectorT4<T>::z() noexcept
{
	return values_[2];
}

template <typename T>
inline const T& VectorT4<T>::w() const noexcept
{
	return values_[3];
}

template <typename T>
inline T& VectorT4<T>::w() noexcept
{
	return values_[3];
}

template <typename T>
inline VectorT2<T> VectorT4<T>::xy() const noexcept
{
	return VectorT2<T>(values_[0], values_[1]);
}

template <typename T>
inline VectorT3<T> VectorT4<T>::xyz() const noexcept
{
	return VectorT3<T>(values_[0], values_[1], values_[2]);
}

template <typename T>
inline const T* VectorT4<T>::data() const noexcept
{
	return values_;
}

template <typename T>
inline T* VectorT4<T>::data() noexcept
{
	return values_;
}

template <typename T>
inline bool VectorT4<T>::isNull() const
{
	return NumericT<T>::isEqualEps(values_[0]) && NumericT<T>::isEqualEps(values_[1])
		&& NumericT<T>::isEqualEps(values_[2]) && NumericT<T>::isEqualEps(values_[3]);
}

template <typename T>
inline bool VectorT4<T>::isUnit(const T eps) const
{
	return NumericT<T>::isEqual(length(), T(1), eps);
}

template <typename T>
inline bool VectorT4<T>::isEqual(const VectorT4<T>& vector, const T eps) const
{
	return NumericT<T>::isEqual(values_[0], vector.values_[0], eps)
			&& NumericT<T>::isEqual(values_[1], vector.values_[1], eps)
			&& NumericT<T>::isEqual(values_[2], vector.values_[2], eps)
			&& NumericT<T>::isEqual(values_[3], vector.values_[3], eps);
}

template <typename T>
inline VectorT4<T>& VectorT4<T>::operator=(const VectorT4<T>& vector)
{
	if (this == &vector)
	{
		return *this;
	}

	values_[0] = vector.values_[0];
	values_[1] = vector.values_[1];
	values_[2] = vector.values_[2];
	values_[3] = vector.values_[3];

	return *this;
}

template <typename T>
inline bool VectorT4<T>::operator==(const VectorT4<T>& vector) const
{
	return NumericT<T>::isEqual(values_[0], vector.values_[0])
		&& NumericT<T>::isEqual(values_[1], vector.values_[1])
		&& NumericT<T>::isEqual(values_[2], vector.values_[2])
		&& NumericT<T>::isEqual(values_[3], vector.values_[3]);
}

template <typename T>
inline bool VectorT4<T>::operator!=(const VectorT4<T>& vector) const
{
	return NumericT<T>::isNotEqual(values_[0], vector.values_[0])
		|| NumericT<T>::isNotEqual(values_[1], vector.values_[1])
		|| NumericT<T>::isNotEqual(values_[2], vector.values_[2])
		|| NumericT<T>::isNotEqual(values_[3], vector.values_[3]);
}

template <typename T>
inline VectorT4<T> VectorT4<T>::operator+(const VectorT4<T>& vector) const
{
	return VectorT4<T>(values_[0] + vector.values_[0], values_[1] + vector.values_[1], values_[2] + vector.values_[2], values_[3] + vector.values_[3]);
}

template <typename T>
inline VectorT4<T>& VectorT4<T>::operator+=(const VectorT4<T>& vector)
{
	values_[0] += vector.values_[0];
	values_[1] += vector.values_[1];
	values_[2] += vector.values_[2];
	values_[3] += vector.values_[3];

	return *this;
}

template <typename T>
inline VectorT4<T> VectorT4<T>::operator-(const VectorT4<T>& vector) const
{
	return VectorT4<T>(values_[0] - vector.values_[0], values_[1] - vector.values_[1], values_[2] - vector.values_[2], values_[3] - vector.values_[3]);
}

template <typename T>
inline VectorT4<T>& VectorT4<T>::operator-=(const VectorT4<T>& vector)
{
	values_[0] -= vector.values_[0];
	values_[1] -= vector.values_[1];
	values_[2] -= vector.values_[2];
	values_[3] -= vector.values_[3];

	return *this;
}

template <typename T>
inline VectorT4<T> VectorT4<T>::operator-() const
{
	return VectorT4<T>(-values_[0], -values_[1], -values_[2], -values_[3]);
}

template <typename T>
inline T VectorT4<T>::operator*(const VectorT4<T>& vector) const
{
	return values_[0] * vector.values_[0] + values_[1] * vector.values_[1] + values_[2] * vector.values_[2] + values_[3] * vector.values_[3];
}

template <typename T>
inline VectorT4<T> VectorT4<T>::operator*(const T& value) const
{
	return VectorT4<T>(values_[0] * value, values_[1] * value, values_[2] * value, values_[3] * value);
}

template <typename T>
inline VectorT4<T>& VectorT4<T>::operator*=(const T& value)
{
	values_[0] *= value;
	values_[1] *= value;
	values_[2] *= value;
	values_[3] *= value;

	return *this;
}

template <typename T>
inline VectorT4<T> VectorT4<T>::operator/(const T& value) const
{
	ocean_assert(NumericT<T>::isNotEqualEps(value));
	const T factor = T(1) / value;

	return VectorT4<T>(values_[0] * factor, values_[1] * factor, values_[2] * factor, values_[3] * factor);
}

template <typename T>
inline VectorT4<T>& VectorT4<T>::operator/=(const T& value)
{
	ocean_assert(NumericT<T>::isNotEqualEps(value));

	const T factor = T(1) / value;

	values_[0] *= factor;
	values_[1] *= factor;
	values_[2] *= factor;
	values_[3] *= factor;

	return *this;
}

template <typename T>
inline bool VectorT4<T>::operator<(const VectorT4<T>& right) const
{
	return values_[0] < right.values_[0]
			|| (values_[0] == right.values_[0] && (values_[1] < right.values_[1]
			|| (values_[1] == right.values_[1] && (values_[2] < right.values_[2]
			|| (values_[2] == right.values_[2] && values_[3] < right.values_[3])))));
}

template <typename T>
inline const T& VectorT4<T>::operator[](const unsigned int index) const noexcept
{
	ocean_assert(index < 4u);
	return values_[index];
}

template <typename T>
inline T& VectorT4<T>::operator[](const unsigned int index) noexcept
{
	ocean_assert(index < 4u);
	return values_[index];
}

template <typename T>
inline const T& VectorT4<T>::operator()(const unsigned int index) const noexcept
{
	ocean_assert(index < 4u);
	return values_[index];
}

template <typename T>
inline T& VectorT4<T>::operator()(const unsigned int index) noexcept
{
	ocean_assert(index < 4u);
	return values_[index];
}

template <typename T>
inline const T* VectorT4<T>::operator()() const noexcept
{
	return values_;
}

template <typename T>
inline T* VectorT4<T>::operator()() noexcept
{
	return values_;
}

template <typename T>
inline size_t VectorT4<T>::operator()(const VectorT4<T>& vector) const
{
	size_t seed = std::hash<T>{}(vector.x());
	seed ^= std::hash<T>{}(vector.y()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(vector.z()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(vector.w()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

template <>
template <>
inline std::vector<VectorT4<float>> VectorT4<float>::vectors2vectors(std::vector<VectorT4<float>>&& vectors)
{
	return std::move(vectors);
}

template <>
template <>
inline std::vector<VectorT4<double>> VectorT4<double>::vectors2vectors(std::vector<VectorT4<double>>&& vectors)
{
	return std::move(vectors);
}

template <typename T>
template <typename U>
inline std::vector<VectorT4<T>> VectorT4<T>::vectors2vectors(std::vector<VectorT4<U>>&& vectors)
{
	std::vector<VectorT4<T>> result;
	result.reserve(vectors.size());

	for (typename std::vector<VectorT4<U>>::const_iterator i = vectors.cbegin(); i != vectors.cend(); ++i)
	{
		result.emplace_back(*i);
	}

	return result;
}

template <>
template <>
inline std::vector<VectorT4<float>> VectorT4<float>::vectors2vectors(const std::vector<VectorT4<float>>& vectors)
{
	return vectors;
}

template <>
template <>
inline std::vector<VectorT4<double>> VectorT4<double>::vectors2vectors(const std::vector<VectorT4<double>>& vectors)
{
	return vectors;
}

template <typename T>
template <typename U>
inline std::vector<VectorT4<T>> VectorT4<T>::vectors2vectors(const std::vector<VectorT4<U>>& vectors)
{
	std::vector<VectorT4<T>> result;
	result.reserve(vectors.size());

	for (typename std::vector<VectorT4<U>>::const_iterator i = vectors.begin(); i != vectors.end(); ++i)
	{
		result.emplace_back(*i);
	}

	return result;
}

template <typename T>
template <typename U>
inline std::vector<VectorT4<T>> VectorT4<T>::vectors2vectors(const VectorT4<U>* vectors, const size_t size)
{
	std::vector<VectorT4<T>> result;
	result.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		result.emplace_back(vectors[n]);
	}

	return result;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const VectorT4<T>& vector)
{
	stream << "[" << vector.x() << ", " << vector.y() << ", " << vector.z() << ", " << vector.w() << "]";

	return stream;
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const VectorT4<T>& vector)
{
	return messageObject << "[" << vector.x() << ", " << vector.y() << ", " << vector.z() << ", " << vector.w() << "]";
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const VectorT4<T>& vector)
{
	return messageObject << "[" << vector.x() << ", " << vector.y() << ", " << vector.z() << ", " << vector.w() << "]";
}

}

#endif // META_OCEAN_MATH_VECTOR4_H
