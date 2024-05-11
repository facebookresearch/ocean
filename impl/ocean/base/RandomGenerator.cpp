/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Thread.h"

namespace Ocean
{

RandomGenerator::RandomGenerator() :
	initialSeed_(threadAndTimeBasedSeed())
{
	seed_ = initialSeed_;
}

RandomGenerator::RandomGenerator(RandomGenerator* optionalGenerator)
{
	if (optionalGenerator != nullptr)
	{
		const unsigned int seedLow = optionalGenerator->lockedRand() & 0xFFFFu;
		const unsigned int seedHigh = (optionalGenerator->lockedRand() & 0xFFFFu) << 16u;

		initialSeed_ = seedLow | seedHigh;
	}
	else
	{
		initialSeed_ = threadAndTimeBasedSeed();
	}

	seed_ = initialSeed_;
}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& randomGenerator)
{
	if (this != &randomGenerator)
	{
		initialSeed_ = randomGenerator.initialSeed_;
		seed_ = randomGenerator.seed_;

		randomGenerator.initialSeed_ = threadAndTimeBasedSeed();
		randomGenerator.seed_ = randomGenerator.initialSeed_;
	}

	return *this;
}

unsigned int RandomGenerator::threadAndTimeBasedSeed()
{
	unsigned int seed = RandomI::random32();
	seed ^= RandomI::timeBasedSeed() + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);

	const uint64_t threadHash = Thread::currentThreadId().hash();
	const unsigned int threadHashLow = (unsigned int)(threadHash & 0xFFFFFFFFull);
	const unsigned int threadHashHigh = (unsigned int)(threadHash >> 32u);

	seed ^= threadHashLow + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);
	seed ^= threadHashHigh + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);

	return seed;
}

}
