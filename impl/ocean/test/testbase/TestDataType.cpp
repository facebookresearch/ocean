/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestDataType.h"

#include "ocean/base/DataType.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestDataType::test(const double testDuration)
{
	Log::info() << "---   Data type test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testChar() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testIntegerShift() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testArbitraryDataType(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testUnsignedTyper() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testAbsoluteDifferenceValueTyper() && allSucceeded;

	Log::info() << " ";

	allSucceeded = testFloat16(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Data type test succeeded.";
	}
	else
	{
		Log::info() << "Data type test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestDataType, Char)
{
	EXPECT_TRUE(TestDataType::testChar());
}

TEST(TestDataType, IntegerShift)
{
	EXPECT_TRUE(TestDataType::testIntegerShift());
}

TEST(TestDataType, ArbitraryDataType)
{
	EXPECT_TRUE(TestDataType::testArbitraryDataType(GTEST_TEST_DURATION));
}

TEST(TestDataType, UnsignedTyper)
{
	EXPECT_TRUE(TestDataType::testUnsignedTyper());
}

TEST(TestDataType, AbsoluteDifferenceValueTyper)
{
	EXPECT_TRUE(TestDataType::testAbsoluteDifferenceValueTyper());
}

TEST(TestDataType, Float16)
{
	EXPECT_TRUE(TestDataType::testFloat16(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestDataType::testChar()
{
	Log::info() << "Signed and unsigned char test:";

	// this test ensures that the signed and unsigned char are correct
	// some platforms or compilers (e.g. GCC) may use the same data type (e.g. an unsigned char) for both

	static_assert(int(char(-110)) == -110, "Invalid data type!");
	static_assert(int(char(110)) == 110, "Invalid data type!");
	static_assert(int((unsigned char)(-110)) > 0, "Invalid data type!");
	static_assert(int((unsigned char)(110)) > 0, "Invalid data type!");

	bool allSucceeded = true;

	char signedCharA = char(-110);
	char signedCharB = char(110);

	unsigned char unsignedCharA = (unsigned char)(-110);
	unsigned char unsignedCharB = (unsigned char)(110);

	if (int(signedCharA) != -110)
	{
		allSucceeded = false;
	}

	if (int(signedCharB) != 110)
	{
		allSucceeded = false;
	}

	if (int(unsignedCharA) < 0)
	{
		allSucceeded = false;
	}

	if (int(unsignedCharB) < 0)
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Char test: succeeded.";
	}
	else
	{
		Log::info() << "Char test: FAILED!";
	}

	return allSucceeded;
}

bool TestDataType::testIntegerShift()
{
	Log::info() << "Test right shift operator:";

	static_assert(int(int(8) >> 1) == int(4) && int(int(-8) >> 1) == int(-4), "Invalid data type!");

	const int valuePositive = 8;
	const int valueNegative = -8;

	const bool allSucceeded = valuePositive >> 1 == 4 && valueNegative >> 1 == -4;

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

bool TestDataType::testArbitraryDataType(const double testDuration)
{
	Log::info() << "Arbitrary data type test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		for (size_t n = 0; n < 1000; ++n)
		{
			const DataType<unsigned char, 3u>::Type data24_a = {{(unsigned char)RandomI::random(255), (unsigned char)RandomI::random(255), (unsigned char)RandomI::random(255)}};
			const DataType<unsigned char, 3u>::Type data24_b = {{(unsigned char)RandomI::random(255), (unsigned char)RandomI::random(255), (unsigned char)RandomI::random(255)}};

			static_assert(sizeof(data24_a) == sizeof(unsigned char) * 3, "Invalid data type!");

			if (data24_a == data24_b && data24_a != data24_b)
			{
				allSucceeded = false;
			}
		}

		for (size_t n = 0; n < 1000; ++n)
		{
			const DataType<unsigned short, 3u>::Type data48_a = {{(unsigned short)(RandomI::random32()), (unsigned short)(RandomI::random32()), (unsigned short)(RandomI::random32())}};
			const DataType<unsigned short, 3u>::Type data48_b = {{(unsigned short)(RandomI::random32()), (unsigned short)(RandomI::random32()), (unsigned short)(RandomI::random32())}};

			static_assert(sizeof(data48_a) == sizeof(unsigned short) * 3, "Invalid data type!");

			if (data48_a == data48_b && data48_a != data48_b)
			{
				allSucceeded = false;
			}
		}

		for (size_t n = 0; n < 1000; ++n)
		{
			const DataType<unsigned long long, 2u>::Type data128_a = {{RandomI::random64(), RandomI::random64()}};
			const DataType<unsigned long long, 2u>::Type data128_b = {{RandomI::random64(), RandomI::random64()}};

			static_assert(sizeof(data128_a) == sizeof(unsigned long long) * 2, "Invalid data type!");

			if (data128_a == data128_b && data128_a != data128_b)
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

bool TestDataType::testUnsignedTyper()
{
	Log::info() << "UnsignedTyper test:";

	bool allSucceeded = true;

	{
		typedef UnsignedTyper<bool> Typer;

		if (typeid(Typer::Type) != typeid(bool))
		{
			allSucceeded = false;
		}

		if constexpr (Typer::isUnsigned != true)
		{
			allSucceeded = false;
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			allSucceeded = false;
		}
	}

	{
		typedef UnsignedTyper<signed char> Typer;

		if (typeid(Typer::Type) != typeid(unsigned char))
		{
			allSucceeded = false;
		}

		if constexpr (Typer::isUnsigned != false)
		{
			allSucceeded = false;
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			allSucceeded = false;
		}
	}

	{
		typedef UnsignedTyper<unsigned char> Typer;

		if (typeid(Typer::Type) != typeid(unsigned char))
		{
			allSucceeded = false;
		}

		if constexpr (Typer::isUnsigned != true)
		{
			allSucceeded = false;
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			allSucceeded = false;
		}
	}

	{
		typedef UnsignedTyper<short> Typer;

		if (typeid(Typer::Type) != typeid(unsigned short))
		{
			allSucceeded = false;
		}

		if constexpr (Typer::isUnsigned != false)
		{
			allSucceeded = false;
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			allSucceeded = false;
		}
	}

	{
		typedef UnsignedTyper<unsigned short> Typer;

		if (typeid(Typer::Type) != typeid(unsigned short))
		{
			allSucceeded = false;
		}

		if constexpr (Typer::isUnsigned != true)
		{
			allSucceeded = false;
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			allSucceeded = false;
		}
	}

	{
		typedef UnsignedTyper<int> Typer;

		if (typeid(Typer::Type) != typeid(unsigned int))
		{
			allSucceeded = false;
		}

		if constexpr (Typer::isUnsigned != false)
		{
			allSucceeded = false;
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			allSucceeded = false;
		}
	}

	{
		typedef UnsignedTyper<unsigned int> Typer;

		if (typeid(Typer::Type) != typeid(unsigned int))
		{
			allSucceeded = false;
		}

		if constexpr (Typer::isUnsigned != true)
		{
			allSucceeded = false;
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			allSucceeded = false;
		}
	}

	{
		typedef UnsignedTyper<long long> Typer;

		if (typeid(Typer::Type) != typeid(unsigned long long))
		{
			allSucceeded = false;
		}

		if constexpr (Typer::isUnsigned != false)
		{
			allSucceeded = false;
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			allSucceeded = false;
		}
	}

	{
		typedef UnsignedTyper<unsigned long long> Typer;

		if (typeid(Typer::Type) != typeid(unsigned long long))
		{
			allSucceeded = false;
		}

		if constexpr (Typer::isUnsigned != true)
		{
			allSucceeded = false;
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			allSucceeded = false;
		}
	}

	{
		typedef UnsignedTyper<float> Typer;

		if (typeid(Typer::Type) != typeid(float))
		{
			allSucceeded = false;
		}

		if constexpr (Typer::isUnsigned != false)
		{
			allSucceeded = false;
		}

		if constexpr (Typer::hasUnsigned != false)
		{
			allSucceeded = false;
		}
	}

	{
		typedef UnsignedTyper<double> Typer;

		if (typeid(Typer::Type) != typeid(double))
		{
			allSucceeded = false;
		}

		if constexpr (Typer::isUnsigned != false)
		{
			allSucceeded = false;
		}

		if constexpr (Typer::hasUnsigned != false)
		{
			allSucceeded = false;
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

bool TestDataType::testAbsoluteDifferenceValueTyper()
{
	Log::info() << "AbsoluteDifferenceValueTyper test:";

	bool allSucceeded = true;

	/*
	 * Data type:            Absolute difference data type:
	 *
	 * char                  unsigned int
	 * unsigned char         unsigned int
	 *
	 * short                 unsigned long long
	 * unsigned short        unsigned long long
	 * int                   unsigned long long
	 * unsigned int          unsigned long long
	 *
	 * long long             unsigned long long
	 * unsigned long long    unsigned long long
	 *
	 * float                 double
	 * double                double
	 */


	static_assert((std::is_same<AbsoluteDifferenceValueTyper<char>::Type, unsigned int>::value), "Invalid datatype!");
	static_assert((std::is_same<AbsoluteDifferenceValueTyper<unsigned char>::Type, unsigned int>::value), "Invalid datatype!");

	static_assert((std::is_same<AbsoluteDifferenceValueTyper<short>::Type, unsigned long long>::value), "Invalid datatype!");
	static_assert((std::is_same<AbsoluteDifferenceValueTyper<unsigned short>::Type, unsigned long long>::value), "Invalid datatype!");
	static_assert((std::is_same<AbsoluteDifferenceValueTyper<int>::Type, unsigned long long>::value), "Invalid datatype!");
	static_assert((std::is_same<AbsoluteDifferenceValueTyper<unsigned int>::Type, unsigned long long>::value), "Invalid datatype!");

	static_assert((std::is_same<AbsoluteDifferenceValueTyper<float>::Type, double>::value), "Invalid datatype!");
	static_assert((std::is_same<AbsoluteDifferenceValueTyper<double>::Type, double>::value), "Invalid datatype!");


	// although we have validated 'AbsoluteDifferenceValueTyper' during compile time already, we apply the test again at runtime

	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<char>::Type, unsigned int>::value == false)
	{
		allSucceeded = false;
	}
	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<unsigned char>::Type, unsigned int>::value == false)
	{
		allSucceeded = false;
	}

	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<short>::Type, unsigned long long>::value == false)
	{
		allSucceeded = false;
	}
	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<unsigned short>::Type, unsigned long long>::value == false)
	{
		allSucceeded = false;
	}
	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<int>::Type, unsigned long long>::value == false)
	{
		allSucceeded = false;
	}
	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<unsigned int>::Type, unsigned long long>::value == false)
	{
		allSucceeded = false;
	}

	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<float>::Type, double>::value == false)
	{
		allSucceeded = false;
	}
	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<double>::Type, double>::value == false)
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

bool TestDataType::testFloat16(const double testDuration)
{
	Log::info() << "Float16 test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	// testing fixed values

	{
		const float value_0_0_0 = float(Float16(0u, 0u, 0u));
		const float value_1_0_0 = float(Float16(1u, 0u, 0u));

		if (value_0_0_0 != 0.0f || value_1_0_0 != -0.0f)
		{
			allSucceeded = false;
		}
	}

	{
		const float value_0_1_0 = float(Float16(0u, 1u, 0u));
		const float value_1_1_0 = float(Float16(1u, 1u, 0u));

		if (NumericF::isNotEqual(value_0_1_0, 0.000000059604645f) || NumericF::isNotEqual(value_1_1_0, -0.000000059604645f))
		{
			allSucceeded = false;
		}
	}

	{
		const float value_0_341_13 = float(Float16(0u, 341u, 13u));
		const float value_1_341_13 = float(Float16(1u, 341u, 13u));

		if (NumericF::isNotEqual(value_0_341_13, 0.33325195f) || NumericF::isNotEqual(value_1_341_13, -0.33325195f))
		{
			allSucceeded = false;
		}
	}

	{
		const float value_0_1023_14 = float(Float16(0u, 1023u, 14u));
		const float value_1_1023_14 = float(Float16(1u, 1023u, 14u));

		if (NumericF::isNotEqual(value_0_1023_14, 0.99951172f) || NumericF::isNotEqual(value_1_1023_14, -0.99951172f))
		{
			allSucceeded = false;
		}
	}

	{
		const float value_0_0_15 = float(Float16(0u, 0u, 15u));
		const float value_1_0_15 = float(Float16(1u, 0u, 15u));

		if (value_0_0_15 != 1.0f || value_1_0_15 != -1.0f)
		{
			allSucceeded = false;
		}
	}

	{
		const float value_0_0_16 = float(Float16(0u, 0u, 16u));
		const float value_1_0_16 = float(Float16(1u, 0u, 16u));

		if (value_0_0_16 != 2.0f || value_1_0_16 != -2.0f)
		{
			allSucceeded = false;
		}
	}

	do
	{
		{
			// testing inverse

			const uint16_t fraction = uint16_t(RandomI::random(1023u));
			const uint16_t exponent = uint16_t(RandomI::random(31u));

			if (Float16(0u, fraction, exponent) != -Float16(1u, fraction, exponent))
			{
				allSucceeded = false;
			}
		}

		{
			// testing random value

			const uint16_t fraction = uint16_t(RandomI::random(1023u));
			const uint16_t exponent = uint16_t(RandomI::random(31u));

			float positiveValue = 0.0f;

			if (exponent == 0u)
			{
				positiveValue = NumericF::pow(2.0f, -14.0f) * float(fraction) / 1024.0f;
			}
			else if (exponent == 31u)
			{
				positiveValue = std::numeric_limits<float>::infinity();
			}
			else
			{
				positiveValue = NumericF::pow(2.0f, float(exponent) - 15.0f) * (1.0f + float(fraction) / 1024.0f);
			}

			const float positiveResult = float(Float16(0u, fraction, exponent));
			const float negativeResult = float(Float16(1u, fraction, exponent));

			if (NumericF::isNotEqual(positiveResult, positiveValue) || NumericF::isNotEqual(negativeResult, -positiveValue))
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

}

}

}
