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

	bool allSucceeded = true;

	if (String::toAString(char('4')) != std::string("4"))
	{
		allSucceeded = false;
	}

	if (String::toAString((unsigned char)('t')) != std::string("t"))
	{
		allSucceeded = false;
	}


	if (String::toAString(short(-47)) != std::string("-47"))
	{
		allSucceeded = false;
	}

	if (String::toAString((unsigned short)47) != std::string("47"))
	{
		allSucceeded = false;
	}


	if (String::toAString(int(-48)) != std::string("-48"))
	{
		allSucceeded = false;
	}

	if (String::toAString((unsigned int)48) != std::string("48"))
	{
		allSucceeded = false;
	}


	if (String::toAString((long long)(-49)) != std::string("-49"))
	{
		allSucceeded = false;
	}

	if (String::toAString((unsigned long long)49) != std::string("49"))
	{
		allSucceeded = false;
	}


	if (String::toAString(ptrdiff_t(-50)) != std::string("-50"))
	{
		allSucceeded = false;
	}

	if (String::toAString(size_t(50)) != std::string("50"))
	{
		allSucceeded = false;
	}


	if (String::toAString(39.1f, 1u) != std::string("39.1"))
	{
		allSucceeded = false;
	}

	if (String::toAString(39.23f, 3u) != std::string("39.230"))
	{
		allSucceeded = false;
	}

	if (String::toAString(39.1, 1u) != std::string("39.1"))
	{
		allSucceeded = false;
	}

	if (String::toAString(39.23, 3u) != std::string("39.230"))
	{
		allSucceeded = false;
	}

	// ensuring that extreme large float values do not use fixed notations

	for (unsigned int n = 0u; n < 100u; ++n)
	{
		for (const double sign : {-1.0, 1.0})
		{
			const double value = std::pow(10.0f, float(n)) * sign;

			const std::string valueString = String::toAString(value);

			if (n < 30u && valueString.size() < n)
			{
				allSucceeded = false;
			}

			if (valueString.size() > 40)
			{
				allSucceeded = false;
			}
		}
	}

	for (unsigned int n = 0u; n < 100u; ++n)
	{
		for (const float sign : {-1.0f, 1.0f})
		{
			const float value = powf(10.0, float(n)) * sign;

			const std::string valueString = String::toAString(value);

			if (n < 20u && valueString.size() < n)
			{
				allSucceeded = false;
			}

			if (valueString.size() > 30)
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

bool TestString::testToWString()
{
	Log::info() << "toWString test:";

	bool allSucceeded = true;

	if (String::toWString(char('4')) != std::wstring(L"4"))
	{
		allSucceeded = false;
	}

	if (String::toWString((unsigned char)('t')) != std::wstring(L"t"))
	{
		allSucceeded = false;
	}


	if (String::toWString(short(-47)) != std::wstring(L"-47"))
	{
		allSucceeded = false;
	}

	if (String::toWString((unsigned short)47) != std::wstring(L"47"))
	{
		allSucceeded = false;
	}


	if (String::toWString(int(-48)) != std::wstring(L"-48"))
	{
		allSucceeded = false;
	}

	if (String::toWString((unsigned int)48) != std::wstring(L"48"))
	{
		allSucceeded = false;
	}


	if (String::toWString((long long)(-49)) != std::wstring(L"-49"))
	{
		allSucceeded = false;
	}

	if (String::toWString((unsigned long long)49) != std::wstring(L"49"))
	{
		allSucceeded = false;
	}


	if (String::toWString(ptrdiff_t(-50)) != std::wstring(L"-50"))
	{
		allSucceeded = false;
	}

	if (String::toWString(size_t(50)) != std::wstring(L"50"))
	{
		allSucceeded = false;
	}


	if (String::toWString(39.1f, 1u) != std::wstring(L"39.1"))
	{
		allSucceeded = false;
	}

	if (String::toWString(39.23f, 3u) != std::wstring(L"39.230"))
	{
		allSucceeded = false;
	}

	if (String::toWString(39.1, 1u) != std::wstring(L"39.1"))
	{
		allSucceeded = false;
	}

	if (String::toWString(39.23, 3u) != std::wstring(L"39.230"))
	{
		allSucceeded = false;
	}

	// ensuring that extreme large float values do not use fixed notations

	for (unsigned int n = 0u; n < 100u; ++n)
	{
		for (const double sign : {-1.0, 1.0})
		{
			const double value = std::pow(10.0f, float(n)) * sign;

			const std::wstring valueString = String::toWString(value);

			if (n < 30u && valueString.size() < n)
			{
				allSucceeded = false;
			}

			if (valueString.size() > 40)
			{
				allSucceeded = false;
			}
		}
	}

	for (unsigned int n = 0u; n < 100u; ++n)
	{
		for (const float sign : {-1.0f, 1.0f})
		{
			const float value = powf(10.0, float(n)) * sign;

			const std::wstring valueString = String::toWString(value);

			if (n < 20u && valueString.size() < n)
			{
				allSucceeded = false;
			}

			if (valueString.size() > 30)
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

bool TestString::testIsBoolean()
{
	Log::info() << "isBoolean test:";

	bool allSucceeded = true;

	bool value = false;

	if (!String::isBoolean("true"))
		allSucceeded = false;

	if (!String::isBoolean("false"))
		allSucceeded = false;

	if (String::isBoolean("TRUE"))
		allSucceeded = false;

	if (String::isBoolean("FALSE"))
		allSucceeded = false;

	if (String::isBoolean(" true"))
		allSucceeded = false;

	if (String::isBoolean("false "))
		allSucceeded = false;

	if (String::isBoolean("trUe"))
		allSucceeded = false;

	if (String::isBoolean("1"))
		allSucceeded = false;

	if (String::isBoolean("0"))
		allSucceeded = false;


	if (!String::isBoolean("TRUE", false))
		allSucceeded = false;

	if (!String::isBoolean("FALsE", false))
		allSucceeded = false;

	if (!String::isBoolean("true", false))
		allSucceeded = false;

	if (!String::isBoolean("false", false))
		allSucceeded = false;

	if (!String::isBoolean("TrUe", false))
		allSucceeded = false;

	if (String::isBoolean("TrUe_", false))
		allSucceeded = false;


	if (!String::isBoolean("true", true, true))
		allSucceeded = false;

	if (!String::isBoolean("false", true, true))
		allSucceeded = false;

	if (String::isBoolean("TRUE", true, true))
		allSucceeded = false;

	if (!String::isBoolean("TRUE", false, true))
		allSucceeded = false;

	if (!String::isBoolean("1", true, true))
		allSucceeded = false;

	if (!String::isBoolean("0", true, true))
		allSucceeded = false;

	if (String::isBoolean("5", true, true))
		allSucceeded = false;


	value = false;
	if (!String::isBoolean("true", true, false, &value) || value != true)
		allSucceeded = false;

	value = true;
	if (!String::isBoolean("false", true, false, &value) || value != false)
		allSucceeded = false;

	value = false;
	if (!String::isBoolean("1", true, true, &value) || value != true)
		allSucceeded = false;

	value = true;
	if (!String::isBoolean("0", true, true, &value) || value != false)
		allSucceeded = false;

	if (String::isBoolean("TEST", true, true, &value))
		allSucceeded = false;

	if (String::isBoolean("truE", true, true, &value))
		allSucceeded = false;

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestString::testIsInteger32()
{
	Log::info() << "isInteger32 test:";

	bool allSucceeded = true;

	if (!String::isInteger32("123"))
		allSucceeded = false;

	if (!String::isInteger32("0"))
		allSucceeded = false;

	if (!String::isInteger32("-133"))
		allSucceeded = false;

	if (!String::isInteger32("-0"))
		allSucceeded = false;

	if (!String::isInteger32("+123"))
		allSucceeded = false;

	if (!String::isInteger32("+9"))
		allSucceeded = false;

	if (String::isInteger32("+"))
		allSucceeded = false;

	if (String::isInteger32("-"))
		allSucceeded = false;

	if (String::isInteger32("123+"))
		allSucceeded = false;

	if (String::isInteger32("-123+4"))
		allSucceeded = false;

	if (String::isInteger32("++123"))
		allSucceeded = false;

	if (String::isInteger32("--0"))
		allSucceeded = false;

	if (String::isInteger32("123."))
		allSucceeded = false;

	if (String::isInteger32("123.3"))
		allSucceeded = false;

	if (String::isInteger32(".2"))
		allSucceeded = false;

	if (!String::isInteger32("123"))
		allSucceeded = false;


	if (!String::isInteger32("2147483647"))
		allSucceeded = false;

	if (!String::isInteger32("+2147483647"))
		allSucceeded = false;

	if (!String::isInteger32("+2147483644"))
		allSucceeded = false;

	if (!String::isInteger32("1147483647"))
		allSucceeded = false;

	if (String::isInteger32("2147483648"))
		allSucceeded = false;

	if (!String::isInteger32("-2147483647"))
		allSucceeded = false;

	if (!String::isInteger32("-2147483648"))
		allSucceeded = false;

	if (String::isInteger32("-2147483649"))
		allSucceeded = false;

	if (String::isInteger32("-4147483649"))
		allSucceeded = false;

	int value = 0;
	if (!String::isInteger32("123", &value) || value != 123)
		allSucceeded = false;

	value = 0;
	if (!String::isInteger32("-99", &value) || value != -99)
		allSucceeded = false;

	value = 0;
	if (!String::isInteger32("2147483647", &value) || value != 2147483647)
		allSucceeded = false;

	value = 0;
	if (!String::isInteger32("-2147483648", &value) || value != -2147483647 - 1)
		allSucceeded = false;

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

bool TestString::testIsUnsignedInteger64()
{
	Log::info() << "isUnsignedInteger64 test:";

	bool allSucceeded = true;

	{
		// positive tests

		if (!String::isUnsignedInteger64("0") || !String::isUnsignedInteger64("+0"))
		{
			allSucceeded = false;
		}

		if (!String::isUnsignedInteger64("1") || !String::isUnsignedInteger64("+1"))
		{
			allSucceeded = false;
		}

		if (!String::isUnsignedInteger64("123") || !String::isUnsignedInteger64("+123"))
		{
			allSucceeded = false;
		}

		if (!String::isUnsignedInteger64("1000") || !String::isUnsignedInteger64("+1000"))
		{
			allSucceeded = false;
		}

		if (!String::isUnsignedInteger64("2147483647") || !String::isUnsignedInteger64("+2147483647"))
		{
			allSucceeded = false;
		}

		if (!String::isUnsignedInteger64("2147483647") || !String::isUnsignedInteger64("+2147483647"))
		{
			allSucceeded = false;
		}

		static_assert(std::numeric_limits<uint64_t>::max() == uint64_t(18446744073709551615ull), "Invalid platform");

		if (!String::isUnsignedInteger64("18446744073709551613") || !String::isUnsignedInteger64("+18446744073709551613"))
		{
			allSucceeded = false;
		}

		if (!String::isUnsignedInteger64("18446744073709551614") || !String::isUnsignedInteger64("+18446744073709551614"))
		{
			allSucceeded = false;
		}

		if (!String::isUnsignedInteger64("18446744073709551615") || !String::isUnsignedInteger64("+18446744073709551615"))
		{
			allSucceeded = false;
		}

		for (unsigned int iteration = 0u; iteration < 1000u; ++iteration)
		{
			const unsigned int digits = RandomI::random(1u, 19u);

			std::string value(digits, '0');

			for (char& character : value)
			{
				character = char('0' + RandomI::random(9u));
			}

			if (!String::isUnsignedInteger64(value) || !String::isUnsignedInteger64("+" + value))
			{
				allSucceeded = false;
			}
		}

		for (unsigned int iteration = 0u; iteration < 1000u; ++iteration)
		{
			const std::string valueString = String::toAString(iteration);

			uint64_t parsedValue = 0ull;
			if (!String::isUnsignedInteger64(valueString, &parsedValue) || uint64_t(iteration) != parsedValue)
			{
				allSucceeded = false;
			}

			parsedValue = 0ull;
			if (!String::isUnsignedInteger64("+" + valueString, &parsedValue) || uint64_t(iteration) != parsedValue)
			{
				allSucceeded = false;
			}
		}

		for (unsigned int iteration = 0u; iteration < 1000u; ++iteration)
		{
			const uint64_t value = RandomI::random64();

			const std::string valueString = String::toAString(value);

			uint64_t parsedValue = 0ull;
			if (!String::isUnsignedInteger64(valueString, &parsedValue) || value != parsedValue)
			{
				allSucceeded = false;
			}
		}
	}


	{
		// negative tests

		if (String::isUnsignedInteger64(""))
		{
			allSucceeded = false;
		}

		if (String::isUnsignedInteger64("-0"))
		{
			allSucceeded = false;
		}

		if (String::isUnsignedInteger64("-1"))
		{
			allSucceeded = false;
		}

		if (String::isUnsignedInteger64("-123"))
		{
			allSucceeded = false;
		}

		if (String::isUnsignedInteger64("-1000"))
		{
			allSucceeded = false;
		}

		if (String::isUnsignedInteger64("1.2"))
		{
			allSucceeded = false;
		}

		if (String::isUnsignedInteger64("+1.2"))
		{
			allSucceeded = false;
		}

		for (unsigned int iteration = 0u; iteration < 1000u; ++iteration)
		{
			const unsigned int digits = RandomI::random(1u, 30u);

			std::string value(digits, '0');

			for (char& character : value)
			{
				character = char('0' + RandomI::random(10u));
			}

			const unsigned int invalidCharacters = RandomI::random(1u, digits);

			std::vector<char> invalid;

			for (unsigned int n = 0u; n < invalidCharacters; /*noop*/)
			{
				const unsigned int index = RandomI::random(digits - 1u);
				const char invalidCharacter = char((int('9') + RandomI::random(1, 245)) % 256);

				if (index == 0u && invalidCharacter == '+')
				{
					continue;
				}

				invalid.emplace_back(invalidCharacter);

				value[index] = invalidCharacter;

				++n;
			}

			if (String::isUnsignedInteger64(value))
			{
				allSucceeded = false;
			}
		}

		for (unsigned int iteration = 0u; iteration < 1000u; ++iteration)
		{
			std::string value("18446744073709551615");

			const unsigned int invalidCharacters = RandomI::random(1u, (unsigned int)(value.size()) - 1u);

			for (unsigned int n = 0u; n < invalidCharacters; /*noop*/)
			{
				const unsigned int index = RandomI::random((unsigned int)(value.size()) - 1u);

				if (value[index] == '9')
				{
					continue;
				}
				else
				{
					const int range = int('9') - int(value[index]);
					ocean_assert(range >= 1 && range <= 9);

					value[index] = value[index] + char(RandomI::random(1, range));
				}

				++n;
			}

			if (String::isUnsignedInteger64(value))
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

bool TestString::testIsHexValue64()
{
	Log::info() << "isHexValue64 test:";

	bool allSucceeded = true;

	if (!String::isHexValue64("123", 3))
		allSucceeded = false;

	if (!String::isHexValue64("0", 1))
		allSucceeded = false;

	if (!String::isHexValue64("0x123", 5))
		allSucceeded = false;

	if (!String::isHexValue64("0x0", 3))
		allSucceeded = false;

	if (!String::isHexValue64("0000000000000001", 16))
		allSucceeded = false;

	if (!String::isHexValue64("0x0000000000000001", 18))
		allSucceeded = false;

	if (!String::isHexValue64("abCdef0700feDcbA", 16))
		allSucceeded = false;

	if (!String::isHexValue64("0xaBcdef0700fEdcbA", 18))
		allSucceeded = false;

	if (String::isHexValue64("2347", 4, true))
		allSucceeded = false;

	if (!String::isHexValue64("0x2347", 6, true))
		allSucceeded = false;

	unsigned long long value = 0ull;
	if (!String::isHexValue64("123", 3, false, &value) || value != 291ull)
		allSucceeded = false;

	value = 0ull;
	if (!String::isHexValue64("0x123", 5, false, &value) || value != 291ull)
		allSucceeded = false;

	value = 0ull;
	if (String::isHexValue64("123", 3, true, &value))
		allSucceeded = false;

	value = 0ull;
	if (!String::isHexValue64("0x123", 5, true, &value) || value != 291ull)
		allSucceeded = false;

	value = 0ull;
	if (!String::isHexValue64("12373bcff00abcde", 16, false, &value) || value != 1312583580673293534ull)
		allSucceeded = false;

	value = 0ull;
	if (!String::isHexValue64("12373Bcff00abCDE", 16, false, &value) || value != 1312583580673293534ull)
		allSucceeded = false;

	value = 0ull;
	if (!String::isHexValue64("0x12373bcff00abcde", 18, false, &value) || value != 1312583580673293534ull)
		allSucceeded = false;

	value = 0ull;
	if (String::isHexValue64("12373bcff00abcde", 16, true, &value))
		allSucceeded = false;

	value = 0ull;
	if (!String::isHexValue64("1234567890", 10, false, &value) || value != 78187493520ull)
		allSucceeded = false;

	value = 0ull;
	if (String::isHexValue64("1234567890", 10, true, &value))
		allSucceeded = false;

	value = 0ull;
	if (!String::isHexValue64("0x1234567890", 12, false, &value) || value != 78187493520ull)
		allSucceeded = false;

	value = 0ull;
	if (!String::isHexValue64("0x1234567890", 12, true, &value) || value != 78187493520ull)
		allSucceeded = false;

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

bool TestString::testIsNumber()
{
	Log::info() << "isNumber test:";

	bool allSucceeded = true;

	double value = false;

	if (!String::isNumber("123.0"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("0.0"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("-133.0"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("-0."))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("+123."))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("+9.7"))
	{
		allSucceeded = false;
	}

	if (String::isNumber("+", true) || String::isNumber("+", false))
	{
		allSucceeded = false;
	}

	if (String::isNumber("-", true) || String::isNumber("-", false))
	{
		allSucceeded = false;
	}

	if (String::isNumber(".", true) || String::isNumber(".", false))
	{
		allSucceeded = false;
	}

	if (String::isNumber("a", true) || String::isNumber("a", false))
	{
		allSucceeded = false;
	}

	if (String::isNumber("String", true) || String::isNumber("String", false))
	{
		allSucceeded = false;
	}

	if (String::isNumber("123.0+"))
	{
		allSucceeded = false;
	}

	if (String::isNumber("-123.0+4"))
	{
		allSucceeded = false;
	}

	if (String::isNumber("++123"))
	{
		allSucceeded = false;
	}

	if (String::isNumber("--0"))
	{
		allSucceeded = false;
	}

	if (String::isNumber("123.0.0"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("123.31"))
	{
		allSucceeded = false;
	}

	if (String::isNumber("1231"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber(".2"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("-.1"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("-.1e+10"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("-.1e-10"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("-.1e+1"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("+.1e20"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("-05.1e20"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("1.0E10"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("1.0E+10"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("0.9E-10"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("9E-10"))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("9E0"))
	{
		allSucceeded = false;
	}


	if (!String::isNumber("1231", true))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("12.31", true))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("12.31e-10", true))
	{
		allSucceeded = false;
	}

	if (!String::isNumber("12.31e+101", true))
	{
		allSucceeded = false;
	}

	if (String::isNumber("-12.-31e+101", true))
	{
		allSucceeded = false;
	}


	value = 0;
	if (!String::isNumber("123", true, &value) || value != 123)
	{
		allSucceeded = false;
	}

	value = 0;
	if (!String::isNumber("-99.2", true, &value) || value != -99.2)
	{
		allSucceeded = false;
	}

	value = 0;
	if (!String::isNumber("-99.2", false, &value) || value != -99.2)
	{
		allSucceeded = false;
	}

	value = 0;
	if (!String::isNumber("10.4e+10", false, &value) || value != 10.4e+10)
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

bool TestString::testTrim(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "trimWhitespace(), trimFront(), trimBack() and trim():";

	bool allSucceeded = true;

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
			if (String::trimWhitespace(stringPair.first) != stringPair.second)
			{
				allSucceeded = false;
			}
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
			if (String::trimWhitespace(stringPair.first) != stringPair.second)
			{
				allSucceeded = false;
			}
		}
	}

	const Timestamp startTimestamp(true);

	do
	{
		std::string valueNonWhite;
		const unsigned int nonWhite = RandomI::random(10u);

		for (unsigned int i = 0u; i < nonWhite; ++i)
		{
			valueNonWhite += char('a' + RandomI::random(25u));
		}

		std::string valueFront;
		const unsigned int frontWhite = RandomI::random(5u);

		for (unsigned int i = 0u; i < frontWhite; ++i)
		{
			valueFront += ' ';
		}

		std::string valueBack;
		const unsigned int backWhite = RandomI::random(5u);

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

				if (!front.empty())
				{
					allSucceeded = false;
				}
				if (!back.empty())
				{
					allSucceeded = false;
				}
				if (!both.empty())
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (front != valueNonWhite + valueBack)
				{
					allSucceeded = false;
				}
				if (back != valueFront + valueNonWhite)
				{
					allSucceeded = false;
				}
				if (both != valueNonWhite)
				{
					allSucceeded = false;
				}
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
				character = RandomI::random(whiteCharacters);
			}

			for (char& character : valueBack)
			{
				character = RandomI::random(whiteCharacters);
			}

			size_t firstNullTerminatorIndex = size_t(-1);

			std::string valueNonWhiteButWithNull = valueNonWhite;

			if (!valueNonWhiteButWithNull.empty() && RandomI::boolean())
			{
				for (unsigned int n = 0u; n < 2u; ++n)
				{
					const size_t nullTerminatorIndex = size_t(RandomI::random((unsigned int)(valueNonWhiteButWithNull.size() - 1)));

					valueNonWhiteButWithNull[nullTerminatorIndex] = '\0';

					firstNullTerminatorIndex = std::min(nullTerminatorIndex, firstNullTerminatorIndex);
				}
			}

			const std::string value = valueFront + valueNonWhiteButWithNull + valueBack;

			const std::string whiteBoth = String::trimWhitespace(value);

			if (valueNonWhiteButWithNull.empty())
			{
				if (!whiteBoth.empty())
				{
					allSucceeded = false;
				}
			}
			else
			{
				for (const char& character : whiteBoth)
				{
					if (Ocean::hasElement(whiteCharacters, character))
					{
						allSucceeded = false;
					}
				}

				if (firstNullTerminatorIndex == size_t(-1))
				{
					if (whiteBoth != valueNonWhiteButWithNull)
					{
						allSucceeded = false;
					}
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
						if (!whiteBoth.empty())
						{
							allSucceeded = false;
						}
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

						if (whiteBoth.size() != testValue.size())
						{
							allSucceeded = false;
						}
						else
						{
							if (whiteBoth != testValue)
							{
								allSucceeded = false;
							}
						}
					}
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

bool TestString::testReplace()
{
	Log::info() << "Test String::replace():";

	bool allSucceeded = true;

	for (const bool onlyFirstOccurrence : {true, false})
	{
		if (String::replace("This is a test string", "test", "replace", onlyFirstOccurrence) != "This is a replace string")
		{
			allSucceeded = false;
		}
	}

	for (const bool onlyFirstOccurrence : {true, false})
	{
		if (String::replace("This is a test string", "Test", "replace", onlyFirstOccurrence) != "This is a test string")
		{
			allSucceeded = false;
		}
	}

	if (String::replace("This is a testtest test string", "test", "replace", true) != "This is a replacetest test string")
	{
		allSucceeded = false;
	}
	if (String::replace("This is a testtest test string", "test", "replace", false) != "This is a replacereplace replace string")
	{
		allSucceeded = false;
	}


	if (String::replace("Test is a test string Test", "Test", "Replace", true) != "Replace is a test string Test")
	{
		allSucceeded = false;
	}
	if (String::replace("Test is a test string Test", "Test", "Replace", false) != "Replace is a test string Replace")
	{
		allSucceeded = false;
	}

	for (const bool onlyFirstOccurrence : {true, false})
	{
		if (String::replace("This is a test string", "", "Replace", onlyFirstOccurrence) != "This is a test string")
		{
			allSucceeded = false;
		}
	}

	if (String::replace("This is a testtest test string", "test", "", true) != "This is a test test string")
	{
		allSucceeded = false;
	}
	if (String::replace("This is a testtest test string", "test", "", false) != "This is a   string")
	{
		allSucceeded = false;
	}

	for (const bool onlyFirstOccurrence : {true, false})
	{
		if (String::replace("Test", "Test", "Test", onlyFirstOccurrence) != "Test")
		{
			allSucceeded = false;
		}
	}

	for (const bool onlyFirstOccurrence : {true, false})
	{
		if (String::replace("Test", "Test", "", onlyFirstOccurrence) != "")
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

bool TestString::testToLower()
{
	Log::info() << "Test String::toLower():";

	bool allSucceeded = true;

	constexpr size_t iterations = 1000;

	for (size_t iteration = 0; iteration < iterations; ++iteration)
	{
		const size_t length = size_t(RandomI::random(1u, 100u));

		// testing char string

		std::string input;
		input.reserve(length);

		while (input.size() < length)
		{
			input += char(RandomI::random(1u, 255u));
		}

		const std::string result = String::toLower(input);

		if (result.size() == input.size())
		{
			for (size_t n = 0u; n < length; ++n)
			{
				char expectedChar = input[n];

				if (input[n] >= 'A' && input[n] <= 'Z') // only basic letters are handled
				{
					expectedChar = char(int(input[n]) - int('A') + int('a'));
				}

				if (expectedChar != result[n])
				{
					allSucceeded = false;
				}
			}
		}
		else
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

bool TestString::testToUpper()
{
	Log::info() << "Test String::toUpper():";

	bool allSucceeded = true;

	constexpr size_t iterations = 1000;

	for (size_t iteration = 0; iteration < iterations; ++iteration)
	{
		const size_t length = size_t(RandomI::random(1u, 100u));

		// testing char string

		std::string input;
		input.reserve(length);

		while (input.size() < length)
		{
			input += char(RandomI::random(1u, 255u));
		}

		const std::string result = String::toUpper(input);

		if (result.size() == input.size())
		{
			for (size_t n = 0u; n < length; ++n)
			{
				char expectedChar = input[n];

				if (input[n] >= 'a' && input[n] <= 'z') // only basic letters are handled
				{
					expectedChar = char(int(input[n]) - int('a') + int('A'));
				}

				if (expectedChar != result[n])
				{
					allSucceeded = false;
				}
			}
		}
		else
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

}

}

}
