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

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestStaticBuffer::test(const double /*testDuration*/, const TestSelector& selector)
{
	TestResult testResult("StaticBuffer test");
	Log::info() << " ";

	if (selector.shouldRun("constructor"))
	{
		testResult = testConstructor();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("access"))
	{
		testResult = testAccess();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("clear"))
	{
		testResult = testClear();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("comparison"))
	{
		testResult = testComparison();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, testConstructor<int32_t, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testConstructor<int32_t, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testConstructor<int32_t, 10>(randomGenerator));

	OCEAN_EXPECT_TRUE(validation, testConstructor<uint8_t, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testConstructor<uint8_t, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testConstructor<uint8_t, 10>(randomGenerator));

	OCEAN_EXPECT_TRUE(validation, testConstructor<float, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testConstructor<float, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testConstructor<float, 10>(randomGenerator));

	OCEAN_EXPECT_TRUE(validation, testConstructor<std::string, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testConstructor<std::string, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testConstructor<std::string, 10>(randomGenerator));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStaticBuffer::testAccess()
{
	Log::info() << "Testing access:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testAccess<int32_t, 1>());
	OCEAN_EXPECT_TRUE(validation, testAccess<int32_t, 2>());
	OCEAN_EXPECT_TRUE(validation, testAccess<int32_t, 10>());

	OCEAN_EXPECT_TRUE(validation, testAccess<uint8_t, 1>());
	OCEAN_EXPECT_TRUE(validation, testAccess<uint8_t, 2>());
	OCEAN_EXPECT_TRUE(validation, testAccess<uint8_t, 10>());

	OCEAN_EXPECT_TRUE(validation, testAccess<float, 1>());
	OCEAN_EXPECT_TRUE(validation, testAccess<float, 2>());
	OCEAN_EXPECT_TRUE(validation, testAccess<float, 10>());

	OCEAN_EXPECT_TRUE(validation, testAccess<std::string, 1>());
	OCEAN_EXPECT_TRUE(validation, testAccess<std::string, 2>());
	OCEAN_EXPECT_TRUE(validation, testAccess<std::string, 10>());

	OCEAN_EXPECT_TRUE(validation, testAccess<Frame, 1>());
	OCEAN_EXPECT_TRUE(validation, testAccess<Frame, 2>());
	OCEAN_EXPECT_TRUE(validation, testAccess<Frame, 10>());

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStaticBuffer::testClear()
{
	Log::info() << "Testing clear:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, testClear<int32_t, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testClear<int32_t, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testClear<int32_t, 10>(randomGenerator));

	OCEAN_EXPECT_TRUE(validation, testClear<uint8_t, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testClear<uint8_t, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testClear<uint8_t, 10>(randomGenerator));

	OCEAN_EXPECT_TRUE(validation, testClear<float, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testClear<float, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testClear<float, 10>(randomGenerator));

	OCEAN_EXPECT_TRUE(validation, testClear<std::string, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testClear<std::string, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testClear<std::string, 10>(randomGenerator));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStaticBuffer::testComparison()
{
	Log::info() << "Testing comparison:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	OCEAN_EXPECT_TRUE(validation, testComparison<int32_t, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testComparison<int32_t, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testComparison<int32_t, 10>(randomGenerator));

	OCEAN_EXPECT_TRUE(validation, testComparison<uint8_t, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testComparison<uint8_t, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testComparison<uint8_t, 10>(randomGenerator));

	OCEAN_EXPECT_TRUE(validation, testComparison<float, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testComparison<float, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testComparison<float, 10>(randomGenerator));

	OCEAN_EXPECT_TRUE(validation, testComparison<std::string, 1>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testComparison<std::string, 2>(randomGenerator));
	OCEAN_EXPECT_TRUE(validation, testComparison<std::string, 10>(randomGenerator));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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
