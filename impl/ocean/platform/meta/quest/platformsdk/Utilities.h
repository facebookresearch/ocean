/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_UTILITIES_H
#define META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_UTILITIES_H

#include "ocean/platform/meta/quest/platformsdk/PlatformSDK.h"

#include <OVR_Platform.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace PlatformSDK
{

/**
 * This class implements utility functions for PlatformSDK.
 * @ingroup platformmetaquestplatformsdk
 */
class OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT Utilities
{
	public:

		/**
		 * Extracts the relevant information from a user array.
		 * @param userArrayHandle The handle to the user array, must be valid
		 * @param userNames Optional resulting readable names of the users, one for each extracted user id
		 * @return The resulting ids of the extracted users
		 */
		static Indices64 extractUsers(ovrUserArrayHandle userArrayHandle, std::vector<std::string>* userNames = nullptr);

		/**
		 * Returns the error of a given message.
		 * @param message The message for which the error will be returned, must be valid
		 * @return The resulting error
		 */
		static std::string errorMessage(ovrMessage* message);
};

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_UTILITIES_H
