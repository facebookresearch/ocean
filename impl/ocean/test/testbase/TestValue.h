/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_VALUE_H
#define META_OCEAN_TEST_TESTBASE_TEST_VALUE_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Value.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a Value test.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestValue
{
	public:

		/**
		 * Invokes all Value tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructor function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testConstructor(const double testDuration);

		/**
		 * Tests the comparison operators.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testComparison(const double testDuration);

		/**
		 * Tests the copy function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCopy(const double testDuration);

		/**
		 * Tests the move function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testMove(const double testDuration);

		/**
		 * Tests the read/read to/from buffer functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testReadWrite(const double testDuration);

	protected:

		/**
		 * Returns a random valid Value object.
		 * @param randomGenerator The random generator to be used
		 * @return The random value object
		 */
		static Value createRandomValue(RandomGenerator& randomGenerator);

		/**
		 * Verifies that the given value object has a specific type and value.
		 * @param object The value object to check
		 * @param value The value the object must have
		 * @return True, if so
		 * @tparam T The data type of the value object
		 */
		template <typename T>
		static bool verifyValue(const Value& object, const T& value);
};

template <>
inline bool TestValue::verifyValue(const Value& object, const bool& value)
{
	if (!object.isBool() || object.boolValue() != value)
	{
		return false;
	}

	if (!object || object.isNull())
	{
		return false;
	}

	return true;
}

template <>
inline bool TestValue::verifyValue(const Value& object, const int32_t& value)
{
	if (!object.isInt() || object.intValue() != value)
	{
		return false;
	}

	if (!object || object.isNull())
	{
		return false;
	}

	return true;
}

template <>
inline bool TestValue::verifyValue(const Value& object, const int64_t& value)
{
	if (!object.isInt64() || object.int64Value() != value)
	{
		return false;
	}

	if (!object || object.isNull())
	{
		return false;
	}

	return true;
}

template <>
inline bool TestValue::verifyValue(const Value& object, const float& value)
{
	if (!object.isFloat() || object.floatValue() != value)
	{
		return false;
	}

	if (!object || object.isNull())
	{
		return false;
	}

	return true;
}

template <>
inline bool TestValue::verifyValue(const Value& object, const double& value)
{
	if (!object.isFloat64() || object.float64Value() != value)
	{
		return false;
	}

	if (!object || object.isNull())
	{
		return false;
	}

	return true;
}

template <>
inline bool TestValue::verifyValue(const Value& object, const std::string& value)
{
	if (!object.isString() || object.stringValue() != value)
	{
		return false;
	}

	if (!object || object.isNull())
	{
		return false;
	}

	return true;
}

template <>
inline bool TestValue::verifyValue(const Value& object, const std::vector<uint8_t>& value)
{
	if (!object.isBuffer())
	{
		return false;
	}

	size_t size = 0;
	const void* data = object.bufferValue(size);

	if (size == value.size())
	{
		if (size >= 1 && memcmp(value.data(), data, size) != 0)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	if (!object || object.isNull())
	{
		return false;
	}

	return true;
}

template <typename T>
bool TestValue::verifyValue(const Value& /*object*/, const T& /*value*/)
{
	ocean_assert(false && "Missing implementation!");
	return false;
}

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_VALUE_H
