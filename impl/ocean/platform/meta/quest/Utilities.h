/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_UTILITIES_H
#define META_OCEAN_PLATFORM_META_QUEST_UTILITIES_H

#include "ocean/platform/meta/quest/Quest.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

/**
 * This class implements utility functions for Meta Quest platforms.
 * @ingroup platformmetaquest
 */
class Utilities
{
	public:

		/**
		 * Restricts the execution of the current thread to Silver Cores only - on Quest.
		 * This functionality needs a special OS which allows to set the thread affinity to Silver cores, by default this is not possible.<br>
		 * Beware: Ensure that this function returns True, otherwise the current thread is not restricted to Silver cores.<br>
		 * Currently, this function is configured for Quest (with Silver cores 0-3, and Gold cores 4-7).<br>
		 * For a more customized behavior, you can set a custom CPU affinity via Platform::Android::Processor::setCurrentThreadAffinity().
		 * @return True, if succeeded
		 * @see restrictThreadExecutionToGoldCoresQuest().
		 */
		static bool restrictThreadExecutionToSilverCoresQuest();

		/**
		 * Restricts the execution of the current thread to Gold Cores only - on Quest.
		 * Beware: Ensure that this function returns True, otherwise the current thread is not restricted to Gold cores.<br>
		 * Currently, this function is configured for Quest (with Silver cores 0-3, and Gold cores 4-7).<br>
		 * @return True, if succeeded
		 * @see restrictThreadExecutionToSilverCoresQuest().
		 */
		static bool restrictThreadExecutionToGoldCoresQuest();
};

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_UTILITIES_H
