/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_RANDOM_GENERATOR_H
#define META_OCEAN_BASE_RANDOM_GENERATOR_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"

#include <cstdlib>

namespace Ocean
{

/**
 * This class implements a generator for random numbers.
 * A random generator object can be used to improve the performance of code (needing random numbers) which is applied on several CPU cores in parallel.<br>
 * The default random number functions of the standard library are thread-safe but may apply expensive locks to synchronize the individual seed parameters of the individual threads.<br>
 * Therefore, this class can be used to improve the code performance significantly in multi-threaded environments.<br>
 * The following code example shows the correct application of this class for multi-core functions (which can be invoked e.g., by the Worker class):<br>
 * @code
 * void multiCoreFunction(RandomGenerator& randomGenerator, unsigned int* data, unsigned int firstObject, unsigned int numberObjects)
 * {
 *     /// create a local random generator object which uses the function's random generator to create a new seed value
 *     RandomGenerator localRandomGenerator(randomGenerator);
 *
 *     for (unsigned int n = firstObject; n <= firstObject + numberObjects; ++n)
 *     {
 *         /// we create random values and use the local random generator (not the function's random generator)
 *         data[n] = RandomI::random(localRandomGenerator, 100u);
 *     }
 * }
 * @endcode
 * @see RandomI, Worker.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT RandomGenerator
{
	public:

		/**
		 * Creates a new random generator and initializes the internal parameter with the random value of the standard random function.
		 * Ensure that RandomI::initialize() has been called before using this constructor.
		 * @see RandomI::initialize().
		 */
		RandomGenerator();

		/**
		 * Creates a new random generator and initializes the internal parameter by a random value provided by the locked random function of the given generator.
		 * @param generator Random number generator used for initialization, the generator's seed will be changed during the initialization
		 */
		explicit inline RandomGenerator(RandomGenerator& generator);

		/**
		 * Creates a new random generator and optional initializes the internal parameter by a random value provided by the locked random function of the given generator, if no generator is provided the standard random function will be used for initialization.
		 * @param optionalGenerator The random number generator used for initialization (the generator's seed will be changed during the initialization), nullptr to use the standard random function for initialization
		 */
		explicit RandomGenerator(RandomGenerator* optionalGenerator);

		/**
		 * Move constructor.
		 * @param randomGenerator The random generator object to be moved
		 */
		inline RandomGenerator(RandomGenerator&& randomGenerator) noexcept;

		/**
		 * Creates a new random generator and initializes the internal parameter by the given value.
		 * @param seed The seed initialization value, with range [0, infinity)
		 */
		explicit inline RandomGenerator(const unsigned int seed);

		/**
		 * Returns the next random number.
		 * Beware: This function is not thread safe.
		 * @return Random number with range [0, 4294967294] == [0, 2^32-2]
		 * @see lockedRand().
		 */
		inline unsigned int rand();

		/**
		 * Returns the next random number.
		 * This function is thread safe.
		 * @return Random number with range [0, 4294967294] == [0, 2^32-2]
		 * @see rand().
		 */
		inline unsigned int lockedRand();

		/**
		 * Returns the current seed value of this object.
		 * This seed value changes whenever a new random number is generated.
		 * @return Current seed value
		 */
		inline unsigned int seed() const;

		/**
		 * Returns the initial seed value which was used to initialize this random generator.
		 * The initial seed value will not change during the lifetime of the generator.
		 * @return The random generator's initial seed value
		 */
		inline unsigned int initialSeed() const;

		/**
		 * Move operator.
		 * @param randomGenerator The random generator object to be moved
		 * @return Reference to this object
		 */
		RandomGenerator& operator=(RandomGenerator&& randomGenerator);

		/**
		 * Generates a new random number using the xorshift32 algorithm.
		 * This function updates the seed in place and returns the new random value.
		 * The xorshift32 algorithm produces values in the range [1, 2^32-1], so we subtract 1 to get [0, 2^32-2].
		 * @param seed The seed value to update, with range [1, infinity)
		 * @return The new random number in range [0, 4294967294] == [0, 2^32-2]
		 */
		static inline unsigned int nextRandomNumber(unsigned int& seed);

		/**
		 * Returns the maximal random value of this generator.
		 * @return Maximal random value of this generator
		 */
		static constexpr unsigned int randMax();

	private:

		/**
		 * Returns a seed value based on the current time, the thread id, and a random value from RandomI.
		 * @return The combined seed value, with range [1, infinity)
		 */
		static unsigned int threadAndTimeBasedSeed();

	private:

		/// The seed value which was used to initialize this random generator.
		unsigned int initialSeed_ = (unsigned int)(-1);

		/// Internal seed parameter used for random number generation, changes whenever a new random number is generated.
		unsigned int seed_ = (unsigned int)(-1);

		/// Generator lock.
		Lock lock_;
};

inline RandomGenerator::RandomGenerator(RandomGenerator& generator)
{
	initialSeed_ = generator.lockedRand();

	if (initialSeed_ == 0u)
	{
		initialSeed_ = 1u; // Xorshift32 requires non-zero seed, ensure it's not zero
	}

	seed_ = initialSeed_;

	// doing first randomization step
	nextRandomNumber(seed_);
}

inline RandomGenerator::RandomGenerator(const unsigned int seed) :
	initialSeed_(seed != 0u ? seed : 1u) // Xorshift32 requires non-zero seed, ensure it's not zero
{

	seed_ = initialSeed_;
}

inline RandomGenerator::RandomGenerator(RandomGenerator&& randomGenerator) noexcept
{
	*this = std::move(randomGenerator);
}

inline unsigned int RandomGenerator::rand()
{
	return nextRandomNumber(seed_);
}

inline unsigned int RandomGenerator::lockedRand()
{
	const ScopedLock scopedLock(lock_);

	return nextRandomNumber(seed_);
}

inline unsigned int RandomGenerator::seed() const
{
	return seed_;
}

inline unsigned int RandomGenerator::initialSeed() const
{
	return initialSeed_;
}

inline unsigned int RandomGenerator::nextRandomNumber(unsigned int& seed)
{
	// Xorshift32 algorithm - generates full 32-bit random numbers
	// The seed must not be zero, otherwise the algorithm will produce infinite zeros
	ocean_assert(seed != 0u);

	seed ^= seed << 13u;
	seed ^= seed >> 17u;
	seed ^= seed << 5u;

	// Xorshift32 produces values in [1, 2^32-1], subtract 1 to get [0, 2^32-2]
	return seed - 1u;
}

constexpr unsigned int RandomGenerator::randMax()
{
	return 0xFFFFFFFEu; // 4294967294 (2^32 - 2), as xorshift32 never produces 0 or 2^32-1
}

}

#endif // META_OCEAN_BASE_RANDOM_GENERATOR_H
