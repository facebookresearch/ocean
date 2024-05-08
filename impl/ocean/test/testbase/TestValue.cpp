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

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestValue::test(const double testDuration)
{
	Log::info() << "---   Value tests:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testComparison(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopy(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMove(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReadWrite(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Value test succeeded.";
	}
	else
	{
		Log::info() << "Value test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// invalid value

			const Value value;

			if (value || !value.isNull())
			{
				allSucceeded = false;
			}

			if (value.isBool() || value.isInt() || value.isInt64() || value.isFloat() || value.isFloat64() || value.isString() || value.isBuffer())
			{
				allSucceeded = false;
			}
		}

		{
			// bool value

			const bool boolValue = RandomI::random(1u) == 0u;

			const Value value(boolValue);

			if (!verifyValue(value, boolValue))
			{
				allSucceeded = false;
			}
		}

		{
			// int32 value

			const int32_t intValue = RandomI::random(-1000, 1000);

			const Value value(intValue);

			if (!verifyValue(value, intValue))
			{
				allSucceeded = false;
			}
		}

		{
			// int64 value

			const int64_t intValue64 = RandomI::random(-1000, 1000);

			const Value value(intValue64);

			if (!verifyValue(value, intValue64))
			{
				allSucceeded = false;
			}
		}

		{
			// float32 value

			const float floatValue = float(RandomI::random(-1000, 1000));

			const Value value(floatValue);

			if (!verifyValue(value, floatValue))
			{
				allSucceeded = false;
			}
		}

		{
			// float64 value

			const double floatValue64 = double(RandomI::random(-1000, 1000));

			const Value value(floatValue64);

			if (!verifyValue(value, floatValue64))
			{
				allSucceeded = false;
			}
		}

		{
			// string value

			const std::string stringValue(RandomI::random(1u, 100u), char(RandomI::random(int('a'), int('z'))));

			const Value value = RandomI::random(1u) == 0u ? Value(stringValue) : Value(stringValue.c_str());

			if (!verifyValue(value, stringValue))
			{
				allSucceeded = false;
			}
		}

		{
			// empty string value

			const std::string stringValue;

			const Value value(stringValue);

			if (!verifyValue(value, stringValue))
			{
				allSucceeded = false;
			}
		}

		{
			// null string

			const char* stringValue = nullptr;

			const Value value(stringValue);

			if (value || !value.isNull() || value.isString())
			{
				allSucceeded = false;
			}
		}

		{
			// buffer value

			std::vector<uint8_t> bufferValue(RandomI::random(100u)); // can also be empty

			for (uint8_t& element : bufferValue)
			{
				element = uint8_t(RandomI::random(255u));
			}

			const Value value(bufferValue.data(), bufferValue.size());

			if (!verifyValue(value, bufferValue))
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

bool TestValue::testComparison(const double testDuration)
{
	Log::info() << "Comparison test:";

	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

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

	if (Value() != Value())
	{
		allSucceeded = false;
	}

	const Timestamp startTimestamp(true);

	do
	{
		{
			// test bool

			const bool value = RandomI::random(1u) == 0u;

			if (Value(value) != Value(value))
			{
				allSucceeded = false;
			}

			if (Value(value) == Value(!value))
			{
				allSucceeded = false;
			}
		}

		{
			// test int32

			const int32_t value = int32_t(RandomI::random32());

			if (Value(value) != Value(value))
			{
				allSucceeded = false;
			}

			int32_t otherValue;

			do
			{
				otherValue = int32_t(RandomI::random32());
			}
			while (value == otherValue);

			if (Value(value) == Value(otherValue))
			{
				allSucceeded = false;
			}
		}

		{
			// test int64

			const int64_t value = int64_t(RandomI::random64());

			if (Value(value) != Value(value))
			{
				allSucceeded = false;
			}

			int64_t otherValue;

			do
			{
				otherValue = int64_t(RandomI::random64());
			}
			while (value == otherValue);

			if (Value(value) == Value(otherValue))
			{
				allSucceeded = false;
			}
		}

		{
			// test float

			const uint32_t intValue = RandomI::random32();

			float floatValue;
			static_assert(sizeof(intValue) == sizeof(floatValue), "Invalid data type!");
			memcpy(&floatValue, &intValue, sizeof(floatValue));

			if ((Value(floatValue) == Value(floatValue)) != (floatValue == floatValue)) // handling edge cases like nan
			{
				allSucceeded = false;
			}

			const uint32_t otherIntValue = RandomI::random32();

			float otherFloatValue;
			memcpy(&otherFloatValue, &otherIntValue, sizeof(otherFloatValue));

			if ((Value(floatValue) == Value(otherFloatValue)) != (floatValue == otherFloatValue))
			{
				allSucceeded = false;
			}
		}

		{
			// test double

			const uint64_t intValue = RandomI::random64();

			double floatValue;
			static_assert(sizeof(intValue) == sizeof(floatValue), "Invalid data type!");
			memcpy(&floatValue, &intValue, sizeof(floatValue));

			if ((Value(floatValue) == Value(floatValue)) != (floatValue == floatValue)) // handling edge cases like nan
			{
				allSucceeded = false;
			}

			const uint64_t otherIntValue = RandomI::random64();

			double otherFloatValue;
			memcpy(&otherFloatValue, &otherIntValue, sizeof(otherFloatValue));

			if ((Value(floatValue) == Value(otherFloatValue)) != (floatValue == otherFloatValue))
			{
				allSucceeded = false;
			}
		}

		{
			// test string

			std::string stringValue;

			const size_t size = size_t(RandomI::random(100u));

			while (stringValue.size() < size)
			{
				stringValue += char(RandomI::random(255));
			}

			if (Value(stringValue) != Value(stringValue))
			{
				allSucceeded = false;
			}

			std::string otherStringValue;

			const size_t otherSize = size_t(RandomI::random(100u));

			while (otherStringValue.size() < otherSize)
			{
				otherStringValue += char(RandomI::random(255));
			}

			if ((Value(stringValue) == Value(otherStringValue)) != (stringValue == otherStringValue))
			{
				allSucceeded = false;
			}
		}

		{
			// test buffer

			std::vector<uint8_t> bufferValue;

			const size_t size = size_t(RandomI::random(100u));

			while (bufferValue.size() < size)
			{
				bufferValue.emplace_back(uint8_t(RandomI::random(255)));
			}

			if (Value(bufferValue.data(), bufferValue.size()) != Value(bufferValue.data(), bufferValue.size()))
			{
				allSucceeded = false;
			}

			std::vector<uint8_t> otherBufferValue;

			const size_t otherSize = size_t(RandomI::random(100u));

			while (otherBufferValue.size() < otherSize)
			{
				otherBufferValue.emplace_back(uint8_t(RandomI::random(255)));
			}

			const bool expected = bufferValue.size() == otherBufferValue.size() && (bufferValue.empty() || memcmp(bufferValue.data(), otherBufferValue.data(), bufferValue.size()) == 0);

			if ((Value(bufferValue.data(), bufferValue.size()) == Value(otherBufferValue.data(), otherBufferValue.size())) != expected)
			{
				allSucceeded = false;
			}
		}

		{
			// testing different data types

			std::vector<Value> values(2);

			Indices32 valueTypeIndices = {Index32(-1), Index32(-1)};

			RandomI::random((unsigned int)(valueTypes.size()) - 1u, valueTypeIndices[0], valueTypeIndices[1]);
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
						value = Value(RandomI::random(1u) == 0u);
						break;
					}

					case Value::VT_INT_32:
					{
						value = Value(int32_t(RandomI::random32()));
						break;
					}

					case Value::VT_INT_64:
					{
						value = Value(int64_t(RandomI::random64()));
						break;
					}

					case Value::VT_FLOAT_32:
					{
						const uint32_t intValue = int32_t(RandomI::random32());

						float floatValue;
						memcpy(&floatValue, &intValue, sizeof(floatValue));

						value = Value(floatValue);
						break;
					}

					case Value::VT_FLOAT_64:
					{
						const uint64_t intValue = int32_t(RandomI::random64());

						double floatValue;
						memcpy(&floatValue, &intValue, sizeof(floatValue));

						value = Value(floatValue);
						break;
					}

					case Value::VT_STRING:
					{
						std::string stringValue;

						const size_t size = size_t(RandomI::random(100u));

						while (stringValue.size() < size)
						{
							stringValue += char(RandomI::random(255));
						}

						value = Value(stringValue);
						break;
					}

					case Value::VT_BUFFER:
					{
						std::vector<uint8_t> bufferValue;

						const size_t size = size_t(RandomI::random(100u));

						while (bufferValue.size() < size)
						{
							bufferValue.emplace_back(uint8_t(RandomI::random(255)));
						}

						value = Value(bufferValue.data(), bufferValue.size());
						break;
					}
				}
			}

			if (!values[0] && !values[1])
			{
				allSucceeded = false;
			}

			if (values[0].type() == values[1].type())
			{
				allSucceeded = false;
			}

			if (values[0] == values[1])
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

bool TestValue::testCopy(const double testDuration)
{
	Log::info() << "Copy constructor and assign operator test:";

	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// invalid value

			const Value value;

			{
				const Value copiedValue(value);

				if (copiedValue || !copiedValue.isNull())
				{
					allSucceeded = false;
				}
			}

			{
				Value assignedValue;
				assignedValue = value;

				if (assignedValue || !assignedValue.isNull())
				{
					allSucceeded = false;
				}
			}
		}

		{
			// bool value

			const bool boolValue = RandomI::random(1u) == 0u;

			const Value value(boolValue);

			{
				const Value copiedValue(value);

				if (!verifyValue(copiedValue, boolValue))
				{
					allSucceeded = false;
				}
			}

			{
				Value assignedValue;
				assignedValue = value;

				if (!verifyValue(assignedValue, boolValue))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// int32 value

			const int32_t intValue = RandomI::random(-1000, 1000);

			const Value value(intValue);

			{
				const Value copiedValue(value);

				if (!verifyValue(copiedValue, intValue))
				{
					allSucceeded = false;
				}
			}

			{
				Value assignedValue;
				assignedValue = value;

				if (!verifyValue(assignedValue, intValue))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// int64 value

			const int64_t intValue64 = RandomI::random(-1000, 1000);

			const Value value(intValue64);

			{
				const Value copiedValue(value);

				if (!verifyValue(copiedValue, intValue64))
				{
					allSucceeded = false;
				}
			}

			{
				Value assignedValue;
				assignedValue = value;

				if (!verifyValue(assignedValue, intValue64))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// float32 value

			const float floatValue = float(RandomI::random(-1000, 1000));

			const Value value(floatValue);

			{
				const Value copiedValue(value);

				if (!verifyValue(copiedValue, floatValue))
				{
					allSucceeded = false;
				}
			}

			{
				Value assignedValue;
				assignedValue = value;

				if (!verifyValue(assignedValue, floatValue))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// float64 value

			const double floatValue64 = double(RandomI::random(-1000, 1000));

			const Value value(floatValue64);

			{
				const Value copiedValue(value);

				if (!verifyValue(copiedValue, floatValue64))
				{
					allSucceeded = false;
				}
			}

			{
				Value assignedValue;
				assignedValue = value;

				if (!verifyValue(assignedValue, floatValue64))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// string value

			const std::string stringValue(RandomI::random(1u, 100u), char(RandomI::random(int('a'), int('z'))));

			const Value value = RandomI::random(1u) == 0u ? Value(stringValue) : Value(stringValue.c_str());

			{
				const Value copiedValue(value);

				if (!verifyValue(copiedValue, stringValue))
				{
					allSucceeded = false;
				}
			}

			{
				Value assignedValue;
				assignedValue = value;

				if (!verifyValue(assignedValue, stringValue))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// empty string value

			const std::string stringValue;

			const Value value(stringValue);

			{
				const Value copiedValue(value);

				if (!verifyValue(copiedValue, stringValue))
				{
					allSucceeded = false;
				}
			}

			{
				Value assignedValue;
				assignedValue = value;

				if (!verifyValue(assignedValue, stringValue))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// null string

			const char* stringValue = nullptr;

			const Value value(stringValue);

			{
				const Value copiedValue(value);

				if (copiedValue || !copiedValue.isNull() || copiedValue.isString())
				{
					allSucceeded = false;
				}
			}

			{
				Value assignedValue;
				assignedValue = value;

				if (assignedValue || !assignedValue.isNull() || assignedValue.isString())
				{
					allSucceeded = false;
				}
			}
		}

		{
			// buffer value

			std::vector<uint8_t> bufferValue(RandomI::random(100u)); // can also be empty

			for (uint8_t& element : bufferValue)
			{
				element = uint8_t(RandomI::random(255u));
			}

			const Value value(bufferValue.data(), bufferValue.size());

			{
				const Value copiedValue(value);

				if (!verifyValue(copiedValue, bufferValue))
				{
					allSucceeded = false;
				}
			}

			{
				Value assignedValue;
				assignedValue = value;

				if (!verifyValue(assignedValue, bufferValue))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// buffer/string/int value

			std::vector<uint8_t> bufferValue(RandomI::random(1u, 100u)); // can also be empty

			for (uint8_t& element : bufferValue)
			{
				element = uint8_t(RandomI::random(255u));
			}

			Value valueA(bufferValue.data(), bufferValue.size());

			const std::string stringValue(RandomI::random(1u, 100u), char(RandomI::random(int('a'), int('z'))));

			Value valueB(stringValue);

			if (RandomI::random(1u) == 0u)
			{
				valueA = valueB;

				if (!verifyValue(valueA, stringValue))
				{
					allSucceeded = false;
				}

				if (!verifyValue(valueB, stringValue))
				{
					allSucceeded = false;
				}
			}
			else
			{
				valueB = valueA;

				if (!verifyValue(valueB, bufferValue))
				{
					allSucceeded = false;
				}

				if (!verifyValue(valueA, bufferValue))
				{
					allSucceeded = false;
				}
			}

			const int32_t intValue = RandomI::random(-1000, 1000);

			Value valueC(intValue);

			if (RandomI::random(1u) == 0u)
			{
				// let's use valueC as source

				if (RandomI::random(1u) == 0u)
				{
					valueA = valueC;

					if (!verifyValue(valueA, intValue))
					{
						allSucceeded = false;
					}
				}
				else
				{
					valueB = valueC;

					if (!verifyValue(valueB, intValue))
					{
						allSucceeded = false;
					}
				}
			}
			else
			{
				// let's use valueC as target

				if (RandomI::random(1u) == 0u)
				{
					valueC = valueA;
				}
				else
				{
					valueC = valueB;
				}

				if (valueC.isBuffer() && !verifyValue(valueC, bufferValue))
				{
					allSucceeded = false;
				}

				if (valueC.isString() && !verifyValue(valueC, stringValue))
				{
					allSucceeded = false;
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

bool TestValue::testMove(const double testDuration)
{
	Log::info() << "Move constructor and move operator test:";

	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// invalid value

			Value value;

			Value constructorValue(std::move(value));

			if (constructorValue || !constructorValue.isNull())
			{
				allSucceeded = false;
			}

			Value operatorValue;
			operatorValue = std::move(constructorValue);

			if (operatorValue || !operatorValue.isNull())
			{
				allSucceeded = false;
			}
		}

		{
			// bool value

			const bool boolValue = RandomI::random(1u) == 0u;

			Value value(boolValue);

			Value constructorValue(std::move(value));

			if (!verifyValue(constructorValue, boolValue))
			{
				allSucceeded = false;
			}

			if (value || !value.isNull())
			{
				allSucceeded = false;
			}

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			if (!verifyValue(assignedValue, boolValue))
			{
				allSucceeded = false;
			}

			if (constructorValue || !constructorValue.isNull())
			{
				allSucceeded = false;
			}
		}

		{
			// int32 value

			const int32_t intValue = RandomI::random(-1000, 1000);

			Value value(intValue);

			Value constructorValue(std::move(value));

			if (!verifyValue(constructorValue, intValue))
			{
				allSucceeded = false;
			}

			if (value || !value.isNull())
			{
				allSucceeded = false;
			}

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			if (!verifyValue(assignedValue, intValue))
			{
				allSucceeded = false;
			}

			if (constructorValue || !constructorValue.isNull())
			{
				allSucceeded = false;
			}
		}

		{
			// int64 value

			const int64_t intValue64 = RandomI::random(-1000, 1000);

			Value value(intValue64);

			Value constructorValue(std::move(value));

			if (!verifyValue(constructorValue, intValue64))
			{
				allSucceeded = false;
			}

			if (value || !value.isNull())
			{
				allSucceeded = false;
			}

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			if (!verifyValue(assignedValue, intValue64))
			{
				allSucceeded = false;
			}

			if (constructorValue || !constructorValue.isNull())
			{
				allSucceeded = false;
			}
		}

		{
			// float32 value

			const float floatValue = float(RandomI::random(-1000, 1000));

			Value value(floatValue);

			Value constructorValue(std::move(value));

			if (!verifyValue(constructorValue, floatValue))
			{
				allSucceeded = false;
			}

			if (value || !value.isNull())
			{
				allSucceeded = false;
			}

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			if (!verifyValue(assignedValue, floatValue))
			{
				allSucceeded = false;
			}

			if (constructorValue || !constructorValue.isNull())
			{
				allSucceeded = false;
			}
		}

		{
			// float64 value

			const double floatValue64 = double(RandomI::random(-1000, 1000));

			Value value(floatValue64);

			Value constructorValue(std::move(value));

			if (!verifyValue(constructorValue, floatValue64))
			{
				allSucceeded = false;
			}

			if (value || !value.isNull())
			{
				allSucceeded = false;
			}

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			if (!verifyValue(assignedValue, floatValue64))
			{
				allSucceeded = false;
			}

			if (constructorValue || !constructorValue.isNull())
			{
				allSucceeded = false;
			}
		}

		{
			// string value

			const std::string stringValue(RandomI::random(1u, 100u), char(RandomI::random(int('a'), int('z'))));

			Value value = RandomI::random(1u) == 0u ? Value(stringValue) : Value(stringValue.c_str());

			Value constructorValue(std::move(value));

			if (!verifyValue(constructorValue, stringValue))
			{
				allSucceeded = false;
			}

			if (value || !value.isNull())
			{
				allSucceeded = false;
			}

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			if (!verifyValue(assignedValue, stringValue))
			{
				allSucceeded = false;
			}

			if (constructorValue || !constructorValue.isNull())
			{
				allSucceeded = false;
			}
		}

		{
			// empty string value

			const std::string stringValue;

			Value value(stringValue);

			Value constructorValue(std::move(value));

			if (!verifyValue(constructorValue, stringValue))
			{
				allSucceeded = false;
			}

			if (value || !value.isNull())
			{
				allSucceeded = false;
			}

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			if (!verifyValue(assignedValue, stringValue))
			{
				allSucceeded = false;
			}

			if (constructorValue || !constructorValue.isNull())
			{
				allSucceeded = false;
			}
		}

		{
			// null string

			const char* stringValue = nullptr;

			Value value(stringValue);

			Value constructorValue(std::move(value));

			if (constructorValue || !constructorValue.isNull() || constructorValue.isString())
			{
				allSucceeded = false;
			}

			if (value || !value.isNull())
			{
				allSucceeded = false;
			}

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			if (assignedValue || !assignedValue.isNull() || assignedValue.isString())
			{
				allSucceeded = false;
			}

			if (constructorValue || !constructorValue.isNull())
			{
				allSucceeded = false;
			}
		}

		{
			// buffer value

			std::vector<uint8_t> bufferValue(RandomI::random(100u)); // can also be empty

			for (uint8_t& element : bufferValue)
			{
				element = uint8_t(RandomI::random(255u));
			}

			Value value(bufferValue.data(), bufferValue.size());

			Value constructorValue(std::move(value));

			if (!verifyValue(constructorValue, bufferValue))
			{
				allSucceeded = false;
			}

			if (value || !value.isNull())
			{
				allSucceeded = false;
			}

			Value assignedValue;
			assignedValue = std::move(constructorValue);

			if (!verifyValue(assignedValue, bufferValue))
			{
				allSucceeded = false;
			}

			if (constructorValue || !constructorValue.isNull())
			{
				allSucceeded = false;
			}
		}

		{
			// buffer/string/int value

			std::vector<uint8_t> bufferValue(RandomI::random(1u, 100u)); // can also be empty

			for (uint8_t& element : bufferValue)
			{
				element = uint8_t(RandomI::random(255u));
			}

			Value valueA(bufferValue.data(), bufferValue.size());

			const std::string stringValue(RandomI::random(1u, 100u), char(RandomI::random(int('a'), int('z'))));

			Value valueB(stringValue);

			if (RandomI::random(1u) == 0u)
			{
				valueA = std::move(valueB);

				if (!verifyValue(valueA, stringValue))
				{
					allSucceeded = false;
				}

				if (valueB || !valueB.isNull())
				{
					allSucceeded = false;
				}
			}
			else
			{
				valueB = std::move(valueA);

				if (!verifyValue(valueB, bufferValue))
				{
					allSucceeded = false;
				}

				if (valueA || !valueA.isNull())
				{
					allSucceeded = false;
				}
			}

			const int32_t intValue = RandomI::random(-1000, 1000);

			Value valueC(intValue);

			if (RandomI::random(1u) == 0u)
			{
				// let's use valueC as source

				if (valueA)
				{
					valueA = std::move(valueC);

					if (!verifyValue(valueA, intValue))
					{
						allSucceeded = false;
					}
				}
				else
				{
					ocean_assert(valueB);
					valueB = std::move(valueC);

					if (!verifyValue(valueB, intValue))
					{
						allSucceeded = false;
					}
				}

				if (valueC || !valueC.isNull())
				{
					allSucceeded = false;
				}
			}
			else
			{
				// let's use valueC as target

				if (valueA)
				{
					valueC = std::move(valueA);

					if (valueA || !valueA.isNull())
					{
						allSucceeded = false;
					}
				}
				else
				{
					ocean_assert(valueB);
					valueC = std::move(valueB);

					if (valueB || !valueB.isNull())
					{
						allSucceeded = false;
					}
				}

				if (valueC.isBuffer() && !verifyValue(valueC, bufferValue))
				{
					allSucceeded = false;
				}

				if (valueC.isString() && !verifyValue(valueC, stringValue))
				{
					allSucceeded = false;
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

bool TestValue::testReadWrite(const double testDuration)
{
	Log::info() << "Read/write to/from buffer test:";

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const Value value(createRandomValue(randomGenerator));
		ocean_assert(value);

		std::vector<uint8_t> buffer;
		const size_t offsetInBuffer = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		if (Value::writeToBuffer(value, buffer, offsetInBuffer))
		{
			if (buffer.size() > offsetInBuffer)
			{
				const Value resultValue = Value::readFromBuffer(buffer.data() + offsetInBuffer, buffer.size() - offsetInBuffer);

				if (resultValue)
				{
					if (resultValue != value)
					{
						allSucceeded = false;
					}
				}
				else
				{
					allSucceeded = false;
				}
			}
			else
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
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
			const bool boolValue = RandomI::random(randomGenerator, 1u) == 0u;

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
			const std::string stringValue(RandomI::random(randomGenerator, 1u, 100u), char(RandomI::random(randomGenerator, int('a'), int('z'))));

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
