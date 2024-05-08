// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/test/testmath/TestScalarAutomaticDifferentiation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Pose.h"
#include "ocean/math/ScalarAutomaticDifferentiation.h"
#include "ocean/math/StaticMatrix.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestScalarAutomaticDifferentiation::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Automatic scalar differentiation test:   ---";
	Log::info() << " ";

	allSucceeded = testSimple(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testFunctions(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNested(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHomography(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPose(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Differentiation test succeeded.";
	else
		Log::info() << "Differentiation test FAILED!";

	return allSucceeded;
}
	
#ifdef OCEAN_USE_GTEST
	
TEST(TestScalarAutomaticDifferentiation, Simple) {
	EXPECT_TRUE(TestScalarAutomaticDifferentiation::testSimple(GTEST_TEST_DURATION));
}

TEST(TestScalarAutomaticDifferentiation, Functions) {
	EXPECT_TRUE(TestScalarAutomaticDifferentiation::testFunctions(GTEST_TEST_DURATION));
}

TEST(TestScalarAutomaticDifferentiation, Nested) {
	EXPECT_TRUE(TestScalarAutomaticDifferentiation::testNested(GTEST_TEST_DURATION));
}

TEST(TestScalarAutomaticDifferentiation, Homography) {
	EXPECT_TRUE(TestScalarAutomaticDifferentiation::testHomography(GTEST_TEST_DURATION));
}

TEST(TestScalarAutomaticDifferentiation, Pose) {
	EXPECT_TRUE(TestScalarAutomaticDifferentiation::testPose(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestScalarAutomaticDifferentiation::testSimple(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing automatic differentiation of simple functions:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Scalar x = Random::scalar(-valueRange, valueRange);
			const Scalar c = Random::scalar(-valueRange, valueRange);

			{
				// f(x) = x
				// f'(x) = 1

				const Scalar derivative = ScalarAutomaticDifferentiation(x).derivative();
				const Scalar test = 1;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = x + c
				// f'(x) = 1

				const Scalar derivative = (ScalarAutomaticDifferentiation(x) + c).derivative();
				const Scalar test = 1;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = c + x
				// f'(x) = 1

				const Scalar derivative = (c + ScalarAutomaticDifferentiation(x)).derivative();
				const Scalar test = 1;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = x - c
				// f'(x) = 1

				const Scalar derivative = (ScalarAutomaticDifferentiation(x) - c).derivative();
				const Scalar test = 1;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = c - x
				// f'(x) = -1

				const Scalar derivative = (c - ScalarAutomaticDifferentiation(x)).derivative();
				const Scalar test = -1;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = x + x + c
				// f'(x) = 2

				const Scalar derivative = (ScalarAutomaticDifferentiation(x) + ScalarAutomaticDifferentiation(x) + c).derivative();
				const Scalar test = 2;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = cx
				// f'(x) = c

				const Scalar derivative = (ScalarAutomaticDifferentiation(x) * c).derivative();
				const Scalar test = c;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = x^2
				// f'(x) = 2x

				const Scalar derivative = (ScalarAutomaticDifferentiation(x) * ScalarAutomaticDifferentiation(x)).derivative();
				const Scalar test = Scalar(2) * x;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = c * x^2
				// f'(x) = 2cx

				const Scalar derivative = (ScalarAutomaticDifferentiation(x) * ScalarAutomaticDifferentiation(x) * c).derivative();
				const Scalar test = Scalar(2) * x * c;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) == x^3
				// f'(x) = 3x^2

				const Scalar derivative = (ScalarAutomaticDifferentiation(x) * ScalarAutomaticDifferentiation(x) * ScalarAutomaticDifferentiation(x)).derivative();
				const Scalar test = Scalar(3) * x * x;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) == 3 * (x + c)
				// f'(x) = 3

				const Scalar derivative = (3 * (ScalarAutomaticDifferentiation(x) + c)).derivative();
				const Scalar test = Scalar(3);

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) == (3 * (x + 2))^2 = (3x + 6)^2 = 9x^2 + 36x + 36
				// f'(x) = 2 * (3x + 6) * 3 = 18x + 36

				const Scalar derivative = (((ScalarAutomaticDifferentiation(x) + 2) * 3) * ((ScalarAutomaticDifferentiation(x) + 2) * 3)).derivative();
				const Scalar test = Scalar(2) * (Scalar(3) * x + Scalar(6)) * Scalar(3);

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) == (3 * (x + c))^2 = (3x + 3c)^2 = 9x^2 + 18cx + 9c^2
				// f'(x) = 2 * (3x + 3c) * 3 = 18x + 18c

				const Scalar derivative = (((ScalarAutomaticDifferentiation(x) + c) * 3) * ((ScalarAutomaticDifferentiation(x) + c) * 3)).derivative();
				const Scalar test = Scalar(2) * (Scalar(3) * x + Scalar(3) * c) * Scalar(3);

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			if (Numeric::isNotEqualEps(x))
			{
				// f(x) = 1 / x
				// f'(x) = -1 / x^2

				const Scalar derivative = (Scalar(1) / ScalarAutomaticDifferentiation(x)).derivative();
				const Scalar test = -Scalar(1) / (x * x);

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			if (Numeric::isNotEqualEps(x))
			{
				// f(x) = c / x
				// f'(x) = -c / x^2

				const Scalar derivative = (c / ScalarAutomaticDifferentiation(x)).derivative();
				const Scalar test = -c / (x * x);

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			if (Numeric::isNotEqualEps(x * x))
			{
				// f(x) = c / x^2
				// f'(x) = -2c / x^3

				const Scalar derivative = (c / (ScalarAutomaticDifferentiation(x) * ScalarAutomaticDifferentiation(x))).derivative();
				const Scalar test = -Scalar(2) * c / (x * x * x);

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			if (Numeric::isNotEqualEps(x * x))
			{
				// f(x) = (c / x) * (1 / x)
				// f'(x) = -2c / x^3

				const Scalar derivative = ((c / ScalarAutomaticDifferentiation(x)) * (1 / ScalarAutomaticDifferentiation(x))).derivative();
				const Scalar test = -Scalar(2) * c / (x * x * x);

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}
		}

	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0u);

	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestScalarAutomaticDifferentiation::testFunctions(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing automatic differentiation of mathematic functions:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Scalar x = Random::scalar(-valueRange, valueRange);
			const Scalar c = Random::scalar(-valueRange, valueRange);

			{
				// f(x) = sin(x)
				// f'(x) = cos(x)

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::sin(ScalarAutomaticDifferentiation(x));

				const Scalar derivative = f.derivative();
				const Scalar test = Numeric::cos(x);

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = cos(x)
				// f'(x) = -sin(x)

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::cos(ScalarAutomaticDifferentiation(x));

				const Scalar derivative = f.derivative();
				const Scalar test = -Numeric::sin(x);

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = tan(x)
				// f'(x) = 1 / cos(x)^2

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::tan(ScalarAutomaticDifferentiation(x));

				const Scalar derivative = f.derivative();
				const Scalar test = Scalar(1) / Numeric::sqr(Numeric::cos(x));

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			if (x >= Scalar(0))
			{
				// f(x) = sqrt(x)
				// f'(x) = 1 / 2 * sqrt(x)

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::sqrt(ScalarAutomaticDifferentiation(x));

				const Scalar derivative = f.derivative();
				const Scalar test = Scalar(1) / (Scalar(2) * Numeric::sqrt(x));

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = exp(x)
				// f'(x) = exp(x)

				const Scalar x2 = Random::scalar(-10, 10);

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::exp(ScalarAutomaticDifferentiation(x2));

				const Scalar derivative = f.derivative();
				const Scalar test = Numeric::exp(x2);

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			if (x > Numeric::eps())
			{
				// f(x) = log(x)
				// f'(x) = 1 / x

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::log(ScalarAutomaticDifferentiation(x));

				const Scalar derivative = f.derivative();
				const Scalar test = Scalar(1) / x;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			if (x > Numeric::eps())
			{
				// f(x) = log2(x) = log(x) / log(2)
				// f'(x) = (1 / x) * log(2) / log(2)^2 = 1 / (x * log(2))

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::log2(ScalarAutomaticDifferentiation(x));

				const Scalar derivative = f.derivative();
				const Scalar test = Scalar(1) / (x * Numeric::log(2));

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			if (x > Numeric::eps())
			{
				// f(x) = log10(x) = log(x) / log(10)
				// f'(x) = (1 / x) * log(10) / log(10)^2 = 1 / (x * log(10))

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::log10(ScalarAutomaticDifferentiation(x));

				const Scalar derivative = f.derivative();
				const Scalar test = Scalar(1) / (x * Numeric::log(10));

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			if (x > Numeric::eps())
			{
				// f(x) = log(x) / log10(x)
				// f'(x) = [log10(x) / x - log(x) / (x * log(10))] / log10(x)^2 = 0

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::log(ScalarAutomaticDifferentiation(x)) / ScalarAutomaticDifferentiation::log10(ScalarAutomaticDifferentiation(x));

				const Scalar derivative = f.derivative();
				const Scalar test = 0;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			if (x >= Scalar(0))
			{
				// f(x, y) = x ^ y
				// f'(x, y) = y * x ^ (y - 1)

				const Scalar y = Random::scalar(-10, 10);

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::pow(ScalarAutomaticDifferentiation(x), y);

				const Scalar derivative = f.derivative();
				const Scalar test = y * Numeric::pow(x, y - Scalar(1));

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = |x|
				// f'(x) = sign(x)

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::abs(ScalarAutomaticDifferentiation(x));

				const Scalar derivative = f.derivative();
				const Scalar test = x >= Scalar(0) ? Scalar(1) : Scalar(-1);

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = min(x, c)
				// f'(x) = x < c: x'      x >= c 0

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::min(ScalarAutomaticDifferentiation(x) * ScalarAutomaticDifferentiation(x), c);

				const Scalar derivative = f.derivative();
				const Scalar test = (x * x) < c ? (2 * x) : 0;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = max(x, c)
				// f'(x) = x > c: x'      x >= c 0

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::max(ScalarAutomaticDifferentiation(x) * ScalarAutomaticDifferentiation(x), c);

				const Scalar derivative = f.derivative();
				const Scalar test = (x * x) > c ? (2 * x) : 0;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0u);

	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestScalarAutomaticDifferentiation::testNested(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing automatic differentiation of nested functions:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Scalar valueRange = std::is_same<float, Scalar>::value ? Scalar(10) : Scalar(1000);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const Scalar x = Random::scalar(-valueRange, valueRange);
			const Scalar c = Random::scalar(-valueRange, valueRange);

			{
				// f(x) = g(h(x))
				// h(x) = x^2
				// g(y) = 2y
				//
				// f'(x) = g'(h(x)) * h'(x)
				//       = g'(x^2) * 2x
				//       = 2 * 2x            = [2 * x^2]' = 4x

				const ScalarAutomaticDifferentiation y = ScalarAutomaticDifferentiation(x) * ScalarAutomaticDifferentiation(x);
				const ScalarAutomaticDifferentiation f = 2 * y;

				const Scalar derivative = f.derivative();
				const Scalar test = Scalar(4) * x;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = g(h(x))
				// h(x) = x^2 + c
				// g(y) = 5 - 2y
				//
				// f'(x) = g'(h(x)) * h'(x)
				//       = g'(x^2 + c) * 2x
				//       = -2 * 2x            = [6 - 2 * (x^2 + c)]' = [6 - 2x^2 - 2c]' = -4x

				const ScalarAutomaticDifferentiation y = ScalarAutomaticDifferentiation(x) * ScalarAutomaticDifferentiation(x) + c;
				const ScalarAutomaticDifferentiation f = 5 - 2 * y;

				const Scalar derivative = f.derivative();
				const Scalar test = Scalar(-4) * x;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = k(g(h(x))
				// h(x) = x^2 + 9
				// g(y) = c * y + 7
				// k(z) = z * 4
				//
				// f'(x) = k'(g(h(x)) * g'(h(x)) * h'(x)
				//       = 4 * c * 2x                     = [((c * (x^2 + 9) + 7) * 4]' = [(cx^2 + 9c + 7) * 4]' = [4cx^2 + 36c + 28]' = 8cx

				const ScalarAutomaticDifferentiation y = ScalarAutomaticDifferentiation(x) * ScalarAutomaticDifferentiation(x) + 9;
				const ScalarAutomaticDifferentiation z = c * ScalarAutomaticDifferentiation(y) + 7;
				const ScalarAutomaticDifferentiation f = z * 4;

				const Scalar derivative = f.derivative();
				const Scalar test = Scalar(8) * c * x;

				if (Numeric::isWeakEqual(derivative, test))
					validIterations++;

				iterations++;
			}

			{
				// f(x) = e^(5x^2 - 3x + c)
				// f'(x) = e^(5x^2 - 3x + c) * (10x - 3)

				const Scalar x2 = Random::scalar(-1, 1);
				const Scalar c2 = Random::scalar(-1, 1);

				const ScalarAutomaticDifferentiation f = ScalarAutomaticDifferentiation::exp(5 * ScalarAutomaticDifferentiation(x2) * ScalarAutomaticDifferentiation(x2) - 3 * ScalarAutomaticDifferentiation(x2) + c2);

				// alternative way to directly use x^2 as value
				const ScalarAutomaticDifferentiation f2 = ScalarAutomaticDifferentiation::exp(5 * ScalarAutomaticDifferentiation(x2 * x2, 2 * x2) - 3 * ScalarAutomaticDifferentiation(x2) + c2);
				ocean_assert(Numeric::isEqual<3>(f.derivative(), f2.derivative(), Scalar(0.1)));

				const Scalar derivative = f.derivative();
				const Scalar test = Numeric::exp(5 * x2 * x2 - 3 * x2 + c2) * (10 * x2 - 3);

				if (Numeric::isEqual<3>(derivative, test, Scalar(0.01)) && Numeric::isEqual<3>(f.derivative(), f2.derivative(), Scalar(0.01)))
					validIterations++;

				iterations++;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0u);

	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestScalarAutomaticDifferentiation::testHomography(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	const unsigned int width = 1920u;
	const unsigned int height = 1080u;

	Log::info() << "Testing automatic differentiation for the homography Jacobian 2x8 for " << numberPoints << " points:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	Vectors2 points(numberPoints);

	Scalars jacobians(numberPoints * 8u * 2u);
	Scalars testJacobians(numberPoints * 8u * 2u);

	HighPerformanceStatistic performanceAutomatic;
	HighPerformanceStatistic performanceDirect;

	const Timestamp startTimestamp(true);

	do
	{
		SquareMatrix3 homography;

		for (unsigned int n = 0u; n < 9u; ++n)
			homography[n] = Random::scalar(-10, 10);

		while (Numeric::isEqualEps(homography[8]))
			homography[8] = Random::scalar(-10, 10);

		for (unsigned int n = 0u; n < 9u; ++n)
			homography[n] /= homography[8];

		for (unsigned int n = 0u; n < numberPoints; ++n)
			points[n] = Vector2(Random::scalar(0, Scalar(width)), Random::scalar(0, Scalar(height)));

		// Homography:
		// |   h0    h3     h6  |
		// |   h1    h4		h7  |
		// |   h2    h5     1   |

		// hx(x, y) = [ h0*x + h3*y + h6 ] / [h2*x + h5*y + z]
		// hy(x, y) = [ h1*x + h4*y + h7 ] / [h2*x + h5*y + z]

		performanceAutomatic.start();

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			const Scalar& x = points[n].x();
			const Scalar& y = points[n].y();

			Scalar* jx = jacobians.data() + n * 8u * 2u;
			Scalar* jy = jx + 8;

			for (unsigned int i = 0u; i < 8u; ++i)
			{
				const ScalarAutomaticDifferentiation dx = (ScalarAutomaticDifferentiation(homography[0], i == 0u) * x + ScalarAutomaticDifferentiation(homography[3], i == 3u) * y + ScalarAutomaticDifferentiation(homography[6], i == 6u)) / (ScalarAutomaticDifferentiation(homography[2], i == 2u) * x + ScalarAutomaticDifferentiation(homography[5], i == 5u) * y + Scalar(1));
				const ScalarAutomaticDifferentiation dy = (ScalarAutomaticDifferentiation(homography[1], i == 1u) * x + ScalarAutomaticDifferentiation(homography[4], i == 4u) * y + ScalarAutomaticDifferentiation(homography[7], i == 7u)) / (ScalarAutomaticDifferentiation(homography[2], i == 2u) * x + ScalarAutomaticDifferentiation(homography[5], i == 5u) * y + Scalar(1));

				jx[i] = dx();
				jy[i] = dy();
			}
		}

		performanceAutomatic.stop();

		performanceDirect.start();

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			const Scalar& x = points[n].x();
			const Scalar& y = points[n].y();

			// code taken from Jacobian::calculateHomographyJacobian2x8

			const Scalar u = homography[0] * x + homography[3] * y + homography[6];
			const Scalar v = homography[1] * x + homography[4] * y + homography[7];
			const Scalar w = homography[2] * x + homography[5] * y + Scalar(1);

			ocean_assert(Numeric::isNotEqualEps(w));
			const Scalar invW = Scalar(1) / w;
			const Scalar invW2 = invW * invW;

			Scalar* jx = testJacobians.data() + n * 8u * 2u;
			Scalar* jy = jx + 8;

			jx[0] = x * invW;
			jx[1] = Scalar(0);
			jx[2] = -x * u * invW2;
			jx[3] = y * invW;
			jx[4] = Scalar(0);
			jx[5] = -y * u * invW2;
			jx[6] = invW;
			jx[7] = Scalar(0);

			jy[0] = Scalar(0);
			jy[1] = x * invW;
			jy[2] = -x * v * invW2;
			jy[3] = Scalar(0);
			jy[4] = y * invW;
			jy[5] = -y * v * invW2;
			jy[6] = Scalar(0);
			jy[7] = invW;
		}

		performanceDirect.stop();

		for (size_t n = 0u; n < jacobians.size(); ++n)
		{
			if (Numeric::isWeakEqual(jacobians[n], testJacobians[n]))
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0u);

	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Performance automatic: " << performanceAutomatic.averageMseconds() << "ms";
	Log::info() << "Performance direct: " << performanceDirect.averageMseconds() << "ms";
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestScalarAutomaticDifferentiation::testPose(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int numberPoints = 50u;

	const unsigned int width = 1920u;
	const unsigned int height = 1080u;

	Log::info() << "Testing automatic differentiation for the 6-DOF camera pose for " << numberPoints << " points:";

	Vectors3 objectPoints(numberPoints);

	Scalars jacobians(numberPoints * 6u * 2u);

	HighPerformanceStatistic performanceAutomatic;

	const Timestamp startTimestamp(true);

	do
	{
		const Vector3 translation(Random::vector3(-10, 10));
		const Quaternion quaternion(Random::quaternion());

		const Pose pose(translation, quaternion);
		const HomogenousMatrix4 transformation(translation, quaternion);

		const HomogenousMatrix4 transformationIF(PinholeCamera::standard2InvertedFlipped(transformation));
		const Pose poseIF(transformationIF);

		const PinholeCamera pinholeCamera(width, height, Numeric::deg2rad(60));

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			Vector2 tmpImagePoint(Random::vector2(0, 1));
			tmpImagePoint.x() *= Scalar(pinholeCamera.width());
			tmpImagePoint.y() *= Scalar(pinholeCamera.height());

			const Line3 ray(pinholeCamera.ray(tmpImagePoint, translation, quaternion));
			const Vector3 objectPoint(ray.point(Random::scalar(1, 5)));

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

		StaticMatrix<ScalarAutomaticDifferentiation, 3, 3> camera33(ScalarAutomaticDifferentiation(0, false));
		camera33.element<0, 0>() = ScalarAutomaticDifferentiation(pinholeCamera.focalLengthX(), false);
		camera33.element<1, 1>() = ScalarAutomaticDifferentiation(pinholeCamera.focalLengthY(), false);
		camera33.element<0, 2>() = ScalarAutomaticDifferentiation(pinholeCamera.principalPointX(), false);
		camera33.element<1, 2>() = ScalarAutomaticDifferentiation(pinholeCamera.principalPointY(), false);
		camera33.element<2, 2>() = ScalarAutomaticDifferentiation(1, false);

		StaticMatrix<ScalarAutomaticDifferentiation, 3, 4> pose34;
		StaticMatrix<ScalarAutomaticDifferentiation, 4, 1> point41;

		for (unsigned int n = 0u; n < numberPoints; ++n)
		{
			const Scalar& x = objectPoints[n].x();
			const Scalar& y = objectPoints[n].y();
			const Scalar& z = objectPoints[n].z();

			Scalar* jx = jacobians.data() + n * 6u * 2u;
			Scalar* jy = jx + 6;

			for (unsigned int i = 0u; i < 6u; ++i)
			{
				const ScalarAutomaticDifferentiation wx(poseIF.rx(), i == 0u);
				const ScalarAutomaticDifferentiation wy(poseIF.ry(), i == 1u);
				const ScalarAutomaticDifferentiation wz(poseIF.rz(), i == 2u);
				const ScalarAutomaticDifferentiation tx(poseIF.x(), i == 3u);
				const ScalarAutomaticDifferentiation ty(poseIF.y(), i == 4u);
				const ScalarAutomaticDifferentiation tz(poseIF.z(), i == 5u);

				const ScalarAutomaticDifferentiation angle(ScalarAutomaticDifferentiation::sqrt(wx * wx +  wy * wy + wz * wz));
				const ScalarAutomaticDifferentiation cosAngle(ScalarAutomaticDifferentiation::cos(angle));
				const ScalarAutomaticDifferentiation cosAngle1_a2 = (1 - cosAngle) / (angle * angle);
				const ScalarAutomaticDifferentiation sin_a(ScalarAutomaticDifferentiation::sin(angle) / angle);

				pose34.element<0, 0>() = cosAngle + cosAngle1_a2 * wx * wx;
				pose34.element<0, 1>() = cosAngle1_a2 * wx * wy - sin_a * wz;
				pose34.element<0, 2>() = cosAngle1_a2 * wx * wz + sin_a * wy;
				pose34.element<0, 3>() = tx;

				pose34.element<1, 0>() = cosAngle1_a2 * wx * wy + sin_a * wz;
				pose34.element<1, 1>() = cosAngle + cosAngle1_a2 * wy * wy;
				pose34.element<1, 2>() = cosAngle1_a2 * wy * wz - sin_a * wx;
				pose34.element<1, 3>() = ty;

				pose34.element<2, 0>() = cosAngle1_a2 * wx * wz - sin_a * wy;
				pose34.element<2, 1>() = cosAngle1_a2 * wy * wz + sin_a * wx;
				pose34.element<2, 2>() = cosAngle + cosAngle1_a2 * wz * wz;
				pose34.element<2, 3>() = tz;

				point41.element<0, 0>() = ScalarAutomaticDifferentiation(x, false);
				point41.element<1, 0>() = ScalarAutomaticDifferentiation(y, false);
				point41.element<2, 0>() = ScalarAutomaticDifferentiation(z, false);
				point41.element<3, 0>() = ScalarAutomaticDifferentiation(1, false);

				StaticMatrix<ScalarAutomaticDifferentiation, 3, 1> result = camera33 * pose34 * point41;

				const ScalarAutomaticDifferentiation dx = result[0] / result[2];
				const ScalarAutomaticDifferentiation dy = result[1] / result[2];

				jx[i] = dx();
				jy[i] = dy();
			}
		}

		performanceAutomatic.stop();

#if 0
		// the performance 'Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6' will be approx. 25 times faster
		Geometry::Jacobian::calculatePoseJacobianRodrigues2nx6(testJacobians.data(), pinholeCamera, poseIF, objectPoints.data(), objectPoints.size(), false);

		for (size_t n = 0u; n < jacobians.size(); ++n)
		{
			if (Numeric::isWeakEqual(jacobians[n], testJacobians[n]))
				validIterations++;

			iterations++;
		}
#endif
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance automatic: " << performanceAutomatic.averageMseconds() << "ms";
	return true;
}

}

}

}
