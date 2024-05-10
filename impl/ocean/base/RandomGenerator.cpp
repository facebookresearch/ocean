/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

namespace Ocean
{

RandomGenerator::RandomGenerator() :
	initialSeed_(timeBasedCombinedSeed())
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
		initialSeed_ = timeBasedCombinedSeed();
	}

	seed_ = initialSeed_;
}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& randomGenerator)
{
	if (this != &randomGenerator)
	{
		initialSeed_ = randomGenerator.initialSeed_;
		seed_ = randomGenerator.seed_;

		randomGenerator.initialSeed_ = timeBasedCombinedSeed();
		randomGenerator.seed_ = randomGenerator.initialSeed_;
	}

	return *this;
}

unsigned int RandomGenerator::timeBasedCombinedSeed()
{
	unsigned int seed = RandomI::random32();
	seed ^= RandomI::timeBasedSeed() + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);

	return seed;
}

}
