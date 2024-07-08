/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestStaticBuffer.h"

#include "ocean/base/Frame.h"
#include "ocean/base/StaticBuffer.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestStaticBuffer::test(const double /*testDuration*/)
{
	Log::info() << "---   StaticBuffer test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testConstructor() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAccess() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testClear() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testComparison() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "StaticBuffer test succeeded.";
	}
	else
	{
		Log::info() << "StaticBuffer test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestStaticBuffer, Constructor)
{
	EXPECT_TRUE(TestStaticBuffer::testConstructor());
}

TEST(TestStaticBuffer, Access)
{
	EXPECT_TRUE(TestStaticBuffer::testAccess());
}

TEST(TestStaticBuffer, Clear)
{
	EXPECT_TRUE(TestStaticBuffer::testClear());
}

TEST(TestStaticBuffer, Comparison)
{
	EXPECT_TRUE(TestStaticBuffer::testComparison());
}

#endif // OCEAN_USE_GTEST

bool TestStaticBuffer::testConstructor()
{
	Log::info() << "Testing constructor:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	allSucceeded = testConstructor<int32_t, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testConstructor<int32_t, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testConstructor<int32_t, 10>(randomGenerator) && allSucceeded;

	allSucceeded = testConstructor<uint8_t, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testConstructor<uint8_t, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testConstructor<uint8_t, 10>(randomGenerator) && allSucceeded;

	allSucceeded = testConstructor<float, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testConstructor<float, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testConstructor<float, 10>(randomGenerator) && allSucceeded;

	allSucceeded = testConstructor<std::string, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testConstructor<std::string, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testConstructor<std::string, 10>(randomGenerator) && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestStaticBuffer::testAccess()
{
	Log::info() << "Testing access:";

	bool allSucceeded = true;

	allSucceeded = testAccess<int32_t, 1>() && allSucceeded;
	allSucceeded = testAccess<int32_t, 2>() && allSucceeded;
	allSucceeded = testAccess<int32_t, 10>() && allSucceeded;

	allSucceeded = testAccess<uint8_t, 1>() && allSucceeded;
	allSucceeded = testAccess<uint8_t, 2>() && allSucceeded;
	allSucceeded = testAccess<uint8_t, 10>() && allSucceeded;

	allSucceeded = testAccess<float, 1>() && allSucceeded;
	allSucceeded = testAccess<float, 2>() && allSucceeded;
	allSucceeded = testAccess<float, 10>() && allSucceeded;

	allSucceeded = testAccess<std::string, 1>() && allSucceeded;
	allSucceeded = testAccess<std::string, 2>() && allSucceeded;
	allSucceeded = testAccess<std::string, 10>() && allSucceeded;

	allSucceeded = testAccess<Frame, 1>() && allSucceeded;
	allSucceeded = testAccess<Frame, 2>() && allSucceeded;
	allSucceeded = testAccess<Frame, 10>() && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestStaticBuffer::testClear()
{
	Log::info() << "Testing clear:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	allSucceeded = testClear<int32_t, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testClear<int32_t, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testClear<int32_t, 10>(randomGenerator) && allSucceeded;

	allSucceeded = testClear<uint8_t, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testClear<uint8_t, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testClear<uint8_t, 10>(randomGenerator) && allSucceeded;

	allSucceeded = testClear<float, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testClear<float, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testClear<float, 10>(randomGenerator) && allSucceeded;

	allSucceeded = testClear<std::string, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testClear<std::string, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testClear<std::string, 10>(randomGenerator) && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

bool TestStaticBuffer::testComparison()
{
	Log::info() << "Testing comparison:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	allSucceeded = testComparison<int32_t, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testComparison<int32_t, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testComparison<int32_t, 10>(randomGenerator) && allSucceeded;

	allSucceeded = testComparison<uint8_t, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testComparison<uint8_t, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testComparison<uint8_t, 10>(randomGenerator) && allSucceeded;

	allSucceeded = testComparison<float, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testComparison<float, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testComparison<float, 10>(randomGenerator) && allSucceeded;

	allSucceeded = testComparison<std::string, 1>(randomGenerator) && allSucceeded;
	allSucceeded = testComparison<std::string, 2>(randomGenerator) && allSucceeded;
	allSucceeded = testComparison<std::string, 10>(randomGenerator) && allSucceeded;

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

template <typename T, size_t tCapacity>
bool TestStaticBuffer::testConstructor(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	{
		std::vector<T> elementVector(tCapacity);

		for (size_t n = 0; n < tCapacity; ++n)
		{
			elementVector[n] = randomValue<T>(randomGenerator);
		}

		{
			const StaticBuffer<T, tCapacity> buffer(elementVector);

			if (buffer.capacity() != tCapacity)
			{
				return false;
			}

			for (size_t n = 0; n < tCapacity; ++n)
			{
				if (buffer[n] != elementVector[n])
				{
					return false;
				}
			}
		}

		{
			const StaticBuffer<T, tCapacity> buffer(elementVector.data());

			if (buffer.capacity() != tCapacity)
			{
				return false;
			}

			for (size_t n = 0; n < tCapacity; ++n)
			{
				if (buffer[n] != elementVector[n])
				{
					return false;
				}
			}
		}
	}

	{
		const T value = randomValue<T>(randomGenerator);

		{
			const StaticBuffer<T, tCapacity> buffer(value);

			if (buffer.capacity() != tCapacity)
			{
				return false;
			}

			if (buffer[0] != value)
			{
				return false;
			}
		}

		{
			const size_t number = size_t(RandomI::random(randomGenerator, (unsigned int)(tCapacity - 1)));

			const StaticBuffer<T, tCapacity> buffer(number, value);

			if (buffer.capacity() != tCapacity)
			{
				return false;
			}

			for (size_t n = 0; n < number; ++n)
			{
				if (buffer[n] != value)
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T, size_t tCapacity>
bool TestStaticBuffer::testAccess()
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	StaticBuffer<T, tCapacity> buffer;

	if (buffer.capacity() != tCapacity)
	{
		return false;
	}

	T* data = buffer.data();

	if (data == nullptr)
	{
		return false;
	}

	for (size_t n = 0; n < tCapacity; ++n)
	{
		if (&buffer[n] != data + n)
		{
			return false;
		}
	}

	if (&buffer.front() != data)
	{
		return false;
	}

	if (&buffer.back() != data + tCapacity - 1)
	{
		return false;
	}

	return true;
}

template <typename T, size_t tCapacity>
bool TestStaticBuffer::testClear(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	StaticBuffer<T, tCapacity> buffer;

	for (size_t n = 0; n < tCapacity; ++n)
	{
		buffer[n] = randomValue<T>(randomGenerator);
	}

	for (size_t n = 0; n < tCapacity; ++n)
	{
		if (buffer[n] == T())
		{
			return false;
		}
	}

	buffer.clear();

	for (size_t n = 0; n < tCapacity; ++n)
	{
		if (buffer[n] != T())
		{
			return false;
		}
	}

	return true;
}

template <typename T, size_t tCapacity>
bool TestStaticBuffer::testComparison(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	StaticBuffer<T, tCapacity> buffer;

	for (size_t n = 0; n < tCapacity; ++n)
	{
		buffer[n] = randomValue<T>(randomGenerator);
	}

	const StaticBuffer<T, tCapacity> buffer2(buffer);

	if (buffer != buffer2)
	{
		return false;
	}

	const StaticBuffer<T, tCapacity> buffer3 = buffer;

	if (buffer != buffer3)
	{
		return false;
	}

	StaticBuffer<T, tCapacity> buffer4 = buffer;

	size_t index = size_t(RandomI::random(randomGenerator, (unsigned int)(tCapacity - 1)));

	const T value = buffer[index];

	while (true)
	{
		T value2 = randomValue<T>(randomGenerator);

		if (value != value2)
		{
			buffer4[index] = std::move(value2);
			break;
		}
	}

	if (buffer == buffer4)
	{
		return false;
	}

	return true;
}

}

}

}
