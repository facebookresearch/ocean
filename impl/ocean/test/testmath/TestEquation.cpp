/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestEquation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Equation.h"
#include "ocean/math/Random.h"

#include "ocean/test/ValidationPrecision.h"

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

	result = testLinearEquation<float>(testDuration) && result;
	Log::info() << " ";
	result = testLinearEquation<double>(testDuration) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testQuadraticEquation<float>(testDuration) && result;
	Log::info() << " ";
	result = testQuadraticEquation<double>(testDuration) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testCubicEquation<float>(testDuration) && result;
	Log::info() << " ";
	result = testCubicEquation<double>(testDuration) && result;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	result = testQuarticEquation<float>(testDuration) && result;
	Log::info() << " ";
	result = testQuarticEquation<double>(testDuration) && result;

	Log::info() << " ";

	if (result)
	{
		Log::info() << "Equation test succeeded.";
	}
	else
	{
		Log::info() << "Equation test FAILED!";
	}

	return result;
}

#ifdef OCEAN_USE_GTEST

TEST(TestEquation, LinearEquation_float)
{
	EXPECT_TRUE(TestEquation::testLinearEquation<float>(GTEST_TEST_DURATION));
}

TEST(TestEquation, LinearEquation_double)
{
	EXPECT_TRUE(TestEquation::testLinearEquation<double>(GTEST_TEST_DURATION));
}

TEST(TestEquation, QuadraticEquation_float)
{
	EXPECT_TRUE(TestEquation::testQuadraticEquation<float>(GTEST_TEST_DURATION));
}

TEST(TestEquation, QuadraticEquation_double)
{
	EXPECT_TRUE(TestEquation::testQuadraticEquation<double>(GTEST_TEST_DURATION));
}

TEST(TestEquation, CubicEquation_float)
{
	EXPECT_TRUE(TestEquation::testCubicEquation<float>(GTEST_TEST_DURATION));
}

TEST(TestEquation, CubicEquation_double)
{
	EXPECT_TRUE(TestEquation::testCubicEquation<double>(GTEST_TEST_DURATION));
}

TEST(TestEquation, QuarticEquation_float)
{
	EXPECT_TRUE(TestEquation::testQuarticEquation<float>(GTEST_TEST_DURATION));
}

TEST(TestEquation, QuarticEquation_double)
{
	EXPECT_TRUE(TestEquation::testQuarticEquation<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestEquation::testLinearEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr size_t iterations = 1000;

	Log::info() << "Solve: a * x + b = 0, with " << TypeNamer::name<T>() << ":";

	constexpr double successThreshold = std::is_same<T, float>::value ? 0.98 : 0.99;
	constexpr T valueRange = std::is_same<float, T>::value ? T(10) : T(1000);

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		std::vector<T> aValues(iterations);
		std::vector<T> bValues(iterations);
		std::vector<T> xValues(iterations, NumericT<T>::minValue());

		for (size_t n = 0; n < iterations; ++n)
		{
			aValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);

			while (NumericT<T>::isEqualEps(aValues[n]))
			{
				aValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			bValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
		}

		bool allSucceeded = true;

		performance.start();
			for (unsigned int n = 0u; n < iterations; ++n)
			{
				allSucceeded = EquationT<T>::solveLinear(aValues[n], bValues[n], xValues[n]) && allSucceeded;
			}
		performance.stop();

		if (!allSucceeded)
		{
			OCEAN_SET_FAILED(validation);
		}

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const T& a = aValues[n];
			const T& b = bValues[n];
			const T& x = xValues[n];

			const T result = a * x + b;

			if (NumericT<T>::isNotEqualEps(result))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance for " << iterations << " iterations: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestEquation::testQuadraticEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr size_t iterations = 1000;

	Log::info() << "Solve: a * x^2 + b * x + c = 0, with " << TypeNamer::name<T>() << ":";

	constexpr double successThreshold = std::is_same<T, float>::value ? 0.95 : 0.99;
	constexpr T valueRange = std::is_same<float, T>::value ? T(10) : T(1000);

	RandomGenerator randomGenerator;
	ValidationPrecision validation(successThreshold, randomGenerator);

	HighPerformanceStatistic performance;

	const Timestamp startTimestamp(true);

	do
	{
		std::vector<T> aValues(iterations);
		std::vector<T> bValues(iterations);
		std::vector<T> cValues(iterations);
		std::vector<T> x1Values(iterations, NumericT<T>::minValue());
		std::vector<T> x2Values(iterations, NumericT<T>::minValue());

		for (size_t n = 0; n < iterations; ++n)
		{
			while (true)
			{
				T a = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);

				while (NumericT<T>::isEqualEps(a))
				{
					a = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				}

				T b = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				T c = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);

				const T discriminant = b * b - T(4) * a * c;

				if (discriminant < T(0))
				{
					continue;
				}

				aValues[n] = a;
				bValues[n] = b;
				cValues[n] = c;

				break;
			}
		}

		bool allSucceeded = true;

		performance.start();
			for (unsigned int n = 0u; n < iterations; ++n)
			{
				allSucceeded = EquationT<T>::solveQuadratic(aValues[n], bValues[n], cValues[n], x1Values[n], x2Values[n]) && allSucceeded;
			}
		performance.stop();

		for (unsigned int n = 0u; n < iterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const T& a = aValues[n];
			const T& b = bValues[n];
			const T& c = cValues[n];
			const T& x1 = x1Values[n];
			const T& x2 = x2Values[n];

			const T result1 = a * x1 * x1 + b * x1 + c;
			const T result2 = a * x2 * x2 + b * x2 + c;

			if (NumericT<T>::isNotWeakEqualEps(result1) || NumericT<T>::isNotWeakEqualEps(result2))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance for " << iterations << " iterations: " << performance;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestEquation::testCubicEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr size_t iterations = 1000;

	Log::info() << "Solve: a * x^3 + b * x^2 + c * x + d = 0, with " << TypeNamer::name<T>() << ":";

	constexpr double successThreshold = std::is_same<T, float>::value ? 0.95 : 0.99;
	constexpr T valueRange = std::is_same<float, T>::value ? T(10) : T(1000);

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (const bool refine : {false, true})
	{
		if (refine)
		{
			Log::info() << "... with refinement";
		}
		else
		{
			Log::info() << "... without refinement";
		}

		const std::string indentation = "  ";

		ValidationPrecision validation(successThreshold, randomGenerator);

		size_t overallExecutions = 0;
		size_t executionsWithRealRoots = 0;

		double sumResidual = 0.0;

		HighPerformanceStatistic performance;

		const Timestamp startTimestamp(true);

		do
		{
			std::vector<T> aValues(iterations);
			std::vector<T> bValues(iterations);
			std::vector<T> cValues(iterations);
			std::vector<T> dValues(iterations);
			std::vector<T> x1Values(iterations, NumericT<T>::minValue());
			std::vector<T> x2Values(iterations, NumericT<T>::minValue());
			std::vector<T> x3Values(iterations, NumericT<T>::minValue());
			std::vector<unsigned int> solutionsCount(iterations, 0u);

			for (size_t n = 0; n < iterations; ++n)
			{
				aValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);

				while (NumericT<T>::isEqualEps(aValues[n]))
				{
					aValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				}

				bValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				cValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				dValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			performance.start();
				for (unsigned int n = 0u; n < iterations; ++n)
				{
					solutionsCount[n] = EquationT<T>::solveCubic(aValues[n], bValues[n], cValues[n], dValues[n], x1Values[n], x2Values[n], x3Values[n], refine);
				}
			performance.stop();

			for (unsigned int n = 0u; n < iterations; ++n)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const T& a = aValues[n];
				const T& b = bValues[n];
				const T& c = cValues[n];
				const T& d = dValues[n];

				const unsigned int solutions = solutionsCount[n];

				++overallExecutions;

				if (solutions == 0u)
				{
					scopedIteration.setInaccurate();
				}
				else
				{
					const std::array<T, 3> xValues = {x1Values[n], x2Values[n], x3Values[n]};

					T maxResidual = T(0);

					for (unsigned int nSolution = 0u; nSolution < solutions; ++nSolution)
					{
						const T& x = xValues[nSolution];

						const T result = a * x * x * x + b * x * x + c * x + d;

						if (NumericT<T>::isNotWeakEqualEps(result))
						{
							scopedIteration.setInaccurate();
						}

						maxResidual = std::max(maxResidual, NumericT<T>::abs(result));
					}

					sumResidual += maxResidual;

					++executionsWithRealRoots;
				}
			}
		}
		while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

		ocean_assert(overallExecutions != 0);
		const double percentRealRoots = double(executionsWithRealRoots) * 100.0 / double(overallExecutions);

		const double averageResidual = NumericD::ratio(sumResidual, double(executionsWithRealRoots), 0.0);

		Log::info() << "Performance for " << iterations << " iterations: " << performance;
		Log::info() << "Average residual: " << String::toAString(averageResidual, 12u);
		Log::info() << "Iterations with real roots: " << String::toAString(percentRealRoots, 1u) << "%";
		Log::info() << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}

template <typename T>
bool TestEquation::testQuarticEquation(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr size_t iterations = 1000;

	Log::info() << "Solve: a * x^4 + b * x^3 + c * x^2 + d * x + e = 0, with " << TypeNamer::name<T>() << ":";

	constexpr double successThreshold = std::is_same<T, float>::value ? 0.90 : 0.99;
	constexpr T valueRange = std::is_same<float, T>::value ? T(10) : T(1000);

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (const bool refine : {false, true})
	{
		if (refine)
		{
			Log::info() << "... with refinement";
		}
		else
		{
			Log::info() << "... without refinement";
		}

		const std::string indentation = "  ";

		ValidationPrecision validation(successThreshold, randomGenerator);

		size_t overallExecutions = 0;
		size_t executionsWithRealRoots = 0;

		double sumResidual = 0.0;

		HighPerformanceStatistic performance;

		const Timestamp startTimestamp(true);

		do
		{
			std::vector<T> aValues(iterations);
			std::vector<T> bValues(iterations);
			std::vector<T> cValues(iterations);
			std::vector<T> dValues(iterations);
			std::vector<T> eValues(iterations);
			std::vector<std::array<T, 4>> xValues(iterations);
			std::vector<unsigned int> solutionsCount(iterations, 0u);

			for (size_t n = 0; n < iterations; ++n)
			{
				aValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);

				while (NumericT<T>::isEqualEps(aValues[n]))
				{
					aValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				}

				bValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				cValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				dValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				eValues[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);

				xValues[n].fill(NumericT<T>::minValue());
			}

			performance.start();
				for (unsigned int n = 0u; n < iterations; ++n)
				{
					solutionsCount[n] = EquationT<T>::solveQuartic(aValues[n], bValues[n], cValues[n], dValues[n], eValues[n], xValues[n].data(), refine);
				}
			performance.stop();

			overallExecutions += iterations;

			for (unsigned int n = 0u; n < iterations; ++n)
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				const T& a = aValues[n];
				const T& b = bValues[n];
				const T& c = cValues[n];
				const T& d = dValues[n];
				const T& e = eValues[n];
				const std::array<T, 4>& xIteration = xValues[n];
				const unsigned int solutions = solutionsCount[n];

				if (solutions != 0u)
				{
					T maxResidual = T(0);

					for (unsigned int i = 0u; i < solutions; ++i)
					{
						const T& x = xIteration[i];

						const T result = a * x * x * x * x + b * x * x * x + c * x * x + d * x + e;

						if (NumericT<T>::isNotWeakEqualEps(result))
						{
							scopedIteration.setInaccurate();
						}

						maxResidual = std::max(maxResidual, NumericT<T>::abs(result));
					}

					sumResidual += maxResidual;

					++executionsWithRealRoots;
				}
			}
		}
		while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

		ocean_assert(overallExecutions != 0);
		const double percentRealRoots = double(executionsWithRealRoots) * 100.0 / double(overallExecutions);

		const double averageResidual = NumericD::ratio(sumResidual, double(executionsWithRealRoots), 0.0);

		Log::info() << indentation << "Performance for " << iterations << " iterations: " << performance;
		Log::info() << indentation << "Average residual: " << String::toAString(averageResidual, 12u);
		Log::info() << indentation << "Iterations with real roots: " << String::toAString(percentRealRoots, 1u) << "%";
		Log::info() << indentation << "Validation: " << validation;

		if (!validation.succeeded())
		{
			allSucceeded = false;
		}
	}

	return allSucceeded;
}




}

}

}
