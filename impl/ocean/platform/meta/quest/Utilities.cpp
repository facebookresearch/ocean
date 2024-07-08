/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/Utilities.h"

#include "ocean/platform/android/Processor.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

bool Utilities::restrictThreadExecutionToSilverCoresQuest()
{
	// by default, thread affinity should set to all cores for which we can set the affinity

	uint32_t currentMask = 0u;
	if (!Platform::Android::Processor::currentThreadAffinity(currentMask))
	{
		return false;
	}

	if ((currentMask & 0b1111u) == 0u)
	{
		// the current thread affinity is not set to any Silver core

		// possibility 1: we have set the affinity to Gold cores before
		// possibility 2: we are not allowed to set the affinity to a Silver core

		// therefore, we simply iterate over all possible combinations of Silver cores (0-3), so mask values between 1 and 15

		for (uint32_t mask = 8u; mask != 0u; --mask)
		{
			if (Platform::Android::Processor::setCurrentThreadAffinity(mask))
			{
				return true;
			}
		}

		return false;
	}

	// we can set the affinity to at least one Silver core - so we will not try all combinations, but will set affinity to those Silver cores only

	const uint32_t silverMask = currentMask & 0b1111u;
	ocean_assert(silverMask != 0u);

	return Platform::Android::Processor::setCurrentThreadAffinity(silverMask);
}

bool Utilities::restrictThreadExecutionToGoldCoresQuest()
{
	// first let's try to set the affinity to all Gold cores, if this does not work we will try all possible combinations again
	if (Platform::Android::Processor::setCurrentThreadAffinity(0b11110000u))
	{
		return true;
	}

	// it's quite common to be restricted to three Gold cores - so let's try every possible combination (mask values between 16 and 240)

	for (uint32_t mask = 240u; mask != 0u; mask -= 16u)
	{
		ocean_assert((mask & 0b11110000u) != 0u);
		ocean_assert((mask & 0b00001111u) == 0u);

		if (Platform::Android::Processor::setCurrentThreadAffinity(mask))
		{
			return true;
		}
	}

	return false;
}

}

}

}

}
