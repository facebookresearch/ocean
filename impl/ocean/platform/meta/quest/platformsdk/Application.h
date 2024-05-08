/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_APPLICATION_H
#define META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_APPLICATION_H

#include "ocean/platform/meta/quest/platformsdk/PlatformSDK.h"
#include "ocean/platform/meta/quest/platformsdk/MessageHandler.h"

#include "ocean/base/Singleton.h"
#include "ocean/base/Timestamp.h"

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
 * This class implements application functionalities based on the PlatformSDK.
 * @ingroup platformmetaquestplatformsdk
 */
class OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT Application :
	public Singleton<Application>,
	protected MessageHandler
{
	friend class Singleton<Application>;
	friend class Manager;

	public:

		/**
		 * Ensures that the mulit-player functionalities are initialized.
		 */
		void ensureInitialization();

		/**
		 * Returns whether the app has been launched with a deep link.
		 * The manager needs to be initialized before calling this function.
		 * @param launchDeepLink The resulting deepLink which has been used to launch the app
		 * @param sessionId Optional resulting session id which has been defined when launching the app
		 * @return True, if so; False, if the if the app was launched normally
		 * @see isInitialized(), hasChangedLaunchDeepLink().
		 */
		bool hasLaunchDeepLink(std::string& launchDeepLink, std::string* sessionId = nullptr) const;

		/**
		 * Returns whether the app has received a new launch deep link while the app was already running.
		 * @param changedDeepLink The resulting deepLink which has been changed
		 * @param sessionId Optional resulting session id which has been changed
		 * @return True, if so
		 */
		bool hasChangedLaunchDeepLink(std::string& changedDeepLink, std::string* sessionId = nullptr);

		/**
		 * Launches another application in the user's library.
		 * @param appId The id of the application to launch, must be valid
		 * @param deepLink Optional deep link to use when launching the app
		 * @return The future containing the success state, invalid in case of a failure
		 */
		std::future<bool> launchAnotherApplication(const uint64_t appId, const std::string& deepLink = std::string());

	protected:

		/**
		 * Creates a new object.
		 */
		Application();

		/**
		 * Checks whether the application has a launch deep link.
		 */
		void checkLaunchDeepLink();

		/**
		 * The event function for ovrMessage_Notification_ApplicationLifecycle_LaunchIntentChanged notifications.
		 * @param message The notification message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onLaunchIntentChangedNotification(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for launchAnotherApplication().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onLaunchedAnotherApplication(ovrMessage* message, const bool succeeded);

	protected:

		/// The subscription objects for all messages.
		MessageScopedSubscriptions messageScopedSubscriptions_;

		/// The deep link which has been used to launch the app, empty if the app was launched normally.
		std::string launchDeepLink_;

		/// The session id which has been defined when launching the app, empty if no session is was provided.
		std::string launchSessionId_;

		/// The timestamp when the launch deep link was assigned the last time.
		Timestamp launchDeepLinkTimestamp_ = Timestamp(false);

		/// The deep link which has arrived while the app was already running, empty otherwise.
		std::string changedLaunchDeepLink_;

		/// The session id which has arrived while the app was already running, empty otherwise.
		std::string changedLaunchSessionId_;

		/// The result queue for launchAnotherApplication().
		RequestQueue<bool> launchAnotherApplicationRequestQueue_;

		/// The application class' lock.
		mutable Lock lock_;
};

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_APPLICATION_H
