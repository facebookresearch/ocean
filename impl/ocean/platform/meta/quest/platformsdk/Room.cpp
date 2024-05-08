/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/platformsdk/Room.h"
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

Room::Room()
{
	messageScopedSubscriptions_.emplace_back(subscribeForMessageResponse(ovrMessage_Notification_Room_InviteAccepted, &Room::onNotification));
	messageScopedSubscriptions_.emplace_back(subscribeForMessageResponse(ovrMessage_Notification_Room_InviteReceived, &Room::onNotificationInvite));
	messageScopedSubscriptions_.emplace_back(subscribeForMessageResponse(ovrMessage_Notification_Room_RoomUpdate, &Room::onNotificationUpdated));
}

void Room::ensureInitialization()
{
	// nothing to do here, just ensuring that the instance exists
}

std::future<uint64_t> Room::createAndJoin(const uint64_t roomId)
{
	const ScopedOptions<ovrRoomOptionsHandle> options(ovr_RoomOptions_Create(), ovr_RoomOptions_Destroy);

	if (roomId != 0ull)
	{
		ovr_RoomOptions_SetRoomId(options, roomId);
	}

	ovr_RoomOptions_SetTurnOffUpdates(options, false);

	if (!invokeRequest(ovr_Room_CreateAndJoinPrivate2(ovrRoom_JoinPolicyFriendsOfMembers, 10u, options), &Room::onCreateAndJoin))
	{
		return std::future<uint64_t>();
	}

	return createAndJoinRequestQueue_.addRequest();
}

std::future<uint64_t> Room::getCurrent()
{
	if (!invokeRequest(ovr_Room_GetCurrent(), &Room::onGetCurrent))
	{
		return std::future<uint64_t>();
	}

	return getCurrentRequestQueue_.addRequest();
}

std::future<Room::Users> Room::invitableUsers(const uint64_t roomId)
{
	const ScopedOptions<ovrRoomOptionsHandle> options(ovr_RoomOptions_Create(), ovr_RoomOptions_Destroy);

	if (roomId != 0ull)
	{
		ovr_RoomOptions_SetRoomId(options, roomId);
	}

	if (!invokeRequest(ovr_Room_GetInvitableUsers2(options), &Room::onInvitableUsers))
	{
		return std::future<Users>();
	}

	return invitableUsersRequestQueue_.addRequest();
}

void Room::launchInvitableUserFlow(const uint64_t roomdId)
{
	ocean_assert(roomdId != 0ull);

	invokeRequest(ovr_Room_LaunchInvitableUserFlow(roomdId), &Room::onLaunchInvitableUserFlow);
}

std::future<bool> Room::joinRoom(const uint64_t roomId)
{
	ocean_assert(roomId != 0ull);
	if (roomId == 0ull)
	{
		return std::future<bool>();
	}

	const ScopedOptions<ovrRoomOptionsHandle> options(ovr_RoomOptions_Create(), ovr_RoomOptions_Destroy);

	ovr_RoomOptions_SetTurnOffUpdates(options, false);

	if (!invokeRequest(ovr_Room_Join2(roomId, options), &Room::onJoinRoom))
	{
		return std::future<bool>();
	}

	return joinRoomRequestQueue_.addRequest();
}

Room::Users Room::extractUsers(ovrUserArrayHandle userArrayHandle)
{
	ocean_assert(userArrayHandle != nullptr);

	if (userArrayHandle == nullptr)
	{
		return Users();
	}

	Users users;

	const size_t arraySize = ovr_UserArray_GetSize(userArrayHandle);
	users.reserve(arraySize);

	for (size_t n = 0; n < arraySize; ++n)
	{
		const ovrUserHandle userHandle = ovr_UserArray_GetElement(userArrayHandle, n);

		const uint64_t userId = ovr_User_GetID(userHandle);

		std::string oculusId;
		if (ovr_User_GetOculusID(userHandle) != nullptr)
		{
			oculusId = ovr_User_GetOculusID(userHandle);
		}

		std::string inviteToken;
		if (ovr_User_GetInviteToken(userHandle) != nullptr)
		{
			inviteToken = ovr_User_GetInviteToken(userHandle);
		}

		if (userId != 0ull)
		{
			users.emplace_back(userId, std::move(oculusId), std::move(inviteToken));
		}
	}

	return users;
}

void Room::onCreateAndJoin(ovrMessage* message, const bool succeeded)
{
	uint64_t roomId = 0ull;

	if (succeeded)
	{
		Log::debug() << "Succeeded to create and to join room";

		const ovrRoomHandle roomHandle = ovr_Message_GetRoom(message);

		roomId = ovr_Room_GetID(roomHandle),
		Log::debug() << "Room id: " << roomId;
	}
	else
	{
		Log::error() << "Failed to create and to join room: " << Utilities::errorMessage(message);
	}

	createAndJoinRequestQueue_.setResponse(roomId);
}

void Room::onGetCurrent(ovrMessage* message, const bool succeeded)
{
	uint64_t roomId = 0ull;

	if (succeeded)
	{
		Log::debug() << "Succeeded to get current room";

		const ovrRoomHandle roomHandle = ovr_Message_GetRoom(message);

		roomId = ovr_Room_GetID(roomHandle),
		Log::debug() << "Room id: " << roomId;

		const uint64_t appId = ovr_Room_GetApplicationID(roomHandle);
		Log::debug() << "App id: " << appId;
	}
	else
	{
		Log::error() << "Failed to get current room: " << Utilities::errorMessage(message);
	}

	getCurrentRequestQueue_.setResponse(roomId);
}

void Room::onInvitableUsers(ovrMessage* message, const bool succeeded)
{
	Users users;

	if (succeeded)
	{
		Log::debug() << "Succeeded to get invitable users to room";

		users = extractUsers(ovr_Message_GetUserArray(message));
	}
	else
	{
		Log::error() << "Failed to get invitable users to room: " << Utilities::errorMessage(message);
	}

	invitableUsersRequestQueue_.setResponse(std::move(users));
}

void Room::onLaunchInvitableUserFlow(ovrMessage* message, const bool succeeded)
{
	if (succeeded)
	{
		Log::debug() << "Succeeded to launch invitable user flow";
	}
	else
	{
		Log::error() << "Failed to launch invitable user flow: " << Utilities::errorMessage(message);
	}
}

void Room::onJoinRoom(ovrMessage* message, const bool succeeded)
{
	if (succeeded)
	{
		Log::debug() << "Succeeded to join room";
	}
	else
	{
		Log::error() << "Failed to join room: " << Utilities::errorMessage(message);
	}

	joinRoomRequestQueue_.setResponse(succeeded);
}

void Room::onNotification(ovrMessage* message, const bool succeeded)
{
	const ovrMessageType messageType = ovr_Message_GetType(message);

	switch (messageType)
	{
		case ovrMessage_Room_Join2:
			Log::debug() << "Room join";
			break;

		case ovrMessage_Room_Leave:
			Log::debug() << "Room leave";
			break;

		case ovrMessage_Room_InviteUser:
			Log::debug() << "Invited user";
			break;

		case ovrMessage_Notification_Room_InviteAccepted:
			Log::debug() << "Invited accepted";
			break;

		default:
			// not intended
			break;
	}
}

void Room::onNotificationInvite(ovrMessage* message, const bool succeeded)
{
	ocean_assert(ovr_Message_GetType(message) == ovrMessage_Notification_Room_InviteReceived);

	if (succeeded)
	{
		Log::debug() << "Received room invite:";

		const ovrRoomInviteNotificationHandle notificationHandle = ovr_Message_GetRoomInviteNotification(message);

		const uint64_t senderUserId = ovr_RoomInviteNotification_GetSenderID(notificationHandle);
		const uint64_t roomId = ovr_RoomInviteNotification_GetRoomID(notificationHandle);

		Log::debug() << "Sender user id: " << senderUserId;
		Log::debug() << "Room id: " << roomId;

		if (senderUserId != 0ull && roomId != 0ull)
		{
			inviteObject_.setResponse(Invite(senderUserId, roomId));
		}
	}
}

void Room::onNotificationUpdated(ovrMessage* message, const bool succeeded)
{
	ocean_assert(ovr_Message_GetType(message) == ovrMessage_Notification_Room_RoomUpdate);

	if (succeeded)
	{
		Log::debug() << "Recevied room update";

		const ovrRoomHandle roomHandle = ovr_Message_GetRoom(message);

		const ovrUserArrayHandle userArrayHandle = ovr_Room_GetUsers(roomHandle);

		Users users = extractUsers(userArrayHandle);

		Log::debug() << "Now " << users.size() << " are in the room";

		const ScopedLock scopedLock(lock_);

		for (const User& user : users)
		{
			usersInRoomSet_.emplace(user.userId());
		}
		ocean_assert(usersInRoomSet_.size() == users.size());

		usersObject_.setResponse(std::move(users));
	}
}

} // namespace PlatformSDK

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
