// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_METAPORTATION_SCANNER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_METAPORTATION_SCANNER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/metaportation/Metaportation.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Thread.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/io/Bitstream.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/network/PackagedUDPClient.h"
#include "ocean/network/PackagedTCPServer.h"

#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Group.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/Text.h"
#include "ocean/rendering/Textures.h"
#include "ocean/rendering/VertexSet.h"

#include "ocean/tracking/SmoothedTransformation.h"

#include "ocean/tracking/mapbuilding/Unified.h"
#include "ocean/tracking/mapbuilding/UnifiedFeatureMap.h"

#include "ocean/tracking/maptexturing/NewTextureGenerator.h"

#include <reconstruction/systems/DefaultFusionSystem.h>

#include <queue>

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements the Metaportation Scanner experience for phones.
 * @ingroup xrplayground
 */
class MetaportationScannerExperience :
	public XRPlaygroundExperience,
	public Metaportation,
	protected Thread
{
	public:

		/**
		 * Definition of individual scanning modes.
		 */
		enum ScanningMode : uint32_t
		{
			/// Invalid scanning mode.
			SM_INVALID = 0u,
			/// Scan with image texture.
			SM_TEXTURED,
			/// Scan with per-vertex colors.
			SM_PER_VERTEX_COLORS
		};

	protected:

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

		/**
		 * Re-definition of SceneElementMeshes.
		 */
		typedef Devices::SceneTracker6DOF::SceneElementMeshes SceneElementMeshes;

		/**
		 * This class implements a storage for the latest feature map.
		 */
		class FeatureMap final : protected Thread
		{
			public:

				/**
				 * Returns the latest feautre map for relocalization, if any exists.
				 * @param unifiedFeatureMap The resulting feature map
				 * @return True, if a map existed
				 */
				bool latestFeatureMap(Tracking::MapBuilding::SharedUnifiedFeatureMap& unifiedFeatureMap);

				/**
				 * Event function for received data from Quest.
				 * @param bitstream The bitstream containing the received data
				 */
				void onReceiveData(IO::InputBitstream& bitstream);

			protected:

				/**
				 * The thread run function.
				 */
				void threadRun() override;

			protected:

				/// The 3D object points for the feature map.
				Vectors3 mapObjectPoints_;

				/// The ids of the object points in the feature map, one for each 3D object point.
				Indices32 mapObjectPointIds_;

				/// The map mapping object point ids to feature descriptors
				std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> mapUnifiedDescriptorMap_;

				/// The feature map for relocalization.
				Tracking::MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap_;

				/// The lock for the feature map.
				Lock lock_;
		};

		/**
		 * This class implements a mesh generator for meshes.
		 */
		class MeshGenerator
		{
			public:

				/**
				 * Default constructor.
				 * @param scanningMode The scanning mode to be used
				 */
				explicit MeshGenerator(const ScanningMode scanningMode);

				/**
				 * Starts the meshing process.
				 * @return True, if succeeded
				 */
				bool start();

				/**
				 * Stops the meshing process.
				 * @return True, if succeeded
				 */
				bool stop();

				/**
				 * Updates the mesh.
				 * @param recentSceneElement The recent scene element holding a depth map, can be invalid to skip
				 * @param frameMedium The frame medium providing the current camera image, must be valid
				 * @param recentSceneElementTimestamp The timestamp of the given scene element, must be valid
				 * @param world_T_recentCamera The transformation between the current camera and world, must be valid
				 * @return True, if succeeded
				 */
				bool updateMesh(Devices::SceneTracker6DOF::SharedSceneElement recentSceneElement, const Media::FrameMediumRef& frameMedium, const Timestamp& recentSceneElementTimestamp, const HomogenousMatrix4& world_T_recentCamera);

				/**
				 * Returns the latest mesh if any
				 * @param texturedMeshMap The map holding the latest textured mesh
				 * @param textureFrame The texture which is used by the mesh
				 * @return True, if so
				 */
				bool latestMesh(Tracking::MapTexturing::NewTextureGenerator::TexturedMeshMap& texturedMeshMap, Frame& textureFrame);

				/**
				 * Retruns whether this generator is currently active.
				 * @return True, if so
				 */
				bool isActive();

			protected:

				/// The scanning mode to be used.
				ScanningMode scanningMode_ = SM_INVALID;

				/// The VOGON fusion system creating the mesh.
				vogon::DefaultFusionSystem vogonFusionSystem_;

				/// The camera frustums of all camera poses which have contributed to the latest VOGON state.
				Frustums frustums_;

				/// The keyframes of all cameras which have contributed to the latest VOGON state.
				Tracking::MapTexturing::NewTextureGenerator::Keyframes keyframes_;

				/// The transformation between the previous camera and world.
				HomogenousMatrix4 world_T_previousCamera_ = HomogenousMatrix4(false);

				/// The timestamp of the previous scene element (the previous camera).
				Timestamp previousSceneElementTimestamp_;

				/// The timestamp when the next textured mesh will be generator.
				Timestamp nextMeshTimestamp_;

				/// The actual generator for the textured mesh.
				Tracking::MapTexturing::NewTextureGenerator textureGenerator_;

				/// True, if the generator is currently active.
				bool isActive_ = false;
		};

		/**
		 * This class implements the relocalizer running in an own thread.
		 */
		class RelocalizerWorker : protected Thread
		{
			public:

				/**
				 * Creates a new relocalizer worker object.
				 * @param frameMedium The frame medium to be used for relocalizing, must be valid
				 * @param sceneTracker6DOF The scene tracker for SLAM access, must be valid
				 * @param featureMap The feature map providing updated feature maps for relocalization
				 */
				RelocalizerWorker(Media::FrameMediumRef frameMedium, Devices::SceneTracker6DOFRef sceneTracker6DOF, FeatureMap& featureMap);

				/**
				 * Destructs this worker.
				 */
				~RelocalizerWorker();

				/**
				 * Returns the smoothed transformation between phone and headset.
				 * @param timestamp The timestamp for which the smoothed transformation will be returned
				 * @return The smoothed transformation
				 */
				HomogenousMatrix4 headsetWorld_T_phoneWorld(const Timestamp& timestamp);

			protected:

				/**
				 * The internal thread function function.
				 */
				void threadRun() override;

			protected:

				/// The frame medium to be used for relocalizing.
				Media::FrameMediumRef frameMedium_;

				/// The scene tracker for SLAM access.
				Devices::SceneTracker6DOFRef sceneTracker6DOF_;

				/// The feature map providing updated feature maps for relocalization.
				FeatureMap& featureMap_;

				 /// The transformation between the map's world and the headset's world.
				Tracking::SmoothedTransformation headsetWorld_T_phoneWorld_ = Tracking::SmoothedTransformation(0.5);
		};

		/**
		 * This class implements an independent thread in which the network communication takes place.
		 */
		class NetworkWorker : protected Thread
		{
			public:

				/**
				 * Creates a new networker object.
				 * @param owner The owner of this worker
				 * @apram featureMap The object for feature map exchange
				 * @param renderingText The rendering text which can be used for some debug output, must be valid
				 */
				NetworkWorker(MetaportationScannerExperience& owner, FeatureMap& featureMap, const Rendering::TextRef& renderingText);

				/**
				 * Destructs the network thread.
				 */
				~NetworkWorker() override;

				/**
				 * Sets a new camera pose to send.
				 * @param world_T_recentCamera The transformation between the recent camera and world, must be valid
				 */
				void newPose(const HomogenousMatrix4& world_T_recentCamera);

				/**
				 * Sets a new mesh to send.
				 * @param hmdWorld_T_phoneWorld The transformation between phone and hmd, must be valid
				 * @param textureMeshMap The map holding the mesh to be sent
				 * @param textureFrame The texture which is used by the mesh, must be valid
				 */
				void newMesh(const HomogenousMatrix4& hmdWorld_T_phoneWorld, Tracking::MapTexturing::NewTextureGenerator::TexturedMeshMap&& texturedMeshMap, Frame&& textureFrame);

				/**
				 * Returns the port of the TCP server.
				 * @return The server's port
				 */
				Network::Port tcpServerPort();

			protected:

				/**
				 * The internal thread function function.
				 */
				void threadRun() override;

				/**
				 * Event function for connection requests.
				 * @param senderAddress The address of the sender
				 * @param senderPort The port of the sender
				 * @param connectionId The id of the connection
				 */
				bool onConnectionRequestFromHeadsetTCP(const Network::Address4& senderAddress, const Network::Port& senderPort, const Network::ConnectionOrientedServer::ConnectionId connectionId);

				/**
				 * Event function for a disconnected connection.
				 * @param connectionId The id of the disconnected connection
				 */
				void onConnectionDisconnectedFromHeadsetTCP(const Network::ConnectionOrientedServer::ConnectionId connectionId);

				/**
				 * Event function for receiving data via TCP from the headset.
				 * @param connectionId The id of the connection from which the data has been received
				 * @param data The received data
				 * @param size The number of bytes received
				 */
				void onReceiveFromHeadsetTCP(const Network::ConnectionOrientedServer::ConnectionId connectionId, const void* data, const size_t size);

			protected:

				/// The owner of this object.
				MetaportationScannerExperience& owner_;

				/// The feature map.
				FeatureMap& featureMap_;

				/// The rendering text which can be used for some debug output.
				Rendering::TextRef renderingText_;

				/// The lock for the camera transformation.
				Lock poseLock_;

				/// The queue holding all meshes.
				std::queue<Meshes> meshesQueue_;

				/// The lock for the mesh.
				Lock meshLock_;

				/// The TCP server which will be used for the data exchange with lower priority.
				Network::PackagedTCPServer tcpServer_;

				/// The UDP client which will send low latency data to the reciever.
				Network::PackagedUDPClient udpClient_;

				/// The connection id for the TCP Server
				std::atomic<Network::PackagedTCPServer::ConnectionId> connectionId_ = Network::PackagedTCPServer::invalidConnectionId();

				/// The UDP address of the headset.
				Network::Address4 udpAddress_;

				/// The UDP port of the headset.
				Network::Port udpPort_;
		};

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~MetaportationScannerExperience() override;

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
		 * Starts the scanning and streaming session.
		 * @param scanningMode The scanning mode to be used
		 * @return True, if succeeded
		 */
		bool start(const ScanningMode scanningMode);

		/**
		 * Stops the scanning process.
		 * @return True, if succeeded
		 */
		bool stop();

		/**
		 * Creates a new GravityAndHeadingNativeExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		MetaportationScannerExperience() = default;

		/**
		 * Event function for new tracking samples from the scene tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from scene tracker
		 */
		void onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample);

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

		/**
		 * The internal thread function.
		 */
		void threadRun() override;

		/**
		 * Creates the image of a QR code.
		 * @param data The binary payload that will be encoded into the QR code, must be valid
		 * @return Frame with QRCode, if successful
		 */
		Frame generateQRCodeFrame(const std::vector<uint8_t>& data);

	protected:

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The scene tracker providing access to be used in this experience.
		Devices::SceneTracker6DOFRef sceneTracker6DOF_;

		/// The object id of the scene tracker.
		Devices::SceneTracker6DOF::ObjectId objectId_ = Devices::SceneTracker6DOF::invalidObjectId();

		/// The subscription object for sample events from the scene tracker.
		Devices::Measurement::SampleEventSubscription sceneTrackerSampleEventSubscription_;

		/// The rendering group node holding the rendering content.
		Rendering::GroupRef renderingGroup_;

		/// The map mapping block ids to rendering objects.
		RenderingBlockMap renderingBlockMap_;

		/// The rendering transform node holding the QR code.
		Rendering::TransformRef renderingTransformQRCode_;

		/// The most recent scene element to process, empty otherwise.
		Devices::SceneTracker6DOF::SharedSceneElement recentSceneElement_;

		/// The recent transformation between camera and world.
		HomogenousMatrix4 phoneWorld_T_recentCamera_ = HomogenousMatrix4(false);

		/// The round robin mesh index to be considered during the next update iteration.
		size_t roundRobinMeshUpdateIndex_ = 0;

		/// The timestamp when the last scene element was updated.
		Timestamp recentSceneElementTimestamp_;

		/// The timestamp when the last rendering object was updated.
		Timestamp renderingObjectTimestamp_;

		/// The frame medium of the video background.
		Media::FrameMediumRef frameMedium_;

		/// The rendering Text node providing user feedback.
		Rendering::TextRef renderingText_;

		/// The container holding the latest feature map.
		FeatureMap featureMap_;

		/// The mesh generator using per-vertex colors.
		std::shared_ptr<MeshGenerator> meshGenerator_;

		/// The network worker thread.
		std::shared_ptr<NetworkWorker> networkWorker_;

		/// The relocalizer worker thread.
		std::shared_ptr<RelocalizerWorker> relocalizerWorker_;

		/// The experience's lock.
		Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_METAPORTATION_SCANNER_EXPERIENCE_H
