/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_EQUATION_H
#define META_OCEAN_MATH_EQUATION_H

#include "ocean/math/Math.h"
#include "ocean/math/Numeric.h"

#include "ocean/base/Utilities.h"

#include <complex>

namespace Ocean
{

// Forward declaration.
template <typename T> class EquationT;

/**
 * Definition of the Equation object, depending on the OCEAN_MATH_USE_SINGLE_PRECISION either with single or double precision float data type.
 * @see EquationT
 * @ingroup math
 */
typedef EquationT<Scalar> Equation;

/**
 * Definition of the Equation class using double values
 * @see EquationT
 * @ingroup math
 */
typedef EquationT<double> EquationD;

/**
 * Definition of the Equation class using float values
 * @see EquationT
 * @ingroup math
 */
typedef EquationT<float> EquationF;

/**
 * This class provides several functions to solve equations with different degree using floating point values with the precission specified by type T.
 * @tparam T Type of passed floating point values
 * @see Equation, EquationF, EquationD.
 * @ingroup math
 */
template <typename T>
class EquationT
{
	public:

		/**
		 * Solves a linear eqation with the form:<br>
		 * ax + b = 0
		 * @param a A parameter, with range (-infinity, infinity) \ {0}, (must not be 0)
		 * @param b B parameter, with range (-infinity, infinity)
		 * @param x Resulting solution
		 * @return True, if succeeded
		 */
		static bool solveLinear(const T a, const T b, T& x);

		/**
		 * Solves an quadratic equation with the form:<br>
		 * ax^2 + bx + c = 0
		 * @param a A parameter, with range (-infinity, infinity) \ {0}, (must not be 0)
		 * @param b B parameter, with range (-infinity, infinity)
		 * @param c C parameter, with range (-infinity, infinity)
		 * @param x1 First resulting solution, with range (-infinity, infinity)
		 * @param x2 Second resulting solution, with range (-infinity, infinity)
		 * @return True, if succeeded
		 */
		static bool solveQuadratic(const T a, const T b, const T c, T& x1, T& x2);

		/**
		 * Solves a cubic equation with the from:<br>
		 * ax^3 + bx^2 + cx + d = 0
		 * @param a A parameter, with range (-infinity, infinity) \ {0}, (must not be 0)
		 * @param b B parameter, with range (-infinity, infinity)
		 * @param c C parameter, with range (-infinity, infinity)
		 * @param d D parameter, with range (-infinity, infinity)
		 * @param x1 First resulting solution, with range (-infinity, infinity)
		 * @param x2 Second resulting solution, with range (-infinity, infinity)
		 * @param x3 Third resulting solution, with range (-infinity, infinity)
		 * @return Number of solutions
		 */
		static unsigned int solveCubic(const T a, const T b, const T c, const T d, T& x1, T& x2, T& x3);

		/**
		 * Solves a quartic equation with the form:<br>
		 * ax^4 + bx^3 + cx^2 + dx + e = 0
		 * @param a A parameter, with range (-infinity, infinity) \ {0}, (must not be 0)
		 * @param b B parameter, with range (-infinity, infinity)
		 * @param c C parameter, with range (-infinity, infinity)
		 * @param d D parameter, with range (-infinity, infinity)
		 * @param e E parameter, with range (-infinity, infinity)
		 * @param x Array with at least four scalar values receiving the (at most) four solutions
		 * @return Number of solutions
		 */
		static unsigned int solveQuartic(const T a, const T b, const T c, const T d, const T e, T* x);
};

template <typename T>
bool EquationT<T>::solveLinear(const T a, const T b, T& x)
{
	ocean_assert(NumericT<T>::isNotEqualEps(a));

	// ax + b = 0

	if (NumericT<T>::isEqualEps(a))
	{
		return false;
	}

	x = -b / a;
	return true;
}

template <typename T>
bool EquationT<T>::solveQuadratic(const T a, const T b, const T c, T& x1, T& x2)
{
	ocean_assert(NumericT<T>::isNotEqualEps(a));

	// ax^2 + bx + c = 0
	// see Numerical Recipes in C++

	if (NumericT<T>::isEqualEps(a))
	{
		return false;
	}

	const T value = b * b - 4 * a * c;
	if (!NumericT<T>::isAbove(value, 0))
	{
		return false;
	}

	const T q = T(-0.5) * (b + ((value > T(0)) ? NumericT<T>::copySign(NumericT<T>::sqrt(value), b) : T(0)));

	if (NumericT<T>::isEqualEps(q))
	{
		x1 = 0;
		x2 = 0;
		return true;
	}

	x1 = q / a;
	x2 = c / q;

#ifdef OCEAN_DEBUG
	if (std::is_same<T, float>::value)
	{
		// for 32 bit float values we have to weaken the zero accuracy
		ocean_assert(NumericT<T>::isEqual(a * x1 * x1 + b * x1 + c, T(0), NumericT<T>::weakEps() * NumericT<T>::abs(x1)));
		ocean_assert(NumericT<T>::isEqual(a * x2 * x2 + b * x2 + c, T(0), NumericT<T>::weakEps() * NumericT<T>::abs(x2)));
	}
	else
	{
		ocean_assert(NumericT<T>::isWeakEqualEps(a * x1 * x1 + b * x1 + c));
		ocean_assert(NumericT<T>::isWeakEqualEps(a * x2 * x2 + b * x2 + c));
	}
#endif

	return true;
}

template <typename T>
unsigned int EquationT<T>::solveCubic(const T a, const T b, const T c, const T d, T& x1, T& x2, T& x3)
{
	ocean_assert(NumericT<T>::isNotEqualEps(a));

	// ax^3 + bx^2 + cx + d = 0
	// see Numerical Recipes in C++

	if (NumericT<T>::isEqualEps(a))
	{
		return 0u;
	}

	const T a1 = 1 / a;
	const T alpha = b * a1;
	const T beta = c * a1;
	const T gamma = d * a1;

	// x^3 + alpha x^2 + beta x + gamma = 0

	// alpha2 = alpha^2
	const T alpha2 = alpha * alpha;

	// q = (alpha^2 - 3b) / 9
	const T q = (alpha2 - 3 * beta) * T(0.11111111111111111111111111111111);

	// r = (2 alpha^3 - 9 alpha beta + 27gamma) / 54
	const T r = (2 * alpha2 * alpha - 9 * alpha * beta + 27 * gamma) * T(0.018518518518518518518518518518519);

	// r2 = r^2
	const T r2 = r * r;

	// q3 = q^3
	const T q3 = q * q * q;

	if (r2 <= q3 + Numeric::eps() && q > NumericT<T>::eps())
	{
		const T sqrtQ = NumericT<T>::sqrt(q);

		// angle = arccos(r / sqrt(q^3))
		// angle_3 = angle / 3
		const T angle_3 = NumericT<T>::acos(minmax<T>(-1, r / (q * sqrtQ), 1)) * T(0.33333333333333333333333333333333);

		// alpha_3 = alpha / 3
		const T alpha_3 = alpha * T(0.33333333333333333333333333333333);

		const T factor = -2 * sqrtQ;

		// x1 = -2 sqrt(q) * cos(angle / 3) - alpha / 3
		x1 = factor * NumericT<T>::cos(angle_3) - alpha_3;

		// x2 = -2 sqrt(q) * cos((angle + 2pi) / 3) - alpha / 3
		x2 = factor * NumericT<T>::cos(angle_3 + T(2.0943951023931954923084289221863)) - alpha_3;

		// x3 = -2 sqrt(q) * cos((angle - 2pi) / 3) - alpha / 3
		x3 = factor * NumericT<T>::cos(angle_3 - T(2.0943951023931954923084289221863)) - alpha_3;

#ifdef OCEAN_DEBUG

		const T value1 = a * x1 * x1 * x1 + b * x1 * x1 + c * x1 + d;
		const T value2 = a * x2 * x2 * x2 + b * x2 * x2 + c * x2 + d;
		const T value3 = a * x3 * x3 * x3 + b * x3 * x3 + c * x3 + d;

		// the accuracy for 32 bit float values may be very poor so that we cannot define any assert
		if (!std::is_same<T, float>::value)
		{
			ocean_assert(NumericT<T>::isEqual(value1, T(0), T(1e-3)));
			ocean_assert(NumericT<T>::isEqual(value2, T(0), T(1e-3)));
			ocean_assert(NumericT<T>::isEqual(value3, T(0), T(1e-3)));
		}
#endif

		return 3u;
	}

	ocean_assert(r2 - q3 >= -Numeric::eps());

	// m = -sign(r) * [abs(r) + sqrt(r^2 - q^3)]^(1/3)
	const T m = -NumericT<T>::copySign(pow(NumericT<T>::abs(r) + NumericT<T>::sqrt(std::max(T(0), r2 - q3)), T(0.33333333333333333333333333333333)), r);

	// n = 0, if m == 0
	// n = q / m, if m != 0
	const T n = NumericT<T>::isEqualEps(m) ? 0 : q / m;

	// x1 = (m + n) - alpha / 3
	x1 = m + n - alpha * T(0.33333333333333333333333333333333);

#ifdef OCEAN_DEBUG

	const T value1 = a * x1 * x1 * x1 + b * x1 * x1 + c * x1 + d;

	// the accuracy for 32 bit float values may be very poor so that we cannot define any assert
	if (!std::is_same<T, float>::value)
	{
		ocean_assert(NumericT<T>::isEqual(value1, T(0), T(1e-3)));
	}

#endif

	return 1u;
}

template <typename T>
unsigned int EquationT<T>::solveQuartic(const T a, const T b, const T c, const T d, const T e, T* x)
{
	ocean_assert(NumericT<T>::isNotEqualEps(a));
	ocean_assert(x != nullptr);

	// ax^4 + bx^3 + cx^2 + dx + e = 0

	if (NumericT<T>::isEqualEps(a))
	{
		return 0u;
	}

	// simplification using substitution:
	/// y^4 + alpha * y^2 + beta * y + gamma = 0

	// 1 / a
	const T a1 = T(1.0) / a;

	// b / a
	const T b_a = b * a1;

	// c / a
	const T c_a = c * a1;

	// (b * b) / (a * a)
	const T b_a2 = b_a * b_a;

	// (b * b * b) / (a * a * a)
	const T b_a3 = b_a2 * b_a;

	// d / a
	const T d_a = d * a1;

	//const T alpha = T(-0.375) * (b / a) * (b / a) + c / a;
	const T alpha = T(-0.375) * b_a2 + c_a;

	//const T beta = T(0.125) * (b / a) * (b / a) * (b / a) - T(0.5) * (b / a) * (c / a) + (d / a);
	const T beta = T(0.125) * b_a3 - T(0.5) * b_a * c_a + d_a;

	//const T gamma = T(-0.01171875) * (b / a) * (b / a) * (b / a) * (b / a) + T(0.0625) * (b / a) * (b / a) * (c / a) - T(0.25) * (b / a) * (d / a) + e / a;
	const T gamma = T(-0.01171875) * b_a3 * b_a + T(0.0625) * b_a2 * c_a - T(0.25) * b_a * d_a + e * a1;

	// y^4 + alpha y^2 + beta y + gamma = 0

	if (NumericT<T>::isEqualEps(beta))
	{
		const std::complex<T> cx1 = std::complex<T>(T(-0.25)) * std::complex<T>(b / a) + NumericT<T>::sqrt(std::complex<T>(T(0.5)) * (std::complex<T>(-alpha) + NumericT<T>::sqrt(std::complex<T>(alpha * alpha - 4 * gamma))));
		const std::complex<T> cx2 = std::complex<T>(T(-0.25)) * std::complex<T>(b / a) + NumericT<T>::sqrt(std::complex<T>(T(0.5)) * (std::complex<T>(-alpha) - NumericT<T>::sqrt(std::complex<T>(alpha * alpha - 4 * gamma))));
		const std::complex<T> cx3 = std::complex<T>(T(-0.25)) * std::complex<T>(b / a) - NumericT<T>::sqrt(std::complex<T>(T(0.5)) * (std::complex<T>(-alpha) + NumericT<T>::sqrt(std::complex<T>(alpha * alpha - 4 * gamma))));
		const std::complex<T> cx4 = std::complex<T>(T(-0.25)) * std::complex<T>(b / a) - NumericT<T>::sqrt(std::complex<T>(T(0.5)) * (std::complex<T>(-alpha) - NumericT<T>::sqrt(std::complex<T>(alpha * alpha - 4 * gamma))));

		ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isWeakEqualEps(cx1 * cx1 * cx1 * cx1 * a + cx1 * cx1 * cx1 * b + cx1 * cx1 * c + cx1 * d + e));
		ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isWeakEqualEps(cx2 * cx2 * cx2 * cx2 * a + cx2 * cx2 * cx2 * b + cx2 * cx2 * c + cx2 * d + e));
		ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isWeakEqualEps(cx3 * cx3 * cx3 * cx3 * a + cx3 * cx3 * cx3 * b + cx3 * cx3 * c + cx3 * d + e));
		ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isWeakEqualEps(cx4 * cx4 * cx4 * cx4 * a + cx4 * cx4 * cx4 * b + cx4 * cx4 * c + cx4 * d + e));

		unsigned int solutions = 0u;

		if (NumericT<T>::isEqualEps(cx1.imag()))
		{
			const T solution = cx1.real();
			if (NumericT<T>::isWeakEqualEps(solution * solution * solution * solution * a + solution * solution * solution * b + solution * solution * c + solution * d + e))
			{
				x[solutions++] = solution;
			}
		}

		if (NumericT<T>::isEqualEps(cx2.imag()))
		{
			const T solution = cx2.real();
			if (NumericT<T>::isWeakEqualEps(solution * solution * solution * solution * a + solution * solution * solution * b + solution * solution * c + solution * d + e))
			{
				x[solutions++] = solution;
			}
		}

		if (NumericT<T>::isEqualEps(cx3.imag()))
		{
			const T solution = cx3.real();
			if (NumericT<T>::isWeakEqualEps(solution * solution * solution * solution * a + solution * solution * solution * b + solution * solution * c + solution * d + e))
			{
				x[solutions++] = solution;
			}
		}

		if (NumericT<T>::isEqualEps(cx4.imag()))
		{
			const T solution = cx4.real();
			if (NumericT<T>::isWeakEqualEps(solution * solution * solution * solution * a + solution * solution * solution * b + solution * solution * c + solution * d + e))
			{
				x[solutions++] = solution;
			}
		}

		return solutions;
	}

	//const std::complex<T> p(-(alpha * alpha) / T(12.0) - gamma);
	const std::complex<T> p(T(-0.08333333333333333333333333333333) * alpha * alpha - gamma);

	//const std::complex<T> q(-(alpha * alpha * alpha) / T(108.0) + (alpha * gamma) / T(3.0) - (beta * beta) / T(8.0));
	const std::complex<T> q(T(-0.00925925925925925925925925925926) * alpha * alpha * alpha + T(0.33333333333333333333333333333333) * alpha * gamma - T(0.125) * beta * beta);

	const std::complex<T> qqSqr = NumericT<T>::sqrt(std::complex<T>(T(0.25)) * q * q + std::complex<T>(T(0.03703703703703703703703703703704)) * p * p * p);

	const std::complex<T> r(std::complex<T>(T(-0.5)) * q + qqSqr);
	if (NumericT<T>::isNan(r) || NumericT<T>::isInf(r))
	{
		return 0u;
	}

	const std::complex<T> u = NumericT<T>::pow(std::complex<T>(r), T(0.33333333333333333333333333333333));

	if (NumericT<T>::isNan(u) || NumericT<T>::isInf(u))
	{
		return 0u;
	}

	std::complex<T> y;
	if (NumericT<T>::isEqualEps(u.real()) && NumericT<T>::isEqualEps(u.imag()))
	{
		y = std::complex<T>(T(-0.83333333333333333333333333333333) * alpha) + u - NumericT<T>::pow(std::complex<T>(q), T(0.33333333333333333333333333333333));
	}
	else
	{
		y = std::complex<T>(T(-0.83333333333333333333333333333333) * alpha) + u - p / (std::complex<T>(3) * u);
	}

	//const std::complex<T> w_(NumericT<T>::sqrt(std::complex<T>(alpha) + std::complex<T>(2) * y));
	const std::complex<T> w(NumericT<T>::sqrt(std::complex<T>(T(0.25) * alpha) + std::complex<T>(T(0.5)) * y));

	//const std::complex<T> cx1 = std::complex<T>(T(-0.25)) * std::complex<T>(b / a) + std::complex<T>(T(0.5)) * (w + NumericT<T>::sqrt(std::complex<T>(-1) * (std::complex<T>(3) * std::complex<T>(alpha) + std::complex<T>(2) * y + std::complex<T>(2) * std::complex<T>(beta) / w)));

	if (NumericT<T>::isEqualEps(w))
	{
		return 0u;
	}

	const std::complex<T> beta2_w(std::complex<T>(T(-0.25) * beta) / w);
	const std::complex<T> alpha3y2 = std::complex<T>(T(-0.75) * alpha) - std::complex<T>(T(0.5)) * y;
	const std::complex<T> b_a4(T(-0.25) * b_a);

	const std::complex<T> sqrtPositive(NumericT<T>::sqrt(alpha3y2 + beta2_w));
	const std::complex<T> sqrtNegative(NumericT<T>::sqrt(alpha3y2 - beta2_w));

	const std::complex<T> cx1 = b_a4 + w + sqrtPositive;
	const std::complex<T> cx2 = b_a4 + w - sqrtPositive;
	const std::complex<T> cx3 = b_a4 - w + sqrtNegative;
	const std::complex<T> cx4 = b_a4 - w - sqrtNegative;

	unsigned int solutions = 0u;

	if (NumericT<T>::isEqualEps(cx1.imag()))
	{
		const T solution = cx1.real();
		if (NumericT<T>::isWeakEqualEps(solution * solution * solution * solution * a + solution * solution * solution * b + solution * solution * c + solution * d + e))
		{
			x[solutions++] = solution;
		}
	}

	if (NumericT<T>::isEqualEps(cx2.imag()))
	{
		const T solution = cx2.real();
		if (NumericT<T>::isWeakEqualEps(solution * solution * solution * solution * a + solution * solution * solution * b + solution * solution * c + solution * d + e))
		{
			x[solutions++] = solution;
		}
	}

	if (NumericT<T>::isEqualEps(cx3.imag()))
	{
		const T solution = cx3.real();
		if (NumericT<T>::isWeakEqualEps(solution * solution * solution * solution * a + solution * solution * solution * b + solution * solution * c + solution * d + e))
		{
			x[solutions++] = solution;
		}
	}

	if (NumericT<T>::isEqualEps(cx4.imag()))
	{
		const T solution = cx4.real();
		if (NumericT<T>::isWeakEqualEps(solution * solution * solution * solution * a + solution * solution * solution * b + solution * solution * c + solution * d + e))
		{
			x[solutions++] = solution;
		}
	}

	return solutions;
}

}

#endif // META_OCEAN_MATH_EQUATION_H
