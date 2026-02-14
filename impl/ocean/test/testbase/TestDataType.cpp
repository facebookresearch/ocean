/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestDataType.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

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

bool TestDataType::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("Data type test");
	Log::info() << " ";

	if (selector.shouldRun("char"))
	{
		testResult = testChar();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("integershift"))
	{
		testResult = testIntegerShift();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("arbitrarydatatype"))
	{
		testResult = testArbitraryDataType(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("unsignedtyper"))
	{
		testResult = testUnsignedTyper();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("absolutedifferencevaluetyper"))
	{
		testResult = testAbsoluteDifferenceValueTyper();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("float16"))
	{
		testResult = testFloat16(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	Validation validation;

	char signedCharA = char(-110);
	char signedCharB = char(110);

	unsigned char unsignedCharA = (unsigned char)(-110);
	unsigned char unsignedCharB = (unsigned char)(110);

	OCEAN_EXPECT_EQUAL(validation, int(signedCharA), -110);

	OCEAN_EXPECT_EQUAL(validation, int(signedCharB), 110);

	OCEAN_EXPECT_GREATER_EQUAL(validation, int(unsignedCharA), 0);

	OCEAN_EXPECT_GREATER_EQUAL(validation, int(unsignedCharB), 0);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataType::testIntegerShift()
{
	Log::info() << "Test right shift operator:";

	static_assert(int(int(8) >> 1) == int(4) && int(int(-8) >> 1) == int(-4), "Invalid data type!");

	const int valuePositive = 8;
	const int valueNegative = -8;

	Validation validation;

	OCEAN_EXPECT_EQUAL(validation, valuePositive >> 1, 4);
	OCEAN_EXPECT_EQUAL(validation, valueNegative >> 1, -4);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataType::testArbitraryDataType(const double testDuration)
{
	Log::info() << "Arbitrary data type test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		for (size_t n = 0; n < 1000; ++n)
		{
			const unsigned char data24_a_0 = (unsigned char)RandomI::random(randomGenerator, 255u);
			const unsigned char data24_a_1 = (unsigned char)RandomI::random(randomGenerator, 255u);
			const unsigned char data24_a_2 = (unsigned char)RandomI::random(randomGenerator, 255u);
			const DataType<unsigned char, 3u>::Type data24_a = {{data24_a_0, data24_a_1, data24_a_2}};

			const unsigned char data24_b_0 = (unsigned char)RandomI::random(randomGenerator, 255u);
			const unsigned char data24_b_1 = (unsigned char)RandomI::random(randomGenerator, 255u);
			const unsigned char data24_b_2 = (unsigned char)RandomI::random(randomGenerator, 255u);
			const DataType<unsigned char, 3u>::Type data24_b = {{data24_b_0, data24_b_1, data24_b_2}};

			static_assert(sizeof(data24_a) == sizeof(unsigned char) * 3, "Invalid data type!");

			OCEAN_EXPECT_FALSE(validation, data24_a == data24_b && data24_a != data24_b);
		}

		for (size_t n = 0; n < 1000; ++n)
		{
			const unsigned short data48_a_0 = (unsigned short)(RandomI::random32(randomGenerator));
			const unsigned short data48_a_1 = (unsigned short)(RandomI::random32(randomGenerator));
			const unsigned short data48_a_2 = (unsigned short)(RandomI::random32(randomGenerator));
			const DataType<unsigned short, 3u>::Type data48_a = {{data48_a_0, data48_a_1, data48_a_2}};

			const unsigned short data48_b_0 = (unsigned short)(RandomI::random32(randomGenerator));
			const unsigned short data48_b_1 = (unsigned short)(RandomI::random32(randomGenerator));
			const unsigned short data48_b_2 = (unsigned short)(RandomI::random32(randomGenerator));
			const DataType<unsigned short, 3u>::Type data48_b = {{data48_b_0, data48_b_1, data48_b_2}};

			static_assert(sizeof(data48_a) == sizeof(unsigned short) * 3, "Invalid data type!");

			OCEAN_EXPECT_FALSE(validation, data48_a == data48_b && data48_a != data48_b);
		}

		for (size_t n = 0; n < 1000; ++n)
		{
			const unsigned long long data128_a_0 = RandomI::random64(randomGenerator);
			const unsigned long long data128_a_1 = RandomI::random64(randomGenerator);
			const DataType<unsigned long long, 2u>::Type data128_a = {{data128_a_0, data128_a_1}};

			const unsigned long long data128_b_0 = RandomI::random64(randomGenerator);
			const unsigned long long data128_b_1 = RandomI::random64(randomGenerator);
			const DataType<unsigned long long, 2u>::Type data128_b = {{data128_b_0, data128_b_1}};

			static_assert(sizeof(data128_a) == sizeof(unsigned long long) * 2, "Invalid data type!");

			OCEAN_EXPECT_FALSE(validation, data128_a == data128_b && data128_a != data128_b);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataType::testUnsignedTyper()
{
	Log::info() << "UnsignedTyper test:";

	Validation validation;

	{
		using Typer = UnsignedTyper<bool>;

		OCEAN_EXPECT_TRUE(validation, typeid(Typer::Type) == typeid(bool));

		if constexpr (Typer::isUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	{
		using Typer = UnsignedTyper<signed char>;

		OCEAN_EXPECT_TRUE(validation, typeid(Typer::Type) == typeid(unsigned char));

		if constexpr (Typer::isUnsigned != false)
		{
			OCEAN_SET_FAILED(validation);
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	{
		using Typer = UnsignedTyper<unsigned char>;

		OCEAN_EXPECT_TRUE(validation, typeid(Typer::Type) == typeid(unsigned char));

		if constexpr (Typer::isUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	{
		using Typer = UnsignedTyper<short>;

		OCEAN_EXPECT_TRUE(validation, typeid(Typer::Type) == typeid(unsigned short));

		if constexpr (Typer::isUnsigned != false)
		{
			OCEAN_SET_FAILED(validation);
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	{
		using Typer = UnsignedTyper<unsigned short>;

		OCEAN_EXPECT_TRUE(validation, typeid(Typer::Type) == typeid(unsigned short));

		if constexpr (Typer::isUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	{
		using Typer = UnsignedTyper<int>;

		OCEAN_EXPECT_TRUE(validation, typeid(Typer::Type) == typeid(unsigned int));

		if constexpr (Typer::isUnsigned != false)
		{
			OCEAN_SET_FAILED(validation);
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	{
		using Typer = UnsignedTyper<unsigned int>;

		OCEAN_EXPECT_TRUE(validation, typeid(Typer::Type) == typeid(unsigned int));

		if constexpr (Typer::isUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	{
		using Typer = UnsignedTyper<long long>;

		OCEAN_EXPECT_TRUE(validation, typeid(Typer::Type) == typeid(unsigned long long));

		if constexpr (Typer::isUnsigned != false)
		{
			OCEAN_SET_FAILED(validation);
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	{
		using Typer = UnsignedTyper<unsigned long long>;

		OCEAN_EXPECT_TRUE(validation, typeid(Typer::Type) == typeid(unsigned long long));

		if constexpr (Typer::isUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}

		if constexpr (Typer::hasUnsigned != true)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	{
		using Typer = UnsignedTyper<float>;

		OCEAN_EXPECT_TRUE(validation, typeid(Typer::Type) == typeid(float));

		if constexpr (Typer::isUnsigned != false)
		{
			OCEAN_SET_FAILED(validation);
		}

		if constexpr (Typer::hasUnsigned != false)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	{
		using Typer = UnsignedTyper<double>;

		OCEAN_EXPECT_TRUE(validation, typeid(Typer::Type) == typeid(double));

		if constexpr (Typer::isUnsigned != false)
		{
			OCEAN_SET_FAILED(validation);
		}

		if constexpr (Typer::hasUnsigned != false)
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataType::testAbsoluteDifferenceValueTyper()
{
	Log::info() << "AbsoluteDifferenceValueTyper test:";

	Validation validation;

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
		OCEAN_SET_FAILED(validation);
	}
	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<unsigned char>::Type, unsigned int>::value == false)
	{
		OCEAN_SET_FAILED(validation);
	}

	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<short>::Type, unsigned long long>::value == false)
	{
		OCEAN_SET_FAILED(validation);
	}
	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<unsigned short>::Type, unsigned long long>::value == false)
	{
		OCEAN_SET_FAILED(validation);
	}
	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<int>::Type, unsigned long long>::value == false)
	{
		OCEAN_SET_FAILED(validation);
	}
	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<unsigned int>::Type, unsigned long long>::value == false)
	{
		OCEAN_SET_FAILED(validation);
	}

	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<float>::Type, double>::value == false)
	{
		OCEAN_SET_FAILED(validation);
	}
	if constexpr (std::is_same<AbsoluteDifferenceValueTyper<double>::Type, double>::value == false)
	{
		OCEAN_SET_FAILED(validation);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestDataType::testFloat16(const double testDuration)
{
	Log::info() << "Float16 test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	// testing fixed values

	{
		const float value_0_0_0 = float(Float16(0u, 0u, 0u));
		const float value_1_0_0 = float(Float16(1u, 0u, 0u));

		OCEAN_EXPECT_EQUAL(validation, value_0_0_0, 0.0f);
		OCEAN_EXPECT_EQUAL(validation, value_1_0_0, -0.0f);
	}

	{
		const float value_0_1_0 = float(Float16(0u, 1u, 0u));
		const float value_1_1_0 = float(Float16(1u, 1u, 0u));

		OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(value_0_1_0, 0.000000059604645f));
		OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(value_1_1_0, -0.000000059604645f));
	}

	{
		const float value_0_341_13 = float(Float16(0u, 341u, 13u));
		const float value_1_341_13 = float(Float16(1u, 341u, 13u));

		OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(value_0_341_13, 0.33325195f));
		OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(value_1_341_13, -0.33325195f));
	}

	{
		const float value_0_1023_14 = float(Float16(0u, 1023u, 14u));
		const float value_1_1023_14 = float(Float16(1u, 1023u, 14u));

		OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(value_0_1023_14, 0.99951172f));
		OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(value_1_1023_14, -0.99951172f));
	}

	{
		const float value_0_0_15 = float(Float16(0u, 0u, 15u));
		const float value_1_0_15 = float(Float16(1u, 0u, 15u));

		OCEAN_EXPECT_EQUAL(validation, value_0_0_15, 1.0f);
		OCEAN_EXPECT_EQUAL(validation, value_1_0_15, -1.0f);
	}

	{
		const float value_0_0_16 = float(Float16(0u, 0u, 16u));
		const float value_1_0_16 = float(Float16(1u, 0u, 16u));

		OCEAN_EXPECT_EQUAL(validation, value_0_0_16, 2.0f);
		OCEAN_EXPECT_EQUAL(validation, value_1_0_16, -2.0f);
	}

	do
	{
		{
			// testing inverse

			const uint16_t fraction = uint16_t(RandomI::random(randomGenerator, 1023u));
			const uint16_t exponent = uint16_t(RandomI::random(randomGenerator, 31u));

			OCEAN_EXPECT_EQUAL(validation, Float16(0u, fraction, exponent), -Float16(1u, fraction, exponent));
		}

		{
			// testing random value

			const uint16_t fraction = uint16_t(RandomI::random(randomGenerator, 1023u));
			const uint16_t exponent = uint16_t(RandomI::random(randomGenerator, 31u));

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

			if (exponent == 31u)
			{
				OCEAN_EXPECT_EQUAL(validation, positiveResult, positiveValue);
				OCEAN_EXPECT_EQUAL(validation, negativeResult, -positiveValue);
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(positiveResult, positiveValue));
				OCEAN_EXPECT_TRUE(validation, NumericF::isEqual(negativeResult, -positiveValue));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
