/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/platformsdk/Multiplayer.h"
#include "ocean/platform/meta/quest/platformsdk/Manager.h"
#include "ocean/platform/meta/quest/platformsdk/Utilities.h"

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

Multiplayer::Multiplayer()
{
	messageScopedSubscriptions_.emplace_back(subscribeForMessageResponse(ovrMessage_Notification_GroupPresence_InvitationsSent, &Multiplayer::onNotification));
	messageScopedSubscriptions_.emplace_back(subscribeForMessageResponse(ovrMessage_Notification_GroupPresence_LeaveIntentReceived, &Multiplayer::onNotification));

	messageScopedSubscriptions_.emplace_back(subscribeForMessageResponse(ovrMessage_Notification_GroupPresence_JoinIntentReceived, &Multiplayer::onNotificationJoinIntentReceived));
}

void Multiplayer::ensureInitialization()
{
	// nothing to do here, just ensuring that the instance exists
}

std::future<Multiplayer::ScopedGroupPresence> Multiplayer::setActiveGroupPresence(const std::string& destinationApiName, const std::string& lobbySessionId, const std::string& matchSessionId)
{
	const ScopedLock scopedLock(groupPresenceLock_);

	if (groupPresenceIsActive_)
	{
		Log::error() << "Group presence is already active";
		ocean_assert(false && "Group presence is already active");

		return std::future<ScopedGroupPresence>();
	}

	if (destinationApiName.empty())
	{
		ocean_assert(false && "Invalid input!");
		return std::future<ScopedGroupPresence>();
	}

	const ScopedOptions<ovrGroupPresenceOptionsHandle> options(ovr_GroupPresenceOptions_Create(), ovr_GroupPresenceOptions_Destroy);

	ovr_GroupPresenceOptions_SetIsJoinable(options, true);
	ovr_GroupPresenceOptions_SetDestinationApiName(options, destinationApiName.c_str());

	if (!lobbySessionId.empty())
	{
		ovr_GroupPresenceOptions_SetLobbySessionId(options, lobbySessionId.c_str());
	}

	if (!matchSessionId.empty())
	{
		ovr_GroupPresenceOptions_SetMatchSessionId(options, matchSessionId.c_str());
	}

	if (!invokeRequest(ovr_GroupPresence_Set(options), &Multiplayer::onSetGroupPresence))
	{
		return std::future<ScopedGroupPresence>();
	}

	groupPresenceIsActive_ = true;

	setGroupPresenceRequestPromise_ = std::promise<ScopedGroupPresence>();
	return setGroupPresenceRequestPromise_.get_future();
}

std::future<Indices64> Multiplayer::invitableUsers()
{
	const ScopedOptions<ovrInviteOptionsHandle> options(ovr_InviteOptions_Create(), ovr_InviteOptions_Destroy);

	if (!invokeRequest(ovr_GroupPresence_GetInvitableUsers(options), &Multiplayer::onInvitableUsers))
	{
		return std::future<Indices64>();
	}

	return invitableUsersRequestQueue_.addRequest();
}

bool Multiplayer::inviteUsers(const Indices64& userIds)
{
	ocean_assert(!userIds.empty());
	if (userIds.empty())
	{
		return false;
	}

	ovrID* ids = const_cast<ovrID*>(userIds.data());

	return invokeRequest(ovr_GroupPresence_SendInvites(ids, (unsigned int)(userIds.size())), &Multiplayer::onInviteUsers);
}

std::future<bool> Multiplayer::launchInvitePanel()
{
	const ScopedLock scopedLock(groupPresenceLock_);

	if (launchInvitePanelRequestPromise_)
	{
		return std::future<bool>();
	}

	const ScopedOptions<ovrInviteOptionsHandle> options(ovr_InviteOptions_Create(), ovr_InviteOptions_Destroy);

	if (!invokeRequest(ovr_GroupPresence_LaunchInvitePanel(options), &Multiplayer::onLaunchInvitePanel))
	{
		return std::future<bool>();
	}

	launchInvitePanelRequestPromise_ = std::promise<bool>();

	return launchInvitePanelRequestPromise_.value().get_future();
}

bool Multiplayer::launchRosterPanel()
{
	const ScopedOptions<ovrRosterOptionsHandle> options(ovr_RosterOptions_Create(), ovr_RosterOptions_Destroy);

	return invokeRequest(ovr_GroupPresence_LaunchRosterPanel(options), &Multiplayer::onLaunchRosterPanel);
}

void Multiplayer::onSetGroupPresence(ovrMessage* message, const bool succeeded)
{
	const ScopedLock scopedLock(groupPresenceLock_);

	ocean_assert(groupPresenceIsActive_);

	ScopedGroupPresence scopedGroupPresence;

	if (succeeded)
	{
		Log::debug() << "Succeeded to set group presence";

		scopedGroupPresence = ScopedGroupPresence(true, std::bind(&Multiplayer::onReleaseGroupPresenceSubscription, this, std::placeholders::_1));
	}
	else
	{
		Log::error() << "Failed to set group presence: " << Utilities::errorMessage(message);

		groupPresenceIsActive_ = false;
	}

	setGroupPresenceRequestPromise_.set_value(std::move(scopedGroupPresence));
}

void Multiplayer::onClearGroupPresence(ovrMessage* message, const bool succeeded)
{
	const ScopedLock scopedLock(groupPresenceLock_);

	if (succeeded)
	{
		Log::debug() << "Succeeded to clear group presence";
	}
	else
	{
		Log::error() << "Failed to clear group presence: " << Utilities::errorMessage(message);
	}
}

void Multiplayer::onInvitableUsers(ovrMessage* message, const bool succeeded)
{
	Indices64 users;

	if (succeeded)
	{
		Log::debug() << "Succeeded to get invitable users";

		const ovrUserArrayHandle userArrayHandle = ovr_Message_GetUserArray(message);

		const size_t arraySize = ovr_UserArray_GetSize(userArrayHandle);

		Log::debug() << "Number of invitable users: " << arraySize;

		for (size_t n = 0; n < ovr_UserArray_GetSize(userArrayHandle); ++n)
		{
			const ovrUserHandle userHandle = ovr_UserArray_GetElement(userArrayHandle, n);

			const ovrID userId = ovr_User_GetID(userHandle);
			Log::debug() << "Invitable user id: " << userId;

			const char* oculusId = ovr_User_GetOculusID(userHandle);
			if (oculusId != nullptr && oculusId[0] != '\0')
			{
				Log::debug() << "Oculus id: " << oculusId;
			}

			users.emplace_back(userId);
		}
	}
	else
	{
		Log::error() << "Failed to get invitable users: " << Utilities::errorMessage(message);
	}

	invitableUsersRequestQueue_.setResponse(std::move(users));
}

void Multiplayer::onInviteUsers(ovrMessage* message, const bool succeeded)
{
	if (succeeded)
	{
		Log::debug() << "Succeeded to invite users";
	}
	else
	{
		Log::error() << "Failed to invite users: " << Utilities::errorMessage(message);
	}
}

void Multiplayer::onLaunchInvitePanel(ovrMessage* message, const bool succeeded)
{
	if (succeeded)
	{
		Log::debug() << "Succeeded to launch invite panel";

		const ScopedLock scopedLock(groupPresenceLock_);

		ocean_assert(launchInvitePanelRequestPromise_);
		if (launchInvitePanelRequestPromise_)
		{
			const ovrInvitePanelResultInfoHandle panelResultHandle = ovr_Message_GetInvitePanelResultInfo(message);
			ocean_assert(panelResultHandle != nullptr);

			const bool invitesSent = ovr_InvitePanelResultInfo_GetInvitesSent(panelResultHandle);

			launchInvitePanelRequestPromise_.value().set_value(invitesSent);
			launchInvitePanelRequestPromise_.reset();
		}
	}
	else
	{
		Log::error() << "Failed to launch invite panel: " << Utilities::errorMessage(message);
	}
}

void Multiplayer::onLaunchRosterPanel(ovrMessage* message, const bool succeeded)
{
	if (succeeded)
	{
		Log::debug() << "Succeeded to launch roster panel";
	}
	else
	{
		Log::error() << "Failed to launch roster panel: " << Utilities::errorMessage(message);
	}
}

void Multiplayer::onNotification(ovrMessage* message, const bool succeeded)
{
	Log::info() << "Multiplayer::onNotification";

	const ovrMessageType messageType = ovr_Message_GetType(message);

	switch (messageType)
	{
		case ovrMessage_Notification_GroupPresence_InvitationsSent:
		{
			const ovrLaunchInvitePanelFlowResultHandle panelFlowResultHandle = ovr_Message_GetLaunchInvitePanelFlowResult(message);
			const ovrUserArrayHandle userArrayHandle = ovr_LaunchInvitePanelFlowResult_GetInvitedUsers(panelFlowResultHandle);

			std::vector<std::string> userNames;
			const Indices64 userIds = Utilities::extractUsers(userArrayHandle, &userNames);

			Log::info() << "Invitations sent:";

			for (size_t n = 0; n < userIds.size(); ++n)
			{
				Log::info() << userIds[n] << ": " << userNames[n];
			}

			break;
		}

		case ovrMessage_Notification_GroupPresence_LeaveIntentReceived:
		{
			const ovrGroupPresenceLeaveIntentHandle handle = ovr_Message_GetGroupPresenceLeaveIntent(message);

				Log::info() << "ovrMessage_Notification_GroupPresence_LeaveIntentReceived";

				const char* destinationApiName = ovr_GroupPresenceLeaveIntent_GetDestinationApiName(handle);
				Log::info() << "Destination API name: " << (destinationApiName != nullptr ? destinationApiName : "NO DATA");

				const char* lobbySessionId = ovr_GroupPresenceLeaveIntent_GetLobbySessionId(handle);
				Log::info() << "Lobby session id: " << (lobbySessionId  != nullptr ? lobbySessionId : "NO DATA");

			break;
		}

		default:
			// not intended
			break;
	}
}

void Multiplayer::onNotificationJoinIntentReceived(ovrMessage* message, const bool succeeded)
{
	ocean_assert(ovr_Message_GetType(message) == ovrMessage_Notification_GroupPresence_JoinIntentReceived);

	if (succeeded)
	{
		const ovrGroupPresenceJoinIntentHandle handle = ovr_Message_GetGroupPresenceJoinIntent(message);

		Log::debug() << "Received join intent for group presence:";

		std::string deepLink;
		if (ovr_GroupPresenceJoinIntent_GetDeeplinkMessage(handle) != nullptr)
		{
			deepLink = ovr_GroupPresenceJoinIntent_GetDeeplinkMessage(handle);
		}

		std::string destinationApiName;
		if (ovr_GroupPresenceJoinIntent_GetDestinationApiName(handle) != nullptr)
		{
			destinationApiName = ovr_GroupPresenceJoinIntent_GetDestinationApiName(handle);
		}

		std::string lobbySessionId;
		if (ovr_GroupPresenceJoinIntent_GetLobbySessionId(handle) != nullptr)
		{
			lobbySessionId = ovr_GroupPresenceJoinIntent_GetLobbySessionId(handle);
		}

		std::string matchSessionId;
		if (ovr_GroupPresenceJoinIntent_GetMatchSessionId(handle) != nullptr)
		{
			matchSessionId = ovr_GroupPresenceJoinIntent_GetMatchSessionId(handle);
		}

		Log::debug() << "Deep link: " << deepLink;
		Log::debug() << "Destination API name: " << destinationApiName;
		Log::debug() << "Lobby session id: " << lobbySessionId;
		Log::debug() << "Match session id: " << matchSessionId;

		joinIntentObject_.setResponse(JoinIntent(std::move(deepLink), std::move(destinationApiName), std::move(lobbySessionId), std::move(matchSessionId)));
	}
}

void Multiplayer::onReleaseGroupPresenceSubscription(const bool& /*unusedValue*/)
{
	const ScopedLock scopedLock(groupPresenceLock_);

	ocean_assert(groupPresenceIsActive_);

	if (!invokeRequest(ovr_GroupPresence_Clear(), &Multiplayer::onClearGroupPresence))
	{
		Log::debug() << "Failed to clear group presence";
	}

	groupPresenceIsActive_ = false;
}

} // namespace PlatformSDK

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
