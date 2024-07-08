/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestEquation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Equation.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestEquation::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool result = true;

	Log::info() << "---   Equation test:   ---";
	Log::info() << " ";

	result = testLinearEquation(testDuration) && result;

	Log::info() << " ";

	result = testQuadraticEquation(testDuration) && result;

	Log::info() << " ";

	result = testCubicEquation(testDuration) && result;

	Log::info() << " ";

	result = testQuarticEquation(testDuration) && result;

	Log::info() << " ";

	if (result)
		Log::info() << "Equation test succeeded.";
	else
		Log::info() << "Equation test FAILED!";

	return result;
}
	
#ifdef OCEAN_USE_GTEST
	
TEST(TestEquation, LinearEquation) {
	EXPECT_TRUE(TestEquation::testLinearEquation(GTEST_TEST_DURATION));
}

TEST(TestEquation, QuadraticEquation) {
	EXPECT_TRUE(TestEquation::testQuadraticEquation(GTEST_TEST_DURATION));
}

TEST(TestEquation, CubicEquation) {
	EXPECT_TRUE(TestEquation::testCubicEquation(GTEST_TEST_DURATION));
}

TEST(TestEquation, QuarticEquation) {
	EXPECT_TRUE(TestEquation::testQuarticEquation(GTEST_TEST_DURATION));
}
	
#endif // OCEAN_USE_GTEST

bool TestEquation::testLinearEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Solve: a * x + b = 0";

	const unsigned int constIterations = 1000u;

	Scalar x;
	Scalar a[constIterations];
	Scalar b[constIterations];

	for (unsigned int n = 0u; n < constIterations; ++n)
	{
		a[n] = Random::scalar(-1000, 1000);

		while (Numeric::isEqualEps(a[n]))
			a[n] = Random::scalar(-1000, 1000);

		b[n] = Random::scalar(-1000, 1000);
	}

	unsigned long long iterations = 0ull;
	unsigned int value = 0u;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
			value += Equation::solveLinear(a[n], b[n], x);

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (value % 2u == 0u)
		Log::info() << "Performance: " << timer.yseconds() / double(constIterations * iterations) << "mys";
	else
		Log::info() << "Performance: " << timer.yseconds() / double(constIterations * iterations) << "mys";

	const double validPercent = validateLinearEquation(testDuration);

	Log::info() << "Validation: " << String::toAString(validPercent * 100.0, 1u) << "% succeeded.";

	if (std::is_same<Scalar, float>::value)
		return validPercent >= 0.98;
	else
		return validPercent >= 0.99;
}

bool TestEquation::testQuadraticEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Solve: a * x^2 + b * x + c = 0";

	const unsigned int constIterations = 1000u;

	Scalar x1, x2;
	Scalar a[constIterations];
	Scalar b[constIterations];
	Scalar c[constIterations];

	for (unsigned int n = 0u; n < constIterations; ++n)
	{
		a[n] = Random::scalar(-1000, 1000);

		while (Numeric::isEqualEps(a[n]))
			a[n] = Random::scalar(-1000, 1000);

		b[n] = Random::scalar(-1000, 1000);
		c[n] = Random::scalar(-1000, 1000);
	}

	unsigned long long iterations = 0ull;
	unsigned int value = 0u;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
			value += Equation::solveQuadratic(a[n], b[n], c[n], x1, x2);

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (value % 2u == 0u)
		Log::info() << "Performance: " << timer.yseconds() / double(constIterations * iterations) << "mys";
	else
		Log::info() << "Performance: " << timer.yseconds() / double(constIterations * iterations) << "mys";

	const double validPercent = validateQuadraticEquation(testDuration);

	Log::info() << "Validation: " << String::toAString(validPercent * 100.0, 1u) << "% succeeded.";

	if (std::is_same<Scalar, float>::value)
		return validPercent >= 0.95;
	else
		return validPercent >= 0.99;
}

bool TestEquation::testCubicEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Solve: a * x^3 + b * x^2 + c * x + d = 0";

	const unsigned int constIterations = 1000u;

	Scalar x1, x2, x3;
	Scalar a[constIterations];
	Scalar b[constIterations];
	Scalar c[constIterations];
	Scalar d[constIterations];

	for (unsigned int n = 0u; n < constIterations; ++n)
	{
		a[n] = Random::scalar(-1000, 1000);

		while (Numeric::isEqualEps(a[n]))
			a[n] = Random::scalar(-1000, 1000);

		b[n] = Random::scalar(-1000, 1000);
		c[n] = Random::scalar(-1000, 1000);
		d[n] = Random::scalar(-1000, 1000);
	}

	unsigned long long iterations = 0ull;
	unsigned int value = 0u;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
			value += Equation::solveCubic(a[n], b[n], c[n], d[n], x1, x2, x3);

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (value % 2u == 0u)
		Log::info() << "Performance: " << timer.yseconds() / double(constIterations * iterations) << "mys";
	else
		Log::info() << "Performance: " << timer.yseconds() / double(constIterations * iterations) << "mys";

	const double validPercent = validateCubicEquation(testDuration);

	Log::info() << "Validation: " << String::toAString(validPercent * 100.0, 1u) << "% succeeded.";

	if (std::is_same<Scalar, float>::value)
		return validPercent >= 0.85;
	else
		return validPercent >= 0.99;
}

bool TestEquation::testQuarticEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Solve: a * x^4 + b * x^3 + c * x^2 + d * x + e = 0";

	const unsigned int constIterations = 1000u;

	Scalar x[4];
	Scalar a[constIterations];
	Scalar b[constIterations];
	Scalar c[constIterations];
	Scalar d[constIterations];
	Scalar e[constIterations];

	for (unsigned int n = 0u; n < constIterations; ++n)
	{
		a[n] = Random::scalar(-1000, 1000);

		while (Numeric::isEqualEps(a[n]))
			a[n] = Random::scalar(-1000, 1000);

		b[n] = Random::scalar(-1000, 1000);
		c[n] = Random::scalar(-1000, 1000);
		d[n] = Random::scalar(-1000, 1000);
		e[n] = Random::scalar(-1000, 1000);
	}

	unsigned long long iterations = 0ull;
	unsigned int value = 0u;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
			value += Equation::solveQuartic(a[n], b[n], c[n], d[n], e[n], x);

		++iterations;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (value % 2u == 0u)
		Log::info() << "Performance: " << timer.yseconds() / double(constIterations * iterations) << "mys";
	else
		Log::info() << "Performance: " << timer.yseconds() / double(constIterations * iterations) << "mys";

	const double validPercent = validateQuarticEquation(testDuration);

	Log::info() << "Validation: " << String::toAString(validPercent * 100.0, 1u) << "% succeeded.";

	if (std::is_same<Scalar, float>::value)
		return validPercent >= 0.90;
	else
		return validPercent >= 0.99;
}

double TestEquation::validateLinearEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int constIterations = 100000u;

	unsigned long long iterations = 0ull;
	unsigned long long invalidIterations = 0ull;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			Scalar a = Random::scalar(-1000, 1000);

			while (Numeric::isEqualEps(a))
				a = Random::scalar(-1000, 1000);

			const Scalar b = Random::scalar(-1000, 1000);

			Scalar x;

			if (Equation::solveLinear(a, b, x))
			{
				if (Numeric::isNotWeakEqualEps(a * x + b))
					++invalidIterations;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	return double(iterations - invalidIterations) / double(iterations);
}

double TestEquation::validateQuadraticEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int constIterations = 100000u;

	unsigned long long iterations = 0ull;
	unsigned long long invalidIterations = 0ull;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			Scalar a = Random::scalar(-1000, 1000);

			while (Numeric::isEqualEps(a))
				a = Random::scalar(-1000, 1000);

			const Scalar b = Random::scalar(-1000, 1000);
			const Scalar c = Random::scalar(-1000, 1000);

			Scalar x1, x2;

			if (Equation::solveQuadratic(a, b, c, x1, x2))
			{
				if (Numeric::isNotWeakEqualEps(a * x1 * x1 + b * x1 + c) || Numeric::isNotWeakEqualEps(a * x2 * x2 + b * x2 + c))
					++invalidIterations;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	return double(iterations - invalidIterations) / double(iterations);
}

double TestEquation::validateCubicEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int constIterations = 100000u;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			Scalar a = Random::scalar(-1000, 1000);

			while (Numeric::isEqualEps(a))
				a = Random::scalar(-1000, 1000);

			const Scalar b = Random::scalar(-1000, 1000);
			const Scalar c = Random::scalar(-1000, 1000);
			const Scalar d = Random::scalar(-1000, 1000);

			Scalar x1, x2, x3;

			const unsigned int solutions = Equation::solveCubic(a, b, c, d, x1, x2, x3);

			if (solutions == 1u)
			{
				if (Numeric::isWeakEqualEps(a * x1 * x1 * x1 + b * x1 * x1 + c * x1 + d))
					++validIterations;
			}
			else if (solutions == 2u)
			{
				if (Numeric::isWeakEqualEps(a * x1 * x1 * x1 + b * x1 * x1 + c * x1 + d)
							&& Numeric::isWeakEqualEps(a * x2 * x2 * x2 + b * x2 * x2 + c * x2 + d))
					++validIterations;
			}
			else if (solutions == 3u)
			{
				if (Numeric::isWeakEqualEps(a * x1 * x1 * x1 + b * x1 * x1 + c * x1 + d)
							&& Numeric::isWeakEqualEps(a * x2 * x2 * x2 + b * x2 * x2 + c * x2 + d)
							&& Numeric::isWeakEqualEps(a * x3 * x3 * x3 + b * x3 * x3 + c * x3 + d))
					++validIterations;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	return double(validIterations) / double(iterations);
}

double TestEquation::validateQuarticEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int constIterations = 100000u;

	unsigned long long iterations = 0ull;
	unsigned long long invalidIterations = 0ull;

	const Timestamp startTimestamp(true);
	HighPerformanceTimer timer;

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			Scalar a = Random::scalar(-1000, 1000);

			while (Numeric::isEqualEps(a))
				a = Random::scalar(-1000, 1000);

			const Scalar b = Random::scalar(-1000, 1000);
			const Scalar c = Random::scalar(-1000, 1000);
			const Scalar d = Random::scalar(-1000, 1000);
			const Scalar e = Random::scalar(-1000, 1000);

			Scalar x[4];

			const unsigned int solutions = Equation::solveQuartic(a, b, c, d, e, x);

			if (solutions == 1u)
			{
				if (Numeric::isNotWeakEqualEps(a * x[0] * x[0] * x[0] * x[0] + b * x[0] * x[0] * x[0] + c * x[0] * x[0] + d * x[0] + e))
					++invalidIterations;
			}
			else if (solutions == 2u)
			{
				if (Numeric::isNotWeakEqualEps(a * x[0] * x[0] * x[0] * x[0] + b * x[0] * x[0] * x[0] + c * x[0] * x[0] + d * x[0] + e)
							|| Numeric::isNotWeakEqualEps(a * x[1] * x[1] * x[1] * x[1] + b * x[1] * x[1] * x[1] + c * x[1] * x[1] + d * x[1] + e))
					++invalidIterations;
			}
			else if (solutions == 3u)
			{
				if (Numeric::isNotWeakEqualEps(a * x[0] * x[0] * x[0] * x[0] + b * x[0] * x[0] * x[0] + c * x[0] * x[0] + d * x[0] + e)
							|| Numeric::isNotWeakEqualEps(a * x[1] * x[1] * x[1] * x[1] + b * x[1] * x[1] * x[1] + c * x[1] * x[1] + d * x[1] + e)
							|| Numeric::isNotWeakEqualEps(a * x[2] * x[2] * x[2] * x[2] + b * x[2] * x[2] * x[2] + c * x[2] * x[2] + d * x[2] + e))
					++invalidIterations;
			}
			else if (solutions == 4u)
			{
				if (Numeric::isNotWeakEqualEps(a * x[0] * x[0] * x[0] * x[0] + b * x[0] * x[0] * x[0] + c * x[0] * x[0] + d * x[0] + e)
							|| Numeric::isNotWeakEqualEps(a * x[1] * x[1] * x[1] * x[1] + b * x[1] * x[1] * x[1] + c * x[1] * x[1] + d * x[1] + e)
							|| Numeric::isNotWeakEqualEps(a * x[2] * x[2] * x[2] * x[2] + b * x[2] * x[2] * x[2] + c * x[2] * x[2] + d * x[2] + e)
							|| Numeric::isNotWeakEqualEps(a * x[3] * x[3] * x[3] * x[3] + b * x[3] * x[3] * x[3] + c * x[3] * x[3] + d * x[3] + e))
					++invalidIterations;
			}

			++iterations;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	return double(iterations - invalidIterations) / double(iterations);
}

}

}

}
