/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_RANDOM_I_H
#define META_OCEAN_BASE_RANDOM_I_H

#include "ocean/base/Base.h"
#include "ocean/base/RandomGenerator.h"

#include <limits>

namespace Ocean
{

/**
 * This class provides base random functions and several random functions for integer data types.
 * Beware: All function must not be used without a initialize() call for each thread!<br>
 * The performance of the random number generator may be very poor in multi-threaded environments.<br>
 * Therefore, use an instance of the RandomGenerator class in functions which are invoked in parallel.<br>
 * @see initialize().
 * @see RandomGenerator.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT RandomI
{
	protected:

		// Forward declaration.
		class ThreadDatabase;

	public:

		/**
		 * Initializes the standard random generator for this thread with a time-based seed value.
		 * Please ensure that you have called RandomI::initialize() just once (for each individual thread) before using any other function of this class.<br>
		 * You also my use a user-defined seed value e.g., to be able to reproduce the behavior of a system using random numbers, just use the second initialize function in this case.
		 */
		static void initialize();

		/**
		 * Initializes the standard random generator for this thread with a user-defined seed value.
		 * In contrast to the time-based initialization function, this function here can be invoked as often as necessary for any thread at any time.
		 * @param value Initialization value to be used, with range [0, infinity)
		 */
		static void initialize(const unsigned int value);

		/**
		 * Returns one random integer number with range [0x00000000, 0xFFFFFFFF].
		 * Use this function only if the standard random() functions with range parameters are not large enough.<br>
		 * @return Random integer number, with range [0, infinity)
		 */
		static uint32_t random32();

		/**
		 * Returns one random integer number with range [0x00000000, 0xFFFFFFFF] using an explicit random generator.
		 * Use this function only if the standard random() functions with range parameters are not large enough.<br>
		 * @param randomGenerator The random generator to be used
		 * @return Random integer number, with range [0, infinity)
		 */
		static uint32_t random32(RandomGenerator& randomGenerator);

		/**
		 * Returns one random integer number with range [0x00000000 00000000, 0xFFFFFFFF FFFFFFFF].
		 * Use this function only if the standard random() functions with range parameters are not large enough.<br>
		 * @return Random integer number, with range [0, infinity)
		 */
		static uint64_t random64();

		/**
		 * Returns one random integer number with range [0x00000000 00000000, 0xFFFFFFFF FFFFFFFF] using an explicit random generator.
		 * Use this function only if the standard random() functions with range parameters are not large enough.<br>
		 * @return Random integer number, with range [0, infinity)
		 */
		static uint64_t random64(RandomGenerator& randomGenerator);

		/**
		 * Returns one random integer value with specified maximum value.
		 * @param maxValue Maximum value, with range [0, infinity)
		 * @return Random integer number, with range [0, maxValue]
		 */
		static unsigned int random(const unsigned int maxValue);

		/**
		 * Returns one random integer value with specified maximum value using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param maxValue Maximum value, with range [0, infinity)
		 * @return Random integer number, with range [0, maxValue]
		 */
		static inline unsigned int random(RandomGenerator& randomGenerator, const unsigned int maxValue);

		/**
		 * Returns one random integer value within a specific range.
		 * @param lower The lower border, with range (-infinity, infinity)
		 * @param upper The upper border, with range [lower, infinity)
		 * @return Random integer number, with range [lower, lower + 2147483647]
		 */
		static int random(const int lower, const int upper);

		/**
		 * Returns one random integer value within a specific range.
		 * @param lower The lower border, with range [0, infinity)
		 * @param upper The upper border, with range [lower, infinity)
		 * @return Random integer number, with range [lower, upper]
		 */
		static unsigned int random(const unsigned int lower, const unsigned int upper);

		/**
		 * Returns one random integer value within a specific range using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param lower The lower border, with range (-infinity, infinity)
		 * @param upper The upper border, with range [lower, lower + 2147483647]
		 * @return Random integer number, with range [lower, upper]
		 */
		static inline int random(RandomGenerator& randomGenerator, const int lower, const int upper);

		/**
		 * Returns one random integer value within a specific range using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param lower The lower border, with range [0, infinity)
		 * @param upper The upper border, with range [lower, infinity)
		 * @return Random integer number, with range [lower, upper]
		 */
		static inline unsigned int random(RandomGenerator& randomGenerator, const unsigned int lower, const unsigned int upper);

		/**
		 * Returns two different random integer values with specified maximum value.
		 * @param maxValue Maximum value, must be large enough to ensure two different random numbers, with range [1, infinity)
		 * @param first Resulting first unique random number, with range [0, maxValue]
		 * @param second Resulting second unique random number, with range [0, maxValue]
		 */
		static void random(const unsigned int maxValue, unsigned int& first, unsigned int& second);

		/**
		 * Returns two different random integer values with specified maximum value using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param maxValue Maximum value, must be large enough to ensure three different random numbers, with range [1, infinity)
		 * @param first Resulting first unique random number, with range [0, maxValue]
		 * @param second Resulting second unique random number, with range [0, maxValue]
		 */
		static void random(RandomGenerator& randomGenerator, const unsigned int maxValue, unsigned int& first, unsigned int& second);

		/**
		 * Returns three different random integer values with specified maximum value.
		 * @param maxValue Maximum value, must be large enough to ensure three different random numbers, with range [2, infinity)
		 * @param first Resulting first unique random number, with range [0, maxValue]
		 * @param second Resulting second unique random number, with range [0, maxValue]
		 * @param third Resulting third unique random number, with range [0, maxValue]
		 */
		static void random(const unsigned int maxValue, unsigned int& first, unsigned int& second, unsigned int& third);

		/**
		 * Returns three different random integer values with specified maximum value using an explicit random generator.
		 * @param randomGenerator The random generator to be used
		 * @param maxValue Maximum value, must be large enough to ensure three different random numbers, with range [2, infinity)
		 * @param first Resulting first unique random number, with range [0, maxValue]
		 * @param second Resulting second unique random number, with range [0, maxValue]
		 * @param third Resulting third unique random number, with range [0, maxValue]
		 */
		static void random(RandomGenerator& randomGenerator, const unsigned int maxValue, unsigned int& first, unsigned int& second, unsigned int& third);

		/**
		 * Returns a random bool value.
		 * @return True or False
		 */
		static inline bool boolean();

		/**
		 * Returns a random bool value.
		 * @param randomGenerator The random generator to be used
		 * @return True or False
		 */
		static inline bool boolean(RandomGenerator& randomGenerator);

		/**
		 * Randomly returns one element from a given vector.
		 * @param elements The elements from which one elements will be chosen randomly, must contain at least one element
		 * @return The randomly selected element
		 * @tparam T The data type of the element
		 */
		template <typename T>
		static T random(const std::vector<T>& elements);

		/**
		 * Randomly returns one element from a given initializer list.
		 * @param elements The elements from which one elements will be chosen randomly, must contain at least one element
		 * @return The randomly selected element
		 * @tparam T The data type of the element
		 */
		template <typename T>
		static T random(const std::initializer_list<T>& elements);

		/**
		 * Randomly returns one element from a given vector.
		 * @param randomGenerator The random generator to be used
		 * @param elements The elements from which one elements will be chosen randomly, must contain at least one element
		 * @return The randomly selected element
		 * @tparam T The data type of the element
		 */
		template <typename T>
		static T random(RandomGenerator& randomGenerator, const std::vector<T>& elements);

		/**
		 * Randomly returns one element from a given initializer list.
		 * @param randomGenerator The random generator to be used
		 * @param elements The elements from which one elements will be chosen randomly, must contain at least one element
		 * @return The randomly selected element
		 * @tparam T The data type of the element
		 */
		template <typename T>
		static T random(RandomGenerator& randomGenerator, const std::initializer_list<T>& elements);

		/**
		 * Returns a seed value based on the current time.
		 * @return Time-based seed value
		 */
		static unsigned int timeBasedSeed();

	protected:

		/**
		 * Returns the maximal random value of the default random number generator.
		 * @return Maximal random value of the default generator
		 */
		static constexpr unsigned int randMax();
};

inline unsigned int RandomI::random(RandomGenerator& randomGenerator, const unsigned int maxValue)
{
	if (maxValue == (unsigned int)(-1))
	{
		return random32(randomGenerator);
	}

	if (maxValue > randomGenerator.randMax())
	{
		return random32(randomGenerator) % (maxValue + 1u);
	}
	else
	{
		return randomGenerator.rand() % (maxValue + 1u);
	}
}

inline int RandomI::random(RandomGenerator& randomGenerator, const int lower, const int upper)
{
	ocean_assert(lower <= upper);
	ocean_assert(int64_t(upper) - int64_t(lower) <= int64_t(std::numeric_limits<int>::max()));

	const unsigned int range = (unsigned int)(upper - lower);

	if (range > randomGenerator.randMax())
	{
		return lower + int(random32(randomGenerator) % (range + 1u));
	}
	else
	{
		return lower + int(randomGenerator.rand() % (range + 1u));
	}
}

inline unsigned int RandomI::random(RandomGenerator& randomGenerator, const unsigned int lower, const unsigned int upper)
{
	ocean_assert(lower <= upper);

	const unsigned int range = upper - lower;

	if (range == (unsigned int)(-1))
	{
		ocean_assert(lower == 0u);
		ocean_assert(upper == (unsigned int)(-1));

		return random32(randomGenerator);
	}

	if (range > randomGenerator.randMax())
	{
		return lower + random32(randomGenerator) % (range + 1u);
	}
	else
	{
		return lower + randomGenerator.rand() % (range + 1u);
	}
}

inline bool RandomI::boolean()
{
	return random(1u) == 0u;
}

inline bool RandomI::boolean(RandomGenerator& randomGenerator)
{
	return random(randomGenerator, 1u) == 0u;
}

template <typename T>
T RandomI::random(const std::vector<T>& elements)
{
	ocean_assert(!elements.empty());

	const unsigned int index = RandomI::random((unsigned int)(elements.size()) - 1u);

	return elements[index];
}

template <typename T>
T RandomI::random(const std::initializer_list<T>& elements)
{
	ocean_assert(elements.size() != 0);

	const unsigned int index = RandomI::random((unsigned int)(elements.size()) - 1u);

	return elements.begin()[index];
}

template <typename T>
T RandomI::random(RandomGenerator& randomGenerator, const std::vector<T>& elements)
{
	ocean_assert(!elements.empty());

	const unsigned int index = RandomI::random(randomGenerator, (unsigned int)(elements.size()) - 1u);

	return elements[index];
}

template <typename T>
T RandomI::random(RandomGenerator& randomGenerator, const std::initializer_list<T>& elements)
{
	ocean_assert(elements.size() != 0);

	const unsigned int index = RandomI::random(randomGenerator, (unsigned int)(elements.size()) - 1u);

	return elements.begin()[index];
}

constexpr unsigned int RandomI::randMax()
{
	return RAND_MAX;
}

}

#endif // META_OCEAN_BASE_RANDOM_I_H
