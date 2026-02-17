/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmath/TestNumeric.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/ValidationPrecision.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestMath
{

bool TestNumeric::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Numeric tests");

	Log::info() << " ";

	if (selector.shouldRun("eps"))
	{
		testResult = testEps();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("weakeps"))
	{
		testResult = testWeakEps();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("isequaldynamic"))
	{
		testResult = testIsEqualDynamic();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("round"))
	{
		testResult = testRound();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("angleconversion"))
	{
		testResult = testAngleConversion<float>(testDuration);
		Log::info() << " ";
		testResult = testAngleConversion<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("angleadjustpositive"))
	{
		testResult = testAngleAdjustPositive(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("angleadjustnull"))
	{
		testResult = testAngleAdjustNull(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("angleisequal"))
	{
		testResult = testAngleIsEqual(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("angleisbelowthreshold"))
	{
		testResult = testAngleIsBelowThreshold(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("gaussiandistribution1"))
	{
		testResult = testGaussianDistribution1(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("gaussiandistribution2"))
	{
		testResult = testGaussianDistribution2(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("gaussiandistribution3"))
	{
		testResult = testGaussianDistribution3(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("abs"))
	{
		testResult = testAbs();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("secureabs"))
	{
		testResult = testSecureAbs();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("floor"))
	{
		testResult = testFloor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("ceil"))
	{
		testResult = testCeil(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("log2"))
	{
		testResult = testLog2(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("dotproduct"))
	{
		testResult = testDotProduct<float>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";

		testResult = testDotProduct<double>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("sign"))
	{
		testResult = testSign(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("copysign"))
	{
		testResult = testCopySign();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("invertsign"))
	{
		testResult = testInvertSign();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("factorial"))
	{
		testResult = testFactorial();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("isnan"))
	{
		testResult = testIsNan<float>(testDuration);
		Log::info() << " ";
		testResult = testIsNan<double>(testDuration);
		Log::info() << " ";
		testResult = testIsNan<uint8_t>(testDuration);
		Log::info() << " ";
		testResult = testIsNan<int32_t>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("isinf"))
	{
		testResult = testIsInf<float>(testDuration);
		Log::info() << " ";
		testResult = testIsInf<double>(testDuration);
		Log::info() << " ";
		testResult = testIsInf<uint8_t>(testDuration);
		Log::info() << " ";
		testResult = testIsInf<int32_t>(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("pow"))
	{
		testResult = testPow();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("isinsidevaluerange"))
	{
		testResult = testIsInsideValueRange(testDuration);

		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	Validation validation;

	OCEAN_EXPECT_EQUAL(validation, NumericT<double>::eps(), 1e-12);

	OCEAN_EXPECT_EQUAL(validation, NumericT<float>::eps(), 1e-6f);


	OCEAN_EXPECT_EQUAL(validation, NumericT<signed char>::eps(), (signed char)(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned char>::eps(), (unsigned char)(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<int8_t>::eps(), int8_t(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<uint8_t>::eps(), uint8_t(0));


	OCEAN_EXPECT_EQUAL(validation, NumericT<short>::eps(), short(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned short>::eps(), (unsigned short)(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<int16_t>::eps(), int16_t(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<uint16_t>::eps(), uint16_t(0));


	OCEAN_EXPECT_EQUAL(validation, NumericT<int>::eps(), 0);

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned int>::eps(), 0u);

	OCEAN_EXPECT_EQUAL(validation, NumericT<int32_t>::eps(), int32_t(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<uint32_t>::eps(), uint32_t(0));


	OCEAN_EXPECT_EQUAL(validation, NumericT<long long>::eps(), 0ll);

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned long long>::eps(), 0ull);

	OCEAN_EXPECT_EQUAL(validation, NumericT<int64_t>::eps(), int64_t(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<uint64_t>::eps(), uint64_t(0));


	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testWeakEps()
{
	Log::info() << "WeakEps test:";

	Validation validation;

	OCEAN_EXPECT_EQUAL(validation, NumericT<double>::weakEps(), 1e-6);

	OCEAN_EXPECT_EQUAL(validation, NumericT<float>::weakEps(), 1e-3f);


	OCEAN_EXPECT_EQUAL(validation, NumericT<signed char>::weakEps(), (signed char)(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned char>::weakEps(), (unsigned char)(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<int8_t>::weakEps(), int8_t(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<uint8_t>::weakEps(), uint8_t(0));


	OCEAN_EXPECT_EQUAL(validation, NumericT<short>::weakEps(), short(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned short>::weakEps(), (unsigned short)(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<int16_t>::weakEps(), int16_t(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<uint16_t>::weakEps(), uint16_t(0));


	OCEAN_EXPECT_EQUAL(validation, NumericT<int>::weakEps(), 0);

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned int>::weakEps(), 0u);

	OCEAN_EXPECT_EQUAL(validation, NumericT<int32_t>::weakEps(), int32_t(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<uint32_t>::weakEps(), uint32_t(0));


	OCEAN_EXPECT_EQUAL(validation, NumericT<long long>::weakEps(), 0ll);

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned long long>::weakEps(), 0ull);

	OCEAN_EXPECT_EQUAL(validation, NumericT<int64_t>::weakEps(), int64_t(0));

	OCEAN_EXPECT_EQUAL(validation, NumericT<uint64_t>::weakEps(), uint64_t(0));


	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testIsEqualDynamic()
{
	Log::info() << "Dynamic isEqual test:";

	Validation validation;

	if (std::is_same<Scalar, double>::value)
	{
		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<-5>(Scalar(0.00001), Scalar(0.00001), Numeric::eps()));
		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<-5>(Scalar(0.00001), Scalar(0.00001) + Numeric::eps() * Scalar(0.1), Numeric::eps()));

		OCEAN_EXPECT_FALSE(validation, Numeric::isEqual<-5>(Scalar(0.0001), Scalar(0.00015), Numeric::eps()));

		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<-5>(Scalar(1), Scalar(1.01), Numeric::weakEps()));
		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<-5>(Scalar(1), Scalar(2), Numeric::weakEps()));
		OCEAN_EXPECT_FALSE(validation, Numeric::isEqual<-5>(Scalar(1), Scalar(2.1), Numeric::weakEps()));

		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<-5>(Scalar(100), Scalar(105), Numeric::weakEps()));
	}
	else
	{
		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<-4>(Scalar(0.0001), Scalar(0.0001), Numeric::eps()));
		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<-4>(Scalar(0.0001), Scalar(0.0001) + Numeric::eps() * Scalar(0.1), Numeric::eps()));

		OCEAN_EXPECT_FALSE(validation, Numeric::isEqual<-4>(Scalar(0.001), Scalar(0.0015), Numeric::eps()));

		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<-4>(Scalar(1), Scalar(1.01), Numeric::weakEps()));
		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<-4>(Scalar(1), Scalar(2), Numeric::weakEps()));
		OCEAN_EXPECT_FALSE(validation, Numeric::isEqual<-2>(Scalar(1), Scalar(2.1), Numeric::weakEps()));

		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<-2>(Scalar(100), Scalar(105), Numeric::weakEps()));
	}

	for (unsigned int n = 0u; n < 500u; ++n)
	{
		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<0>(Scalar(n), Scalar(n), Numeric::eps()));
		OCEAN_EXPECT_FALSE(validation, Numeric::isEqual<0>(Scalar(n), Scalar(n) - Scalar(1), Numeric::eps()));

		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<-4>(Scalar(n), Scalar(n), Numeric::eps()));

		if (std::is_same<Scalar, double>::value)
		{
			OCEAN_EXPECT_FALSE(validation, Numeric::isEqual<-4>(Scalar(n), Scalar(n) - Scalar(1), Numeric::eps()));
		}
		else
		{
			OCEAN_EXPECT_FALSE(validation, Numeric::isEqual<-2>(Scalar(n), Scalar(n) - Scalar(1), Numeric::eps()));
		}

		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual<6>(Scalar(n), Scalar(n), Numeric::eps()));
		OCEAN_EXPECT_FALSE(validation, Numeric::isEqual<6>(Scalar(n), Scalar(n) - Scalar(1), Numeric::eps()));
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testRound()
{
	Log::info() << "Round test:";

	Validation validation;

	OCEAN_EXPECT_EQUAL(validation, Numeric::round32(Scalar(1.4)), 1);
	OCEAN_EXPECT_EQUAL(validation, Numeric::round32(Scalar(1.5)), 2);
	OCEAN_EXPECT_EQUAL(validation, Numeric::round32(Scalar(10.4)), 10);
	OCEAN_EXPECT_EQUAL(validation, Numeric::round32(Scalar(10.6)), 11);

	OCEAN_EXPECT_EQUAL(validation, Numeric::round32(Scalar(-1.4)), -1);
	OCEAN_EXPECT_EQUAL(validation, Numeric::round32(Scalar(-1.5)), -2);
	OCEAN_EXPECT_EQUAL(validation, Numeric::round32(Scalar(-10.4)), -10);
	OCEAN_EXPECT_EQUAL(validation, Numeric::round32(Scalar(-10.6)), -11);

	OCEAN_EXPECT_EQUAL(validation, Numeric::round64(Scalar(1.4)), int64_t(1));
	OCEAN_EXPECT_EQUAL(validation, Numeric::round64(Scalar(1.5)), int64_t(2));
	OCEAN_EXPECT_EQUAL(validation, Numeric::round64(Scalar(10.4)), int64_t(10));
	OCEAN_EXPECT_EQUAL(validation, Numeric::round64(Scalar(10.6)), int64_t(11));

	OCEAN_EXPECT_EQUAL(validation, Numeric::round64(Scalar(-1.4)), int64_t(-1));
	OCEAN_EXPECT_EQUAL(validation, Numeric::round64(Scalar(-1.5)), int64_t(-2));
	OCEAN_EXPECT_EQUAL(validation, Numeric::round64(Scalar(-10.4)), int64_t(-10));
	OCEAN_EXPECT_EQUAL(validation, Numeric::round64(Scalar(-10.6)), int64_t(-11));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestNumeric::testAngleConversion(const double testDuration)
{
	Log::info() << "Angle conversion test for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			const Scalar deg = Random::scalar(randomGenerator, -1000, 1000);
			const Scalar rad = Numeric::deg2rad(deg);

			const Scalar radTest = deg * Numeric::pi() / Scalar(180);

			OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(rad, radTest, Numeric::weakEps()));
		}

		{
			const Scalar rad = Random::scalar(randomGenerator, -20, 20);
			const Scalar deg = Numeric::rad2deg(rad);

			const Scalar degTest = rad * Scalar(180) / Numeric::pi();

			OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(deg, degTest, Numeric::weakEps()));
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testAngleAdjustPositive(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Angle adjust positive test:";

	const unsigned int constIterations = 100000u;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.95, randomGenerator);

	// for 32 bit float values we need to weaken the epsilon by one magnitude
	const Scalar epsilon = std::is_same<Scalar, float>::value ? Numeric::eps() * Scalar(10) : Numeric::eps();

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0; n < constIterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Scalar value = Random::scalar(randomGenerator, 0, Numeric::pi2() - Numeric::eps());

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(value), value, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(value + Numeric::pi2() * 5), value, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(value + Numeric::pi2() * 6), value, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(value - Numeric::pi2() * 5), value, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(value - Numeric::pi2() * 6), value, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsEqual(Numeric::angleAdjustPositive(Numeric::pi2()), 0, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}
		}
	}
	while (validation.needMoreIterations() || Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testAngleAdjustNull(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Angle adjust null test:";

	const unsigned int constIterations = 100000u;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.95, randomGenerator);

	// for 32 bit float values we need to weaken the epsilon by one magnitude
	const Scalar epsilon = std::is_same<Scalar, float>::value ? Numeric::eps() * Scalar(10) : Numeric::eps();

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0; n < constIterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Scalar value = Random::scalar(randomGenerator, -Numeric::pi() + Numeric::eps(), Numeric::pi());

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(value), value, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(value + Numeric::pi2() * 5), value, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(value + Numeric::pi2() * 6), value, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(value - Numeric::pi2() * 5), value, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(value - Numeric::pi2() * 6), value, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsEqual(Numeric::angleAdjustNull(Numeric::pi2()), 0, epsilon))
			{
				scopedIteration.setInaccurate();
				continue;
			}
		}
	}
	while (validation.needMoreIterations() || Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testAngleIsEqual(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Angle is equal:";

	const unsigned int constIterations = 100000u;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Scalar value = Random::scalar(randomGenerator, 0, Numeric::pi2());

			if (!Numeric::angleIsWeakEqual(value, value + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100))))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (Numeric::angleIsWeakEqual(value + Random::scalar(randomGenerator, Scalar(0.1), 1), value + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100))))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsWeakEqual(value, value - Numeric::pi2()))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (Numeric::angleIsWeakEqual(value - Random::scalar(randomGenerator, Scalar(0.1), 1), Numeric::pi2() - value))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsWeakEqual(value + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100)), value - Numeric::pi2() + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100))))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (Numeric::angleIsWeakEqual(value + Random::scalar(randomGenerator, Scalar(0.1), 1) + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100)), Numeric::pi2() - value + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100))))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsWeakEqual(Numeric::eps(), Numeric::pi2() - Numeric::eps()))
			{
				scopedIteration.setInaccurate();
				continue;
			}
		}
	}
	while (validation.needMoreIterations() || Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testAngleIsBelowThreshold(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Angle is below threshold:";

	const unsigned int constIterations = 100000u;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.95, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			Scalar value = Random::scalar(randomGenerator, 0, Numeric::pi2());
			Scalar offset = Random::scalar(randomGenerator, -Numeric::deg2rad(20 - Numeric::eps()), Numeric::deg2rad(Scalar(20 - Numeric::eps())));

			if (!Numeric::angleIsBelowThreshold(value, value + offset, Numeric::deg2rad(20)))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (!Numeric::angleIsBelowThreshold(value + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100)), value + offset + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100)), Numeric::deg2rad(20)))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			offset = Numeric::deg2rad(20) + Random::scalar(randomGenerator, Numeric::eps(), Numeric::deg2rad(5));

			if (Numeric::angleIsBelowThreshold(value, value + offset, Numeric::deg2rad(20)))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (Numeric::angleIsBelowThreshold(value + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100)), value + offset + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100)), Numeric::deg2rad(20)))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			offset = -Numeric::deg2rad(20) - Random::scalar(randomGenerator, Numeric::eps(), Numeric::deg2rad(5));

			if (Numeric::angleIsBelowThreshold(value, value + offset, Numeric::deg2rad(20)))
			{
				scopedIteration.setInaccurate();
				continue;
			}

			if (Numeric::angleIsBelowThreshold(value + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100)), value + offset + Numeric::pi2() * Scalar(Random::random(randomGenerator, -100, 100)), Numeric::deg2rad(20)))
			{
				scopedIteration.setInaccurate();
				continue;
			}
		}
	}
	while (validation.needMoreIterations() || Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testGaussianDistribution1(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "1D Gaussian distribution test:";

	const unsigned int constIterations = 100000u;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.95, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Scalar sigma = Random::scalar(randomGenerator, Scalar(0.01), Scalar(10));
			const Scalar x = Random::scalar(randomGenerator, Scalar(-sigma * 5), Scalar(sigma * 5));

			const Scalar value = Numeric::gaussianDistribution(x, sigma);
			const Scalar testValue = 1 / (sigma * Numeric::sqrt(2 * Numeric::pi())) * Numeric::pow(Numeric::e(), - Scalar(0.5) * (x * x) / (sigma * sigma));

			if (!Numeric::isEqual(value, testValue))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testGaussianDistribution2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "2D Gaussian distribution test:";

	const unsigned int constIterations = 100000u;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.95, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (size_t n = 0; n < constIterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Scalar sigmaX = Random::scalar(randomGenerator, Scalar(0.01), Scalar(10));
			const Scalar sigmaY = Random::scalar(randomGenerator, Scalar(0.01), Scalar(10));

			const Scalar x = Random::scalar(randomGenerator, Scalar(-sigmaX * 5), Scalar(sigmaX * 5));
			const Scalar y = Random::scalar(randomGenerator, Scalar(-sigmaY * 5), Scalar(sigmaY * 5));

			const Scalar value = Numeric::gaussianDistribution2(x, y, sigmaX, sigmaY);
			const Scalar testValue = Numeric::gaussianDistribution(x, sigmaX) * Numeric::gaussianDistribution(y, sigmaY);

			if (!Numeric::isEqual(value, testValue))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testGaussianDistribution3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "3D Gaussian distribution test:";

	const unsigned int constIterations = 100000u;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.95, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < constIterations; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Scalar sigmaX = Random::scalar(randomGenerator, Scalar(0.01), Scalar(10));
			const Scalar sigmaY = Random::scalar(randomGenerator, Scalar(0.01), Scalar(10));
			const Scalar sigmaZ = Random::scalar(randomGenerator, Scalar(0.01), Scalar(10));

			const Scalar x = Random::scalar(randomGenerator, Scalar(-sigmaX * 5), Scalar(sigmaX * 5));
			const Scalar y = Random::scalar(randomGenerator, Scalar(-sigmaY * 5), Scalar(sigmaY * 5));
			const Scalar z = Random::scalar(randomGenerator, Scalar(-sigmaZ * 5), Scalar(sigmaZ * 5));

			const Scalar value = Numeric::gaussianDistribution3(x, y, z, sigmaX, sigmaY, sigmaZ);
			const Scalar testValue = Numeric::gaussianDistribution(x, sigmaX) * Numeric::gaussianDistribution(y, sigmaY) * Numeric::gaussianDistribution(z, sigmaZ);

			if (!Numeric::isEqual(value, testValue))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testAbs()
{
	Log::info() << "Abs test:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, NumericT<signed char>::abs(5) == (signed char)(5) && NumericT<signed char>::abs(-5) == (signed char)(5));

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned char>::abs(5), (unsigned char)5);

	OCEAN_EXPECT_TRUE(validation, NumericT<short>::abs(5) == short(5) && NumericT<short>::abs(-5) == short(5));

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned short>::abs(5), (unsigned short)5);

	OCEAN_EXPECT_TRUE(validation, NumericT<int>::abs(5) == int(5) && NumericT<int>::abs(-5) == int(5));

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned int>::abs(5), (unsigned int)5);

	OCEAN_EXPECT_TRUE(validation, NumericT<long long>::abs(5) == (long long)5 && NumericT<long long>::abs(-5) == (long long)5);

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned long long>::abs(5), (unsigned long long)5);

	OCEAN_EXPECT_TRUE(validation, NumericT<float>::abs(5.0f) == 5.0f && NumericT<float>::abs(-5.0f) == 5.0f);

	OCEAN_EXPECT_TRUE(validation, NumericT<double>::abs(5.0) == 5.0 && NumericT<double>::abs(-5.0) == 5.0);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testSecureAbs()
{
	Log::info() << "Secure abs test:";

	Validation validation;

	// char: [-128, 127]
	{
		const unsigned char a0 = NumericT<signed char>::secureAbs((signed char)(127));
		const int a1 = NumericT<signed char>::secureAbs((signed char)(127));
		const unsigned int a2 = NumericT<signed char>::secureAbs((signed char)(127));

		OCEAN_EXPECT_TRUE(validation, a0 == 127u && a1 == 127 && a2 == 127u);
	}
	{
		const unsigned char a0 = NumericT<signed char>::secureAbs((signed char)(-128));
		const int a1 = NumericT<signed char>::secureAbs((signed char)(-128));
		const unsigned int a2 = NumericT<signed char>::secureAbs((signed char)(-128));

		OCEAN_EXPECT_TRUE(validation, a0 == 128u && a1 == 128 && a2 == 128u);
	}
	{
		const unsigned char a0 = NumericT<signed char>::secureAbs((signed char)(uint8_t(128)));
		const int a1 = NumericT<signed char>::secureAbs((signed char)(uint8_t(uint8_t(128))));
		const unsigned int a2 = NumericT<signed char>::secureAbs((signed char)(uint8_t(128)));

		OCEAN_EXPECT_TRUE(validation, a0 == 128u && a1 == 128 && a2 == 128u);
	}

	// short: [-32768, 32767]
	{
		const unsigned short a0 = NumericT<short>::secureAbs(short(32767));
		const int a1 = NumericT<short>::secureAbs(short(32767));
		const unsigned int a2 = NumericT<short>::secureAbs(short(32767));

		OCEAN_EXPECT_TRUE(validation, a0 == 32767u && a1 == 32767 && a2 == 32767u);
	}
	{
		const unsigned short a0 = NumericT<short>::secureAbs(short(-32768));
		const int a1 = NumericT<short>::secureAbs(short(-32768));
		const unsigned int a2 = NumericT<short>::secureAbs(short(-32768));

		OCEAN_EXPECT_TRUE(validation, a0 == 32768u && a1 == 32768 && a2 == 32768u);
	}
	{
		const unsigned short a0 = NumericT<short>::secureAbs(short(uint16_t(32768)));
		const int a1 = NumericT<short>::secureAbs(short(uint16_t(32768)));
		const unsigned int a2 = NumericT<short>::secureAbs(short(uint16_t(32768)));

		OCEAN_EXPECT_TRUE(validation, a0 == 32768u && a1 == 32768 && a2 == 32768u);
	}

	// int: [-2147483648, 2147483647]
	{
		const unsigned int a0 = NumericT<int>::secureAbs(int(2147483647));
		const long long a1 = NumericT<int>::secureAbs(int(2147483647));
		const unsigned long long a2 = NumericT<int>::secureAbs(int(2147483647));

		OCEAN_EXPECT_TRUE(validation, a0 == 2147483647u && a1 == 2147483647 && a2 == 2147483647u);
	}

	{
		// as -2147483648 may be composed of an unaray (-) operator and a signed integer value we create -2147483648 by subtracting -1 from -2147483647
		const unsigned int a0 = NumericT<int>::secureAbs(-2147483647 - 1);
		const long long a1 = NumericT<int>::secureAbs(-2147483647 - 1);
		const unsigned long long a2 = NumericT<int>::secureAbs(-2147483647 - 1);

		OCEAN_EXPECT_TRUE(validation, a0 == 2147483648u && a1 == 2147483648ll && a2 == 2147483648ull);
	}
	{
		const unsigned int a0 = NumericT<int>::secureAbs(static_cast<int>(int64_t(2147483648ll)));
		const long long a1 = NumericT<int>::secureAbs(static_cast<int>(int64_t(2147483648ll)));
		const unsigned long long a2 = NumericT<int>::secureAbs(static_cast<int>(int64_t(2147483648ll)));

		OCEAN_EXPECT_TRUE(validation, a0 == 2147483648u && a1 == 2147483648ll && a2 == 2147483648ull);
	}

	// long long: [-9223372036854775808, 9223372036854775807]
	{
		const unsigned long long a0 = NumericT<long long>::secureAbs(9223372036854775807ll);

		OCEAN_EXPECT_EQUAL(validation, a0, 9223372036854775807ull);
	}
	{
		// as -9223372036854775808 may be composed of an unaray (-) operator and a signed long long value we create -9223372036854775808 by subtracting -1 from -9223372036854775807
		const unsigned long long a0 = NumericT<long long>::secureAbs(-9223372036854775807ll - 1ll);

		OCEAN_EXPECT_EQUAL(validation, a0, 9223372036854775808ull);
	}

	OCEAN_EXPECT_TRUE(validation, NumericT<signed char>::secureAbs(5) == (signed char)(5) && NumericT<signed char>::secureAbs(-5) == (signed char)(5));

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned char>::secureAbs(5), (unsigned char)5);

	OCEAN_EXPECT_TRUE(validation, NumericT<short>::secureAbs(5) == short(5) && NumericT<short>::secureAbs(-5) == short(5));

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned short>::secureAbs(5), (unsigned short)5);

	OCEAN_EXPECT_TRUE(validation, NumericT<int>::secureAbs(5) == int(5) && NumericT<int>::secureAbs(-5) == int(5));

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned int>::secureAbs(5), (unsigned int)5);

	OCEAN_EXPECT_TRUE(validation, NumericT<long long>::secureAbs(5) == (long long)5 && NumericT<long long>::secureAbs(-5) == (long long)5);

	OCEAN_EXPECT_EQUAL(validation, NumericT<unsigned long long>::secureAbs(5), (unsigned long long)5);

	OCEAN_EXPECT_TRUE(validation, NumericT<float>::secureAbs(5.0f) == 5.0f && NumericT<float>::secureAbs(-5.0f) == 5.0f);

	OCEAN_EXPECT_TRUE(validation, NumericT<double>::secureAbs(5.0) == 5.0 && NumericT<double>::secureAbs(-5.0) == 5.0);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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
		{
			values[n] = Random::scalar(-1000, 1000);
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceFloor);

			for (size_t n = 0; n < size; ++n)
			{
				results[n] = int(Numeric::floor(values[n]));
			}
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceCast);

			for (size_t n = 0; n < size; ++n)
			{
				results[n] = int(values[n]);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	if (results[0] == 0) // we make this dummy check to receive a serious result
	{
		Log::info() << "Performance: " << performanceFloor.averageMseconds() * 1000.0 / double(size) << "mys";
	}
	else
	{
		Log::info() << "Performance: " << performanceFloor.averageMseconds() * 1000.0 / double(size) << "mys";
	}

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
		{
			values[n] = Random::scalar(-1000, 1000);
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceCeil);

			for (size_t n = 0; n < size; ++n)
			{
				results[n] = int(Numeric::ceil(values[n]));
			}
		}

		{
			const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceCast);

			for (size_t n = 0; n < size; ++n)
			{
				results[n] = int(values[n]);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	if (results[0] == 0) // we make this dummy check to receive a serious result
	{
		Log::info() << "Performance: " << performanceCeil.averageMseconds() * 1000.0 / double(size) << "mys";
	}
	else
	{
		Log::info() << "Performance: " << performanceCeil.averageMseconds() * 1000.0 / double(size) << "mys";
	}

	Log::info() << "Performance integer casting: " << performanceCast.averageMseconds() * 1000.0 / double(size) << "mys";
	Log::info() << "Performance factor: " << String::toAString(performanceCeil.averageMseconds() / performanceCast.averageMseconds(), 2u) << " (times slower)";

	return true;
}

bool TestNumeric::testLog2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test log2 function:";

	static const size_t size = 10000;

	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (size_t n = 0; n < size; ++n)
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

			const Scalar value = Random::scalar(randomGenerator, Numeric::weakEps(), Scalar(1000));
			const Scalar valueLog2 = Numeric::log2(value);

			const Scalar test = Numeric::pow(Scalar(2), valueLog2);

			if (!Numeric::isWeakEqual(value, test))
			{
				scopedIteration.setInaccurate();
			}
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		OCEAN_EXPECT_EQUAL(validation, NumericT<signed char>::sign(0), (signed char)(0));

		OCEAN_EXPECT_EQUAL(validation, NumericT<signed char>::sign((signed char)(RandomI::random(randomGenerator, 1, 127))), (signed char)(1));

		OCEAN_EXPECT_EQUAL(validation, NumericT<signed char>::sign((signed char)(RandomI::random(randomGenerator, -127, -1))), (signed char)(-1));


		OCEAN_EXPECT_EQUAL(validation, NumericT<short>::sign(0), short(0));

		OCEAN_EXPECT_EQUAL(validation, NumericT<short>::sign(short(RandomI::random(randomGenerator, 1, 1000))), short(1));

		OCEAN_EXPECT_EQUAL(validation, NumericT<short>::sign(short(RandomI::random(randomGenerator, -1000, -1))), short(-1));


		OCEAN_EXPECT_EQUAL(validation, NumericT<int>::sign(0), 0);

		OCEAN_EXPECT_EQUAL(validation, NumericT<int>::sign(RandomI::random(randomGenerator, 1, 1000)), 1);

		OCEAN_EXPECT_EQUAL(validation, NumericT<int>::sign(RandomI::random(randomGenerator, -1000, -1)), -1);


		OCEAN_EXPECT_EQUAL(validation, NumericT<long long>::sign(0ll), 0ll);

		OCEAN_EXPECT_EQUAL(validation, NumericT<long long>::sign(RandomI::random(randomGenerator, 1, 1000)), 1ll);

		OCEAN_EXPECT_EQUAL(validation, NumericT<long long>::sign(RandomI::random(randomGenerator, -1000, -1)), -1ll);


		OCEAN_EXPECT_EQUAL(validation, NumericT<float>::sign(0.0f), 0.0f);

		OCEAN_EXPECT_EQUAL(validation, NumericT<float>::sign(RandomF::scalar(randomGenerator, 0.01f, 100.0f)), 1.0f);

		OCEAN_EXPECT_EQUAL(validation, NumericT<float>::sign(RandomF::scalar(randomGenerator, -100.0f, -0.01f)), -1.0f);


		OCEAN_EXPECT_EQUAL(validation, NumericT<double>::sign(0.0), 0.0);

		OCEAN_EXPECT_EQUAL(validation, NumericT<double>::sign(RandomD::scalar(randomGenerator, 0.01, 100.0)), 1.0);

		OCEAN_EXPECT_EQUAL(validation, NumericT<double>::sign(RandomD::scalar(randomGenerator, -100.0, -0.01f)), -1.0);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testCopySign()
{
	Log::info() << "Test copySign function:";

	Validation validation;

	OCEAN_EXPECT_EQUAL(validation, NumericT<int>::copySign(-5, 1), 5);

	OCEAN_EXPECT_EQUAL(validation, NumericT<int>::copySign(-5, -1), -5);

	OCEAN_EXPECT_EQUAL(validation, NumericT<int>::copySign(5000, 4), 5000);

	OCEAN_EXPECT_EQUAL(validation, NumericT<int>::copySign(5000, -4), -5000);


	OCEAN_EXPECT_EQUAL(validation, NumericT<float>::copySign(-5.123f, 1.555f), 5.123f);

	OCEAN_EXPECT_EQUAL(validation, NumericT<float>::copySign(-5.123f, -1.678f), -5.123f);

	OCEAN_EXPECT_EQUAL(validation, NumericT<float>::copySign(5000.456f, 4.321f), 5000.456f);

	OCEAN_EXPECT_EQUAL(validation, NumericT<float>::copySign(5000.456f, -4.753f), -5000.456f);


	OCEAN_EXPECT_EQUAL(validation, NumericT<double>::copySign(-5.123, 1.444), 5.123);

	OCEAN_EXPECT_EQUAL(validation, NumericT<double>::copySign(-5.123, -1.489), -5.123);

	OCEAN_EXPECT_EQUAL(validation, NumericT<double>::copySign(5000.456, 4.258), 5000.456);

	OCEAN_EXPECT_EQUAL(validation, NumericT<double>::copySign(5000.456, -4.654), -5000.456);


	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testInvertSign()
{
	Log::info() << "Test invertSign function:";

	Validation validation;

	OCEAN_EXPECT_EQUAL(validation, NumericT<int>::invertSign(-5, 1), -5);

	OCEAN_EXPECT_EQUAL(validation, NumericT<int>::invertSign(-5, -1), 5);

	OCEAN_EXPECT_EQUAL(validation, NumericT<int>::invertSign(5000, 4), -5000);

	OCEAN_EXPECT_EQUAL(validation, NumericT<int>::invertSign(5000, -4), 5000);


	OCEAN_EXPECT_EQUAL(validation, NumericT<float>::invertSign(-5.123f, 1.555f), -5.123f);

	OCEAN_EXPECT_EQUAL(validation, NumericT<float>::invertSign(-5.123f, -1.678f), 5.123f);

	OCEAN_EXPECT_EQUAL(validation, NumericT<float>::invertSign(5000.456f, 4.321f), -5000.456f);

	OCEAN_EXPECT_EQUAL(validation, NumericT<float>::invertSign(5000.456f, -4.753f), 5000.456f);


	OCEAN_EXPECT_EQUAL(validation, NumericT<double>::invertSign(-5.123, 1.444), -5.123);

	OCEAN_EXPECT_EQUAL(validation, NumericT<double>::invertSign(-5.123, -1.489), 5.123);

	OCEAN_EXPECT_EQUAL(validation, NumericT<double>::invertSign(5000.456, 4.258), -5000.456);

	OCEAN_EXPECT_EQUAL(validation, NumericT<double>::invertSign(5000.456, -4.654), 5000.456);


	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testFactorial()
{
	Log::info() << "Test factorial function:";

	Validation validation;

	for (unsigned int n = 0u; n <= 12u; ++n)
	{
		const unsigned int factorial = NumericT<unsigned int>::factorial(n); // n!

		if (n == 0u)
		{
			OCEAN_EXPECT_EQUAL(validation, factorial, 1u);
		}
		else
		{
			// let's test the remaining values by simple multiplication

			double value = 1.0;

			for (unsigned int i = 1u; i <= n; ++i)
			{
				value *= double(i);
			}

			OCEAN_EXPECT_EQUAL(validation, double(factorial), value);
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestNumeric::testIsNan(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test isNan function for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_FALSE(validation, NumericT<T>::isNan(T(0)) || NumericT<T>::isNan(T(1)) || NumericT<T>::isNan(T(-1)));

	if constexpr (std::is_floating_point<T>::value)
	{
		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isNan(NumericT<T>::nan()));
	}

	const Timestamp startTimestamp(true);

	do
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			{
				const T value = T(RandomT<T>::scalar(randomGenerator, T(-100000), T(100000)));

				OCEAN_EXPECT_FALSE(validation, NumericT<T>::isNan(value));
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

				OCEAN_EXPECT_EQUAL(validation, valueIsNan, NumericF::isNan(floatValue));

#ifdef OCEAN_DEBUG
				// in a not-optimized build, we can use std's isnan() function as well
				OCEAN_EXPECT_EQUAL(validation, NumericF::isNan(floatValue), bool(std::isnan(floatValue)));
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

				OCEAN_EXPECT_EQUAL(validation, valueIsNan, NumericD::isNan(doubleValue));

#ifdef OCEAN_DEBUG
				// in a not-optimized build, we can use std's isnan() function as well
				OCEAN_EXPECT_EQUAL(validation, NumericD::isNan(doubleValue), bool(std::isnan(doubleValue)));
#endif
			}
		}
		else
		{
			// any value is not a NaN

			const T value = T(RandomI::random64(randomGenerator));

			OCEAN_EXPECT_FALSE(validation, NumericT<T>::isNan(value));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T>
bool TestNumeric::testIsInf(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test isInf function for '" << TypeNamer::name<T>() << "':";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_FALSE(validation, NumericT<T>::isInf(T(0)) || NumericT<T>::isInf(T(1)) || NumericT<T>::isInf(T(-1)));

	if constexpr (std::is_floating_point<T>::value)
	{
		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isInf(std::numeric_limits<T>::infinity()));

		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isInf(NumericT<T>::inf()));

		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isInf(-std::numeric_limits<T>::infinity()));

		OCEAN_EXPECT_TRUE(validation, NumericT<T>::isInf(-NumericT<T>::inf()));

		const T stdValue = std::numeric_limits<T>::infinity();
		const T numericValue = NumericT<T>::inf();

		OCEAN_EXPECT_EQUAL(validation, memcmp(&stdValue, &numericValue, sizeof(T)), 0);
	}

	const Timestamp startTimestamp(true);

	do
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			{
				const T value = T(RandomT<T>::scalar(randomGenerator, T(-100000), T(100000)));

				OCEAN_EXPECT_FALSE(validation, NumericT<T>::isInf(value));
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

				OCEAN_EXPECT_EQUAL(validation, valueIsInf, NumericF::isInf(floatValue));

#ifdef OCEAN_DEBUG
				// in a not-optimized build, we can use std's isinf() function as well
				OCEAN_EXPECT_EQUAL(validation, NumericF::isInf(floatValue), bool(std::isinf(floatValue)));
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

				OCEAN_EXPECT_EQUAL(validation, valueIsInf, NumericD::isInf(doubleValue));

#ifdef OCEAN_DEBUG
				// in a not-optimized build, we can use std's isinf() function as well
				OCEAN_EXPECT_EQUAL(validation, NumericD::isInf(doubleValue), bool(std::isinf(doubleValue)));
#endif
			}
		}
		else
		{
			// any value is not a NaN

			const T value = T(RandomI::random64(randomGenerator));

			OCEAN_EXPECT_FALSE(validation, NumericT<T>::isInf(value));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testPow()
{
	Log::info() << "Test pow function:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(Numeric::pow(8, 0), Scalar(1)));

	OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(Numeric::pow(1, 1), Scalar(1)));

	OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(Numeric::pow(Scalar(0.5), 2), Scalar(0.25)));

	OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(Numeric::pow(Scalar(-0.5), 2), Scalar(0.25)));

	OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(Numeric::pow(Scalar(-0.5), -2), Scalar(4)));

	OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(Numeric::integerPow(8, 0u), Scalar(1)));

	OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(Numeric::integerPow(1, 1u), Scalar(1)));

	OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(Numeric::integerPow(1, 2u), Scalar(1)));

	OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(Numeric::integerPow(Scalar(0.5), 1u), Scalar(0.5)));

	OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(Numeric::integerPow(Scalar(0.5), 2u), Scalar(0.25)));

	OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(Numeric::integerPow(Scalar(10), 4u), Scalar(1e4)));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestNumeric::testIsInsideValueRange(const double testDuration)
{
	Log::info() << "Test pow function:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	// testing special cases

	OCEAN_EXPECT_TRUE(validation, NumericT<std::string>::isInsideValueRange(std::string("test")));

	OCEAN_EXPECT_FALSE(validation, NumericT<std::string>::isInsideValueRange(false));

	const Timestamp startTimestamp(true);

	do
	{
		{
			// identical data types

			OCEAN_EXPECT_TRUE(validation, NumericT<bool>::isInsideValueRange(RandomI::boolean(randomGenerator)));

			OCEAN_EXPECT_TRUE(validation, NumericT<int8_t>::isInsideValueRange(int8_t(RandomI::random64(randomGenerator))));
			OCEAN_EXPECT_TRUE(validation, NumericT<uint8_t>::isInsideValueRange(uint8_t(RandomI::random64(randomGenerator))));

			OCEAN_EXPECT_TRUE(validation, NumericT<int16_t>::isInsideValueRange(int16_t(RandomI::random64(randomGenerator))));
			OCEAN_EXPECT_TRUE(validation, NumericT<uint16_t>::isInsideValueRange(uint16_t(RandomI::random64(randomGenerator))));

			OCEAN_EXPECT_TRUE(validation, NumericT<int32_t>::isInsideValueRange(int32_t(RandomI::random64(randomGenerator))));
			OCEAN_EXPECT_TRUE(validation, NumericT<uint32_t>::isInsideValueRange(uint32_t(RandomI::random64(randomGenerator))));

			OCEAN_EXPECT_TRUE(validation, NumericT<int64_t>::isInsideValueRange(int64_t(RandomI::random64(randomGenerator))));
			OCEAN_EXPECT_TRUE(validation, NumericT<uint64_t>::isInsideValueRange(uint64_t(RandomI::random64(randomGenerator))));

			OCEAN_EXPECT_TRUE(validation, NumericT<float>::isInsideValueRange(RandomF::scalar(randomGenerator, -1000.0f, 1000.0f)));
			OCEAN_EXPECT_TRUE(validation, NumericT<double>::isInsideValueRange(RandomD::scalar(randomGenerator, 1000000.0, 1000000.0)));
		}

		{
			// float -> double, and double -> float

			OCEAN_EXPECT_TRUE(validation, NumericT<float>::isInsideValueRange(RandomD::scalar(randomGenerator, 1000000.0, 1000000.0)));

			OCEAN_EXPECT_TRUE(validation, NumericT<double>::isInsideValueRange(RandomF::scalar(randomGenerator, -1000.0f, 1000.0f)));
		}

		{
			// float -> uint8_t, uint8_t -> float

			OCEAN_EXPECT_FALSE(validation, NumericT<float>::isInsideValueRange(uint8_t(RandomI::random32(randomGenerator))));

			OCEAN_EXPECT_FALSE(validation, NumericT<uint8_t>::isInsideValueRange(RandomF::scalar(randomGenerator, -1000.0f, 1000.0f)));
		}

		{
			// double -> uint8_t, double -> float

			OCEAN_EXPECT_FALSE(validation, NumericT<double>::isInsideValueRange(uint8_t(RandomI::random32(randomGenerator))));

			OCEAN_EXPECT_FALSE(validation, NumericT<uint8_t>::isInsideValueRange(RandomD::scalar(randomGenerator, -1000.0, 1000.0)));
		}

		{
			// int8_t

			const int8_t value = int8_t(RandomI::random64(randomGenerator));

			OCEAN_EXPECT_TRUE(validation, NumericT<int16_t>::isInsideValueRange(value));
			OCEAN_EXPECT_TRUE(validation, NumericT<int32_t>::isInsideValueRange(value));
			OCEAN_EXPECT_TRUE(validation, NumericT<int64_t>::isInsideValueRange(value));

			OCEAN_EXPECT_EQUAL(validation, NumericT<uint8_t>::isInsideValueRange(value), value >= int8_t(0));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint16_t>::isInsideValueRange(value), value >= int8_t(0));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint32_t>::isInsideValueRange(value), value >= int8_t(0));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint64_t>::isInsideValueRange(value), value >= int8_t(0));

			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int32_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint32_t>(value));
		}

		{
			// int16_t

			const int16_t value = int16_t(RandomI::random64(randomGenerator));

			OCEAN_EXPECT_EQUAL(validation, NumericT<int8_t>::isInsideValueRange(value), value >= int16_t(-128) && value <= int16_t(127));
			OCEAN_EXPECT_TRUE(validation, NumericT<int32_t>::isInsideValueRange(value));
			OCEAN_EXPECT_TRUE(validation, NumericT<int64_t>::isInsideValueRange(value));

			OCEAN_EXPECT_EQUAL(validation, NumericT<uint8_t>::isInsideValueRange(value), value >= int16_t(0) && value <= int16_t(255));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint16_t>::isInsideValueRange(value), value >= int16_t(0));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint32_t>::isInsideValueRange(value), value >= int16_t(0));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint64_t>::isInsideValueRange(value), value >= int16_t(0));

			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int32_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint32_t>(value));
		}

		{
			// int32_t

			const int32_t value = int32_t(RandomI::random64(randomGenerator));

			OCEAN_EXPECT_EQUAL(validation, NumericT<int8_t>::isInsideValueRange(value), value >= int32_t(-128) && value <= int32_t(127));
			OCEAN_EXPECT_EQUAL(validation, NumericT<int16_t>::isInsideValueRange(value), value >= int32_t(-32768) && value <= int32_t(32767));
			OCEAN_EXPECT_TRUE(validation, NumericT<int64_t>::isInsideValueRange(value));

			OCEAN_EXPECT_EQUAL(validation, NumericT<uint8_t>::isInsideValueRange(value), value >= int32_t(0) && value <= int32_t(255));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint16_t>::isInsideValueRange(value), value >= int32_t(0) && value <= int32_t(65535));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint32_t>::isInsideValueRange(value), value >= int32_t(0));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint64_t>::isInsideValueRange(value), value >= int32_t(0));

			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int32_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint32_t>(value));
		}

		{
			// int64_t

			const int64_t value = int64_t(RandomI::random64(randomGenerator));

			OCEAN_EXPECT_EQUAL(validation, NumericT<int8_t>::isInsideValueRange(value), value >= int64_t(-128) && value <= int64_t(127ll));
			OCEAN_EXPECT_EQUAL(validation, NumericT<int16_t>::isInsideValueRange(value), value >= int64_t(-32768) && value <= int64_t(32767ll));
			OCEAN_EXPECT_EQUAL(validation, NumericT<int32_t>::isInsideValueRange(value), value >= int64_t(-2147483648ll) && value <= int64_t(2147483647ll));


			OCEAN_EXPECT_EQUAL(validation, NumericT<uint8_t>::isInsideValueRange(value), value >= int64_t(0) && value <= int64_t(255ll));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint16_t>::isInsideValueRange(value), value >= int64_t(0) && value <= int64_t(65535ll));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint32_t>::isInsideValueRange(value), value >= int64_t(0) && value <= int64_t(4294967295ll));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint64_t>::isInsideValueRange(value), value >= int64_t(0));
		}

		{
			// uint8_t

			const uint8_t value = uint8_t(RandomI::random64(randomGenerator));

			OCEAN_EXPECT_TRUE(validation, NumericT<uint16_t>::isInsideValueRange(value));
			OCEAN_EXPECT_TRUE(validation, NumericT<uint32_t>::isInsideValueRange(value));
			OCEAN_EXPECT_TRUE(validation, NumericT<uint64_t>::isInsideValueRange(value));

			OCEAN_EXPECT_EQUAL(validation, NumericT<int8_t>::isInsideValueRange(value), value <= uint8_t(127));
			OCEAN_EXPECT_TRUE(validation, NumericT<int16_t>::isInsideValueRange(value));
			OCEAN_EXPECT_TRUE(validation, NumericT<int32_t>::isInsideValueRange(value));
			OCEAN_EXPECT_TRUE(validation, NumericT<int64_t>::isInsideValueRange(value));

			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int32_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint32_t>(value));
		}

		{
			// uint16_t

			const uint16_t value = uint16_t(RandomI::random64(randomGenerator));

			OCEAN_EXPECT_EQUAL(validation, NumericT<uint8_t>::isInsideValueRange(value), value <= uint16_t(255));
			OCEAN_EXPECT_TRUE(validation, NumericT<uint32_t>::isInsideValueRange(value));
			OCEAN_EXPECT_TRUE(validation, NumericT<uint64_t>::isInsideValueRange(value));

			OCEAN_EXPECT_EQUAL(validation, NumericT<int8_t>::isInsideValueRange(value), value <= uint16_t(127));
			OCEAN_EXPECT_EQUAL(validation, NumericT<int16_t>::isInsideValueRange(value), value <= uint16_t(32767));
			OCEAN_EXPECT_TRUE(validation, NumericT<int32_t>::isInsideValueRange(value));
			OCEAN_EXPECT_TRUE(validation, NumericT<int64_t>::isInsideValueRange(value));

			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int32_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint32_t>(value));
		}

		{
			// uint32_t

			const uint32_t value = uint32_t(RandomI::random64(randomGenerator));

			OCEAN_EXPECT_EQUAL(validation, NumericT<uint8_t>::isInsideValueRange(value), value <= uint32_t(255));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint16_t>::isInsideValueRange(value), value <= uint32_t(65535));
			OCEAN_EXPECT_TRUE(validation, NumericT<uint64_t>::isInsideValueRange(value));

			OCEAN_EXPECT_EQUAL(validation, NumericT<int8_t>::isInsideValueRange(value), value <= uint32_t(127));
			OCEAN_EXPECT_EQUAL(validation, NumericT<int16_t>::isInsideValueRange(value), value <= uint32_t(32767));
			OCEAN_EXPECT_EQUAL(validation, NumericT<int32_t>::isInsideValueRange(value), value <= uint32_t(2147483647));
			OCEAN_EXPECT_TRUE(validation, NumericT<int64_t>::isInsideValueRange(value));

			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint8_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint16_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<int32_t>(value));
			OCEAN_EXPECT_TRUE(validation, verifyInsideValueRangeWithCast<uint32_t>(value));
		}

		{
			// uint64_t

			const uint64_t value = uint64_t(RandomI::random64(randomGenerator));

			OCEAN_EXPECT_EQUAL(validation, NumericT<uint8_t>::isInsideValueRange(value), value <= uint64_t(255ull));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint16_t>::isInsideValueRange(value), value <= uint64_t(65535ull));
			OCEAN_EXPECT_EQUAL(validation, NumericT<uint32_t>::isInsideValueRange(value), value <= uint64_t(4294967295ull));

			OCEAN_EXPECT_EQUAL(validation, NumericT<int8_t>::isInsideValueRange(value), value <= uint64_t(127ull));
			OCEAN_EXPECT_EQUAL(validation, NumericT<int16_t>::isInsideValueRange(value), value <= uint64_t(32767ull));
			OCEAN_EXPECT_EQUAL(validation, NumericT<int32_t>::isInsideValueRange(value), value <= uint64_t(2147483647ull));
			OCEAN_EXPECT_EQUAL(validation, NumericT<int64_t>::isInsideValueRange(value), value <= uint64_t(9223372036854775807ull));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T, size_t tSize>
bool TestNumeric::testDotProduct(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with vector size " << String::insertCharacter(String::toAString(tSize), ',', 3, false) << ":";

	std::vector<T> vectorA(tSize);
	std::vector<T> vectorB(tSize);

	const T valueRange = std::is_same<T, double>::value ? T(1) : T(0.1);
	RandomGenerator randomGenerator;
	ValidationPrecision validation(0.99, randomGenerator);

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
		while (!startTimestamp.hasTimePassed(testDuration));

		if (dummyValue == T(1))
		{
			Log::info() << "Standard performance as reference: " << String::toAString(performanceStandard.averageMseconds()) << "ms";
		}
		else
		{
			Log::info() << "Standard performance as reference: " << String::toAString(performanceStandard.averageMseconds()) << "ms";
		}
	}

	{
		// now we test the default implementation of the framework using SIMD optimizations

		HighPerformanceStatistic performance;
		const Timestamp startTimestamp(true);

		do
		{
			ValidationPrecision::ScopedIteration scopedIteration(validation);

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

			if (!NumericT<T>::isEqual(result, test, NumericT<T>::eps() * (std::is_same<T, double>::value ? T(10) : T(1000))))
			{
				scopedIteration.setInaccurate();
			}
		}
		while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

		Log::info() << "Ocean performance individual: " << String::toAString(performance.averageMseconds()) << "ms";
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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
