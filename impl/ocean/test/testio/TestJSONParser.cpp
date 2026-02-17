/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestJSONParser.h"

#include "ocean/test/TestResult.h"

#include "ocean/base/Timestamp.h"
#include "ocean/base/RandomI.h"

#include "ocean/math/Random.h"

#include "ocean/io/JSONParser.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

std::string TestJSONParser::JSONTestData::randomString(RandomGenerator& randomGenerator, const size_t maxLength)
{
	const size_t length = size_t(RandomI::random(randomGenerator, 0u, (unsigned int)maxLength));

	std::string result;
	result.reserve(length);

	for (size_t i = 0; i < length; ++i)
	{
		// Generate ASCII printable characters, avoiding special JSON characters that need escaping
		const char c = char(RandomI::random(randomGenerator, 32, 126));

		// Escape special JSON characters
		if (c == '"' || c == '\\')
		{
			result += '\\';
		}

		result += c;
	}

	return result;
}

TestJSONParser::JSONTestData TestJSONParser::JSONTestData::randomArray(RandomGenerator& randomGenerator, const unsigned int remainingDepth)
{
	const size_t arraySize = size_t(RandomI::random(randomGenerator, 0u, 5u));

	JSONTestData result;
	result.jsonString_ = "[";

	IO::JSONParser::JSONValue::Array array;

	for (size_t i = 0; i < arraySize; ++i)
	{
		if (i > 0)
		{
			result.jsonString_ += ", ";
		}

		JSONTestData elementData = randomValue(randomGenerator, remainingDepth);
		result.jsonString_ += elementData.jsonString_;
		array.push_back(std::move(elementData.expectedValue_));
	}

	result.jsonString_ += "]";
	result.expectedValue_ = IO::JSONParser::JSONValue(std::move(array));

	return result;
}

TestJSONParser::JSONTestData TestJSONParser::JSONTestData::randomObject(RandomGenerator& randomGenerator, const unsigned int remainingDepth)
{
	const size_t objectSize = size_t(RandomI::random(randomGenerator, 0u, 5u));

	JSONTestData result;
	result.jsonString_ = "{";

	IO::JSONParser::JSONValue::ObjectMap object;

	for (size_t i = 0; i < objectSize; ++i)
	{
		if (i > 0)
		{
			result.jsonString_ += ", ";
		}

		// Generate a random key
		const std::string key = "key_" + String::toAString(i);
		result.jsonString_ += "\"" + key + "\": ";

		// Generate a random value
		JSONTestData valueData = randomValue(randomGenerator, remainingDepth);
		result.jsonString_ += valueData.jsonString_;
		object[key] = std::move(valueData.expectedValue_);
	}

	result.jsonString_ += "}";
	result.expectedValue_ = IO::JSONParser::JSONValue(std::move(object));

	return result;
}

TestJSONParser::JSONTestData TestJSONParser::JSONTestData::randomValue(RandomGenerator& randomGenerator, const unsigned int remainingDepth)
{
	JSONTestData result;

	// At maximum depth, only generate primitives
	if (remainingDepth == 0u)
	{
		const unsigned int primitiveType = RandomI::random(randomGenerator, 0u, 3u);

		switch (primitiveType)
		{
			case 0u:
				result.jsonString_ = "null";
				result.expectedValue_ = IO::JSONParser::JSONValue(nullptr);
				break;

			case 1u:
			{
				const bool boolValue = RandomI::boolean(randomGenerator);
				result.jsonString_ = boolValue ? "true" : "false";
				result.expectedValue_ = IO::JSONParser::JSONValue(boolValue);
				break;
			}

			case 2u:
			{
				const int intValue = RandomI::random(randomGenerator, -1000, 1000);
				result.jsonString_ = String::toAString(intValue);
				result.expectedValue_ = IO::JSONParser::JSONValue(double(intValue));
				break;
			}

			case 3u:
			{
				const std::string stringValue = randomString(randomGenerator);
				result.jsonString_ = "\"" + stringValue + "\"";
				result.expectedValue_ = IO::JSONParser::JSONValue(stringValue);
				break;
			}

			default:
				ocean_assert(false && "This should never happen!");
		}

		return result;
	}

	// Generate any type including nested structures
	const unsigned int valueType = RandomI::random(randomGenerator, 0u, 5u);

	switch (valueType)
	{
		case 0u:
			result.jsonString_ = "null";
			result.expectedValue_ = IO::JSONParser::JSONValue(nullptr);
			break;

		case 1u:
		{
			const bool boolValue = RandomI::boolean(randomGenerator);
			result.jsonString_ = boolValue ? "true" : "false";
			result.expectedValue_ = IO::JSONParser::JSONValue(boolValue);
			break;
		}

		case 2u:
		{
			const double doubleValue = RandomD::scalar(randomGenerator, -10000.0, 10000.0);
			result.jsonString_ = String::toAString(doubleValue);
			result.expectedValue_ = IO::JSONParser::JSONValue(doubleValue);
			break;
		}

		case 3u:
		{
			const std::string stringValue = randomString(randomGenerator);
			result.jsonString_ = "\"" + stringValue + "\"";
			result.expectedValue_ = IO::JSONParser::JSONValue(stringValue);
			break;
		}

		case 4u:
			return randomArray(randomGenerator, remainingDepth - 1u);

		case 5u:
			return randomObject(randomGenerator, remainingDepth - 1u);

		default:
			ocean_assert(false && "This should never happen!");
	}

	return result;
}

bool TestJSONParser::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("JSON Parser test");
	Log::info() << " ";

	if (selector.shouldRun("primitives"))
	{
		testResult = testPrimitives(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("arrays"))
	{
		testResult = testArrays(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("objects"))
	{
		testResult = testObjects();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("nestedstructures"))
	{
		testResult = testNestedStructures();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("errorhandling"))
	{
		testResult = testErrorHandling();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("inputsources"))
	{
		testResult = testInputSources();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("randomjson"))
	{
		testResult = testRandomJSON(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("strictandlenientparsing"))
	{
		testResult = testStrictAndLenientParsing();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestJSONParser, Primitives)
{
	EXPECT_TRUE(TestJSONParser::testPrimitives(GTEST_TEST_DURATION));
}

TEST(TestJSONParser, Arrays)
{
	EXPECT_TRUE(TestJSONParser::testArrays(GTEST_TEST_DURATION));
}

TEST(TestJSONParser, Objects)
{
	EXPECT_TRUE(TestJSONParser::testObjects());
}

TEST(TestJSONParser, NestedStructures)
{
	EXPECT_TRUE(TestJSONParser::testNestedStructures());
}

TEST(TestJSONParser, ErrorHandling)
{
	EXPECT_TRUE(TestJSONParser::testErrorHandling());
}

TEST(TestJSONParser, InputSources)
{
	EXPECT_TRUE(TestJSONParser::testInputSources());
}

TEST(TestJSONParser, RandomJSON)
{
	EXPECT_TRUE(TestJSONParser::testRandomJSON(GTEST_TEST_DURATION));
}

TEST(TestJSONParser, StrictAndLenientParsing)
{
	EXPECT_TRUE(TestJSONParser::testStrictAndLenientParsing());
}

#endif // OCEAN_USE_GTEST

bool TestJSONParser::testPrimitives(const double testDuration)
{
	Log::info() << "Primitives test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	{
		// null

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "null", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isNull());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	{
		// true

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "true", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isBoolean());
		OCEAN_EXPECT_TRUE(validation, value.boolean());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	{
		// false

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "false", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isBoolean());
		OCEAN_EXPECT_FALSE(validation, value.boolean());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	{
		// empty string

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "\"\"", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isString());
		OCEAN_EXPECT_TRUE(validation, value.string().empty());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	const Timestamp startTimestamp(true);

	do
	{
		{
			// integer

			const int testValue = RandomI::random(randomGenerator, -10000, 10000);
			const std::string jsonData = String::toAString(testValue);

			std::string errorMessage;
			IO::JSONParser::JSONValue value = IO::JSONParser::parse("", jsonData, false, &errorMessage);

			OCEAN_EXPECT_TRUE(validation, value.isValid());
			OCEAN_EXPECT_TRUE(validation, value.isNumber());
			OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

			if (value)
			{
				const double parsedValue = value.number();

				OCEAN_EXPECT_EQUAL(validation, NumericD::round32(parsedValue), testValue);
			}
		}

		{
			// floating point

			const double testValue = RandomD::scalar(randomGenerator, -10000.0, 10000.0);
			const std::string jsonData = String::toAString(testValue);

			std::string errorMessage;
			IO::JSONParser::JSONValue value = IO::JSONParser::parse("", jsonData, false, &errorMessage);

			OCEAN_EXPECT_TRUE(validation, value.isValid());
			OCEAN_EXPECT_TRUE(validation, value.isNumber());
			OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

			if (value)
			{
				const double parsedValue = value.number();

				OCEAN_EXPECT_TRUE(validation, NumericD::isEqual(parsedValue, testValue, 0.001));
			}
		}

		{
			// string

			std::string testString = "Hello, JSON World!";

			const unsigned int randomIndex = RandomI::random(randomGenerator, 0u, (unsigned int)(testString.size() - 1u));
			testString[randomIndex] = char('a') + char(RandomI::random(randomGenerator, 0u, 25u));

			const std::string jsonData = "\"" + testString + "\"";

			std::string errorMessage;
			IO::JSONParser::JSONValue value = IO::JSONParser::parse("", jsonData, false, &errorMessage);

			OCEAN_EXPECT_TRUE(validation, value.isValid());
			OCEAN_EXPECT_TRUE(validation, value.isString());
			OCEAN_EXPECT_EQUAL(validation, value.string(), testString);
			OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestJSONParser::testArrays(const double testDuration)
{
	Log::info() << "Arrays test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	{
		// empty array

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[]", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isArray());
		OCEAN_EXPECT_TRUE(validation, value.array().empty());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	{
		// array with multiple elements

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[1, 2, 3, 4, 5]", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isArray());
		OCEAN_EXPECT_EQUAL(validation, value.array().size(), size_t(5));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isArray() && value.array().size() == 5)
		{
			const IO::JSONParser::JSONValue::Array& array = value.array();
			for (size_t i = 0; i < array.size(); ++i)
			{
				OCEAN_EXPECT_TRUE(validation, array[i].isNumber());
				OCEAN_EXPECT_EQUAL(validation, array[i].number(), double(i + 1));
			}
		}
	}

	{
		// array with mixed types

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[null, true, false, 73, \"test\"]", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isArray());
		OCEAN_EXPECT_EQUAL(validation, value.array().size(), size_t(5));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isArray() && value.array().size() == 5)
		{
			const IO::JSONParser::JSONValue::Array& array = value.array();

			OCEAN_EXPECT_TRUE(validation, array[0].isNull());

			OCEAN_EXPECT_TRUE(validation, array[1].isBoolean());
			OCEAN_EXPECT_TRUE(validation, array[1].boolean());

			OCEAN_EXPECT_TRUE(validation, array[2].isBoolean());
			OCEAN_EXPECT_FALSE(validation, array[2].boolean());

			OCEAN_EXPECT_TRUE(validation, array[3].isNumber());
			OCEAN_EXPECT_EQUAL(validation, array[3].number(), 73.0);

			OCEAN_EXPECT_TRUE(validation, array[4].isString());
			OCEAN_EXPECT_EQUAL(validation, array[4].string(), std::string("test"));
		}
	}

	{
		// array with whitespace

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[ 1 , 2 , 3 ]", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isArray());
		OCEAN_EXPECT_EQUAL(validation, value.array().size(), size_t(3));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	const Timestamp startTimestamp(true);

	do
	{
		{
			// array with one element

			const double numberValue = RandomD::scalar(randomGenerator, -10000.0, 10000.0);

			std::string errorMessage;
			IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[" + String::toAString(numberValue, 5u) + "]", false, &errorMessage);

			OCEAN_EXPECT_TRUE(validation, value.isValid());
			OCEAN_EXPECT_TRUE(validation, value.isArray());
			OCEAN_EXPECT_EQUAL(validation, value.array().size(), size_t(1));
			OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

			if (value && value.isArray() && value.array().size() == 1)
			{
				const IO::JSONParser::JSONValue::Array& array = value.array();

				OCEAN_EXPECT_TRUE(validation, array[0].isNumber());
				OCEAN_EXPECT_TRUE(validation, NumericD::isEqual(array[0].number(), numberValue, 0.001));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestJSONParser::testObjects()
{
	Log::info() << "Objects test:";

	Validation validation;

	{
		// empty object

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{}", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_TRUE(validation, value.object().empty());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	{
		// object with single key-value pair

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"key\": \"value\"}", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_EQUAL(validation, value.object().size(), size_t(1));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isObject() && value.object().size() == 1)
		{
			const IO::JSONParser::JSONValue::ObjectMap& object = value.object();
			IO::JSONParser::JSONValue::ObjectMap::const_iterator iKey = object.find("key");

			OCEAN_EXPECT_TRUE(validation, iKey != object.end());

			if (iKey != object.end())
			{
				OCEAN_EXPECT_TRUE(validation, iKey->second.isString());
				OCEAN_EXPECT_EQUAL(validation, iKey->second.string(), std::string("value"));
			}
		}
	}

	{
		// object with multiple key-value pairs

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"name\": \"test\", \"age\": 59, \"active\": true}", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_EQUAL(validation, value.object().size(), size_t(3));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isObject() && value.object().size() == 3)
		{
			const IO::JSONParser::JSONValue::ObjectMap& object = value.object();

			IO::JSONParser::JSONValue::ObjectMap::const_iterator iName = object.find("name");
			OCEAN_EXPECT_TRUE(validation, iName != object.end());

			if (iName != object.end())
			{
				OCEAN_EXPECT_TRUE(validation, iName->second.isString());
				OCEAN_EXPECT_EQUAL(validation, iName->second.string(), std::string("test"));
			}

			IO::JSONParser::JSONValue::ObjectMap::const_iterator iAge = object.find("age");
			OCEAN_EXPECT_TRUE(validation, iAge != object.end());

			if (iAge != object.end())
			{
				OCEAN_EXPECT_TRUE(validation, iAge->second.isNumber());
				OCEAN_EXPECT_EQUAL(validation, iAge->second.number(), 59.0);
			}

			IO::JSONParser::JSONValue::ObjectMap::const_iterator iActive = object.find("active");
			OCEAN_EXPECT_TRUE(validation, iActive != object.end());

			if (iActive != object.end())
			{
				OCEAN_EXPECT_TRUE(validation, iActive->second.isBoolean());
				OCEAN_EXPECT_TRUE(validation, iActive->second.boolean());
			}
		}
	}

	{
		// object with whitespace

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{ \"key\" : \"value\" }", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_EQUAL(validation, value.object().size(), size_t(1));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestJSONParser::testNestedStructures()
{
	Log::info() << "Nested structures test:";

	Validation validation;

	{
		// nested arrays

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[[1, 2], [3, 4], [5, 6]]", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isArray());
		OCEAN_EXPECT_EQUAL(validation, value.array().size(), size_t(3));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isArray() && value.array().size() == 3)
		{
			const IO::JSONParser::JSONValue::Array& array = value.array();
			for (size_t i = 0; i < array.size(); ++i)
			{
				OCEAN_EXPECT_TRUE(validation, array[i].isArray());
				OCEAN_EXPECT_EQUAL(validation, array[i].array().size(), size_t(2));
			}
		}
	}

	{
		// nested objects

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"outer\": {\"inner\": \"value\"}}", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isObject())
		{
			const IO::JSONParser::JSONValue::ObjectMap& object = value.object();

			IO::JSONParser::JSONValue::ObjectMap::const_iterator iOuter = object.find("outer");
			OCEAN_EXPECT_TRUE(validation, iOuter != object.end());

			if (iOuter != object.end())
			{
				OCEAN_EXPECT_TRUE(validation, iOuter->second.isObject());

				if (iOuter->second.isObject())
				{
					const IO::JSONParser::JSONValue::ObjectMap& innerObject = iOuter->second.object();

					IO::JSONParser::JSONValue::ObjectMap::const_iterator iInner = innerObject.find("inner");
					OCEAN_EXPECT_TRUE(validation, iInner != innerObject.end());

					if (iInner != innerObject.end())
					{
						OCEAN_EXPECT_TRUE(validation, iInner->second.isString());
						OCEAN_EXPECT_EQUAL(validation, iInner->second.string(), std::string("value"));
					}
				}
			}
		}
	}

	{
		// array of objects

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[{\"id\": 1}, {\"id\": 2}, {\"id\": 3}]", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isArray());
		OCEAN_EXPECT_EQUAL(validation, value.array().size(), size_t(3));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isArray() && value.array().size() == 3)
		{
			const IO::JSONParser::JSONValue::Array& array = value.array();

			for (size_t i = 0; i < array.size(); ++i)
			{
				OCEAN_EXPECT_TRUE(validation, array[i].isObject());

				if (array[i].isObject())
				{
					const IO::JSONParser::JSONValue::ObjectMap& object = array[i].object();

					IO::JSONParser::JSONValue::ObjectMap::const_iterator iId = object.find("id");
					OCEAN_EXPECT_TRUE(validation, iId != object.end());

					if (iId != object.end())
					{
						OCEAN_EXPECT_TRUE(validation, iId->second.isNumber());
						OCEAN_EXPECT_EQUAL(validation, iId->second.number(), double(i + 1));
					}
				}
			}
		}
	}

	{
		// object with arrays

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"numbers\": [1, 2, 3], \"strings\": [\"a\", \"b\", \"c\"]}", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isObject())
		{
			const IO::JSONParser::JSONValue::ObjectMap& object = value.object();

			IO::JSONParser::JSONValue::ObjectMap::const_iterator iNumbers = object.find("numbers");
			OCEAN_EXPECT_TRUE(validation, iNumbers != object.end());

			if (iNumbers != object.end())
			{
				OCEAN_EXPECT_TRUE(validation, iNumbers->second.isArray());
				OCEAN_EXPECT_EQUAL(validation, iNumbers->second.array().size(), size_t(3));
			}

			IO::JSONParser::JSONValue::ObjectMap::const_iterator iStrings = object.find("strings");
			OCEAN_EXPECT_TRUE(validation, iStrings != object.end());

			if (iStrings != object.end())
			{
				OCEAN_EXPECT_TRUE(validation, iStrings->second.isArray());
				OCEAN_EXPECT_EQUAL(validation, iStrings->second.array().size(), size_t(3));
			}
		}
	}

	{
		// deeply nested structure

		std::string errorMessage;
		const std::string jsonData = R"(
		{
			"level1": {
				"level2": {
					"level3": {
						"value": 48
					}
				}
			}
		}
		)";

		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", jsonData, false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isObject())
		{
			const IO::JSONParser::JSONValue::ObjectMap& object1 = value.object();

			IO::JSONParser::JSONValue::ObjectMap::const_iterator iLevel1 = object1.find("level1");
			OCEAN_EXPECT_TRUE(validation, iLevel1 != object1.end());

			if (iLevel1 != object1.end())
			{
				OCEAN_EXPECT_TRUE(validation, iLevel1->second.isObject());

				if (iLevel1->second.isObject())
				{
					const IO::JSONParser::JSONValue::ObjectMap& object2 = iLevel1->second.object();

					IO::JSONParser::JSONValue::ObjectMap::const_iterator iLevel2 = object2.find("level2");
					OCEAN_EXPECT_TRUE(validation, iLevel2 != object2.end());

					if (iLevel2 != object2.end())
					{
						OCEAN_EXPECT_TRUE(validation, iLevel2->second.isObject());

						if (iLevel2->second.isObject())
						{
							const IO::JSONParser::JSONValue::ObjectMap& object3 = iLevel2->second.object();

							IO::JSONParser::JSONValue::ObjectMap::const_iterator iLevel3 = object3.find("level3");
							OCEAN_EXPECT_TRUE(validation, iLevel3 != object3.end());

							if (iLevel3 != object3.end())
							{
								OCEAN_EXPECT_TRUE(validation, iLevel3->second.isObject());

								if (iLevel3->second.isObject())
								{
									const IO::JSONParser::JSONValue::ObjectMap& object4 = iLevel3->second.object();

									IO::JSONParser::JSONValue::ObjectMap::const_iterator iValue = object4.find("value");
									OCEAN_EXPECT_TRUE(validation, iValue != object4.end());

									if (iValue != object4.end())
									{
										OCEAN_EXPECT_TRUE(validation, iValue->second.isNumber());
										OCEAN_EXPECT_TRUE(validation, NumericD::isEqual(iValue->second.number(), 48.0, 0.001));
									}
								}
							}
						}
					}
				}
			}
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestJSONParser::testErrorHandling()
{
	Log::info() << "Error handling test:";

	Validation validation;

	{
		// invalid JSON (trailing comma in array)

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[1, 2, 3,]", true, &errorMessage);

		OCEAN_EXPECT_FALSE(validation, value.isValid());
		OCEAN_EXPECT_FALSE(validation, errorMessage.empty());
	}

	{
		// invalid JSON (trailing comma in object)

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"key\": \"value\",}", true, &errorMessage);

		OCEAN_EXPECT_FALSE(validation, value.isValid());
		OCEAN_EXPECT_FALSE(validation, errorMessage.empty());
	}

	{
		// invalid JSON (missing closing bracket)

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[1, 2, 3", false, &errorMessage);

		OCEAN_EXPECT_FALSE(validation, value.isValid());
		OCEAN_EXPECT_FALSE(validation, errorMessage.empty());
	}

	{
		// invalid JSON (missing closing brace)

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"key\": \"value\"", false, &errorMessage);

		OCEAN_EXPECT_FALSE(validation, value.isValid());
		OCEAN_EXPECT_FALSE(validation, errorMessage.empty());
	}

	{
		// invalid JSON (missing colon in object)

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"key\" \"value\"}", false, &errorMessage);

		OCEAN_EXPECT_FALSE(validation, value.isValid());
		OCEAN_EXPECT_FALSE(validation, errorMessage.empty());
	}

	{
		// invalid JSON (missing value after colon)

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"key\":}", false, &errorMessage);

		OCEAN_EXPECT_FALSE(validation, value.isValid());
		OCEAN_EXPECT_FALSE(validation, errorMessage.empty());
	}

	{
		// invalid JSON (non-string key in object)

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{33: \"value\"}", false, &errorMessage);

		OCEAN_EXPECT_FALSE(validation, value.isValid());
		OCEAN_EXPECT_FALSE(validation, errorMessage.empty());
	}

	{
		// whitespace-only input

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "   \t\n  ", false, &errorMessage);

		OCEAN_EXPECT_FALSE(validation, value.isValid());
		OCEAN_EXPECT_FALSE(validation, errorMessage.empty());
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestJSONParser::testInputSources()
{
	Log::info() << "Input sources test:";

	Validation validation;

	{
		// parsing from string buffer

		const std::string jsonData = "{\"test\": true}";
		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", jsonData, false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	{
		// parsing from moved string buffer

		std::string jsonData = "{\"test\": false}";
		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", std::move(jsonData), false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	{
		// parsing from stream

		const std::string jsonData = "[1, 2, 3]";
		std::shared_ptr<std::istream> stream = std::make_shared<std::istringstream>(jsonData);

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse(stream, false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isArray());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestJSONParser::testRandomJSON(const double testDuration)
{
	Log::info() << "Random JSON test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int depth = RandomI::random(randomGenerator, 0u, 5u);

		const JSONTestData testData = JSONTestData::randomValue(randomGenerator, depth);

		std::string errorMessage;
		IO::JSONParser::JSONValue parsedValue = IO::JSONParser::parse("", testData.jsonString_, false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, parsedValue.isValid());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (!parsedValue.isValid())
		{
			Log::error() << "Failed to parse generated JSON: " << testData.jsonString_;
			Log::error() << "Error: " << errorMessage;
		}
		else
		{
			if (!compareJSONValues(validation, parsedValue, testData.expectedValue_))
			{
				Log::error() << "Parsed value does not match expected value for JSON: " << testData.jsonString_;
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestJSONParser::testStrictAndLenientParsing()
{
	Log::info() << "Strict and lenient parsing test:";

	Validation validation;

	{
		// Lenient mode: array with trailing comma should succeed

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[1, 2, 3,]", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isArray());
		OCEAN_EXPECT_EQUAL(validation, value.array().size(), size_t(3));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isArray() && value.array().size() == 3)
		{
			const IO::JSONParser::JSONValue::Array& array = value.array();
			OCEAN_EXPECT_EQUAL(validation, array[0].number(), 1.0);
			OCEAN_EXPECT_EQUAL(validation, array[1].number(), 2.0);
			OCEAN_EXPECT_EQUAL(validation, array[2].number(), 3.0);
		}
	}

	{
		// Strict mode: array with trailing comma should fail

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[1, 2, 3,]", true, &errorMessage);

		OCEAN_EXPECT_FALSE(validation, value.isValid());
		OCEAN_EXPECT_FALSE(validation, errorMessage.empty());
	}

	{
		// Lenient mode: object with trailing comma should succeed

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"a\": 1, \"b\": 2,}", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_EQUAL(validation, value.object().size(), size_t(2));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isObject() && value.object().size() == 2)
		{
			const IO::JSONParser::JSONValue::ObjectMap& object = value.object();

			IO::JSONParser::JSONValue::ObjectMap::const_iterator iA = object.find("a");
			OCEAN_EXPECT_TRUE(validation, iA != object.end());
			if (iA != object.end())
			{
				OCEAN_EXPECT_EQUAL(validation, iA->second.number(), 1.0);
			}

			IO::JSONParser::JSONValue::ObjectMap::const_iterator iB = object.find("b");
			OCEAN_EXPECT_TRUE(validation, iB != object.end());
			if (iB != object.end())
			{
				OCEAN_EXPECT_EQUAL(validation, iB->second.number(), 2.0);
			}
		}
	}

	{
		// Strict mode: object with trailing comma should fail

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"a\": 1, \"b\": 2,}", true, &errorMessage);

		OCEAN_EXPECT_FALSE(validation, value.isValid());
		OCEAN_EXPECT_FALSE(validation, errorMessage.empty());
	}

	{
		// Lenient mode: nested structure with trailing commas

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"arr\": [1, 2,], \"obj\": {\"x\": 10,},}", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		if (value && value.isObject())
		{
			const IO::JSONParser::JSONValue::ObjectMap& object = value.object();

			IO::JSONParser::JSONValue::ObjectMap::const_iterator iArr = object.find("arr");
			OCEAN_EXPECT_TRUE(validation, iArr != object.end());
			if (iArr != object.end())
			{
				OCEAN_EXPECT_TRUE(validation, iArr->second.isArray());
				OCEAN_EXPECT_EQUAL(validation, iArr->second.array().size(), size_t(2));
			}

			IO::JSONParser::JSONValue::ObjectMap::const_iterator iObj = object.find("obj");
			OCEAN_EXPECT_TRUE(validation, iObj != object.end());
			if (iObj != object.end())
			{
				OCEAN_EXPECT_TRUE(validation, iObj->second.isObject());
				OCEAN_EXPECT_EQUAL(validation, iObj->second.object().size(), size_t(1));
			}
		}
	}

	{
		// Strict mode: nested structure with trailing commas should fail

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"arr\": [1, 2,], \"obj\": {\"x\": 10,},}", true, &errorMessage);

		OCEAN_EXPECT_FALSE(validation, value.isValid());
		OCEAN_EXPECT_FALSE(validation, errorMessage.empty());
	}

	{
		// Lenient mode (default): trailing comma

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[\"a\", \"b\", \"c\",]", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isArray());
		OCEAN_EXPECT_EQUAL(validation, value.array().size(), size_t(3));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	{
		// Lenient mode: array with only trailing comma

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "[1,]", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isArray());
		OCEAN_EXPECT_EQUAL(validation, value.array().size(), size_t(1));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	{
		// Lenient mode: object with only trailing comma

		std::string errorMessage;
		IO::JSONParser::JSONValue value = IO::JSONParser::parse("", "{\"key\": \"value\",}", false, &errorMessage);

		OCEAN_EXPECT_TRUE(validation, value.isValid());
		OCEAN_EXPECT_TRUE(validation, value.isObject());
		OCEAN_EXPECT_EQUAL(validation, value.object().size(), size_t(1));
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	{
		// Both modes: valid JSON without trailing commas should always succeed

		std::string errorMessage;
		IO::JSONParser::JSONValue strictValue = IO::JSONParser::parse("", "[1, 2, 3]", true, &errorMessage);
		OCEAN_EXPECT_TRUE(validation, strictValue.isValid());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());

		errorMessage.clear();
		IO::JSONParser::JSONValue lenientValue = IO::JSONParser::parse("", "[1, 2, 3]", false, &errorMessage);
		OCEAN_EXPECT_TRUE(validation, lenientValue.isValid());
		OCEAN_EXPECT_TRUE(validation, errorMessage.empty());
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestJSONParser::compareJSONValues(Validation& validation, const IO::JSONParser::JSONValue& parsed, const IO::JSONParser::JSONValue& expected)
{
	if (parsed.isNull() && expected.isNull())
	{
		return true;
	}

	if (parsed.isBoolean() && expected.isBoolean())
	{
		OCEAN_EXPECT_EQUAL(validation, parsed.boolean(), expected.boolean());
		return parsed.boolean() == expected.boolean();
	}

	if (parsed.isNumber() && expected.isNumber())
	{
		OCEAN_EXPECT_TRUE(validation, NumericD::isEqual(parsed.number(), expected.number(), 0.001));
		return NumericD::isEqual(parsed.number(), expected.number(), 0.001);
	}

	if (parsed.isString() && expected.isString())
	{
		OCEAN_EXPECT_EQUAL(validation, parsed.string(), expected.string());
		return parsed.string() == expected.string();
	}

	if (parsed.isArray() && expected.isArray())
	{
		const IO::JSONParser::JSONValue::Array& parsedArray = parsed.array();
		const IO::JSONParser::JSONValue::Array& expectedArray = expected.array();

		OCEAN_EXPECT_EQUAL(validation, parsedArray.size(), expectedArray.size());

		if (parsedArray.size() != expectedArray.size())
		{
			return false;
		}

		for (size_t i = 0; i < parsedArray.size(); ++i)
		{
			if (!compareJSONValues(validation, parsedArray[i], expectedArray[i]))
			{
				return false;
			}
		}

		return true;
	}

	if (parsed.isObject() && expected.isObject())
	{
		const IO::JSONParser::JSONValue::ObjectMap& parsedObject = parsed.object();
		const IO::JSONParser::JSONValue::ObjectMap& expectedObject = expected.object();

		OCEAN_EXPECT_EQUAL(validation, parsedObject.size(), expectedObject.size());

		if (parsedObject.size() != expectedObject.size())
		{
			return false;
		}

		for (IO::JSONParser::JSONValue::ObjectMap::const_iterator expectedIt = expectedObject.begin(); expectedIt != expectedObject.end(); ++expectedIt)
		{
			const IO::JSONParser::JSONValue::ObjectMap::const_iterator parsedIt = parsedObject.find(expectedIt->first);
			OCEAN_EXPECT_TRUE(validation, parsedIt != parsedObject.end());

			if (parsedIt == parsedObject.end())
			{
				return false;
			}

			if (!compareJSONValues(validation, parsedIt->second, expectedIt->second))
			{
				return false;
			}
		}

		return true;
	}

	OCEAN_SET_FAILED(validation);
	return false;
}

} // namespace TestIO

} // namespace Test

} // namespace Ocean
