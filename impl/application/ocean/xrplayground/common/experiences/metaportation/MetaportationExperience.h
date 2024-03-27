// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_METAPORTATION_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_METAPORTATION_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/metaportation/Metaportation.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/io/Directory.h"

#include "ocean/math/Box3.h"
#include "ocean/math/SampleMap.h"

#include "ocean/media/Audio.h"
#include "ocean/media/Movie.h"

#include "ocean/network/PackagedUDPServer.h"
#include "ocean/network/PackagedTCPClient.h"

#include "ocean/rendering/TriangleFace.h"
#include "ocean/rendering/VertexSet.h"

#include "ocean/tracking/SmoothedTransformation.h"

#include "ocean/tracking/mapbuilding/Unified.h"
#include "ocean/tracking/mapbuilding/UnifiedFeatureMap.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/vrapi/application/VRTableMenu.h"

	#include "metaonly/ocean/platform/meta/avatars/Manager.h"

	#include "ocean/platform/meta/quest/platformsdk/DistributedStates.h"
	#include "ocean/platform/meta/quest/platformsdk/Network.h"
#endif

#include <queue>

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

/**
 * This experience shows how to create a portal of a local space and how to invite other users to this space.
 * @ingroup xrplayground
 */
class MetaportationExperience :
	public XRPlaygroundExperience,
	public Metaportation
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
			/// The user wants to load and to share the room and wants to invite a friend.
			AS_LOAD_ROOM,
			/// The user needs to invite a friend to share the loaded room.
			AS_SHARE_LOADED_ROOM_INVITE,
			/// The user wants to join a room and waits for an invite.
			AS_JOIN_ROOM_WAIT_FOR_INVITE,
			/// The user has accepted the invite to join a room.
			AS_JOIN_ROOM_ACCEPTED_INVITE
		};

		/**
		 * This class holds the relevant input data for all threads.
		 */
		class InputData
		{
			public:

				/**
				 * Updates the input data.
				 * @param yFrames The new frames, at least one
				 * @param cameras The new camera profiles, one for each frame
				 * @param The transformation between device and world, must be valid
				 * @param device_T_cameras The transformations between cameras and device, one for each frame
				 * @param timestamp The timestamp of the new input data, must be valid
				 */
				void updateInputData(std::shared_ptr<Frames> yFrames, SharedAnyCameras cameras, const HomogenousMatrix4& world_T_device, HomogenousMatrices4 device_T_cameras, const Timestamp& timestamp);

				/**
				 * Returns the latest input data.
				 * @param lastTimestamp The timestamp of the last input data the caller used, can be invalid, will be updated with the timestamp of the latest input data if any
				 * @param yFrames The resulting latest frames
				 * @param cameras The resulting camera profiles for the latest frames, one for each frame
				 * @param world_T_device The resulting transformation between device and world
				 * @param device_T_cameras The resulting transformations between cameras and device, one for each frame
				 * @return True, if new input data was available
				 */
				bool latestInputData(Timestamp& lastTimestamp, std::shared_ptr<Frames>& yFrames, SharedAnyCameras& cameras, HomogenousMatrix4& world_T_device, HomogenousMatrices4& device_T_cameras) const;

			protected:

				/// The timestamp of the current input data.
				Timestamp timestamp_ = Timestamp(false);

				/// The current frames.
				std::shared_ptr<Frames> yFrames_;

				/// The current camera profiles.
				SharedAnyCameras cameras_;

				/// The transformation between device and world.
				HomogenousMatrix4 world_T_device_ = HomogenousMatrix4(false);

				/// The transformations between cameras and device.
				HomogenousMatrices4 device_T_cameras_;

				/// The lock for the input data.
				mutable Lock lock_;
		};

		/**
		 * This class holds the relevant map data for all threads.
		 */
		class MapData
		{
			public:

				/**
				 * Updates the map data.
				 * @param objectPoints The 3D object points of the feature map
				 * @param objectPointStabilityFactors The stability factors of the individual feature points
				 * @param multiDescriptors The multi-descriptors of the individual feature poitns
				 */
				void updateMapData(Vectors3&& objectPoints, Scalars&& objectPointStabilityFactors, std::vector<CV::Detector::FREAKDescriptors32>&& multiDescriptors);

				/**
				 * Returns the last map data.
				 * @param objectPoints The resulting 3D object points of the feature map
				 * @param objectPointStabilityFactors The resulting stability factors of the individual feature points
				 * @param multiDescriptors The resulting multi-descriptors of the individual feature poitns
				 * @return True, if this object contained new map data
				 */
				bool latestMapData(Vectors3& objectPoints, Scalars& objectPointStabilityFactors, std::vector<CV::Detector::FREAKDescriptors32>& multiDescriptors) const;

				/**
				 * Updates the feature map.
				 * @param unifiedFeatureMap The new feature map
				 */
				void updateFeatureMap(Tracking::MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap);

				/**
				 * Returns the latest feature map.
				 * @param unifiedFeatureMap The resulting feature map
				 * @return True, if this object contained a new feature map
				 */
				bool latestFeatureMap(Tracking::MapBuilding::SharedUnifiedFeatureMap& unifiedFeatureMap) const;

			protected:

				/// The 3D object points of the feature map.
				Vectors3 objectPoints_;

				/// The stability factors of the individual feature points.
				Scalars objectPointStabilityFactors_;

				/// The multi-descriptors of the individual feature poitns.
				std::vector<CV::Detector::FREAKDescriptors32> multiDescriptors_;

				/// The feature latest feature map.
				Tracking::MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap_;

				/// The lock for the map data.
				mutable Lock lock_;
		};

		class Game
		{
			public:

				virtual ~Game();

				virtual bool initialize(const Rendering::TransformRef& renderingTransform_headset, const Rendering::TransformRef& renderingTransform_headset_T_relocalizer, Rendering::TransformRef& renderingTransform_relocalizer_T_meshes, const Rendering::EngineRef& engine, const Timestamp timestamp);

				/**
				 * Game pre-update function.
				 * @param engine The rendering engine to be used
				 * @param timestamp The timestamp
				 */
				virtual void preUpdateGame(const Rendering::EngineRef& engine, const Timestamp timestamp) = 0;

				inline bool isInitialized() const;

			protected:

				explicit Game(const bool isHost);

			protected:

				/// The id of the local user.
				uint64_t userId_ = 0ull;

				bool isHost_ = false;

				Platform::Meta::Quest::PlatformSDK::DistributedStates distributedStates_;

				Rendering::TransformRef renderingTransformParent_headset_;
				Rendering::TransformRef renderingTransformParent_headset_T_relocalizer_;
				Rendering::TransformRef renderingTransformParent_relocalizer_T_meshes_;

				Rendering::GroupRef renderingGroup_headset_;

				/// The rendering transform node holding the transformation between an optional tracker's world system and the headset's world (e.g., due to a relocalization),
				Rendering::GroupRef renderingGroup_relocalizer_;

				/// The rendering transform node holding all mesh elemets.
				Rendering::GroupRef renderingGroup_meshes_;
		};

		typedef std::shared_ptr<Game> SharedGame;

		class GameVirus : public Game
		{
			protected:

				static constexpr unsigned int numberVirus_ = 50u;

				enum ElementId : uint32_t
				{
					/// Invalid element id.
					EI_INVALID = 0u,

					/// The element id of the hit counter for the host.
					EI_COUNTER_HOST,
					/// The element id of the hit counter for the guest.
					EI_COUNTER_GUEST,

					/// The element id of the host's laser pose.
					EI_LASER_HOST,
					/// The element id of the guest's laser pose.
					EI_LASER_GUEST,

					/// The element id for the position of the first virus.
					EI_VIRUS_POSITIONS_START,
					/// The element id for the position of the (exclusive) last virus.
					EI_VIRUS_POSITIONS_END = EI_VIRUS_POSITIONS_START + numberVirus_
				};

				class Virus
				{
					public:

						Virus(Rendering::TransformRef renderingTransform);

					public:

						bool isPlaced_ = false;

						Rendering::TransformRef renderingTransform_;
				};

				typedef std::unordered_map<uint32_t, Virus> VirusMap;

			public:

				explicit GameVirus(const bool isHost, const Box3& boundingBox);

				~GameVirus() override;

				bool initialize(const Rendering::TransformRef& renderingTransform_headset, const Rendering::TransformRef& renderingTransform_headset_T_relocalizer, Rendering::TransformRef& renderingTransform_relocalizer_T_meshes, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

				/**
				 * Game pre-update function.
				 * @see Game::preUpdateGame().
				 */
				void preUpdateGame(const Rendering::EngineRef& engine, const Timestamp timestamp) override;

			protected:

				void playAudioLaser();
				void playAudioHit();

				/**
				* Event callback function for handled content.
				* @param content The content which has been handled
				* @param succeeded True, if the content could be handled; False, if the content could not be handled
				* @param scenes The rendering scenes that have been created when handling the content
				*/
				void onContentHandled(const std::string& content, const bool succeeded, const std::vector<Rendering::SceneRef>& scenes);

			protected:

				Box3 boundingBox_;

				std::queue<Media::AudioRef> audiosLaser_;
				std::queue<Media::AudioRef> audiosHit_;

				Rendering::TransformRef renderingTransformLaserLocal_;
				Rendering::TransformRef renderingTransformLaserRemote_;

				Rendering::TransformRef renderingTransformVirus_;

				Rendering::TransformRef renderingTransformScore_;
				Rendering::TextRef renderingTextScore_;

				Timestamp hideTimestampLaserLocal_ = Timestamp(false);
				Timestamp hideTimestampLaserRemote_ = Timestamp(false);

				unsigned int hitCounter_ = 0u;

				VirusMap virusMap_;

				Timestamp virusPlacementTimestamp_ = Timestamp(false);

				Timestamp gameOverTimestamp_ = Timestamp(false);
		};

		class GameMovie : public Game
		{
			protected:

				enum ElementId : uint32_t
				{
					/// Invalid element id.
					EI_INVALID = 0u,

					EI_TRANSFORMATION_MOUNT = 1000u,
					EI_PLAY_PAUSE = 1001u
				};

			public:

				explicit GameMovie(const bool isHost);

				~GameMovie() override;

				bool initialize(const Rendering::TransformRef& renderingTransform_headset, const Rendering::TransformRef& renderingTransform_headset_T_relocalizer, Rendering::TransformRef& renderingTransform_relocalizer_T_meshes, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

				/**
				 * Game pre-update function.
				 * @see Game::preUpdateGame().
				 */
				void preUpdateGame(const Rendering::EngineRef& engine, const Timestamp timestamp) override;

			protected:

				Rendering::TransformRef renderingTransformMount_;

				Media::MovieRef movie_;

				bool definingMountLocation_ = true;
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

				/**
				 * Definition of a map mapping bock ids to rendering objects.
				 */
				typedef std::unordered_map<VectorI3, std::pair<Rendering::TransformRef, Box3>, VectorI3> RenderingBlockMap;

				/**
				 * Definition of an unordered map mapping game names to game objects.
				 */
				typedef std::unordered_map<std::string, SharedGame> GameMap;

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

				/**
				 * Key release function.
				 * @see Experience::onKeyRelease().
				 */
				virtual void onKeyRelease(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp);

			protected:

				/**
				 * Renders the portal.
				 * @param engine The rendering engine to be used
				 * @param timestamp The timestamp
				 */
				void renderPortal(const Rendering::EngineRef& engine, const Timestamp timestamp);

				/**
				 * Event function to show the portal menu.
				 * @param world_T_device The transformation between device and headset world
				 */
				virtual void onShowPortalMenu(const HomogenousMatrix4& world_T_device);

				/**
				 * Event function when a portal menu entry was selected.
				 * @param entryUrl The URL of the entry which was selected
				 */
				virtual void onPortalMenuEntrySelected(const std::string& entryUrl);

				/**
				 * Event function when a new mesh has arrived
				 * @param bitstream The bitstream holding the new mesh
				 * @return True, if the mesh was the last mesh in the series; False, if there are still pending meshes
				 */
				bool onReceiveMesh(IO::InputBitstream& bitstream);

				/**
				 * Event function when a new meshes has arrived.
				 * @param meshes The new meshes
				 */
				virtual void onNewMesh(const Meshes& meshes);

			protected:

				/// The id of the remote user.
				uint64_t remoteUserId_ = 0ull;

				/// The rendering transform node holding the rendering content.
				Rendering::TransformRef renderingTransform_headset_;

				/// The rendering transform node holding the transformation between an optional tracker's world system and the headset's world (e.g., due to a relocalization),
				Rendering::TransformRef renderingTransform_headset_T_relocalizer_;

				/// The rendering transform node in which e.g., the meshes are anchored.
				Rendering::TransformRef renderingTransform_relocalizer_T_meshes_;

				/// The rendering group node holding all mesh elements.
				Rendering::GroupRef renderingGroup_meshes_;

				/// The transformation between the mesh and the headset's world.
				Tracking::SmoothedTransformation world_T_mesh_ = Tracking::SmoothedTransformation(1.0);

				/// The rendering Transform object holding the text node, defined in the headset's world.
				Rendering::TransformRef renderingTransformText_headset_;

				/// The rendering Text object holding the instruction text.
				Rendering::TextRef renderingText_;

				/// The map holding all rendering blocks.
				RenderingBlockMap renderingBlockMap_;

				/// The 3D bounding box of the meshes.
				Box3 boundingBox_;

				/// The queue holding the latest meshes objects which need to be rendered.
				std::queue<Meshes> meshesQueue_;

				/// The render mode to be used.
				RenderMode renderMode_ = RM_TEXTURED;

				/// The timestamp when the next network throughput will be displayed.
				Timestamp nextNetworkDataThroughputTimestamp_;

				/// The table menu allowing to show a simple user interface.
				Platform::Meta::Quest::VrApi::Application::VRTableMenu vrTableMenu_;

				/// The timestamp when the A button was pressed the last time.
				Timestamp timestampPressedButtonA_;

				/// The game map with currently active games.
				GameMap gameMap_;

				/// The portal's lock.
				mutable Lock lock_;
		};

		// Forward declaration.
		class MapHandlingThread;

		/**
		 * This class implements a creator portal.
		 */
		class PortalCreator :
			public PortalBase,
			protected Thread
		{
			friend class MapHandlingThread;

			protected:

				/**
				 * Definition of a vector holding a buffer.
				 */
				typedef std::vector<uint8_t> Buffer;

				/**
				 * Definition of a queue holding buffers.
				 */
				typedef std::queue<Buffer> BufferQueue;

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
				 * Event function to show the portal menu.
				 * @see PortalBase::onShowPortalMenu().
				 */
				void onShowPortalMenu(const HomogenousMatrix4& world_T_device) override;

				/**
				 * Event function when a portal menu entry was selected.
				 * @see PortalBase::onPortalMenuEntrySelected().
				 */
				void onPortalMenuEntrySelected(const std::string& entryUrl) override;

				/**
				 * Event function when a new meshes has arrived.
				 * @see PortalBase::onReceiveMesh()
				 */
				void onNewMesh(const Meshes& meshes) override;

				/**
				 * Event function for receiving data from mobile phone via TCP.
				 * @param data The data that has been received
				 * @param size The size of the data, in bytes
				 */
				void onReceiveFromMobileTCP(const void* data, const size_t size);

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

				/**
				 * Saves the current room to a directory.
				 * @param directory The directory to which the room will be written
				 * @return True, if succeeded
				 */
				bool saveRoom(const IO::Directory& directory);

			protected:

				/// The subscription object to receive data from remote headsets.
				Platform::Meta::Quest::PlatformSDK::Network::ReceiveScopedSubscription receiveSubscription_;

				/// The rendering Transform object for the scanning device location.
				Rendering::TransformRef renderingTransformDevice_headset_;

				/// The timestamp when the last device transformation was set.
				std::atomic<Timestamp> lastDeviceTimestamp_ = Timestamp(false);

				/// The TCP client which will be used for the data exchange with lower priority..
				Network::PackagedTCPClient tcpClient_;

				/// The UDP server which will receive data with low latency constraints.
				Network::PackagedUDPServer udpServer_;

				/// The queue with mesh buffers to be sent.
				BufferQueue meshBufferQueue_;

				/// The creator's map data.
				std::shared_ptr<MapData> mapData_;

				/// The manager holding all textured meshes.
				MeshesManager meshesManager_;
		};

		/**
		 * This class implements a loader for a portal.
		 */
		class PortalLoader :
			public PortalBase,
			protected Thread
		{
			protected:

				/**
				 * Definition of a vector holding a buffer.
				 */
				typedef std::vector<uint8_t> Buffer;

			public:

				/**
				 * Initializes the portal.
				 * @param parent The portal's parent Transform node, must be valid
				 * @param engine The rendering engine to be used
				 * @param timestamp The current timestamp
				 * @param directory The directory of the room
				 * @return True, if succeeded
				 */
				bool initialize(const Rendering::TransformRef& parent, const Rendering::EngineRef& engine, const Timestamp timestamp, const IO::Directory& directory);

				/**
				 * Sets the id of the remote user.
				 * @param remoteUserId The id of the remote user, must be valid
				 * @return True, if succeeded
				 */
				bool setRemoteUserId(const uint64_t remoteUserId);

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
				 * Event function to show the portal menu.
				 * @see PortalBase::onShowPortalMenu().
				 */
				void onShowPortalMenu(const HomogenousMatrix4& world_T_device) override;

				/**
				 * Event function when a portal menu entry was selected.
				 * @see PortalBase::onPortalMenuEntrySelected().
				 */
				void onPortalMenuEntrySelected(const std::string& entryUrl) override;

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

				/**
				 * Loads a room from a directory.
				 * @param directory The directory to which the room will be read
				 * @return True, if succeeded
				 */
				bool loadRoom(const IO::Directory& directory);

			protected:

				/// The directory containing the room to load.
				IO::Directory directory_;

				/// The meshes files to be sent to the remote user.
				IO::Files meshesFilesForRemoteUser_;

				/// The subscription object to receive data from remote headsets.
				Platform::Meta::Quest::PlatformSDK::Network::ReceiveScopedSubscription receiveSubscription_;

				/// The rendering transform node holding the objet points of the map.
				Rendering::TransformRef renderingTransformMapPoints_;

				/// The on-device relocalizer tracker.
				Devices::SceneTracker6DOFRef sceneTracker_;

				/// True, if the relocalizer has provided at least one valid pose.
				bool firstRelocalizationReceived_ = false;
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
				Timestamp preUpdate(const Rendering::EngineRef& engine, const Timestamp timestamp);

			protected:

				/**
				 * Event function to show the portal menu.
				 * @see PortalBase::onShowPortalMenu().
				 */
				void onShowPortalMenu(const HomogenousMatrix4& world_T_device) override;

				/**
				 * Event function when a portal menu entry was selected.
				 * @see PortalBase::onPortalMenuEntrySelected().
				 */
				void onPortalMenuEntrySelected(const std::string& entryUrl) override;

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

				/// The translation between the host's floor and world.
				Vector3 hostWorld_t_hostFloor_ = Vector3(0, 0, 0);

				/// The optional transformation between the relocalizer tracker's world and the headset's world.
				HomogenousMatrix4 headsetWorld_T_trackerWorld_ = HomogenousMatrix4(false);

				/// The portal's lock.
				Lock lock_;
		};

		/**
		 * This class implements an own thread for the map creator.
		 */
		class MapCreatorThread final : protected Thread
		{
			public:

				/**
				 * Creates a new thread object.
				 * @param inputData The input data the map creator will used
				 * @param mapData The map data receiving the maps
				 */
				MapCreatorThread(const InputData& inputData, MapData& mapData);

			protected:

				/**
				 * The thread run function.
				 */
				void threadRun() override;

			protected:

				/// The input data the map creator will used.
				const InputData& inputData_;

				/// The map data receiving the maps.
				MapData& mapData_;
		};

		/**
		 * This class implements an own thread for handling maps.
		 */
		class MapHandlingThread final : public Thread
		{
			public:

				/**
				 * Creates a new thread object.
				 * @param owner The owner of this object
				 * @param mapData The map data receiving the maps
				 */
				MapHandlingThread(PortalCreator& owner, MapData& mapData);

			protected:

				/**
				 * The thread run function.
				 */
				void threadRun() override;

			protected:

				/// The portal creator.
				PortalCreator& owner_;

				/// The map data.
				MapData& mapData_;
		};

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~MetaportationExperience() override;

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
		 * Key release function.
		 * @see Experience::onKeyRelease().
		 */
		void onKeyRelease(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

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
		 * Detects QRCodes an both stereo cameras and extracts the network address for the map alignment experience.
		 * @param inputData The input data holding the camera images
		 * @param address The resulting address of the mobile phone
		 * @param port The resulting port of the mobile phone
		 * @return True, if succeeded
		 */
		static bool detectQRCodeWithAddress(const InputData& inputData, Network::Address4& address, Network::Port& port);

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

		/// The portal creator in case this user loads a portal.
		std::shared_ptr<PortalLoader> portalLoader_;

		/// The portal reciver in case this user recives a portal.
		std::shared_ptr<PortalReceiver> portalReceiver_;

		/// The timestamp when the user started a jump interaction.
		Timestamp jumpStartTimestamp_;
};

inline bool MetaportationExperience::Game::isInitialized() const
{
	return bool(renderingGroup_headset_);
}

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

/**
 * This experience shows how to use Avatars in an experience.
 * @ingroup xrplayground
 */
class MetaportationExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~MetaportationExperience() override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();
};

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_METAPORTATION_EXPERIENCE_H
