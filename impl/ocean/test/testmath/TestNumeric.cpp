/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestNumeric.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestNumeric::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Numeric tests:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testEps() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testWeakEps() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsEqualDynamic() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRound() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAngleConversion<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testAngleConversion<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAngleAdjustPositive(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAngleAdjustNull(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAngleIsEqual(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAngleIsBelowThreshold(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testGaussianDistribution1(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testGaussianDistribution2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testGaussianDistribution3(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAbs() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSecureAbs() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFloor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCeil(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testLog2(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDotProduct<float>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDotProduct<double>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSign(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopySign() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInvertSign() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFactorial() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsNan<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testIsNan<double>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testIsNan<uint8_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testIsNan<int32_t>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsInf<float>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testIsInf<double>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testIsInf<uint8_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testIsInf<int32_t>(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPow() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsInsideValueRange(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Numeric test succeeded.";
	}
	else
	{
		Log::info() << "Numeric test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestNumeric, Eps)
{
	EXPECT_TRUE(TestNumeric::testEps());
}

TEST(TestNumeric, WeakEps)
{
	EXPECT_TRUE(TestNumeric::testWeakEps());
}

TEST(TestNumeric, IsEqualDynamic)
{
	EXPECT_TRUE(TestNumeric::testIsEqualDynamic());
}

TEST(TestNumeric, Round)
{
	EXPECT_TRUE(TestNumeric::testRound());
}


TEST(TestNumeric, AngleConversion_float)
{
	EXPECT_TRUE((TestNumeric::testAngleConversion<float>(GTEST_TEST_DURATION)));
}

TEST(TestNumeric, AngleConversion_double)
{
	EXPECT_TRUE((TestNumeric::testAngleConversion<double>(GTEST_TEST_DURATION)));
}


TEST(TestNumeric, AngleAdjustPositive)
{
	EXPECT_TRUE(TestNumeric::testAngleAdjustPositive(GTEST_TEST_DURATION));
}

TEST(TestNumeric, AngleAdjustNull)
{
	EXPECT_TRUE(TestNumeric::testAngleAdjustNull(GTEST_TEST_DURATION));
}

TEST(TestNumeric, AngleIsEqual)
{
	EXPECT_TRUE(TestNumeric::testAngleIsEqual(GTEST_TEST_DURATION));
}

TEST(TestNumeric, AngleIsBelowThreshold)
{
	EXPECT_TRUE(TestNumeric::testAngleIsBelowThreshold(GTEST_TEST_DURATION));
}

TEST(TestNumeric, GaussianDistribution1)
{
	EXPECT_TRUE(TestNumeric::testGaussianDistribution1(GTEST_TEST_DURATION));
}

TEST(TestNumeric, GaussianDistribution2)
{
	EXPECT_TRUE(TestNumeric::testGaussianDistribution2(GTEST_TEST_DURATION));
}

TEST(TestNumeric, GaussianDistribution3)
{
	EXPECT_TRUE(TestNumeric::testGaussianDistribution3(GTEST_TEST_DURATION));
}

TEST(TestNumeric, Abs)
{
	EXPECT_TRUE(TestNumeric::testAbs());
}

TEST(TestNumeric, SecureAbs)
{
	EXPECT_TRUE(TestNumeric::testSecureAbs());
}

TEST(TestNumeric, Floor)
{
	EXPECT_TRUE(TestNumeric::testFloor(GTEST_TEST_DURATION));
}

TEST(TestNumeric, Ceil)
{
	EXPECT_TRUE(TestNumeric::testCeil(GTEST_TEST_DURATION));
}

TEST(TestNumeric, Log2)
{
	EXPECT_TRUE(TestNumeric::testLog2(GTEST_TEST_DURATION));
}

TEST(TestNumeric, DotProduct32)
{
	EXPECT_TRUE(TestNumeric::testDotProduct<float>(GTEST_TEST_DURATION));
}

TEST(TestNumeric, DotProduct64)
{
	EXPECT_TRUE(TestNumeric::testDotProduct<double>(GTEST_TEST_DURATION));
}

TEST(TestNumeric, Sign)
{
	EXPECT_TRUE(TestNumeric::testSign(GTEST_TEST_DURATION));
}

TEST(TestNumeric, CopySign)
{
	EXPECT_TRUE(TestNumeric::testCopySign());
}

TEST(TestNumeric, InvertSign)
{
	EXPECT_TRUE(TestNumeric::testInvertSign());
}

TEST(TestNumeric, Factorial)
{
	EXPECT_TRUE(TestNumeric::testFactorial());
}


TEST(TestNumeric, IsNan_float)
{
	EXPECT_TRUE((TestNumeric::testIsNan<float>(GTEST_TEST_DURATION)));
}

TEST(TestNumeric, IsNan_double)
{
	EXPECT_TRUE((TestNumeric::testIsNan<double>(GTEST_TEST_DURATION)));
}

TEST(TestNumeric, IsNan_uint8)
{
	EXPECT_TRUE((TestNumeric::testIsNan<uint8_t>(GTEST_TEST_DURATION)));
}

TEST(TestNumeric, IsNan_int32)
{
	EXPECT_TRUE((TestNumeric::testIsNan<int32_t>(GTEST_TEST_DURATION)));
}


TEST(TestNumeric, IsInf_float)
{
	EXPECT_TRUE((TestNumeric::testIsInf<float>(GTEST_TEST_DURATION)));
}

TEST(TestNumeric, IsInf_double)
{
	EXPECT_TRUE((TestNumeric::testIsInf<double>(GTEST_TEST_DURATION)));
}

TEST(TestNumeric, IsInf_uint8)
{
	EXPECT_TRUE((TestNumeric::testIsInf<uint8_t>(GTEST_TEST_DURATION)));
}

TEST(TestNumeric, IsInf_int32)
{
	EXPECT_TRUE((TestNumeric::testIsInf<int32_t>(GTEST_TEST_DURATION)));
}


TEST(TestNumeric, Pow)
{
	EXPECT_TRUE(TestNumeric::testPow());
}

TEST(TestNumeric, IsInsideValueRange)
{
	EXPECT_TRUE(TestNumeric::testIsInsideValueRange(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestNumeric::testEps()
{
	Log::info() << "Eps test:";

	bool allSucceeded = true;

	if (NumericT<double>::eps() != 1e-12)
	{
		allSucceeded = false;
	}

	if (NumericT<float>::eps() != 1e-6f)
	{
		allSucceeded = false;
	}


	if (NumericT<signed char>::eps() != (signed char)(0))
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned char>::eps() != (unsigned char)(0))
	{
		allSucceeded = false;
	}

	if (NumericT<int8_t>::eps() != int8_t(0))
	{
		allSucceeded = false;
	}

	if (NumericT<uint8_t>::eps() != uint8_t(0))
	{
		allSucceeded = false;
	}


	if (NumericT<short>::eps() != short(0))
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned short>::eps() != (unsigned short)(0))
	{
		allSucceeded = false;
	}

	if (NumericT<int16_t>::eps() != int16_t(0))
	{
		allSucceeded = false;
	}

	if (NumericT<uint16_t>::eps() != uint16_t(0))
	{
		allSucceeded = false;
	}


	if (NumericT<int>::eps() != 0)
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned int>::eps() != 0u)
	{
		allSucceeded = false;
	}

	if (NumericT<int32_t>::eps() != int32_t(0))
	{
		allSucceeded = false;
	}

	if (NumericT<uint32_t>::eps() != uint32_t(0))
	{
		allSucceeded = false;
	}


	if (NumericT<long long>::eps() != 0ll)
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned long long>::eps() != 0ull)
	{
		allSucceeded = false;
	}

	if (NumericT<int64_t>::eps() != int64_t(0))
	{
		allSucceeded = false;
	}

	if (NumericT<uint64_t>::eps() != uint64_t(0))
	{
		allSucceeded = false;
	}


	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestNumeric::testWeakEps()
{
	Log::info() << "WeakEps test:";

	bool allSucceeded = true;

	if (NumericT<double>::weakEps() != 1e-6)
	{
		allSucceeded = false;
	}

	if (NumericT<float>::weakEps() != 1e-3f)
	{
		allSucceeded = false;
	}


	if (NumericT<signed char>::weakEps() != (signed char)(0))
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned char>::weakEps() != (unsigned char)(0))
	{
		allSucceeded = false;
	}

	if (NumericT<int8_t>::weakEps() != int8_t(0))
	{
		allSucceeded = false;
	}

	if (NumericT<uint8_t>::weakEps() != uint8_t(0))
	{
		allSucceeded = false;
	}


	if (NumericT<short>::weakEps() != short(0))
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned short>::weakEps() != (unsigned short)(0))
	{
		allSucceeded = false;
	}

	if (NumericT<int16_t>::weakEps() != int16_t(0))
	{
		allSucceeded = false;
	}

	if (NumericT<uint16_t>::weakEps() != uint16_t(0))
	{
		allSucceeded = false;
	}


	if (NumericT<int>::weakEps() != 0)
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned int>::weakEps() != 0u)
	{
		allSucceeded = false;
	}

	if (NumericT<int32_t>::weakEps() != int32_t(0))
	{
		allSucceeded = false;
	}

	if (NumericT<uint32_t>::weakEps() != uint32_t(0))
	{
		allSucceeded = false;
	}


	if (NumericT<long long>::weakEps() != 0ll)
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned long long>::weakEps() != 0ull)
	{
		allSucceeded = false;
	}

	if (NumericT<int64_t>::weakEps() != int64_t(0))
	{
		allSucceeded = false;
	}

	if (NumericT<uint64_t>::weakEps() != uint64_t(0))
	{
		allSucceeded = false;
	}


	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestNumeric::testIsEqualDynamic()
{
	Log::info() << "Dynamic isEqual test:";

	bool allSucceeded = true;

	if (std::is_same<Scalar, double>::value)
	{
		allSucceeded = Numeric::isEqual<-5>(Scalar(0.00001), Scalar(0.00001), Numeric::eps()) && allSucceeded;
		allSucceeded = Numeric::isEqual<-5>(Scalar(0.00001), Scalar(0.00001) + Numeric::eps() * Scalar(0.1), Numeric::eps()) && allSucceeded;

		allSucceeded = !Numeric::isEqual<-5>(Scalar(0.0001), Scalar(0.00015), Numeric::eps()) && allSucceeded;

		allSucceeded = Numeric::isEqual<-5>(Scalar(1), Scalar(1.01), Numeric::weakEps()) && allSucceeded;
		allSucceeded = Numeric::isEqual<-5>(Scalar(1), Scalar(2), Numeric::weakEps()) && allSucceeded;
		allSucceeded = !Numeric::isEqual<-5>(Scalar(1), Scalar(2.1), Numeric::weakEps()) && allSucceeded;

		allSucceeded = Numeric::isEqual<-5>(Scalar(100), Scalar(105), Numeric::weakEps()) && allSucceeded;
	}
	else
	{
		allSucceeded = Numeric::isEqual<-4>(Scalar(0.0001), Scalar(0.0001), Numeric::eps()) && allSucceeded;
		allSucceeded = Numeric::isEqual<-4>(Scalar(0.0001), Scalar(0.0001) + Numeric::eps() * Scalar(0.1), Numeric::eps()) && allSucceeded;

		allSucceeded = !Numeric::isEqual<-4>(Scalar(0.001), Scalar(0.0015), Numeric::eps()) && allSucceeded;

		allSucceeded = Numeric::isEqual<-4>(Scalar(1), Scalar(1.01), Numeric::weakEps()) && allSucceeded;
		allSucceeded = Numeric::isEqual<-4>(Scalar(1), Scalar(2), Numeric::weakEps()) && allSucceeded;
		allSucceeded = !Numeric::isEqual<-2>(Scalar(1), Scalar(2.1), Numeric::weakEps()) && allSucceeded;

		allSucceeded = Numeric::isEqual<-2>(Scalar(100), Scalar(105), Numeric::weakEps()) && allSucceeded;
	}

	for (unsigned int n = 0u; n < 500u; ++n)
	{
		allSucceeded = Numeric::isEqual<0>(Scalar(n), Scalar(n), Numeric::eps()) && allSucceeded;
		allSucceeded = !Numeric::isEqual<0>(Scalar(n), Scalar(n) - Scalar(1), Numeric::eps()) && allSucceeded;

		allSucceeded = Numeric::isEqual<-4>(Scalar(n), Scalar(n), Numeric::eps()) && allSucceeded;

		if (std::is_same<Scalar, double>::value)
			allSucceeded = !Numeric::isEqual<-4>(Scalar(n), Scalar(n) - Scalar(1), Numeric::eps()) && allSucceeded;
		else
			allSucceeded = !Numeric::isEqual<-2>(Scalar(n), Scalar(n) - Scalar(1), Numeric::eps()) && allSucceeded;

		allSucceeded = Numeric::isEqual<6>(Scalar(n), Scalar(n), Numeric::eps()) && allSucceeded;
		allSucceeded = !Numeric::isEqual<6>(Scalar(n), Scalar(n) - Scalar(1), Numeric::eps()) && allSucceeded;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestNumeric::testRound()
{
	Log::info() << "Round test:";

	bool allSucceeded = true;

	allSucceeded = Numeric::round32(Scalar(1.4)) == 1 && allSucceeded;
	allSucceeded = Numeric::round32(Scalar(1.5)) == 2 && allSucceeded;
	allSucceeded = Numeric::round32(Scalar(10.4)) == 10 && allSucceeded;
	allSucceeded = Numeric::round32(Scalar(10.6)) == 11 && allSucceeded;

	allSucceeded = Numeric::round32(Scalar(-1.4)) == -1 && allSucceeded;
	allSucceeded = Numeric::round32(Scalar(-1.5)) == -2 && allSucceeded;
	allSucceeded = Numeric::round32(Scalar(-10.4)) == -10 && allSucceeded;
	allSucceeded = Numeric::round32(Scalar(-10.6)) == -11 && allSucceeded;

	allSucceeded = Numeric::round64(Scalar(1.4)) == (long long)(1) && allSucceeded;
	allSucceeded = Numeric::round64(Scalar(1.5)) == (long long)(2) && allSucceeded;
	allSucceeded = Numeric::round64(Scalar(10.4)) == (long long)(10) && allSucceeded;
	allSucceeded = Numeric::round64(Scalar(10.6)) == (long long)(11) && allSucceeded;

	allSucceeded = Numeric::round64(Scalar(-1.4)) == (long long)(-1) && allSucceeded;
	allSucceeded = Numeric::round64(Scalar(-1.5)) == (long long)(-2) && allSucceeded;
	allSucceeded = Numeric::round64(Scalar(-10.4)) == (long long)(-10) && allSucceeded;
	allSucceeded = Numeric::round64(Scalar(-10.6)) == (long long)(-11) && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestNumeric::testAngleConversion(const double testDuration)
{
	Log::info() << "Angle conversion test for '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		{
			const Scalar deg = Random::scalar(-1000, 1000);
			const Scalar rad = Numeric::deg2rad(deg);

			const Scalar radTest = deg * Numeric::pi() / Scalar(180);

			if (!Numeric::isEqual(rad, radTest, Numeric::weakEps()))
			{
				allSucceeded = false;
			}
		}

		{
			const Scalar rad = Random::scalar(-20, 20);
			const Scalar deg = Numeric::rad2deg(rad);

			const Scalar degTest = rad * Scalar(180) / Numeric::pi();

			if (!Numeric::isEqual(deg, degTest, Numeric::weakEps()))
			{
				allSucceeded = false;
			}
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	if (allSucceeded)
	{
		Log::info() << "Validation: Succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestNumeric::testAngleAdjustPositive(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Angle adjust positive test:";

	const unsigned int constIterations = 100000u;
	unsigned long long validIterations = 0ull;
	unsigned long long iterations = 0ull;

	// for 32 bit float values we need to weaken the epsilon by one magnitude
	const Scalar epsilon = std::is_same<Scalar, float>::value ? Numeric::eps() * Scalar(10) : Numeric::eps();

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0; n < constIterations; ++n)
		{
			++iterations;
			const Scalar value = Random::scalar(0, Numeric::pi2() - Numeric::eps());

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(value), value, epsilon))
				continue;

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(value + Numeric::pi2() * 5), value, epsilon))
				continue;

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(value + Numeric::pi2() * 6), value, epsilon))
				continue;

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(value - Numeric::pi2() * 5), value, epsilon))
				continue;

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(value - Numeric::pi2() * 6), value, epsilon))
				continue;

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(Numeric::pi2()), 0, epsilon))
				continue;

			++validIterations;
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestNumeric::testAngleAdjustNull(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Angle adjust null test:";

	const unsigned int constIterations = 100000u;
	unsigned long long validIterations = 0ull;
	unsigned long long iterations = 0ull;

	// for 32 bit float values we need to weaken the epsilon by one magnitude
	const Scalar epsilon = std::is_same<Scalar, float>::value ? Numeric::eps() * Scalar(10) : Numeric::eps();

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0; n < constIterations; ++n)
		{
			++iterations;
			const Scalar value = Random::scalar(-Numeric::pi() + Numeric::eps(), Numeric::pi());

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(value), value, epsilon))
				continue;

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(value + Numeric::pi2() * 5), value, epsilon))
				continue;

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(value + Numeric::pi2() * 6), value, epsilon))
				continue;

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(value - Numeric::pi2() * 5), value, epsilon))
				continue;

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(value - Numeric::pi2() * 6), value, epsilon))
				continue;

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(Numeric::pi2()), 0, epsilon))
				continue;

			++validIterations;
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestNumeric::testAngleIsEqual(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Angle is equal:";

	const unsigned int constIterations = 100000u;
	unsigned long long validIterations = 0ull;
	unsigned long long iterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			++iterations;
			const Scalar value = Random::scalar(0, Numeric::pi2());

			if (!Numeric::angleIsWeakEqual(value, value + Numeric::pi2() * Scalar(Random::random(-100, 100))))
				continue;

			if (Numeric::angleIsWeakEqual(value + Random::scalar(Scalar(0.1), 1), value + Numeric::pi2() * Scalar(Random::random(-100, 100))))
				continue;

			if (!Numeric::angleIsWeakEqual(value, value - Numeric::pi2()))
				continue;

			if (Numeric::angleIsWeakEqual(value - Random::scalar(Scalar(0.1), 1), Numeric::pi2() - value))
				continue;

			if (!Numeric::angleIsWeakEqual(value + Numeric::pi2() * Scalar(Random::random(-100, 100)), value - Numeric::pi2() + Numeric::pi2() * Scalar(Random::random(-100, 100))))
				continue;

			if (Numeric::angleIsWeakEqual(value + Random::scalar(Scalar(0.1), 1) + Numeric::pi2() * Scalar(Random::random(-100, 100)), Numeric::pi2() - value + Numeric::pi2() * Scalar(Random::random(-100, 100))))
				continue;

			if (!Numeric::angleIsWeakEqual(Numeric::eps(), Numeric::pi2() - Numeric::eps()))
				continue;

			++validIterations;
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.99;
}

bool TestNumeric::testAngleIsBelowThreshold(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Angle is below threshold:";

	const unsigned int constIterations = 100000u;
	unsigned long long validIterations = 0ull;
	unsigned long long iterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			++iterations;
			Scalar value = Random::scalar(0, Numeric::pi2());
			Scalar offset = Random::scalar(-Numeric::deg2rad(20 - Numeric::eps()), Numeric::deg2rad(Scalar(20 - Numeric::eps())));

			if (!Numeric::angleIsBelowThreshold(value, value + offset, Numeric::deg2rad(20)))
				continue;

			if (!Numeric::angleIsBelowThreshold(value + Numeric::pi2() * Scalar(Random::random(-100, 100)), value + offset + Numeric::pi2() * Scalar(Random::random(-100, 100)), Numeric::deg2rad(20)))
				continue;

			offset = Numeric::deg2rad(20) + Random::scalar(Numeric::eps(), Numeric::deg2rad(5));

			if (Numeric::angleIsBelowThreshold(value, value + offset, Numeric::deg2rad(20)))
				continue;

			if (Numeric::angleIsBelowThreshold(value + Numeric::pi2() * Scalar(Random::random(-100, 100)), value + offset + Numeric::pi2() * Scalar(Random::random(-100, 100)), Numeric::deg2rad(20)))
				continue;

			offset = -Numeric::deg2rad(20) - Random::scalar(Numeric::eps(), Numeric::deg2rad(5));

			if (Numeric::angleIsBelowThreshold(value, value + offset, Numeric::deg2rad(20)))
				continue;

			if (Numeric::angleIsBelowThreshold(value + Numeric::pi2() * Scalar(Random::random(-100, 100)), value + offset + Numeric::pi2() * Scalar(Random::random(-100, 100)), Numeric::deg2rad(20)))
				continue;

			++validIterations;
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestNumeric::testGaussianDistribution1(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "1D Gaussian distribution test:";

	const unsigned int constIterations = 100000u;
	unsigned long long validIterations = 0ull;
	unsigned long long iterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			const Scalar sigma = Random::scalar(Scalar(0.01), Scalar(10));
			const Scalar x = Random::scalar(Scalar(-sigma * 5), Scalar(sigma * 5));

			const Scalar value = Numeric::gaussianDistribution(x, sigma);
			const Scalar testValue = 1 / (sigma * Numeric::sqrt(2 * Numeric::pi())) * Numeric::pow(Numeric::e(), - Scalar(0.5) * (x * x) / (sigma * sigma));

			if (Numeric::isEqual(value, testValue))
				validIterations++;

			++iterations;
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestNumeric::testGaussianDistribution2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "2D Gaussian distribution test:";

	const unsigned int constIterations = 100000u;
	unsigned long long validIterations = 0ull;
	unsigned long long iterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (size_t n = 0; n < constIterations; ++n)
		{
			const Scalar sigmaX = Random::scalar(Scalar(0.01), Scalar(10));
			const Scalar sigmaY = Random::scalar(Scalar(0.01), Scalar(10));

			const Scalar x = Random::scalar(Scalar(-sigmaX * 5), Scalar(sigmaX * 5));
			const Scalar y = Random::scalar(Scalar(-sigmaY * 5), Scalar(sigmaY * 5));

			const Scalar value = Numeric::gaussianDistribution2(x, y, sigmaX, sigmaY);
			const Scalar testValue = Numeric::gaussianDistribution(x, sigmaX) * Numeric::gaussianDistribution(y, sigmaY);

			if (Numeric::isEqual(value, testValue))
				validIterations++;

			++iterations;
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestNumeric::testGaussianDistribution3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "3D Gaussian distribution test:";

	const unsigned int constIterations = 100000u;
	unsigned long long validIterations = 0ull;
	unsigned long long iterations = 0ull;

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			const Scalar sigmaX = Random::scalar(Scalar(0.01), Scalar(10));
			const Scalar sigmaY = Random::scalar(Scalar(0.01), Scalar(10));
			const Scalar sigmaZ = Random::scalar(Scalar(0.01), Scalar(10));

			const Scalar x = Random::scalar(Scalar(-sigmaX * 5), Scalar(sigmaX * 5));
			const Scalar y = Random::scalar(Scalar(-sigmaY * 5), Scalar(sigmaY * 5));
			const Scalar z = Random::scalar(Scalar(-sigmaZ * 5), Scalar(sigmaZ * 5));

			const Scalar value = Numeric::gaussianDistribution3(x, y, z, sigmaX, sigmaY, sigmaZ);
			const Scalar testValue = Numeric::gaussianDistribution(x, sigmaX) * Numeric::gaussianDistribution(y, sigmaY) * Numeric::gaussianDistribution(z, sigmaZ);

			if (Numeric::isEqual(value, testValue))
				++validIterations;

			++iterations;
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	ocean_assert(iterations != 0ull);
	const double percent = double(validIterations) / double(iterations);

	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

	return percent >= 0.95;
}

bool TestNumeric::testAbs()
{
	Log::info() << "Abs test:";

	bool allSucceeded = true;

	if (NumericT<signed char>::abs(5) != (signed char)(5) || NumericT<signed char>::abs(-5) != (signed char)(5))
		allSucceeded = false;

	if (NumericT<unsigned char>::abs(5) != (unsigned char)5)
		allSucceeded = false;

	if (NumericT<short>::abs(5) != short(5) || NumericT<short>::abs(-5) != short(5))
		allSucceeded = false;

	if (NumericT<unsigned short>::abs(5) != (unsigned short)5)
		allSucceeded = false;

	if (NumericT<int>::abs(5) != int(5) || NumericT<int>::abs(-5) != int(5))
		allSucceeded = false;

	if (NumericT<unsigned int>::abs(5) != (unsigned int)5)
		allSucceeded = false;

	if (NumericT<long long>::abs(5) != (long long)5 || NumericT<long long>::abs(-5) != (long long)5)
		allSucceeded = false;

	if (NumericT<unsigned long long>::abs(5) != (unsigned long long)5)
		allSucceeded = false;

	if (NumericT<float>::abs(5.0f) != 5.0f || NumericT<float>::abs(-5.0f) != 5.0f)
		allSucceeded = false;

	if (NumericT<double>::abs(5.0) != 5.0 || NumericT<double>::abs(-5.0) != 5.0)
		allSucceeded = false;

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestNumeric::testSecureAbs()
{
	Log::info() << "Secure abs test:";

	bool allSucceeded = true;

	// char: [-128, 127]
	{
		const unsigned char a0 = NumericT<signed char>::secureAbs((signed char)(127));
		const int a1 = NumericT<signed char>::secureAbs((signed char)(127));
		const unsigned int a2 = NumericT<signed char>::secureAbs((signed char)(127));

		if (a0 != 127u || a1 != 127 || a2 != 127u)
		{
			allSucceeded = false;
		}
	}
	{
		const unsigned char a0 = NumericT<signed char>::secureAbs((signed char)(-128));
		const int a1 = NumericT<signed char>::secureAbs((signed char)(-128));
		const unsigned int a2 = NumericT<signed char>::secureAbs((signed char)(-128));

		if (a0 != 128u || a1 != 128 || a2 != 128u)
		{
			allSucceeded = false;
		}
	}
	{
		const unsigned char a0 = NumericT<signed char>::secureAbs((signed char)(uint8_t(128)));
		const int a1 = NumericT<signed char>::secureAbs((signed char)(uint8_t(uint8_t(128))));
		const unsigned int a2 = NumericT<signed char>::secureAbs((signed char)(uint8_t(128)));

		if (a0 != 128u || a1 != 128 || a2 != 128u)
		{
			allSucceeded = false;
		}
	}

	// short: [-32768, 32767]
	{
		const unsigned short a0 = NumericT<short>::secureAbs(short(32767));
		const int a1 = NumericT<short>::secureAbs(short(32767));
		const unsigned int a2 = NumericT<short>::secureAbs(short(32767));

		if (a0 != 32767u || a1 != 32767 || a2 != 32767u)
		{
			allSucceeded = false;
		}
	}
	{
		const unsigned short a0 = NumericT<short>::secureAbs(short(-32768));
		const int a1 = NumericT<short>::secureAbs(short(-32768));
		const unsigned int a2 = NumericT<short>::secureAbs(short(-32768));

		if (a0 != 32768u || a1 != 32768 || a2 != 32768u)
		{
			allSucceeded = false;
		}
	}
	{
		const unsigned short a0 = NumericT<short>::secureAbs(short(uint16_t(32768)));
		const int a1 = NumericT<short>::secureAbs(short(uint16_t(32768)));
		const unsigned int a2 = NumericT<short>::secureAbs(short(uint16_t(32768)));

		if (a0 != 32768u || a1 != 32768 || a2 != 32768u)
		{
			allSucceeded = false;
		}
	}

	// int: [-2147483648, 2147483647]
	{
		const unsigned int a0 = NumericT<int>::secureAbs(int(2147483647));
		const long long a1 = NumericT<int>::secureAbs(int(2147483647));
		const unsigned long long a2 = NumericT<int>::secureAbs(int(2147483647));

		if (a0 != 2147483647u || a1 != 2147483647 || a2 != 2147483647u)
		{
			allSucceeded = false;
		}
	}

	{
		// as -2147483648 may be composed of an unaray (-) operator and a signed integer value we create -2147483648 by subtracting -1 from -2147483647
		const unsigned int a0 = NumericT<int>::secureAbs(-2147483647 - 1);
		const long long a1 = NumericT<int>::secureAbs(-2147483647 - 1);
		const unsigned long long a2 = NumericT<int>::secureAbs(-2147483647 - 1);

		if (a0 != 2147483648u || a1 != 2147483648ll || a2 != 2147483648ull)
		{
			allSucceeded = false;
		}
	}
	{
		const unsigned int a0 = NumericT<int>::secureAbs(static_cast<int>(int64_t(2147483648ll)));
		const long long a1 = NumericT<int>::secureAbs(static_cast<int>(int64_t(2147483648ll)));
		const unsigned long long a2 = NumericT<int>::secureAbs(static_cast<int>(int64_t(2147483648ll)));

		if (a0 != 2147483648u || a1 != 2147483648ll || a2 != 2147483648ull)
		{
			allSucceeded = false;
		}
	}

	// long long: [-9223372036854775808, 9223372036854775807]
	{
		const unsigned long long a0 = NumericT<long long>::secureAbs(9223372036854775807ll);

		if (a0 != 9223372036854775807ull)
		{
			allSucceeded = false;
		}
	}
	{
		// as -9223372036854775808 may be composed of an unaray (-) operator and a signed long long value we create -9223372036854775808 by subtracting -1 from -9223372036854775807
		const unsigned long long a0 = NumericT<long long>::secureAbs(-9223372036854775807ll - 1ll);

		if (a0 != 9223372036854775808ull)
		{
			allSucceeded = false;
		}
	}

	if (NumericT<signed char>::secureAbs(5) != (signed char)(5) || NumericT<signed char>::secureAbs(-5) != (signed char)(5))
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned char>::secureAbs(5) != (unsigned char)5)
	{
		allSucceeded = false;
	}

	if (NumericT<short>::secureAbs(5) != short(5) || NumericT<short>::secureAbs(-5) != short(5))
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned short>::secureAbs(5) != (unsigned short)5)
	{
		allSucceeded = false;
	}

	if (NumericT<int>::secureAbs(5) != int(5) || NumericT<int>::secureAbs(-5) != int(5))
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned int>::secureAbs(5) != (unsigned int)5)
	{
		allSucceeded = false;
	}

	if (NumericT<long long>::secureAbs(5) != (long long)5 || NumericT<long long>::secureAbs(-5) != (long long)5)
	{
		allSucceeded = false;
	}

	if (NumericT<unsigned long long>::secureAbs(5) != (unsigned long long)5)
	{
		allSucceeded = false;
	}

	if (NumericT<float>::secureAbs(5.0f) != 5.0f || NumericT<float>::secureAbs(-5.0f) != 5.0f)
	{
		allSucceeded = false;
	}

	if (NumericT<double>::secureAbs(5.0) != 5.0 || NumericT<double>::secureAbs(-5.0) != 5.0)
	{
		allSucceeded = false;
	}

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

bool TestNumeric::testFloor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test floor function:";

	HighPerformanceStatistic performanceFloor;
	HighPerformanceStatistic performanceCast;

	static const size_t size = 1000000;

	Scalars values(size);
	std::vector<int> results(size);

	const Timestamp startTimestamp(true);

	do
	{
		for (size_t n = 0; n < size; ++n)
			values[n] = Random::scalar(-1000, 1000);

		{
			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceFloor);

			for (size_t n = 0; n < size; ++n)
				results[n] = int(Numeric::floor(values[n]));
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceCast);

			for (size_t n = 0; n < size; ++n)
				results[n] = int(values[n]);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (results[0] == 0) // we make this dummy check to receive a serious result
		Log::info() << "Performance: " << performanceFloor.averageMseconds() * 1000.0 / double(size) << "mys";
	else
		Log::info() << "Performance: " << performanceFloor.averageMseconds() * 1000.0 / double(size) << "mys";

	Log::info() << "Performance integer casting: " << performanceCast.averageMseconds() * 1000.0 / double(size) << "mys";
	Log::info() << "Performance factor: " << String::toAString(performanceFloor.averageMseconds() / performanceCast.averageMseconds(), 2u) << " (times slower)";

	return true;
}

bool TestNumeric::testCeil(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test ceil function:";

	HighPerformanceStatistic performanceCeil;
	HighPerformanceStatistic performanceCast;

	static const size_t size = 1000000;

	Scalars values(size);
	std::vector<int> results(size);

	const Timestamp startTimestamp(true);

	do
	{
		for (size_t n = 0; n < size; ++n)
			values[n] = Random::scalar(-1000, 1000);

		{
			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceCeil);

			for (size_t n = 0; n < size; ++n)
				results[n] = int(Numeric::ceil(values[n]));
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceCast);

			for (size_t n = 0; n < size; ++n)
				results[n] = int(values[n]);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (results[0] == 0) // we make this dummy check to receive a serious result
		Log::info() << "Performance: " << performanceCeil.averageMseconds() * 1000.0 / double(size) << "mys";
	else
		Log::info() << "Performance: " << performanceCeil.averageMseconds() * 1000.0 / double(size) << "mys";

	Log::info() << "Performance integer casting: " << performanceCast.averageMseconds() * 1000.0 / double(size) << "mys";
	Log::info() << "Performance factor: " << String::toAString(performanceCeil.averageMseconds() / performanceCast.averageMseconds(), 2u) << " (times slower)";

	return true;
}

bool TestNumeric::testLog2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test log2 function:";

	unsigned long long iterations = 0ull;
	unsigned long long validIterations = 0ull;

	static const size_t size = 10000;

	const Timestamp startTimestamp(true);

	do
	{
		for (size_t n = 0; n < size; ++n)
		{
			const Scalar value = Random::scalar(Numeric::weakEps(), Scalar(1000));
			const Scalar valueLog2 = Numeric::log2(value);

			const Scalar test = Numeric::pow(Scalar(2), valueLog2);

			if (Numeric::isWeakEqual(value, test))
				validIterations++;

			iterations++;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	const double percent = double(validIterations) / double(iterations);
	Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "%";

	return percent >= 0.99;
}

template <typename T>
bool TestNumeric::testDotProduct(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing dot product function with " << sizeof(T) * 8 << "bit precision:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDotProduct<T, 10000>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDotProduct<T, 100000>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDotProduct<T, 1000000>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDotProduct<T, 10000000>(testDuration) && allSucceeded;

	return allSucceeded;
}

bool TestNumeric::testSign(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test sign function:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		if (NumericT<signed char>::sign(0) != 0)
			allSucceeded = false;

		if (NumericT<signed char>::sign((signed char)(RandomI::random(1, 127))) != 1)
			allSucceeded = false;

		if (NumericT<signed char>::sign((signed char)(RandomI::random(-127, -1))) != -1)
			allSucceeded = false;


		if (NumericT<short>::sign(0) != 0)
			allSucceeded = false;

		if (NumericT<short>::sign(short(RandomI::random(1, 1000))) != 1)
			allSucceeded = false;

		if (NumericT<short>::sign(short(RandomI::random(-1000, -1))) != -1)
			allSucceeded = false;


		if (NumericT<int>::sign(0) != 0)
			allSucceeded = false;

		if (NumericT<int>::sign(RandomI::random(1, 1000)) != 1)
			allSucceeded = false;

		if (NumericT<int>::sign(RandomI::random(-1000, -1)) != -1)
			allSucceeded = false;


		if (NumericT<long long>::sign(0ll) != 0ll)
			allSucceeded = false;

		if (NumericT<long long>::sign(RandomI::random(1, 1000)) != 1ll)
			allSucceeded = false;

		if (NumericT<long long>::sign(RandomI::random(-1000, -1)) != -1ll)
			allSucceeded = false;


		if (NumericT<float>::sign(0.0f) != 0.0f)
			allSucceeded = false;

		if (NumericT<float>::sign(RandomF::scalar(0.01f, 100.0f)) != 1.0f)
			allSucceeded = false;

		if (NumericT<float>::sign(RandomF::scalar(-100.0f, -0.01f)) != -1.0f)
			allSucceeded = false;


		if (NumericT<double>::sign(0.0) != 0.0)
			allSucceeded = false;

		if (NumericT<double>::sign(RandomD::scalar(0.01, 100.0)) != 1.0)
			allSucceeded = false;

		if (NumericT<double>::sign(RandomD::scalar(-100.0, -0.01f)) != -1.0)
			allSucceeded = false;
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

bool TestNumeric::testCopySign()
{
	Log::info() << "Test copySign function:";

	bool allSucceeded = true;

	if (NumericT<int>::copySign(-5, 1) != 5)
		allSucceeded = false;

	if (NumericT<int>::copySign(-5, -1) != -5)
		allSucceeded = false;

	if (NumericT<int>::copySign(5000, 4) != 5000)
		allSucceeded = false;

	if (NumericT<int>::copySign(5000, -4) != -5000)
		allSucceeded = false;


	if (NumericT<float>::copySign(-5.123f, 1.555f) != 5.123f)
		allSucceeded = false;

	if (NumericT<float>::copySign(-5.123f, -1.678f) != -5.123f)
		allSucceeded = false;

	if (NumericT<float>::copySign(5000.456f, 4.321f) != 5000.456f)
		allSucceeded = false;

	if (NumericT<float>::copySign(5000.456f, -4.753f) != -5000.456f)
		allSucceeded = false;


	if (NumericT<double>::copySign(-5.123, 1.444) != 5.123)
		allSucceeded = false;

	if (NumericT<double>::copySign(-5.123, -1.489) != -5.123)
		allSucceeded = false;

	if (NumericT<double>::copySign(5000.456, 4.258) != 5000.456)
		allSucceeded = false;

	if (NumericT<double>::copySign(5000.456, -4.654) != -5000.456)
		allSucceeded = false;


	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestNumeric::testInvertSign()
{
	Log::info() << "Test invertSign function:";

	bool allSucceeded = true;

	if (NumericT<int>::invertSign(-5, 1) != -5)
		allSucceeded = false;

	if (NumericT<int>::invertSign(-5, -1) != 5)
		allSucceeded = false;

	if (NumericT<int>::invertSign(5000, 4) != -5000)
		allSucceeded = false;

	if (NumericT<int>::invertSign(5000, -4) != 5000)
		allSucceeded = false;


	if (NumericT<float>::invertSign(-5.123f, 1.555f) != -5.123f)
		allSucceeded = false;

	if (NumericT<float>::invertSign(-5.123f, -1.678f) != 5.123f)
		allSucceeded = false;

	if (NumericT<float>::invertSign(5000.456f, 4.321f) != -5000.456f)
		allSucceeded = false;

	if (NumericT<float>::invertSign(5000.456f, -4.753f) != 5000.456f)
		allSucceeded = false;


	if (NumericT<double>::invertSign(-5.123, 1.444) != -5.123)
		allSucceeded = false;

	if (NumericT<double>::invertSign(-5.123, -1.489) != 5.123)
		allSucceeded = false;

	if (NumericT<double>::invertSign(5000.456, 4.258) != -5000.456)
		allSucceeded = false;

	if (NumericT<double>::invertSign(5000.456, -4.654) != 5000.456)
		allSucceeded = false;


	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestNumeric::testFactorial()
{
	Log::info() << "Test factorial function:";

	bool allSucceeded = true;

	for (unsigned int n = 0u; n <= 12u; ++n)
	{
		const unsigned int factorial = NumericT<unsigned int>::factorial(n); // n!

		if (n == 0u)
		{
			if (factorial != 1u)
			{
				allSucceeded = false;
			}
		}
		else
		{
			// let's test the remaining values by simple multiplication

			double value = 1.0;

			for (unsigned int i = 1u; i <= n; ++i)
			{
				value *= double(i);
			}

			if (double(factorial) != value)
			{
				allSucceeded = false;
			}
		}
	}

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

template <typename T>
bool TestNumeric::testIsNan(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test isNan function for '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	if (NumericT<T>::isNan(T(0)) || NumericT<T>::isNan(T(1)) || NumericT<T>::isNan(T(-1)))
	{
		allSucceeded = false;
	}

	if constexpr (std::is_floating_point<T>::value)
	{
		if (!NumericT<T>::isNan(NumericT<T>::nan()))
		{
			allSucceeded = false;
		}
	}

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			{
				const T value = T(RandomT<T>::scalar(randomGenerator, T(-100000), T(100000)));

				if (NumericT<T>::isNan(value))
				{
					allSucceeded = false;
				}
			}

			if constexpr (std::is_same<T, float>::value)
			{
				constexpr unsigned int exponentBits = 8u;
				constexpr unsigned int mantissaBits = 23u;

				static_assert(exponentBits + mantissaBits + 1u == sizeof(float) * 8, "Invalid data type!");

				const uint32_t intValue = RandomI::random32(randomGenerator);

				bool allExponentBitsAreOne = true;
				bool oneMantissaBitIsOne = false; // at least one bit is one

				for (unsigned int n = 0u; n < exponentBits; ++n)
				{
					const uint32_t checkValue = 1u << (mantissaBits + n);

					if ((intValue & checkValue) != checkValue)
					{
						allExponentBitsAreOne = false;
						break;
					}
				}

				if (allExponentBitsAreOne)
				{
					for (unsigned int n = 0u; n < mantissaBits; ++n)
					{
						const uint32_t checkValue = 1u << n;

						if ((intValue & checkValue) == checkValue)
						{
							oneMantissaBitIsOne = true;
							break;
						}
					}
				}

				const bool valueIsNan = allExponentBitsAreOne && oneMantissaBitIsOne;

				float floatValue;
				memcpy(&floatValue, &intValue, sizeof(float));

				if (valueIsNan != NumericF::isNan(floatValue))
				{
					allSucceeded = false;
				}

#ifdef OCEAN_DEBUG
				// in a not-optimized build, we can use std's isnan() function as well
				if (NumericF::isNan(floatValue) != std::isnan(floatValue))
				{
					allSucceeded = false;
				}
#endif
			}
			else
			{
				ocean_assert((std::is_same<T, double>::value));

				constexpr unsigned int exponentBits = 11u;
				constexpr unsigned int mantissaBits = 52u;

				static_assert(exponentBits + mantissaBits + 1u == sizeof(double) * 8, "Invalid data type!");

				const uint64_t intValue = RandomI::random64(randomGenerator);

				bool allExponentBitsAreOne = true;
				bool oneMantissaBitIsOne = false; // at least one bit is one

				for (unsigned int n = 0u; n < exponentBits; ++n)
				{
					const uint64_t checkValue = 1ull << uint64_t(mantissaBits + n);

					if ((intValue & checkValue) != checkValue)
					{
						allExponentBitsAreOne = false;
						break;
					}
				}

				if (allExponentBitsAreOne)
				{
					for (unsigned int n = 0u; n < mantissaBits; ++n)
					{
						const uint32_t checkValue = 1ull << uint64_t(n);

						if ((intValue & checkValue) == checkValue)
						{
							oneMantissaBitIsOne = true;
							break;
						}
					}
				}

				const bool valueIsNan = allExponentBitsAreOne && oneMantissaBitIsOne;

				double doubleValue;
				memcpy(&doubleValue, &intValue, sizeof(double));

				if (valueIsNan != NumericD::isNan(doubleValue))
				{
					allSucceeded = false;
				}

#ifdef OCEAN_DEBUG
				// in a not-optimized build, we can use std's isnan() function as well
				if (NumericD::isNan(doubleValue) != std::isnan(doubleValue))
				{
					allSucceeded = false;
				}
#endif
			}
		}
		else
		{
			// any value is not a NaN

			const T value = T(RandomI::random64(randomGenerator));

			if (NumericT<T>::isNan(value))
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

template <typename T>
bool TestNumeric::testIsInf(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test isInf function for '" << TypeNamer::name<T>() << "':";

	bool allSucceeded = true;

	if (NumericT<T>::isInf(T(0)) || NumericT<T>::isInf(T(1)) || NumericT<T>::isInf(T(-1)))
	{
		allSucceeded = false;
	}

	if constexpr (std::is_floating_point<T>::value)
	{
		if (!NumericT<T>::isInf(std::numeric_limits<T>::infinity()))
		{
			allSucceeded = false;
		}

		if (!NumericT<T>::isInf(NumericT<T>::inf()))
		{
			allSucceeded = false;
		}

		if (!NumericT<T>::isInf(-std::numeric_limits<T>::infinity()))
		{
			allSucceeded = false;
		}

		if (!NumericT<T>::isInf(-NumericT<T>::inf()))
		{
			allSucceeded = false;
		}

		const T stdValue = std::numeric_limits<T>::infinity();
		const T numericValue = NumericT<T>::inf();

		if (memcmp(&stdValue, &numericValue, sizeof(T)) != 0)
		{
			allSucceeded = false;
		}
	}

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			{
				const T value = T(RandomT<T>::scalar(randomGenerator, T(-100000), T(100000)));

				if (NumericT<T>::isInf(value))
				{
					allSucceeded = false;
				}
			}

			if constexpr (std::is_same<T, float>::value)
			{
				constexpr unsigned int exponentBits = 8u;
				constexpr unsigned int mantissaBits = 23u;

				static_assert(exponentBits + mantissaBits + 1u == sizeof(float) * 8, "Invalid data type!");

				const uint32_t intValue = RandomI::random32(randomGenerator);

				bool allExponentBitsAreOne = true;
				bool oneMantissaBitIsOne = false; // at least one bit is one

				for (unsigned int n = 0u; n < exponentBits; ++n)
				{
					const uint32_t checkValue = 1u << (mantissaBits + n);

					if ((intValue & checkValue) != checkValue)
					{
						allExponentBitsAreOne = false;
						break;
					}
				}

				if (allExponentBitsAreOne)
				{
					for (unsigned int n = 0u; n < mantissaBits; ++n)
					{
						const uint32_t checkValue = 1u << n;

						if ((intValue & checkValue) == checkValue)
						{
							oneMantissaBitIsOne = true;
							break;
						}
					}
				}

				const bool valueIsInf = allExponentBitsAreOne && !oneMantissaBitIsOne;

				float floatValue;
				memcpy(&floatValue, &intValue, sizeof(float));

				if (valueIsInf != NumericF::isInf(floatValue))
				{
					allSucceeded = false;
				}

#ifdef OCEAN_DEBUG
				// in a not-optimized build, we can use std's isinf() function as well
				if (NumericF::isInf(floatValue) != std::isinf(floatValue))
				{
					allSucceeded = false;
				}
#endif
			}
			else
			{
				ocean_assert((std::is_same<T, double>::value));

				constexpr unsigned int exponentBits = 11u;
				constexpr unsigned int mantissaBits = 52u;

				static_assert(exponentBits + mantissaBits + 1u == sizeof(double) * 8, "Invalid data type!");

				const uint64_t intValue = RandomI::random64(randomGenerator);

				bool allExponentBitsAreOne = true;
				bool oneMantissaBitIsOne = false; // at least one bit is one

				for (unsigned int n = 0u; n < exponentBits; ++n)
				{
					const uint64_t checkValue = 1ull << uint64_t(mantissaBits + n);

					if ((intValue & checkValue) != checkValue)
					{
						allExponentBitsAreOne = false;
						break;
					}
				}

				if (allExponentBitsAreOne)
				{
					for (unsigned int n = 0u; n < mantissaBits; ++n)
					{
						const uint32_t checkValue = 1ull << uint64_t(n);

						if ((intValue & checkValue) == checkValue)
						{
							oneMantissaBitIsOne = true;
							break;
						}
					}
				}

				const bool valueIsInf = allExponentBitsAreOne && !oneMantissaBitIsOne;

				double doubleValue;
				memcpy(&doubleValue, &intValue, sizeof(double));

				if (valueIsInf != NumericD::isInf(doubleValue))
				{
					allSucceeded = false;
				}

#ifdef OCEAN_DEBUG
				// in a not-optimized build, we can use std's isinf() function as well
				if (NumericD::isInf(doubleValue) != std::isinf(doubleValue))
				{
					allSucceeded = false;
				}
#endif
			}
		}
		else
		{
			// any value is not a NaN

			const T value = T(RandomI::random64(randomGenerator));

			if (NumericT<T>::isInf(value))
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

bool TestNumeric::testPow()
{
	Log::info() << "Test pow function:";

	bool allSucceeded = true;

	if (Numeric::isNotEqual(Numeric::pow(8, 0), 1))
	{
		allSucceeded = false;
	}

	if (Numeric::isNotEqual(Numeric::pow(1, 1), 1))
	{
		allSucceeded = false;
	}

	if (Numeric::isNotEqual(Numeric::pow(Scalar(0.5), 2), Scalar(0.25)))
	{
		allSucceeded = false;
	}

	if (Numeric::isNotEqual(Numeric::pow(Scalar(-0.5), 2), Scalar(0.25)))
	{
		allSucceeded = false;
	}

	if (Numeric::isNotEqual(Numeric::pow(Scalar(-0.5), -2), Scalar(4)))
	{
		allSucceeded = false;
	}

	if (Numeric::isNotEqual(Numeric::integerPow(8, 0u), Scalar(1)))
	{
		allSucceeded = false;
	}

	if (Numeric::isNotEqual(Numeric::integerPow(1, 1u), Scalar(1)))
	{
		allSucceeded = false;
	}

	if (Numeric::isNotEqual(Numeric::integerPow(1, 2u), Scalar(1)))
	{
		allSucceeded = false;
	}

	if (Numeric::isNotEqual(Numeric::integerPow(Scalar(0.5), 1u), Scalar(0.5)))
	{
		allSucceeded = false;
	}

	if (Numeric::isNotEqual(Numeric::integerPow(Scalar(0.5), 2u), Scalar(0.25)))
	{
		allSucceeded = false;
	}

	if (Numeric::isNotEqual(Numeric::integerPow(Scalar(10), 4u), Scalar(1e4)))
	{
		allSucceeded = false;
	}

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

bool TestNumeric::testIsInsideValueRange(const double testDuration)
{
	Log::info() << "Test pow function:";

	bool allSucceeded = true;

	// testing special cases

	if (!NumericT<std::string>::isInsideValueRange(std::string("test")))
	{
		allSucceeded = false;
	}

	if (NumericT<std::string>::isInsideValueRange(false))
	{
		allSucceeded = false;
	}

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// identical data types

			if (!NumericT<bool>::isInsideValueRange(RandomI::random(randomGenerator, 1u) == 0u))
			{
				allSucceeded = false;
			}

			if (!NumericT<int8_t>::isInsideValueRange(int8_t(RandomI::random64(randomGenerator))))
			{
				allSucceeded = false;
			}
			if (!NumericT<uint8_t>::isInsideValueRange(uint8_t(RandomI::random64(randomGenerator))))
			{
				allSucceeded = false;
			}

			if (!NumericT<int16_t>::isInsideValueRange(int16_t(RandomI::random64(randomGenerator))))
			{
				allSucceeded = false;
			}
			if (!NumericT<uint16_t>::isInsideValueRange(uint16_t(RandomI::random64(randomGenerator))))
			{
				allSucceeded = false;
			}

			if (!NumericT<int32_t>::isInsideValueRange(int32_t(RandomI::random64(randomGenerator))))
			{
				allSucceeded = false;
			}
			if (!NumericT<uint32_t>::isInsideValueRange(uint32_t(RandomI::random64(randomGenerator))))
			{
				allSucceeded = false;
			}

			if (!NumericT<int64_t>::isInsideValueRange(int64_t(RandomI::random64(randomGenerator))))
			{
				allSucceeded = false;
			}
			if (!NumericT<uint64_t>::isInsideValueRange(uint64_t(RandomI::random64(randomGenerator))))
			{
				allSucceeded = false;
			}

			if (!NumericT<float>::isInsideValueRange(RandomF::scalar(randomGenerator, -1000.0f, 1000.0f)))
			{
				allSucceeded = false;
			}
			if (!NumericT<double>::isInsideValueRange(RandomD::scalar(randomGenerator, 1000000.0, 1000000.0)))
			{
				allSucceeded = false;
			}
		}

		{
			// float -> double, and double -> float

			if (!NumericT<float>::isInsideValueRange(RandomD::scalar(randomGenerator, 1000000.0, 1000000.0)))
			{
				allSucceeded = false;
			}

			if (!NumericT<double>::isInsideValueRange(RandomF::scalar(randomGenerator, -1000.0f, 1000.0f)))
			{
				allSucceeded = false;
			}
		}

		{
			// float -> uint8_t, uint8_t -> float

			if (NumericT<float>::isInsideValueRange(uint8_t(RandomI::random32(randomGenerator))))
			{
				allSucceeded = false;
			}

			if (NumericT<uint8_t>::isInsideValueRange(RandomF::scalar(randomGenerator, -1000.0f, 1000.0f)))
			{
				allSucceeded = false;
			}
		}

		{
			// double -> uint8_t, double -> float

			if (NumericT<double>::isInsideValueRange(uint8_t(RandomI::random32(randomGenerator))))
			{
				allSucceeded = false;
			}

			if (NumericT<uint8_t>::isInsideValueRange(RandomD::scalar(randomGenerator, -1000.0, 1000.0)))
			{
				allSucceeded = false;
			}
		}

		{
			// int8_t

			const int8_t value = int8_t(RandomI::random64(randomGenerator));

			if (!NumericT<int16_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}
			if (!NumericT<int32_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}
			if (!NumericT<int64_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}

			if (NumericT<uint8_t>::isInsideValueRange(value) != (value >= int8_t(0)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint16_t>::isInsideValueRange(value) != (value >= int8_t(0)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint32_t>::isInsideValueRange(value) != (value >= int8_t(0)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint64_t>::isInsideValueRange(value) != (value >= int8_t(0)))
			{
				allSucceeded = false;
			}

			if (!verifyInsideValueRangeWithCast<int8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int32_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint32_t>(value))
			{
				allSucceeded = false;
			}
		}

		{
			// int16_t

			const int16_t value = int16_t(RandomI::random64(randomGenerator));

			if (NumericT<int8_t>::isInsideValueRange(value) != (value >= int16_t(-128) && value <= int16_t(127)))
			{
				allSucceeded = false;
			}
			if (!NumericT<int32_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}
			if (!NumericT<int64_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}

			if (NumericT<uint8_t>::isInsideValueRange(value) != (value >= int16_t(0) && value <= int16_t(255)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint16_t>::isInsideValueRange(value) != (value >= int16_t(0)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint32_t>::isInsideValueRange(value) != (value >= int16_t(0)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint64_t>::isInsideValueRange(value) != (value >= int16_t(0)))
			{
				allSucceeded = false;
			}

			if (!verifyInsideValueRangeWithCast<int8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int32_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint32_t>(value))
			{
				allSucceeded = false;
			}
		}

		{
			// int32_t

			const int32_t value = int32_t(RandomI::random64(randomGenerator));

			if (NumericT<int8_t>::isInsideValueRange(value) != (value >= int32_t(-128) && value <= int32_t(127)))
			{
				allSucceeded = false;
			}
			if (NumericT<int16_t>::isInsideValueRange(value) != (value >= int32_t(-32768) && value <= int32_t(32767)))
			{
				allSucceeded = false;
			}
			if (!NumericT<int64_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}

			if (NumericT<uint8_t>::isInsideValueRange(value) != (value >= int32_t(0) && value <= int32_t(255)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint16_t>::isInsideValueRange(value) != (value >= int32_t(0) && value <= int32_t(65535)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint32_t>::isInsideValueRange(value) != (value >= int32_t(0)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint64_t>::isInsideValueRange(value) != (value >= int32_t(0)))
			{
				allSucceeded = false;
			}

			if (!verifyInsideValueRangeWithCast<int8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int32_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint32_t>(value))
			{
				allSucceeded = false;
			}
		}

		{
			// int64_t

			const int64_t value = int64_t(RandomI::random64(randomGenerator));

			if (NumericT<int8_t>::isInsideValueRange(value) != (value >= int64_t(-128) && value <= int64_t(127ll)))
			{
				allSucceeded = false;
			}
			if (NumericT<int16_t>::isInsideValueRange(value) != (value >= int64_t(-32768) && value <= int64_t(32767ll)))
			{
				allSucceeded = false;
			}
			if (NumericT<int32_t>::isInsideValueRange(value) != (value >= int64_t(-2147483648ll) && value <= int64_t(2147483647ll)))
			{
				allSucceeded = false;
			}


			if (NumericT<uint8_t>::isInsideValueRange(value) != (value >= int64_t(0) && value <= int64_t(255ll)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint16_t>::isInsideValueRange(value) != (value >= int64_t(0) && value <= int64_t(65535ll)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint32_t>::isInsideValueRange(value) != (value >= int64_t(0) && value <= int64_t(4294967295ll)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint64_t>::isInsideValueRange(value) != (value >= int64_t(0)))
			{
				allSucceeded = false;
			}
		}

		{
			// uint8_t

			const uint8_t value = uint8_t(RandomI::random64(randomGenerator));

			if (!NumericT<uint16_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}
			if (!NumericT<uint32_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}
			if (!NumericT<uint64_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}

			if (NumericT<int8_t>::isInsideValueRange(value) != (value <= uint8_t(127)))
			{
				allSucceeded = false;
			}
			if (!NumericT<int16_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}
			if (!NumericT<int32_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}
			if (!NumericT<int64_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}

			if (!verifyInsideValueRangeWithCast<int8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int32_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint32_t>(value))
			{
				allSucceeded = false;
			}
		}

		{
			// uint16_t

			const uint16_t value = uint16_t(RandomI::random64(randomGenerator));

			if (NumericT<uint8_t>::isInsideValueRange(value) != (value <= uint16_t(255)))
			{
				allSucceeded = false;
			}
			if (!NumericT<uint32_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}
			if (!NumericT<uint64_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}

			if (NumericT<int8_t>::isInsideValueRange(value) != (value <= uint16_t(127)))
			{
				allSucceeded = false;
			}
			if (NumericT<int16_t>::isInsideValueRange(value) != (value <= uint16_t(32767)))
			{
				allSucceeded = false;
			}
			if (!NumericT<int32_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}
			if (!NumericT<int64_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}

			if (!verifyInsideValueRangeWithCast<int8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int32_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint32_t>(value))
			{
				allSucceeded = false;
			}
		}

		{
			// uint32_t

			const uint32_t value = uint32_t(RandomI::random64(randomGenerator));

			if (NumericT<uint8_t>::isInsideValueRange(value) != (value <= uint32_t(255)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint16_t>::isInsideValueRange(value) != (value <= uint32_t(65535)))
			{
				allSucceeded = false;
			}
			if (!NumericT<uint64_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}

			if (NumericT<int8_t>::isInsideValueRange(value) != (value <= uint32_t(127)))
			{
				allSucceeded = false;
			}
			if (NumericT<int16_t>::isInsideValueRange(value) != (value <= uint32_t(32767)))
			{
				allSucceeded = false;
			}
			if (NumericT<int32_t>::isInsideValueRange(value) != (value <= uint32_t(2147483647)))
			{
				allSucceeded = false;
			}
			if (!NumericT<int64_t>::isInsideValueRange(value))
			{
				allSucceeded = false;
			}

			if (!verifyInsideValueRangeWithCast<int8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint8_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint16_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<int32_t>(value))
			{
				allSucceeded = false;
			}
			if (!verifyInsideValueRangeWithCast<uint32_t>(value))
			{
				allSucceeded = false;
			}
		}

		{
			// uint64_t

			const uint64_t value = uint64_t(RandomI::random64(randomGenerator));

			if (NumericT<uint8_t>::isInsideValueRange(value) != (value <= uint64_t(255ull)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint16_t>::isInsideValueRange(value) != (value <= uint64_t(65535ull)))
			{
				allSucceeded = false;
			}
			if (NumericT<uint32_t>::isInsideValueRange(value) != (value <= uint64_t(4294967295ull)))
			{
				allSucceeded = false;
			}

			if (NumericT<int8_t>::isInsideValueRange(value) != (value <= uint64_t(127ull)))
			{
				allSucceeded = false;
			}
			if (NumericT<int16_t>::isInsideValueRange(value) != (value <= uint64_t(32767ull)))
			{
				allSucceeded = false;
			}
			if (NumericT<int32_t>::isInsideValueRange(value) != (value <= uint64_t(2147483647ull)))
			{
				allSucceeded = false;
			}
			if (NumericT<int64_t>::isInsideValueRange(value) != (value <= uint64_t(9223372036854775807ull)))
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

template <typename T, size_t tSize>
bool TestNumeric::testDotProduct(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with vector size " << String::insertCharacter(String::toAString(tSize), ',', 3, false) << ":";

	bool allSucceeded = true;

	std::vector<T> vectorA(tSize);
	std::vector<T> vectorB(tSize);

	const T valueRange = std::is_same<T, double>::value ? T(1) : T(0.1);
	RandomGenerator randomGenerator;

	{
		// first we test the standard implementation not using any kind of explicit optimization

		HighPerformanceStatistic performanceStandard;
		const Timestamp startTimestamp(true);

		T dummyValue = T(0);

		do
		{
			for (unsigned int n = 0u; n < tSize; ++n)
			{
				vectorA[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				vectorB[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			{
				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceStandard);
				dummyValue += dotProduct(vectorA.data(), vectorB.data(), tSize);
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		if (dummyValue == T(1))
			Log::info() << "Standard performance as reference: " << String::toAString(performanceStandard.averageMseconds()) << "ms";
		else
			Log::info() << "Standard performance as reference: " << String::toAString(performanceStandard.averageMseconds()) << "ms";
	}

	{
		// now we test the default implementation of the framework using SIMD optimizations

		unsigned long long iterations = 0ull;
		unsigned long long validIterations = 0ull;

		HighPerformanceStatistic performance;
		const Timestamp startTimestamp(true);

		do
		{
			for (unsigned int n = 0u; n < tSize; ++n)
			{
				vectorA[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
				vectorB[n] = RandomT<T>::scalar(randomGenerator, -valueRange, valueRange);
			}

			T result;

			{
				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);
				result = NumericT<T>::dot(vectorA.data(), vectorB.data(), tSize);
			}

			const T test = dotProduct<T>(vectorA.data(), vectorB.data(), tSize);

			if (NumericT<T>::isEqual(result, test, NumericT<T>::eps() * (std::is_same<T, double>::value ? T(10) : T(1000))))
				validIterations++;

			iterations++;
		}
		while (startTimestamp + testDuration > Timestamp(true));

		ocean_assert(iterations != 0ull);
		const double percent = double(validIterations) / double(iterations);

		Log::info() << "Ocean performance individual: " << String::toAString(performance.averageMseconds()) << "ms";
		Log::info() << "Validation: " << String::toAString(percent * 100.0, 1u) << "% succeeded.";

		allSucceeded = percent >= 0.99 && allSucceeded;
	}

	return allSucceeded;
}

template <typename T, typename TValue>
bool TestNumeric::verifyInsideValueRangeWithCast(const TValue value)
{
	static_assert(sizeof(T) < 8 && sizeof(TValue) < 8, "Invalid data type!");

	const double dValue = double(value);
	const T tValue = T(value);
	const double dtValue = double(tValue);

	const bool isInsideValueRange = dValue == dtValue;

	if (NumericT<T>::isInsideValueRange(value) != isInsideValueRange)
	{
		return false;
	}

	return true;
}

}

}

}
