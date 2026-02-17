/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestStaticVector.h"
#include "ocean/test/testbase/TestStaticBuffer.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/StaticVector.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestStaticVector::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("StaticVector test");
	Log::info() << " ";

	if (selector.shouldRun("constructor"))
	{
		testResult = testConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("access"))
	{
		testResult = testAccess(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("clear"))
	{
		testResult = testClear(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("resize"))
	{
		testResult = testResize(testDuration);

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

	if (selector.shouldRun("pushback"))
	{
		testResult = testPushBack(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("emplaceback"))
	{
		testResult = testEmplaceBack(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("popback"))
	{
		testResult = testPopBack(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("erase"))
	{
		testResult = testErase(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("iterator"))
	{
		testResult = testIterator(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

TEST(TestStaticVector, PushBack)
{
	EXPECT_TRUE(TestStaticVector::testPushBack(GTEST_TEST_DURATION));
}

TEST(TestStaticVector, EmplaceBack)
{
	EXPECT_TRUE(TestStaticVector::testEmplaceBack(GTEST_TEST_DURATION));
}

TEST(TestStaticVector, PopBack)
{
	EXPECT_TRUE(TestStaticVector::testPopBack(GTEST_TEST_DURATION));
}

TEST(TestStaticVector, Erase)
{
	EXPECT_TRUE(TestStaticVector::testErase(GTEST_TEST_DURATION));
}

TEST(TestStaticVector, Iterator)
{
	EXPECT_TRUE(TestStaticVector::testIterator(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestStaticVector::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing constructor:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
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
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStaticVector::testAccess(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing access:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		OCEAN_EXPECT_TRUE(validation, testAccess<int32_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testAccess<int32_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testAccess<int32_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testAccess<uint8_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testAccess<uint8_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testAccess<uint8_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testAccess<float, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testAccess<float, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testAccess<float, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testAccess<std::string, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testAccess<std::string, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testAccess<std::string, 10>(randomGenerator));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStaticVector::testClear(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing clear:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
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
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStaticVector::testResize(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing resize:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		OCEAN_EXPECT_TRUE(validation, testResize<int32_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testResize<int32_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testResize<int32_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testResize<uint8_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testResize<uint8_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testResize<uint8_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testResize<float, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testResize<float, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testResize<float, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testResize<std::string, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testResize<std::string, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testResize<std::string, 10>(randomGenerator));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStaticVector::testComparison(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing comparison:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
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
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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

bool TestStaticVector::testPushBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing pushBack:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		OCEAN_EXPECT_TRUE(validation, testPushBack<int32_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPushBack<int32_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPushBack<int32_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testPushBack<uint8_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPushBack<uint8_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPushBack<uint8_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testPushBack<float, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPushBack<float, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPushBack<float, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testPushBack<std::string, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPushBack<std::string, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPushBack<std::string, 10>(randomGenerator));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStaticVector::testEmplaceBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing emplaceBack:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<int32_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<int32_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<int32_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<uint8_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<uint8_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<uint8_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<float, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<float, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<float, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<std::string, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<std::string, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testEmplaceBack<std::string, 10>(randomGenerator));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStaticVector::testPopBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing popBack:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		OCEAN_EXPECT_TRUE(validation, testPopBack<int32_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPopBack<int32_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPopBack<int32_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testPopBack<uint8_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPopBack<uint8_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPopBack<uint8_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testPopBack<float, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPopBack<float, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPopBack<float, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testPopBack<std::string, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPopBack<std::string, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testPopBack<std::string, 10>(randomGenerator));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStaticVector::testErase(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing erase:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		OCEAN_EXPECT_TRUE(validation, testErase<int32_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testErase<int32_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testErase<int32_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testErase<uint8_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testErase<uint8_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testErase<uint8_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testErase<float, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testErase<float, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testErase<float, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testErase<std::string, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testErase<std::string, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testErase<std::string, 10>(randomGenerator));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T, size_t tCapacity>
bool TestStaticVector::testPushBack(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	{
		// pushBack()

		StaticVector<T, tCapacity> staticVector;

		std::vector<T> expectedValues;

		for (size_t n = 0; n < tCapacity; ++n)
		{
			const T value = TestStaticBuffer::randomValue<T>(randomGenerator);
			expectedValues.push_back(value);

			if (n % 2 == 0)
			{
				staticVector.pushBack(value);
			}
			else
			{
				T movableValue = value;
				staticVector.pushBack(std::move(movableValue));
			}

			if (staticVector.size() != n + 1)
			{
				return false;
			}

			if (staticVector[n] != expectedValues[n])
			{
				return false;
			}
		}

		if (!staticVector.occupied())
		{
			return false;
		}
	}

	{
		// securePushBack()

		StaticVector<T, tCapacity> staticVector;

		for (size_t n = 0; n < tCapacity; ++n)
		{
			const T value = TestStaticBuffer::randomValue<T>(randomGenerator);

			if (n % 2 == 0)
			{
				if (!staticVector.securePushBack(value))
				{
					return false;
				}
			}
			else
			{
				T movableValue = value;
				if (!staticVector.securePushBack(std::move(movableValue)))
				{
					return false;
				}
			}
		}

		const T extraValue = TestStaticBuffer::randomValue<T>(randomGenerator);

		if (staticVector.securePushBack(extraValue))
		{
			return false;
		}

		T movableExtraValue = extraValue;

		if (staticVector.securePushBack(std::move(movableExtraValue)))
		{
			return false;
		}
	}

	if constexpr (tCapacity >= 3)
	{
		// pushBack() with vector

		StaticVector<T, tCapacity> staticVector;

		staticVector.pushBack(TestStaticBuffer::randomValue<T>(randomGenerator));

		std::vector<T> vectorToPush;
		const size_t pushCount = std::min(size_t(3), tCapacity - staticVector.size());

		for (size_t n = 0; n < pushCount; ++n)
		{
			vectorToPush.push_back(TestStaticBuffer::randomValue<T>(randomGenerator));
		}

		const size_t sizeBefore = staticVector.size();
		staticVector.pushBack(vectorToPush);

		if (staticVector.size() != sizeBefore + pushCount)
		{
			return false;
		}

		for (size_t n = 0; n < pushCount; ++n)
		{
			if (staticVector[sizeBefore + n] != vectorToPush[n])
			{
				return false;
			}
		}
	}

	if constexpr (tCapacity >= 3)
	{
		// pushBack() with static vector

		StaticVector<T, tCapacity> staticVector1;
		staticVector1.pushBack(TestStaticBuffer::randomValue<T>(randomGenerator));

		StaticVector<T, 3> staticVector2;
		const size_t pushCount = std::min(size_t(2), tCapacity - staticVector1.size());

		for (size_t n = 0; n < pushCount; ++n)
		{
			staticVector2.pushBack(TestStaticBuffer::randomValue<T>(randomGenerator));
		}

		const size_t sizeBefore = staticVector1.size();
		staticVector1.pushBack(staticVector2);

		if (staticVector1.size() != sizeBefore + pushCount)
		{
			return false;
		}

		for (size_t n = 0; n < pushCount; ++n)
		{
			if (staticVector1[sizeBefore + n] != staticVector2[n])
			{
				return false;
			}
		}
	}

	{
		// bool operator

		StaticVector<T, tCapacity> staticVector;

		if (staticVector)
		{
			return false;
		}

		staticVector.pushBack(TestStaticBuffer::randomValue<T>(randomGenerator));

		if (!staticVector)
		{
			return false;
		}
	}

	return true;
}

template <typename T, size_t tCapacity>
bool TestStaticVector::testEmplaceBack(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	{
		// emplaceBack()

		StaticVector<T, tCapacity> staticVector;

		std::vector<T> expectedValues;

		for (size_t n = 0; n < tCapacity; ++n)
		{
			const T value = TestStaticBuffer::randomValue<T>(randomGenerator);
			expectedValues.push_back(value);

			// Emplace using copy constructor
			staticVector.emplaceBack(value);

			if (staticVector.size() != n + 1)
			{
				return false;
			}

			if (staticVector[n] != expectedValues[n])
			{
				return false;
			}
		}

		if (!staticVector.occupied())
		{
			return false;
		}
	}

	{
		// secureEmplaceBack()

		StaticVector<T, tCapacity> staticVector;

		for (size_t n = 0; n < tCapacity; ++n)
		{
			const T value = TestStaticBuffer::randomValue<T>(randomGenerator);

			if (!staticVector.secureEmplaceBack(value))
			{
				return false;
			}
		}

		const T extraValue = TestStaticBuffer::randomValue<T>(randomGenerator);

		if (staticVector.secureEmplaceBack(extraValue))
		{
			return false;
		}
	}

	if constexpr (std::is_same<T, std::string>::value)
	{
		// Test with std::string multi-argument constructor
		StaticVector<std::string, tCapacity> staticVector;

		// Emplace a string constructed with (count, char)
		staticVector.emplaceBack(5, 'x');

		if (staticVector.size() != 1)
		{
			return false;
		}

		if (staticVector[0] != "xxxxx")
		{
			return false;
		}
	}

	return true;
}

template <typename T, size_t tCapacity>
bool TestStaticVector::testPopBack(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	{
		// popBack()

		StaticVector<T, tCapacity> staticVector;

		std::vector<T> expectedValues;

		for (size_t n = 0; n < tCapacity; ++n)
		{
			const T value = TestStaticBuffer::randomValue<T>(randomGenerator);
			expectedValues.push_back(value);
			staticVector.pushBack(value);
		}

		for (size_t n = tCapacity; n > 0; --n)
		{
			if (staticVector.size() != n)
			{
				return false;
			}

			staticVector.popBack();

			if (staticVector.size() != n - 1)
			{
				return false;
			}

			const T* data = staticVector.data();
			if (data[n - 1] != T())
			{
				return false;
			}
		}

		if (!staticVector.empty())
		{
			return false;
		}
	}

	{
		// securePopBack()

		StaticVector<T, tCapacity> staticVector;

		for (size_t n = 0; n < tCapacity; ++n)
		{
			staticVector.pushBack(TestStaticBuffer::randomValue<T>(randomGenerator));
		}

		for (size_t n = tCapacity; n > 0; --n)
		{
			staticVector.securePopBack();

			if (staticVector.size() != n - 1)
			{
				return false;
			}
		}

		// calling on empty vector should be safe
		staticVector.securePopBack();

		if (staticVector.size() != 0)
		{
			return false;
		}
	}

	{
		// weakPopBack()

		StaticVector<T, tCapacity> staticVector;

		std::vector<T> expectedValues;

		for (size_t n = 0; n < tCapacity; ++n)
		{
			const T value = TestStaticBuffer::randomValue<T>(randomGenerator);
			expectedValues.push_back(value);
			staticVector.pushBack(value);
		}

		for (size_t n = tCapacity; n > 0; --n)
		{
			staticVector.weakPopBack();

			if (staticVector.size() != n - 1)
			{
				return false;
			}

			// Verify the element was NOT reset (weak variant)
			const T* data = staticVector.data();
			if (data[n - 1] != expectedValues[n - 1])
			{
				return false;
			}
		}
	}

	{
		// secureWeakPopBack()

		StaticVector<T, tCapacity> staticVector;

		for (size_t n = 0; n < tCapacity; ++n)
		{
			staticVector.pushBack(TestStaticBuffer::randomValue<T>(randomGenerator));
		}

		for (size_t n = tCapacity; n > 0; --n)
		{
			staticVector.secureWeakPopBack();

			if (staticVector.size() != n - 1)
			{
				return false;
			}
		}

		// calling on empty vector should be safe
		staticVector.secureWeakPopBack();

		if (staticVector.size() != 0)
		{
			return false;
		}
	}

	return true;
}

template <typename T, size_t tCapacity>
bool TestStaticVector::testErase(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	if constexpr (tCapacity >= 3)
	{
		// erase()

		StaticVector<T, tCapacity> staticVector;

		std::vector<T> expectedValues;

		for (size_t n = 0; n < tCapacity; ++n)
		{
			const T value = TestStaticBuffer::randomValue<T>(randomGenerator);
			expectedValues.push_back(value);
			staticVector.pushBack(value);
		}

		const size_t eraseIndex = tCapacity / 2;
		staticVector.erase(eraseIndex);
		expectedValues.erase(expectedValues.begin() + eraseIndex);

		if (staticVector.size() != tCapacity - 1)
		{
			return false;
		}

		for (size_t n = 0; n < staticVector.size(); ++n)
		{
			if (staticVector[n] != expectedValues[n])
			{
				return false;
			}
		}

		const T* data = staticVector.data();
		if (data[staticVector.size()] != T())
		{
			return false;
		}

		staticVector.erase(0);
		expectedValues.erase(expectedValues.begin());

		if (staticVector.size() != tCapacity - 2)
		{
			return false;
		}

		for (size_t n = 0; n < staticVector.size(); ++n)
		{
			if (staticVector[n] != expectedValues[n])
			{
				return false;
			}
		}

		staticVector.erase(staticVector.size() - 1);

		if (staticVector.size() != tCapacity - 3)
		{
			return false;
		}
	}

	if constexpr (tCapacity >= 3)
	{
		// unstableErase()

		StaticVector<T, tCapacity> staticVector;

		for (size_t n = 0; n < tCapacity; ++n)
		{
			staticVector.pushBack(TestStaticBuffer::randomValue<T>(randomGenerator));
		}

		const size_t eraseIndex = 1;
		const T lastElement = staticVector.back();

		staticVector.unstableErase(eraseIndex);

		if (staticVector.size() != tCapacity - 1)
		{
			return false;
		}

		if (staticVector[eraseIndex] != lastElement)
		{
			return false;
		}

		const T* data = staticVector.data();
		if (data[staticVector.size()] != T())
		{
			return false;
		}

		const size_t sizeBefore = staticVector.size();
		staticVector.unstableErase(staticVector.size() - 1);

		if (staticVector.size() != sizeBefore - 1)
		{
			return false;
		}
	}

	return true;
}

bool TestStaticVector::testIterator(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing iterator:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		OCEAN_EXPECT_TRUE(validation, testIterator<int32_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testIterator<int32_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testIterator<int32_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testIterator<uint8_t, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testIterator<uint8_t, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testIterator<uint8_t, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testIterator<float, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testIterator<float, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testIterator<float, 10>(randomGenerator));

		OCEAN_EXPECT_TRUE(validation, testIterator<std::string, 1>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testIterator<std::string, 2>(randomGenerator));
		OCEAN_EXPECT_TRUE(validation, testIterator<std::string, 10>(randomGenerator));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <typename T, size_t tCapacity>
bool TestStaticVector::testIterator(RandomGenerator& randomGenerator)
{
	static_assert(tCapacity >= 1, "Invalid capacity");

	for (size_t size = 0; size <= tCapacity; ++size)
	{
		StaticVector<T, tCapacity> staticVector;
		std::vector<T> expectedValues;

		for (size_t n = 0; n < size; ++n)
		{
			const T value = TestStaticBuffer::randomValue<T>(randomGenerator);
			staticVector.pushBack(value);
			expectedValues.push_back(value);
		}

		{
			T* beginPtr = staticVector.begin();
			T* endPtr = staticVector.end();

			if (beginPtr != staticVector.data())
			{
				return false;
			}

			if (endPtr != staticVector.data() + size)
			{
				return false;
			}

			if (static_cast<size_t>(endPtr - beginPtr) != size)
			{
				return false;
			}
		}

		{
			const StaticVector<T, tCapacity>& constStaticVector = staticVector;

			const T* beginPtr = constStaticVector.begin();
			const T* endPtr = constStaticVector.end();

			if (beginPtr != constStaticVector.data())
			{
				return false;
			}

			if (endPtr != constStaticVector.data() + size)
			{
				return false;
			}

			if (static_cast<size_t>(endPtr - beginPtr) != size)
			{
				return false;
			}
		}

		{
			size_t index = 0;

			for (T& element : staticVector)
			{
				if (index >= expectedValues.size())
				{
					return false;
				}

				if (element != expectedValues[index])
				{
					return false;
				}

				++index;
			}

			if (index != size)
			{
				return false;
			}
		}

		{
			const StaticVector<T, tCapacity>& constStaticVector = staticVector;

			size_t index = 0;

			for (const T& element : constStaticVector)
			{
				if (index >= expectedValues.size())
				{
					return false;
				}

				if (element != expectedValues[index])
				{
					return false;
				}

				++index;
			}

			if (index != size)
			{
				return false;
			}
		}

		if (size > 0)
		{
			for (T& element : staticVector)
			{
				element = TestStaticBuffer::randomValue<T>(randomGenerator);
			}

			size_t index = 0;

			for (T* it = staticVector.begin(); it != staticVector.end(); ++it)
			{
				if (*it != staticVector[index])
				{
					return false;
				}

				++index;
			}
		}
	}

	return true;
}

}

}

}
