// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_AVATARS_AVATAR_H
#define META_OCEAN_PLATFORM_META_AVATARS_AVATAR_H

#include "ocean/platform/meta/avatars/Avatars.h"
#include "ocean/platform/meta/avatars/Input.h"
#include "ocean/platform/meta/avatars/Primitives.h"
#include "ocean/platform/meta/avatars/Shaders.h"
#include "ocean/platform/meta/avatars/Utilities.h"

#include "ocean/media/Microphone.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Transform.h"

#include <ovrAvatar2/Asset.h>
#include <ovrAvatar2/AvatarSDKVersion.h>
#include <ovrAvatar2/Entity.h>
#include <ovrAvatar2/internal/Graphs.h>
#include <ovrAvatar2/tracking/Body.h>
#include <ovrAvatar2/tracking/LipSync.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

/**
 * This class holds the relevant information for one Avatar.
 * This class is not thread-safe as this class is never exposed.
 * @ingroup platformmetaavatars
 */
class OCEAN_PLATFORM_META_AVATARS_EXPORT Avatar
{
	friend class Manager;

	public:

		/**
		 * Definition of individual user types.
		 */
		enum UserType : uint32_t
		{
			/// Unknown user type.
			UT_UNKNOWN = 0u,
			/// User of a Meta account.
			UT_META,
			/// User of a Facebook account.
			UT_FACEBOOK,
			/// User of a Instagram account.
			UT_INSTAGRAM,
			/// User of a Oculus account.
			UT_OCULUS,

#if defined(AVATAR2_RELEASE_VERSION) && AVATAR2_RELEASE_VERSION >= 23
			/// User of a WhatsApp account.
			UT_WHATSAPP,
#endif

			/// End type.
			UT_END
		};

	protected:

		/**
		 * This class holds the relevant information of a node (an mesh of a primitive).
		 */
		class Node
		{
			friend class Avatar;

			public:

				/**
				 * Creates a new node object.
				 * @param primitiveId The primitive id of the node, must be valid
				 */
				explicit inline Node(const ovrAvatar2PrimitiveId primitiveId);

			protected:

				/// The node's primitive id.
				ovrAvatar2PrimitiveId primitiveId_ = ovrAvatar2PrimitiveId_Invalid;

				/// The node's Geometry object.
				Rendering::GeometryRef geometry_;
		};

		/**
		 * Definition of an unordered map mapping node ids to node objects.
		 */
		typedef std::unordered_map<ovrAvatar2NodeId, Node, Utilities> NodeMap;

		/**
		 * Definition of an unordered set holding node ids.
		 */
		typedef std::unordered_set<ovrAvatar2NodeId, Utilities> NodeSet;

	public:

		/**
		 * Destructs this avatar object and releases all resources.
		 */
		~Avatar();

		/**
		 * Updates the body tracking data of the avatar.
		 * @param remoteBodyTrackingData The body tracking data to be used to update this avatar, invalid in case no tracking data exists
		 * @param yGroundPlaneElevation The elevation at which the local ground plane is located within the y-direction, with range (-infinity, infinity)
		 */
		void updateBodyTrackingData(Input::SharedBodyTrackingData&& remoteBodyTrackingData, const Scalar yGroundPlaneElevation);

		/**
		 * Updates the avatar rendering objects, should be called for each frame.
		 * @param primitives The primitives of all avatars
		 * @param shaders The shaders of all avatars
		 * @param engine The rendering engine to be used
		 * @param renderRemoteAvatars True, to render remote avatars; False, to hide all remote avatars
		 * @param overwriteEntityId The explicit entity id of the avatar to be used for rendering, can be used to workaround issues in Avatars SDK if only default system models are loaded; ovrAvatar2EntityId_Invalid to use this Avatar's entity id
		 * @return True, if succeeded
		 * @see updateBody().
		 */
		bool updateRendering(const Primitives& primitives, Shaders& shaders, const Rendering::Engine& engine, const bool renderRemoteAvatars, const ovrAvatar2EntityId overwriteEntityId = ovrAvatar2EntityId_Invalid);

		/**
		 * Creates a new rendering instance of the avatar.
		 * @param engine The rendering engine to be used
		 * @return The resulting Transform node with the avatar as child, invalid if the rendering instance could not yet be created
		 */
		Rendering::TransformRef createAvatar(const Rendering::Engine& engine) const;

		/**
		 * Returns the user id of this avatar.
		 * @return The avatar's user id, 0 if invalid
		 */
		inline uint64_t userId() const;

		/**
		 * Returns the transformation between the this avatar at a remote headset and the remote headset's world.
		 * This transformation can be used to e.g., allow an avatar to freely move in a remote world.
		 * @return The transformation between this avatar at the remote headset and the remote headset's world, invalid to hide the local avatar at the remote side
		 */
		inline HomogenousMatrix4 remoteHeadsetWorld_T_remoteAvatar() const;

		/**
		 * Returns the transformation between the avatar's head and the world.
		 * In case this avatar is a remote avatar, remoteHeadsetWorld_T_remoteAvatar() in applied to the head transformation as well.
		 * @return The avatar's head transformation, invalid if unknown
		 */
		inline HomogenousMatrix4 world_T_head() const;

		/**
		 * Sets or updates the transformation between the this avatar at a remote headset and the remote headset's world.
		 * This transformation can be used to e.g., allow an avatar to freely move in a remote world.
		 * This avatar must be a local avatar.
		 * @param remoteHeadsetWorld_T_remoteAvatar The transformation between this avatar at the remote headset and the remote headset's world, invalid to hide the local avatar at the remote side
		 */
		inline void setRemoteHeadsetWorld_T_remoteAvatar(const HomogenousMatrix4& remoteHeadsetWorld_T_remoteAvatar);

		/**
		 * Returns the entity id of this avatar.
		 * @return The avatar's entity id
		 */
		inline ovrAvatar2EntityId entityId() const;

		/**
		 * Returns whether this avatar is currently represented by the default system model.
		 * @return True, if so; False, if the fully customized avatar has been loaded
		 */
		inline bool isDefaultSystemModel() const;

		/**
		 * Returns whether this avatar is a local avatar (the local user of the quest).
		 * @return True, if so; False, if the avatar is a remote avatar (connected via network)
		 */
		inline bool isLocal() const;

		/**
		 * Returns whether this object represents a valid user.
		 * @return True, if so
		 */
		inline bool isValid() const;

	protected:

		/**
		 * Creates a new avatar for a given user id.
		 * @param userType The type of the user for which the avatar will be created, must be valid
		 * @param userId The id of the user for which the avatar will be created, must be valid
		 * @param isLocal True, if the user id is for the local avatar; False, if the user id is for a remote avatar
		 * @see isValid().
		 */
		Avatar(const UserType userType, const uint64_t userId, const bool isLocal = true);

		/**
		 * Disabled copy constructor.
		 * @param avatar The avatar object which would be copy
		 */
		Avatar(const Avatar& avatar) = delete;

		/**
		 * Disabled assign operator.
		 * @param avatar The avatar object that would be assigned
		 * @return Reference to this object
		 */
		Avatar& operator=(const Avatar& avatar) = delete;

		/**
		 * Releases the avatar and releases all resources.
		 */
		void release();

		/**
		 * Event function for new microphone samples.
		 * @param samplesType The type of the samples
		 * @param data The samples data, must be valid
		 * @param size The size of the data, in bytes, with range [1, infinity)
		 */
		void onMicrophoneSamples(const Media::Microphone::SamplesType samplesType, const void* data, const size_t size);

		/**
		 * The static event function to fill input controller states.
		 * @param inputControlState The input control state to be filled, must be valid
		 * @param userContext The context of the event, must be valid
		 * @return True, if at least one hand is visible
		 */
		static bool onDetermineInputControlState(ovrAvatar2InputControlState* inputControlState, void* userContext);

		/**
		 * The static event function to fill input tracking states.
		 * @param inputTrackingState The input tracking state to be filled, must be valid
		 * @param userContext The context of the event, must be valid
		 * @return True, if at least one hand is visible
		 */
		static bool onDetermineInputTrackingState(ovrAvatar2InputTrackingState* inputTrackingState, void* userContext);

		/**
		 * The static event function to fill hand tracking states.
		 * @param handTrackingState The hand tracking state to be filled, must be valid
		 * @param userContext The context of the event, must be valid
		 * @return True, if at least one hand is visible
		 */
		static bool onDetermineHandTrackingState(ovrAvatar2HandTrackingState* handTrackingState, void* userContext);

		/**
		 * The static event function to fill lip sync states.
		 * @param lipSyncState The lip sync state to be filled, must be valid
		 * @param userContext The context of the event, must be valid
		 * @return True, if the state has been filled
		 */
		static bool onDetermineLipSyncState(ovrAvatar2LipSyncState* lipSyncState, void* userContext);

		/**
		 * Translates the user type to a graph type.
		 * @param userType The user type to translate, must be valid
		 * @param readable Optional resulting readable string, nullptr if not of interest
		 * @return The translated graph type
		 */
		static ovrAvatar2Graph translateUserType(const UserType userType, std::string* readable = nullptr);

		/**
		 * Translates the graph type to a user type.
		 * @param graphType The graph type to translate, must be valid
		 * @return The translated user type
		 */
		static UserType translateUserType(const ovrAvatar2Graph graphType);

	protected:

		/// The avatar's user type.
		UserType userType_ = UT_UNKNOWN;

		/// The user id of this avatar.
		uint64_t userId_ = 0ull;

		/// True, if this avatar is currently represented by the default system model; False, if the fully customized avatar has been loaded.
		bool isDefaultSystemModel_ = true;

		/// True, if the user id is for the local avatar; False, if the user id is for a remote avatar.
		bool isLocal_ = false;

		/// The elevation at which the local ground plane is located within the y-direction, with range (-infinity, infinity).
		Scalar yGroundPlaneElevation_ = Scalar(0);

		/// The transformation between this avatar at the remote headset and the remote headset's world, invalid to hide the local avatar at the remote side.
		HomogenousMatrix4 remoteHeadsetWorld_T_remoteAvatar_ = HomogenousMatrix4(false);

		/// The transformation between this avatar's head and this avatar's local world, invalid if unknown.
		HomogenousMatrix4 world_T_head_ = HomogenousMatrix4(false);

		/// The entity id of this avatar.
		ovrAvatar2EntityId entityId_ = ovrAvatar2EntityId_Invalid;

		/// The version the entity's nodes have been changed the last time.
		ovrAvatar2EntityRenderStateVersion nodesVersion_ = ovrAvatar2EntityRenderStateVersion_Invalid;

		/// The version the visibility of the entity's nodes has been changed the last time.
		ovrAvatar2EntityRenderStateVersion visibilityVersion_ = ovrAvatar2EntityRenderStateVersion_Invalid;

		/// The most recent skin matrices, two for each joint.
		HomogenousMatricesF4 skinMatrices_;

		/// The map mapping node ids to nodes.
		NodeMap nodeMap_;

		/// The tranform holding all Geometry objects of this avatar.
		Rendering::TransformRef renderingTransform_;

		/// The manager's body tracking provider.
		ovrAvatar2BodyProvider* bodyTrackingProvider_ = nullptr;

		/// The manager's lip sync provider for local avatars.
		ovrAvatar2LipSyncProvider* localLipSyncProvider_ = nullptr;

		/// The lip sync context for local avatars.
		ovrAvatar2LipSyncContext localLipSyncContext_ = {nullptr, nullptr};

		/// The microphone to be used.
		Media::MicrophoneRef microphone_;

		/// The subscription for microphone samples for local avatars.
		Media::Microphone::SamplesScopedSubscription microphoneSubscription_;

		/// The body tracking states either for the local avatar or received from remote.
		Input::SharedBodyTrackingData bodyTrackingData_;

		/// True, if the body tracker data has been updated before; False, if the body tracker data has not yet been set at all.
		bool hasUpdatedBodyTrackingData_ = false;
};

inline Avatar::Node::Node(const ovrAvatar2PrimitiveId primitiveId) :
	primitiveId_(primitiveId)
{
	ocean_assert(primitiveId_ != ovrAvatar2PrimitiveId_Invalid);
}

inline uint64_t Avatar::userId() const
{
	return userId_;
}

inline HomogenousMatrix4 Avatar::remoteHeadsetWorld_T_remoteAvatar() const
{
	return remoteHeadsetWorld_T_remoteAvatar_;
}

inline HomogenousMatrix4 Avatar::world_T_head() const
{
	if (!isLocal_ && world_T_head_.isValid())
	{
		// this avatar is an avatar of a remote user, so we apply the custom transformation between avatar and world

		if (remoteHeadsetWorld_T_remoteAvatar_.isValid())
		{
			return remoteHeadsetWorld_T_remoteAvatar_ * world_T_head_;
		}

		return HomogenousMatrix4(false);
	}

	return world_T_head_;
}

inline void Avatar::setRemoteHeadsetWorld_T_remoteAvatar(const HomogenousMatrix4& remoteHeadsetWorld_T_remoteAvatar)
{
	ocean_assert(isLocal_);

	remoteHeadsetWorld_T_remoteAvatar_ = remoteHeadsetWorld_T_remoteAvatar;
}

inline ovrAvatar2EntityId Avatar::entityId() const
{
	return entityId_;
}

inline bool Avatar::isDefaultSystemModel() const
{
	return isDefaultSystemModel_;
}

inline bool Avatar::isLocal() const
{
	return isLocal_;
}

inline bool Avatar::isValid() const
{
	ocean_assert(userId_ == 0ull || entityId_ != ovrAvatar2EntityId_Invalid);
	return userId_ != 0ull;
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_AVATARS_AVATAR_H
