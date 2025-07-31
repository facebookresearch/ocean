/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestApproximation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Approximation.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestApproximation::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Approximation Test:   ---";
	Log::info() << " ";

	allSucceeded = testSqrt8(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSqrt16(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSqrt17(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAtan2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testExp<float, false>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testExp<float, true>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testExp<double, false>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testExp<double, true>(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Approximation test succeeded.";
	}
	else
	{
		Log::info() << "Approximation box test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestApproximation, Sqrt8)
{
	EXPECT_TRUE(TestApproximation::testSqrt8(GTEST_TEST_DURATION));
}

TEST(TestApproximation, Sqrt16)
{
	EXPECT_TRUE(TestApproximation::testSqrt16(GTEST_TEST_DURATION));
}

TEST(TestApproximation, Sqrt17)
{
	EXPECT_TRUE(TestApproximation::testSqrt17(GTEST_TEST_DURATION));
}

TEST(TestApproximation, Atan2)
{
	EXPECT_TRUE(TestApproximation::testAtan2(GTEST_TEST_DURATION));
}

TEST(TestApproximation, ExpFloatLower)
{
	EXPECT_TRUE((TestApproximation::testExp<float, false>(GTEST_TEST_DURATION)));
}

TEST(TestApproximation, ExpFloatHigher)
{
	EXPECT_TRUE((TestApproximation::testExp<float, true>(GTEST_TEST_DURATION)));
}

TEST(TestApproximation, ExpDoubleLower)
{
	EXPECT_TRUE((TestApproximation::testExp<double, false>(GTEST_TEST_DURATION)));
}

TEST(TestApproximation, ExpDoubleHigher)
{
	EXPECT_TRUE((TestApproximation::testExp<double, true>(GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

bool TestApproximation::testSqrt8(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test sqrt() approximation for 8 bit integers:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const uint8_t value = uint8_t(RandomI::random(0u, 255u));
			const uint32_t valueSqrt = Approximation::sqrt(value);

			const double testValueSqrt = NumericD::sqrt(double(value));
			const uint32_t roundedTestValueSqrt = uint32_t(testValueSqrt + 0.5);

			if (valueSqrt != roundedTestValueSqrt)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestApproximation::testSqrt16(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test sqrt() approximation for 16 bit integers:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const uint16_t value = uint16_t(RandomI::random32() % 0x0000FFFFu);
			const uint32_t valueSqrt = Approximation::sqrt(value);

			const double testValueSqrt = NumericD::sqrt(double(value));
			const uint32_t roundedTestValueSqrt = uint32_t(testValueSqrt + 0.5);

			if (valueSqrt != roundedTestValueSqrt)
			{
				// the approximated result will be accurate within the range [0, 65280]
				if (value <= 65280u || valueSqrt != 255u || roundedTestValueSqrt != 256u)
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestApproximation::testSqrt17(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test sqrt() approximation for 32 bit integers:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const uint32_t value = RandomI::random32() % 131072u;
			const uint32_t valueSqrt = Approximation::sqrt(value);

			const double testValueSqrt = NumericD::sqrt(double((value / 2u) * 2u));
			const uint32_t roundedTestValueSqrt = uint32_t(testValueSqrt + 0.5);

			if (valueSqrt != roundedTestValueSqrt)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestApproximation::testAtan2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test atan2 approximation:";

	const size_t constIterations = 100000;

	Scalars xValues(constIterations);
	Scalars yValues(constIterations);

	for (size_t n = 0; n < constIterations; ++n)
	{
		xValues[n] = Random::scalar(-1000, 1000);
		yValues[n] = Random::scalar(-1000, 1000);
	}

	Scalar valueNumeric = Scalar(0);
	unsigned long long numericIterations = 0ull;

	HighPerformanceTimer timer;

	// test the numeric implementation
	const Timestamp startTimestampNumeric(true);
	do
	{
		for (size_t n = 0; n < constIterations; ++n)
		{
			valueNumeric += Numeric::atan2(yValues[n], xValues[n]);
		}

		numericIterations++;
	}
	while (startTimestampNumeric + testDuration > Timestamp(true));

	ocean_assert(numericIterations != 0ull && constIterations != 0);
	const double numericTime = timer.nseconds() / (double(numericIterations) * double(constIterations));

	// test the approximated implementation
	Scalar valueApproximation = Scalar(0);
	unsigned long long approximationIterations = 0ull;

	timer.start();

	const Timestamp startTimestampApproximation(true);
	do
	{
		for (size_t n = 0; n < constIterations; ++n)
		{
			valueApproximation += Approximation::atan2(yValues[n], xValues[n]);
		}

		approximationIterations++;
	}
	while (startTimestampApproximation + testDuration > Timestamp(true));

	ocean_assert(numericIterations != 0ull && constIterations != 0);
	const double approximationTime = timer.nseconds() / (double(approximationIterations) * (constIterations));

	ocean_assert(numericIterations != 0u);

	Log::info() << "Standard performance: " << numericTime << "ns";
	Log::info() << "Approximation performance: " << approximationTime << "ns";

	ocean_assert(Numeric::isNotEqualEps(Scalar(numericTime)));

	if (valueApproximation >= valueNumeric)
	{
		Log::info() << "Approximation performance factor: " << String::toAString(Scalar(numericTime) / Scalar(approximationTime), 1u) << "x";
	}
	else
	{
		Log::info() << "Approximation performance factor: " << String::toAString(Scalar(numericTime) / Scalar(approximationTime), 1u) << "x";
	}

	const bool succeeded = validateAtan2(testDuration);

	if (succeeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return succeeded;
}

template <typename T, bool tHigherAccuracy>
bool TestApproximation::testExp(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test exp approximation for " << TypeNamer::name<T>() << (tHigherAccuracy ? " higher accuracy:" : " lower accuracy:");
	Log::info() << " ";

	HighPerformanceStatistic performanceAccurate;
	HighPerformanceStatistic performanceApproximated;

	const Timestamp startTimestamp(true);

	const unsigned int repetitions = 10000u;

	std::vector<T> xs(repetitions);
	std::vector<T> accurateExps(repetitions);
	std::vector<T> approximatedExps(repetitions);

	std::vector<double> errors;
	errors.reserve(1024 * 8);

	do
	{
		for (unsigned int n = 0u; n < repetitions; ++n)
		{
			xs[n] = RandomT<T>::scalar(-1, 1);
		}

		performanceAccurate.start();
		for (unsigned int n = 0u; n < repetitions; ++n)
		{
			accurateExps[n] = NumericT<T>::exp(xs[n]);
		}
		performanceAccurate.stop();

		performanceApproximated.start();
		for (unsigned int n = 0u; n < repetitions; ++n)
		{
			approximatedExps[n] = Approximation::exp<T, tHigherAccuracy>(xs[n]);
		}
		performanceApproximated.stop();

		for (unsigned int n = 0u; n < repetitions; ++n)
		{
			errors.push_back(NumericD::abs(double(accurateExps[n]) - double(approximatedExps[n])));
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(!errors.empty());
	std::sort(errors.begin(), errors.end());

	Log::info() << "Performance accurate: " << performanceAccurate.averageMseconds() * (1000.0 / double(repetitions)) << "ns";
	Log::info() << "Performance approximated: " << performanceApproximated.averageMseconds() * (1000.0 / double(repetitions)) << "ns";
	Log::info() << "Performance factor: " << String::toAString(performanceAccurate.average() / performanceApproximated.average(), 1u) << "x";

	Log::info() << " ";

	Log::info() << "Best error: " << errors.front();
	Log::info() << "Median error: " << errors[errors.size() / 2];
	Log::info() << "p90 error: " << errors[errors.size() * 90 / 100];
	Log::info() << "p95 error: " << errors[errors.size() * 95 / 100];
	Log::info() << "p99 error: " << errors[errors.size() * 99 / 100];
	Log::info() << "Worst error: "<< errors.back();

	const bool succeeded = errors.back() < T(0.1);

	if (succeeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return succeeded;
}

bool TestApproximation::validateAtan2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int constIterations = 100000u;

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0; n < constIterations; ++n)
		{
			int x = Random::random(-1000, 1000);
			int y = Random::random(-1000, 1000);

			while (Numeric::isEqualEps(Scalar(x)) && Numeric::isEqualEps(Scalar(y)))
			{
				x = Random::random(-1000, 1000);
				y = Random::random(-1000, 1000);
			}

			const Scalar angle = Numeric::atan2(Scalar(y), Scalar(x));
			const Scalar angle2 = Approximation::atan2(Scalar(y), Scalar(x));

			++iterations;

			if (angle2 < -Numeric::pi() && angle2 > Numeric::pi())
				continue;

			const Scalar angleDeg = Numeric::rad2deg(angle);
			const Scalar angleDeg2 = Numeric::rad2deg(angle2);

			const Scalar diffDeg = Numeric::abs(angleDeg - angleDeg2);

			if (Numeric::abs(diffDeg) < 1)
			{
				++validIterations;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	return percent >= 0.99;
}

}

}

}
