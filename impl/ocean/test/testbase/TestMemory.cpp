/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestMemory.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"
#include "ocean/base/Memory.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestMemory::test(const double testDuration, Worker& worker, const TestSelector& selector)
{
	TestResult testResult("Memory tests");

	Log::info() << " ";

	if (selector.shouldRun("object"))
	{
		testResult = testObject(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("allocation"))
	{
		testResult = testAllocation(testDuration, worker);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("isinside"))
	{
		testResult = testIsInside(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestMemory, Object)
{
	EXPECT_TRUE(TestMemory::testObject(GTEST_TEST_DURATION));
}

TEST(TestMemory, Allocation)
{
	Worker worker;
	EXPECT_TRUE(TestMemory::testAllocation(GTEST_TEST_DURATION, worker));
}

TEST(TestMemory, IsInside)
{
	EXPECT_TRUE(TestMemory::testIsInside(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestMemory::testObject(const double testDuration)
{
	Log::info() << "Memory object test:";

	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const size_t elements = size_t(std::min(RandomI::random(randomGenerator, 1024u * 1024u) + 1u, 1024u * 1024u));
		ocean_assert(elements >= 1);

		{
			// memory without explicit data type, without explicit memory alignment

			Memory memory(elements);

			OCEAN_EXPECT_FALSE(validation, memory.isReadOnly());
			OCEAN_EXPECT_TRUE(validation, memory.isOwner());
			OCEAN_EXPECT_EQUAL(validation, memory.constdata(), (const void*)(memory.data()));

			if (memory && memory.data() && !memory.isNull() && memory.size() == elements)
			{
				memset(memory.data(), 0x80, elements);

				for (size_t n = 0; n < elements; ++n)
				{
					OCEAN_EXPECT_EQUAL(validation, ((const uint8_t*)(memory.data()))[n], uint8_t(0x80));
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}


			// now, let's test the move operator

			Memory memoryMoved = std::move(memory);
			ocean_assert(memory.data() == nullptr);

			OCEAN_EXPECT_EQUAL(validation, memory.data(), (void*)(nullptr));
			OCEAN_EXPECT_EQUAL(validation, memory.constdata(), (const void*)(nullptr));

			OCEAN_EXPECT_FALSE(validation, memoryMoved.isReadOnly());
			OCEAN_EXPECT_TRUE(validation, memoryMoved.isOwner());
			OCEAN_EXPECT_EQUAL(validation, memoryMoved.constdata(), (const void*)(memoryMoved.data()));

			if (memoryMoved && memoryMoved.data() && !memoryMoved.isNull() && memoryMoved.size() == elements)
			{
				memset(memoryMoved.data(), 0x20, elements);

				for (size_t n = 0; n < elements; ++n)
				{
					OCEAN_EXPECT_EQUAL(validation, ((const uint8_t*)(memoryMoved.data()))[n], uint8_t(0x20));
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}

			// now, let's check the move constructor

			Memory memoryConstructorMoved(std::move(memoryMoved));
			ocean_assert(memoryMoved.data() == nullptr);

			OCEAN_EXPECT_EQUAL(validation, memoryMoved.data(), (void*)(nullptr));
			OCEAN_EXPECT_EQUAL(validation, memoryMoved.constdata(), (const void*)(nullptr));

			OCEAN_EXPECT_FALSE(validation, memoryConstructorMoved.isReadOnly());
			OCEAN_EXPECT_TRUE(validation, memoryConstructorMoved.isOwner());
			OCEAN_EXPECT_EQUAL(validation, memoryConstructorMoved.constdata(), (const void*)(memoryConstructorMoved.data()));

			if (memoryConstructorMoved && memoryConstructorMoved.data() && !memoryConstructorMoved.isNull() && memoryConstructorMoved.size() == elements)
			{
				memset(memoryConstructorMoved.data(), 0x10, elements);

				for (size_t n = 0; n < elements; ++n)
				{
					OCEAN_EXPECT_EQUAL(validation, ((const uint8_t*)(memoryConstructorMoved.data()))[n], uint8_t(0x10));
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		{
			// memory without explicit data type, but with explicit memory alignment

			const size_t memoryAlignment = size_t(RandomI::random(randomGenerator, 1u, 64u));

			Memory memory(elements, memoryAlignment);

			OCEAN_EXPECT_FALSE(validation, memory.isReadOnly());
			OCEAN_EXPECT_TRUE(validation, memory.isOwner());
			OCEAN_EXPECT_EQUAL(validation, memory.constdata(), (const void*)(memory.data()));

			if (memory.data() && (size_t(memory.data()) % memoryAlignment == size_t(0)) && memory.size() == elements)
			{
				memset(memory.data(), 0x80, elements);

				for (size_t n = 0; n < elements; ++n)
				{
					OCEAN_EXPECT_EQUAL(validation, ((const uint8_t*)(memory.data()))[n], uint8_t(0x80));
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}

			// now, let's test the move operator

			Memory memoryMoved = std::move(memory);
			ocean_assert(memory.data() == nullptr);

			OCEAN_EXPECT_EQUAL(validation, memory.data(), (void*)(nullptr));
			OCEAN_EXPECT_EQUAL(validation, memory.constdata(), (const void*)(nullptr));

			OCEAN_EXPECT_FALSE(validation, memoryMoved.isReadOnly());
			OCEAN_EXPECT_TRUE(validation, memoryMoved.isOwner());
			OCEAN_EXPECT_EQUAL(validation, memoryMoved.constdata(), (const void*)(memoryMoved.data()));

			if (memoryMoved.data() && (size_t(memoryMoved.data()) % memoryAlignment == size_t(0)))
			{
				memset(memoryMoved.data(), 0x20, elements);

				for (size_t n = 0; n < elements; ++n)
				{
					OCEAN_EXPECT_EQUAL(validation, ((const uint8_t*)(memoryMoved.data()))[n], uint8_t(0x20));
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}

			// now, let's check the move constructor

			Memory memoryConstructorMoved(std::move(memoryMoved));
			ocean_assert(memoryMoved.data() == nullptr);

			OCEAN_EXPECT_EQUAL(validation, memoryMoved.data(), (void*)(nullptr));
			OCEAN_EXPECT_EQUAL(validation, memoryMoved.constdata(), (const void*)(nullptr));

			OCEAN_EXPECT_FALSE(validation, memoryConstructorMoved.isReadOnly());
			OCEAN_EXPECT_TRUE(validation, memoryConstructorMoved.isOwner());
			OCEAN_EXPECT_EQUAL(validation, memoryConstructorMoved.constdata(), (const void*)(memoryConstructorMoved.data()));

			if (memoryConstructorMoved.data() && (size_t(memoryConstructorMoved.data()) % memoryAlignment == size_t(0)) && memoryConstructorMoved.size() == elements)
			{
				memset(memoryConstructorMoved.data(), 0x10, elements);

				for (size_t n = 0; n < elements; ++n)
				{
					OCEAN_EXPECT_EQUAL(validation, ((const uint8_t*)(memoryConstructorMoved.data()))[n], uint8_t(0x10));
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		{
			// writable memory with used data

			Memory memoryOwner(1024 * 1024 * 50);

			Memory writableMemory(memoryOwner.data(), memoryOwner.size());

			OCEAN_EXPECT_FALSE(validation, writableMemory.isNull());
			OCEAN_EXPECT_FALSE(validation, writableMemory.isReadOnly());
			OCEAN_EXPECT_FALSE(validation, writableMemory.isOwner());

			Memory movedWritableMemory(std::move(writableMemory));

			OCEAN_EXPECT_TRUE(validation, writableMemory.isNull());
			OCEAN_EXPECT_EQUAL(validation, writableMemory.data(), (void*)(nullptr));
			OCEAN_EXPECT_EQUAL(validation, writableMemory.constdata(), (const void*)(nullptr));

			OCEAN_EXPECT_FALSE(validation, movedWritableMemory.isNull());
			OCEAN_EXPECT_FALSE(validation, movedWritableMemory.isReadOnly());
			OCEAN_EXPECT_FALSE(validation, movedWritableMemory.isOwner());

			OCEAN_EXPECT_NOT_EQUAL(validation, movedWritableMemory.data(), (void*)(nullptr));
			OCEAN_EXPECT_NOT_EQUAL(validation, movedWritableMemory.constdata(), (const void*)(nullptr));
		}

		{
			// read-only memory with used data

			Memory memoryOwner(1024 * 1024 * 50);

			Memory readOnlyMemory(memoryOwner.constdata(), memoryOwner.size());

			OCEAN_EXPECT_FALSE(validation, readOnlyMemory.isNull());
			OCEAN_EXPECT_TRUE(validation, readOnlyMemory.isReadOnly());
			OCEAN_EXPECT_FALSE(validation, readOnlyMemory.isOwner());

			Memory movedReadOnlyMemory(std::move(readOnlyMemory));

			OCEAN_EXPECT_TRUE(validation, readOnlyMemory.isNull());
			OCEAN_EXPECT_EQUAL(validation, readOnlyMemory.data(), (void*)(nullptr));
			OCEAN_EXPECT_EQUAL(validation, readOnlyMemory.constdata(), (const void*)(nullptr));

			OCEAN_EXPECT_FALSE(validation, movedReadOnlyMemory.isNull());
			OCEAN_EXPECT_TRUE(validation, movedReadOnlyMemory.isReadOnly());
			OCEAN_EXPECT_FALSE(validation, movedReadOnlyMemory.isOwner());

			OCEAN_EXPECT_EQUAL(validation, movedReadOnlyMemory.data(), (void*)(nullptr));
			OCEAN_EXPECT_NOT_EQUAL(validation, movedReadOnlyMemory.constdata(), (const void*)(nullptr));
		}

		{
			// memory with explicit data type

			Memory memory = Memory::create<uint8_t>(elements);

			OCEAN_EXPECT_FALSE(validation, memory.isReadOnly());
			OCEAN_EXPECT_TRUE(validation, memory.isOwner());
			OCEAN_EXPECT_EQUAL(validation, memory.constdata(), (const void*)(memory.data()));

			if (memory.data<uint8_t>() && (size_t(memory.data<uint8_t>()) % sizeof(uint8_t) == size_t(0)) && (size_t(memory.data()) % sizeof(uint8_t) == size_t(0)) && memory.size() == elements * sizeof(uint8_t))
			{
				memset(memory.data<uint8_t>(), 0x00, elements * sizeof(uint8_t));

				for (size_t n = 0; n < elements; ++n)
				{
					OCEAN_EXPECT_EQUAL(validation, ((const uint8_t*)(memory.data<uint8_t>()))[n], uint8_t(0));
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		{
			// memory with explicit data type

			Memory memory = Memory::create<int16_t>(elements);

			OCEAN_EXPECT_FALSE(validation, memory.isReadOnly());
			OCEAN_EXPECT_TRUE(validation, memory.isOwner());
			OCEAN_EXPECT_EQUAL(validation, memory.constdata(), (const void*)(memory.data()));

			if (memory.data<int16_t>() && (size_t(memory.data<int16_t>()) % sizeof(int16_t) == size_t(0)) && (size_t(memory.data()) % sizeof(int16_t) == size_t(0)) && memory.size() == elements * sizeof(int16_t))
			{
				memset(memory.data<int16_t>(), 0x00, elements * sizeof(int16_t));

				for (size_t n = 0; n < elements; ++n)
				{
					OCEAN_EXPECT_EQUAL(validation, ((const int16_t*)(memory.data<int16_t>()))[n], int16_t(0));
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		{
			// memory with explicit data type

			Memory memory = Memory::create<int32_t>(elements);

			OCEAN_EXPECT_FALSE(validation, memory.isReadOnly());
			OCEAN_EXPECT_TRUE(validation, memory.isOwner());
			OCEAN_EXPECT_EQUAL(validation, memory.constdata(), (const void*)(memory.data()));

			if (memory.data<int32_t>() && (size_t(memory.data<int32_t>()) % sizeof(int32_t) == size_t(0)) && (size_t(memory.data()) % sizeof(int32_t) == size_t(0)) && memory.size() == elements * sizeof(int32_t))
			{
				memset(memory.data<int32_t>(), 0x00, elements * sizeof(int32_t));

				for (size_t n = 0; n < elements; ++n)
				{
					OCEAN_EXPECT_EQUAL(validation, ((const int32_t*)(memory.data<int32_t>()))[n], int32_t(0));
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		{
			// memory with explicit data type

			Memory memory = Memory::create<float>(elements);

			OCEAN_EXPECT_FALSE(validation, memory.isReadOnly());
			OCEAN_EXPECT_TRUE(validation, memory.isOwner());
			OCEAN_EXPECT_EQUAL(validation, memory.constdata(), (const void*)(memory.data()));

			if (memory.data<float>() && (size_t(memory.data<float>()) % sizeof(float) == size_t(0)) && (size_t(memory.data()) % sizeof(float) == size_t(0)))
			{
				memset(memory.data<float>(), 0x00, elements * sizeof(float));

				for (size_t n = 0; n < elements; ++n)
				{
					OCEAN_EXPECT_EQUAL(validation, ((const float*)(memory.data<float>()))[n], 0.0f);
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		{
			// memory with explicit data type

			Memory memory = Memory::create<double>(elements);

			OCEAN_EXPECT_FALSE(validation, memory.isReadOnly());
			OCEAN_EXPECT_TRUE(validation, memory.isOwner());
			OCEAN_EXPECT_EQUAL(validation, memory.constdata(), (const void*)(memory.data()));

			if (memory.data<double>() && (size_t(memory.data<double>()) % sizeof(double) == size_t(0)) && (size_t(memory.data()) % sizeof(double) == size_t(0)) && memory.size() == elements * sizeof(double))
			{
				memset(memory.data<double>(), 0x00, elements * sizeof(double));

				for (size_t n = 0; n < elements; ++n)
				{
					OCEAN_EXPECT_EQUAL(validation, ((const double*)(memory.data<double>()))[n], 0.0);
				}
			}
			else
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMemory::testAllocation(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Allocation test:";
	Log::info() << " ";

#ifdef OCEAN_HARDWARE_REDUCED_PERFORMANCE

	const unsigned int memSize[] =
	{
		512u * 512u,
		5u * 1024u * 1024u,
		10u * 1024u * 1024u
	};

	const unsigned int tests = 3u;

#else

	const unsigned int memSize[] =
	{
		512u * 512u,
		1024u * 1024u,
		5u * 1024u * 1024u,
		10u * 1024u * 1024u,
		100u * 1024u * 1024u,
		500u * 1024u * 1024u
	};

	unsigned int tests = 6u;

#endif // OCEAN_HARDWARE_REDUCED_PERFORMANCE

	int8_t* b = new int8_t[memSize[tests - 1u]];

	Timestamp startTimestamp;
	const double testDurationHalf = testDuration * 0.5;

	for (unsigned int i = 0u; i < tests; ++i)
	{
		Log::info().newLine(i != 0u);

		HighPerformanceStatistic performanceNew;
		HighPerformanceStatistic performanceMalloc;
		HighPerformanceStatistic performanceMemSet;
		HighPerformanceStatistic performanceMemSetShared;
		HighPerformanceStatistic performanceMemCopy;
		HighPerformanceStatistic performanceMemCopyShared;

		startTimestamp.toNow();

		do
		{
			performanceNew.start();
			int8_t* buffer = new int8_t[memSize[i]];
			buffer[0] = 0;
			performanceNew.stop();

			performanceMemSet.start();
			memset(buffer, 0x13, memSize[i]);
			performanceMemSet.stop();

			performanceMemCopy.start();
			memcpy(b, buffer, memSize[i]);
			performanceMemCopy.stop();

			if (worker)
			{
				performanceMemSetShared.start();
				Memory::memset(buffer, 0x13, memSize[i], &worker);
				performanceMemSetShared.stop();

				performanceMemCopyShared.start();
				Memory::memcpy(b, buffer, memSize[i], &worker);
				performanceMemCopyShared.stop();
			}

			delete [] buffer;
		}
		while (startTimestamp + testDurationHalf > Timestamp(true));

		startTimestamp.toNow();

		do
		{
			performanceMalloc.start();
			int8_t* buffer = (int8_t*) malloc(memSize[i]);
			buffer[0] = 0;
			performanceMalloc.stop();

			performanceMemSet.start();
			memset(buffer, 0x13, memSize[i]);
			performanceMemSet.stop();

			performanceMemCopy.start();
			memcpy(b, buffer, memSize[i]);
			performanceMemCopy.stop();

			if (worker)
			{
				performanceMemSetShared.start();
				Memory::memset(buffer, 0x13, memSize[i], &worker);
				performanceMemSetShared.stop();

				performanceMemCopyShared.start();
				Memory::memcpy(b, buffer, memSize[i], &worker);
				performanceMemCopyShared.stop();
			}

			free(buffer);
		}
		while (startTimestamp + testDurationHalf > Timestamp(true));

		startTimestamp.toNow();

		do
		{
			performanceNew.start();
			int8_t* buffer = new int8_t[memSize[i]];
			buffer[0] = 0;
			performanceNew.stop();

			performanceMemSet.start();
			memset(buffer, 0x13, memSize[i]);
			performanceMemSet.stop();

			performanceMemCopy.start();
			memcpy(b, buffer, memSize[i]);
			performanceMemCopy.stop();

			if (worker)
			{
				performanceMemSetShared.start();
				Memory::memset(buffer, 0x13, memSize[i], &worker);
				performanceMemSetShared.stop();

				performanceMemCopyShared.start();
				Memory::memcpy(b, buffer, memSize[i], &worker);
				performanceMemCopyShared.stop();
			}

			delete [] buffer;
		}
		while (startTimestamp + testDurationHalf > Timestamp(true));

		startTimestamp.toNow();

		do
		{
			performanceMalloc.start();
			int8_t* buffer = (int8_t*) malloc(memSize[i]);
			buffer[0] = 0;
			performanceMalloc.stop();

			performanceMemSet.start();
			memset(buffer, 0x13, memSize[i]);
			performanceMemSet.stop();

			performanceMemCopy.start();
			memcpy(b, buffer, memSize[i]);
			performanceMemCopy.stop();

			if (worker)
			{
				performanceMemSetShared.start();
				Memory::memset(buffer, 0x13, memSize[i], &worker);
				performanceMemSetShared.stop();

				performanceMemCopyShared.start();
				Memory::memcpy(b, buffer, memSize[i], &worker);
				performanceMemCopyShared.stop();
			}

			free(buffer);
		}
		while (startTimestamp + testDurationHalf > Timestamp(true));

		Log::info() << "Memory size: " << memSize[i] / 1024 << "KB (" << memSize[i] / (1024 * 1024) << "MB)";
		Log::info() << "Performance new: best " << performanceNew.bestMseconds() * 1000.0 << "ys, average " << performanceNew.averageMseconds() * 1000.0 << "mys";
		Log::info() << "Performance malloc: best " << performanceMalloc.bestMseconds() * 1000.0 << "ys, average " << performanceMalloc.averageMseconds() * 1000.0 << "mys";
		Log::info() << "Performance memset: best " << performanceMemSet.bestMseconds() << "ms, average " << performanceMemSet.averageMseconds() << "ms";
		Log::info() << "Performance memset (multicore): best " << performanceMemSetShared.bestMseconds() << "ms, average " << performanceMemSetShared.averageMseconds() << "ms";
		Log::info() << "Performance memcpy: best " << performanceMemCopy.bestMseconds() << "ms, average " << performanceMemCopy.averageMseconds() << "ms";
		Log::info() << "Performance memcpy (multicore): best " << performanceMemCopyShared.bestMseconds() << "ms, average " << performanceMemCopyShared.averageMseconds() << "ms";
	}

	delete [] b;

	return true;
}

bool TestMemory::testIsInside(const double testDuration)
{
	Log::info() << "IsInside test:";
	Log::info() << " ";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	Timestamp startTimestamp (true);

	do
	{
		const size_t memorySize = size_t(RandomI::random(randomGenerator, 1u, 1024u));

		const Memory memory(memorySize);

		const size_t pointer = size_t(memory.constdata());

		{
			// a memory pointer partially outside of the memory (to the left)

			//               [       memory       ]
			// [   memoryOutside   ]

			const size_t outsideOffset = size_t(RandomI::random(randomGenerator, 1, 1024));

			ocean_assert_accuracy(pointer >= outsideOffset);
			if (pointer >= outsideOffset)
			{
				const uint8_t* memoryOutside = memory.constdata<uint8_t>() - outsideOffset;
				const size_t memoryOutsideSize = size_t(RandomI::random(randomGenerator, 1u, 1024u));

				OCEAN_EXPECT_FALSE(validation, memory.isInside(memoryOutside, memoryOutsideSize));

				OCEAN_EXPECT_FALSE(validation, memory.isInside(memoryOutside, memoryOutside + memoryOutsideSize));

				OCEAN_EXPECT_TRUE(validation, memory.isInside(memoryOutside, size_t(0))); // an empty range is always inside the memory
			}
		}


		{
			// a memory pointer entirely outside of the memory (to the right)

			// [       memory       ]    [   memoryOutside   ]

			const uint8_t* memoryOutside = memory.constdata<uint8_t>() + memory.size();
			const size_t memoryOutsideSize = size_t(RandomI::random(randomGenerator, 1u, 1024u));

			OCEAN_EXPECT_FALSE(validation, memory.isInside(memoryOutside, memoryOutsideSize));

			OCEAN_EXPECT_FALSE(validation, memory.isInside(memoryOutside, memoryOutside + memoryOutsideSize));

			OCEAN_EXPECT_TRUE(validation, memory.isInside(memoryOutside, size_t(0))); // an empty range is always inside the memory
		}

		{
			// a memory pointer partially outside of the memory (to the right)

			//  [       memory       ]
			//                    [   memoryOutside   ]

			const size_t offset = size_t(RandomI::random(randomGenerator, 1024u));

			const uint8_t* memoryOutside = memory.constdata<uint8_t>() + offset;
			const size_t memoryOutsideSize = size_t(RandomI::random(randomGenerator, std::max(1, int(memory.size()) - int(offset)) + 1, 2048));

			OCEAN_EXPECT_FALSE(validation, memory.isInside(memoryOutside, memoryOutsideSize));

			OCEAN_EXPECT_FALSE(validation, memory.isInside(memoryOutside, memoryOutside + memoryOutsideSize));

			OCEAN_EXPECT_TRUE(validation, memory.isInside(memoryOutside, size_t(0))); // an empty range is always inside the memory
		}

		{
			// a memory entirely inside

			// [       memory              ]
			//     [   memoryOutside   ]

			const size_t offset = size_t(RandomI::random(randomGenerator, 0, int(memory.size()) - 1));

			const uint8_t* memoryInside = memory.constdata<uint8_t>() + offset;
			const size_t memoryInsideSize = size_t(RandomI::random(randomGenerator, 1, int(memory.size()) - int(offset)));

			OCEAN_EXPECT_TRUE(validation, memory.isInside(memoryInside, memoryInsideSize));

			OCEAN_EXPECT_TRUE(validation, memory.isInside(memoryInside, memoryInside + memoryInsideSize));

			OCEAN_EXPECT_TRUE(validation, memory.isInside(memoryInside, size_t(0))); // an empty range is always inside the memory
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
