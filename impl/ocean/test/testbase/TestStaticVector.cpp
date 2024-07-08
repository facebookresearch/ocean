/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestStaticVector.h"
#include "ocean/test/testbase/TestStaticBuffer.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/StaticVector.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestStaticVector::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   StaticVector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAccess(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testClear(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testResize(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testComparison(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "StaticVector test succeeded.";
	}
	else
	{
		Log::info() << "StaticVector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestStaticVector, Constructor)
{
	EXPECT_TRUE(TestStaticVector::testConstructor(GTEST_TEST_DURATION));
}

TEST(TestStaticVector, Access)
{
	EXPECT_TRUE(TestStaticVector::testAccess(GTEST_TEST_DURATION));
}

TEST(TestStaticVector, Clear)
{
	EXPECT_TRUE(TestStaticVector::testClear(GTEST_TEST_DURATION));
}

TEST(TestStaticVector, Resize)
{
	EXPECT_TRUE(TestStaticVector::testResize(GTEST_TEST_DURATION));
}

TEST(TestStaticVector, Comparison)
{
	EXPECT_TRUE(TestStaticVector::testComparison(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestStaticVector::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing constructor:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
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
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestStaticVector::testAccess(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing access:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		allSucceeded = testAccess<int32_t, 1>(randomGenerator) && allSucceeded;
		allSucceeded = testAccess<int32_t, 2>(randomGenerator) && allSucceeded;
		allSucceeded = testAccess<int32_t, 10>(randomGenerator) && allSucceeded;

		allSucceeded = testAccess<uint8_t, 1>(randomGenerator) && allSucceeded;
		allSucceeded = testAccess<uint8_t, 2>(randomGenerator) && allSucceeded;
		allSucceeded = testAccess<uint8_t, 10>(randomGenerator) && allSucceeded;

		allSucceeded = testAccess<float, 1>(randomGenerator) && allSucceeded;
		allSucceeded = testAccess<float, 2>(randomGenerator) && allSucceeded;
		allSucceeded = testAccess<float, 10>(randomGenerator) && allSucceeded;

		allSucceeded = testAccess<std::string, 1>(randomGenerator) && allSucceeded;
		allSucceeded = testAccess<std::string, 2>(randomGenerator) && allSucceeded;
		allSucceeded = testAccess<std::string, 10>(randomGenerator) && allSucceeded;
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestStaticVector::testClear(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing clear:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
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
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestStaticVector::testResize(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing resize:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		allSucceeded = testResize<int32_t, 1>(randomGenerator) && allSucceeded;
		allSucceeded = testResize<int32_t, 2>(randomGenerator) && allSucceeded;
		allSucceeded = testResize<int32_t, 10>(randomGenerator) && allSucceeded;

		allSucceeded = testResize<uint8_t, 1>(randomGenerator) && allSucceeded;
		allSucceeded = testResize<uint8_t, 2>(randomGenerator) && allSucceeded;
		allSucceeded = testResize<uint8_t, 10>(randomGenerator) && allSucceeded;

		allSucceeded = testResize<float, 1>(randomGenerator) && allSucceeded;
		allSucceeded = testResize<float, 2>(randomGenerator) && allSucceeded;
		allSucceeded = testResize<float, 10>(randomGenerator) && allSucceeded;

		allSucceeded = testResize<std::string, 1>(randomGenerator) && allSucceeded;
		allSucceeded = testResize<std::string, 2>(randomGenerator) && allSucceeded;
		allSucceeded = testResize<std::string, 10>(randomGenerator) && allSucceeded;
	}
	while (startTimestamp + testDuration > Timestamp(true));

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

bool TestStaticVector::testComparison(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing comparison:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
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
	}
	while (startTimestamp + testDuration > Timestamp(true));

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
bool TestStaticVector::testConstructor(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	{
		const size_t size = size_t(RandomI::random(randomGenerator, (unsigned int)(tCapacity)));

		std::vector<T> elementVector(size);

		for (T& element : elementVector)
		{
			element = TestStaticBuffer::randomValue<T>(randomGenerator);
		}

		{
			const StaticVector<T, tCapacity> staticVector(elementVector);

			if (staticVector.capacity() != tCapacity)
			{
				return false;
			}

			if (staticVector.size() != size)
			{
				return false;
			}

			if (staticVector.occupied() != (size == tCapacity))
			{
				return false;
			}

			for (size_t n = 0; n < size; ++n)
			{
				if (staticVector[n] != elementVector[n])
				{
					return false;
				}
			}
		}

		{
			const StaticVector<T, tCapacity> staticVector(elementVector.data(), elementVector.size());

			if (staticVector.capacity() != tCapacity)
			{
				return false;
			}

			if (staticVector.size() != size)
			{
				return false;
			}

			for (size_t n = 0; n < size; ++n)
			{
				if (staticVector[n] != elementVector[n])
				{
					return false;
				}
			}
		}

		{
			const std::vector<T> copyElementVector(elementVector);

			const StaticVector<T, tCapacity> staticVector(std::move(elementVector));

			if (staticVector.capacity() != tCapacity)
			{
				return false;
			}

			if (staticVector.size() != size)
			{
				return false;
			}

			for (size_t n = 0; n < size; ++n)
			{
				if (staticVector[n] != copyElementVector[n])
				{
					return false;
				}
			}
		}
	}

	{
		const T value = TestStaticBuffer::randomValue<T>(randomGenerator);

		{
			const StaticVector<T, tCapacity> staticVector(value);

			if (staticVector.capacity() != tCapacity)
			{
				return false;
			}

			if (staticVector.size() != 1)
			{
				return false;
			}

			if (staticVector[0] != value)
			{
				return false;
			}
		}

		{
			const size_t number = size_t(RandomI::random(randomGenerator, (unsigned int)(tCapacity - 1)));

			const StaticVector<T, tCapacity> staticVector(number, value);

			if (staticVector.capacity() != tCapacity)
			{
				return false;
			}

			if (staticVector.size() != number)
			{
				return false;
			}

			for (size_t n = 0; n < number; ++n)
			{
				if (staticVector[n] != value)
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T, size_t tCapacity>
bool TestStaticVector::testAccess(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	for (size_t size = 0; size < tCapacity; ++size)
	{
		StaticVector<T, tCapacity> staticVector(size, TestStaticBuffer::randomValue<T>(randomGenerator));

		if (staticVector.capacity() != tCapacity)
		{
			return false;
		}

		if (staticVector.size() != size)
		{
			return false;
		}

		if (staticVector.empty() != (size == 0))
		{
			return false;
		}

		T* data = staticVector.data();

		if (data == nullptr)
		{
			return false;
		}

		if (size == 0)
		{
			continue;
		}

		for (size_t n = 0; n < size; ++n)
		{
			if (&staticVector[n] != data + n)
			{
				return false;
			}
		}

		if (&staticVector.front() != data)
		{
			return false;
		}

		if (&staticVector.back() != data + size - 1)
		{
			return false;
		}
	}

	return true;
}

template <typename T, size_t tCapacity>
bool TestStaticVector::testClear(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	const size_t size = size_t(RandomI::random(randomGenerator, (unsigned int)(tCapacity)));

	std::vector<T> elementVector(size);

	for (T& element : elementVector)
	{
		element = TestStaticBuffer::randomValue<T>(randomGenerator);
	}

	{
		StaticVector<T, tCapacity> staticVector(elementVector);

		if (staticVector.capacity() != tCapacity || staticVector.size() != size)
		{
			return false;
		}

		staticVector.clear();

		if (staticVector.capacity() != tCapacity || staticVector.size() != 0)
		{
			return false;
		}

		const T* data = staticVector.data(); // always points to the buffer's data, valid even if vector is empty

		for (size_t n = 0; n < size; ++n)
		{
			if (data[n] != T())
			{
				return false;
			}
		}
	}

	{
		StaticVector<T, tCapacity> staticVector(elementVector);

		if (staticVector.capacity() != tCapacity || staticVector.size() != size)
		{
			return false;
		}

		staticVector.weakClear();

		if (staticVector.capacity() != tCapacity || staticVector.size() != 0)
		{
			return false;
		}

		const T* data = staticVector.data(); // always points to the buffer's data, valid even if vector is empty

		for (size_t n = 0; n < size; ++n)
		{
			if (data[n] != elementVector[n])
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T, size_t tCapacity>
bool TestStaticVector::testResize(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	const size_t size = size_t(RandomI::random(randomGenerator, (unsigned int)(tCapacity)));

	std::vector<T> elementVector(size);

	for (T& element : elementVector)
	{
		element = TestStaticBuffer::randomValue<T>(randomGenerator);
	}

	{
		// testing resize()

		StaticVector<T, tCapacity> staticVector(elementVector);

		if (staticVector.capacity() != tCapacity)
		{
			return false;
		}

		if (staticVector.size() != size)
		{
			return false;
		}

		const size_t newSize = size_t(RandomI::random(randomGenerator, (unsigned int)(tCapacity)));

		staticVector.resize(newSize);

		if (staticVector.capacity() != tCapacity)
		{
			return false;
		}

		if (staticVector.size() != newSize)
		{
			return false;
		}

		// we ensure that all old values still exists

		for (size_t n = 0; n < std::min(size, newSize); ++n)
		{
			if (staticVector[n] != elementVector[n])
			{
				return false;
			}
		}

		// we ensure that all new values are default

		for (size_t n = std::max(size, newSize); n < newSize; ++n)
		{
			if (staticVector[n] != T())
			{
				return false;
			}
		}
	}

	{
		// testing weakResize()

		StaticVector<T, tCapacity> staticVector(elementVector);

		if (staticVector.capacity() != tCapacity)
		{
			return false;
		}

		if (staticVector.size() != size)
		{
			return false;
		}

		const size_t newSize = size_t(RandomI::random(randomGenerator, (unsigned int)(tCapacity)));

		staticVector.weakResize(newSize);

		if (staticVector.capacity() != tCapacity)
		{
			return false;
		}

		if (staticVector.size() != newSize)
		{
			return false;
		}

		const T* data = staticVector.data(); // always points to the buffer's data, valid even if vector is empty

		// we ensure that all old values still exists

		for (size_t n = 0; n < size; ++n)
		{
			if (data[n] != elementVector[n])
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T, size_t tCapacity>
bool TestStaticVector::testComparison(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	StaticVector<T, tCapacity> staticVector;

	const size_t size = size_t(RandomI::random(randomGenerator, (unsigned int)(tCapacity)));

	staticVector.resize(size);

	for (size_t n = 0; n < size; ++n)
	{
		staticVector[n] = TestStaticBuffer::randomValue<T>(randomGenerator);
	}

	const StaticVector<T, tCapacity> staticVector2(staticVector);

	if (staticVector != staticVector2)
	{
		return false;
	}

	const StaticVector<T, tCapacity> staticVector3 = staticVector;

	if (staticVector != staticVector3)
	{
		return false;
	}

	if (size != 0)
	{
		StaticVector<T, tCapacity> staticVector4 = staticVector;

		size_t index = size_t(RandomI::random(randomGenerator, (unsigned int)(size - 1)));

		const T value = staticVector[index];

		while (true)
		{
			T value2 = TestStaticBuffer::randomValue<T>(randomGenerator);

			if (value != value2)
			{
				staticVector4[index] = std::move(value2);
				break;
			}
		}

		if (staticVector == staticVector4)
		{
			return false;
		}
	}

	while (true)
	{
		const size_t newSize = size_t(RandomI::random(randomGenerator, (unsigned int)(tCapacity)));

		if (newSize == size)
		{
			continue;
		}

		{
			StaticVector<T, tCapacity> staticVector5 = staticVector;
			staticVector5.resize(newSize);

			if (staticVector5 == staticVector)
			{
				return false;
			}
		}

		{
			StaticVector<T, tCapacity> staticVector5 = staticVector;
			staticVector5.weakResize(newSize);

			if (staticVector5 == staticVector)
			{
				return false;
			}
		}

		break;
	}

	return true;
}

}

}

}
