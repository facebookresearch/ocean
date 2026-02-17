/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestValue.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestValue::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("Value test");
	Log::info() << " ";

	if (selector.shouldRun("constructor"))
	{
		testResult = testConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("comparison"))
	{
		testResult = testComparison(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("copy"))
	{
		testResult = testCopy(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("move"))
	{
		testResult = testMove(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("readwrite"))
	{
		testResult = testReadWrite(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestValue, Constructor)
{
	EXPECT_TRUE(TestValue::testConstructor(GTEST_TEST_DURATION));
}

TEST(TestValue, Comparison)
{
	EXPECT_TRUE(TestValue::testComparison(GTEST_TEST_DURATION));
}

TEST(TestValue, Copy)
{
	EXPECT_TRUE(TestValue::testCopy(GTEST_TEST_DURATION));
}

TEST(TestValue, Move)
{
	EXPECT_TRUE(TestValue::testMove(GTEST_TEST_DURATION));
}

TEST(TestValue, ReadWrite)
{
	EXPECT_TRUE(TestValue::testReadWrite(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestValue::testConstructor(const double testDuration)
{
	Log::info() << "Constructor test:";

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// invalid value

			const Value value;

			OCEAN_EXPECT_TRUE(validation, !value);
			OCEAN_EXPECT_TRUE(validation, value.isNull());

			OCEAN_EXPECT_FALSE(validation, value.isBool() || value.isInt() || value.isInt64() || value.isFloat() || value.isFloat64() || value.isString() || value.isBuffer());
		}

		{
			// bool value

			const bool boolValue = RandomI::boolean(randomGenerator);

			const Value value(boolValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(value, boolValue));
		}

		{
			// int32 value

			const int32_t intValue = RandomI::random(randomGenerator, -1000, 1000);

			const Value value(intValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(value, intValue));
		}

		{
			// int64 value

			const int64_t intValue64 = RandomI::random(randomGenerator, -1000, 1000);

			const Value value(intValue64);

			OCEAN_EXPECT_TRUE(validation, verifyValue(value, intValue64));
		}

		{
			// float32 value

			const float floatValue = float(RandomI::random(randomGenerator, -1000, 1000));

			const Value value(floatValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(value, floatValue));
		}

		{
			// float64 value

			const double floatValue64 = double(RandomI::random(randomGenerator, -1000, 1000));

			const Value value(floatValue64);

			OCEAN_EXPECT_TRUE(validation, verifyValue(value, floatValue64));
		}

		{
			// string value

			const unsigned int stringLength = RandomI::random(randomGenerator, 1u, 100u);
			const std::string stringValue(stringLength, char(RandomI::random(randomGenerator, int('a'), int('z'))));

			const Value value = RandomI::boolean(randomGenerator) ? Value(stringValue) : Value(stringValue.c_str());

			OCEAN_EXPECT_TRUE(validation, verifyValue(value, stringValue));
		}

		{
			// empty string value

			const std::string stringValue;

			const Value value(stringValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(value, stringValue));
		}

		{
			// null string

			const char* stringValue = nullptr;

			const Value value(stringValue);

			OCEAN_EXPECT_TRUE(validation, !value);
			OCEAN_EXPECT_TRUE(validation, value.isNull());
			OCEAN_EXPECT_FALSE(validation, value.isString());
		}

		{
			// buffer value

			std::vector<uint8_t> bufferValue(RandomI::random(randomGenerator, 100u)); // can also be empty

			for (uint8_t& element : bufferValue)
			{
				element = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			const Value value(bufferValue.data(), bufferValue.size());

			OCEAN_EXPECT_TRUE(validation, verifyValue(value, bufferValue));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestValue::testComparison(const double testDuration)
{
	Log::info() << "Comparison test:";

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const std::vector<Value::ValueType> valueTypes =
	{
		Value::VT_INVALID,
		Value::VT_BOOL,
		Value::VT_INT_32,
		Value::VT_INT_64,
		Value::VT_FLOAT_32,
		Value::VT_FLOAT_64,
		Value::VT_STRING,
		Value::VT_BUFFER
	};

	OCEAN_EXPECT_EQUAL(validation, Value(), Value());

	const Timestamp startTimestamp(true);

	do
	{
		{
			// test bool

			const bool value = RandomI::boolean(randomGenerator);

			OCEAN_EXPECT_EQUAL(validation, Value(value), Value(value));

			OCEAN_EXPECT_NOT_EQUAL(validation, Value(value), Value(!value));
		}

		{
			// test int32

			const int32_t value = int32_t(RandomI::random32(randomGenerator));

			OCEAN_EXPECT_EQUAL(validation, Value(value), Value(value));

			int32_t otherValue;

			do
			{
				otherValue = int32_t(RandomI::random32(randomGenerator));
			}
			while (value == otherValue);

			OCEAN_EXPECT_NOT_EQUAL(validation, Value(value), Value(otherValue));
		}

		{
			// test int64

			const int64_t value = int64_t(RandomI::random64(randomGenerator));

			OCEAN_EXPECT_EQUAL(validation, Value(value), Value(value));

			int64_t otherValue;

			do
			{
				otherValue = int64_t(RandomI::random64(randomGenerator));
			}
			while (value == otherValue);

			OCEAN_EXPECT_NOT_EQUAL(validation, Value(value), Value(otherValue));
		}

		{
			// test float

			const uint32_t intValue = RandomI::random32(randomGenerator);

			float floatValue;
			static_assert(sizeof(intValue) == sizeof(floatValue), "Invalid data type!");
			memcpy(&floatValue, &intValue, sizeof(floatValue));

			OCEAN_EXPECT_EQUAL(validation, Value(floatValue) == Value(floatValue), floatValue == floatValue); // handling edge cases like nan

			const uint32_t otherIntValue = RandomI::random32(randomGenerator);

			float otherFloatValue;
			memcpy(&otherFloatValue, &otherIntValue, sizeof(otherFloatValue));

			OCEAN_EXPECT_EQUAL(validation, Value(floatValue) == Value(otherFloatValue), floatValue == otherFloatValue);
		}

		{
			// test double

			const uint64_t intValue = RandomI::random64(randomGenerator);

			double floatValue;
			static_assert(sizeof(intValue) == sizeof(floatValue), "Invalid data type!");
			memcpy(&floatValue, &intValue, sizeof(floatValue));

			OCEAN_EXPECT_EQUAL(validation, Value(floatValue) == Value(floatValue), floatValue == floatValue); // handling edge cases like nan

			const uint64_t otherIntValue = RandomI::random64(randomGenerator);

			double otherFloatValue;
			memcpy(&otherFloatValue, &otherIntValue, sizeof(otherFloatValue));

			OCEAN_EXPECT_EQUAL(validation, Value(floatValue) == Value(otherFloatValue), floatValue == otherFloatValue);
		}

		{
			// test string

			std::string stringValue;

			const size_t size = size_t(RandomI::random(randomGenerator, 100u));

			while (stringValue.size() < size)
			{
				stringValue += char(RandomI::random(randomGenerator, 255));
			}

			OCEAN_EXPECT_EQUAL(validation, Value(stringValue), Value(stringValue));

			std::string otherStringValue;

			const size_t otherSize = size_t(RandomI::random(randomGenerator, 100u));

			while (otherStringValue.size() < otherSize)
			{
				otherStringValue += char(RandomI::random(randomGenerator, 255));
			}

			OCEAN_EXPECT_EQUAL(validation, Value(stringValue) == Value(otherStringValue), stringValue == otherStringValue);
		}

		{
			// test buffer

			std::vector<uint8_t> bufferValue;

			const size_t size = size_t(RandomI::random(randomGenerator, 100u));

			while (bufferValue.size() < size)
			{
				bufferValue.emplace_back(uint8_t(RandomI::random(randomGenerator, 255)));
			}

			OCEAN_EXPECT_EQUAL(validation, Value(bufferValue.data(), bufferValue.size()), Value(bufferValue.data(), bufferValue.size()));

			std::vector<uint8_t> otherBufferValue;

			const size_t otherSize = size_t(RandomI::random(randomGenerator, 100u));

			while (otherBufferValue.size() < otherSize)
			{
				otherBufferValue.emplace_back(uint8_t(RandomI::random(randomGenerator, 255)));
			}

			const bool expected = bufferValue.size() == otherBufferValue.size() && (bufferValue.empty() || memcmp(bufferValue.data(), otherBufferValue.data(), bufferValue.size()) == 0);

			OCEAN_EXPECT_EQUAL(validation, Value(bufferValue.data(), bufferValue.size()) == Value(otherBufferValue.data(), otherBufferValue.size()), expected);
		}

		{
			// testing different data types

			std::vector<Value> values(2);

			Indices32 valueTypeIndices = {Index32(-1), Index32(-1)};

			RandomI::random(randomGenerator, (unsigned int)(valueTypes.size()) - 1u, valueTypeIndices[0], valueTypeIndices[1]);
			ocean_assert(valueTypeIndices[0] != valueTypeIndices[1]);

			for (size_t n = 0; n < values.size(); ++n)
			{
				const Value::ValueType valueType = valueTypes[valueTypeIndices[n]];
				Value& value = values[n];

				switch (valueType)
				{
					case Value::VT_INVALID:
					{
						// nothing to do here
						break;
					}

					case Value::VT_BOOL:
					{
						value = Value(RandomI::boolean(randomGenerator));
						break;
					}

					case Value::VT_INT_32:
					{
						value = Value(int32_t(RandomI::random32(randomGenerator)));
						break;
					}

					case Value::VT_INT_64:
					{
						value = Value(int64_t(RandomI::random64(randomGenerator)));
						break;
					}

					case Value::VT_FLOAT_32:
					{
						const uint32_t floatIntValue = RandomI::random32(randomGenerator);

						float floatValue;
						memcpy(&floatValue, &floatIntValue, sizeof(floatValue));

						value = Value(floatValue);
						break;
					}

					case Value::VT_FLOAT_64:
					{
						const uint64_t floatIntValue = RandomI::random64(randomGenerator);

						double floatValue;
						memcpy(&floatValue, &floatIntValue, sizeof(floatValue));

						value = Value(floatValue);
						break;
					}

					case Value::VT_STRING:
					{
						std::string stringValue;

						const size_t stringSize = size_t(RandomI::random(randomGenerator, 100u));

						while (stringValue.size() < stringSize)
						{
							stringValue += char(RandomI::random(randomGenerator, 255));
						}

						value = Value(stringValue);
						break;
					}

					case Value::VT_BUFFER:
					{
						std::vector<uint8_t> bufferValue;

						const size_t bufferSize = size_t(RandomI::random(randomGenerator, 100u));

						while (bufferValue.size() < bufferSize)
						{
							bufferValue.emplace_back(uint8_t(RandomI::random(randomGenerator, 255)));
						}

						value = Value(bufferValue.data(), bufferValue.size());
						break;
					}
				}
			}

			OCEAN_EXPECT_TRUE(validation, values[0] || values[1]);

			OCEAN_EXPECT_NOT_EQUAL(validation, values[0].type(), values[1].type());

			OCEAN_EXPECT_NOT_EQUAL(validation, values[0], values[1]);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestValue::testCopy(const double testDuration)
{
	Log::info() << "Copy constructor and assign operator test:";

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// invalid value

			const Value value;

			{
				const Value copiedValue(value);

				OCEAN_EXPECT_TRUE(validation, !copiedValue);
				OCEAN_EXPECT_TRUE(validation, copiedValue.isNull());
			}

			{
				Value assignedValue;
				assignedValue = value;

				OCEAN_EXPECT_TRUE(validation, !assignedValue);
				OCEAN_EXPECT_TRUE(validation, assignedValue.isNull());
			}
		}

		{
			// bool value

			const bool boolValue = RandomI::boolean(randomGenerator);

			const Value value(boolValue);

			{
				const Value copiedValue(value);

				OCEAN_EXPECT_TRUE(validation, verifyValue(copiedValue, boolValue));
			}

			{
				Value assignedValue;
				assignedValue = value;

				OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, boolValue));
			}
		}

		{
			// int32 value

			const int32_t intValue = RandomI::random(randomGenerator, -1000, 1000);

			const Value value(intValue);

			{
				const Value copiedValue(value);

				OCEAN_EXPECT_TRUE(validation, verifyValue(copiedValue, intValue));
			}

			{
				Value assignedValue;
				assignedValue = value;

				OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, intValue));
			}
		}

		{
			// int64 value

			const int64_t intValue64 = RandomI::random(randomGenerator, -1000, 1000);

			const Value value(intValue64);

			{
				const Value copiedValue(value);

				OCEAN_EXPECT_TRUE(validation, verifyValue(copiedValue, intValue64));
			}

			{
				Value assignedValue;
				assignedValue = value;

				OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, intValue64));
			}
		}

		{
			// float32 value

			const float floatValue = float(RandomI::random(randomGenerator, -1000, 1000));

			const Value value(floatValue);

			{
				const Value copiedValue(value);

				OCEAN_EXPECT_TRUE(validation, verifyValue(copiedValue, floatValue));
			}

			{
				Value assignedValue;
				assignedValue = value;

				OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, floatValue));
			}
		}

		{
			// float64 value

			const double floatValue64 = double(RandomI::random(randomGenerator, -1000, 1000));

			const Value value(floatValue64);

			{
				const Value copiedValue(value);

				OCEAN_EXPECT_TRUE(validation, verifyValue(copiedValue, floatValue64));
			}

			{
				Value assignedValue;
				assignedValue = value;

				OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, floatValue64));
			}
		}

		{
			// string value

			const unsigned int stringLength = RandomI::random(randomGenerator, 1u, 100u);
			const std::string stringValue(stringLength, char(RandomI::random(randomGenerator, int('a'), int('z'))));

			const Value value = RandomI::boolean(randomGenerator) ? Value(stringValue) : Value(stringValue.c_str());

			{
				const Value copiedValue(value);

				OCEAN_EXPECT_TRUE(validation, verifyValue(copiedValue, stringValue));
			}

			{
				Value assignedValue;
				assignedValue = value;

				OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, stringValue));
			}
		}

		{
			// empty string value

			const std::string stringValue;

			const Value value(stringValue);

			{
				const Value copiedValue(value);

				OCEAN_EXPECT_TRUE(validation, verifyValue(copiedValue, stringValue));
			}

			{
				Value assignedValue;
				assignedValue = value;

				OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, stringValue));
			}
		}

		{
			// null string

			const char* stringValue = nullptr;

			const Value value(stringValue);

			{
				const Value copiedValue(value);

				OCEAN_EXPECT_TRUE(validation, !copiedValue);
				OCEAN_EXPECT_TRUE(validation, copiedValue.isNull());
				OCEAN_EXPECT_FALSE(validation, copiedValue.isString());
			}

			{
				Value assignedValue;
				assignedValue = value;

				OCEAN_EXPECT_TRUE(validation, !assignedValue);
				OCEAN_EXPECT_TRUE(validation, assignedValue.isNull());
				OCEAN_EXPECT_FALSE(validation, assignedValue.isString());
			}
		}

		{
			// buffer value

			std::vector<uint8_t> bufferValue(RandomI::random(randomGenerator, 100u)); // can also be empty

			for (uint8_t& element : bufferValue)
			{
				element = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			const Value value(bufferValue.data(), bufferValue.size());

			{
				const Value copiedValue(value);

				OCEAN_EXPECT_TRUE(validation, verifyValue(copiedValue, bufferValue));
			}

			{
				Value assignedValue;
				assignedValue = value;

				OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, bufferValue));
			}
		}

		{
			// buffer/string/int value

			std::vector<uint8_t> bufferValue(RandomI::random(randomGenerator, 1u, 100u)); // can also be empty

			for (uint8_t& element : bufferValue)
			{
				element = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			Value valueA(bufferValue.data(), bufferValue.size());

			const unsigned int stringLength = RandomI::random(randomGenerator, 1u, 100u);
			const std::string stringValue(stringLength, char(RandomI::random(randomGenerator, int('a'), int('z'))));

			Value valueB(stringValue);

			if (RandomI::boolean(randomGenerator))
			{
				valueA = valueB;

				OCEAN_EXPECT_TRUE(validation, verifyValue(valueA, stringValue));

				OCEAN_EXPECT_TRUE(validation, verifyValue(valueB, stringValue));
			}
			else
			{
				valueB = valueA;

				OCEAN_EXPECT_TRUE(validation, verifyValue(valueB, bufferValue));

				OCEAN_EXPECT_TRUE(validation, verifyValue(valueA, bufferValue));
			}

			const int32_t intValue = RandomI::random(randomGenerator, -1000, 1000);

			Value valueC(intValue);

			if (RandomI::boolean(randomGenerator))
			{
				// let's use valueC as source

				if (RandomI::boolean(randomGenerator))
				{
					valueA = valueC;

					OCEAN_EXPECT_TRUE(validation, verifyValue(valueA, intValue));
				}
				else
				{
					valueB = valueC;

					OCEAN_EXPECT_TRUE(validation, verifyValue(valueB, intValue));
				}
			}
			else
			{
				// let's use valueC as target

				if (RandomI::boolean(randomGenerator))
				{
					valueC = valueA;
				}
				else
				{
					valueC = valueB;
				}

				if (valueC.isBuffer())
				{
					OCEAN_EXPECT_TRUE(validation, verifyValue(valueC, bufferValue));
				}

				if (valueC.isString())
				{
					OCEAN_EXPECT_TRUE(validation, verifyValue(valueC, stringValue));
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestValue::testMove(const double testDuration)
{
	Log::info() << "Move constructor and move operator test:";

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// invalid value

			Value value;

			Value constructorValue(std::move(value));

			OCEAN_EXPECT_TRUE(validation, !constructorValue);
			OCEAN_EXPECT_TRUE(validation, constructorValue.isNull());

			Value operatorValue;
			operatorValue = std::move(constructorValue);

			OCEAN_EXPECT_TRUE(validation, !operatorValue);
			OCEAN_EXPECT_TRUE(validation, operatorValue.isNull());
		}

		{
			// bool value

			const bool boolValue = RandomI::boolean(randomGenerator);

			Value value(boolValue);

			Value constructorValue(std::move(value));

			OCEAN_EXPECT_TRUE(validation, verifyValue(constructorValue, boolValue));

			OCEAN_EXPECT_TRUE(validation, !value);
			OCEAN_EXPECT_TRUE(validation, value.isNull());

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, boolValue));

			OCEAN_EXPECT_TRUE(validation, !constructorValue);
			OCEAN_EXPECT_TRUE(validation, constructorValue.isNull());
		}

		{
			// int32 value

			const int32_t intValue = RandomI::random(randomGenerator, -1000, 1000);

			Value value(intValue);

			Value constructorValue(std::move(value));

			OCEAN_EXPECT_TRUE(validation, verifyValue(constructorValue, intValue));

			OCEAN_EXPECT_TRUE(validation, !value);
			OCEAN_EXPECT_TRUE(validation, value.isNull());

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, intValue));

			OCEAN_EXPECT_TRUE(validation, !constructorValue);
			OCEAN_EXPECT_TRUE(validation, constructorValue.isNull());
		}

		{
			// int64 value

			const int64_t intValue64 = RandomI::random(randomGenerator, -1000, 1000);

			Value value(intValue64);

			Value constructorValue(std::move(value));

			OCEAN_EXPECT_TRUE(validation, verifyValue(constructorValue, intValue64));

			OCEAN_EXPECT_TRUE(validation, !value);
			OCEAN_EXPECT_TRUE(validation, value.isNull());

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, intValue64));

			OCEAN_EXPECT_TRUE(validation, !constructorValue);
			OCEAN_EXPECT_TRUE(validation, constructorValue.isNull());
		}

		{
			// float32 value

			const float floatValue = float(RandomI::random(randomGenerator, -1000, 1000));

			Value value(floatValue);

			Value constructorValue(std::move(value));

			OCEAN_EXPECT_TRUE(validation, verifyValue(constructorValue, floatValue));

			OCEAN_EXPECT_TRUE(validation, !value);
			OCEAN_EXPECT_TRUE(validation, value.isNull());

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, floatValue));

			OCEAN_EXPECT_TRUE(validation, !constructorValue);
			OCEAN_EXPECT_TRUE(validation, constructorValue.isNull());
		}

		{
			// float64 value

			const double floatValue64 = double(RandomI::random(randomGenerator, -1000, 1000));

			Value value(floatValue64);

			Value constructorValue(std::move(value));

			OCEAN_EXPECT_TRUE(validation, verifyValue(constructorValue, floatValue64));

			OCEAN_EXPECT_TRUE(validation, !value);
			OCEAN_EXPECT_TRUE(validation, value.isNull());

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, floatValue64));

			OCEAN_EXPECT_TRUE(validation, !constructorValue);
			OCEAN_EXPECT_TRUE(validation, constructorValue.isNull());
		}

		{
			// string value

			const unsigned int stringLength = RandomI::random(randomGenerator, 1u, 100u);
			const std::string stringValue(stringLength, char(RandomI::random(randomGenerator, int('a'), int('z'))));

			Value value = RandomI::boolean(randomGenerator) ? Value(stringValue) : Value(stringValue.c_str());

			Value constructorValue(std::move(value));

			OCEAN_EXPECT_TRUE(validation, verifyValue(constructorValue, stringValue));

			OCEAN_EXPECT_TRUE(validation, !value);
			OCEAN_EXPECT_TRUE(validation, value.isNull());

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, stringValue));

			OCEAN_EXPECT_TRUE(validation, !constructorValue);
			OCEAN_EXPECT_TRUE(validation, constructorValue.isNull());
		}

		{
			// empty string value

			const std::string stringValue;

			Value value(stringValue);

			Value constructorValue(std::move(value));

			OCEAN_EXPECT_TRUE(validation, verifyValue(constructorValue, stringValue));

			OCEAN_EXPECT_TRUE(validation, !value);
			OCEAN_EXPECT_TRUE(validation, value.isNull());

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, stringValue));

			OCEAN_EXPECT_TRUE(validation, !constructorValue);
			OCEAN_EXPECT_TRUE(validation, constructorValue.isNull());
		}

		{
			// null string

			const char* stringValue = nullptr;

			Value value(stringValue);

			Value constructorValue(std::move(value));

			OCEAN_EXPECT_TRUE(validation, !constructorValue);
			OCEAN_EXPECT_TRUE(validation, constructorValue.isNull());
			OCEAN_EXPECT_FALSE(validation, constructorValue.isString());

			OCEAN_EXPECT_TRUE(validation, !value);
			OCEAN_EXPECT_TRUE(validation, value.isNull());

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			OCEAN_EXPECT_TRUE(validation, !assignedValue);
			OCEAN_EXPECT_TRUE(validation, assignedValue.isNull());
			OCEAN_EXPECT_FALSE(validation, assignedValue.isString());

			OCEAN_EXPECT_TRUE(validation, !constructorValue);
			OCEAN_EXPECT_TRUE(validation, constructorValue.isNull());
		}

		{
			// buffer value

			std::vector<uint8_t> bufferValue(RandomI::random(randomGenerator, 100u)); // can also be empty

			for (uint8_t& element : bufferValue)
			{
				element = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			Value value(bufferValue.data(), bufferValue.size());

			Value constructorValue(std::move(value));

			OCEAN_EXPECT_TRUE(validation, verifyValue(constructorValue, bufferValue));

			OCEAN_EXPECT_TRUE(validation, !value);
			OCEAN_EXPECT_TRUE(validation, value.isNull());

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			OCEAN_EXPECT_TRUE(validation, verifyValue(assignedValue, bufferValue));

			OCEAN_EXPECT_TRUE(validation, !constructorValue);
			OCEAN_EXPECT_TRUE(validation, constructorValue.isNull());
		}

		{
			// buffer/string/int value

			std::vector<uint8_t> bufferValue(RandomI::random(randomGenerator, 1u, 100u)); // can also be empty

			for (uint8_t& element : bufferValue)
			{
				element = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			Value valueA(bufferValue.data(), bufferValue.size());

			const unsigned int stringLength = RandomI::random(randomGenerator, 1u, 100u);
			const std::string stringValue(stringLength, char(RandomI::random(randomGenerator, int('a'), int('z'))));

			Value valueB(stringValue);

			if (RandomI::boolean(randomGenerator))
			{
				valueA = std::move(valueB);

				OCEAN_EXPECT_TRUE(validation, verifyValue(valueA, stringValue));

				OCEAN_EXPECT_TRUE(validation, !valueB);
				OCEAN_EXPECT_TRUE(validation, valueB.isNull());
			}
			else
			{
				valueB = std::move(valueA);

				OCEAN_EXPECT_TRUE(validation, verifyValue(valueB, bufferValue));

				OCEAN_EXPECT_TRUE(validation, !valueA);
				OCEAN_EXPECT_TRUE(validation, valueA.isNull());
			}

			const int32_t intValue = RandomI::random(randomGenerator, -1000, 1000);

			Value valueC(intValue);

			if (RandomI::boolean(randomGenerator))
			{
				// let's use valueC as source

				if (valueA)
				{
					valueA = std::move(valueC);

					OCEAN_EXPECT_TRUE(validation, verifyValue(valueA, intValue));
				}
				else
				{
					ocean_assert(valueB);
					valueB = std::move(valueC);

					OCEAN_EXPECT_TRUE(validation, verifyValue(valueB, intValue));
				}

				OCEAN_EXPECT_TRUE(validation, !valueC);
				OCEAN_EXPECT_TRUE(validation, valueC.isNull());
			}
			else
			{
				// let's use valueC as target

				if (valueA)
				{
					valueC = std::move(valueA);

					OCEAN_EXPECT_TRUE(validation, !valueA);
					OCEAN_EXPECT_TRUE(validation, valueA.isNull());
				}
				else
				{
					ocean_assert(valueB);
					valueC = std::move(valueB);

					OCEAN_EXPECT_TRUE(validation, !valueB);
					OCEAN_EXPECT_TRUE(validation, valueB.isNull());
				}

				if (valueC.isBuffer())
				{
					OCEAN_EXPECT_TRUE(validation, verifyValue(valueC, bufferValue));
				}

				if (valueC.isString())
				{
					OCEAN_EXPECT_TRUE(validation, verifyValue(valueC, stringValue));
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestValue::testReadWrite(const double testDuration)
{
	Log::info() << "Read/write to/from buffer test:";

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Value value(createRandomValue(randomGenerator));
		ocean_assert(value);

		std::vector<uint8_t> buffer;
		const unsigned int maxOffsetInBuffer = RandomI::random(randomGenerator, 1u, 100u);
		const size_t offsetInBuffer = maxOffsetInBuffer * RandomI::random(randomGenerator, 1u);

		if (Value::writeToBuffer(value, buffer, offsetInBuffer))
		{
			if (buffer.size() > offsetInBuffer)
			{
				const Value resultValue = Value::readFromBuffer(buffer.data() + offsetInBuffer, buffer.size() - offsetInBuffer);

				if (resultValue)
				{
					OCEAN_EXPECT_EQUAL(validation, resultValue, value);
				}
				else
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

Value TestValue::createRandomValue(RandomGenerator& randomGenerator)
{
	const std::vector<Value::ValueType> valueTypes = {Value::VT_BOOL, Value::VT_INT_32, Value::VT_INT_64, Value::VT_FLOAT_32, Value::VT_FLOAT_64, Value::VT_STRING, Value::VT_BUFFER};

	const Value::ValueType valueType = RandomI::random(randomGenerator, valueTypes);

	switch (valueType)
	{
		case Value::VT_INVALID:
			break;

		case Value::VT_BOOL:
		{
			const bool boolValue = RandomI::boolean(randomGenerator);

			return Value(boolValue);
		}

		case Value::VT_INT_32:
		{
			const int32_t intValue = RandomI::random(randomGenerator, -1000, 1000);

			return Value(intValue);
		}

		case Value::VT_INT_64:
		{
			const int64_t intValue64 = RandomI::random(randomGenerator, -1000, 1000);

			return Value(intValue64);
		}

		case Value::VT_FLOAT_32:
		{
			const float floatValue = float(RandomI::random(randomGenerator, -1000, 1000));

			return Value(floatValue);
		}

		case Value::VT_FLOAT_64:
		{
			const double floatValue64 = double(RandomI::random(randomGenerator, -1000, 1000));

			return Value(floatValue64);
		}

		case Value::VT_STRING:
		{
			const unsigned int stringLength = RandomI::random(randomGenerator, 1u, 100u);
			const std::string stringValue(stringLength, char(RandomI::random(randomGenerator, int('a'), int('z'))));

			switch (RandomI::random(randomGenerator, 2u))
			{
				case 0u:
					return Value(stringValue);

				case 1u:
					return Value(stringValue.c_str());

				default:
					break;
			}

			return Value(std::string()); // empty string value
		}

		case Value::VT_BUFFER:
		{
			std::vector<uint8_t> bufferValue(RandomI::random(randomGenerator, 100u)); // can also be empty

			for (uint8_t& element : bufferValue)
			{
				element = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			return Value(bufferValue.data(), bufferValue.size());
		}
	}

	ocean_assert(false && "This must never happen!");
	return Value(false);
}

}

}

}
