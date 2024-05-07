// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_MATH_SCALAR_AUTOMATIC_DIFFERENTIATION_H
#define META_OCEAN_MATH_SCALAR_AUTOMATIC_DIFFERENTIATION_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"

namespace Ocean
{

// Forward declaration.
template <typename T> class ScalarAutomaticDifferentiationT;

/**
 * Definition of a scalar differentiation object using the data type of Scalar as parameter.
 * @see ScalarAutomaticDifferentiationT
 * @ingroup math
 */
typedef ScalarAutomaticDifferentiationT<Scalar> ScalarAutomaticDifferentiation;

/**
 * Definition of a scalar differentiation object using double as data type.
 * @see ScalarAutomaticDifferentiationT
 * @ingroup math
 */
typedef ScalarAutomaticDifferentiationT<double> ScalarAutomaticDifferentiationD;

/**
 * Definition of a scalar differentiation object using float as data type.
 * @see ScalarAutomaticDifferentiationT
 * @ingroup math
 */
typedef ScalarAutomaticDifferentiationT<float> ScalarAutomaticDifferentiationF;

/**
 * This class implements an automatic differentiation functionality for scalar values.
 * The automatic differentiation is realized by a pair of two values using the forward mode: the actual scalar value of a function and the corresponding derivative at this location.<br>
 * Therefore, each object holds the value of x and x' for a given parameter x.<br>
 * Automatic differentiation is a nice tool for fast prototyping of e.g., non-linear optimization functions.<br>
 * The accuracy of the resulting derivative is almost ideal and significantly better compared to numerical differentiation.<br>
 * However, in general the performance of the automatic differentiation (using the forward method) will be at least two times slower than calculating the derivative directory.<br>
 * The following code snippet shows a simple example how the ScalarAutomaticDifferentiation class can be used.
 * @code
 * // the following line determines the derivative f'(x) of f(x) = x^2 for x = 5.5
 * const ScalarAutomaticDifferentiation automaticDerivative0 = ScalarAutomaticDifferentiation(5.5) * ScalarAutomaticDifferentiation(5.5);
 * const Scalar derivative0 = automaticDerivative0.derivative();
 *
 * // the following line determines the derivative of f'(x) of f(x) = x * sin(x) + 7 for x = 3
 * const ScalarAutomaticDifferentiation automaticDerivative1 = ScalarAutomaticDifferentiation(3) * ScalarAutomaticDifferentiation::sin(ScalarAutomaticDifferentiation(3)) + 7;
 * const Scalar derivative1 = automaticDerivative1.derivative();
 * @endcode
 *
 * In the case a Jacobian matrix needs to be determined the constructor with additional boolean parameter may be used for calculations:
 * @code
 * // we determine the 1x2 Jacobian matrix for f(x, y) = x^2 + 3y + 5
 * // the Jacobian will have the following layout:
 * // | df/dx   df/dy |
 *
 * Scalar jacobian[2];
 *
 * Scalar x = 3; // any value for x
 * Scalar y = 7; // any value for y
 *
 * for (unsigned int n = 0u; n < 2u; ++n)
 * {
 *     const ScalarAutomaticDifferentiation dx = ScalarAutomaticDifferentiation(x, n == 0u) * ScalarAutomaticDifferentiation(x, n == 0u);
 *     const ScalarAutomaticDifferentiation dy = ScalarAutomaticDifferentiation(y, n == 1u) * 3;
 *
 *     const ScalarAutomaticDifferentiation d = dx + dy + 5;
 *     jacobian[n] = d(); // or d.derivative();
 * }
 * @endcode
 * @tparam T The data type of the scalar
 * @ingroup math
 */
template <typename T>
class ScalarAutomaticDifferentiationT
{
	template <typename T1, typename T2> friend ScalarAutomaticDifferentiationT<T1> operator+(const T2& left, const ScalarAutomaticDifferentiationT<T1>& right);
	template <typename T1, typename T2> friend ScalarAutomaticDifferentiationT<T1> operator-(const T2& left, const ScalarAutomaticDifferentiationT<T1>& right);
	template <typename T1, typename T2> friend ScalarAutomaticDifferentiationT<T1> operator*(const T2& left, const ScalarAutomaticDifferentiationT<T1>& right);
	template <typename T1, typename T2> friend ScalarAutomaticDifferentiationT<T1> operator/(const T2& left, const ScalarAutomaticDifferentiationT<T1>& right);

	public:

		/**
		 * Creates a new differentiation object wihtout initializing the parameters.
		 */
		inline ScalarAutomaticDifferentiationT();

		/**
		 * Creates a new differentiation object for a given scalar value (not a constant.
		 * The derivative for the provided scalar value will be set to 1.
		 * @param value The scalar value defining the object
		 */
		explicit inline ScalarAutomaticDifferentiationT(const T& value);

		/**
		 * Creates a new differentiation object by a given scalar and it's known derivative of the function at the specified location 'value'.
		 * @param value The scalar value defining the object
		 * @param derivative The derivative of the function at location 'value', e.g., 1 for a scalar parameter, 0 for a constant
		 */
		inline ScalarAutomaticDifferentiationT(const T& value, const T& derivative);

		/**
		 * Creates a new differentiation object by a given scalar or constant value, while a boolean state specifies whether the parameter is a scalar or a constant.
		 * @param value The scalar or constant value defining the object
		 * @param isVariable True, if the provided value is a scalar (with derivative 1); False, if the provided value is a constant (with derivative 0)
		 */
		inline ScalarAutomaticDifferentiationT(const T& value, const bool isVariable);

		/**
		 * Returns the actual derivative value of this object.
		 * @return The object's derivative value
		 */
		inline const T& derivative() const;

		/**
		 * Returns the actual derivative value of this object.
		 * @return The object's derivative value
		 */
		inline const T& operator()() const;

		/**
		 * Adds a scalar value to this differentiation object.
		 * @param right The scalar value
		 * @return The differntiation object with added scalar
		 */
		inline ScalarAutomaticDifferentiationT<T> operator+(const T& right) const;

		/**
		 * Adds a scalar value to this differentiation object.
		 * @param right The scalar value
		 * @return The reference to this object
		 */
		inline ScalarAutomaticDifferentiationT<T>& operator+=(const T& right);

		/**
		 * Adds two differentiation objects and determines the sum derivative.
		 * @param right The right differentiation object
		 * @return The sum derivative
		 */
		inline ScalarAutomaticDifferentiationT<T> operator+(const ScalarAutomaticDifferentiationT<T>& right) const;

		/**
		 * Adds two differentiation objects and determines the sum derivative.
		 * @param right The right differentiation object
		 * @return The reference to this object
		 */
		inline ScalarAutomaticDifferentiationT<T>& operator+=(const ScalarAutomaticDifferentiationT<T>& right);

		/**
		 * Subtracts a scalar value from this differentiation object.
		 * @param right The scalar value
		 * @return The differentiation object with subtracted scalar
		 */
		inline ScalarAutomaticDifferentiationT<T> operator-(const T& right) const;

		/**
		 * Subtracts a scalar value from this differentiation object.
		 * @param right The scalar value
		 * @return The reference to this object
		 */
		inline ScalarAutomaticDifferentiationT<T>& operator-=(const T& right);

		/**
		 * Subtracts two differentiation objects and determines the resulting derivative.
		 * @param right The right differentiation object
		 * @return The resulting derivative
		 */
		inline ScalarAutomaticDifferentiationT<T> operator-(const ScalarAutomaticDifferentiationT<T>& right) const;

		/**
		 * Subtracts two differentiation objects and determines the resulting derivative.
		 * @param right The right differentiation object
		 * @return The reference to this object
		 */
		inline ScalarAutomaticDifferentiationT<T>& operator-=(const ScalarAutomaticDifferentiationT<T>& right);

		/**
		 * Multiplies two differentiation objects and determines the product derivative.
		 * @param right The right differentiation object
		 * @return The product derivative
		 */
		inline ScalarAutomaticDifferentiationT<T> operator*(const ScalarAutomaticDifferentiationT<T>& right) const;

		/**
		 * Multiplies two differentiation objects and determines the product derivative.
		 * @param right The right differentiation object
		 * @return The reference to this object
		 */
		inline ScalarAutomaticDifferentiationT<T>& operator*=(const ScalarAutomaticDifferentiationT<T>& right);

		/**
		 * Multiplies this differentiation objects with a scalar.
		 * @param right The right scalar value
		 * @return The resulting differentiation object
		 */
		inline ScalarAutomaticDifferentiationT<T> operator*(const T& right) const;

		/**
		 * Multiplies this differentiation objects with a scalar.
		 * @param right The right scalar value
		 * @return The reference to this object
		 */
		inline ScalarAutomaticDifferentiationT<T>& operator*=(const T& right);

		/**
		 * Divides two differentiation objects and determines the quotient derivative.
		 * @param right The right differentiation object, while the object's value must not be zero
		 * @return The quotient derivative
		 */
		inline ScalarAutomaticDifferentiationT<T> operator/(const ScalarAutomaticDifferentiationT<T>& right) const;

		/**
		 * Divides two differentiation objects and determines the quotient derivative.
		 * @param right The right differentiation object, while the object's value must not be zero
		 * @return The reference to this object
		 */
		inline ScalarAutomaticDifferentiationT<T>& operator/=(const ScalarAutomaticDifferentiationT<T>& right);

		/**
		 * Divides this differentiation object by a scalar value.
		 * @param right The right scalar value, must not be zero
		 * @return The resulting differentiation object
		 */
		inline ScalarAutomaticDifferentiationT<T> operator/(const T& right) const;

		/**
		 * Divides this differentiation object by a scalar value.
		 * @param right The right scalar value, must not be zero
		 * @return The reference to this object
		 */
		inline ScalarAutomaticDifferentiationT<T>& operator/=(const T& right);

		/**
		 * Determines the derivative of the sinus function.
		 * @param value The value for which the derivative will be determined, in radians
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> sin(const ScalarAutomaticDifferentiationT<T>& value);

		/**
		 * Determines the derivative of the cosine function.
		 * @param value The value for which the derivative will be determined, in radians
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> cos(const ScalarAutomaticDifferentiationT<T>& value);

		/**
		 * Determines the derivative of the tangent function.
		 * @param value The value for which the derivative will be determined, in radian
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> tan(const ScalarAutomaticDifferentiationT<T>& value);

		/**
		 * Determines the derivative of the square root function.
		 * @param value The value for which the derivative will be determined, with value range [0, infinity)
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> sqrt(const ScalarAutomaticDifferentiationT<T>& value);

		/**
		 * Determines the derivative of the square function.
		 * @param value The value for which the derivative will be determined
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> sqr(const ScalarAutomaticDifferentiationT<T>& value);

		/**
		 * Determines the derivative of the exponential function.
		 * @param value The value for which the derivative will be determined
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> exp(const ScalarAutomaticDifferentiationT<T>& value);

		/**
		 * Determines the derivative of the natural logarithm.
		 * @param value The value for which the derivative will be determined, with value range (0, infinity)
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> log(const ScalarAutomaticDifferentiationT<T>& value);

		/**
		 * Determines the derivative of the logarithm to the base 2.
		 * @param value The value for which the derivative will be determined, with value range (0, infinity)
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> log2(const ScalarAutomaticDifferentiationT<T>& value);

		/**
		 * Determines the derivative of the logarithm to the base 10.
		 * @param value The value for which the derivative will be determined, with value range (0, infinity)
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> log10(const ScalarAutomaticDifferentiationT<T>& value);

		/**
		 * Determines the derivative of the power function calculating x to the power of y.
		 * @param x The value for which the derivative will be determined, with value range [0, infinity)
		 * @param y The exponent, with range (-infinity, infinity)
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> pow(const ScalarAutomaticDifferentiationT<T>& x, const T& y);

		/**
		 * Determines the derivative of the abs function.
		 * @param value The value for which the derivative will be determined
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> abs(const ScalarAutomaticDifferentiationT<T>& value);

		/**
		 * Determines the derivative of the min function.
		 * @param value The value for which the derivative will be determined
		 * @param second The second scalar value that will be used for minimum comparison
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> min(const ScalarAutomaticDifferentiationT<T>& value, const T& second);

		/**
		 * Determines the derivative of the max function.
		 * @param value The value for which the derivative will be determined
		 * @param second The second scalar value that will be used for maximum comparison
		 * @return The resulting derivative value
		 */
		static inline ScalarAutomaticDifferentiationT<T> max(const ScalarAutomaticDifferentiationT<T>& value, const T& second);

	protected:

		/// The scalar value of this object.
		T scalarValue;

		/// The actual derivative of this object.
		T scalarDerivative;
};

template <typename T>
inline ScalarAutomaticDifferentiationT<T>::ScalarAutomaticDifferentiationT()
{
	// nothing to do here
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T>::ScalarAutomaticDifferentiationT(const T& value) :
	scalarValue(value),
	scalarDerivative(value == T(0) ? T(0) : T(1))
{
	// x' = 1, if x != 0
	// x' = 1, if x == 0
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T>::ScalarAutomaticDifferentiationT(const T& value, const T& derivative) :
	scalarValue(value),
	scalarDerivative(derivative)
{
	// nothing to do here
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T>::ScalarAutomaticDifferentiationT(const T& value, const bool isVariable) :
	scalarValue(value),
	scalarDerivative(isVariable ? T(1) : T(0))
{
	// nothing to do here
}

template <typename T>
inline const T& ScalarAutomaticDifferentiationT<T>::derivative() const
{
	return scalarDerivative;
}

template <typename T>
inline const T& ScalarAutomaticDifferentiationT<T>::operator()() const
{
	return scalarDerivative;
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::operator+(const T& right) const
{
	// f(x) = x + c
	// f'(x) = x'

	return ScalarAutomaticDifferentiationT<T>(scalarValue + right, scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T>& ScalarAutomaticDifferentiationT<T>::operator+=(const T& right)
{
	scalarValue += right;
	return *this;
}

template <typename T1, typename T2>
inline ScalarAutomaticDifferentiationT<T1> operator+(const T2& left, const ScalarAutomaticDifferentiationT<T1>& right)
{
	// f(x) = c + x
	// f'(x) = x'

	return ScalarAutomaticDifferentiationT<T1>(T1(left) + right.scalarValue, right.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::operator+(const ScalarAutomaticDifferentiationT<T>& right) const
{
	// u' + v' = (u' + v')
	return ScalarAutomaticDifferentiationT<T>(scalarValue + right.scalarValue, scalarDerivative + right.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T>& ScalarAutomaticDifferentiationT<T>::operator+=(const ScalarAutomaticDifferentiationT<T>& right)
{
	scalarValue += right.scalarValue;
	scalarDerivative += right.scalarDerivative;

	return *this;
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::operator-(const ScalarAutomaticDifferentiationT<T>& right) const
{
	// (u - v)' = u' - v'
	return ScalarAutomaticDifferentiationT<T>(scalarValue - right.scalarValue, scalarDerivative - right.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T>& ScalarAutomaticDifferentiationT<T>::operator-=(const ScalarAutomaticDifferentiationT<T>& right)
{
	scalarValue -= right.scalarValue;
	scalarDerivative -= right.scalarDerivative;

	return *this;
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::operator-(const T& right) const
{
	// f(x) = x - c
	// f'(x) = x'

	return ScalarAutomaticDifferentiationT<T>(scalarValue - right, scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T>& ScalarAutomaticDifferentiationT<T>::operator-=(const T& right)
{
	scalarValue -= right;
	return *this;
}

template <typename T1, typename T2>
inline ScalarAutomaticDifferentiationT<T1> operator-(const T2& left, const ScalarAutomaticDifferentiationT<T1>& right)
{
	// f(x) = c - x
	// f'(x) = -x'

	return ScalarAutomaticDifferentiationT<T1>(T1(left) - right.scalarValue, -right.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::operator*(const ScalarAutomaticDifferentiationT<T>& right) const
{
	// u' * v' = u' * v + u * v'

	return ScalarAutomaticDifferentiationT<T>(scalarValue * right.scalarValue, scalarDerivative * right.scalarValue + scalarValue * right.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T>& ScalarAutomaticDifferentiationT<T>::operator*=(const ScalarAutomaticDifferentiationT<T>& right)
{
	*this = *this * right;
	return *this;
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::operator*(const T& right) const
{
	// f(x) = x * c
	// f'(x) = x' * c

	return ScalarAutomaticDifferentiationT<T>(scalarValue * right, scalarDerivative * right);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T>& ScalarAutomaticDifferentiationT<T>::operator*=(const T& right)
{
	scalarValue *= right;
	return *this;
}

template <typename T1, typename T2>
inline ScalarAutomaticDifferentiationT<T1> operator*(const T2& left, const ScalarAutomaticDifferentiationT<T1>& right)
{
	// f(x) = c * x
	// f'(x) = c * x'

	return ScalarAutomaticDifferentiationT<T1>(T1(left) * right.scalarValue, T1(left) * right.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::operator/(const ScalarAutomaticDifferentiationT<T>& right) const
{
	// (u / v)' = (u' * v - u * v') / v^2

	ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isNotEqualEps(right.scalarValue));

	return ScalarAutomaticDifferentiationT<T>(scalarValue / right.scalarValue, (scalarDerivative * right.scalarValue - scalarValue * right.scalarDerivative) / (right.scalarValue * right.scalarValue));
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T>& ScalarAutomaticDifferentiationT<T>::operator/=(const ScalarAutomaticDifferentiationT& right)
{
	*this = *this / right;
	return *this;
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::operator/(const T& right) const
{
	// f(x) = x / c
	// f'(x) = x' / c

	ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isNotEqualEps(right));

	return ScalarAutomaticDifferentiationT<T>(scalarValue / right, scalarDerivative / right);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T>& ScalarAutomaticDifferentiationT<T>::operator/=(const T& right)
{
	ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isNotEqualEps(right));

	scalarValue /= right;
	scalarDerivative /= right;

	return *this;
}

template <typename T1, typename T2>
inline ScalarAutomaticDifferentiationT<T1> operator/(const T2& left, const ScalarAutomaticDifferentiationT<T1>& right)
{
	// f(x) = c / x = c * x^-1
	// f'(x) = -c / x^2

	ocean_assert((std::is_same<T1, float>::value) || (std::is_same<T2, float>::value) || NumericT<T1>::isNotEqualEps(right.scalarValue));

	return ScalarAutomaticDifferentiationT<T1>(T1(left) / right.scalarValue, -T1(left) / (right.scalarValue * right.scalarValue) * right.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::sin(const ScalarAutomaticDifferentiationT<T>& value)
{
	// f(x) = sin(x)
	// f'(x) = cos(x) * x'

	return ScalarAutomaticDifferentiationT<T>(NumericT<T>::sin(value.scalarValue), NumericT<T>::cos(value.scalarValue) * value.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::cos(const ScalarAutomaticDifferentiationT<T>& value)
{
	// f(x) = cos(x)
	// f'(x) = -sin(x) * x'

	return ScalarAutomaticDifferentiationT<T>(NumericT<T>::cos(value.scalarValue), -NumericT<T>::sin(value.scalarValue) * value.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::tan(const ScalarAutomaticDifferentiationT<T>& value)
{
	// f(x) = tan(x)
	// f'(x) = 1 / (cos(x) * cos(x)) * x'

	return ScalarAutomaticDifferentiationT<T>(NumericT<T>::tan(value.scalarValue), value.scalarDerivative / NumericT<T>::sqr(NumericT<T>::cos(value.scalarValue)));
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::sqrt(const ScalarAutomaticDifferentiationT<T>& value)
{
	// f(x) = sqrt(x)
	// f'(x) = 1 / (2 * sqrt(x)) * x'

	ocean_assert(value.scalarValue >= T(0));

	const T sqrtValue = NumericT<T>::sqrt(value.scalarValue);

	return ScalarAutomaticDifferentiationT<T>(sqrtValue, T(0.5) * value.scalarDerivative / sqrtValue);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::sqr(const ScalarAutomaticDifferentiationT<T>& value)
{
	// f(x) = x^2
	// f'(x) = 2x * x'

	return ScalarAutomaticDifferentiationT<T>(value.scalarValue * value.scalarValue, T(2) * value.scalarValue * value.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::exp(const ScalarAutomaticDifferentiationT<T>& value)
{
	// f(x) = exp(x) = e^x
	// f'(x) = e^x * x'

	const T expValue = NumericT<T>::exp(value.scalarValue);

	return ScalarAutomaticDifferentiationT<T>(expValue, expValue * value.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::log(const ScalarAutomaticDifferentiationT<T>& value)
{
	// f(x) = log(x)
	// f'(x) = x' / x

	ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isNotEqualEps(value.scalarValue));

	return ScalarAutomaticDifferentiationT<T>(NumericT<T>::log(value.scalarValue), value.scalarDerivative / value.scalarValue);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::log2(const ScalarAutomaticDifferentiationT<T>& value)
{
	// f(x) = log_2(x)
	// f'(x) = x' / (x * log(2))

	ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isNotEqualEps(value.scalarValue));

	return ScalarAutomaticDifferentiationT<T>(NumericT<T>::log2(value.scalarValue), value.scalarDerivative / (value.scalarValue * T(0.69314718055994530941723212145818)));
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::log10(const ScalarAutomaticDifferentiationT<T>& value)
{
	// f(x) = log_10(x)
	// f'(x) = x' / (x * log(10))

	ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isNotEqualEps(value.scalarValue));

	return ScalarAutomaticDifferentiationT<T>(NumericT<T>::log10(value.scalarValue), value.scalarDerivative / (value.scalarValue * T(2.3025850929940456840179914546844)));
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::pow(const ScalarAutomaticDifferentiationT<T>& x, const T& y)
{
	// f(x, y) = x^y
	// f'(x) = y * x^(y - 1) * x'

	ocean_assert(x.scalarValue >= T(0));

	return ScalarAutomaticDifferentiationT<T>(NumericT<T>::pow(x.scalarValue, y), y * NumericT<T>::pow(x.scalarValue, y - T(1)) * x.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::abs(const ScalarAutomaticDifferentiationT<T>& value)
{
	// f(x) = |x|
	// f'(x) = sign(x) * x'

	return ScalarAutomaticDifferentiationT<T>(NumericT<T>::abs(value.scalarValue), value.scalarValue >= T(0) ? value.scalarDerivative : -value.scalarDerivative);
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::min(const ScalarAutomaticDifferentiationT<T>& value, const T& second)
{
	// f(x) = min(x, c)
	//         | x', x < c
	// f'(x) = | 0, x >= c

	if (value.scalarValue < second)
	{
		return ScalarAutomaticDifferentiationT<T>(value.scalarValue, value.scalarDerivative);
	}
	else
	{
		return ScalarAutomaticDifferentiationT<T>(second, T(0));
	}
}

template <typename T>
inline ScalarAutomaticDifferentiationT<T> ScalarAutomaticDifferentiationT<T>::max(const ScalarAutomaticDifferentiationT<T>& value, const T& second)
{
	// f(x) = max(x, c)
	//         | x', x > c
	// f'(x) = | 0, x <= c

	if (value.scalarValue > second)
	{
		return ScalarAutomaticDifferentiationT<T>(value.scalarValue, value.scalarDerivative);
	}
	else
	{
		return ScalarAutomaticDifferentiationT<T>(second, T(0));
	}
}

}

#endif // META_OCEAN_MATH_SCALAR_AUTOMATIC_DIFFERENTIATION_H
