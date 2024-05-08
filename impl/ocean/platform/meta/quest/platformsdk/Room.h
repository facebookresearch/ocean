/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_ROOM_H
#define META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_ROOM_H

#include "ocean/platform/meta/quest/platformsdk/PlatformSDK.h"
#include "ocean/platform/meta/quest/platformsdk/Manager.h"
#include "ocean/platform/meta/quest/platformsdk/MessageHandler.h"

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
 * Deprecated.
 * Starting in January 2023, the Oculus Rooms API will be deprecated and removed from the list of available Platform APIs for integration.
 * https://fburl.com/tyeuf18b
 *
 * User Multiplayer instead.
 *
 * This class implements functionalities for rooms.
 * @ingroup platformmetaquestplatformsdk
 * @see Multiplayer.
 */
class OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_EXPORT Room :
	public Singleton<Room>,
	protected MessageHandler
{
	friend class Singleton<Room>;

	public:

		/**
		 * This class holds relevent information of a user.
		 */
		class User
		{
			public:

				/**
				 * Creates a new user object.
				 * @param userId The id of the user, must be valid
				 * @param oculusId Optional Oculus id of the user, if known
				 * @param inviteToken Optionl invite token for the user, if known
				 */
				inline User(const uint64_t userId, std::string oculusId, std::string inviteToken);

				/**
				 * Returns the id of the user.
				 * @return The user's id
				 */
				inline uint64_t userId() const;

				/**
				 * Returns the Oculus id of the user.
				 * @param The user's Oculus id, empty if unknown
				 */
				inline const std::string& oculusId() const;

				/**
				 * Returns the invite token for the user.
				 * @return The user's (ontime) invite token, empty if unknown
				 */
				inline const std::string& inviteToken() const;

			protected:

				/// The user's id.
				uint64_t userId_ = 0ull;

				/// The Oculus id of the user.
				std::string oculusId_;

				/// The user's (ontime) invite token.
				std::string inviteToken_;
		};

		/**
		 * Definition of a vector holding users.
		 */
		typedef std::vector<User> Users;

		/**
		 * This class holds the relevant information of a room invite.
		 */
		class Invite
		{
			public:

				/**
				 * Default constructor for an invalid invite.
				 */
				Invite() = default;

				/**
				 * Creates a new invite object.
				 * @param userId The id of the user from which the invite as been sent, must be valid
				 * @param roomId The id of the room to which the invite belongs, must be valid
				 */
				inline Invite(const uint64_t userId, const uint64_t roomId);

				/**
				 * Returns the id of the user from which the invite as been sent.
				 * @return The sender's user id
				 */
				inline uint64_t userId() const;

				/**
				 * Returns the id of the room to which the invite belongs.
				 * @return The room's id
				 */
				inline uint64_t roomId() const;

			protected:

				/// The id of the user from which the invite as been sent.
				uint64_t userId_ = 0ull;

				/// The id of the room to which the invite belongs.
				uint64_t roomId_ = 0ull;
		};

	public:

		/**
		 * Ensures that the room functionalities are initialized.
		 */
		void ensureInitialization();

		/**
		 * Creates and joins a new room.
		 * @param roomId The optionl id of the room to create, 0 to create a room with random id
		 * @return The future holding the id of the created room, invalid in case of a failure
		 * @see hasLatestCreateAndJoinResult().
		 */
		std::future<uint64_t> createAndJoin(const uint64_t roomId = 0ull);

		/**
		 * Returns whether a latest result for the createAndJoin() function is available.
		 * @param result The result if available
		 * @return True, if so
		 */
		inline bool hasLatestCreateAndJoinResult(uint64_t& result);

		/**
		 * Returns the id of the current room.
		 * @return The future holding the id of the current room, invalid in case of a failure
		 * @see hasLatestGetCurrentResult().
		 */
		std::future<uint64_t> getCurrent();

		/**
		 * Returns whether a latest result for the getCurrent() function is available.
		 * @param result The result if available
		 * @return True, if so
		 */
		inline bool hasLatestGetCurrentResult(uint64_t& result);

		/**
		 * Determines the invitable users for the current room.
		 * @param roomId The id of the room for which the list of users will be determined
		 * @return The future holding the users, invalid in case of a failure
		 * @see hasLatestInvitableUsersResult().
		 */
		std::future<Users> invitableUsers(const uint64_t roomId);

		/**
		 * Returns whether a latest result for the invitableUsers() function is available.
		 * @param result The result if available
		 * @return True, if so
		 */
		inline bool hasLatestInvitableUsersResult(Users& result);

		/**
		 * Launches the invitable user flow for a room.
		 * @param roomId The id of the room for which the flow will be launched, must be valid
		 */
		void launchInvitableUserFlow(const uint64_t roomId);

		/**
		 * Returns whether a latest result for a room invite is available.
		 * This invite can be received any time from a remote user.
		 * @param result The result if available
		 * @return True, if so
		 */
		inline bool hasLatestInviteResult(Invite& result);

		/**
		 * Joins the (local) user to a room.
		 * @param roomId The id of the room to join, must be valid
		 * @return The future of the join result, invalid in case of a failure
		 * @see hasLatestJoinRoomResult().
		 */
		std::future<bool> joinRoom(const uint64_t roomId);

		/**
		 * Returns whether a latest result for the joinRoom() function is available.
		 * @param result The result if available
		 * @return True, if so
		 */
		inline bool hasLatestJoinRoomResult(bool& result);

		/**
		 * Returns whether a latest result for users in the current room is available.
		 * A new result will be available whenever a user is joining or leaving the current room.
		 * @param result The result if available
		 * @return True, if so
		 */
		inline bool hasLatestUsersResult(Users& result);

		/**
		 * Returns whether a user is currently in the current room.
		 * @param userId The id of the user to check
		 * @return True, if so
		 */
		inline bool isUserInRoom(const uint64_t userId) const;

		/**
		 * Extracts the relevant information from a user array.
		 * @param userArrayHandle The handle to the user array, must be valid
		 * @return The resulting extracted users
		 */
		static Users extractUsers(ovrUserArrayHandle userArrayHandle);

	protected:

		/**
		 * Creates a new object.
		 */
		Room();

		/**
		 * The response function for createAndJoinRoom().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onCreateAndJoin(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for getCurrent().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onGetCurrent(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for invitableUsers().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onInvitableUsers(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for launchInvitableUserFlow().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onLaunchInvitableUserFlow(ovrMessage* message, const bool succeeded);

		/**
		 * The response function for joinRoom().
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onJoinRoom(ovrMessage* message, const bool succeeded);

		/**
		 * The event function for notifications.
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onNotification(ovrMessage* message, const bool succeeded);

		/**
		 * The event function for invite notifications.
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onNotificationInvite(ovrMessage* message, const bool succeeded);

		/**
		 * The event function for room update notifications.
		 * @param message The response message, must be valid
		 * @param succeeded True, if the request succeeded
		 */
		void onNotificationUpdated(ovrMessage* message, const bool succeeded);

	protected:

		/// The subscription objects for all messages.
		MessageScopedSubscriptions messageScopedSubscriptions_;

		/// The result queue for createAndJoinRoom().
		RequestQueue<uint64_t> createAndJoinRequestQueue_;

		/// The result queue for getCurrent().
		RequestQueue<uint64_t> getCurrentRequestQueue_;

		/// The result queue for invitableUsers().
		RequestQueue<Users> invitableUsersRequestQueue_;

		/// The result object for invite notifications.
		RequestObject<Invite> inviteObject_;

		/// The result queue for joinRoom().
		RequestQueue<bool> joinRoomRequestQueue_;

		/// The result object for room update notification.
		RequestObject<Users> usersObject_;

		/// The set of user ids currently in the current room.
		UnorderedIndexSet64 usersInRoomSet_;

		/// The lock for the users.
		mutable Lock lock_;
};

inline Room::User::User(const uint64_t userId, std::string oculusId, std::string inviteToken) :
	userId_(userId),
	oculusId_(std::move(oculusId)),
	inviteToken_(std::move(inviteToken))
{
	ocean_assert(userId_ != 0ull);
}

inline uint64_t Room::User::userId() const
{
	return userId_;
}

inline const std::string& Room::User::oculusId() const
{
	return oculusId_;
}

inline const std::string& Room::User::inviteToken() const
{
	return inviteToken_;
}

inline Room::Invite::Invite(const uint64_t userId, const uint64_t roomId) :
	userId_(userId),
	roomId_(roomId)
{
	ocean_assert(userId_ != 0ull);
	ocean_assert(roomId_ != 0ull);
}

inline uint64_t Room::Invite::userId() const
{
	return userId_;
}

inline uint64_t Room::Invite::roomId() const
{
	return roomId_;
}

inline bool Room::hasLatestCreateAndJoinResult(uint64_t& result)
{
	return createAndJoinRequestQueue_.latestResponse(result);
}

inline bool Room::hasLatestGetCurrentResult(uint64_t& result)
{
	return getCurrentRequestQueue_.latestResponse(result);
}

inline bool Room::hasLatestInvitableUsersResult(Users& result)
{
	return invitableUsersRequestQueue_.latestResponse(result);
}

inline bool Room::hasLatestInviteResult(Invite& result)
{
	return inviteObject_.latestResponse(result);
}

inline bool Room::hasLatestJoinRoomResult(bool& result)
{
	return joinRoomRequestQueue_.latestResponse(result);
}

inline bool Room::hasLatestUsersResult(Users& result)
{
	return usersObject_.latestResponse(result);
}

inline bool Room::isUserInRoom(const uint64_t userId) const
{
	const ScopedLock scopedLock(lock_);

	return usersInRoomSet_.find(userId) != usersInRoomSet_.cend();
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_PLATFORMSDK_ROOM_H
