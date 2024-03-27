// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_LEGACY_METAPORTATION_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_LEGACY_METAPORTATION_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/math/SampleMap.h"

#include "ocean/network/PackagedUDPServer.h"
#include "ocean/network/PackagedTCPServer.h"

#include "ocean/rendering/TriangleFace.h"
#include "ocean/rendering/VertexSet.h"

#include "ocean/tracking/SmoothedTransformation.h"

#include "ocean/tracking/mapbuilding/Unified.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/vrapi/application/VRTableMenu.h"

	#include "metaonly/ocean/platform/meta/avatars/Manager.h"

	#include "ocean/platform/meta/quest/platformsdk/Network.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

/**
 * This experience shows how to create a portal of a local space and how to invite other users to this space.
 * @ingroup xrplayground
 */
class LegacyMetaportationExperience : public XRPlaygroundExperience
{
	protected:

		/**
		 * Definition of individual application states.
		 */
		enum ApplicationState
		{
			/// The application is idling.
			AS_IDLE,
			/// The user has to decide which mode is used.
			AS_USER_SELECTING_MODE,
			/// The user wants to share the room and wants to invite a friend.
			AS_SHARE_ROOM_INVITE,
			/// The needs to connect with the phone.
			AS_SHARE_ROOM_CONNECT_WITH_PHONE,
			/// The user wants to join a room and waits for an invite.
			AS_JOIN_ROOM_WAIT_FOR_INVITE,
			/// The user has accepted the invite to join a room.
			AS_JOIN_ROOM_ACCEPTED_INVITE
		};

		/**
		 * The base class for creator and receiver portals.
		 */
		class PortalBase
		{
			protected:

				/**
				 * Definition of individual render modes.
				 */
				enum RenderMode : uint32_t
				{
					/// The environment is composed of a textured mesh.
					RM_TEXTURED = 0u,
					/// The environment is composed of points.
					RM_POINTS,
					/// The environment is composed of a shaded mesh.
					RM_SHADED,
					/// The end element.
					RM_END
				};

			protected:

				/**
				 * Definition of a class holding the relevant information of a mesh.
				 */
				class MeshObject
				{
					public:

						/// The transformation between the mesh and world.
						HomogenousMatrix4 world_T_mesh_ = HomogenousMatrix4(false);

						/// The vertices of the mesh.
						Vectors3 vertices_;

						/// The per-vertex normals of the mesh, if known.
						Vectors3 perVertexNormals_;

						/// The per-vertex colors of the mesh, if known.
						RGBAColors perVertexColors_;

						/// The per-vertex texture coordinates, if known.
						Vectors2 textureCoordinates_;

						/// The faces of the triangles.
						Rendering::TriangleFaces triangleFaces_;

						/// The texture frame.
						Frame textureFrame_;

						/// The Transform node holding the mesh.
						Rendering::TransformRef renderingTransform_;
				};

				/**
				 * Definition of an unordered map mapping unique mesh ids to mesh objects.
				 */
				typedef std::unordered_map<Index32, MeshObject> MeshObjectMap;

			public:

				/**
				 * Destructs the object.
				 */
				virtual ~PortalBase();

				/**
				 * Initializes the portal.
				 * @param parent The portal's parent Transform node, must be valid
				 * @param engine The rendering engine to be used
				 * @param timestamp The current timestamp
				 * @return True, if succeeded
				 */
				bool initialize(const Rendering::TransformRef& parent, const Rendering::EngineRef& engine, const Timestamp timestamp);

				/**
				 * Releases the portal and all resources.
				 */
				virtual void release();

				/**
				 * Key press function.
				 * @see Experience::onKeyPress().
				 */
				virtual void onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp);

			protected:

				/**
				 * Renders the portal.
				 * @param engine The rendering engine to be used
				 */
				void renderPortal(const Rendering::EngineRef& engine);

				/**
				 * Event function when a new mesh has arrived
				 * @param bitstream The bitstream holding the new mesh
				 * @return True, if the mesh was the last mesh in the series; False, if there are still pending meshes
				 */
				bool onReceiveMesh(IO::InputBitstream& bitstream);

			protected:

				/// The id of the remote user.
				uint64_t remoteUserId_ = 0ull;

				/// The rendering transform node holding the rendering content.
				Rendering::TransformRef renderingTransform_;

				/// The rendering Text object holding the instruction text.
				Rendering::TextRef renderingText_;

				/// The map mapping unique mesh ids to active mesh objects.
				MeshObjectMap activeMeshObjectMap_;

				/// The map mapping unique mesh ids to the next mesh objects.
				MeshObjectMap nextMeshObjectMap_;

				/// The map mapping unique mesh ids to pending mesh objects.
				MeshObjectMap pendingMeshObjectMap_;

				/// The render mode to be used.
				RenderMode renderMode_ = RM_TEXTURED;

				/// True, when rendering needs to be upated.
				bool updateRendering_ = false;

				/// The transformation between the mesh's world and the headset's world.
				HomogenousMatrix4 headsetWorld_T_meshWorld_ = HomogenousMatrix4(true);

				/// The timestamp when the next network throughput will be displayed.
				Timestamp nextNetworkDataThroughputTimestamp_;

				/// The portal's lock.
				mutable Lock lock_;
		};

		/**
		 * This class implements a creator portal.
		 */
		class PortalCreator :
			public PortalBase,
			protected Thread
		{
			protected:

				/**
				 * Definition of a vector holding a buffer.
				 */
				typedef std::vector<uint8_t> Buffer;

				/**
				 * Definition of a vector holding bufers.
				 */
				typedef std::vector<Buffer> Buffers;

			public:

				/**
				 * Initializes the portal.
				 * @param parent The portal's parent Transform node, must be valid
				 * @param engine The rendering engine to be used
				 * @param timestamp The current timestamp
				 * @return True, if succeeded
				 */
				bool initialize(const Rendering::TransformRef& parent, const Rendering::EngineRef& engine, const Timestamp timestamp);

				/**
				 * Sets the id of the remote user.
				 * @param remoteUserId The id of the remote user, must be valid
				 * @return True, if succeeded
				 */
				bool setRemoteUserId(const uint64_t remoteUserId);

				/**
				 * Returns the transformation between the remote avatar and this headset's world.
				 * @return The resulting transformation
				 */
				inline HomogenousMatrix4 remoteHeadsetWorld_T_remoteAvatar() const;

				/**
				 * Releases the portal and all resources.
				 */
				void release() override;

				/**
				 * Updates all rendering components.
				 * @param engine The rendering engine to be used
				 * @param timestamp The current timestamp
				 * @return The updated timestamp
				 */
				Timestamp preUpdate(const Rendering::EngineRef& engine, const Timestamp timestamp);

			protected:

				/**
				 * Event function for connection requests.
				 * @param senderAddress The address of the sender
				 * @param senderPort The port of the sender
				 * @param connectionId The id of the connection
				 */
				bool onConnectionRequestFromMobileTCP(const Network::Address4& senderAddress, const Network::Port& senderPort, const Network::ConnectionOrientedServer::ConnectionId connectionId);

				/**
				 * Event function for a disconnected connection.
				 * @param connectionId The id of the disconnected connection
				 */
				void onConnectionDisconnectedFromMobileTCP(const Network::ConnectionOrientedServer::ConnectionId connectionId);

				/**
				 * Event function for receiving data from mobile phone via TCP.
				 * @param connectionId The id of the connection from which the data has been received
				 * @param data The data that has been received
				 * @param size The size of the data, in bytes
				 */
				void onReceiveFromMobileTCP(const Network::ConnectionOrientedServer::ConnectionId connectionId, const void* data, const size_t size);

				/**
				 * Event function for reciving data from mobile phone via UDP.
				 * @param address The address of the sender
				 * @param port The port of the sender
				 * @param data The data that has been received
				 * @param size The number of bytes
				 * @param messageId The message id
				 */
				void onReceiveFromMobileUDP(const Network::Address4& address, const Network::Port& port, const void* data, const size_t size, const Network::PackagedUDPServer::MessageId messageId);

				/**
				 * Event function for receiving data from a remote headset.
				 * @param senderUserId The id of the user sending the data
				 * @param componentId The id of the component to which this message belongs
				 * @param data The data that has been received
				 * @param size The size of the data, in bytes
				 * @param connectionType The type of the connection which has been used
				 */
				void onReceiveFromHeadset(const uint64_t senderUserId, const Platform::Meta::Quest::PlatformSDK::Network::ComponentId componentId, const void* data, const size_t size, const Platform::Meta::Quest::PlatformSDK::Network::ConnectionType connectionType);

				/**
				 * The thread's run function.
				 */
				void threadRun() override;

			protected:

				/// The subscription object to receive data from remote headsets.
				Platform::Meta::Quest::PlatformSDK::Network::ReceiveScopedSubscription receiveSubscription_;

				/// The rendering Transform object for the scanning device location.
				Rendering::TransformRef renderingTransformDevice_;

				/// The TCP server which will receive the data from the sender.
				Network::PackagedTCPServer tcpServer_;

				/// The UDP server which will receive data with low latency constraints.
				Network::PackagedUDPServer udpServer_;

				/// The most recent camera pose, invalid otherwise.
				HomogenousMatrix4 world_T_recentCamera_ = HomogenousMatrix4(false);

				/// The most recent 3D object points of the feature map.
				Vectors3 mapObjectPoints_;

				/// The most recent ids of the 3D object points of the feature map, one for each object point.
				Indices32 mapObjectPointIds_;

				/// The most recent map mapping object point ids to descriptors.
				std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> unifiedDescriptorMap_;

				/// The map with recent headset poses
				SampleMap<HomogenousMatrix4> headsetPoses_ = SampleMap<HomogenousMatrix4>(500);

				/// The transformation between the map's world and the headset's world.
				Tracking::SmoothedTransformation headsetWorld_T_mapWorld_ = Tracking::SmoothedTransformation(2);

				/// The transformation between the remote avatar and the remote headset's world (in case the remote avatar jumps/moves manually).
				HomogenousMatrix4 remoteHeadsetWorld_T_remoteAvatar_ = HomogenousMatrix4(true);

				/// The mesh buffers ready to be sent.
				Buffers completeMeshBuffers_;

				/// The pending mesh buffers.
				Buffers pendingMeshBuffers_;
		};

		/**
		 * This class implements a receiver portal.
		 */
		class PortalReceiver : public PortalBase
		{
			public:

				/**
				 * Initializes the portal.
				 * @param parent The portal's parent Transform node, must be valid
				 * @param engine The rendering engine to be used
				 * @param timestamp The current timestamp
				 * @return True, if succeeded
				 */
				bool initialize(const Rendering::TransformRef& parent, const uint64_t senderUserId, const Rendering::EngineRef& engine, const Timestamp timestamp);

				/**
				 * Releases the portal and all resources.
				 */
				void release() override;

				/**
				 * Updates all rendering components.
				 * @param engine The rendering engine to be used
				 * @param timestamp The current timestamp
				 * @return The updated timestamp
				 */
				Timestamp preUpdate(const HomogenousMatrix4& headsetWorld_T_avatar, const Rendering::EngineRef& engine, const Timestamp timestamp);

			protected:

				/**
				 * Event function for receiving data from a remote headset.
				 * @param senderUserId The id of the user sending the data
				 * @param componentId The id of the component to which this message belongs
				 * @param data The data that has been received
				 * @param size The size of the data, in bytes
				 * @param connectionType The type of the connection which has been used
				 */
				void onReceiveFromHeadset(const uint64_t senderUserId, const Platform::Meta::Quest::PlatformSDK::Network::ComponentId componentId, const void* data, const size_t size, const Platform::Meta::Quest::PlatformSDK::Network::ConnectionType connectionType);

			protected:

				/// The subscription object to receive data from remote headsets.
				Platform::Meta::Quest::PlatformSDK::Network::ReceiveScopedSubscription receiveSubscription_;
		};

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~LegacyMetaportationExperience() override;

		/**
		 * Loads this experience.
		 * @see Experience::load().
		 */
		bool load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

		/**
		 * Unloads this experience.
		 * @see Experience::unload().
		 */
		bool unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
		 * @see Experience::preUpdate().
		 */
		Timestamp preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Key press function.
		 * @see Experience::onKeyPress().
		 */
		void onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Handles potential movements of the user using the joystick.
		 * @param timestamp The current timestamp
		 */
		void handleUserMovement(const Timestamp& timestamp);

		/**
		 * The event function for a new rendering instance of an avatar.
		 * @param userId The id of the user of the new instance
		 * @param avatarTransform The Transform node with the avatar as child, invalid if the rendering instance could not yet be created
		 */
		void onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform);

		/**
		 * Returns the zone name for a given room id.
		 * @param roomId The id of the room, must be valid
		 * @return The name of the zone
		 */
		static std::string roomZoneName(const uint64_t roomId);

	protected:

		/// The application's state.
		ApplicationState applicationState_ = AS_IDLE;

		/// The rendering transformation object holding the mesh and object located in the mesh.
		Rendering::TransformRef renderingTransformMesh_;

		/// The rendering transformation object holding the remote avatar.
		Rendering::TransformRef renderingTransformRemoteAvatar_;

		/// The group containing text visualizations.
		Rendering::GroupRef renderingTextGroup_;

		/// The unique id of the user.
		uint64_t userId_ = 0ull;

		/// The id of the local room, if the user created one.
		uint64_t localRoomId_ = 0ull;

		/// The id of the remote user.
		uint64_t remoteUserId_ = 0ull;

		/// The id of the remote room to join, if the user was invited to a room.
		uint64_t remoteRoomId_ = 0ull;

		/// The subscription objects for the zone.
		Platform::Meta::Avatars::Manager::ZoneScopedSubscription zoneScopedSubscription_;

		/// The subscription objects for the avatars.
		Platform::Meta::Avatars::Manager::AvatarScopedSubscriptions avatarScopedSubscriptions_;

		/// The table menu allowing to show a simple user interface.
		Platform::Meta::Quest::VrApi::Application::VRTableMenu vrTableMenu_;

		/// The portal creator in case this user creates a portal.
		std::shared_ptr<PortalCreator> portalCreator_;

		/// The portal reciver in case this user recives a portal.
		std::shared_ptr<PortalReceiver> portalReceiver_;

		/// The timestamp when the user started a jump interaction.
		Timestamp jumpStartTimestamp_;
};

inline HomogenousMatrix4 LegacyMetaportationExperience::PortalCreator::remoteHeadsetWorld_T_remoteAvatar() const
{
	const ScopedLock scopedLock(lock_);

	return remoteHeadsetWorld_T_remoteAvatar_;
}

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

/**
 * This experience shows how to use Avatars in an experience.
 * @ingroup xrplayground
 */
class LegacyMetaportationExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~LegacyMetaportationExperience() override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();
};

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_LEGACY_METAPORTATION_EXPERIENCE_H
