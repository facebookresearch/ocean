/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestUtilities.h"

#include "ocean/base/Utilities.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestUtilities::test(const double testDuration)
{
	Log::info() << "---   Utilities test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testMinmax(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testModulo(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testRingDistance(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testMirrorValue(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDivisionBy2(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testIsPowerOfTwo(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Utilities test succeeded.";
	else
		Log::info() << "Utilities test FAILED!";

	return allSucceeded;
}
	
#ifdef OCEAN_USE_GTEST
	
TEST(TestUtilities, Minmax) {
	EXPECT_TRUE(TestUtilities::testMinmax(GTEST_TEST_DURATION));
}
	
TEST(TestUtilities, Modulo) {
	EXPECT_TRUE(TestUtilities::testModulo(GTEST_TEST_DURATION));
}

TEST(TestUtilities, RingDistance) {
	EXPECT_TRUE(TestUtilities::testRingDistance(GTEST_TEST_DURATION));
}

TEST(TestUtilities, MirrorValue) {
	EXPECT_TRUE(TestUtilities::testMirrorValue(GTEST_TEST_DURATION));
}

TEST(TestUtilities, DivisionBy2) {
	EXPECT_TRUE(TestUtilities::testDivisionBy2(GTEST_TEST_DURATION));
}

TEST(TestUtilities, IsPowerOfTwo) {
	EXPECT_TRUE(TestUtilities::testIsPowerOfTwo(GTEST_TEST_DURATION));
}
	
#endif // OCEAN_USE_GTEST

bool TestUtilities::testMinmax(const double testDuration)
{
	Log::info() << "Minmax test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		{
			const int value = RandomI::random(-500, 500);
			ocean_assert(value >= -500 && value <= 500);

			const int valueMin = RandomI::random(-1000, 1000);
			const int valueMax = RandomI::random(valueMin, 1000);

			const int result = minmax(valueMin, value, valueMax);

			if (value <= valueMin && result != valueMin)
				allSucceeded = false;
			if (value >= valueMax && result != valueMax)
				allSucceeded = false;
			if (value >= valueMin && value <= valueMax && value != result)
				allSucceeded = false;
		}

		{
			const unsigned int value = RandomI::random(5000u);
			ocean_assert(value >= 0u && value <= 5000u);

			const unsigned int valueMin = RandomI::random(2000u);
			const unsigned int valueMax = RandomI::random(valueMin, 2000u);

			const unsigned int result = minmax(valueMin, value, valueMax);

			if (value <= valueMin && result != valueMin)
				allSucceeded = false;
			if (value >= valueMax && result != valueMax)
				allSucceeded = false;
			if (value >= valueMin && value <= valueMax && value != result)
				allSucceeded = false;
		}

		{
			const float value = float(RandomI::random(-2000, 2000)) / 20.0f;
			ocean_assert(value >= -100.0f && value <= 100.0f);

			const float valueMin = float(RandomI::random(-2000, 2000)) / 2.0f;
			const float valueMax = valueMin + float(RandomI::random(0, 2000)) / 4.0f;

			const float result = minmax(valueMin, value, valueMax);

			if (value <= valueMin && result != valueMin)
				allSucceeded = false;
			if (value >= valueMax && result != valueMax)
				allSucceeded = false;
			if (value >= valueMin && value <= valueMax && value != result)
				allSucceeded = false;
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestUtilities::testModulo(const double testDuration)
{
	Log::info() << "Modulo test:";

	bool allSucceeded = true;

	if (modulo<char>(0, 10) != char(0)
			|| modulo<char>(1, 10) != char(1)
			|| modulo<char>(9, 10) != char(9)
			|| modulo<char>(10, 10) != char(0)
			|| modulo<char>(11, 10) != char(1)
			|| modulo<char>(-1, 10) != char(9)
			|| modulo<char>(-2, 10) != char(8)
			|| modulo<char>(-9, 10) != char(1)
			|| modulo<char>(-10, 10) != char(0)
			|| modulo<char>(-11, 10) != char(9))
		allSucceeded = false;

	if (modulo<short>(0, 10) != short(0)
			|| modulo<short>(1, 10) != short(1)
			|| modulo<short>(9, 10) != short(9)
			|| modulo<short>(10, 10) != short(0)
			|| modulo<short>(11, 10) != short(1)
			|| modulo<short>(-1, 10) != short(9)
			|| modulo<short>(-2, 10) != short(8)
			|| modulo<short>(-9, 10) != short(1)
			|| modulo<short>(-10, 10) != short(0)
			|| modulo<short>(-11, 10) != short(9))
		allSucceeded = false;

	if (modulo<int>(0, 10) != int(0)
			|| modulo<int>(1, 10) != int(1)
			|| modulo<int>(9, 10) != int(9)
			|| modulo<int>(10, 10) != int(0)
			|| modulo<int>(11, 10) != int(1)
			|| modulo<int>(-1, 10) != int(9)
			|| modulo<int>(-2, 10) != int(8)
			|| modulo<int>(-9, 10) != int(1)
			|| modulo<int>(-10, 10) != int(0)
			|| modulo<int>(-11, 10) != int(9))
		allSucceeded = false;

	if (modulo<long long>(0, 10) != (long long)(0)
			|| modulo<long long>(1, 10) != (long long)(1)
			|| modulo<long long>(9, 10) != (long long)(9)
			|| modulo<long long>(10, 10) != (long long)(0)
			|| modulo<long long>(11, 10) != (long long)(1)
			|| modulo<long long>(-1, 10) != (long long)(9)
			|| modulo<long long>(-2, 10) != (long long)(8)
			|| modulo<long long>(-9, 10) != (long long)(1)
			|| modulo<long long>(-10, 10) != (long long)(0)
			|| modulo<long long>(-11, 10) != (long long)(9))
		allSucceeded = false;

	if (modulo<float>(0, 10) != float(0)
			|| modulo<float>(1, 10) != float(1)
			|| modulo<float>(9, 10) != float(9)
			|| modulo<float>(9.4f, 10) != float(9.4f)
			|| modulo<float>(9.5f, 10) != float(9.5f)
			|| modulo<float>(10, 10) != float(0)
			|| modulo<float>(11, 10) != float(1)
			|| modulo<float>(-1, 10) != float(9)
			|| modulo<float>(-1.4f, 10) != float(8.6f)
			|| modulo<float>(-1.5f, 10) != float(8.5f)
			|| modulo<float>(-2, 10) != float(8)
			|| modulo<float>(-9, 10) != float(1)
			|| modulo<float>(-10, 10) != float(0)
			|| modulo<float>(-11, 10) != float(9))
		allSucceeded = false;

	if (modulo<double>(0, 10) != double(0)
			|| modulo<double>(1, 10) != double(1)
			|| modulo<double>(9, 10) != double(9)
			|| modulo<double>(9.4, 10) != double(9.4)
			|| modulo<double>(9.5, 10) != double(9.5)
			|| modulo<double>(10, 10) != double(0)
			|| modulo<double>(11, 10) != double(1)
			|| modulo<double>(-1, 10) != double(9)
			|| modulo<double>(-1.4, 10) != double(8.6)
			|| modulo<double>(-1.5, 10) != double(8.5)
			|| modulo<double>(-2, 10) != double(8)
			|| modulo<double>(-9, 10) != double(1)
			|| modulo<double>(-10, 10) != double(0)
			|| modulo<double>(-11, 10) != double(9))
		allSucceeded = false;

	if (modulo<unsigned char>(0, 10) != (unsigned char)(0)
			|| modulo<unsigned char>(1, 10) != (unsigned char)(1)
			|| modulo<unsigned char>(9, 10) != (unsigned char)(9)
			|| modulo<unsigned char>(10, 10) != (unsigned char)(0)
			|| modulo<unsigned char>(11, 10) != (unsigned char)(1)
			|| modulo<unsigned char>(55, 10) != (unsigned char)(5)
			|| modulo<unsigned char>(10, 17) != (unsigned char)(10)
			|| modulo<unsigned char>(100, 17) != (unsigned char)(15))
		allSucceeded = false;

	const Timestamp startTimestamp(true);
	do
	{
		for (size_t n = 0; n < 1000; ++n)
		{
			const unsigned int ringSize = RandomI::random(1u, 2000u);
			const int value = RandomI::random(-5000, 5000);

			const unsigned int moduloValue = (unsigned int)modulo(value, int(ringSize));

			if (value >= 0)
			{
				ocean_assert(value % int(ringSize) == int(moduloValue));

				if (value < int(ringSize))
				{
					if (int(moduloValue) != value)
						allSucceeded = false;
				}
				else
				{
					if (int(((unsigned int)value / ringSize) * ringSize + moduloValue) != value)
						allSucceeded = false;
				}
			}
			else
			{
				const int factor = int(-value) / int(ringSize) + 1;

				int positiveValue = factor * int(ringSize) + value;
				ocean_assert(positiveValue >= 0);

				if (positiveValue % int(ringSize) != int(moduloValue))
					allSucceeded = false;
			}
		}

		for (size_t n = 0; n < 1000; ++n)
		{
			const double ringSize = RandomI::random(1u, 2000u) / 200.0;
			ocean_assert(ringSize > 0 && ringSize <= 10.0);

			const double value = (double(RandomI::random32()) - double(2147483648)) / double(67108864);
			ocean_assert(value >= -32.0 && value <= 32.0);

			const double moduloValue = modulo(value, ringSize);

			if (value >= 0)
			{
				ocean_assert(std::abs(fmod(value, ringSize) - moduloValue) <= 1e-12);

				if (value < ringSize)
				{
					if (value != moduloValue)
						allSucceeded = false;
				}
				else
				{
					if (double(int(value / ringSize)) * ringSize + moduloValue - value > 1e-12)
						allSucceeded = false;
				}
			}
			else
			{
				const double absValue = fabs(value);

				if (absValue < ringSize)
				{
					const double testValue = ringSize - absValue;
					ocean_assert(testValue > 0 && testValue < ringSize);

					if (testValue - moduloValue > 1e-12)
						allSucceeded = false;
				}
				else
				{
					const double testValue = fmod(ringSize - fmod(absValue, ringSize), ringSize);
					ocean_assert(testValue >= 0 && testValue < ringSize);

					if (testValue - moduloValue > 1e-12)
						allSucceeded = false;
				}
			}
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestUtilities::testRingDistance(const double testDuration)
{
	Log::info() << "Ring distance test:";

	bool allSucceeded = true;

	if (ringDistance(0, 0, 10) != 0
			|| ringDistance(0, 1, 10) != 1
			|| ringDistance(0, 2, 10) != 2
			|| ringDistance(0, 3, 10) != 3
			|| ringDistance(0, 5, 10) != 5
			|| ringDistance(0, 6, 10) != 4
			|| ringDistance(0, 7, 10) != 3
			|| ringDistance(0, 9, 10) != 1)
		allSucceeded = false;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int ringSize = RandomI::random(1u, 2000u);
		const unsigned int value0 = RandomI::random(ringSize - 1u);
		const unsigned int value1 = RandomI::random(ringSize - 1u);
		ocean_assert(value0 < ringSize);
		ocean_assert(value1 < ringSize);

		const unsigned int result = ringDistance(value0, value1, ringSize);

		if (int(result) > abs(int(value0) - int(value1)))
			allSucceeded = false;

		const unsigned int valueLow = value0 <= value1 ? value0 : value1;
		const unsigned int valueHigh = value0 <= value1 ? value1 : value0;
		ocean_assert(valueHigh >= valueLow);
		ocean_assert(valueHigh < ringSize);

		const unsigned int directDistance = valueHigh - valueLow;
		const unsigned int indirectDistance = (ringSize - valueHigh) + valueLow;

		if (directDistance < indirectDistance && directDistance != result)
			allSucceeded = false;
		if (indirectDistance < directDistance && indirectDistance != result)
			allSucceeded = false;
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestUtilities::testMirrorValue(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Mirror value test:";
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int size = RandomI::random(1u, 2000u);

		const int low = -int(size);
		const int high = int(size * 2);

		for (int n = low; n < 0; ++n)
		{
			const int result = mirrorValue(n, size);
			const int test = abs(n) - 1;

			if (result != test)
				allSucceeded = false;
		}

		for (int n = 0; n < int(size); ++n)
		{
			const int result = mirrorValue(n, size);

			if (result != n)
				allSucceeded = false;
		}

		for (int n = int(size); n < high; ++n)
		{
			const int result = mirrorValue(n, size);
			const int test = int(size) - (n - int(size)) - 1;

			if (result != test)
				allSucceeded = false;
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestUtilities::testDivisionBy2(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Division by 2 test:";
	bool allSucceeded = true;

	const Timestamp startTimestamp(true);
	do
	{
		// 8 bit integers
		{
			const char value = char(RandomI::random(-128, 127));
			const char value_2 = Utilities::divideBy2(value);

			if (int(value_2) != int(value) / 2)
				allSucceeded = false;
		}
		{
			const unsigned char value = (unsigned char)RandomI::random(255u);
			const unsigned char value_2 = Utilities::divideBy2(value);

			if ((unsigned int)(value_2) != (unsigned int)(value) / 2u)
				allSucceeded = false;
		}

		// 16 bit integers
		{
			const short value = short(RandomI::random32());
			const short value_2 = Utilities::divideBy2(value);

			if (int(value_2) != int(value) / 2)
				allSucceeded = false;
		}
		{
			const unsigned short value = (unsigned short)RandomI::random32();
			const unsigned short value_2 = Utilities::divideBy2(value);

			if ((unsigned int)(value_2) != (unsigned int)(value) / 2u)
				allSucceeded = false;
		}

		// 32 bit integers
		{
			const int value = int(RandomI::random32());
			const int value_2 = Utilities::divideBy2(value);

			if ((long long)(value_2) != (long long)(value) / 2ll)
				allSucceeded = false;
		}
		{
			const unsigned int value = RandomI::random32();
			const unsigned int value_2 = Utilities::divideBy2(value);

			if ((unsigned long long)(value_2) != (unsigned long long)(value) / 2ull)
				allSucceeded = false;
		}

		// 64 bit integers
		{
			const long long value = (long long)(RandomI::random64());
			const long long value_2 = Utilities::divideBy2(value);

			if ((long long)(value_2) != (long long)(value) / 2ll)
				allSucceeded = false;
		}
		{
			const unsigned long long value = RandomI::random64();
			const unsigned long long value_2 = Utilities::divideBy2(value);

			if ((unsigned long long)(value_2) != (unsigned long long)(value) / 2ull)
				allSucceeded = false;
		}

		// 32 bit floating point
		{
			const int nominator = int(RandomI::random32());
			int denominator = int(RandomI::random32());

			while (denominator == 0)
				denominator = int(RandomI::random32());

			const float value = float(nominator) / float(denominator);
			const float value_2 = Utilities::divideBy2(value);

			if (value_2 != value / 2.0f)
				allSucceeded = false;
		}

		// 64 bit floating point
		{
			const long long nominator = (long long)(RandomI::random64());
			long long denominator = (long long)(RandomI::random64());

			while (denominator == 0ll)
				denominator = (long long)(RandomI::random64());

			const double value = double(nominator) / double(denominator);
			const double value_2 = Utilities::divideBy2(value);

			if (value_2 != value / 2.0)
				allSucceeded = false;
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestUtilities::testIsPowerOfTwo(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Is power of two test:";
	bool allSucceeded = true;

	if (Utilities::isPowerOfTwo(0u))
		allSucceeded = false;

	for (unsigned int n = 0u; n < 32u; ++n)
		if (!Utilities::isPowerOfTwo(1u << n))
			allSucceeded = false;

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
					testBits++;

				testValue >>= 1u;
			}
		
			const bool result = Utilities::isPowerOfTwo(value);
			const bool testResult = testBits == 1u;

			if (result != testResult)
				allSucceeded = false;
		}
	}
	while (Timestamp(true) < startTimestamp + testDuration);

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

}

}

}
