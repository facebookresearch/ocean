/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestAutomaticDifferentiation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/AutomaticDifferentiation.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Pose.h"
#include "ocean/math/StaticMatrix.h"
#include "ocean/math/Random.h"

#include "ocean/test/ValidationPrecision.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestAutomaticDifferentiation::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Automatic scalar differentiation test:   ---";
	Log::info() << " ";

	allSucceeded = testSimple<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testSimple<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFunctions<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testFunctions<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNested<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNested<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHomography<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testHomography<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPose<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testPose<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Differentiation test succeeded.";
	}
	else
	{
		Log::info() << "Differentiation test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAutomaticDifferentiation, Simple_float)
{
	EXPECT_TRUE(TestAutomaticDifferentiation::testSimple<float>(GTEST_TEST_DURATION));
}

TEST(TestAutomaticDifferentiation, Simple_double)
{
	EXPECT_TRUE(TestAutomaticDifferentiation::testSimple<double>(GTEST_TEST_DURATION));
}


TEST(TestAutomaticDifferentiation, Functions_float)
{
	EXPECT_TRUE(TestAutomaticDifferentiation::testFunctions<float>(GTEST_TEST_DURATION));
}

TEST(TestAutomaticDifferentiation, Functions_double)
{
	EXPECT_TRUE(TestAutomaticDifferentiation::testFunctions<double>(GTEST_TEST_DURATION));
}


TEST(TestAutomaticDifferentiation, Nested_float)
{
	EXPECT_TRUE(TestAutomaticDifferentiation::testNested<float>(GTEST_TEST_DURATION));
}

TEST(TestAutomaticDifferentiation, Nested_double)
{
	EXPECT_TRUE(TestAutomaticDifferentiation::testNested<double>(GTEST_TEST_DURATION));
}


TEST(TestAutomaticDifferentiation, Homography_float)
{
	EXPECT_TRUE(TestAutomaticDifferentiation::testHomography<float>(GTEST_TEST_DURATION));
}

TEST(TestAutomaticDifferentiation, Homography_double)
{
	EXPECT_TRUE(TestAutomaticDifferentiation::testHomography<double>(GTEST_TEST_DURATION));
}


TEST(TestAutomaticDifferentiation, Pose_float)
{
	EXPECT_TRUE(TestAutomaticDifferentiation::testPose<float>(GTEST_TEST_DURATION));
}

TEST(TestAutomaticDifferentiation, Pose_double)
{
	EXPECT_TRUE(TestAutomaticDifferentiation::testPose<double>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

template <typename T>
bool TestAutomaticDifferentiation::testSimple(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing automatic differentiation of simple functions with " << TypeNamer::name<T>() << ":";

	using AutoDiff = AutomaticDifferentiationT<T>;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	constexpr T valueRange = std::is_same<float, T>::value ? T(10) : T(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const T x = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			const T c = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = x
				// f'(x) = 1

				const AutoDiff autoDiff(x);

				const T expectedValue = x;
				constexpr T expectedDerivative = 1;

				if (autoDiff.value() != expectedValue)
				{
					OCEAN_SET_FAILED(validation);
				}

				if (autoDiff.derivative() != expectedDerivative)
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = x + c
				// f'(x) = 1

				const AutoDiff autoDiff = AutoDiff(x) + c;

				const T expectedValue = x + c;
				constexpr T expectedDerivative = 1;

				if (autoDiff.value() != expectedValue)
				{
					scopedIteration.setInaccurate();
				}

				if (autoDiff.derivative() != expectedDerivative)
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = c + x
				// f'(x) = 1

				const AutoDiff autoDiff = c + AutoDiff(x);

				const T expectedValue = c + x;
				constexpr T expectedDerivative = 1;

				if (autoDiff.value() != expectedValue)
				{
					scopedIteration.setInaccurate();
				}

				if (autoDiff.derivative() != expectedDerivative)
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = x - c
				// f'(x) = 1

				const AutoDiff autoDiff = AutoDiff(x) - c;

				const T expectedValue = x - c;
				constexpr T expectedDerivative = 1;

				if (autoDiff.value() != expectedValue)
				{
					scopedIteration.setInaccurate();
				}

				if (autoDiff.derivative() != expectedDerivative)
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = c - x
				// f'(x) = -1

				const AutoDiff autoDiff = c - AutoDiff(x);

				const T expectedValue = c - x;
				constexpr T expectedDerivative = -1;

				if (autoDiff.value() != expectedValue)
				{
					scopedIteration.setInaccurate();
				}

				if (autoDiff.derivative() != expectedDerivative)
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = x + x + c
				// f'(x) = 2

				const AutoDiff autoDiff = AutoDiff(x) + AutoDiff(x) + c;

				const T expectedValue = x + x + c;
				constexpr T expectedDerivative = 2;

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = x * c
				// f'(x) = c

				const AutoDiff autoDiff = AutoDiff(x) * c;

				const T expectedValue = x * c;
				const T expectedDerivative = c;

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = c * x
				// f'(x) = c

				const AutoDiff autoDiff = c * AutoDiff(x);

				const T expectedValue = c * x;
				const T expectedDerivative = c;

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = x^2
				// f'(x) = 2x

				const AutoDiff autoDiff = AutoDiff(x) * AutoDiff(x);

				const T expectedValue = x * x;
				const T expectedDerivative = T(2) * x;

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = x^2 * c
				// f'(x) = 2cx

				const AutoDiff autoDiff = AutoDiff(x) * AutoDiff(x) * c;

				const T expectedValue = x * x * c;
				const T expectedDerivative = T(2) * c * x;

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) == x^3
				// f'(x) = 3x^2

				const AutoDiff autoDiff = AutoDiff(x) * AutoDiff(x) * AutoDiff(x);

				const T expectedValue = x * x * x;
				const T expectedDerivative = T(3) * x * x;

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) == 3 * (x + c)
				// f'(x) = 3

				const AutoDiff autoDiff = T(3) * (AutoDiff(x) + c);

				const T expectedValue = T(3) * (x + c);
				constexpr T expectedDerivative = T(3);

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) == (3 * (x + 2))^2 = (3x + 6)^2 = 9x^2 + 36x + 36
				// f'(x) = 2 * (3x + 6) * 3 = 18x + 36

				const AutoDiff autoDiff = ((T(3) * (AutoDiff(x) + T(2))) * (T(3) * (AutoDiff(x) + T(2))));

				const T expectedValue = NumericT<T>::sqr(T(3) * (x + T(2)));
				const T expectedDerivative = T(18) * x + T(36);

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) == (3 * (x + c))^2 = (3x + 3c)^2 = 9x^2 + 18cx + 9c^2
				// f'(x) = 2 * (3x + 3c) * 3 = 18x + 18c

				const AutoDiff autoDiff = (((AutoDiff(x) + c) * T(3)) * ((AutoDiff(x) + c) * T(3)));

				const T expectedValue = NumericT<T>::sqr(T(3) * (x + c));
				const T expectedDerivative = T(18) * x + T(18) * c;

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			if (NumericT<T>::isNotEqualEps(x))
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = 1 / x
				// f'(x) = -1 / x^2

				const AutoDiff autoDiff = T(1) / AutoDiff(x);

				const T expectedValue = T(1) / x;
				const T expectedDerivative = -T(1) / (x * x);

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			if (NumericT<T>::isNotEqualEps(x))
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = c / x
				// f'(x) = -c / x^2

				const AutoDiff autoDiff = c / AutoDiff(x);

				const T expectedValue = c / x;
				const T expectedDerivative = -c / (x * x);

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			if (NumericT<T>::isNotEqualEps(x * x))
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = c / x^2
				// f'(x) = -2c / x^3

				const AutoDiff autoDiff = c / (AutoDiff(x) * AutoDiff(x));

				const T expectedValue = c / (x * x);
				const T expectedDerivative = -T(2) * c / (x * x * x);

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			if (NumericT<T>::isNotEqualEps(x * x))
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = (c / x) * (1 / x)
				// f'(x) = -2c / x^3

				const AutoDiff autoDiff = (c / AutoDiff(x)) * (1 / AutoDiff(x));

				const T expectedValue = (c / x) * (1 / x);
				const T expectedDerivative = (-T(2) * c) / (x * x * x);

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = -x
				// f'(x) = -1

				const AutoDiff autoDiff = -AutoDiff(x);

				const T expectedValue = -x;
				const T expectedDerivative = -T(1);

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = -x*x
				// f'(x) = -2x

				const AutoDiff autoDiff = -AutoDiff(x) * AutoDiff(x);

				const T expectedValue = -x * x;
				const T expectedDerivative = -T(2) * x;

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestAutomaticDifferentiation::testFunctions(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing automatic differentiation of mathematic functions with " << TypeNamer::name<T>() << ":";

	using AutoDiff = AutomaticDifferentiationT<T>;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	constexpr T valueRange = std::is_same<float, T>::value ? T(10) : T(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const T x = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			const T c = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = sin(x)
				// f'(x) = cos(x)

				const AutoDiff autoDiff = AutoDiff::sin(AutoDiff(x));

				const T expectedValue = NumericT<T>::sin(x);
				const T expectedDerivative = NumericT<T>::cos(x);

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = cos(x)
				// f'(x) = -sin(x)

				const AutoDiff autoDiff = AutoDiff::cos(AutoDiff(x));

				const T expectedValue = NumericT<T>::cos(x);
				const T expectedDerivative = -NumericT<T>::sin(x);

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = tan(x)
				// f'(x) = 1 / cos(x)^2

				const AutoDiff autoDiff = AutoDiff::tan(AutoDiff(x));

				const T expectedValue = NumericT<T>::tan(x);
				const T expectedDerivative = T(1) / NumericT<T>::sqr(NumericT<T>::cos(x));

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			if (x >= T(0))
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = sqrt(x)
				// f'(x) = 1 / (2 * sqrt(x))

				const AutoDiff autoDiff = AutoDiff::sqrt(AutoDiff(x));

				const T expectedValue = NumericT<T>::sqrt(x);
				const T expectedDerivative = T(1) / (T(2) * NumericT<T>::sqrt(x));

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = exp(x)
				// f'(x) = exp(x)

				const AutoDiff autoDiff = AutoDiff::exp(AutoDiff(x));

				const T expectedValue = NumericT<T>::exp(x);
				const T expectedDerivative = NumericT<T>::exp(x);

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			if (x > NumericT<T>::eps())
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = log(x)
				// f'(x) = 1 / x

				const AutoDiff autoDiff = AutoDiff::log(AutoDiff(x));

				const T expectedValue = NumericT<T>::log(x);
				const T expectedDerivative = T(1) / x;

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			if (x > Numeric::eps())
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = log2(x) = log(x) / log(2)
				// f'(x) = (1 / x) * log(2) / log(2)^2 = 1 / (x * log(2))

				const AutoDiff autoDiff = AutoDiff::log2(AutoDiff(x));

				const T expectedValue = NumericT<T>::log2(x);
				const T expectedDerivative = T(1) / (x * NumericT<T>::log(2));

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			if (x > NumericT<T>::eps())
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = log10(x) = log(x) / log(10)
				// f'(x) = (1 / x) * log(10) / log(10)^2 = 1 / (x * log(10))

				const AutoDiff autoDiff = AutoDiff::log10(AutoDiff(x));

				const T expectedValue = NumericT<T>::log10(x);
				const T expectedDerivative = T(1) / (x * NumericT<T>::log(10));

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			if (x > NumericT<T>::eps())
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = log(x) / log10(x)
				// f'(x) = [log10(x) / x - log(x) / (x * log(10))] / log10(x)^2 = 0

				const AutoDiff autoDiff = AutoDiff::log(AutoDiff(x)) / AutoDiff::log10(AutoDiff(x));

				const T expectedValue = NumericT<T>::log(x) / NumericT<T>::log10(x);
				constexpr T expectedDerivative = T(0);

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			if (x >= T(0))
			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x, y) = x ^ y
				// f'(x, y) = y * x ^ (y - 1)

				const T y = RandomT<T>::scalar(randomGenerator, -10, 10);

				const AutoDiff autoDiff = AutoDiff::pow(AutoDiff(x), y);

				const T expectedValue = NumericT<T>::pow(x, y);
				const T expectedDerivative = y * NumericT<T>::pow(x, y - T(1));

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = |x|
				// f'(x) = sign(x)

				const AutoDiff autoDiff = AutoDiff::abs(AutoDiff(x));

				const T expectedValue = NumericT<T>::abs(x);
				const T expectedDerivative = NumericT<T>::sign(x);

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = min(x * x, c)
				// f'(x) = x * x < c:       2x
				//              else:       0

				const AutoDiff autoDiff = AutoDiff::min(AutoDiff(x) * AutoDiff(x), c);

				const T expectedValue = std::min(x * x, c);
				const T expectedDerivative = x * x < c ? T(2) * x : T(0);

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = max(x * x, c)
				// f'(x) = x * x > c:         2x
				//              else:         0

				const AutoDiff autoDiff = AutoDiff::max(AutoDiff(x) * AutoDiff(x), c);

				const T expectedValue = std::max(x * x, c);
				const T expectedDerivative = x * x > c ? T(2) * x : T(0);

				if (NumericT<T>::isNotEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestAutomaticDifferentiation::testNested(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing automatic differentiation of nested functions with " << TypeNamer::name<T>() << ":";

	using AutoDiff = AutomaticDifferentiationT<T>;

	RandomGenerator randomGenerator;

	constexpr double successThreshold = std::is_same<float, T>::value ? 0.95 : 0.98;

	ValidationPrecision validation(successThreshold, randomGenerator);

	constexpr T valueRange = std::is_same<float, T>::value ? T(10) : T(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const T x = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			const T c = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = g(h(x))
				// h(x) = x^2
				// g(y) = 2y
				//
				// f'(x) = g'(h(x)) * h'(x)
				//       = g'(x^2) * 2x
				//       = 2 * 2x            = [2 * x^2]' = 4x

				const AutoDiff y = AutoDiff(x) * AutoDiff(x);
				const AutoDiff autoDiff = T(2) * y;

				const T expectedValue = T(2) * (x * x);
				const T expectedDerivative = T(4) * x;

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = g(h(x))
				// h(x) = x^2 + c
				// g(y) = 5 - 2y
				//
				// f'(x) = g'(h(x)) * h'(x)
				//       = g'(x^2 + c) * 2x
				//       = -2 * 2x            = [6 - 2 * (x^2 + c)]' = [6 - 2x^2 - 2c]' = -4x

				const AutoDiff y = AutoDiff(x) * AutoDiff(x) + c;
				const AutoDiff autoDiff = T(5) - T(2) * y;

				const T expectedValue = T(5) - T(2) * (x * x + c);
				const T expectedDerivative = -T(4) * x;

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = k(g(h(x))
				// h(x) = x^2 + 9
				// g(y) = c * y + 7
				// k(z) = z * 4
				//
				// f'(x) = k'(g(h(x)) * g'(h(x)) * h'(x)
				//       = 4 * c * 2x                     = [((c * (x^2 + 9) + 7) * 4]' = [(cx^2 + 9c + 7) * 4]' = [4cx^2 + 36c + 28]' = 8cx

				const AutoDiff y = AutoDiff(x) * AutoDiff(x) + T(9);
				const AutoDiff z = c * y + T(7);
				const AutoDiff autoDiff = z * T(4);

				const T expectedValue = (c * (x * x + T(9)) + T(7)) * T(4);
				const T expectedDerivative = T(8) * c * x;

				if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
				{
					scopedIteration.setInaccurate();
				}

				if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
				{
					scopedIteration.setInaccurate();
				}
			}

			{
				ValidationPrecision::ScopedIteration scopedIteration(validation);

				// f(x) = e^(5x^2 - 3x + c)
				// f'(x) = e^(5x^2 - 3x + c) * (10x - 3)

				const T _x = RandomT<T>::scalar(randomGenerator, -1, 1);
				const T _c = RandomT<T>::scalar(randomGenerator, -1, 1);

				const T expectedValue = NumericT<T>::exp(T(5) * _x * _x - T(3) * _x + _c);
				const T expectedDerivative = NumericT<T>::exp(T(5) * _x * _x - T(3) * _x + _c) * (T(10) * _x - T(3));

				{
					const AutoDiff autoDiff = AutoDiff::exp(T(5) * AutoDiff(_x) * AutoDiff(_x) - T(3) * AutoDiff(_x) + _c);

					if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
					{
						scopedIteration.setInaccurate();
					}

					if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
					{
						scopedIteration.setInaccurate();
					}
				}

				{
					// alternative way to directly use x^2 as value

					const AutoDiff autoDiff = AutoDiff::exp(T(5) * AutoDiff(_x * _x, T(2) * _x) - T(3) * AutoDiff(_x) + _c);

					if (NumericT<T>::isNotWeakEqual(autoDiff.value(), expectedValue))
					{
						scopedIteration.setInaccurate();
					}

					if (NumericT<T>::isNotWeakEqual(autoDiff.derivative(), expectedDerivative))
					{
						scopedIteration.setInaccurate();
					}
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestAutomaticDifferentiation::testHomography(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	const unsigned int width = 1920u;
	const unsigned int height = 1080u;

	Log::info() << "Testing automatic differentiation for the homography Jacobian 2x8 for " << numberPoints << " points with " << TypeNamer::name<T>() << ":";

	using AutoDiff = AutomaticDifferentiationT<T>;

	RandomGenerator randomGenerator;

	ValidationPrecision validation(0.99, randomGenerator);

	HighPerformanceStatistic performanceAutomatic;
	HighPerformanceStatistic performanceDirect;

	const Timestamp startTimestamp(true);

	do
	{
		VectorsT2<T> points(numberPoints);

		std::vector<T> jacobians(numberPoints * 8u * 2u);
		std::vector<T> testJacobians(numberPoints * 8u * 2u);

		SquareMatrixT3<T> homography;

		for (unsigned int n = 0u; n < 9u; ++n)
		{
			homography[n] = RandomT<T>::scalar(randomGenerator, -10, 10);
		}

		while (NumericT<T>::isEqualEps(homography[8]))
		{
			homography[8] = RandomT<T>::scalar(randomGenerator, -10, 10);
		}

		for (unsigned int n = 0u; n < 9u; ++n)
		{
			homography[n] /= homography[8];
		}

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			points[n] = RandomT<T>::vector2(randomGenerator, T(0), T(width), T(0), T(height));
		}

		// Homography:
		// |   h0    h3     h6  |
		// |   h1    h4		h7  |
		// |   h2    h5     1   |

		// hx(x, y) = [ h0*x + h3*y + h6 ] / [h2*x + h5*y + z]
		// hy(x, y) = [ h1*x + h4*y + h7 ] / [h2*x + h5*y + z]

		performanceAutomatic.start();

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			const T& x = points[n].x();
			const T& y = points[n].y();

			T* jx = jacobians.data() + n * 8u * 2u;
			T* jy = jx + 8;

			for (unsigned int i = 0u; i < 8u; ++i)
			{
				const AutoDiff dx = (AutoDiff(homography[0], i == 0u) * x + AutoDiff(homography[3], i == 3u) * y + AutoDiff(homography[6], i == 6u)) / (AutoDiff(homography[2], i == 2u) * x + AutoDiff(homography[5], i == 5u) * y + T(1));
				const AutoDiff dy = (AutoDiff(homography[1], i == 1u) * x + AutoDiff(homography[4], i == 4u) * y + AutoDiff(homography[7], i == 7u)) / (AutoDiff(homography[2], i == 2u) * x + AutoDiff(homography[5], i == 5u) * y + T(1));

				jx[i] = dx();
				jy[i] = dy();
			}
		}

		performanceAutomatic.stop();

		performanceDirect.start();

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			const T& x = points[n].x();
			const T& y = points[n].y();

			// code taken from Jacobian::calculateHomographyJacobian2x8

			const T u = homography[0] * x + homography[3] * y + homography[6];
			const T v = homography[1] * x + homography[4] * y + homography[7];
			const T w = homography[2] * x + homography[5] * y + T(1);

			ocean_assert(NumericT<T>::isNotEqualEps(w));
			const T invW = T(1) / w;
			const T invW2 = invW * invW;

			T* jx = testJacobians.data() + n * 8u * 2u;
			T* jy = jx + 8;

			jx[0] = x * invW;
			jx[1] = T(0);
			jx[2] = -x * u * invW2;
			jx[3] = y * invW;
			jx[4] = T(0);
			jx[5] = -y * u * invW2;
			jx[6] = invW;
			jx[7] = T(0);

			jy[0] = T(0);
			jy[1] = x * invW;
			jy[2] = -x * v * invW2;
			jy[3] = T(0);
			jy[4] = y * invW;
			jy[5] = -y * v * invW2;
			jy[6] = T(0);
			jy[7] = invW;
		}

		performanceDirect.stop();

		size_t accurateJacobians = 0;

		for (size_t n = 0u; n < jacobians.size(); ++n)
		{
			if (NumericT<T>::isWeakEqual(jacobians[n], testJacobians[n]))
			{
				++accurateJacobians;
			}
		}

		validation.addIterations(accurateJacobians, jacobians.size());
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance automatic: " << performanceAutomatic;
	Log::info() << "Performance direct: " << performanceDirect;
	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestAutomaticDifferentiation::testPose(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	const unsigned int width = 1920u;
	const unsigned int height = 1080u;

	Log::info() << "Testing automatic differentiation for the 6-DOF camera pose for " << numberPoints << " points with " << TypeNamer::name<T>() << ":";

	using AutoDiff = AutomaticDifferentiationT<T>;

	HighPerformanceStatistic performanceAutomatic;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		VectorsT3<T> objectPoints(numberPoints);

		std::vector<T> jacobians(numberPoints * 6u * 2u);

		const VectorT3<T> translation(RandomT<T>::vector3(randomGenerator, -10, 10));
		const QuaternionT<T> quaternion(RandomT<T>::quaternion(randomGenerator));

		const PoseT<T> world_P_camera(translation, quaternion);
		const HomogenousMatrixT4<T> world_T_camera(translation, quaternion);

		const HomogenousMatrixT4<T> flippedCamera_T_world(CameraT<T>::standard2InvertedFlipped(world_T_camera));
		const PoseT<T> flippedCamera_P_world(flippedCamera_T_world);

		const PinholeCameraT<T> pinholeCamera(width, height, NumericT<T>::deg2rad(60));

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			const VectorT2<T> imagePoint = RandomT<T>::vector2(randomGenerator, T(0), T(pinholeCamera.width()), T(0), T(pinholeCamera.height()));

			const LineT3<T> ray(pinholeCamera.ray(imagePoint, translation, quaternion));
			const VectorT3<T> objectPoint(ray.point(RandomT<T>::scalar(randomGenerator, 1, 5)));

			objectPoints[n] = objectPoint;
		}

		/**
		 * Rodriguez formula:
		 *
		 * Exponential map w := (wx, wy, wz)
		 * Angle axis rotation: r := (nx, ny, nz, a)
		 *                         = (nx, ny, nz, ||w||)
		 *                         = (nx, ny, nz, ||w||)
		 *                         = (wx / ||w||, wy / ||w||, wz / ||w||, ||w||)
		 *
		 * Angle a := ||w|| = sqrt(wx^2 + wy^2 + wz^2)
		 *
		 * R := cos(a) * I + (1 - cos(a)) * nnT + sin(a) * [n]x
		 *    = cos(a) * I + ((1 - cos(a)) / a^2) * wwT + (sin(a) / a) * [w]x
		 * with [n]x the skew symmetric matrix of n.
		 *
		 *              | 1 0 0 |                          | wx^2    wx*wy   wx*wz |                  |  0  -wz   wy |
		 * R = cos(a) * | 0 1 0 | + ((1 - cos(a)) / a^2) * | wx*wy   wy^2    wy*wz | + (sin(a) / a) * |  wz   0  -wx |
		 *              | 0 0 1 |                          | wx*wz   wy*wz   wz^2  |                  | -wy  wx   0  |
		 *
		 * | zx |   | Fx   0  mx |   | Rxx Ryx Rzx Tx |   | X |
		 * | zy | = | 0   Fy  my | * | Rxy Ryy Rzy Ty | * | Y |
		 * | z  |   | 0    0   1 |   | Rxz Ryz Rzz Tz |   | Z |
		 */

		performanceAutomatic.start();

		StaticMatrix<AutoDiff, 3, 3> camera33(AutoDiff(0, false));
		camera33.template element<0, 0>() = AutoDiff(pinholeCamera.focalLengthX(), false);
		camera33.template element<1, 1>() = AutoDiff(pinholeCamera.focalLengthY(), false);
		camera33.template element<0, 2>() = AutoDiff(pinholeCamera.principalPointX(), false);
		camera33.template element<1, 2>() = AutoDiff(pinholeCamera.principalPointY(), false);
		camera33.template element<2, 2>() = AutoDiff(1, false);

		StaticMatrix<AutoDiff, 3, 4> pose34;
		StaticMatrix<AutoDiff, 4, 1> point41;

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			const T& x = objectPoints[n].x();
			const T& y = objectPoints[n].y();
			const T& z = objectPoints[n].z();

			T* jx = jacobians.data() + n * 6u * 2u;
			T* jy = jx + 6;

			for (unsigned int i = 0u; i < 6u; ++i)
			{
				const AutoDiff wx(flippedCamera_P_world.rx(), i == 0u);
				const AutoDiff wy(flippedCamera_P_world.ry(), i == 1u);
				const AutoDiff wz(flippedCamera_P_world.rz(), i == 2u);
				const AutoDiff tx(flippedCamera_P_world.x(), i == 3u);
				const AutoDiff ty(flippedCamera_P_world.y(), i == 4u);
				const AutoDiff tz(flippedCamera_P_world.z(), i == 5u);

				const AutoDiff angle(AutoDiff::sqrt(wx * wx +  wy * wy + wz * wz));
				const AutoDiff cosAngle(AutoDiff::cos(angle));
				const AutoDiff cosAngle1_a2 = (1 - cosAngle) / (angle * angle);
				const AutoDiff sin_a(AutoDiff::sin(angle) / angle);

				pose34.template element<0, 0>() = cosAngle + cosAngle1_a2 * wx * wx;
				pose34.template element<0, 1>() = cosAngle1_a2 * wx * wy - sin_a * wz;
				pose34.template element<0, 2>() = cosAngle1_a2 * wx * wz + sin_a * wy;
				pose34.template element<0, 3>() = tx;

				pose34.template element<1, 0>() = cosAngle1_a2 * wx * wy + sin_a * wz;
				pose34.template element<1, 1>() = cosAngle + cosAngle1_a2 * wy * wy;
				pose34.template element<1, 2>() = cosAngle1_a2 * wy * wz - sin_a * wx;
				pose34.template element<1, 3>() = ty;

				pose34.template element<2, 0>() = cosAngle1_a2 * wx * wz - sin_a * wy;
				pose34.template element<2, 1>() = cosAngle1_a2 * wy * wz + sin_a * wx;
				pose34.template element<2, 2>() = cosAngle + cosAngle1_a2 * wz * wz;
				pose34.template element<2, 3>() = tz;

				point41.template element<0, 0>() = AutoDiff(x, false);
				point41.template element<1, 0>() = AutoDiff(y, false);
				point41.template element<2, 0>() = AutoDiff(z, false);
				point41.template element<3, 0>() = AutoDiff(1, false);

				StaticMatrix<AutoDiff, 3, 1> result = camera33 * pose34 * point41;

				const AutoDiff dx = result[0] / result[2];
				const AutoDiff dy = result[1] / result[2];

				jx[i] = dx();
				jy[i] = dy();
			}
		}

		performanceAutomatic.stop();

#if 0
		// the performance 'Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6' will be approx. 25 times faster
		Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6(testJacobians.data(), pinholeCamera, flippedCamera_P_world, objectPoints.data(), objectPoints.size(), false);
#endif
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance automatic: " << performanceAutomatic;
	return true;
}

}

}

}
