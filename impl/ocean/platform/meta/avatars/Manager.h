// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_AVATARS_MANAGER_H
#define META_OCEAN_PLATFORM_META_AVATARS_MANAGER_H

#include "ocean/platform/meta/avatars/Avatars.h"
#include "ocean/platform/meta/avatars/Avatar.h"
#include "ocean/platform/meta/avatars/Images.h"
#include "ocean/platform/meta/avatars/Input.h"
#include "ocean/platform/meta/avatars/Primitives.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Memory.h"
#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Singleton.h"

#include "ocean/network/verts/Driver.h"
#include "ocean/network/verts/Entity.h"
#include "ocean/network/verts/Node.h"
#include "ocean/network/verts/Voip.h"

#include "ocean/rendering/Engine.h"

#include <ovrAvatar2/Asset.h>
#include <ovrAvatar2/System.h>
#include <ovrAvatar2/Tracking.h>

#include <queue>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

/**
 * This class implements the manager of for all avatars.
 * @ingroup platformmetaavatars
 */
class OCEAN_PLATFORM_META_AVATARS_EXPORT Manager : public Singleton<Manager>
{
	friend class Singleton<Manager>;

	public:

		/**
		 * Re-definition of UserType.
		 * @see Avatar::UserType.
		 */
		using UserType = Avatar::UserType;

		/**
		 * Definition of a callback function for a new rendering instance of an avatar.
		 * @param userId The id of the user
		 * @param avatarTransform The Transform node with the avatar as child, invalid if the rendering instance could not yet be created
		 */
		typedef std::function<void(const uint64_t userId, Rendering::TransformRef avatarTransform)> CreateAvatarCallback;

		/**
		 * Definition of a scoped subscription object for zones.
		 */
		typedef ScopedSubscription<std::string, Manager> ZoneScopedSubscription;

		/**
		 * Definition of a scoped subscription object for avatars.
		 */
		typedef ScopedSubscription<uint64_t, Manager> AvatarScopedSubscription;

		/**
		 * Definition of a vector holding subscription object for avatars.
		 */
		typedef std::vector<AvatarScopedSubscription> AvatarScopedSubscriptions;

		/**
		 * Definition of an unordered map mapping user ids to subscription objects for avatars.
		 */
		typedef std::unordered_map<uint64_t, AvatarScopedSubscription> AvatarScopedSubscriptionMap;

		/**
		 * Definition of a pair combining a user id with a user type.
		 */
		typedef std::pair<uint64_t, UserType> UserPair;

		/**
		 * Definition of a vector holding user pairs.
		 */
		typedef std::vector<UserPair> UserPairs;

		/**
		 * Definition of a callback function for changed (added and removed) avatars.
		 * @param zoneName The name of the zone to which the avatars belong
		 * @param addedAvatars The avatars which have been added since the last event
		 * @param removedAvatars The avatars which have been removed since the last event
		 */
		typedef std::function<void(const std::string& zoneName, const UserPairs& addedAvatars, const Indices64& removedAvatars)> ChangedAvatarsCallbackFunction;

		/**
		 * Definition of a subscription object for changed avatars.
		 */
		typedef ScopedSubscriptionHandler::ScopedSubscriptionType ChangedAvatarsScopedSubscription;

	protected:

		/**
		 * This class implements a scoped local file.
		 */
		class ScopedLocalFile
		{
			public:

				/**
				 * Returns the data pointer to the memory of the file.
				 * @return The file's memory
				 */
				const void* data();

				/**
				 * Returns the length of the file in bytes.
				 * @return The file's length, in bytes, with range [0, infinity)
				 */
				size_t size();

				/**
				 * Creates a new resource object.
				 * @param filename The name of the local file, must be valid
				 * @return The resource object, nullptr if the file does not exist
				 */
				static std::unique_ptr<ScopedLocalFile> create(const char* filename);

			protected:

				/**
				 * Creates a new resource object.
				 * @param filename The name of the local file, must exist
				 */
				explicit ScopedLocalFile(const std::string& filename);

				/**
				 * Copies the file's content to the memory.
				 * @return True, if succeeded
				 */
				bool readFile();

			protected:

				/// The filename of this object.
				std::string filename_;

				/// The memory holding the file's data.
				Memory memory_;
		};

		/**
		 * This calls holds the relevant information of a resource event.
		 */
		class ResourceEvent
		{
			friend class Manager;

			public:

				/**
				 * Creates a new event object.
				 * @param resourceId The id of the resource, must be valid
				 * @param status The status of the resource
				 */
				inline ResourceEvent(const ovrAvatar2ResourceId resourceId, const ovrAvatar2AssetStatus status);

			protected:

				/// The id of the resource to which this event belongs.
				ovrAvatar2ResourceId resourceId_ = ovrAvatar2ResourceId_Invalid;

				/// The status of the event.
				ovrAvatar2AssetStatus status_ = ovrAvatar2AssetStatus_LoadFailed;
		};

		/**
		 * Definition of a queue with resource events.
		 */
		typedef std::queue<ResourceEvent> ResourceEventQueue;

		/**
		 * Definition of a pair combining a usage counter with an avatar.
		 */
		typedef std::pair<std::shared_ptr<Avatar>, unsigned int> AvatarUsagePair;

		/**
		 * Definition of an unordered map mapping user ids to avatar pairs.
		 */
		typedef std::unordered_map<uint64_t, AvatarUsagePair> AvatarMap;

		/**
		 * Definition of a vector holding avatar create callbacks.
		 */
		typedef std::vector<std::pair<uint64_t, CreateAvatarCallback>> CreateAvatarCallbacks;

		/**
		 * Definition of an unordered map mapping user ids (of remote users) to remote body tracking data objects.
		 */
		typedef std::unordered_map<uint64_t, Input::SharedBodyTrackingData> RemoteBodyTrackingMap;

		/**
		 * Definition of an unordered map mapping user ids (of remote users) to timestamp.
		 */
		typedef std::unordered_map<uint64_t, Timestamp> PendingRemoteUserMap;

		/**
		 * Definition of a subscription handler for changed avatars events.
		 */
		typedef ScopedSubscriptionCallbackHandler<ChangedAvatarsCallbackFunction, Manager, true> ChangedAvatarsCallbackHandler;

		/**
		 * This class holds the relevant information for a zone.
		 */
		class Zone
		{
			public:

				/**
				 * Creates a new zone object.
				 * @param driver The driver object, must be valid
				 * @param newEntityScopedSubscription The subscription object for new entity events
				 * @param useVoip True, to use voip in this zone; False, to avoid using voip
				 */
				Zone(Network::Verts::SharedDriver driver, Network::Verts::Driver::NewEntityScopedSubscription newEntityScopedSubscription, const bool useVoip);

			public:

				/// The usage counter of this zone.
				unsigned int usageCounter_ = 0u;

				/// The driver of the zone.
				Network::Verts::SharedDriver driver_;

				/// The zone's node associated with the local user.
				Network::Verts::SharedNode localAvatarNode_;

				/// The individual nodes for remote users, one for each remote user and zone.
				Network::Verts::SharedNodes remoteAvatarNodes_;

				/// The set holding the ids of all remote users in this zone.
				UnorderedIndexSet64 remoteUserIds_;

				/// The subscription object for new entity events.
				Network::Verts::Driver::NewEntityScopedSubscription newEntityScopedSubscription_;

				/// The subscription object for voip.
				Network::Verts::Voip::VoipScopedSubscription voipScopedSubscription_;
		};

		/**
		 * Definition of a map mapping zone names to zone objects.
		 */
		typedef std::unordered_map<std::string, Zone> ZoneMap;

		/// The name of the VERTS node type holding the avatar data.
		static constexpr const char* vertsAvatarNodeType_ = "OCEAN_AVATAR_DATA";

	public:

		/**
		 * Initializes the manager.
		 * @param ovrPlatform The local platform, must be valid
		 * @param userType The type of the user, must be valid
		 * @param userId The id of the local user, must be valid
		 * @param accessToken The access token for the local user, can be received via PlatformSDK::Manger(), must be valid
		 * @param clientName The name of the client, must be valid
		 * @param clientVersion The version of the client, must be valid
		 * @param useVoip True to automatically use voice over ip together with each remote avatar; False, to avoid using voip
		 * @return True, if succeeded
		 */
		bool initialize(const ovrAvatar2Platform ovrPlatform, const UserType userType, const uint64_t userId, const std::string& accessToken, const std::string& clientName = "Ocean Avatars", const std::string& clientVersion = "0.1", const bool useVoip = true);

		/**
		 * Adds an additional access token for other user type not associated with the local user's user type.
		 * @param userType The type of the user for which an access token will be set, must be valid
		 * @param accessToken The access token for the user type, must be valid
		 * @return True, if succeeded
		 */
		bool setAccessToken(const UserType userType, const std::string& accessToken);

		/**
		 * Updates the body tracking data of the local user.
		 * This function should be called before update().
		 * @param bodyTrackingData The body tracking daata of the local avatar, invalid if unknown
		 * @see update().
		 */
		void updateLocalBodyTrackingData(Input::SharedBodyTrackingData&& bodyTrackingData);

		/**
		 * Updates the avatar manager and the entire avatar system, should be called for each new frame.
		 * This function should be called after updateLocalBodyTrackingData().
		 * @param engine The rendering engine to be used
		 * @param timestamp The current timestamp in host domain, must be valid
		 * @see updateLocalBodyTrackingData().
		 */
		void update(const Rendering::EngineRef& engine, const double timestamp);

		/**
		 * Sets the elevation of the ground plane.
		 * Avatars are placed in relation to the ground plane.
		 * @param yElevation The elevation at which the local ground plane is located within the y-direction, with range (-infinity, infinity)
		 */
		inline void setGroundPlaneElevation(const Scalar yElevation);

		/**
		 * Returns the id of the local user.
		 * @return The local user's id, 0 if not initialized yet
		 */
		inline uint64_t userId() const;

		/**
		 * Returns the type of the local user.
		 * @return The local user's type, Avatar::UT_UNKNOWN if not initialized yet
		 */
		inline UserType userType() const;

		/**
		 * Loads the avatar of a user.
		 * @param userId The id of the user to load, must be valid
		 * @param userType The type of the user, Avatar::UT_UNKNOWN to use the local user's user type
		 * @return The subscription object, the user will loaded as long as the object exists
		 * @see createAvatar().
		 */
		[[nodiscard]] AvatarScopedSubscription loadUser(const uint64_t userId, const UserType userType = Avatar::UT_UNKNOWN);

		/**
		 * Creates the rendering instance of an avatar.
		 * @param userId The id of the user for which the avatar rendering instance will be created, must be valid
		 * @param createAvatarCallback The callback function which will be called once the rendering object has been created, must be valid
		 * @param userType The type of the user, Avatar::UT_UNKNOWN to use the local user's user type
		 * @return Tue subscription object, the user will loaded as long as the object exists
		 * @see loadUser().
		 */
		[[nodiscard]] AvatarScopedSubscription createAvatar(const uint64_t userId, CreateAvatarCallback createAvatarCallback, const UserType userType = Avatar::UT_UNKNOWN);

		/**
		 * Joins a zone to get access to all remote avatars in this zone via network.
		 * @param zoneName The name of the zone to join, must be valid
		 * @return The subscription object, the zone will be left when the object is dispoed
		 */
		[[nodiscard]] ZoneScopedSubscription joinZone(const std::string& zoneName);

		/**
		 * Adds a new callback function for changed avatars events.
		 * @param changedAvatarsCallbackFunction The callback function to add, must be valid
		 * @return The subscription object, the callback function will exist as long as the subscription object exists
		 */
		[[nodiscard]] inline ChangedAvatarsScopedSubscription addChangedAvatarsCallback(ChangedAvatarsCallbackFunction changedAvatarsCallbackFunction);

		/**
		 * Sets or updates the transformation between the local avatar at a remote headset and the remote headset's world.
		 * This transformation can be used to e.g., allow the local avatar to freely move in a remote world.
		 * @param remoteHeadsetWorld_T_remoteAvatar The transformation between the local avatar at the remote headset and the remote headset's world, invalid to hide the local avatar at the remote side
		 * @return True, if a transformation could be set for the local avatar
		 */
		bool setRemoteHeadsetWorld_T_remoteAvatar(const HomogenousMatrix4& remoteHeadsetWorld_T_remoteAvatar);

		/**
		 * Returns the transformation between an avatar at a remote headset and the remote headset's world.
		 * In case the avatar is a remote avatar, the function returns the transformation between the avatar at the local headset and the local headset's world.
		 * This transformation can be used to e.g., allow the local avatar to freely move in a remote world.
		 * @param userId The id of the avatar, can be a local or remote avatar, must be valid
		 * @return The transformation between the local avatar at the remote headset and the remote headset's world, invalid if the avatar does not exist or is hidden at the remote side
		 */
		HomogenousMatrix4 remoteHeadsetWorld_T_remoteAvatar(const uint64_t userId) const;

		/**
		 * Returns the transformation between an avatar's head and the world.
		 * In case the avatar is a remote avatar, the avatar's remoteHeadsetWorld_T_remoteAvatar() in applied to the head transformation as well.
		 * @param userId The id of the avatar, can be a local or remote avatar, must be valid
		 * @return The avatar's head transformation, invalid if unknown
		 */
		HomogenousMatrix4 world_T_head(const uint64_t userId) const;

		/**
		 * Releases the manager and all avatar resoruces.
		 */
		void release();

	protected:

		/**
		 * Protected default constructor.
		 */
		Manager() = default;

		/**
		 * Destructs the manager and releases all resources.
		 */
		~Manager();

		/**
		 * Leaves a zone.
		 * @param zoneName The name of the zone to leave
		 * @see joinZone().
		 */
		void leaveZone(const std::string& zoneName);

		/**
		 * Unloads the avatar of a user.
		 * @param userId The id of the user to unload
		 * @see createAvatar().
		 */
		void unloadUser(const uint64_t& userId);

		/**
		 * The event function for resources.
		 * @param resource The resource of the event, must be valid
		 */
		void onResource(const ovrAvatar2Asset_Resource* resource);

		/**
		 * The event function for requests.
		 * @param requestId The id of the request, must be valid
		 * @param status The status of the request
		 */
		void onRequest(const ovrAvatar2RequestId requestId, const ovrAvatar2Result status);

		/**
		 * The event function for new verts entities.
		 * @param driver The driver sending the event
		 * @param entity The new entity
		 */
		void onNewVertsEntity(Network::Verts::Driver& driver, const Network::Verts::SharedEntity& entity);

		/**
		 * The static event function for logs.
		 * @param priority The priority of the log
		 * @param message The message to log, must be valid
		 * @param userContext The context of the event, must be valid
		 */
		static void onLogging(ovrAvatar2LogLevel priority, const char* message, void* userContext);

		/**
		 * The static event function for resources.
		 * @param resource The resource of the event, must be valid
		 * @param userContext The context of the event, must be valid
		 */
		static void onResource(const ovrAvatar2Asset_Resource* resource, void* userContext);

		/**
		 * The static event function for requests.
		 * @param requestId The id of the request, must be valid
		 * @param status The status of the request
		 * @param requestContext The context of the event, must be valid
		 */
		static void onRequest(ovrAvatar2RequestId requestId, ovrAvatar2Result status, void* requestContext);

		/**
		 * The static event function to open a file.
		 * @param fileReaderContext The context of the file reader, must be valid
		 * @param filename The name of the filename to open
		 * @return The handle to the file, nullptr if the file could not be opened
		 */
		static void* onFileOpen(void* fileReaderContext, const char* filename);

		/**
		 * The static event function to read an opened file.
		 * @param fileReaderContext The context of the file reader, must be valid
		 * @param fileHandle The handle to the file, must be valid
		 * @param fileData The resulting pointer to the file's content, must not be released by the caller
		 * @param fileSize The resulting size of the content of the file, in bytes
		 * @return True, if succeeded
		 */
		static bool onFileRead(void* fileReaderContext, void* fileHandle, const void** fileData, uint64_t* fileSize);

		/**
		 * The static event function to close an opened file.
		 * @param fileReaderContext The context of the file reader, must be valid
		 * @param fileHandle The handle to the file to close, must be valid
		 * @return True, if succeeded
		 */
		static bool onFileClose(void* fileReaderContext, void* fileHandle);

	protected:

		/// True, if the manager has been initialized successfully.
		bool initialized_ = false;

		/// The type of the local user.
		UserType userType_ = Avatar::UT_UNKNOWN;

		/// The elevation of the ground plane within the y-direction.
		Scalar yGroundPlaneElevation_ = Scalar(0);

		/// The id of the local/current user.
		uint64_t userId_ = 0ull;

		/// True, to render remote avatars; False, to hide all remote avatars.
		bool renderRemoteAvatars_ = false;

		/// The most recent body tracking data of the local user.
		Input::SharedBodyTrackingData recentLocalBodyTrackerData_;

		/// The timestamp when the manager has been updated the last time.
		Timestamp lastTimestamp_ = Timestamp(false);

		/// The images of all avatars.
		Images images_;

		/// The primitive of all avatars.
		Primitives primitives_;

		/// The shaders of all avatars.
		Shaders shaders_;

		/// The map mapping user ids to avatars.
		AvatarMap avatarMap_;

		/// The currently active callback functions for new instances of avatars.
		CreateAvatarCallbacks createAvatarCallbacks_;

		/// The handler for changed avatar event subscriptions.
		ChangedAvatarsCallbackHandler changedAvatarsCallbackHandler_;

		/// The manager's lock.
		mutable Lock managerLock_;

		/// The queue for resource events.
		ResourceEventQueue resourceEventQueue_;

		/// The lock for resource events.
		Lock resourceLock_;

		/// The map mapping user ids (of remote users) to their corresponding body tracking data objects.
		RemoteBodyTrackingMap remoteBodyTrackingMap_;

		/// A resusable memory buffer.
		std::vector<uint8_t> reusableNetworkBuffer_;

		/// The map mapping zone names to zone objects.
		ZoneMap zoneMap_;

		/// True to automatically use voice over ip together with each remote avatar; False, to avoid using voip.
		bool useVoip_ = false;
};

inline Manager::ResourceEvent::ResourceEvent(const ovrAvatar2ResourceId resourceId, const ovrAvatar2AssetStatus status) :
	resourceId_(resourceId),
	status_(status)
{
	ocean_assert(resourceId_ != ovrAvatar2ResourceId_Invalid);
}

inline void Manager::setGroundPlaneElevation(const Scalar yElevation)
{
	const ScopedLock scopedLock(managerLock_);

	yGroundPlaneElevation_ = yElevation;
}

inline uint64_t Manager::userId() const
{
	const ScopedLock scopedLock(managerLock_);

	return userId_;
}

inline Manager::UserType Manager::userType() const
{
	const ScopedLock scopedLock(managerLock_);

	return userType_;
}

inline Manager::ChangedAvatarsScopedSubscription Manager::addChangedAvatarsCallback(ChangedAvatarsCallbackFunction changedAvatarsCallbackFunction)
{
	return changedAvatarsCallbackHandler_.addCallback(std::move(changedAvatarsCallbackFunction));
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_AVATARS_MANAGER_H
