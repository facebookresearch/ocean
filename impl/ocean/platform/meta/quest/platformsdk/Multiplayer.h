/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MULTIPLAYER_H
#define META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MULTIPLAYER_H

#include "ocean/platform/meta/quest/platformsdk/PlatformSDK.h"
#include "ocean/platform/meta/quest/platformsdk/MessageHandler.h"

#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Singleton.h"

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
 * This class implements multi-player functionalities based on GroupPresence.
 * @ingroup platformmetaquestplatformsdk
 */
class OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT Multiplayer :
	public Singleton<Multiplayer>,
	protected MessageHandler
{
	friend class Singleton<Multiplayer>;

	public:

		/**
		 * Definition of a subscription object for an active GroupPresence configuration.
		 */
		using ScopedGroupPresence = ScopedSubscriptionT<bool, Multiplayer>;

		/**
		 * This class holds the relevant information of a received join intent.
		 */
		class JoinIntent
		{
			public:

				/**
				 * Default constructor.
				 */
				JoinIntent() = default;

				/**
				 * Creates a new join intent object.
				 * @param deepLink The deep link of the join intent, can be empty
				 * @param destinationApiName The destination API name of the join intent, can be empty
				 * @param lobbySessionId The id of the lobby session of the join intent, can be empty
				 * @param matchSessionId The id of the match session of the join intent, can be empty
				 */
				inline JoinIntent(std::string&& deepLink, std::string&& destinationApiName, std::string&& lobbySessionId, std::string&& matchSessionId);

			public:

				/// The deep link of the join intent, can be empty.
				std::string deepLink_;

				/// The destination API name of the join intent, can be empty.
				std::string destinationApiName_;

				/// The id of the lobby session of the join intent, can be empty.
				std::string lobbySessionId_;

				/// The id of the match session of the join intent, can be empty.
				std::string matchSessionId_;
		};

	public:

		/**
		 * Ensures that the mulit-player functionalities are initialized.
		 */
		void ensureInitialization();

		/**
		 * Sets the current GroupPresence and activates it (sets it as joinable).
		 * The group presence will be set and will be active until the scoped group presence object of the resulting future exists.<br>
		 * Only one active group presence can be set at the same time.
		 * @param destinationApiName The name of the destination API, must be valid
		 * @param lobbySessionId The lobby session id to set, can be empty
		 * @param matchSessionId The lobby session id to set, can be empty
		 */
		[[nodiscard]] std::future<ScopedGroupPresence> setActiveGroupPresence(const std::string& destinationApiName, const std::string& lobbySessionId, const std::string& matchSessionId = std::string());

		/**
		 * Requests a list of invitable users.
		 * @return A future object which will provide the ids of all invitable users, if successful
		 */
		std::future<Indices64> invitableUsers();

		/**
		 * Returns whether a latest result for the invitableUsers() function is available.
		 * @param result The result if available
		 * @return True, if so
		 */
		inline bool hasLatestInvitableUsersResult(Indices64& result);

		/**
		 * Returns whether a joint intent has been received.
		 * @param joinIntent The resulting join intent object holding the relevant information
		 * @return True, if so
		 */
		inline bool hasLatestJointIntent(JoinIntent& joinIntent);

		/**
		 * Invites several users.
		 * @param userIds The ids of the user to invite
		 * @return True, if succeeded
		 */
		bool inviteUsers(const Indices64& userIds);

		/**
		 * Launches the panel to invite users.
		 * @return A future object which will provide whether at least one invite has been sent, if successful
		 */
		std::future<bool> launchInvitePanel();

		/**
		 * Launches the roster panel showing all users with same group presence configuration.
		 * @return True, if succeeded
		 */
		bool launchRosterPanel();

	protected:

		/**
		 * Creates a new object.
		 */
		Multiplayer();

		/**
		 * The response function for setGroupPresence().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onSetGroupPresence(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for ovr_GroupPresence_Clear().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onClearGroupPresence(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for invitableUsers().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onInvitableUsers(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for inviteUsers().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onInviteUsers(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for launchInvitePanel().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onLaunchInvitePanel(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for launchRosterPanel().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onLaunchRosterPanel(ovrMessage* message, const bool succeeded);

		/**
		 * The event function for notifications.
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onNotification(ovrMessage* message, const bool succeeded);

		/**
		 * The event function for join event received notifications.
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onNotificationJoinIntentReceived(ovrMessage* message, const bool succeeded);

		/**
		 * The event function called when the group presence subscription needs to be released.
		 * @param unusedValue An unused value
		 */
		void onReleaseGroupPresenceSubscription(const bool& unusedValue);

	protected:

		/// The subscription objects for all messages.
		MessageScopedSubscriptions messageScopedSubscriptions_;

		/// The result queue for invitableUsers().
		RequestQueue<Indices64> invitableUsersRequestQueue_;

		/// The promise for an active group presence configuration.
		std::promise<ScopedGroupPresence> setGroupPresenceRequestPromise_;

		/// The promise for launch invite panel responses.
		std::optional<std::promise<bool>> launchInvitePanelRequestPromise_;

		/// True, if currently a group presence configuration is active.
		bool groupPresenceIsActive_ = false;

		/// The lock for group presence configurations.
		Lock groupPresenceLock_;

		/// The result object for join intent notifications.
		RequestObject<JoinIntent> joinIntentObject_;
};

inline Multiplayer::JoinIntent::JoinIntent(std::string&& deepLink, std::string&& destinationApiName, std::string&& lobbySessionId, std::string&& matchSessionId) :
	deepLink_(std::move(deepLink)),
	destinationApiName_(std::move(destinationApiName)),
	lobbySessionId_(std::move(lobbySessionId)),
	matchSessionId_(std::move(matchSessionId))
{
	// nothing to do here
}

inline bool Multiplayer::hasLatestInvitableUsersResult(Indices64& result)
{
	return invitableUsersRequestQueue_.latestResponse(result);
}

inline bool Multiplayer::hasLatestJointIntent(JoinIntent& joinIntent)
{
	return joinIntentObject_.latestResponse(joinIntent);
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_MULTIPLAYER_H
