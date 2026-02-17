/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestUtilities.h"

#include "ocean/base/Utilities.h"
#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestUtilities::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("Utilities test");

	Log::info() << " ";

	if (selector.shouldRun("minmax"))
	{
		testResult = testMinmax(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("modulo"))
	{
		testResult = testModulo(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("ringdistance"))
	{
		testResult = testRingDistance(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("mirrorvalue"))
	{
		testResult = testMirrorValue(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("divisionby2"))
	{
		testResult = testDivisionBy2(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("ispoweroftwo"))
	{
		testResult = testIsPowerOfTwo(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestUtilities, Minmax)
{
	EXPECT_TRUE(TestUtilities::testMinmax(GTEST_TEST_DURATION));
}

TEST(TestUtilities, Modulo)
{
	EXPECT_TRUE(TestUtilities::testModulo(GTEST_TEST_DURATION));
}

TEST(TestUtilities, RingDistance)
{
	EXPECT_TRUE(TestUtilities::testRingDistance(GTEST_TEST_DURATION));
}

TEST(TestUtilities, MirrorValue)
{
	EXPECT_TRUE(TestUtilities::testMirrorValue(GTEST_TEST_DURATION));
}

TEST(TestUtilities, DivisionBy2)
{
	EXPECT_TRUE(TestUtilities::testDivisionBy2(GTEST_TEST_DURATION));
}

TEST(TestUtilities, IsPowerOfTwo)
{
	EXPECT_TRUE(TestUtilities::testIsPowerOfTwo(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestUtilities::testMinmax(const double testDuration)
{
	Log::info() << "Minmax test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		{
			const int value = RandomI::random(randomGenerator, -500, 500);
			ocean_assert(value >= -500 && value <= 500);

			const int valueMin = RandomI::random(randomGenerator, -1000, 1000);
			const int valueMax = RandomI::random(randomGenerator, valueMin, 1000);

			const int result = minmax(valueMin, value, valueMax);

			OCEAN_EXPECT_TRUE(validation, !(value <= valueMin) || result == valueMin);
			OCEAN_EXPECT_TRUE(validation, !(value >= valueMax) || result == valueMax);
			OCEAN_EXPECT_TRUE(validation, !(value >= valueMin && value <= valueMax) || value == result);
		}

		{
			const unsigned int value = RandomI::random(randomGenerator, 5000u);
			ocean_assert(value >= 0u && value <= 5000u);

			const unsigned int valueMin = RandomI::random(randomGenerator, 2000u);
			const unsigned int valueMax = RandomI::random(randomGenerator, valueMin, 2000u);

			const unsigned int result = minmax(valueMin, value, valueMax);

			OCEAN_EXPECT_TRUE(validation, !(value <= valueMin) || result == valueMin);
			OCEAN_EXPECT_TRUE(validation, !(value >= valueMax) || result == valueMax);
			OCEAN_EXPECT_TRUE(validation, !(value >= valueMin && value <= valueMax) || value == result);
		}

		{
			const float value = float(RandomI::random(randomGenerator, -2000, 2000)) / 20.0f;
			ocean_assert(value >= -100.0f && value <= 100.0f);

			const float valueMin = float(RandomI::random(randomGenerator, -2000, 2000)) / 2.0f;
			const float valueMax = valueMin + float(RandomI::random(randomGenerator, 0, 2000)) / 4.0f;

			const float result = minmax(valueMin, value, valueMax);

			OCEAN_EXPECT_TRUE(validation, !(value <= valueMin) || result == valueMin);
			OCEAN_EXPECT_TRUE(validation, !(value >= valueMax) || result == valueMax);
			OCEAN_EXPECT_TRUE(validation, !(value >= valueMin && value <= valueMax) || value == result);
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testModulo(const double testDuration)
{
	Log::info() << "Modulo test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_EQUAL(validation, modulo<char>(0, 10), char(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<char>(1, 10), char(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<char>(9, 10), char(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<char>(10, 10), char(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<char>(11, 10), char(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<char>(-1, 10), char(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<char>(-2, 10), char(8));
	OCEAN_EXPECT_EQUAL(validation, modulo<char>(-9, 10), char(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<char>(-10, 10), char(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<char>(-11, 10), char(9));

	OCEAN_EXPECT_EQUAL(validation, modulo<short>(0, 10), short(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<short>(1, 10), short(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<short>(9, 10), short(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<short>(10, 10), short(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<short>(11, 10), short(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<short>(-1, 10), short(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<short>(-2, 10), short(8));
	OCEAN_EXPECT_EQUAL(validation, modulo<short>(-9, 10), short(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<short>(-10, 10), short(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<short>(-11, 10), short(9));

	OCEAN_EXPECT_EQUAL(validation, modulo<int>(0, 10), int(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<int>(1, 10), int(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<int>(9, 10), int(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<int>(10, 10), int(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<int>(11, 10), int(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<int>(-1, 10), int(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<int>(-2, 10), int(8));
	OCEAN_EXPECT_EQUAL(validation, modulo<int>(-9, 10), int(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<int>(-10, 10), int(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<int>(-11, 10), int(9));

	OCEAN_EXPECT_EQUAL(validation, modulo<long long>(0, 10), (long long)(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<long long>(1, 10), (long long)(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<long long>(9, 10), (long long)(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<long long>(10, 10), (long long)(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<long long>(11, 10), (long long)(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<long long>(-1, 10), (long long)(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<long long>(-2, 10), (long long)(8));
	OCEAN_EXPECT_EQUAL(validation, modulo<long long>(-9, 10), (long long)(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<long long>(-10, 10), (long long)(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<long long>(-11, 10), (long long)(9));

	OCEAN_EXPECT_EQUAL(validation, modulo<float>(0, 10), float(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(1, 10), float(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(9, 10), float(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(9.4f, 10), float(9.4f));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(9.5f, 10), float(9.5f));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(10, 10), float(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(11, 10), float(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(-1, 10), float(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(-1.4f, 10), float(8.6f));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(-1.5f, 10), float(8.5f));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(-2, 10), float(8));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(-9, 10), float(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(-10, 10), float(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<float>(-11, 10), float(9));

	OCEAN_EXPECT_EQUAL(validation, modulo<double>(0, 10), double(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(1, 10), double(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(9, 10), double(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(9.4, 10), double(9.4));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(9.5, 10), double(9.5));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(10, 10), double(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(11, 10), double(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(-1, 10), double(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(-1.4, 10), double(8.6));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(-1.5, 10), double(8.5));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(-2, 10), double(8));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(-9, 10), double(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(-10, 10), double(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<double>(-11, 10), double(9));

	OCEAN_EXPECT_EQUAL(validation, modulo<unsigned char>(0, 10), (unsigned char)(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<unsigned char>(1, 10), (unsigned char)(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<unsigned char>(9, 10), (unsigned char)(9));
	OCEAN_EXPECT_EQUAL(validation, modulo<unsigned char>(10, 10), (unsigned char)(0));
	OCEAN_EXPECT_EQUAL(validation, modulo<unsigned char>(11, 10), (unsigned char)(1));
	OCEAN_EXPECT_EQUAL(validation, modulo<unsigned char>(55, 10), (unsigned char)(5));
	OCEAN_EXPECT_EQUAL(validation, modulo<unsigned char>(10, 17), (unsigned char)(10));
	OCEAN_EXPECT_EQUAL(validation, modulo<unsigned char>(100, 17), (unsigned char)(15));

	const Timestamp startTimestamp(true);
	do
	{
		for (size_t n = 0; n < 1000; ++n)
		{
			const unsigned int ringSize = RandomI::random(randomGenerator, 1u, 2000u);
			const int value = RandomI::random(randomGenerator, -5000, 5000);

			const unsigned int moduloValue = (unsigned int)modulo(value, int(ringSize));

			if (value >= 0)
			{
				ocean_assert(value % int(ringSize) == int(moduloValue));

				if (value < int(ringSize))
				{
					OCEAN_EXPECT_EQUAL(validation, int(moduloValue), value);
				}
				else
				{
					OCEAN_EXPECT_EQUAL(validation, int(((unsigned int)value / ringSize) * ringSize + moduloValue), value);
				}
			}
			else
			{
				const int factor = int(-value) / int(ringSize) + 1;

				int positiveValue = factor * int(ringSize) + value;
				ocean_assert(positiveValue >= 0);

				OCEAN_EXPECT_EQUAL(validation, positiveValue % int(ringSize), int(moduloValue));
			}
		}

		for (size_t n = 0; n < 1000; ++n)
		{
			const double ringSize = RandomI::random(randomGenerator, 1u, 2000u) / 200.0;
			ocean_assert(ringSize > 0 && ringSize <= 10.0);

			const double value = (double(RandomI::random32()) - double(2147483648)) / double(67108864);
			ocean_assert(value >= -32.0 && value <= 32.0);

			const double moduloValue = modulo(value, ringSize);

			if (value >= 0)
			{
				ocean_assert(std::abs(fmod(value, ringSize) - moduloValue) <= 1e-12);

				if (value < ringSize)
				{
					OCEAN_EXPECT_EQUAL(validation, moduloValue, value);
				}
				else
				{
					OCEAN_EXPECT_TRUE(validation, double(int(value / ringSize)) * ringSize + moduloValue - value <= 1e-12);
				}
			}
			else
			{
				const double absValue = fabs(value);

				if (absValue < ringSize)
				{
					const double testValue = ringSize - absValue;
					ocean_assert(testValue > 0 && testValue < ringSize);

					OCEAN_EXPECT_TRUE(validation, testValue - moduloValue <= 1e-12);
				}
				else
				{
					const double testValue = fmod(ringSize - fmod(absValue, ringSize), ringSize);
					ocean_assert(testValue >= 0 && testValue < ringSize);

					OCEAN_EXPECT_TRUE(validation, testValue - moduloValue <= 1e-12);
				}
			}
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testRingDistance(const double testDuration)
{
	Log::info() << "Ring distance test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_EQUAL(validation, ringDistance(0u, 0u, 10u), 0u);
	OCEAN_EXPECT_EQUAL(validation, ringDistance(0u, 1u, 10u), 1u);
	OCEAN_EXPECT_EQUAL(validation, ringDistance(0u, 2u, 10u), 2u);
	OCEAN_EXPECT_EQUAL(validation, ringDistance(0u, 3u, 10u), 3u);
	OCEAN_EXPECT_EQUAL(validation, ringDistance(0u, 5u, 10u), 5u);
	OCEAN_EXPECT_EQUAL(validation, ringDistance(0u, 6u, 10u), 4u);
	OCEAN_EXPECT_EQUAL(validation, ringDistance(0u, 7u, 10u), 3u);
	OCEAN_EXPECT_EQUAL(validation, ringDistance(0u, 9u, 10u), 1u);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int ringSize = RandomI::random(randomGenerator, 1u, 2000u);
		const unsigned int value0 = RandomI::random(randomGenerator, ringSize - 1u);
		const unsigned int value1 = RandomI::random(randomGenerator, ringSize - 1u);
		ocean_assert(value0 < ringSize);
		ocean_assert(value1 < ringSize);

		const unsigned int result = ringDistance(value0, value1, ringSize);

		OCEAN_EXPECT_TRUE(validation, int(result) <= abs(int(value0) - int(value1)));

		const unsigned int valueLow = value0 <= value1 ? value0 : value1;
		const unsigned int valueHigh = value0 <= value1 ? value1 : value0;
		ocean_assert(valueHigh >= valueLow);
		ocean_assert(valueHigh < ringSize);

		const unsigned int directDistance = valueHigh - valueLow;
		const unsigned int indirectDistance = (ringSize - valueHigh) + valueLow;

		OCEAN_EXPECT_TRUE(validation, !(directDistance < indirectDistance) || directDistance == result);
		OCEAN_EXPECT_TRUE(validation, !(indirectDistance < directDistance) || indirectDistance == result);
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testMirrorValue(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Mirror value test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int size = RandomI::random(randomGenerator, 1u, 2000u);

		const int low = -int(size);
		const int high = int(size * 2);

		for (int n = low; n < 0; ++n)
		{
			const int result = mirrorValue(n, size);
			const int test = abs(n) - 1;

			OCEAN_EXPECT_EQUAL(validation, result, test);
		}

		for (int n = 0; n < int(size); ++n)
		{
			const int result = mirrorValue(n, size);

			OCEAN_EXPECT_EQUAL(validation, result, n);
		}

		for (int n = int(size); n < high; ++n)
		{
			const int result = mirrorValue(n, size);
			const int test = int(size) - (n - int(size)) - 1;

			OCEAN_EXPECT_EQUAL(validation, result, test);
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testDivisionBy2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Division by 2 test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		// 8 bit integers
		{
			const char value = char(RandomI::random(randomGenerator, -128, 127));
			const char value_2 = Utilities::divideBy2(value);

			OCEAN_EXPECT_EQUAL(validation, int(value_2), int(value) / 2);
		}
		{
			const unsigned char value = (unsigned char)RandomI::random(randomGenerator, 255u);
			const unsigned char value_2 = Utilities::divideBy2(value);

			OCEAN_EXPECT_EQUAL(validation, (unsigned int)(value_2), (unsigned int)(value) / 2u);
		}

		// 16 bit integers
		{
			const short value = short(RandomI::random32());
			const short value_2 = Utilities::divideBy2(value);

			OCEAN_EXPECT_EQUAL(validation, int(value_2), int(value) / 2);
		}
		{
			const unsigned short value = (unsigned short)RandomI::random32();
			const unsigned short value_2 = Utilities::divideBy2(value);

			OCEAN_EXPECT_EQUAL(validation, (unsigned int)(value_2), (unsigned int)(value) / 2u);
		}

		// 32 bit integers
		{
			const int value = int(RandomI::random32());
			const int value_2 = Utilities::divideBy2(value);

			OCEAN_EXPECT_EQUAL(validation, (long long)(value_2), (long long)(value) / 2ll);
		}
		{
			const unsigned int value = RandomI::random32();
			const unsigned int value_2 = Utilities::divideBy2(value);

			OCEAN_EXPECT_EQUAL(validation, (unsigned long long)(value_2), (unsigned long long)(value) / 2ull);
		}

		// 64 bit integers
		{
			const long long value = (long long)(RandomI::random64());
			const long long value_2 = Utilities::divideBy2(value);

			OCEAN_EXPECT_EQUAL(validation, (long long)(value_2), (long long)(value) / 2ll);
		}
		{
			const unsigned long long value = RandomI::random64();
			const unsigned long long value_2 = Utilities::divideBy2(value);

			OCEAN_EXPECT_EQUAL(validation, (unsigned long long)(value_2), (unsigned long long)(value) / 2ull);
		}

		// 32 bit floating point
		{
			const int nominator = int(RandomI::random32());
			int denominator = int(RandomI::random32());

			while (denominator == 0)
			{
				denominator = int(RandomI::random32());
			}

			const float value = float(nominator) / float(denominator);
			const float value_2 = Utilities::divideBy2(value);

			OCEAN_EXPECT_EQUAL(validation, value_2, value / 2.0f);
		}

		// 64 bit floating point
		{
			const long long nominator = (long long)(RandomI::random64());
			long long denominator = (long long)(RandomI::random64());

			while (denominator == 0ll)
			{
				denominator = (long long)(RandomI::random64());
			}

			const double value = double(nominator) / double(denominator);
			const double value_2 = Utilities::divideBy2(value);

			OCEAN_EXPECT_EQUAL(validation, value_2, value / 2.0);
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testIsPowerOfTwo(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Is power of two test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_FALSE(validation, Utilities::isPowerOfTwo(0u));

	for (unsigned int n = 0u; n < 32u; ++n)
	{
		OCEAN_EXPECT_TRUE(validation, Utilities::isPowerOfTwo(1u << n));
	}

	const Timestamp startTimestamp(true);

	do
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int value = RandomI::random32();

			unsigned int testBits = 0u;
			unsigned int testValue = value;

			for (unsigned int i = 0u; testBits <= 1u && i < 32u; ++i)
			{
				if (testValue & 0x00000001u)
				{
					testBits++;
				}

				testValue >>= 1u;
			}

			const bool result = Utilities::isPowerOfTwo(value);
			const bool testResult = testBits == 1u;

			OCEAN_EXPECT_EQUAL(validation, result, testResult);
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
