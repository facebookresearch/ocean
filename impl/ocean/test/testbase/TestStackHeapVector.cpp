/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestStackHeapVector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/StackHeapVector.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestStackHeapVector::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   StackHeapVector test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAssign(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPushBack(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testResize(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPerformance(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "StackHeapVector test succeeded.";
	}
	else
	{
		Log::info() << "StackHeapVector test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestStackHeapVector, Constructor)
{
	EXPECT_TRUE(TestStackHeapVector::testConstructor(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, Assign)
{
	EXPECT_TRUE(TestStackHeapVector::testAssign(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, PushBack)
{
	EXPECT_TRUE(TestStackHeapVector::testPushBack(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, Resize)
{
	EXPECT_TRUE(TestStackHeapVector::testResize(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, Performance)
{
	EXPECT_TRUE(TestStackHeapVector::testPerformance(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestStackHeapVector::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing constructor:";

	bool allSucceeded = true;

	allSucceeded = testConstructor<1>(testDuration) && allSucceeded;
	allSucceeded = testConstructor<2>(testDuration) && allSucceeded;
	allSucceeded = testConstructor<3>(testDuration) && allSucceeded;
	allSucceeded = testConstructor<4>(testDuration) && allSucceeded;
	allSucceeded = testConstructor<5>(testDuration) && allSucceeded;
	allSucceeded = testConstructor<6>(testDuration) && allSucceeded;
	allSucceeded = testConstructor<7>(testDuration) && allSucceeded;
	allSucceeded = testConstructor<8>(testDuration) && allSucceeded;
	allSucceeded = testConstructor<31>(testDuration) && allSucceeded;
	allSucceeded = testConstructor<32>(testDuration) && allSucceeded;
	allSucceeded = testConstructor<64>(testDuration) && allSucceeded;

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

bool TestStackHeapVector::testAssign(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing assign:";

	bool allSucceeded = true;

	allSucceeded = testAssign<1>(testDuration) && allSucceeded;
	allSucceeded = testAssign<2>(testDuration) && allSucceeded;
	allSucceeded = testAssign<3>(testDuration) && allSucceeded;
	allSucceeded = testAssign<4>(testDuration) && allSucceeded;
	allSucceeded = testAssign<5>(testDuration) && allSucceeded;
	allSucceeded = testAssign<6>(testDuration) && allSucceeded;
	allSucceeded = testAssign<7>(testDuration) && allSucceeded;
	allSucceeded = testAssign<8>(testDuration) && allSucceeded;
	allSucceeded = testAssign<31>(testDuration) && allSucceeded;
	allSucceeded = testAssign<32>(testDuration) && allSucceeded;
	allSucceeded = testAssign<64>(testDuration) && allSucceeded;

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

bool TestStackHeapVector::testPushBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing push back:";

	bool allSucceeded = true;

	allSucceeded = testPushBack<1>(testDuration) && allSucceeded;
	allSucceeded = testPushBack<2>(testDuration) && allSucceeded;
	allSucceeded = testPushBack<3>(testDuration) && allSucceeded;
	allSucceeded = testPushBack<4>(testDuration) && allSucceeded;
	allSucceeded = testPushBack<5>(testDuration) && allSucceeded;
	allSucceeded = testPushBack<6>(testDuration) && allSucceeded;
	allSucceeded = testPushBack<7>(testDuration) && allSucceeded;
	allSucceeded = testPushBack<8>(testDuration) && allSucceeded;
	allSucceeded = testPushBack<31>(testDuration) && allSucceeded;
	allSucceeded = testPushBack<32>(testDuration) && allSucceeded;
	allSucceeded = testPushBack<64>(testDuration) && allSucceeded;

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

bool TestStackHeapVector::testResize(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing resize:";

	bool allSucceeded = true;

	allSucceeded = testResize<1>(testDuration) && allSucceeded;
	allSucceeded = testResize<2>(testDuration) && allSucceeded;
	allSucceeded = testResize<3>(testDuration) && allSucceeded;
	allSucceeded = testResize<4>(testDuration) && allSucceeded;
	allSucceeded = testResize<5>(testDuration) && allSucceeded;
	allSucceeded = testResize<6>(testDuration) && allSucceeded;
	allSucceeded = testResize<7>(testDuration) && allSucceeded;
	allSucceeded = testResize<8>(testDuration) && allSucceeded;
	allSucceeded = testResize<31>(testDuration) && allSucceeded;
	allSucceeded = testResize<32>(testDuration) && allSucceeded;
	allSucceeded = testResize<64>(testDuration) && allSucceeded;

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

bool TestStackHeapVector::testPerformance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing performance:";

	bool allSucceeded = true;

	allSucceeded = testPerformance<1>(testDuration) && allSucceeded;
	allSucceeded = testPerformance<2>(testDuration) && allSucceeded;
	allSucceeded = testPerformance<3>(testDuration) && allSucceeded;
	allSucceeded = testPerformance<4>(testDuration) && allSucceeded;
	allSucceeded = testPerformance<5>(testDuration) && allSucceeded;
	allSucceeded = testPerformance<6>(testDuration) && allSucceeded;
	allSucceeded = testPerformance<7>(testDuration) && allSucceeded;
	allSucceeded = testPerformance<8>(testDuration) && allSucceeded;
	allSucceeded = testPerformance<31>(testDuration) && allSucceeded;
	allSucceeded = testPerformance<32>(testDuration) && allSucceeded;
	allSucceeded = testPerformance<64>(testDuration) && allSucceeded;

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

template <size_t tStackSize>
bool TestStackHeapVector::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// default constructor

			const StackHeapVector<uint64_t, tStackSize> defaultVector;

			OCEAN_EXPECT_TRUE(validation, defaultVector.isEmpty());
			OCEAN_EXPECT_EQUAL(validation, defaultVector.size(), size_t(0));
		}

		{
			// default constructor

			const StackHeapVector<Frame, tStackSize> defaultVector;

			OCEAN_EXPECT_TRUE(validation, defaultVector.isEmpty());
			OCEAN_EXPECT_EQUAL(validation, defaultVector.size(), size_t(0));
		}

		{
			// default constructor

			const StackHeapVector<std::string, tStackSize> defaultVector;

			OCEAN_EXPECT_TRUE(validation, defaultVector.isEmpty());
			OCEAN_EXPECT_EQUAL(validation, defaultVector.size(), size_t(0));
		}

		{
			const size_t numberElements = size_t(RandomI::random(randomGenerator, 1000u));

			std::string value;

			if (RandomI::boolean(randomGenerator))
			{
				value = String::toAString(RandomI::random64(randomGenerator));
			}

			const StackHeapVector<std::string, tStackSize> stackHeapVector(numberElements, value);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), numberElements);
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.isEmpty(), numberElements == 0);

			for (size_t n = 0; n < numberElements; ++n)
			{
				if (stackHeapVector[n] != value)
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			size_t counter = 0;

			for (const std::string& element : stackHeapVector)
			{
				OCEAN_EXPECT_EQUAL(validation, element, value);

				++counter;
			}

			OCEAN_EXPECT_EQUAL(validation, counter, numberElements);

			counter = 0;

			StackHeapVector<std::string, tStackSize> copyStackHeapVector(stackHeapVector);

			for (std::string& element : copyStackHeapVector)
			{
				OCEAN_EXPECT_EQUAL(validation, element, value);

				++counter;
			}

			OCEAN_EXPECT_EQUAL(validation, counter, numberElements);

			copyStackHeapVector.clear();

			OCEAN_EXPECT_TRUE(validation, copyStackHeapVector.isEmpty());
			OCEAN_EXPECT_EQUAL(validation, copyStackHeapVector.size(), size_t(0));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackSize>
bool TestStackHeapVector::testAssign(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		StackHeapVector<uint64_t, tStackSize> stackHeapVector;

		{
			const size_t capacity = size_t(RandomI::random(randomGenerator, 100u));

			stackHeapVector.setCapacity(capacity);

			const size_t expectedCapacity = std::max(tStackSize, capacity);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.capacity(), expectedCapacity);

			const uint64_t value = RandomI::random64(randomGenerator);

			const size_t size = size_t(RandomI::random(randomGenerator, 100u));

			stackHeapVector.assign(size, value);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), size);

			for (size_t n = 0; n < stackHeapVector.size(); ++n)
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], value);
			}

			for (const uint64_t element : stackHeapVector)
			{
				OCEAN_EXPECT_EQUAL(validation, element, value);
			}
		}

		{
			const size_t capacity = size_t(RandomI::random(randomGenerator, 100u));

			const size_t expectedCapacity = std::max(std::max(tStackSize, capacity), std::max(stackHeapVector.size(), stackHeapVector.capacity()));

			stackHeapVector.setCapacity(capacity);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.capacity(), expectedCapacity);

			const size_t size = size_t(RandomI::random(randomGenerator, 100u));

			const uint64_t value = RandomI::random64(randomGenerator);

			stackHeapVector.assign(size, value);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), size);

			for (size_t n = 0; n < stackHeapVector.size(); ++n)
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], value);
			}

			for (const uint64_t element : stackHeapVector)
			{
				OCEAN_EXPECT_EQUAL(validation, element, value);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackSize>
bool TestStackHeapVector::testPushBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		StackHeapVector<std::string, tStackSize> stackHeapVector;

		const size_t insertSize = size_t(RandomI::random(randomGenerator, 10));

		for (size_t n = 0; n < insertSize; ++n)
		{
			stackHeapVector.pushBack(String::toAString(n));

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), n + 1);
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], String::toAString(n));
		}

		const size_t removeSize = size_t(RandomI::random(randomGenerator, (unsigned int)(insertSize)));

		for (size_t n = 0; n < removeSize; ++n)
		{
			stackHeapVector.popBack();

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), insertSize - n - 1);
		}

		const size_t expectedSize = insertSize - removeSize;
		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), expectedSize);

		const size_t insertSize2 = size_t(RandomI::random(randomGenerator, 10));

		for (size_t n = 0; n < insertSize2; ++n)
		{
			stackHeapVector.emplaceBack(String::toAString(n));

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), expectedSize + n + 1);
		}

		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), expectedSize + insertSize2);

		for (size_t n = 0; n < stackHeapVector.size(); ++n)
		{
			if (n < expectedSize)
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], String::toAString(n));
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], String::toAString(n - expectedSize));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackSize>
bool TestStackHeapVector::testResize(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const size_t initialSize = size_t(RandomI::random(randomGenerator, 10));

		StackHeapVector<std::string, tStackSize> stackHeapVector(initialSize, std::string("i"));

		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), initialSize);

		for (const std::string& element : stackHeapVector)
		{
			OCEAN_EXPECT_EQUAL(validation, element, std::string("i"));
		}

		const size_t resize0 = size_t(RandomI::random(randomGenerator, 10));

		stackHeapVector.resize(resize0);

		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), resize0);

		for (size_t n = 0; n < stackHeapVector.size(); ++n)
		{
			if (n < initialSize)
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], std::string("i"));
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, stackHeapVector[n].empty());
			}
		}

		const size_t resize1 = size_t(RandomI::random(randomGenerator, 10));

		stackHeapVector.resize(resize1);

		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), resize1);

		for (size_t n = 0; n < stackHeapVector.size(); ++n)
		{
			if (n < std::min(initialSize, resize0))
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], std::string("i"));
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, stackHeapVector[n].empty());
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackSize>
bool TestStackHeapVector::testPerformance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr size_t iterations = 1000;

	Log::info() << "... with stack size " << tStackSize << ":";

	RandomGenerator randomGenerator;

	for (const bool stayBelowStackSize : {false, true})
	{
		if (stayBelowStackSize)
		{
			Log::info() << "Staying below stack size:";
		}
		else
		{
			Log::info() << "Extending stack size:";
		}

		HighPerformanceStatistic performanceDefault;
		HighPerformanceStatistic performanceStackHeap;

		const Timestamp startTimestamp(true);

		do
		{
			const size_t size = stayBelowStackSize ? RandomI::random(randomGenerator, 1u, (unsigned int)(tStackSize)) : RandomI::random(randomGenerator, (unsigned int)(tStackSize + 1u), (unsigned int)(tStackSize * 2u));
			const bool emplace = RandomI::boolean(randomGenerator);

			performanceStackHeap.start();

			for (size_t i = 0; i < iterations; ++i)
			{
				StackHeapVector<Frame::Plane, tStackSize> stackHeapVector;

				if (emplace)
				{
					for (size_t n = 0; n < size; ++n)
					{
						stackHeapVector.emplaceBack(Frame::Plane());
					}
				}
				else
				{
					for (size_t n = 0; n < size; ++n)
					{
						stackHeapVector.pushBack(Frame::Plane());
					}
				}
			}

			performanceStackHeap.stop();


			performanceDefault.start();

			for (size_t i = 0; i < iterations; ++i)
			{
				std::vector<Frame::Plane> vector;

				if (emplace)
				{
					for (size_t n = 0; n < size; ++n)
					{
						vector.emplace_back(Frame::Plane());
					}
				}
				else
				{
					for (size_t n = 0; n < size; ++n)
					{
						vector.push_back(Frame::Plane());
					}
				}
			}

			performanceDefault.stop();
		}
		while (!startTimestamp.hasTimePassed(testDuration));

		Log::info() << "Default performance: " << performanceDefault;
		Log::info() << "Stack-heap performance: " << performanceStackHeap;
	}

	Log::info() << " ";

	return true;
}

}

}

}
