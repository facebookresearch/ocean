/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_VECTOR3_H
#define META_OCEAN_MATH_VECTOR3_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Vector2.h"

#include <type_traits>
#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class VectorT3;

/**
 * Definition of a 3D vector.
 * @see VectorT3
 * @ingroup math
 */
typedef VectorT3<Scalar> Vector3;

/**
 * Definition of a 3D vector with double values.
 * @see VectorT3
 * @ingroup math
 */
typedef VectorT3<double> VectorD3;

/**
 * Definition of a 3D vector with float values.
 * @see VectorT3
 * @ingroup math
 */
typedef VectorT3<float> VectorF3;

/**
 * Definition of a 3D vector with integer values.
 * @see VectorT3
 * @ingroup math
 */
typedef VectorT3<int> VectorI3;

/**
 * Definition of a typename alias for vectors with VectorT3 objects.
 * @see VectorT3
 * @ingroup math
 */
template <typename T>
using VectorsT3 = std::vector<VectorT3<T>>;

/**
 * Definition of a vector holding Vector3 objects.
 * @see Vector3
 * @ingroup math
 */
typedef std::vector<Vector3> Vectors3;

/**
 * Definition of a vector holding VectorD3 objects.
 * @see VectorD3
 * @ingroup math
 */
typedef std::vector<VectorD3> VectorsD3;

/**
 * Definition of a vector holding VectorF3 objects.
 * @see VectorF3
 * @ingroup math
 */
typedef std::vector<VectorF3> VectorsF3;

/**
 * Definition of a vector holding VectorI3 objects.
 * @see VectorI3
 * @ingroup math
 */
typedef std::vector<VectorI3> VectorsI3;

/**
 * This class implements a vector with three elements.
 * The element order is: (x, y, z).
 * @tparam T Data type of the vector elements.
 * @see Vector3, VectorF3, VectorD3.
 * @ingroup math
 */
template <typename T>
class VectorT3
{
	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates a new 3D vector with undefined elements.
		 * Beware: The elements are neither zero nor a specific value!
		 * This is useful in situations where VectorT3 objects are created (e.g, by a large array or vector)
		 * and their values are assigned in a function afterwards.
		 */
		inline VectorT3() noexcept;

		/**
		 * Creates a new 3D vector.
		 * @param setToHomogeneous Determines whether a homogeneous vector (0, 0, 1) will be created, otherwise the vector is initialized with zeros
		 */
		inline explicit VectorT3(const bool setToHomogeneous) noexcept;

		/**
		 * Creates a new 3D vector with three given elements.
		 * @param x X value
		 * @param y Y value
		 * @param z Z value
		 */
		inline VectorT3(const T& x, const T& y, const T& z) noexcept;

		/**
		 * Creates a new 3D vector with a given array of elements.
		 * @param valueArray Array with at least three elements
		 */
		inline explicit VectorT3(const T* valueArray) noexcept;

		/**
		 * Creates a new 3D vector with a given 2D vector defining the first two elements and a single value defining the third element.
		 * @param vector 2D vector defining the first two elements (X and Y value)
		 * @param z Z value defining the third element
		 */
		inline explicit VectorT3(const VectorT2<T>& vector, const T& z = 0) noexcept;

		/**
		 * Copies a vector.
		 * @param vector 3D vector that is copied
		 */
		inline VectorT3(const VectorT3<T>& vector) noexcept;

		/**
		 * Copies a vector with different element data type than T.
		 * @param vector The 3D vector to copy
		 * @tparam U The element data type of the second vector
		 */
		template <typename U>
		inline explicit VectorT3(const VectorT3<U>& vector) noexcept;

		/**
		 * Returns the cross product of two vectors.
		 * The cross product of two parallel vectors (or of at least one zero vector) results in a zero vector.
		 * @param vector The right vector
		 * @return The resulting cross product of both vectors
		 * @see isParallel().
		 */
		inline VectorT3<T> cross(const VectorT3<T>& vector) const;

		/**
		 * Returns the normalized vector.
		 * Beware: This function does not throw an exception if this vector cannot be normalized.<br>
		 * Thus, ensure that this vector is not zero before calling this function.<br>
		 * Or even better, use different normalization functions like: normalizedOrZero(), normalizedOrValue(), or normalize().<br>
		 * In case, the vector cannot be normalized, an uninitialized vector will be returned (due to performance reasons).
		 * @return This vector as unit vector (vector with length 1)
		 * @see normalizedOrZero(), normalizedOrValue(), normalize().
		 */
		inline VectorT3<T> normalized() const;

		/**
		 * Returns the normalized vector.
		 * If this vector cannot be normalized the zero vector is returned.
		 * @return Vector with length 1
		 */
		inline VectorT3<T> normalizedOrZero() const;

		/**
		 * Returns the normalized vector.
		 * If this vector cannot be normalized the given vector is returned.
		 * @param value Vector that will be returned if the vector cannot be normalized
		 * @return Vector with length 1
		 */
		inline VectorT3<T> normalizedOrValue(const VectorT3<T>& value) const;

		/**
		 * Normalizes this vector
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
		 * Returns the distance between this 3D position and a second 3D position.
		 * @param right Second 3D position
		 * @return Distance between the two points
		 */
		inline T distance(const VectorT3<T>& right) const;

		/**
		 * Returns the square distance between this 3D position and a second 3D position.
		 * @param right Second 3D position
		 * @return Square distance between the two points
		 */
		inline T sqrDistance(const VectorT3<T>& right) const;

		/**
		 * Returns the angle between this vector and a second vectors.
		 * Beware: This vector must not be zero.<br>
		 * Beware: This function does not throw an exception if one or both vectors are zero.<br>
		 * In case, the angle cannot be determined -1 will be returned.
		 * @param right Second vector, must not be zero
		 * @return Angle between both vectors in radian, with range [0, PI], -1 in case of an error
		 */
		T angle(const VectorT3<T>& right) const;

		/**
		 * Returns the reflected vector of this vector corresponding to a given normal vector.
		 * The dot product between this vector and the normal vector must be positive.<br>
		 * @param normal The normal vector used to determine the reflection vector for
		 * @return Resulting reflection vector
		 */
		VectorT3<T> reflect(const VectorT3<T>& normal) const;

		/**
		 * Returns the refracted vector of this vector corresponding to a given normal vector.
		 * The dot product between this vector and the normal vector must be positive.<br>
		 * @param normal The normal vector used to determine the refracted vector for
		 * @param index Refraction index that is the ratio between the leaving and the entering refraction index, an index lower than 1 describes the transition from a thin into a thick medium, with range (0, infinity)
		 * @return Resulting refracted vector
		 */
		VectorT3<T> refract(const VectorT3<T>& normal, const T index) const;

		/**
		 * Returns a vector that is perpendicular to this vector.
		 * If this vector is a zero vector, than the resulting vector will be arbitrary.
		 * @return Resulting perpendicular vector, a unity vector is not guaranteed
		 */
		VectorT3<T> perpendicular() const;

		/**
		 * Returns whether two vectors are parallel.
		 * A zero vector will not be parallel.
		 * @param right The right vector
		 * @return True, if so
		 * @see cross().
		 */
		bool isParallel(const VectorT3<T>& right) const;

		/**
		 * Returns whether two vectors are orthogonal.
		 * A zero vector will not be orthogonal.
		 * @param right The right vector
		 * @return True, if so
		 */
		inline bool isOrthogonal(const VectorT3<T>& right) const;

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
		 * Returns the x and y component of the vector as new 2D vector.
		 * @return New 2D vector
		 */
		inline VectorT2<T> xy() const noexcept;

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
		 * @param eps Epsilon to be used
		 * @return True, if so
		 */
		inline bool isEqual(const VectorT3<T>& vector, const T eps) const;

		/**
		 * Copy assigns a vector
		 * @param vector 3D vector that is copied
		 */
		inline VectorT3& operator=(const VectorT3<T>& vector);

		/**
		 * Returns whether two vectors are identical up to a small epsilon.
		 * @param vector Right vector
		 * @return True, if so
		 */
		inline bool operator==(const VectorT3<T>& vector) const;

		/**
		 * Returns whether two vectors are not identical up to a small epsilon.
		 * @param vector Right vector
		 * @return True, if so
		 */
		inline bool operator!=(const VectorT3<T>& vector) const;

		/**
		 * Adds two vectors.
		 * @param vector Right vector
		 * @return Sum vector
		 */
		inline VectorT3<T> operator+(const VectorT3<T>& vector) const;

		/**
		 * Adds and assigns two vectors.
		 * @param vector Right vector
		 * @return Reference to this vector
		 */
		inline VectorT3<T>& operator+=(const VectorT3<T>& vector);

		/**
		 * Subtracts two vectors.
		 * @param vector Right vector
		 * @return Difference vector
		 */
		inline VectorT3<T> operator-(const VectorT3<T>& vector) const;

		/**
		 * Subtracts and assigns two vectors.
		 * @param vector Right vector
		 * @return Reference to this vector
		 */
		inline VectorT3<T>& operator-=(const VectorT3<T>& vector);

		/**
		 * Returns the negated vector.
		 * @return Negated vector
		 */
		inline VectorT3<T> operator-() const;

		/**
		 * Returns the dot product of two vectors.
		 * @param vector Right vector
		 * @return Dot product
		 */
		inline T operator*(const VectorT3<T>& vector) const;

		/**
		 * Multiplies this vector with a scalar.
		 * @param value Scalar value
		 * @return Resulting vector
		 */
		inline VectorT3<T> operator*(const T& value) const;

		/**
		 * Multiplies and assigns this vector with a scalar.
		 * @param value Scalar value
		 * @return Reference to this vector
		 */
		inline VectorT3<T>& operator*=(const T& value);

		/**
		 * Divides this vector by a scalar.
		 * Beware: This function does not throw an exception if the given value is zero.<br>
		 * Thus, ensure that given value is not zero before calling this function.<br>
		 * In case, the given value is zero, the result is undefined.
		 * @param value Scalar value to be used as denominator, must not be zero
		 * @return Resulting vector
		 */
		inline VectorT3<T> operator/(const T& value) const;

		/**
		 * Divides and assigns this vector by a scalar.
		 * Beware: This function does not throw an exception if the given value is zero.<br>
		 * Thus, ensure that given value is not zero before calling this function.<br>
		 * In case, the given value is zero, the result is undefined.
		 * @param value Scalar value to be used as denominator, must not be zero
		 * @return Reference to this vector
		 */
		inline VectorT3<T>& operator/=(const T& value);

		/**
		 * Compares two vector objects and returns whether the left vector represents a smaller value than the right vector.
		 * First the first component of both vectors are compared, if these values are equal then the next components are compares and so on.<br>
		 * @param vector The second vector to compare
		 * @return True, if so
		 */
		inline bool operator<(const VectorT3<T>& vector) const;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 2]
		 * @return Element of the vector
		 */
		inline const T& operator[](const unsigned int index) const noexcept;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 2]
		 * @return Element of the vector
		 */
		inline T& operator[](const unsigned int index) noexcept;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 2]
		 * @return Element of the vector
		 */
		inline const T& operator()(const unsigned int index) const noexcept;

		/**
		 * Element access operator.
		 * @param index The index of the element to access, with range [0, 2]
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
		inline size_t operator()(const VectorT3<T>& vector) const;

		/**
		 * Converts vectors with specific data type to vectors with different data type.
		 * @param vectors The vectors to convert
		 * @return The converted vectors
		 * @tparam U The element data type of the vectors to convert
		 */
		template <typename U>
		static inline std::vector<VectorT3<T>> vectors2vectors(std::vector<VectorT3<U>>&& vectors);

		/**
		 * Converts vectors with specific data type to vectors with different data type.
		 * @param vectors The vectors to convert
		 * @return The converted vectors
		 * @tparam U The element data type of the vectors to convert
		 */
		template <typename U>
		static inline std::vector<VectorT3<T>> vectors2vectors(const std::vector<VectorT3<U>>& vectors);

		/**
		 * Converts vectors with specific data type to vectors with different data type.
		 * @param vectors The vectors to convert
		 * @param size The number of vector to convert
		 * @return The converted vectors
		 * @tparam U The element data type of the vectors to convert
		 */
		template <typename U>
		static inline std::vector<VectorT3<T>> vectors2vectors(const VectorT3<U>* vectors, const size_t size);

	protected:

		/// The three values of the vector, with element order x, y, z.
		T values_[3];
};

template <typename T>
inline VectorT3<T>::VectorT3() noexcept
{
	static_assert(std::is_arithmetic<T>::value, "VectorT3 only supports arithmetic types");
	// nothing to do here
}

template <typename T>
inline VectorT3<T>::VectorT3(const bool setToHomogeneous) noexcept
{
	if (setToHomogeneous)
	{
		values_[0] = T(0.0);
		values_[1] = T(0.0);
		values_[2] = T(1.0);
	}
	else
	{
		values_[0] = T(0.0);
		values_[1] = T(0.0);
		values_[2] = T(0.0);
	}
}

template <typename T>
inline VectorT3<T>::VectorT3(const T& x, const T& y, const T& z) noexcept
{
	values_[0] = x;
	values_[1] = y;
	values_[2] = z;
}

template <typename T>
inline VectorT3<T>::VectorT3(const T* valueArray) noexcept
{
	memcpy(values_, valueArray, sizeof(T) * 3);
}

template <typename T>
inline VectorT3<T>::VectorT3(const VectorT2<T>& vector, const T& z) noexcept
{
	values_[0] = vector[0];
	values_[1] = vector[1];
	values_[2] = z;
}

template <typename T>
inline VectorT3<T>::VectorT3(const VectorT3<T>& vector) noexcept
{
	values_[0] = vector.values_[0];
	values_[1] = vector.values_[1];
	values_[2] = vector.values_[2];
}

template <typename T>
template <typename U>
inline VectorT3<T>::VectorT3(const VectorT3<U>& vector) noexcept
{
	values_[0] = T(vector[0]);
	values_[1] = T(vector[1]);
	values_[2] = T(vector[2]);
}

template <typename T>
inline VectorT3<T> VectorT3<T>::cross(const VectorT3<T>& vector) const
{
	return VectorT3<T>(values_[1] * vector.values_[2] - values_[2] * vector.values_[1],
						values_[2] * vector.values_[0] - values_[0] * vector.values_[2],
						values_[0] * vector.values_[1] - values_[1] * vector.values_[0]);
}

template <typename T>
inline VectorT3<T> VectorT3<T>::normalized() const
{
	const T len = length();
	if (NumericT<T>::isEqualEps(len))
	{
		ocean_assert(false && "Division by zero!");
		return VectorT3<T>();
	}

	const T factor = T(1) / len;
	return VectorT3<T>(values_[0] * factor, values_[1] * factor, values_[2] * factor);
}

template <typename T>
inline VectorT3<T> VectorT3<T>::normalizedOrZero() const
{
	const T len = length();

	if (NumericT<T>::isEqualEps(len))
	{
		return *this;
	}

	const T factor = T(1) / len;
	return VectorT3<T>(values_[0] * factor, values_[1] * factor, values_[2] * factor);
}

template <typename T>
inline VectorT3<T> VectorT3<T>::normalizedOrValue(const VectorT3<T>& value) const
{
	const T len = length();

	if (NumericT<T>::isEqualEps(len))
	{
		return value;
	}

	const T factor = T(1) / len;
	return VectorT3<T>(values_[0] * factor, values_[1] * factor, values_[2] * factor);
}

template <typename T>
inline bool VectorT3<T>::normalize()
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

	return true;
}

template <typename T>
inline T VectorT3<T>::length() const
{
	return NumericT<T>::sqrt(values_[0] * values_[0] + values_[1] * values_[1] + values_[2] * values_[2]);
}

template <typename T>
inline T VectorT3<T>::sqr() const
{
	return values_[0] * values_[0] + values_[1] * values_[1] + values_[2] * values_[2];
}

template <typename T>
inline T VectorT3<T>::distance(const VectorT3<T>& right) const
{
	return NumericT<T>::sqrt(NumericT<T>::sqr(values_[0] - right.values_[0]) + NumericT<T>::sqr(values_[1] - right.values_[1]) + NumericT<T>::sqr(values_[2] - right.values_[2]));
}

template <typename T>
inline T VectorT3<T>::sqrDistance(const VectorT3<T>& right) const
{
	return NumericT<T>::sqr(values_[0] - right.values_[0]) + NumericT<T>::sqr(values_[1] - right.values_[1]) + NumericT<T>::sqr(values_[2] - right.values_[2]);
}

template <typename T>
T VectorT3<T>::angle(const VectorT3<T>& right) const
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

	const T dot = values_[0] * right.values_[0] + values_[1] * right.values_[1] + values_[2] * right.values_[2];

	return NumericT<T>::acos((dot / thisLength) / rightLength);
}

template <typename T>
VectorT3<T> VectorT3<T>::reflect(const VectorT3<T>& normal) const
{
	ocean_assert(NumericT<T>::isNotEqualEps(normal.length()));
	ocean_assert(*this * normal >= 0);

#ifdef OCEAN_DEBUG
	const VectorT3<T> result(normal * ((normal * *this) * 2) - *this);
	ocean_assert(int(angle(normal) * 100) == int(result.angle(normal) * 100));
	ocean_assert(result * normal >= 0);
	ocean_assert(NumericT<T>::isWeakEqual(length(), result.length()));
#endif // OCEAN_DEBUG

	return normal * ((normal * *this) * 2) - *this;
}

template <typename T>
VectorT3<T> VectorT3<T>::refract(const VectorT3<T>& normal, const T index) const
{
	ocean_assert(NumericT<T>::isNotEqualEps(normal.length()));
	ocean_assert(*this * normal >= 0);
	ocean_assert(index > T(0));

	const T dot = normal * *this;

	const T sqrtValue = T(1) - (index * index) * (T(1) - dot * dot);

	// check whether a total internal reflection occurs
	if (sqrtValue < T(0))
	{
		return reflect(normal);
	}

#ifdef OCEAN_DEBUG
	const VectorT3<T> result(normal * (index * dot - NumericT<T>::sqrt(sqrtValue)) - *this * index);
	const T angle0 = angle(normal);
	const T angle1 = result.angle(-normal);
	const T sin0 = NumericT<T>::sin(angle0);
	const T sin1 = NumericT<T>::sin(angle1);
	ocean_assert(NumericT<T>::isNotEqualEps(sin1));
	ocean_assert(NumericT<T>::isWeakEqual(index, sin0 / sin1));
	ocean_assert(NumericT<T>::isWeakEqual(length(), result.length()));
#endif // OCEAN_DEBUG

	return normal * (index * dot - NumericT<T>::sqrt(T(1) - (index * index) * (T(1) - dot * dot))) - *this * index;
}

template <typename T>
VectorT3<T> VectorT3<T>::perpendicular() const
{
	if (NumericT<T>::isNotEqualEps(values_[0]) || NumericT<T>::isNotEqualEps(values_[2]))
	{
		return cross(VectorT3<T>(0, 1, 0));
	}

	if (NumericT<T>::isNotEqualEps(values_[1]))
	{
		return cross(VectorT3<T>(1, 0, 0));
	}

	ocean_assert(isNull());
	return VectorT3<T>(1, 0, 0);
}

template <typename T>
bool VectorT3<T>::isParallel(const VectorT3<T>& right) const
{
#ifdef OCEAN_DEBUG
	if (std::is_same<double, Scalar>::value)
	{
		const VectorT3<T> normalizedThis(normalizedOrZero());
		const VectorT3<T> normalizedRight(right.normalizedOrZero());

		const T dotProduct = normalizedThis * normalizedRight;

		const bool debugResult = NumericT<T>::isEqual(dotProduct, 1) || NumericT<T>::isEqual(dotProduct, -1);
		const VectorT3<T> crossVector = cross(right);

		ocean_assert_accuracy(debugResult == (!isNull() && !right.isNull() && crossVector.isNull()));
	}
#endif

	return !isNull() && !right.isNull() && cross(right).isNull();
}

template <typename T>
inline bool VectorT3<T>::isOrthogonal(const VectorT3<T>& right) const
{
	return NumericT<T>::isEqualEps(values_[0] * right.values_[0] + values_[1] * right.values_[1] + values_[2] * right.values_[2]);
}

template <typename T>
inline const T& VectorT3<T>::x() const noexcept
{
	return values_[0];
}

template <typename T>
inline T& VectorT3<T>::x() noexcept
{
	return values_[0];
}

template <typename T>
inline const T& VectorT3<T>::y() const noexcept
{
	return values_[1];
}

template <typename T>
inline T& VectorT3<T>::y() noexcept
{
	return values_[1];
}

template <typename T>
inline const T& VectorT3<T>::z() const noexcept
{
	return values_[2];
}

template <typename T>
inline T& VectorT3<T>::z() noexcept
{
	return values_[2];
}

template <typename T>
inline VectorT2<T> VectorT3<T>::xy() const noexcept
{
	return VectorT2<T>(values_[0], values_[1]);
}

template <typename T>
inline const T* VectorT3<T>::data() const noexcept
{
	return values_;
}

template <typename T>
inline T* VectorT3<T>::data() noexcept
{
	return values_;
}

template <typename T>
inline bool VectorT3<T>::isNull() const
{
	return NumericT<T>::isEqualEps(values_[0]) && NumericT<T>::isEqualEps(values_[1])
		&& NumericT<T>::isEqualEps(values_[2]);
}

template <typename T>
inline bool VectorT3<T>::isUnit(const T eps) const
{
	return NumericT<T>::isEqual(length(), T(1), eps);
}

template <typename T>
inline bool VectorT3<T>::isEqual(const VectorT3<T>& vector, const T eps) const
{
	return NumericT<T>::isEqual(values_[0], vector.values_[0], eps)
			&& NumericT<T>::isEqual(values_[1], vector.values_[1], eps)
			&& NumericT<T>::isEqual(values_[2], vector.values_[2], eps);
}

template <typename T>
inline VectorT3<T>& VectorT3<T>::operator=(const VectorT3<T>& vector)
{
	if (this == &vector)
	{
		return *this;
	}

	values_[0] = vector.values_[0];
	values_[1] = vector.values_[1];
	values_[2] = vector.values_[2];

	return *this;
}

template <typename T>
inline bool VectorT3<T>::operator==(const VectorT3<T>& vector) const
{
	return NumericT<T>::isEqual(values_[0], vector.values_[0])
		&& NumericT<T>::isEqual(values_[1], vector.values_[1])
		&& NumericT<T>::isEqual(values_[2], vector.values_[2]);
}

template <typename T>
inline bool VectorT3<T>::operator!=(const VectorT3<T>& vector) const
{
	return NumericT<T>::isNotEqual(values_[0], vector.values_[0])
		|| NumericT<T>::isNotEqual(values_[1], vector.values_[1])
		|| NumericT<T>::isNotEqual(values_[2], vector.values_[2]);
}

template <typename T>
inline VectorT3<T> VectorT3<T>::operator+(const VectorT3<T>& vector) const
{
	return VectorT3<T>(values_[0] + vector.values_[0], values_[1] + vector.values_[1], values_[2] + vector.values_[2]);
}

template <typename T>
inline VectorT3<T>& VectorT3<T>::operator+=(const VectorT3<T>& vector)
{
	values_[0] += vector.values_[0];
	values_[1] += vector.values_[1];
	values_[2] += vector.values_[2];

	return *this;
}

template <typename T>
inline VectorT3<T> VectorT3<T>::operator-(const VectorT3<T>& vector) const
{
	return VectorT3<T>(values_[0] - vector.values_[0], values_[1] - vector.values_[1], values_[2] - vector.values_[2]);
}

template <typename T>
inline VectorT3<T>& VectorT3<T>::operator-=(const VectorT3<T>& vector)
{
	values_[0] -= vector.values_[0];
	values_[1] -= vector.values_[1];
	values_[2] -= vector.values_[2];

	return *this;
}

template <typename T>
inline VectorT3<T> VectorT3<T>::operator-() const
{
	return VectorT3<T>(-values_[0], -values_[1], -values_[2]);
}

template <typename T>
inline T VectorT3<T>::operator*(const VectorT3<T>& vector) const
{
	return values_[0] * vector.values_[0] + values_[1] * vector.values_[1] + values_[2] * vector.values_[2];
}

template <typename T>
inline VectorT3<T> VectorT3<T>::operator*(const T& value) const
{
	return VectorT3<T>(values_[0] * value, values_[1] * value, values_[2] * value);
}

template <typename T>
inline VectorT3<T>& VectorT3<T>::operator*=(const T& value)
{
	values_[0] *= value;
	values_[1] *= value;
	values_[2] *= value;

	return *this;
}

template <typename T>
inline VectorT3<T> VectorT3<T>::operator/(const T& value) const
{
	ocean_assert(NumericT<T>::isNotEqualEps(value));
	T factor = T(1) / value;

	return VectorT3<T>(values_[0] * factor, values_[1] * factor, values_[2] * factor);
}

template <typename T>
inline VectorT3<T>& VectorT3<T>::operator/=(const T& value)
{
	ocean_assert(NumericT<T>::isNotEqualEps(value));

	T factor = T(1) / value;

	values_[0] *= factor;
	values_[1] *= factor;
	values_[2] *= factor;

	return *this;
}

template <typename T>
inline bool VectorT3<T>::operator<(const VectorT3<T>& right) const
{
	return values_[0] < right.values_[0]
			|| (values_[0] == right.values_[0] && (values_[1] < right.values_[1]
			|| (values_[1] == right.values_[1] && values_[2] < right.values_[2])));
}

template <typename T>
inline const T& VectorT3<T>::operator[](const unsigned int index) const noexcept
{
	ocean_assert(index < 3u);
	return values_[index];
}

template <typename T>
inline T& VectorT3<T>::operator[](const unsigned int index) noexcept
{
	ocean_assert(index < 3u);
	return values_[index];
}

template <typename T>
inline const T& VectorT3<T>::operator()(const unsigned int index) const noexcept
{
	ocean_assert(index < 3u);
	return values_[index];
}

template <typename T>
inline T& VectorT3<T>::operator()(const unsigned int index) noexcept
{
	ocean_assert(index < 3u);
	return values_[index];
}

template <typename T>
inline const T* VectorT3<T>::operator()() const noexcept
{
	return values_;
}

template <typename T>
inline T* VectorT3<T>::operator()() noexcept
{
	return values_;
}

template <typename T>
inline size_t VectorT3<T>::operator()(const VectorT3<T>& vector) const
{
	size_t seed = std::hash<T>{}(vector.x());
	seed ^= std::hash<T>{}(vector.y()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(vector.z()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

template <>
template <>
inline std::vector<VectorT3<float>> VectorT3<float>::vectors2vectors(std::vector<VectorT3<float>>&& vectors)
{
	return std::move(vectors);
}

template <>
template <>
inline std::vector<VectorT3<double>> VectorT3<double>::vectors2vectors(std::vector<VectorT3<double>>&& vectors)
{
	return std::move(vectors);
}

template <typename T>
template <typename U>
inline std::vector<VectorT3<T>> VectorT3<T>::vectors2vectors(std::vector<VectorT3<U>>&& vectors)
{
	std::vector<VectorT3<T>> result;
	result.reserve(vectors.size());

	for (typename std::vector<VectorT3<U>>::const_iterator i = vectors.cbegin(); i != vectors.cend(); ++i)
	{
		result.emplace_back(*i);
	}

	return result;
}

template <>
template <>
inline std::vector<VectorT3<float>> VectorT3<float>::vectors2vectors(const std::vector<VectorT3<float>>& vectors)
{
	return vectors;
}

template <>
template <>
inline std::vector<VectorT3<double>> VectorT3<double>::vectors2vectors(const std::vector<VectorT3<double>>& vectors)
{
	return vectors;
}

template <typename T>
template <typename U>
inline std::vector<VectorT3<T>> VectorT3<T>::vectors2vectors(const std::vector<VectorT3<U>>& vectors)
{
	std::vector<VectorT3<T>> result;
	result.reserve(vectors.size());

	for (typename std::vector<VectorT3<U>>::const_iterator i = vectors.cbegin(); i != vectors.cend(); ++i)
	{
		result.emplace_back(*i);
	}

	return result;
}

template <typename T>
template <typename U>
inline std::vector<VectorT3<T>> VectorT3<T>::vectors2vectors(const VectorT3<U>* vectors, const size_t size)
{
	std::vector< VectorT3<T>> result;
	result.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		result.emplace_back(vectors[n]);
	}

	return result;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const VectorT3<T>& vector)
{
	stream << "[" << vector.x() << ", " << vector.y() << ", " << vector.z() << "]";

	return stream;
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const VectorT3<T>& vector)
{
	return messageObject << "[" << vector.x() << ", " << vector.y() << ", " << vector.z() << "]";
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const VectorT3<T>& vector)
{
	return messageObject << "[" << vector.x() << ", " << vector.y() << ", " << vector.z() << "]";
}

}

#endif // META_OCEAN_MATH_VECTOR3_H
