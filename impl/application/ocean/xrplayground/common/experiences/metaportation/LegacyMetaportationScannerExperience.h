// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_LEGACY_METAPORTATION_SCANNER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_LEGACY_METAPORTATION_SCANNER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Thread.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/io/Bitstream.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/network/PackagedUDPClient.h"
#include "ocean/network/PackagedTCPClient.h"

#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Group.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/Text.h"
#include "ocean/rendering/VertexSet.h"

#include "ocean/tracking/mapbuilding/PatchTracker.h"
#include "ocean/tracking/mapbuilding/Unified.h"

#include "ocean/tracking/maptexturing/TextureGenerator.h"

#include <reconstruction/systems/DefaultFusionSystem.h>

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements the Metaportation Scanner experience for phones.
 * @ingroup xrplayground
 */
class LegacyMetaportationScannerExperience :
	public XRPlaygroundExperience,
	protected Thread
{
	friend class LegacyMetaportationExperience;

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
		 * Definition of a class holding a mesh.
		 */
		class MeshObject
		{
			public:

				/**
				 * Creates a new mesh object.
				 * @param vertices The vertices to update
				 * @param textureCoordinates The texture coordinates, one for each vetex
				 * @param triangleFaces The triangle faces
				 * @param textureFrame The texture
				 * @param world_T_mesh The transformation between mesh and world, may be identity
				 */
				MeshObject(Vectors3&& vertices, Vectors2&& textureCoordinates, Rendering::TriangleFaces&& triangleFaces, Frame&& textureFrame, const HomogenousMatrix4& world_T_mesh);

				/**
				 * Creates a new mesh object.
				 * @param vertices The vertices to update
				 * @param perVertexNormals The per-vertex normals of the mesh
				 * @param perVertexColors The per-vertex colors of the mesh
				 * @param triangleFaces The triangle faces
				 * @param world_T_mesh The transformation between mesh and world, may be identity
				 */
				MeshObject(Vectors3&& vertices, Vectors3&& perVertexNormals, RGBAColors&& perVertexColors, Rendering::TriangleFaces&& triangleFaces, const HomogenousMatrix4& world_T_mesh);

				/**
				 * Returns the vertices of this mesh.
				 * @return The mesh's vertices
				 */
				inline const Vectors3& vertices() const;

				/**
				 * Returns the per-vertex normals of this mesh.
				 * @return The mesh's per-vertex normals
				 */
				inline const Vectors3& perVertexNormals() const;

				/**
				 * Returns the per-vertex colors of this mesh.
				 * @return The mesh's per-vertex colors, if any
				 */
				inline const RGBAColors& perVertexColors() const;

				/**
				 * Returns the texture coordinates of this mesh.
				 * @return The mesh's texture coordinates, if any
				 */
				inline const Vectors2& textureCoordinates() const;

				/**
				 * Returns the triangle faces of this mesh.
				 * @return The mesh's triangle faces
				 */
				inline const Rendering::TriangleFaces& triangleFaces() const;

				/**
				 * Returns the texture frame of this mesh.
				 * @return The mesh's texture frame
				 */
				inline const Frame& textureFrame() const;

				/**
				 * Returns the transformation between mesh and world.
				 * @return The transformation between mesh and world, may be identity
				 */
				inline const HomogenousMatrix4& world_T_mesh() const;

			protected:

				/// The vertices of this mesh.
				Vectors3 vertices_;

				/// The per-vertex normals of this mesh.
				Vectors3 perVertexNormals_;

				/// The per-vertex colors of this mesh.
				RGBAColors perVertexColors_;

				/// The texture coordinates of this mesh, if any.
				Vectors2 textureCoordinates_;

				/// The texture frame of this mesh.
				Frame textureFrame_;

				/// The triangle faces of this mesh.
				Rendering::TriangleFaces triangleFaces_;

				/// The transformation between mesh and world, may be identity.
				HomogenousMatrix4 world_T_mesh_ = HomogenousMatrix4(false);
		};

		/**
		 * Definition of an unordered map mapping unique mesh ids to mesh objects.
		 */
		typedef std::unordered_map<Index32, MeshObject> MeshObjectMap;

		/**
		 * Definition of a class holding a mesh.
		 */
		class MeshRenderingObject
		{
			public:

				/**
				 * Creates a new mesh rendering object.
				 * @param engine The rendering engine to be used
				 */
				MeshRenderingObject(const Rendering::Engine& engine);

				/**
				 * Updates the rendering object with a textured mesh.
				 * @param engine The rendering engine to be used
				 * @param vertices The vertices to update
				 * @param textureCoordinates The texture coordinates, one for each vetex
				 * @param triangleFaces The triangle faces
				 * @param textureFrame The texture
				 * @param world_T_mesh The transformation between mesh and world, may be identity
				 */
				void update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors2& textureCoordinates, const Rendering::TriangleFaces& triangleFaces, const Frame& textureFrame, const HomogenousMatrix4& world_T_mesh);

				/**
				 * Updates the rendering object with a non-textured mesh.
				 * @param engine The rendering engine to be used
				 * @param vertices The vertices to update
				 * @param perVertexNormals The texture coordinates, one for each vetex
				 * @param perVertexColors The mesh colors, one for each vertex, empty if unknown
				 * @param triangleFaces The triangle faces
				 * @param world_T_mesh The transformation between mesh and world, may be identity
				 */
				void update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors3& perVertexNormals, const RGBAColors& perVertexColors, const Rendering::TriangleFaces& triangleFaces, const HomogenousMatrix4& world_T_mesh);

				/**
				 * Updates the rendering object with a non-textured mesh.
				 * @param engine The rendering engine to be used
				 * @param vertices The vertices to update
				 * @param perVertexNormals The texture coordinates, one for each vetex
				 * @param perVertexColors The mesh colors, one for each vertex, empty if unknown
				 * @param triangleFaceIndices The indices of the triangle faces
				 * @param world_T_mesh The transformation between mesh and world, may be identity
				 */
				void update(const Rendering::Engine& engine, const Vectors3& vertices, const Vectors3& perVertexNormals, const RGBAColors& perVertexColors, const Indices32& triangleFaceIndices, const HomogenousMatrix4& world_T_mesh);

				/**
				 * Updates the rendering object with a non-textured mesh.
				 * @param engine The rendering engine to be used
				 * @param vertices The vertices to update
				 * @param perVertexColors The mesh colors, one for each vertex, empty if unknown
				 * @param triangleFaces The triangle faces
				 * @param world_T_mesh The transformation between mesh and world, may be identity
				 */
				void update(const Rendering::Engine& engine, const Vectors3& vertices, const RGBAColors& perVertexColors, const Rendering::TriangleFaces& triangleFaces, const HomogenousMatrix4& world_T_mesh);

				/**
				 * Returns the object's Transform node.
				 * @return The Transform node
				 */
				inline Rendering::TransformRef transform() const;

			protected:

				/// The VertexSet object.
				Rendering::VertexSetRef vertexSet_;

				/// The Triangles object.
				Rendering::TrianglesRef triangles_;

				/// The FrameTexture2D object for textured meshes.
				Rendering::FrameTexture2DRef texture_;

				/// The Material object for non-textured meshes.
				Rendering::MaterialRef material_;

				/// The transform object holding the geometry.
				Rendering::TransformRef transform_;
		};

		/**
		 * Definition of an unordered map mapping unique mesh ids to mesh objects.
		 */
		typedef std::unordered_map<Index32, MeshRenderingObject> MeshRenderingObjectMap;

		/**
		 * This class implements a simple worker thread executing a map creator.
		 */
		class MapCreatorWorker : protected Thread
		{
			public:

				/**
				 * Creates a new map creator object based on a 6-DOF tracker and the frame medium connected with the tracker.
				 * @param tracker6DOF The 6-DOF tracker, must be valid
				 * @param frameMedium The frame medium, must be valid
				 */
				MapCreatorWorker(const Devices::Tracker6DOFRef& tracker6DOF, const Media::FrameMediumRef& frameMedium);

				/**
				 * Destructs the object.
				 */
				~MapCreatorWorker() override;

				/**
				 * Returns the map.
				 * @param objectPoints The resulting 3D object points of the map
				 * @param objectPointIds The resulting id of the object points, one for each object point
				 * @param unifiedDescriptorMap The resulting descriptor map
				 * @return True, if succeeded
				 */
				bool latestMap(Vectors3& objectPoints, Indices32& objectPointIds, std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap>& unifiedDescriptorMap) const;

			protected:

				/**
				 * The internal thread function function.
				 */
				void threadRun() override;

			protected:

				/// The tracker
				Devices::Tracker6DOFRef tracker6DOF_;

				/// The frame medium used as input.
				Media::FrameMediumRef frameMedium_;

				/// The internal patch tracker.
				std::shared_ptr<Tracking::MapBuilding::PatchTracker> patchTracker_;

				/// The timestamp of the first frame.
				Timestamp firstFrameTimestamp_;

				/// The object's lock.
				mutable Lock lock_;
		};

		/**
		 * This class implements a generator for meshes with texture.
		 */
		class TexturedMeshGenerator
		{
			protected:

				/**
				 * Definition of individual texture processor states.
				 */
				enum ProcessorState : uint32_t
				{
					/// The processor is currently stopped.
					PS_STOPPED = 0u,
					/// The processor is currently idling.
					PS_IDLE,
					/// The processor is updating the mesh.
					PS_UPDATING_MESH,
					/// The processor has updated the mesh.
					PS_MESH_UPDATED,
					/// The processor is exporting the mesh.
					PS_MESH_EXPORTING,
					/// The processor is processing the current frame.
					PS_PROCESS_FRAME
				};

			public:

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
				 * @param recentSceneElement The recent scene element, can be invalid to skip
				 * @return True, if succeeded
				 */
				bool updateMesh(Devices::SceneTracker6DOF::SharedSceneElement recentSceneElement);

				/**
				 * Processes the current camera frame.
				 * @param frameMedium The frame medium providing the camera data, must be valid
				 * @param recentSceneElementTimestamp The timestamp of the recent scene element, must be valid
				 * @param world_T_recentCamera The transformation between camera and world, must be valid
				 * @param engine The rendering engine to be used
				 */
				void processFrame(const Media::FrameMediumRef& frameMedium, const Timestamp& recentSceneElementTimestamp, const HomogenousMatrix4& world_T_recentCamera, const Rendering::EngineRef& engine);

				/**
				 * Exports the latest mesh.
				 * @param meshObjectMap The resulting map holding the mesh
				 * @return True, if succeeded
				 */
				bool exportMesh(MeshObjectMap& meshObjectMap);

				/**
				 * Returns whether the mesh generator is currently active.
				 * @return True, if succeeded
				 */
				bool isActive();

			protected:

				/// The texture generator.
				Tracking::MapTexturing::TextureGenerator textureGenerator_;

				/// The current processor state.
				ProcessorState processorState_ = PS_STOPPED;

				/// The timestamp of the last frame which has been processed.
				Timestamp lastProcessedFrameTimestamp_;

				/// The generator's lock.
				Lock lock_;
		};

		/**
		 * This class implements a mesh generator for meshes with per-vertex colors.
		 */
		class ColoredMeshGenerator
		{
			public:

				/**
				 * Default constructor.
				 */
				ColoredMeshGenerator();

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
				 * Exports the current mesh.
				 * @param meshObjectMap The resulting map holding the mesh
				 * @return True, if succeeds
				 */
				bool exportMesh(MeshObjectMap& meshObjectMap);

				/**
				 * Retruns whether this generator is currently active.
				 * @return True, if so
				 */
				bool isActive();

			protected:

				/// The VOGON fusion system creating the mesh.
				vogon::DefaultFusionSystem vogonFusionSystem_;

				/// True, if the generator is currently active.
				bool isActive_ = false;
		};

		/**
		 * This class implements an independent thread in which the network communication takes place.
		 */
		class NetworkWorker : protected Thread
		{
			public:

				/**
				 * Creates a new networker object.
				 * @param renderingText The rendering text which can be used for some debug output, must be valid
				 * @param address The address of the receiver, must be valid
				 * @param port The port of the receiver, must be valid
				 */
				NetworkWorker(const Rendering::TextRef& renderingText, const Network::Address4& address, const Network::Port& port);

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
				 * Sets a new feature map to send.
				 * @param objectPoints The 3D object points of the feature map
				 * @param objectPointIds The ids of the 3D object points, one for each 3D object point
				 * @param unifiedDescriptorMap The map mapping object point ids to feature descriptors, must be valid
				 */
				void newMap(Vectors3&& objectPoints, Indices32&& objectPointIds, std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap>&& unifiedDescriptorMap);

				/**
				 * Sets a new mesh to send.
				 * @param meshObjectMap The new mesh to send
				 */
				void newMesh(MeshObjectMap&& meshObjectMap);

			protected:

				/**
				 * The internal thread function function.
				 */
				void threadRun() override;

			protected:

				/// The rendering text which can be used for some debug output.
				Rendering::TextRef renderingText_;

				/// The recent transformation between camera and world.
				HomogenousMatrix4 world_T_recentCamera_ = HomogenousMatrix4(false);

				/// The lock for the camera transformation.
				Lock poseLock_;

				/// The 3D object points for the feature map.
				Vectors3 mapObjectPoints_;

				/// The ids of the object points in the feature map, one for each 3D object point.
				Indices32 mapObjectPointIds_;

				/// The map mapping object point ids to feature descriptors
				std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> mapUnifiedDescriptorMap_;

				/// The lock for the feature map.
				Lock mapLock_;

				/// The mesh.
				MeshObjectMap meshObjectMap_;

				/// The lock for the mesh.
				Lock meshLock_;

				/// The TCP client which will send the data to the receiver.
				Network::PackagedTCPClient tcpClient_;

				/// The UDP client which will send low latency data to the reciever.
				Network::PackagedUDPClient udpClient_;

				/// The address of the target host receiving the stream.
				Network::Address4 address_;

				/// The port of the target host.
				Network::Port port_;
		};

		/// The unique tag for a transformation.
		static constexpr uint64_t transformationTag_ = IO::Tag::string2tag("_OCNHTR_");

		/// The unique tag for a textured mesh.
		static constexpr uint64_t meshTag_ = IO::Tag::string2tag("_OCNMES_");

		/// The unique tag for a map.
		static constexpr uint64_t mapTag_ = IO::Tag::string2tag("_OCNMAP_");

		/// The unique tag for object points.
		static constexpr uint64_t objectPointsTag_ = IO::Tag::string2tag("_OCNOPT_");

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~LegacyMetaportationScannerExperience() override;

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
		 * @param address The address of the target host receiving the stream, must be valid
		 * @param port The port of the target host
		 * @return True, if succeeded
		 */
		bool start(const ScanningMode scanningMode, const Network::Address4& address, const Network::Port& port);

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
		LegacyMetaportationScannerExperience() = default;

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

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		/**
		 * Shows the user interaction on iOS.
		 * This function contains platform specific code for iOS.
		 * @param userInterface The application's user interface
		 */
		void showUserInterinterfaceIOS(const Interaction::UserInterface& userInterface);

		/**
		 * Unloads the user interaction on iOS.
		 * This function contains platform specific code for iOS.
		 * @param userInterface The application's user interface
		 */
		void unloadUserInterinterfaceIOS(const Interaction::UserInterface& userInterface);

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		/**
		 * Writes a 6-DOF transformation to a bitstream.
		 * @param transformation The transformation to write, must be valid
		 * @param bitstream The output stream to which the transformation will be written
		 * @return True, if succeeded
		 */
		static bool writeTransformationToStream(const HomogenousMatrix4& transformation, IO::OutputBitstream& bitstream);

		/**
		 * Writes a mesh to a bitstream.
		 * @param meshId The unique id of the mesh
		 * @param remainingMeshes The number of remaining meshes belonging to this mesh, with range [0, infinity)
		 * @param world_T_mesh The transformation between the mesh and the world
		 * @param vertices The vertices of the mesh
		 * @param perVertexNormals The per-vertex normals of the mesh, one for each vertex, empty if unknown
		 * @param perVertexColors The per-vertex colors of the mesh, one for each vertex, empty if unknown
		 * @param textureCoordinates The per-vertex texture coordinates of the mesh, one for each vertex, empty if unknown
		 * @param triangleFaces The faces of the triangles
		 * @param textureFrame The texture frame, invalid if unknown
		 * @param bitstream The input stream to which the mesh will be written
		 * @return True, if succeeded
		 */
		static bool writeMeshToStream(const Index32 meshId, const unsigned int remainingMeshes, const HomogenousMatrix4& world_T_mesh, const Vectors3& vertices, const Vectors3& perVertexNormals, const RGBAColors& perVertexColors, const Vectors2& textureCoordinates, const Rendering::TriangleFaces& triangleFaces, const Frame& textureFrame, IO::OutputBitstream& bitstream);

		/**
		 * Writes 3D object points to a bitstream.
		 * @param objectPoints The 3D object points
		 * @param objectPointIds The ids of the 3D object points, one for each object point
		 * @param bitstream The input stream to which the points will be written
		 * @return True, if succeeded
		 */
		static bool writeObjectPointsToStream(const Vectors3& objectPoints, const Indices32& objectPointIds, IO::OutputBitstream& bitstream);

		/**
		 * Reads a 6-DOF transformation from a bitstream.
		 * @param bitstream The input stream from which the transformation will be read
		 * @param transformation The resulting transformation
		 * @return True, if succeeded
		 */
		static bool readTransformationFromStream(IO::InputBitstream& bitstream, HomogenousMatrix4& transformation);

		/**
		 * Reads a mesh from a bitstream.
		 * @param bitstream The input stream from which the pose will be read
		 * @param meshId The resulting unique id of the mesh
		 * @param remainingMeshes The number of remaining meshes belonging to this mesh, with range [0, infinity)
		 * @param world_T_mesh The resulting transformation between the mesh and the world
		 * @param vertices The resulting vertices of the mesh
		 * @param perVertexNormals The resulting per-vertex normals of the mesh, one for each vertex, empty if unknown
		 * @param perVertexColors The resulting per-vertex colors of the mesh, one for each vertex, empty if unknown
		 * @param textureCoordinates The resulting per-vertex texture coordinates of the mesh, one for each vertex
		 * @param triangleFaces The resulting faces of the triangles
		 * @param textureFrame The resulting texture frame of the mesh
		 * @return True, if succeeded
		 */
		static bool readMeshFromStream(IO::InputBitstream& bitstream, Index32& meshId, unsigned int& remainingMeshes, HomogenousMatrix4& world_T_mesh, Vectors3& vertices, Vectors3& perVertexNormals, RGBAColors& perVertexColors, Vectors2& textureCoordinates, Rendering::TriangleFaces& triangleFaces, Frame& textureFrame);

		/**
		 * Returns the id of the mesh which in a bitstream.
		 * @param bitstream The input stream in which the mesh id is determined, the position in the stream will not change
		 * @param meshId The resulting unique id of the mesh
		 * @return True, if succeeded
		 */
		static bool determineMeshIdInStream(IO::InputBitstream& bitstream, Index32& meshId);

		/**
		 * Reads 3D object points from a bitstream.
		 * @param bitstream The input stream from which the points will be read
		 * @param objectPoints The resulting 3D object points
		 * @param objectPointIds The resulting ids of the object points, one for each object point
		 * @return True, if succeeded
		 */
		static bool readObjectPointsFromStream(IO::InputBitstream& bitstream, Vectors3& objectPoints, Indices32& objectPointIds);

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

		/// The most recent scene element to process, empty otherwise.
		Devices::SceneTracker6DOF::SharedSceneElement recentSceneElement_;

		/// The recent transformation between camera and world.
		HomogenousMatrix4 world_T_recentCamera_ = HomogenousMatrix4(false);

		/// The map mapping mesh ids to mesh rendering objects.
		MeshRenderingObjectMap meshRenderingObjectMap_;

		/// The map mapping mesh ids to mesh objects used for rendering.
		MeshObjectMap meshObjectMapForRendering_;

		/// The map mapping mesh ids to mesh objects used for sending via network.
		MeshObjectMap meshObjectMapForSending_;

		/// The lock for the map with mesh rendering objects.
		Lock meshRenderingObjectMapLock_;

		/// The lock for the map with mesh objects.
		Lock meshObjectMapLock_;

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

		/// The address of the target host receiving the stream.
		Network::Address4 address_;

		/// The port of the target host.
		Network::Port port_;

		/// The mesh generator using a texture.
		std::shared_ptr<TexturedMeshGenerator> texturedMeshGenerator_;

		/// The mesh generator using per-vertex colors.
		std::shared_ptr<ColoredMeshGenerator> coloredMeshGenerator_;

		/// The scanning mode to be used.
		ScanningMode scanningMode_ = SM_INVALID;

		/// The experience's lock.
		Lock lock_;
};

inline const Vectors3& LegacyMetaportationScannerExperience::MeshObject::vertices() const
{
	return vertices_;
}

inline const Vectors3& LegacyMetaportationScannerExperience::MeshObject::perVertexNormals() const
{
	return perVertexNormals_;
}

inline const RGBAColors& LegacyMetaportationScannerExperience::MeshObject::perVertexColors() const
{
	return perVertexColors_;
}

inline const Vectors2& LegacyMetaportationScannerExperience::MeshObject::textureCoordinates() const
{
	return textureCoordinates_;
}

inline const Rendering::TriangleFaces& LegacyMetaportationScannerExperience::MeshObject::triangleFaces() const
{
	return triangleFaces_;
}

inline const Frame& LegacyMetaportationScannerExperience::MeshObject::textureFrame() const
{
	return textureFrame_;
}

inline const HomogenousMatrix4& LegacyMetaportationScannerExperience::MeshObject::world_T_mesh() const
{
	return world_T_mesh_;
}

inline Rendering::TransformRef LegacyMetaportationScannerExperience::MeshRenderingObject::transform() const
{
	return transform_;
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_LEGACY_METAPORTATION_SCANNER_EXPERIENCE_H
