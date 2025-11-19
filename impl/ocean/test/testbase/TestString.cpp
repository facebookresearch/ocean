/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestString.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Utilities.h"

#include "ocean/test/Validation.h"

#include <cstddef>
#include <cmath>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestString::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   String test:   --";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testToAString() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testToWString() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTrim(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsBoolean() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsInteger32() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsUnsignedInteger64() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsHexValue64() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsNumber() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReplace() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testToLower() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testToUpper() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "String test succeeded.";
	}
	else
	{
		Log::info() << "String test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestString, ToAString)
{
	EXPECT_TRUE(TestString::testToAString());
}

TEST(TestString, ToWString)
{
	EXPECT_TRUE(TestString::testToWString());
}

TEST(TestString, Trim)
{
	EXPECT_TRUE(TestString::testTrim(GTEST_TEST_DURATION));
}

TEST(TestString, IsBoolean)
{
	EXPECT_TRUE(TestString::testIsBoolean());
}

TEST(TestString, IsInteger32)
{
	EXPECT_TRUE(TestString::testIsInteger32());
}

TEST(TestString, IsUnsignedInteger64)
{
	EXPECT_TRUE(TestString::testIsUnsignedInteger64());
}

TEST(TestString, IsHexValue64)
{
	EXPECT_TRUE(TestString::testIsHexValue64());
}

TEST(TestString, IsNumber)
{
	EXPECT_TRUE(TestString::testIsNumber());
}

TEST(TestString, Replace)
{
	EXPECT_TRUE(TestString::testReplace());
}

TEST(TestString, ToLower)
{
	EXPECT_TRUE(TestString::testToLower());
}

TEST(TestString, ToUpper)
{
	EXPECT_TRUE(TestString::testToUpper());
}

#endif // OCEAN_USE_GTEST

bool TestString::testToAString()
{
	Log::info() << "toAString test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_EQUAL(validation, String::toAString(char('4')), std::string("4"));
	OCEAN_EXPECT_EQUAL(validation, String::toAString((unsigned char)('t')), std::string("t"));

	OCEAN_EXPECT_EQUAL(validation, String::toAString(short(-47)), std::string("-47"));
	OCEAN_EXPECT_EQUAL(validation, String::toAString((unsigned short)47), std::string("47"));

	OCEAN_EXPECT_EQUAL(validation, String::toAString(int(-48)), std::string("-48"));
	OCEAN_EXPECT_EQUAL(validation, String::toAString((unsigned int)48), std::string("48"));

	OCEAN_EXPECT_EQUAL(validation, String::toAString((long long)(-49)), std::string("-49"));
	OCEAN_EXPECT_EQUAL(validation, String::toAString((unsigned long long)49), std::string("49"));

	OCEAN_EXPECT_EQUAL(validation, String::toAString(ptrdiff_t(-50)), std::string("-50"));
	OCEAN_EXPECT_EQUAL(validation, String::toAString(size_t(50)), std::string("50"));

	OCEAN_EXPECT_EQUAL(validation, String::toAString(39.1f, 1u), std::string("39.1"));
	OCEAN_EXPECT_EQUAL(validation, String::toAString(39.23f, 3u), std::string("39.230"));
	OCEAN_EXPECT_EQUAL(validation, String::toAString(39.1, 1u), std::string("39.1"));
	OCEAN_EXPECT_EQUAL(validation, String::toAString(39.23, 3u), std::string("39.230"));

	// ensuring that extreme large float values do not use fixed notations

	for (unsigned int n = 0u; n < 100u; ++n)
	{
		for (const double sign : {-1.0, 1.0})
		{
			const double value = std::pow(10.0f, float(n)) * sign;

			const std::string valueString = String::toAString(value);

			if (n < 30u)
			{
				OCEAN_EXPECT_GREATER_EQUAL(validation, valueString.size(), size_t(n));
			}

			OCEAN_EXPECT_LESS_EQUAL(validation, valueString.size(), size_t(40));
		}
	}

	for (unsigned int n = 0u; n < 100u; ++n)
	{
		for (const float sign : {-1.0f, 1.0f})
		{
			const float value = powf(10.0, float(n)) * sign;

			const std::string valueString = String::toAString(value);

			if (n < 20u)
			{
				OCEAN_EXPECT_GREATER_EQUAL(validation, valueString.size(), size_t(n));
			}

			OCEAN_EXPECT_LESS_EQUAL(validation, valueString.size(), size_t(30));
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestString::testToWString()
{
	Log::info() << "toWString test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_EQUAL(validation, String::toWString(char('4')), std::wstring(L"4"));
	OCEAN_EXPECT_EQUAL(validation, String::toWString((unsigned char)('t')), std::wstring(L"t"));

	OCEAN_EXPECT_EQUAL(validation, String::toWString(short(-47)), std::wstring(L"-47"));
	OCEAN_EXPECT_EQUAL(validation, String::toWString((unsigned short)47), std::wstring(L"47"));

	OCEAN_EXPECT_EQUAL(validation, String::toWString(int(-48)), std::wstring(L"-48"));
	OCEAN_EXPECT_EQUAL(validation, String::toWString((unsigned int)48), std::wstring(L"48"));

	OCEAN_EXPECT_EQUAL(validation, String::toWString((long long)(-49)), std::wstring(L"-49"));
	OCEAN_EXPECT_EQUAL(validation, String::toWString((unsigned long long)49), std::wstring(L"49"));

	OCEAN_EXPECT_EQUAL(validation, String::toWString(ptrdiff_t(-50)), std::wstring(L"-50"));
	OCEAN_EXPECT_EQUAL(validation, String::toWString(size_t(50)), std::wstring(L"50"));

	OCEAN_EXPECT_EQUAL(validation, String::toWString(39.1f, 1u), std::wstring(L"39.1"));
	OCEAN_EXPECT_EQUAL(validation, String::toWString(39.23f, 3u), std::wstring(L"39.230"));
	OCEAN_EXPECT_EQUAL(validation, String::toWString(39.1, 1u), std::wstring(L"39.1"));
	OCEAN_EXPECT_EQUAL(validation, String::toWString(39.23, 3u), std::wstring(L"39.230"));

	// ensuring that extreme large float values do not use fixed notations

	for (unsigned int n = 0u; n < 100u; ++n)
	{
		for (const double sign : {-1.0, 1.0})
		{
			const double value = std::pow(10.0f, float(n)) * sign;

			const std::wstring valueString = String::toWString(value);

			if (n < 30u)
			{
				OCEAN_EXPECT_GREATER_EQUAL(validation, valueString.size(), size_t(n));
			}

			OCEAN_EXPECT_LESS_EQUAL(validation, valueString.size(), size_t(40));
		}
	}

	for (unsigned int n = 0u; n < 100u; ++n)
	{
		for (const float sign : {-1.0f, 1.0f})
		{
			const float value = powf(10.0, float(n)) * sign;

			const std::wstring valueString = String::toWString(value);

			if (n < 20u)
			{
				OCEAN_EXPECT_GREATER_EQUAL(validation, valueString.size(), size_t(n));
			}

			OCEAN_EXPECT_LESS_EQUAL(validation, valueString.size(), size_t(30));
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestString::testIsBoolean()
{
	Log::info() << "isBoolean test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	bool value = false;

	OCEAN_EXPECT_TRUE(validation, String::isBoolean("true"));
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("false"));
	OCEAN_EXPECT_FALSE(validation, String::isBoolean("TRUE"));
	OCEAN_EXPECT_FALSE(validation, String::isBoolean("FALSE"));
	OCEAN_EXPECT_FALSE(validation, String::isBoolean(" true"));
	OCEAN_EXPECT_FALSE(validation, String::isBoolean("false "));
	OCEAN_EXPECT_FALSE(validation, String::isBoolean("trUe"));
	OCEAN_EXPECT_FALSE(validation, String::isBoolean("1"));
	OCEAN_EXPECT_FALSE(validation, String::isBoolean("0"));

	OCEAN_EXPECT_TRUE(validation, String::isBoolean("TRUE", false));
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("FALsE", false));
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("true", false));
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("false", false));
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("TrUe", false));
	OCEAN_EXPECT_FALSE(validation, String::isBoolean("TrUe_", false));

	OCEAN_EXPECT_TRUE(validation, String::isBoolean("true", true, true));
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("false", true, true));
	OCEAN_EXPECT_FALSE(validation, String::isBoolean("TRUE", true, true));
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("TRUE", false, true));
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("1", true, true));
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("0", true, true));
	OCEAN_EXPECT_FALSE(validation, String::isBoolean("5", true, true));

	value = false;
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("true", true, false, &value));
	OCEAN_EXPECT_EQUAL(validation, value, true);

	value = true;
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("false", true, false, &value));
	OCEAN_EXPECT_EQUAL(validation, value, false);

	value = false;
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("1", true, true, &value));
	OCEAN_EXPECT_EQUAL(validation, value, true);

	value = true;
	OCEAN_EXPECT_TRUE(validation, String::isBoolean("0", true, true, &value));
	OCEAN_EXPECT_EQUAL(validation, value, false);

	OCEAN_EXPECT_FALSE(validation, String::isBoolean("TEST", true, true, &value));
	OCEAN_EXPECT_FALSE(validation, String::isBoolean("truE", true, true, &value));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestString::testIsInteger32()
{
	Log::info() << "isInteger32 test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, String::isInteger32("123"));
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("0"));
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("-133"));
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("-0"));
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("+123"));
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("+9"));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32("+"));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32("-"));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32("123+"));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32("-123+4"));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32("++123"));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32("--0"));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32("123."));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32("123.3"));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32(".2"));
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("123"));

	OCEAN_EXPECT_TRUE(validation, String::isInteger32("2147483647"));
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("+2147483647"));
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("+2147483644"));
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("1147483647"));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32("2147483648"));
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("-2147483647"));
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("-2147483648"));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32("-2147483649"));
	OCEAN_EXPECT_FALSE(validation, String::isInteger32("-4147483649"));

	int value = 0;
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("123", &value));
	OCEAN_EXPECT_EQUAL(validation, value, 123);

	value = 0;
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("-99", &value));
	OCEAN_EXPECT_EQUAL(validation, value, -99);

	value = 0;
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("2147483647", &value));
	OCEAN_EXPECT_EQUAL(validation, value, 2147483647);

	value = 0;
	OCEAN_EXPECT_TRUE(validation, String::isInteger32("-2147483648", &value));
	OCEAN_EXPECT_EQUAL(validation, value, -2147483647 - 1);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestString::testIsUnsignedInteger64()
{
	Log::info() << "isUnsignedInteger64 test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	{
		// positive tests

		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("0"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("+0"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("1"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("+1"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("123"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("+123"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("1000"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("+1000"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("2147483647"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("+2147483647"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("2147483647"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("+2147483647"));

		static_assert(std::numeric_limits<uint64_t>::max() == uint64_t(18446744073709551615ull), "Invalid platform");

		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("18446744073709551613"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("+18446744073709551613"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("18446744073709551614"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("+18446744073709551614"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("18446744073709551615"));
		OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("+18446744073709551615"));

		for (unsigned int iteration = 0u; iteration < 1000u; ++iteration)
		{
			const unsigned int digits = RandomI::random(randomGenerator, 1u, 19u);

			std::string value(digits, '0');

			for (char& character : value)
			{
				character = char('0' + RandomI::random(randomGenerator, 9u));
			}

			OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64(value));
			OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("+" + value));
		}

		for (unsigned int iteration = 0u; iteration < 1000u; ++iteration)
		{
			const std::string valueString = String::toAString(iteration);

			uint64_t parsedValue = 0ull;
			OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64(valueString, &parsedValue));
			OCEAN_EXPECT_EQUAL(validation, parsedValue, uint64_t(iteration));

			parsedValue = 0ull;
			OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64("+" + valueString, &parsedValue));
			OCEAN_EXPECT_EQUAL(validation, parsedValue, uint64_t(iteration));
		}

		for (unsigned int iteration = 0u; iteration < 1000u; ++iteration)
		{
			const uint64_t value = RandomI::random64(randomGenerator);

			const std::string valueString = String::toAString(value);

			uint64_t parsedValue = 0ull;
			OCEAN_EXPECT_TRUE(validation, String::isUnsignedInteger64(valueString, &parsedValue));
			OCEAN_EXPECT_EQUAL(validation, parsedValue, value);
		}
	}


	{
		// negative tests

		OCEAN_EXPECT_FALSE(validation, String::isUnsignedInteger64(""));
		OCEAN_EXPECT_FALSE(validation, String::isUnsignedInteger64("-0"));
		OCEAN_EXPECT_FALSE(validation, String::isUnsignedInteger64("-1"));
		OCEAN_EXPECT_FALSE(validation, String::isUnsignedInteger64("-123"));
		OCEAN_EXPECT_FALSE(validation, String::isUnsignedInteger64("-1000"));
		OCEAN_EXPECT_FALSE(validation, String::isUnsignedInteger64("1.2"));
		OCEAN_EXPECT_FALSE(validation, String::isUnsignedInteger64("+1.2"));

		for (unsigned int iteration = 0u; iteration < 1000u; ++iteration)
		{
			const unsigned int digits = RandomI::random(randomGenerator, 1u, 30u);

			std::string value(digits, '0');

			for (char& character : value)
			{
				character = char('0' + RandomI::random(randomGenerator, 10u));
			}

			const unsigned int invalidCharacters = RandomI::random(randomGenerator, 1u, digits);

			std::vector<char> invalid;

			for (unsigned int n = 0u; n < invalidCharacters; /*noop*/)
			{
				const unsigned int index = RandomI::random(randomGenerator, digits - 1u);
				const char invalidCharacter = char((int('9') + RandomI::random(randomGenerator, 1, 245)) % 256);

				if (index == 0u && invalidCharacter == '+')
				{
					continue;
				}

				invalid.emplace_back(invalidCharacter);

				value[index] = invalidCharacter;

				++n;
			}

			OCEAN_EXPECT_FALSE(validation, String::isUnsignedInteger64(value));
		}

		for (unsigned int iteration = 0u; iteration < 1000u; ++iteration)
		{
			std::string value("18446744073709551615");

			const unsigned int invalidCharacters = RandomI::random(randomGenerator, 1u, (unsigned int)(value.size()) - 1u);

			for (unsigned int n = 0u; n < invalidCharacters; /*noop*/)
			{
				const unsigned int index = RandomI::random(randomGenerator, (unsigned int)(value.size()) - 1u);

				if (value[index] == '9')
				{
					continue;
				}
				else
				{
					const int range = int('9') - int(value[index]);
					ocean_assert(range >= 1 && range <= 9);

					value[index] = value[index] + char(RandomI::random(randomGenerator, 1, range));
				}

				++n;
			}

			OCEAN_EXPECT_FALSE(validation, String::isUnsignedInteger64(value));
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestString::testIsHexValue64()
{
	Log::info() << "isHexValue64 test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("123", 3));
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0", 1));
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0x123", 5));
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0x0", 3));
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0000000000000001", 16));
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0x0000000000000001", 18));
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("abCdef0700feDcbA", 16));
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0xaBcdef0700fEdcbA", 18));
	OCEAN_EXPECT_FALSE(validation, String::isHexValue64("2347", 4, true));
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0x2347", 6, true));

	unsigned long long value = 0ull;
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("123", 3, false, &value));
	OCEAN_EXPECT_EQUAL(validation, value, 291ull);

	value = 0ull;
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0x123", 5, false, &value));
	OCEAN_EXPECT_EQUAL(validation, value, 291ull);

	value = 0ull;
	OCEAN_EXPECT_FALSE(validation, String::isHexValue64("123", 3, true, &value));

	value = 0ull;
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0x123", 5, true, &value));
	OCEAN_EXPECT_EQUAL(validation, value, 291ull);

	value = 0ull;
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("12373bcff00abcde", 16, false, &value));
	OCEAN_EXPECT_EQUAL(validation, value, 1312583580673293534ull);

	value = 0ull;
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("12373Bcff00abCDE", 16, false, &value));
	OCEAN_EXPECT_EQUAL(validation, value, 1312583580673293534ull);

	value = 0ull;
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0x12373bcff00abcde", 18, false, &value));
	OCEAN_EXPECT_EQUAL(validation, value, 1312583580673293534ull);

	value = 0ull;
	OCEAN_EXPECT_FALSE(validation, String::isHexValue64("12373bcff00abcde", 16, true, &value));

	value = 0ull;
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("1234567890", 10, false, &value));
	OCEAN_EXPECT_EQUAL(validation, value, 78187493520ull);

	value = 0ull;
	OCEAN_EXPECT_FALSE(validation, String::isHexValue64("1234567890", 10, true, &value));

	value = 0ull;
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0x1234567890", 12, false, &value));
	OCEAN_EXPECT_EQUAL(validation, value, 78187493520ull);

	value = 0ull;
	OCEAN_EXPECT_TRUE(validation, String::isHexValue64("0x1234567890", 12, true, &value));
	OCEAN_EXPECT_EQUAL(validation, value, 78187493520ull);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestString::testIsNumber()
{
	Log::info() << "isNumber test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	double value = false;

	OCEAN_EXPECT_TRUE(validation, String::isNumber("123.0"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("0.0"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("-133.0"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("-0."));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("+123."));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("+9.7"));
	OCEAN_EXPECT_FALSE(validation, String::isNumber("+", true) || String::isNumber("+", false));
	OCEAN_EXPECT_FALSE(validation, String::isNumber("-", true) || String::isNumber("-", false));
	OCEAN_EXPECT_FALSE(validation, String::isNumber(".", true) || String::isNumber(".", false));
	OCEAN_EXPECT_FALSE(validation, String::isNumber("a", true) || String::isNumber("a", false));
	OCEAN_EXPECT_FALSE(validation, String::isNumber("String", true) || String::isNumber("String", false));
	OCEAN_EXPECT_FALSE(validation, String::isNumber("123.0+"));
	OCEAN_EXPECT_FALSE(validation, String::isNumber("-123.0+4"));
	OCEAN_EXPECT_FALSE(validation, String::isNumber("++123"));
	OCEAN_EXPECT_FALSE(validation, String::isNumber("--0"));
	OCEAN_EXPECT_FALSE(validation, String::isNumber("123.0.0"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("123.31"));
	OCEAN_EXPECT_FALSE(validation, String::isNumber("1231"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber(".2"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("-.1"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("-.1e+10"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("-.1e-10"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("-.1e+1"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("+.1e20"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("-05.1e20"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("1.0E10"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("1.0E+10"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("0.9E-10"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("9E-10"));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("9E0"));

	OCEAN_EXPECT_TRUE(validation, String::isNumber("1231", true));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("12.31", true));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("12.31e-10", true));
	OCEAN_EXPECT_TRUE(validation, String::isNumber("12.31e+101", true));
	OCEAN_EXPECT_FALSE(validation, String::isNumber("-12.-31e+101", true));

	value = 0;
	OCEAN_EXPECT_TRUE(validation, String::isNumber("123", true, &value));
	OCEAN_EXPECT_EQUAL(validation, value, 123.0);

	value = 0;
	OCEAN_EXPECT_TRUE(validation, String::isNumber("-99.2", true, &value));
	OCEAN_EXPECT_EQUAL(validation, value, -99.2);

	value = 0;
	OCEAN_EXPECT_TRUE(validation, String::isNumber("-99.2", false, &value));
	OCEAN_EXPECT_EQUAL(validation, value, -99.2);

	value = 0;
	OCEAN_EXPECT_TRUE(validation, String::isNumber("10.4e+10", false, &value));
	OCEAN_EXPECT_EQUAL(validation, value, 10.4e+10);

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestString::testTrim(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "trimWhitespace(), trimFront(), trimBack() and trim():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	{
		const StringPairs<char> stringPairs =
		{
			{"", ""},
			{" ", ""},
			{"\t\f\nXY\v\f\r ", "XY"},
			{"Test123", "Test123"},
			{" Test123 ", "Test123"},
			{" \n Test123 \r ", "Test123"},
			{"Test123  \r ", "Test123"},
			{"   Test123", "Test123"},
			{"\t   T2 \v   ", "T2"},
			{"   T    ", "T"},
			{"   T    \t", "T"},
		};

		for (const StringPair<char>& stringPair : stringPairs)
		{
			OCEAN_EXPECT_EQUAL(validation, String::trimWhitespace(stringPair.first), stringPair.second);
		}
	}

	{
		const StringPairs<wchar_t> stringPairs =
		{
			{L"", L""},
			{L" ", L""},
			{L"\t\f\nXY\v\f\r ", L"XY"},
			{L"Test123", L"Test123"},
			{L" Test123 ", L"Test123"},
			{L" \n Test123 \r ", L"Test123"},
			{L"Test123  \r ", L"Test123"},
			{L"   Test123", L"Test123"},
			{L"\t   T2 \v   ", L"T2"},
			{L"   T    ", L"T"},
			{L"   T    \t", L"T"},
		};

		for (const StringPair<wchar_t>& stringPair : stringPairs)
		{
			OCEAN_EXPECT_EQUAL(validation, String::trimWhitespace(stringPair.first), stringPair.second);
		}
	}

	const Timestamp startTimestamp(true);

	do
	{
		std::string valueNonWhite;
		const unsigned int nonWhite = RandomI::random(randomGenerator, 10u);

		for (unsigned int i = 0u; i < nonWhite; ++i)
		{
			valueNonWhite += char('a' + RandomI::random(randomGenerator, 25u));
		}

		std::string valueFront;
		const unsigned int frontWhite = RandomI::random(randomGenerator, 5u);

		for (unsigned int i = 0u; i < frontWhite; ++i)
		{
			valueFront += ' ';
		}

		std::string valueBack;
		const unsigned int backWhite = RandomI::random(randomGenerator, 5u);

		for (unsigned int i = 0u; i < backWhite; ++i)
		{
			valueBack += ' ';
		}

		{
			// testing standard function

			const std::string value = valueFront + valueNonWhite + valueBack;

			const std::string front = String::trimFront(value, ' ');
			const std::string back = String::trimBack(value, ' ');
			const std::string both = String::trim(value, ' ');

			if (valueNonWhite.empty())
			{
				// everything must be empty

				OCEAN_EXPECT_TRUE(validation, front.empty());
				OCEAN_EXPECT_TRUE(validation, back.empty());
				OCEAN_EXPECT_TRUE(validation, both.empty());
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, front, valueNonWhite + valueBack);
				OCEAN_EXPECT_EQUAL(validation, back, valueFront + valueNonWhite);
				OCEAN_EXPECT_EQUAL(validation, both, valueNonWhite);
			}
		}

		{
			// testing white function

			const std::vector<char> whiteCharacters =
			{
				' ',
				'\f',
				'\n',
				'\r',
				'\t',
				'\v',
				'\0'
			};

			for (char& character : valueFront)
			{
				character = RandomI::random(randomGenerator, whiteCharacters);
			}

			for (char& character : valueBack)
			{
				character = RandomI::random(randomGenerator, whiteCharacters);
			}

			size_t firstNullTerminatorIndex = size_t(-1);

			std::string valueNonWhiteButWithNull = valueNonWhite;

			if (!valueNonWhiteButWithNull.empty() && RandomI::boolean(randomGenerator))
			{
				for (unsigned int n = 0u; n < 2u; ++n)
				{
					const size_t nullTerminatorIndex = size_t(RandomI::random(randomGenerator, (unsigned int)(valueNonWhiteButWithNull.size() - 1)));

					valueNonWhiteButWithNull[nullTerminatorIndex] = '\0';

					firstNullTerminatorIndex = std::min(nullTerminatorIndex, firstNullTerminatorIndex);
				}
			}

			const std::string value = valueFront + valueNonWhiteButWithNull + valueBack;

			const std::string whiteBoth = String::trimWhitespace(value);

			if (valueNonWhiteButWithNull.empty())
			{
				OCEAN_EXPECT_TRUE(validation, whiteBoth.empty());
			}
			else
			{
				for (const char& character : whiteBoth)
				{
					OCEAN_EXPECT_FALSE(validation, Ocean::hasElement(whiteCharacters, character));
				}

				if (firstNullTerminatorIndex == size_t(-1))
				{
					OCEAN_EXPECT_EQUAL(validation, whiteBoth, valueNonWhiteButWithNull);
				}
				else
				{
					ocean_assert(!valueNonWhiteButWithNull.empty());

					size_t startIndex = 0;
					while (startIndex < valueNonWhiteButWithNull.size())
					{
						if (valueNonWhiteButWithNull[startIndex] != '\0')
						{
							break;
						}

						++startIndex;
					}

					if (startIndex == valueNonWhiteButWithNull.size())
					{
						OCEAN_EXPECT_TRUE(validation, whiteBoth.empty());
					}
					else
					{
						size_t endIndex = startIndex + 1;
						while (endIndex < valueNonWhiteButWithNull.size())
						{
							if (valueNonWhiteButWithNull[endIndex] == '\0')
							{
								break;
							}

							++endIndex;
						}

						const std::string testValue = valueNonWhiteButWithNull.substr(startIndex, endIndex - startIndex);

						OCEAN_EXPECT_EQUAL(validation, whiteBoth.size(), testValue.size());

						if (whiteBoth.size() == testValue.size())
						{
							OCEAN_EXPECT_EQUAL(validation, whiteBoth, testValue);
						}
					}
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestString::testReplace()
{
	Log::info() << "Test String::replace():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	for (const bool onlyFirstOccurrence : {true, false})
	{
		OCEAN_EXPECT_EQUAL(validation, String::replace("This is a test string", "test", "replace", onlyFirstOccurrence), std::string("This is a replace string"));
	}

	for (const bool onlyFirstOccurrence : {true, false})
	{
		OCEAN_EXPECT_EQUAL(validation, String::replace("This is a test string", "Test", "replace", onlyFirstOccurrence), std::string("This is a test string"));
	}

	OCEAN_EXPECT_EQUAL(validation, String::replace("This is a testtest test string", "test", "replace", true), std::string("This is a replacetest test string"));
	OCEAN_EXPECT_EQUAL(validation, String::replace("This is a testtest test string", "test", "replace", false), std::string("This is a replacereplace replace string"));

	OCEAN_EXPECT_EQUAL(validation, String::replace("Test is a test string Test", "Test", "Replace", true), std::string("Replace is a test string Test"));
	OCEAN_EXPECT_EQUAL(validation, String::replace("Test is a test string Test", "Test", "Replace", false), std::string("Replace is a test string Replace"));

	for (const bool onlyFirstOccurrence : {true, false})
	{
		OCEAN_EXPECT_EQUAL(validation, String::replace("This is a test string", "", "Replace", onlyFirstOccurrence), std::string("This is a test string"));
	}

	OCEAN_EXPECT_EQUAL(validation, String::replace("This is a testtest test string", "test", "", true), std::string("This is a test test string"));
	OCEAN_EXPECT_EQUAL(validation, String::replace("This is a testtest test string", "test", "", false), std::string("This is a   string"));

	for (const bool onlyFirstOccurrence : {true, false})
	{
		OCEAN_EXPECT_EQUAL(validation, String::replace("Test", "Test", "Test", onlyFirstOccurrence), std::string("Test"));
	}

	for (const bool onlyFirstOccurrence : {true, false})
	{
		OCEAN_EXPECT_EQUAL(validation, String::replace("Test", "Test", "", onlyFirstOccurrence), std::string(""));
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestString::testToLower()
{
	Log::info() << "Test String::toLower():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	constexpr size_t iterations = 1000;

	for (size_t iteration = 0; iteration < iterations; ++iteration)
	{
		const size_t length = size_t(RandomI::random(randomGenerator, 1u, 100u));

		// testing char string

		std::string input;
		input.reserve(length);

		while (input.size() < length)
		{
			input += char(RandomI::random(randomGenerator, 1u, 255u));
		}

		const std::string result = String::toLower(input);

		OCEAN_EXPECT_EQUAL(validation, result.size(), input.size());

		if (result.size() == input.size())
		{
			for (size_t n = 0u; n < length; ++n)
			{
				char expectedChar = input[n];

				if (input[n] >= 'A' && input[n] <= 'Z') // only basic letters are handled
				{
					expectedChar = char(int(input[n]) - int('A') + int('a'));
				}

				OCEAN_EXPECT_EQUAL(validation, expectedChar, result[n]);
			}
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestString::testToUpper()
{
	Log::info() << "Test String::toUpper():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	constexpr size_t iterations = 1000;

	for (size_t iteration = 0; iteration < iterations; ++iteration)
	{
		const size_t length = size_t(RandomI::random(randomGenerator, 1u, 100u));

		// testing char string

		std::string input;
		input.reserve(length);

		while (input.size() < length)
		{
			input += char(RandomI::random(randomGenerator, 1u, 255u));
		}

		const std::string result = String::toUpper(input);

		OCEAN_EXPECT_EQUAL(validation, result.size(), input.size());

		if (result.size() == input.size())
		{
			for (size_t n = 0u; n < length; ++n)
			{
				char expectedChar = input[n];

				if (input[n] >= 'a' && input[n] <= 'z') // only basic letters are handled
				{
					expectedChar = char(int(input[n]) - int('a') + int('A'));
				}

				OCEAN_EXPECT_EQUAL(validation, expectedChar, result[n]);
			}
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
