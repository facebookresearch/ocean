/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/RandomI.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include <ctime>
#include <cstdlib>

namespace Ocean
{

/**
 * This class implements a simple helper class allowing to store thread ids.
 */
class RandomI::ThreadDatabase : public Singleton<ThreadDatabase>
{
	protected:

		/**
		 * Definition of a set holding thread ids.
		 */
		typedef std::set<Thread::ThreadId> ThreadIds;

	public:

		/**
		 * Returns whether the id of the calling thread has been added to this database.
		 * @return True, if so
		 */
		bool containsThread() const;

		/**
		 * Adds the calling thread to this database.
		 */
		void addThread();

	protected:

		/// The ids of the threads which have been added.
		ThreadIds threadIds_;

		/// The lock of this database.
		mutable Lock lock_;
};

bool RandomI::ThreadDatabase::containsThread() const
{
	const ScopedLock scopedLock(lock_);

	return threadIds_.find(Thread::currentThreadId()) != threadIds_.end();
}

void RandomI::ThreadDatabase::addThread()
{
	const ScopedLock scopedLock(lock_);

	threadIds_.insert(Thread::currentThreadId());
}

void RandomI::initialize()
{
#ifdef OCEAN_INTENSIVE_DEBUG
	// the following assert is intended to detect unintended calls of RandomI::initialize() (e.g., several times within the same thread at individual locations in the call stack)
	// it is highly recommended to get rid of multiple initialization calls
	// however, the system will not crash, it is a hint for the developer, so you just can resume debugging and fix the issue later
	ocean_assert(!ThreadDatabase::get().containsThread() && "This thread has been initialized already, simply ensure that you call RandomI::initialize() for each thread just only once e.g., during start of application, of when the thread is created!");

	ThreadDatabase::get().addThread();
#endif

	srand(timeBasedSeed());
}

void RandomI::initialize(const unsigned int value)
{
#ifdef OCEAN_INTENSIVE_DEBUG
	// we do not check whether this thread has been initialized already as we receive an explicit value (it may be intended)
	ThreadDatabase::get().addThread();
#endif

	srand(value);
}

uint32_t RandomI::random32()
{
#ifdef OCEAN_INTENSIVE_DEBUG
	ocean_assert(ThreadDatabase::get().containsThread() && "This thread has not been initialized yet, simply call RandomI::initialize() for this thread (just once) e.g., during start of application!");
#endif

#if defined(_WINDOWS)

	static_assert(RAND_MAX == 0x7FFFu, "Invalid RAND_MAX value!");

	// we have 15 random bits per rand() call
	return uint32_t(rand()) | uint32_t(rand()) << 15u | uint32_t(rand()) << 30u;

#elif defined(__GNUC__)

	static_assert(RAND_MAX == 0x7FFFFFFFu, "Invalid RAND_MAX value!");

	// we have 31 random bits per rand() call
	return uint32_t(rand()) | uint32_t(rand()) << 31u;

#else

	static_assert(RAND_MAX == 0xFFFFFFFFu, "Invalid RAND_MAX value!");

	return rand();

#endif

}

uint32_t RandomI::random32(RandomGenerator& generator)
{
	ocean_assert(generator.randMax() == 0x7FFFu); // This implementation assumes 15 random bits.

	return uint32_t(generator.rand()) | uint32_t(generator.rand()) << 15u | uint32_t(generator.rand()) << 30u;
}

uint64_t RandomI::random64()
{
#ifdef OCEAN_INTENSIVE_DEBUG
	ocean_assert(ThreadDatabase::get().containsThread() && "This thread has not been initialized yet, simply call RandomI::initialize() for this thread (just once) e.g., during start of application!");
#endif

#if defined(_WINDOWS)

	static_assert(RAND_MAX == 0x7FFFu, "Invalid RAND_MAX value!");

	// we have 15 random bits per rand() call
	return uint64_t(rand()) | uint64_t(rand()) << 15ull | uint64_t(rand()) << 30ull | uint64_t(rand()) << 45ull | uint64_t(rand()) << 60ull;

#elif defined(__GNUC__)

	static_assert(RAND_MAX == 0x7FFFFFFFu, "Invalid RAND_MAX value!");

	// we have 31 random bits per rand() call
	return uint64_t(rand()) | uint64_t(rand()) << 31ull | uint64_t(rand()) << 62ull;

#else

	static_assert(RAND_MAX == 0xFFFFFFFFFFFFFFFFull, "Invalid RAND_MAX value!");

	return rand();

#endif

}

uint64_t RandomI::random64(RandomGenerator& generator)
{
	ocean_assert(generator.randMax() == 0x7FFFu);

	// we have 15 random bits per rand() call
	return uint64_t(generator.rand()) | uint64_t(generator.rand()) << 15ull | uint64_t(generator.rand()) << 30ull | uint64_t(generator.rand()) << 45ull | uint64_t(generator.rand()) << 60ull;
}

unsigned int RandomI::random(const unsigned int maxValue)
{
#ifdef OCEAN_INTENSIVE_DEBUG
	ocean_assert(ThreadDatabase::get().containsThread() && "This thread has not been initialized yet, simply call RandomI::initialize() for this thread (just once) e.g., during start of application!");
#endif

	if (maxValue == (unsigned int)(-1))
	{
		return random32();
	}

	if (maxValue > randMax())
	{
		return random32() % (maxValue + 1u);
	}
	else
	{
#ifdef __GNUC__
		return (unsigned int)(int64_t(rand()) % int64_t(maxValue + 1u));
#else
		return rand() % (maxValue + 1u);
#endif
	}
}

int RandomI::random(const int lower, const int upper)
{
#ifdef OCEAN_INTENSIVE_DEBUG
	ocean_assert(ThreadDatabase::get().containsThread() && "This thread has not been initialized yet, simply call RandomI::initialize() for this thread (just once) e.g., during start of application!");
#endif

	ocean_assert(lower <= upper);
	ocean_assert(int64_t(upper) - int64_t(lower) <= int64_t(std::numeric_limits<int>::max()));

	const unsigned int range = upper - lower;

	if (range > randMax())
	{
		return lower + int(random32() % (range + 1u));
	}
	else
	{
#ifdef __GNUC__
		return int(int64_t(lower) + int64_t(rand() % (range + 1u)));
#else
		return lower + int((unsigned int)(rand()) % (range + 1u));
#endif
	}
}

unsigned int RandomI::random(const unsigned int lower, const unsigned int upper)
{
#ifdef OCEAN_INTENSIVE_DEBUG
	ocean_assert(ThreadDatabase::get().containsThread() && "This thread has not been initialized yet, simply call RandomI::initialize() for this thread (just once) e.g., during start of application!");
#endif

	ocean_assert(lower <= upper);

	const unsigned int range = upper - lower;

	if (range == (unsigned int)(-1))
	{
		ocean_assert(lower == 0u);
		ocean_assert(upper == (unsigned int)(-1));

		return random32();
	}

	if (range > randMax())
	{
		return lower + random32() % (range + 1u);
	}
	else
	{
#ifdef __GNUC__
		return lower + (rand() % (range + 1u));
#else
		return lower + (unsigned int)(rand()) % (range + 1u);
#endif
	}
}

void RandomI::random(const unsigned int maxValue, unsigned int& first, unsigned int& second)
{
	ocean_assert(maxValue >= 1u);

	if (maxValue == 1u)
	{
		first = random(1u);
		second = 1u - first;
	}
	else
	{
		first = random(maxValue);
		second = random(maxValue);

		while (first == second)
		{
			first = random(maxValue);

			if (first != second)
			{
				break;
			}

			second = random(maxValue);
		}
	}

	ocean_assert(first != second);
}

void RandomI::random(RandomGenerator& generator, const unsigned int maxValue, unsigned int& first, unsigned int& second)
{
	ocean_assert(maxValue >= 1u);

	if (maxValue == 1u)
	{
		first = random(generator, 1u);
		second = 1u - first;
	}
	else
	{
		first = random(generator, maxValue);
		second = random(generator, maxValue);

		while (first == second)
		{
			first = random(generator, maxValue);

			if (first != second)
			{
				break;
			}

			second = random(generator, maxValue);
		}
	}

	ocean_assert(first != second);
}

void RandomI::random(const unsigned int maxValue, unsigned int& first, unsigned int& second, unsigned int& third)
{
	ocean_assert(maxValue >= 2u);

	if (maxValue == 2u)
	{
		first = random(2u);

		do
		{
			second = random(2u);
		}
		while (first == second);

		do
		{
			third = random(2u);
		}
		while (first == third || second == third);
	}
	else
	{
		first = random(maxValue);
		second = random(maxValue);
		third = random(maxValue);

		while (first == second || first == third || second == third)
		{
			first = random(maxValue);
			second = random(maxValue);
			third = random(maxValue);
		}
	}

	ocean_assert(first != second && second != third && first != third);
}

void RandomI::random(RandomGenerator& generator, const unsigned int maxValue, unsigned int& first, unsigned int& second, unsigned int& third)
{
	ocean_assert(maxValue >= 2u);

	if (maxValue == 2u)
	{
		first = random(generator, 2u);

		do
		{
			second = random(generator, 2u);
		}
		while (first == second);

		do
		{
			third = random(generator, 2u);
		}
		while (first == third || second == third);
	}
	else
	{
		first = random(generator, maxValue);
		second = random(generator, maxValue);
		third = random(generator, maxValue);

		while (first == second || first == third || second == third)
		{
			first = random(generator, maxValue);
			second = random(generator, maxValue);
			third = random(generator, maxValue);
		}
	}

	ocean_assert(first != second && second != third && first != third);
}

unsigned int RandomI::timeBasedSeed()
{
	return (unsigned int)(time(nullptr));
}

}
