/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_AUTOMATIC_DIFFERENTIATION_H
#define META_OCEAN_MATH_AUTOMATIC_DIFFERENTIATION_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"

namespace Ocean
{

// Forward declaration.
template <typename T, typename TNumeric> class AutomaticDifferentiationT;

/**
 * Definition of a scalar differentiation object using the data type of Scalar as parameter.
 * @see AutomaticDifferentiationT
 * @ingroup math
 */
typedef AutomaticDifferentiationT<Scalar, Numeric> AutomaticDifferentiation;

/**
 * Definition of a scalar differentiation object using double as data type.
 * @see AutomaticDifferentiationT
 * @ingroup math
 */
typedef AutomaticDifferentiationT<double, NumericD> AutomaticDifferentiationD;

/**
 * Definition of a scalar differentiation object using float as data type.
 * @see AutomaticDifferentiationT
 * @ingroup math
 */
typedef AutomaticDifferentiationT<float, NumericF> AutomaticDifferentiationF;

/**
 * This class implements an automatic differentiation functionality.
 * The automatic differentiation is realized by a pair of two values using the forward mode: the actual value of a function and the corresponding derivative at this location.<br>
 * Therefore, each object holds the value of x and x' for a given parameter x.<br>
 * Automatic differentiation is a nice tool for fast prototyping of e.g., non-linear optimization functions.<br>
 * The accuracy of the resulting derivative is almost ideal and significantly better compared to numerical differentiation.<br>
 * However, in general the performance of the automatic differentiation (using the forward method) will be at least two times slower than calculating the derivative directory.<br>
 * The following code snippet shows a simple example how the AutomaticDifferentiation class can be used.
 * @code
 * // the following line determines the derivative f'(x) of f(x) = x^2 for x = 5.5
 * const AutomaticDifferentiation automaticDerivative0 = AutomaticDifferentiation(5.5) * AutomaticDifferentiation(5.5);
 * const Scalar derivative0 = automaticDerivative0.derivative();
 *
 * // the following line determines the derivative of f'(x) of f(x) = x * sin(x) + 7 for x = 3
 * const AutomaticDifferentiation automaticDerivative1 = AutomaticDifferentiation(3) * AutomaticDifferentiation::sin(AutomaticDifferentiation(3)) + 7;
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
 *     const AutomaticDifferentiation dx = AutomaticDifferentiation(x, n == 0u) * AutomaticDifferentiation(x, n == 0u);
 *     const AutomaticDifferentiation dy = AutomaticDifferentiation(y, n == 1u) * 3;
 *
 *     const AutomaticDifferentiation d = dx + dy + 5;
 *     jacobian[n] = d(); // or d.derivative();
 * }
 * @endcode
 * @tparam T The data type of the scalar
 * @tparam TNumeric The numeric class providing access to standard mathematical functions like sin, cos, sqrt, etc.
 * @ingroup math
 */
template <typename T, typename TNumeric = NumericT<T>>
class AutomaticDifferentiationT
{
	template <typename T1, typename TNumeric1, typename T2> friend AutomaticDifferentiationT<T1, TNumeric1> operator+(const T2& left, const AutomaticDifferentiationT<T1, TNumeric1>& right);
	template <typename T1, typename TNumeric1, typename T2> friend AutomaticDifferentiationT<T1, TNumeric1> operator-(const T2& left, const AutomaticDifferentiationT<T1, TNumeric1>& right);
	template <typename T1, typename TNumeric1, typename T2> friend AutomaticDifferentiationT<T1, TNumeric1> operator*(const T2& left, const AutomaticDifferentiationT<T1, TNumeric1>& right);
	template <typename T1, typename TNumeric1, typename T2> friend AutomaticDifferentiationT<T1, TNumeric1> operator/(const T2& left, const AutomaticDifferentiationT<T1, TNumeric1>& right);

	public:

		/**
		 * Creates a new differentiation object without initializing the parameters.
		 */
		AutomaticDifferentiationT() = default;

		/**
		 * Creates a new differentiation object for a given scalar value (not a constant).
		 * The derivative for the provided scalar value will be set to 1.
		 * @param value The scalar value defining the object
		 */
		explicit inline AutomaticDifferentiationT(const T& value);

		/**
		 * Creates a new differentiation object by a given scalar and it's known derivative of the function at the specified location 'value'.
		 * @param value The scalar value defining the object
		 * @param derivative The derivative of the function at location 'value', e.g., 1 for a scalar parameter, 0 for a constant
		 */
		inline AutomaticDifferentiationT(const T& value, const T& derivative);

		/**
		 * Creates a new differentiation object by a given scalar or constant value, while a boolean state specifies whether the parameter is a scalar or a constant.
		 * @param value The scalar or constant value defining the object
		 * @param isVariable True, if the provided value is a scalar (with derivative 1); False, if the provided value is a constant (with derivative 0)
		 */
		inline AutomaticDifferentiationT(const T& value, const bool isVariable);

		/**
		 * Returns the actual derivative of this object.
		 * @return The object's derivative
		 */
		inline const T& derivative() const;

		/**
		 * Returns the value of this object.
		 * @return The object's value
		 */
		inline const T& value() const;

		/**
		 * Returns the actual derivative value of this object.
		 * @return The object's derivative value
		 */
		inline const T& operator()() const;

		/**
		 * Adds a scalar value to this differentiation object.
		 * @param right The scalar value
		 * @return The differentiation object with added scalar
		 */
		inline AutomaticDifferentiationT<T, TNumeric> operator+(const T& right) const;

		/**
		 * Adds a scalar value to this differentiation object.
		 * @param right The scalar value
		 * @return The reference to this object
		 */
		inline AutomaticDifferentiationT<T, TNumeric>& operator+=(const T& right);

		/**
		 * Adds two differentiation objects and determines the sum derivative.
		 * @param right The right differentiation object
		 * @return The sum derivative
		 */
		inline AutomaticDifferentiationT<T, TNumeric> operator+(const AutomaticDifferentiationT<T, TNumeric>& right) const;

		/**
		 * Adds two differentiation objects and determines the sum derivative.
		 * @param right The right differentiation object
		 * @return The reference to this object
		 */
		inline AutomaticDifferentiationT<T, TNumeric>& operator+=(const AutomaticDifferentiationT<T, TNumeric>& right);

		/**
		 * Subtracts a scalar value from this differentiation object.
		 * @param right The scalar value
		 * @return The differentiation object with subtracted scalar
		 */
		inline AutomaticDifferentiationT<T, TNumeric> operator-(const T& right) const;

		/**
		 * Subtracts a scalar value from this differentiation object.
		 * @param right The scalar value
		 * @return The reference to this object
		 */
		inline AutomaticDifferentiationT<T, TNumeric>& operator-=(const T& right);

		/**
		 * Subtracts two differentiation objects and determines the resulting derivative.
		 * @param right The right differentiation object
		 * @return The resulting derivative
		 */
		inline AutomaticDifferentiationT<T, TNumeric> operator-(const AutomaticDifferentiationT<T, TNumeric>& right) const;

		/**
		 * Unary negation operator returns the negative of this differentiation object.
		 * @return The negative differentiation object
		 */
		inline AutomaticDifferentiationT<T, TNumeric> operator-() const;

		/**
		 * Subtracts two differentiation objects and determines the resulting derivative.
		 * @param right The right differentiation object
		 * @return The reference to this object
		 */
		inline AutomaticDifferentiationT<T, TNumeric>& operator-=(const AutomaticDifferentiationT<T, TNumeric>& right);

		/**
		 * Multiplies two differentiation objects and determines the product derivative.
		 * @param right The right differentiation object
		 * @return The product derivative
		 */
		inline AutomaticDifferentiationT<T, TNumeric> operator*(const AutomaticDifferentiationT<T, TNumeric>& right) const;

		/**
		 * Multiplies two differentiation objects and determines the product derivative.
		 * @param right The right differentiation object
		 * @return The reference to this object
		 */
		inline AutomaticDifferentiationT<T, TNumeric>& operator*=(const AutomaticDifferentiationT<T, TNumeric>& right);

		/**
		 * Multiplies this differentiation objects with a scalar.
		 * @param right The right scalar value
		 * @return The resulting differentiation object
		 */
		inline AutomaticDifferentiationT<T, TNumeric> operator*(const T& right) const;

		/**
		 * Multiplies this differentiation objects with a scalar.
		 * @param right The right scalar value
		 * @return The reference to this object
		 */
		inline AutomaticDifferentiationT<T, TNumeric>& operator*=(const T& right);

		/**
		 * Divides two differentiation objects and determines the quotient derivative.
		 * @param right The right differentiation object, while the object's value must not be zero
		 * @return The quotient derivative
		 */
		inline AutomaticDifferentiationT<T, TNumeric> operator/(const AutomaticDifferentiationT<T, TNumeric>& right) const;

		/**
		 * Divides two differentiation objects and determines the quotient derivative.
		 * @param right The right differentiation object, while the object's value must not be zero
		 * @return The reference to this object
		 */
		inline AutomaticDifferentiationT<T, TNumeric>& operator/=(const AutomaticDifferentiationT<T, TNumeric>& right);

		/**
		 * Divides this differentiation object by a scalar value.
		 * @param right The right scalar value, must not be zero
		 * @return The resulting differentiation object
		 */
		inline AutomaticDifferentiationT<T, TNumeric> operator/(const T& right) const;

		/**
		 * Divides this differentiation object by a scalar value.
		 * @param right The right scalar value, must not be zero
		 * @return The reference to this object
		 */
		inline AutomaticDifferentiationT<T, TNumeric>& operator/=(const T& right);

		/**
		 * Determines the derivative of the sinus function.
		 * @param value The value for which the derivative will be determined, in radians
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> sin(const AutomaticDifferentiationT<T, TNumeric>& value);

		/**
		 * Determines the derivative of the cosine function.
		 * @param value The value for which the derivative will be determined, in radians
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> cos(const AutomaticDifferentiationT<T, TNumeric>& value);

		/**
		 * Determines the derivative of the tangent function.
		 * @param value The value for which the derivative will be determined, in radian
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> tan(const AutomaticDifferentiationT<T, TNumeric>& value);

		/**
		 * Determines the derivative of the square root function.
		 * @param value The value for which the derivative will be determined, with value range [0, infinity)
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> sqrt(const AutomaticDifferentiationT<T, TNumeric>& value);

		/**
		 * Determines the derivative of the square function.
		 * @param value The value for which the derivative will be determined
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> sqr(const AutomaticDifferentiationT<T, TNumeric>& value);

		/**
		 * Determines the derivative of the exponential function.
		 * @param value The value for which the derivative will be determined
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> exp(const AutomaticDifferentiationT<T, TNumeric>& value);

		/**
		 * Determines the derivative of the natural logarithm.
		 * @param value The value for which the derivative will be determined, with value range (0, infinity)
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> log(const AutomaticDifferentiationT<T, TNumeric>& value);

		/**
		 * Determines the derivative of the logarithm to the base 2.
		 * @param value The value for which the derivative will be determined, with value range (0, infinity)
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> log2(const AutomaticDifferentiationT<T, TNumeric>& value);

		/**
		 * Determines the derivative of the logarithm to the base 10.
		 * @param value The value for which the derivative will be determined, with value range (0, infinity)
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> log10(const AutomaticDifferentiationT<T, TNumeric>& value);

		/**
		 * Determines the derivative of the power function calculating x to the power of y.
		 * @param x The value for which the derivative will be determined, with value range [0, infinity)
		 * @param y The exponent, with range (-infinity, infinity)
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> pow(const AutomaticDifferentiationT<T, TNumeric>& x, const T& y);

		/**
		 * Determines the derivative of the abs function.
		 * @param value The value for which the derivative will be determined
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> abs(const AutomaticDifferentiationT<T, TNumeric>& value);

		/**
		 * Determines the derivative of the min function.
		 * @param value The value for which the derivative will be determined
		 * @param second The second scalar value that will be used for minimum comparison
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> min(const AutomaticDifferentiationT<T, TNumeric>& value, const T& second);

		/**
		 * Determines the derivative of the max function.
		 * @param value The value for which the derivative will be determined
		 * @param second The second scalar value that will be used for maximum comparison
		 * @return The resulting derivative value
		 */
		static inline AutomaticDifferentiationT<T, TNumeric> max(const AutomaticDifferentiationT<T, TNumeric>& value, const T& second);

	protected:

		/// The scalar value of this object.
		T value_ = T(0);

		/// The actual derivative of this object.
		T derivative_ = T(0);
};

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric>::AutomaticDifferentiationT(const T& value) :
	value_(value),
	derivative_(value == T(0) ? T(0) : T(1))
{
	// x' = 1, if x != 0
	// x' = 1, if x == 0
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric>::AutomaticDifferentiationT(const T& value, const T& derivative) :
	value_(value),
	derivative_(derivative)
{
	// nothing to do here
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric>::AutomaticDifferentiationT(const T& value, const bool isVariable) :
	value_(value),
	derivative_(isVariable ? T(1) : T(0))
{
	// nothing to do here
}

template <typename T, typename TNumeric>
inline const T& AutomaticDifferentiationT<T, TNumeric>::derivative() const
{
	return derivative_;
}

template <typename T, typename TNumeric>
inline const T& AutomaticDifferentiationT<T, TNumeric>::value() const
{
	return value_;
}

template <typename T, typename TNumeric>
inline const T& AutomaticDifferentiationT<T, TNumeric>::operator()() const
{
	return derivative_;
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::operator+(const T& right) const
{
	// f(x) = x + c
	// f'(x) = x'

	return AutomaticDifferentiationT<T, TNumeric>(value_ + right, derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric>& AutomaticDifferentiationT<T, TNumeric>::operator+=(const T& right)
{
	value_ += right;
	return *this;
}

template <typename T1, typename TNumeric1, typename T2>
inline AutomaticDifferentiationT<T1, TNumeric1> operator+(const T2& left, const AutomaticDifferentiationT<T1, TNumeric1>& right)
{
	// f(x) = c + x
	// f'(x) = x'

	return AutomaticDifferentiationT<T1, TNumeric1>(T1(left) + right.value_, right.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::operator+(const AutomaticDifferentiationT<T, TNumeric>& right) const
{
	// u' + v' = (u' + v')
	return AutomaticDifferentiationT<T, TNumeric>(value_ + right.value_, derivative_ + right.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric>& AutomaticDifferentiationT<T, TNumeric>::operator+=(const AutomaticDifferentiationT<T, TNumeric>& right)
{
	value_ += right.value_;
	derivative_ += right.derivative_;

	return *this;
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::operator-(const AutomaticDifferentiationT<T, TNumeric>& right) const
{
	// (u - v)' = u' - v'
	return AutomaticDifferentiationT<T, TNumeric>(value_ - right.value_, derivative_ - right.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::operator-() const
{
	// f(x) = -x
	// f'(x) = -x'

	return AutomaticDifferentiationT<T, TNumeric>(-value_, -derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric>& AutomaticDifferentiationT<T, TNumeric>::operator-=(const AutomaticDifferentiationT<T, TNumeric>& right)
{
	value_ -= right.value_;
	derivative_ -= right.derivative_;

	return *this;
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::operator-(const T& right) const
{
	// f(x) = x - c
	// f'(x) = x'

	return AutomaticDifferentiationT<T, TNumeric>(value_ - right, derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric>& AutomaticDifferentiationT<T, TNumeric>::operator-=(const T& right)
{
	value_ -= right;
	return *this;
}

template <typename T1, typename TNumeric1, typename T2>
inline AutomaticDifferentiationT<T1, TNumeric1> operator-(const T2& left, const AutomaticDifferentiationT<T1, TNumeric1>& right)
{
	// f(x) = c - x
	// f'(x) = -x'

	return AutomaticDifferentiationT<T1, TNumeric1>(T1(left) - right.value_, -right.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::operator*(const AutomaticDifferentiationT<T, TNumeric>& right) const
{
	// u' * v' = u' * v + u * v'

	return AutomaticDifferentiationT<T, TNumeric>(value_ * right.value_, derivative_ * right.value_ + value_ * right.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric>& AutomaticDifferentiationT<T, TNumeric>::operator*=(const AutomaticDifferentiationT<T, TNumeric>& right)
{
	*this = *this * right;
	return *this;
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::operator*(const T& right) const
{
	// f(x) = x * c
	// f'(x) = x' * c

	return AutomaticDifferentiationT<T, TNumeric>(value_ * right, derivative_ * right);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric>& AutomaticDifferentiationT<T, TNumeric>::operator*=(const T& right)
{
	value_ *= right;
	return *this;
}

template <typename T1, typename TNumeric1, typename T2>
inline AutomaticDifferentiationT<T1, TNumeric1> operator*(const T2& left, const AutomaticDifferentiationT<T1, TNumeric1>& right)
{
	// f(x) = c * x
	// f'(x) = c * x'

	return AutomaticDifferentiationT<T1, TNumeric1>(T1(left) * right.value_, T1(left) * right.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::operator/(const AutomaticDifferentiationT<T, TNumeric>& right) const
{
	// (u / v)' = (u' * v - u * v') / v^2

	ocean_assert((std::is_same<T, float>::value) || TNumeric::isNotEqualEps(right.value_));

	return AutomaticDifferentiationT<T, TNumeric>(value_ / right.value_, (derivative_ * right.value_ - value_ * right.derivative_) / (right.value_ * right.value_));
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric>& AutomaticDifferentiationT<T, TNumeric>::operator/=(const AutomaticDifferentiationT<T, TNumeric>& right)
{
	*this = *this / right;
	return *this;
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::operator/(const T& right) const
{
	// f(x) = x / c
	// f'(x) = x' / c

	ocean_assert((std::is_same<T, float>::value) || TNumeric::isNotEqualEps(right));

	return AutomaticDifferentiationT<T, TNumeric>(value_ / right, derivative_ / right);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric>& AutomaticDifferentiationT<T, TNumeric>::operator/=(const T& right)
{
	ocean_assert((std::is_same<T, float>::value) || TNumeric::isNotEqualEps(right));

	value_ /= right;
	derivative_ /= right;

	return *this;
}

template <typename T1, typename TNumeric1, typename T2>
inline AutomaticDifferentiationT<T1, TNumeric1> operator/(const T2& left, const AutomaticDifferentiationT<T1, TNumeric1>& right)
{
	// f(x) = c / x = c * x^-1
	// f'(x) = -c / x^2

	ocean_assert((std::is_same<T1, float>::value) || (std::is_same<T2, float>::value) || NumericT<T1>::isNotEqualEps(right.value_));

	return AutomaticDifferentiationT<T1, TNumeric1>(T1(left) / right.value_, -T1(left) / (right.value_ * right.value_) * right.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::sin(const AutomaticDifferentiationT<T, TNumeric>& value)
{
	// f(x) = sin(x)
	// f'(x) = cos(x) * x'

	return AutomaticDifferentiationT<T, TNumeric>(TNumeric::sin(value.value_), TNumeric::cos(value.value_) * value.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::cos(const AutomaticDifferentiationT<T, TNumeric>& value)
{
	// f(x) = cos(x)
	// f'(x) = -sin(x) * x'

	return AutomaticDifferentiationT<T, TNumeric>(TNumeric::cos(value.value_), -TNumeric::sin(value.value_) * value.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::tan(const AutomaticDifferentiationT<T, TNumeric>& value)
{
	// f(x) = tan(x)
	// f'(x) = 1 / (cos(x) * cos(x)) * x'

	return AutomaticDifferentiationT<T, TNumeric>(TNumeric::tan(value.value_), value.derivative_ / TNumeric::sqr(TNumeric::cos(value.value_)));
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::sqrt(const AutomaticDifferentiationT<T, TNumeric>& value)
{
	// f(x) = sqrt(x)
	// f'(x) = 1 / (2 * sqrt(x)) * x'

	ocean_assert(value.value_ >= T(0));

	const T sqrtValue = TNumeric::sqrt(value.value_);

	return AutomaticDifferentiationT<T, TNumeric>(sqrtValue, T(0.5) * value.derivative_ / sqrtValue);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::sqr(const AutomaticDifferentiationT<T, TNumeric>& value)
{
	// f(x) = x^2
	// f'(x) = 2x * x'

	return AutomaticDifferentiationT<T, TNumeric>(value.value_ * value.value_, T(2) * value.value_ * value.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::exp(const AutomaticDifferentiationT<T, TNumeric>& value)
{
	// f(x) = exp(x) = e^x
	// f'(x) = e^x * x'

	const T expValue = TNumeric::exp(value.value_);

	return AutomaticDifferentiationT<T, TNumeric>(expValue, expValue * value.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::log(const AutomaticDifferentiationT<T, TNumeric>& value)
{
	// f(x) = log(x)
	// f'(x) = x' / x

	ocean_assert((std::is_same<T, float>::value) || TNumeric::isNotEqualEps(value.value_));

	return AutomaticDifferentiationT<T, TNumeric>(TNumeric::log(value.value_), value.derivative_ / value.value_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::log2(const AutomaticDifferentiationT<T, TNumeric>& value)
{
	// f(x) = log_2(x)
	// f'(x) = x' / (x * log(2))

	ocean_assert((std::is_same<T, float>::value) || TNumeric::isNotEqualEps(value.value_));

	return AutomaticDifferentiationT<T, TNumeric>(TNumeric::log2(value.value_), value.derivative_ / (value.value_ * T(0.69314718055994530941723212145818)));
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::log10(const AutomaticDifferentiationT<T, TNumeric>& value)
{
	// f(x) = log_10(x)
	// f'(x) = x' / (x * log(10))

	ocean_assert((std::is_same<T, float>::value) || TNumeric::isNotEqualEps(value.value_));

	return AutomaticDifferentiationT<T, TNumeric>(TNumeric::log10(value.value_), value.derivative_ / (value.value_ * T(2.3025850929940456840179914546844)));
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::pow(const AutomaticDifferentiationT<T, TNumeric>& x, const T& y)
{
	// f(x, y) = x^y
	// f'(x) = y * x^(y - 1) * x'

	ocean_assert(x.value_ >= T(0));

	return AutomaticDifferentiationT<T, TNumeric>(TNumeric::pow(x.value_, y), y * TNumeric::pow(x.value_, y - T(1)) * x.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::abs(const AutomaticDifferentiationT<T, TNumeric>& value)
{
	// f(x) = |x|
	// f'(x) = sign(x) * x'

	return AutomaticDifferentiationT<T, TNumeric>(TNumeric::abs(value.value_), value.value_ >= T(0) ? value.derivative_ : -value.derivative_);
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::min(const AutomaticDifferentiationT<T, TNumeric>& value, const T& second)
{
	// f(x) = min(x, c)
	//         | x', x < c
	// f'(x) = | 0, x >= c

	if (value.value_ < second)
	{
		return AutomaticDifferentiationT<T, TNumeric>(value.value_, value.derivative_);
	}
	else
	{
		return AutomaticDifferentiationT<T, TNumeric>(second, T(0));
	}
}

template <typename T, typename TNumeric>
inline AutomaticDifferentiationT<T, TNumeric> AutomaticDifferentiationT<T, TNumeric>::max(const AutomaticDifferentiationT<T, TNumeric>& value, const T& second)
{
	// f(x) = max(x, c)
	//         | x', x > c
	// f'(x) = | 0, x <= c

	if (value.value_ > second)
	{
		return AutomaticDifferentiationT<T, TNumeric>(value.value_, value.derivative_);
	}
	else
	{
		return AutomaticDifferentiationT<T, TNumeric>(second, T(0));
	}
}

}

#endif // META_OCEAN_MATH_AUTOMATIC_DIFFERENTIATION_H
