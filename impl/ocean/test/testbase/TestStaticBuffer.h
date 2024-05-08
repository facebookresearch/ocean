/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_STATIC_BUFFER_H
#define META_OCEAN_TEST_TESTBASE_TEST_STATIC_BUFFER_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements tests for StaticBuffer.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestStaticBuffer
{
	friend class TestStaticVector;

	public:

		/**
		 * Invokes all tests.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the constructor functions.
		 * @return True, if succeeded
		 */
		static bool testConstructor();

		/**
		 * Tests the access functions.
		 * @return True, if succeeded
		 */
		static bool testAccess();

		/**
		 * Tests the clear function.
		 * @return True, if succeeded
		 */
		static bool testClear();

		/**
		 * Tests the comparison functions.
		 * @return True, if succeeded
		 */
		static bool testComparison();

	protected:

		/**
		 * Tests the constructor functions.
		 * @param randomGenerator The random generator to be used
		 * @return True, if succeeded
		 * @tparam T The data type of the buffer
		 * @tparam tCapacity The capacity of the buffer, with range [1, infinity)
		 */
		template <typename T, size_t tCapacity>
		static bool testConstructor(RandomGenerator& randomGenerator);

		/**
		 * Tests the access functions.
		 * @return True, if succeeded
		 * @tparam T The data type of the buffer
		 * @tparam tCapacity The capacity of the buffer, with range [1, infinity)
		 */
		template <typename T, size_t tCapacity>
		static bool testAccess();

		/**
		 * Tests the clear function.
		 * @param randomGenerator The random generator to be used
		 * @return True, if succeeded
		 * @tparam T The data type of the buffer
		 * @tparam tCapacity The capacity of the buffer, with range [1, infinity)
		 */
		template <typename T, size_t tCapacity>
		static bool testClear(RandomGenerator& randomGenerator);

		/**
		 * Tests the comparison functions.
		 * @param randomGenerator The random generator to be used
		 * @return True, if succeeded
		 * @tparam T The data type of the buffer
		 * @tparam tCapacity The capacity of the buffer, with range [1, infinity)
		 */
		template <typename T, size_t tCapacity>
		static bool testComparison(RandomGenerator& randomGenerator);

		/**
		 * Returns a random value.
		 * @param randomGenerator The random generator to be used
		 * @return The random value
		 * @tparam T The data type of the value
		 */
		template <typename T>
		static inline T randomValue(RandomGenerator& randomGenerator);
};

template <>
inline int32_t TestStaticBuffer::randomValue(RandomGenerator& randomGenerator)
{
	while (true)
	{
		const int32_t value = RandomI::random(randomGenerator, -1000, 1000);

		if (value != 0)
		{
			return value;
		}
	}
}

template <>
inline uint8_t TestStaticBuffer::randomValue(RandomGenerator& randomGenerator)
{
	return uint8_t(RandomI::random(randomGenerator, 1u, 255u));
}

template <>
inline float TestStaticBuffer::randomValue(RandomGenerator& randomGenerator)
{
	while (true)
	{
		const float value = float(RandomI::random(randomGenerator, -1000, 1000));

		if (value != 0.0f)
		{
			return value;
		}
	}
}

template <>
inline std::string TestStaticBuffer::randomValue(RandomGenerator& randomGenerator)
{
	return String::toAString(RandomI::random(randomGenerator, 1000u));
}

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_STATIC_BUFFER_H
