/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_VECTOR2_H
#define META_OCEAN_MATH_VECTOR2_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"

#include <type_traits>
#include <vector>

namespace Ocean
{

// Forward declaration.
template <typename T> class VectorT2;

/**
 * Definition of a 2D vector.
 * @see VectorT2
 * @ingroup math
 */
typedef VectorT2<Scalar> Vector2;

/**
 * Definition of a 2D vector with double values.
 * @see VectorT2
 * @ingroup math
 */
typedef VectorT2<double> VectorD2;

/**
 * Definition of a 2D vector with float values.
 * @see VectorT2
 * @ingroup math
 */
typedef VectorT2<float> VectorF2;

/**
 * Definition of a 2D vector with integer values.
 * @see VectorT2
 * @ingroup math
 */
typedef VectorT2<int> VectorI2;

/**
 * Definition of a typename alias for vectors with VectorT2 objects.
 * @see VectorT2
 * @ingroup math
 */
template <typename T>
using VectorsT2 = std::vector<VectorT2<T>>;

/**
 * Definition of a vector holding Vector2 objects.
 * @see Vector2
 * @ingroup math
 */
typedef std::vector<Vector2> Vectors2;

/**
 * Definition of a vector holding VectorD2 objects.
 * @see VectorD2
 * @ingroup math
 */
typedef std::vector<VectorD2> VectorsD2;

/**
 * Definition of a vector holding VectorF2 objects.
 * @see VectorF2
 * @ingroup math
 */
typedef std::vector<VectorF2> VectorsF2;

/**
 * Definition of a vector holding VectorI2 objects.
 * @see VectorI2
 * @ingroup math
 */
typedef std::vector<VectorI2> VectorsI2;

/**
 * This class implements a vector with two elements.
 * The element order is: (x, y).
 * @tparam T Data type of the vector elements.
 * @see Vector2, VectorF2, VectorF3.
 * @ingroup math
 */
template <typename T>
class VectorT2
{
	public:

		/**
		 * Definition of the used data type.
		 */
		typedef T Type;

	public:

		/**
		 * Creates a new 2D vector with undefined elements.
		 * Beware: The elements are neither zero nor a specific value!
		 * This is useful in situations where VectorT2 objects are created (e.g, by a large array or vector)
		 * and their values are assigned in a function afterwards.
		 */
		inline VectorT2() noexcept;

		/**
		 * Creates a new 2D vector.
		 * @param setToHomogeneous Determines whether a homogeneous vector (0, 1) will be created, otherwise the vector is initialized with zeros
		 */
		inline explicit VectorT2(const bool setToHomogeneous) noexcept;

		/**
		 * Creates a new 2D vector with specified coordinates.
		 * @param x X value
		 * @param y Y value
		 */
		inline VectorT2(const T x, const T y) noexcept;

		/**
		 * Creates a new 2D vector with a given array of elements.
		 * @param valueArray Array with at least two elements
		 */
		inline explicit VectorT2(const T* valueArray) noexcept;

		/**
		 * Copies a vector.
		 * @param vector 2D vector that is copied
		 */
		inline VectorT2(const VectorT2<T>& vector) noexcept;

		/**
		 * Copies a vector with different element data type than T.
		 * @param vector The 2D vector to copy
		 * @tparam U The element data type of the second vector
		 */
		template <typename U>
		inline explicit VectorT2(const VectorT2<U>& vector) noexcept;

		/**
		 * Returns the cross product of two 2D vectors.
		 * The cross product is the resulting z-component of a cross product between two 3D vectors having 0 as z-component.
		 * @param vector Right vector
		 * @return Cross product
		 * @see perpendicular().
		 */
		inline T cross(const VectorT2<T>& vector) const;

		/**
		 * Returns a vector perpendicular to this vectors.
		 * The cross product between this vector and the resulting perpendicular vector will have a positive value.<br>
		 * @return Perpendicular vector with identical length as this vector
		 * @see cross().
		 */
		inline VectorT2<T> perpendicular() const;

		/**
		 * Returns the normalized vector.
		 * Beware: This function does not throw an exception if this vector cannot be normalized.<br>
		 * Thus, ensure that this vector is not zero before calling this function.<br>
		 * Or even better, use different normalization functions like: normalizedOrZero(), normalizedOrValue(), or normalize().<br>
		 * In case, the vector cannot be normalized, an uninitialized vector will be returned (due to performance reasons).
		 * @return This vector as unit vector (vector with length 1)
		 * @see normalizedOrZero(), normalizedOrValue(), normalize().
		 */
		inline VectorT2<T> normalized() const;

		/**
		 * Returns the normalized vector.
		 * If this vector cannot be normalized the zero vector is returned.
		 * @return Vector with length 1
		 */
		inline VectorT2<T> normalizedOrZero() const;

		/**
		 * Returns the normalized vector.
		 * If this vector cannot be normalized the given vector is returned.
		 * @param value Vector that will be returned if the vector cannot be normalized
		 * @return Vector with length 1
		 */
		inline VectorT2<T> normalizedOrValue(const VectorT2<T>& value) const;

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
		 * Returns the distance between this 2D position and a second 2D position.
		 * @param right Second 2D position
		 * @return Distance between the two points
		 */
		inline T distance(const VectorT2<T>& right) const;

		/**
		 * Returns the square distance between this 2D position and a second 2D position.
		 * @param right Second 2D position
		 * @return Square distance between the two points
		 */
		inline T sqrDistance(const VectorT2<T>& right) const;

		/**
		 * Returns the angle between this vector and a second vectors.
		 * Beware: This vector must not be zero.<br>
		 * Beware: This function does not throw an exception if one or both vectors are zero.<br>
		 * In case, the angle cannot be determined -1 will be returned.
		 * @param right Second vector, must not be zero
		 * @return Angle between both vectors in radian, with range [0, PI], -1 in case of an error
		 * @see isNull().
		 */
		T angle(const VectorT2<T>& right) const;

		/**
		 * Returns whether two vectors are parallel.
		 * A zero vector is be parallel.
		 * @param right The right vector
		 * @param epsilon The epsilon to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isParallel(const VectorT2<T>& right, const T epsilon = NumericT<T>::eps()) const;

		/**
		 * Returns whether two vectors are orthogonal.
		 * A zero vector is not orthogonal.
		 * @param right The right vector
		 * @param epsilon The epsilon to be used, with range [0, infinity)
		 * @return True, if so
		 */
		bool isOrthogonal(const VectorT2<T>& right, const T epsilon = NumericT<T>::eps()) const;

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
		inline bool isEqual(const VectorT2<T>& vector, const T eps) const;

		/**
		 * Copy assigns a vector
		 * @param vector 2D vector that is copied
		 */
		inline VectorT2<T>& operator=(const VectorT2<T>& vector);

		/**
		 * Returns whether two vectors are identical up to a small epsilon.
		 * @param vector Right vector
		 * @return True, if so
		 */
		inline bool operator==(const VectorT2<T>& vector) const;

		/**
		 * Returns whether two vectors are not identical up to a small epsilon.
		 * @param vector Right vector
		 * @return True, if so
		 */
		inline bool operator!=(const VectorT2<T>& vector) const;

		/**
		 * Adds two vectors.
		 * @param vector Right vector
		 * @return Sum vector
		 */
		inline VectorT2<T> operator+(const VectorT2<T>& vector) const;

		/**
		 * Adds and assigns two vectors.
		 * @param vector Right vector
		 * @return Reference to this vector
		 */
		inline VectorT2<T>& operator+=(const VectorT2<T>& vector);

		/**
		 * Subtracts two vectors.
		 * @param vector Right vector
		 * @return Difference vector
		 */
		inline VectorT2<T> operator-(const VectorT2<T>& vector) const;

		/**
		 * Subtracts and assigns two vectors.
		 * @param vector Right vector
		 * @return Reference to this vector
		 */
		inline VectorT2<T>& operator-=(const VectorT2<T>& vector);

		/**
		 * Returns the negated vector.
		 * @return Negated vector
		 */
		inline VectorT2<T> operator-() const;

		/**
		 * Returns the dot product of two vectors.
		 * @param vector Right vector
		 * @return Dot product
		 */
		inline T operator*(const VectorT2<T>& vector) const;

		/**
		 * Multiplies this vector with a scalar.
		 * @param value Scalar value
		 * @return Resulting vector
		 */
		inline VectorT2<T> operator*(const T& value) const;

		/**
		 * Multiplies and assigns this vector with a scalar.
		 * @param value Scalar value
		 * @return Reference to this vector
		 */
		inline VectorT2<T>& operator*=(const T& value);

		/**
		 * Divides this vector by a scalar.
		 * Beware: This function does not throw an exception if the given value is zero.<br>
		 * Thus, ensure that given value is not zero before calling this function.<br>
		 * In case, the given value is zero, the result is undefined.
		 * @param value Scalar value to be used as denominator, must not be zero
		 * @return Resulting vector
		 */
		inline VectorT2<T> operator/(const T& value) const;

		/**
		 * Divides and assigns this vector by a scalar.
		 * Beware: This function does not throw an exception if the given value is zero.<br>
		 * Thus, ensure that given value is not zero before calling this function.<br>
		 * In case, the given value is zero, the result is undefined.
		 * @param value Scalar value to be used as denominator, must not be zero
		 * @return Reference to this vector
		 */
		inline VectorT2<T>& operator/=(const T& value);

		/**
		 * Compares two vector objects and returns whether the left vector represents a smaller value than the right vector.
		 * First the first component of both vectors are compared, if these values are equal then the next components are compares and so on.<br>
		 * @param vector The second vector to compare
		 * @return True, if so
		 */
		inline bool operator<(const VectorT2<T>& vector) const;

		/**
		 * Element access operator.
		 * @param index Index of the element to access, with range [0, 1]
		 * @return Element of the vector
		 */
		inline const T& operator[](const unsigned int index) const noexcept;

		/**
		 * Element access operator.
		 * @param index Index of the element to access, with range [0, 1]
		 * @return Element of the vector
		 */
		inline T& operator[](const unsigned int index) noexcept;

		/**
		 * Element access operator.
		 * @param index Index of the element to access, with range [0, 1]
		 * @return Element of the vector
		 */
		inline const T& operator()(const unsigned int index) const noexcept;

		/**
		 * Element access operator.
		 * @param index Index of the element to access, with range [0, 1]
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
		inline size_t operator()(const VectorT2<T>& vector) const;

		/**
		 * Converts vectors with specific data type to vectors with different data type.
		 * @param vectors The vectors to convert
		 * @return The converted vectors
		 * @tparam U The element data type of the vectors to convert
		 */
		template <typename U>
		static inline std::vector<VectorT2<T>> vectors2vectors(std::vector<VectorT2<U>>&& vectors);

		/**
		 * Converts vectors with specific data type to vectors with different data type.
		 * @param vectors The vectors to convert
		 * @return The converted vectors
		 * @tparam U The element data type of the vectors to convert
		 */
		template <typename U>
		static inline std::vector<VectorT2<T>> vectors2vectors(const std::vector<VectorT2<U>>& vectors);

		/**
		 * Converts vectors with specific data type to vectors with different data type.
		 * @param vectors The vectors to convert
		 * @param size The number of vector to convert
		 * @return The converted vectors
		 * @tparam U The element data type of the vectors to convert
		 */
		template <typename U>
		static inline std::vector<VectorT2<T>> vectors2vectors(const VectorT2<U>* vectors, const size_t size);

	protected:

		/// The two values of the vector, with element order x, y.
		T values_[2];
};

template <typename T>
inline VectorT2<T>::VectorT2() noexcept
{
	static_assert(std::is_arithmetic<T>::value, "VectorT2 only supports arithmetic types");
	// nothing to do here
}

template <typename T>
inline VectorT2<T>::VectorT2(const bool setToHomogeneous) noexcept
{
	if (setToHomogeneous)
	{
		values_[0] = T(0.0);
		values_[1] = T(1.0);
	}
	else
	{
		values_[0] = T(0.0);
		values_[1] = T(0.0);
	}
}

template <typename T>
inline VectorT2<T>::VectorT2(const T x, const T y) noexcept
{
	values_[0] = x;
	values_[1] = y;
}

template <typename T>
inline VectorT2<T>::VectorT2(const T* valueArray) noexcept
{
	values_[0] = valueArray[0];
	values_[1] = valueArray[1];
}

template <typename T>
inline VectorT2<T>::VectorT2(const VectorT2<T>& vector) noexcept
{
	values_[0] = vector.values_[0];
	values_[1] = vector.values_[1];
}

template <typename T>
template <typename U>
inline VectorT2<T>::VectorT2(const VectorT2<U>& vector) noexcept
{
	values_[0] = T(vector[0]);
	values_[1] = T(vector[1]);
}

template <typename T>
inline T VectorT2<T>::cross(const VectorT2<T>& vector) const
{
	return values_[0] * vector.values_[1] - vector.values_[0] * values_[1];
}

template <typename T>
inline VectorT2<T> VectorT2<T>::perpendicular() const
{
	ocean_assert((values_[0] == 0 && values_[1] == 0) || cross(VectorT2<T>(-values_[1], values_[0])) > 0);

	return VectorT2<T>(-values_[1], values_[0]);
}

template <typename T>
inline VectorT2<T> VectorT2<T>::normalized() const
{
	T len = length();
	if (NumericT<T>::isEqualEps(len))
	{
		ocean_assert(false && "Devision by zero!");
		return VectorT2<T>();
	}

	const T factor = T(1) / len;
	return VectorT2<T>(values_[0] * factor, values_[1] * factor);
}

template <typename T>
inline VectorT2<T> VectorT2<T>::normalizedOrZero() const
{
	const T len = length();

	if (NumericT<T>::isEqualEps(len))
	{
		return *this;
	}

	const T factor = T(1) / len;
	return VectorT2<T>(values_[0] * factor, values_[1] * factor);
}

template <typename T>
inline VectorT2<T> VectorT2<T>::normalizedOrValue(const VectorT2<T>& value) const
{
	const T len = length();

	if (NumericT<T>::isEqualEps(len))
	{
		return value;
	}

	const T factor = T(1) / len;
	return VectorT2<T>(values_[0] * factor, values_[1] * factor);
}

template <typename T>
inline bool VectorT2<T>::normalize()
{
	const T len = length();
	if (NumericT<T>::isEqualEps(len))
	{
		return false;
	}

	const T factor = T(1) / len;
	values_[0] *= factor;
	values_[1] *= factor;
	return true;
}

template <typename T>
inline T VectorT2<T>::length() const
{
	return NumericT<T>::sqrt(values_[0] * values_[0] + values_[1] * values_[1]);
}

template <typename T>
inline T VectorT2<T>::sqr() const
{
	return values_[0] * values_[0] + values_[1] * values_[1];
}

template <typename T>
inline T VectorT2<T>::distance(const VectorT2<T>& right) const
{
	return NumericT<T>::sqrt(sqrDistance(right));
}

template <typename T>
inline T VectorT2<T>::sqrDistance(const VectorT2<T>& right) const
{
#ifdef OCEAN_DEBUG
	if (!std::is_same<T, typename SignedTyper<T>::Type>::value)
	{
		typedef typename SignedTyper<T>::Type SignedT;

		// -15 == 46 - 61
		// 225 == -15 * -15

		// 4294967281u == 46u - 61u
		// 225u == 4294967281u * 4294967281u

		const SignedT debugSqr = NumericT<SignedT>::sqr(SignedT(values_[0]) - SignedT(right.values_[0])) + NumericT<SignedT>::sqr(SignedT(values_[1]) - SignedT(right.values_[1]));

		ocean_assert(T(debugSqr) == NumericT<T>::sqr(values_[0] - right.values_[0]) + NumericT<T>::sqr(values_[1] - right.values_[1]));
	}
#endif

	return NumericT<T>::sqr(values_[0] - right.values_[0]) + NumericT<T>::sqr(values_[1] - right.values_[1]);
}

template <typename T>
T VectorT2<T>::angle(const VectorT2<T>& right) const
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

	const T dot = values_[0] * right.values_[0] + values_[1] * right.values_[1];

	return NumericT<T>::acos((dot / thisLength) / rightLength);
}

template <typename T>
bool VectorT2<T>::isParallel(const VectorT2<T>& right, const T epsilon) const
{
	ocean_assert(epsilon >= T(0));

	const VectorT2<T> normalizedThis(normalizedOrZero());
	const VectorT2<T> normalizedRight(right.normalizedOrZero());

	const T dotProduct = normalizedThis * normalizedRight;

	return NumericT<T>::isEqual(dotProduct, T(1), epsilon) || NumericT<T>::isEqual(dotProduct, T(-1), epsilon);
}

template <typename T>
bool VectorT2<T>::isOrthogonal(const VectorT2<T>& right, const T epsilon) const
{
	ocean_assert(epsilon >= T(0));

	return NumericT<T>::isEqual(values_[0] * right.values_[0] + values_[1] * right.values_[1], T(0), epsilon);
}

template <typename T>
inline const T& VectorT2<T>::x() const noexcept
{
	return values_[0];
}

template <typename T>
inline T& VectorT2<T>::x() noexcept
{
	return values_[0];
}

template <typename T>
inline const T& VectorT2<T>::y() const noexcept
{
	return values_[1];
}

template <typename T>
inline T& VectorT2<T>::y() noexcept
{
	return values_[1];
}

template <typename T>
inline const T* VectorT2<T>::data() const noexcept
{
	return values_;
}

template <typename T>
inline T* VectorT2<T>::data() noexcept
{
	return values_;
}

template <typename T>
inline bool VectorT2<T>::isNull() const
{
	return NumericT<T>::isEqualEps(values_[0]) && NumericT<T>::isEqualEps(values_[1]);
}

template <typename T>
inline bool VectorT2<T>::isUnit(const T eps) const
{
	return NumericT<T>::isEqual(length(), T(1), eps);
}

template <typename T>
inline bool VectorT2<T>::isEqual(const VectorT2<T>& vector, const T eps) const
{
	return NumericT<T>::isEqual(values_[0], vector.values_[0], eps)
			&& NumericT<T>::isEqual(values_[1], vector.values_[1], eps);
}

template <typename T>
inline VectorT2<T>& VectorT2<T>::operator=(const VectorT2<T>& vector)
{
	if (this == &vector)
	{
		return *this;
	}

	values_[0] = vector.values_[0];
	values_[1] = vector.values_[1];

	return *this;
}

template <typename T>
inline bool VectorT2<T>::operator==(const VectorT2<T>& vector) const
{
	return NumericT<T>::isEqual(values_[0], vector.values_[0])
			&& NumericT<T>::isEqual(values_[1], vector.values_[1]);
}

template <typename T>
inline bool VectorT2<T>::operator!=(const VectorT2<T>& vector) const
{
	return NumericT<T>::isNotEqual(values_[0], vector.values_[0])
			|| NumericT<T>::isNotEqual(values_[1], vector.values_[1]);
}

template <typename T>
inline VectorT2<T> VectorT2<T>::operator+(const VectorT2<T>& vector) const
{
	return VectorT2<T>(values_[0] + vector.values_[0], values_[1] + vector.values_[1]);
}

template <typename T>
inline VectorT2<T>& VectorT2<T>::operator+=(const VectorT2<T>& vector)
{
	values_[0] += vector.values_[0];
	values_[1] += vector.values_[1];

	return *this;
}

template <typename T>
inline VectorT2<T> VectorT2<T>::operator-(const VectorT2<T>& vector) const
{
	return VectorT2<T>(values_[0] - vector.values_[0], values_[1] - vector.values_[1]);
}

template <typename T>
inline VectorT2<T>& VectorT2<T>::operator-=(const VectorT2<T>& vector)
{
	values_[0] -= vector.values_[0];
	values_[1] -= vector.values_[1];

	return *this;
}

template <typename T>
inline VectorT2<T> VectorT2<T>::operator-() const
{
	return VectorT2<T>(-values_[0], -values_[1]);
}

template <typename T>
inline T VectorT2<T>::operator*(const VectorT2<T>& vector) const
{
	return values_[0] * vector.values_[0] + values_[1] * vector.values_[1];
}

template <typename T>
inline VectorT2<T> VectorT2<T>::operator*(const T& value) const
{
	return VectorT2<T>(values_[0] * value, values_[1] * value);
}

template <typename T>
inline VectorT2<T>& VectorT2<T>::operator*=(const T& value)
{
	values_[0] *= value;
	values_[1] *= value;

	return *this;
}

template <typename T>
inline VectorT2<T> VectorT2<T>::operator/(const T& value) const
{
	ocean_assert(NumericT<T>::isNotEqualEps(value));
	const T factor = T(1) / value;

	return VectorT2<T>(values_[0] * factor, values_[1] * factor);
}

template <typename T>
inline VectorT2<T>& VectorT2<T>::operator/=(const T& value)
{
	ocean_assert(NumericT<T>::isNotEqualEps(value));
	const T factor = T(1) / value;

	values_[0] *= factor;
	values_[1] *= factor;

	return *this;
}

template <typename T>
inline bool VectorT2<T>::operator<(const VectorT2<T>& vector) const
{
	return values_[0] < vector.values_[0] || (values_[0] == vector.values_[0] && values_[1] < vector.values_[1]);
}

template <typename T>
inline const T& VectorT2<T>::operator[](const unsigned int index) const noexcept
{
	ocean_assert(index < 2u);
	return values_[index];
}

template <typename T>
inline T& VectorT2<T>::operator[](const unsigned int index) noexcept
{
	ocean_assert(index < 2u);
	return values_[index];
}

template <typename T>
inline const T& VectorT2<T>::operator()(const unsigned int index) const noexcept
{
	ocean_assert(index < 2u);
	return values_[index];
}

template <typename T>
inline T& VectorT2<T>::operator()(const unsigned int index) noexcept
{
	ocean_assert(index < 2u);
	return values_[index];
}

template <typename T>
inline const T* VectorT2<T>::operator()() const noexcept
{
	return values_;
}

template <typename T>
inline T* VectorT2<T>::operator()() noexcept
{
	return values_;
}

template <typename T>
inline size_t VectorT2<T>::operator()(const VectorT2<T>& vector) const
{
	size_t seed = std::hash<T>{}(vector.x());
	seed ^= std::hash<T>{}(vector.y()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

template <>
template <>
inline std::vector<VectorT2<float>> VectorT2<float>::vectors2vectors(std::vector<VectorT2<float>>&& vectors)
{
	return std::move(vectors);
}

template <>
template <>
inline std::vector<VectorT2<double>> VectorT2<double>::vectors2vectors(std::vector<VectorT2<double>>&& vectors)
{
	return std::move(vectors);
}

template <typename T>
template <typename U>
inline std::vector<VectorT2<T>> VectorT2<T>::vectors2vectors(std::vector<VectorT2<U>>&& vectors)
{
	std::vector<VectorT2<T>> result;
	result.reserve(vectors.size());

	for (typename std::vector<VectorT2<U>>::const_iterator i = vectors.cbegin(); i != vectors.cend(); ++i)
	{
		result.emplace_back(*i);
	}

	return result;
}

template <>
template <>
inline std::vector<VectorT2<float>> VectorT2<float>::vectors2vectors(const std::vector<VectorT2<float>>& vectors)
{
	return vectors;
}

template <>
template <>
inline std::vector<VectorT2<double>> VectorT2<double>::vectors2vectors(const std::vector<VectorT2<double>>& vectors)
{
	return vectors;
}

template <typename T>
template <typename U>
inline std::vector<VectorT2<T>> VectorT2<T>::vectors2vectors(const std::vector<VectorT2<U>>& vectors)
{
	std::vector<VectorT2<T>> result;
	result.reserve(vectors.size());

	for (typename std::vector<VectorT2<U>>::const_iterator i = vectors.cbegin(); i != vectors.cend(); ++i)
	{
		result.emplace_back(*i);
	}

	return result;
}

template <typename T>
template <typename U>
inline std::vector<VectorT2<T>> VectorT2<T>::vectors2vectors(const VectorT2<U>* vectors, const size_t size)
{
	std::vector<VectorT2<T>> result;
	result.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		result.emplace_back(vectors[n]);
	}

	return result;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const VectorT2<T>& vector)
{
	stream << "[" << vector.x() << ", " << vector.y() << "]";

	return stream;
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const VectorT2<T>& vector)
{
	return messageObject << "[" << vector.x() << ", " << vector.y() << "]";
}

template <bool tActive, typename T>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const VectorT2<T>& vector)
{
	return messageObject << "[" << vector.x() << ", " << vector.y() << "]";
}

}

#endif // META_OCEAN_MATH_VECTOR2_H
