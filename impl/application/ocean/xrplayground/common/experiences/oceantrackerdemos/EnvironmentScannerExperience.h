// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ENVIRONMENT_SCANNER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ENVIRONMENT_SCANNER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/io/Bitstream.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/network/verts/Driver.h"

#include "ocean/rendering/Group.h"
#include "ocean/rendering/Text.h"

#include "ocean/tracking/mapbuilding/PatchTracker.h"
#include "ocean/tracking/mapbuilding/Unified.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to scan the environment and how to stream this information to another device.
 * The experience sends the scanned data to the EnviornmentRenderer experience.
 * @see EnvironmentRendererExperience.
 * @ingroup xrplayground
 */
class EnvironmentScannerExperience :
	public XRPlaygroundExperience,
	protected Thread
{
	friend class EnvironmentRendererExperience;

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
				 * Updates the new mesh based on a given mesh from a scene element.
				 * @param mesh The scene element's mesh to be used to update this object
				 * @param currentTimestamp The current timestamp, must be valid
				 * @return True, if succeeded
				 */
				bool update(const SceneElementMeshes::SharedMesh& mesh, const Timestamp& currentTimestamp);

			public:

				/// The timestamp when the mesh has been updated the last time.
				Timestamp lastUpdateTimestamp_ = Timestamp(false);

				/// The timestamp the rendering mesh has been updated the last time.
				Timestamp lastReneringTimestamp_ = Timestamp(false);

				/// The timestamp the mesh has been sent the last time.
				Timestamp lastSentTimestamp_ = Timestamp(false);

				/// The scene elements representing the mesh.
				SceneElementMeshes::SharedMesh mesh_;

				/// The Transform node rendering the mesh.
				Rendering::TransformRef renderingTransform_;

				/// The color of the mesh.
				RGBAColor color_;
		};

		/**
		 * Definition of an unordered map mapping unique mesh ids to mesh objects.
		 */
		typedef std::unordered_map<Index32, MeshObject> MeshObjectMap;

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

				/// The object's lock.
				mutable Lock lock_;
		};

		/// The unique tag for a transformation.
		static constexpr uint64_t transformationTag_ = IO::Tag::string2tag("_OCNHTR_");

		/// The unique tag for a mesh.
		static constexpr uint64_t meshTag_ = IO::Tag::string2tag("_OCNMSH_");

		/// The unique tag for a map.
		static constexpr uint64_t mapTag_ = IO::Tag::string2tag("_OCNMAP_");

		/// The unique tag for object points.
		static constexpr uint64_t objectPointsTag_ = IO::Tag::string2tag("_OCNOPT_");

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~EnvironmentScannerExperience() override;

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
		 * @return True, if succeeded
		 */
		bool startScanning();

		/**
		 * Stops the scanning and streaming session.
		 * @return True, if succeeded
		 */
		bool stopScanning();

		/**
		 * Creates a new EnvironmentScannerExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		EnvironmentScannerExperience() = default;

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
		 * The event function for changed users.
		 * @param driver The driver sending the event
		 * @param addedUsers The ids of all added users
		 * @param removedUsers The ids of all removed users
		 */
		void onChangedUsers(Network::Verts::Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers);

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
		 * @param world_T_mesh The transformation between the mesh and the world
		 * @param vertices The vertices of the mesh
		 * @param perVertexNormals The per-vertex normals of the mesh, one for each vertex
		 * @param triangleIndices The indices of the triangles, 3 consecutive indices for one triangle, must be a multiple of 3
		 * @param bitstream The input stream to which the mesh will be written
		 * @return True, if succeeded
		 */
		static bool writeMeshToStream(const Index32 meshId, const HomogenousMatrix4& world_T_mesh, const Vectors3& vertices, const Vectors3& perVertexNormals, const Indices32& triangleIndices, IO::OutputBitstream& bitstream);

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
		 * @param world_T_mesh The resulting transformation between the mesh and the world
		 * @param vertices The resulting vertices of the mesh
		 * @param perVertexNormals The resulting per-vertex normals of the mesh, one for each vertex
		 * @param triangleIndices The resulting indices of the triangles, 3 consecutive indices for one triangle, must be a multiple of 3
		 * @return True, if succeeded
		 */
		static bool readMeshFromStream(IO::InputBitstream& bitstream, Index32& meshId, HomogenousMatrix4& world_T_mesh, Vectors3& vertices, Vectors3& perVertexNormals, Indices32& triangleIndices);

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

		/// The most recent camera pose, invalid otherwise.
		HomogenousMatrix4 world_T_recentCamera_ = HomogenousMatrix4(false);

		/// The most recent scene element to process, empty otherwise.
		Devices::SceneTracker6DOF::SharedSceneElements recentSceneElements_;

		/// The map mapping mesh ids to mesh objects.
		MeshObjectMap meshObjectMap_;

		/// The round robin mesh index to be considered during the next update iteration.
		size_t roundRobinMeshUpdateIndex = 0;

		/// The timestamp when the last rendering object was updated.
		Timestamp renderingObjectTimestamp_;

		/// The rendering Text node providing user feedback.
		Rendering::TextRef renderingText_;

		/// The frame medium which will be used as source for the camera stream.
		Media::FrameMediumRef frameMedium_;

		/// The experience's lock.
		Lock lock_;

		/// The VERTS driver for the network communication.
		Network::Verts::SharedDriver vertsDriver_;

		/// The subscription object for changed users events.
		Network::Verts::Driver::ChangedUsersScopedSubscription changedUsersScopedSubscription_;

		/// The VERTS node for camera poses.
		Network::Verts::SharedNode vertsDevicePoseNode_;

		/// The lastest buffer holding the map to be sent.
		std::vector<uint8_t> mapBuffer_;

		/// The latest buffer holding the meshes to be sent.
		std::vector<uint8_t> meshesBuffer_;

		/// The ids of all active users.
		UnorderedIndexSet64 userIds_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ENVIRONMENT_SCANNER_EXPERIENCE_H
