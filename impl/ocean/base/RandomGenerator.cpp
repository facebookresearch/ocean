// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

namespace Ocean
{

RandomGenerator::RandomGenerator() :
	seed_(RandomI::random32())
{
	// nothing to do here
}

RandomGenerator::RandomGenerator(RandomGenerator* optionalGenerator)
{
	if (optionalGenerator)
	{
		seed_ = ((unsigned int)(optionalGenerator->lockedRand()) & 0xFFFFu) | (((unsigned int)(optionalGenerator->lockedRand()) & 0xFFFFu) << 16);
	}
	else
	{
		seed_ = RandomI::random32();
	}
}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& randomGenerator)
{
	if (this != &randomGenerator)
	{
		seed_ = randomGenerator.seed_;
		randomGenerator.seed_ = RandomI::random32();
	}

	return *this;
}

}
