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
	initialSeed_(RandomI::random32())
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
		initialSeed_ = RandomI::random32();
	}

	seed_ = initialSeed_;
}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& randomGenerator)
{
	if (this != &randomGenerator)
	{
		initialSeed_ = randomGenerator.initialSeed_;
		seed_ = randomGenerator.seed_;

		randomGenerator.initialSeed_ = RandomI::random32();
		randomGenerator.seed_ = randomGenerator.initialSeed_;
	}

	return *this;
}

}
