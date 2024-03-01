// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ENVIRONMENT_RENDERER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ENVIRONMENT_RENDERER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/rendering/Text.h"

#include "ocean/math/SampleMap.h"

#include "metaonly/ocean/network/verts/Driver.h"

#include "ocean/tracking/SmoothedTransformation.h"

#include "ocean/tracking/mapbuilding/Unified.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to render an environment which is scanned and received from a different device.
 * The experience receives the environment from the EnvironmentScanner experience.
 * @see EnvironmentScannerExperience.
 * @ingroup xrplayground
 */
class EnvironmentRendererExperience :
	public XRPlaygroundExperience,
	protected Thread
{
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

				/// The per-vertex normals of the mesh.
				Vectors3 perVertexNormals_;

				/// The vertex indices of the triangles, 3 consecutive indices for each triangle.
				Indices32 triangleIndices_;

				/// The Transform node holding the mesh.
				Rendering::TransformRef renderingTransform_;

				/// True, if the mesh has changed and needs to be forwarded to the rendering engine.
				bool hasChanged_ = false;
		};

		/**
		 * Definition of an unordered map mapping unique mesh ids to mesh objects.
		 */
		typedef std::unordered_map<Index32, MeshObject> MeshObjectMap;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~EnvironmentRendererExperience() override;

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
		 * Creates a new EnvironmentRendererExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		EnvironmentRendererExperience() = default;

		/**
		 * The event function for new entities.
		 * @param driver The driver sending the event
		 * @param entity The new entity
		 */
		void onNewEntity(Network::Verts::Driver& driver, const Network::Verts::SharedEntity& entity);

		/**
		 * The event function for changed users.
		 * @param driver The driver sending the event
		 * @param addedUsers The ids of all added users
		 * @param removedUsers The ids of all removed users
		 */
		void onChangedUsers(Network::Verts::Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers);

		/**
		 * Event function for received data on a channel.
		 * @param driver The VERTS driver sending the event
		 * @param sessionId The session id of the sender, will be valid
		 * @param userId The user id of the sender, will be valid
		 * @param identifier The identifier of the container, will be valid
		 * @param version The version of the container, with range [0, infinity)
		 * @param buffer The buffer of the container
		 */
		void onReceiveContainer(Network::Verts::Driver& driver, const uint32_t sessionId, const uint64_t userId, const std::string& identifier, const unsigned int version, const Network::Verts::Driver::SharedBuffer& buffer);

		/**
		 * The thread's run function.
		 */
		void threadRun() override;

	protected:

		/// The rendering transform node holding the rendering content.
		Rendering::TransformRef renderingTransform_;

		/// The rendering Text object holding the instruction text.
		Rendering::TextRef renderingText_;

		/// The rendering Transform object for the scanning device location.
		Rendering::TransformRef renderingTransformDevice_;

		/// The most recent camera pose, invalid otherwise.
		HomogenousMatrix4 world_T_recentCamera_ = HomogenousMatrix4(false);

		/// The map mapping unique mesh ids to mesh objects.
		MeshObjectMap meshObjectMap_;

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

		/// True, to render a mesh; False, to render a point cloud.
		bool renderMesh_ = true;

		/// Experience's lock.
		Lock lock_;

		/// The VERTS driver for the network communication.
		Network::Verts::SharedDriver vertsDriver_;

		/// The VERTS node containing the camera pose.
		Network::Verts::SharedNode vertsDevicePoseNode_;

		/// The subscription object for new entity events.
		Network::Verts::Driver::NewEntityScopedSubscription newEntityScopedSubscription_;

		/// The subscription object for changed users events.
		Network::Verts::Driver::ChangedUsersScopedSubscription changedUsersScopedSubscription_;

		/// The subscription object for data receive events.
		Network::Verts::Driver::ReceiveContainerScopedSubscription receiveContainerScopedSubscription_;

		/// The ids of all active users.
		UnorderedIndexSet64 userIds_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ENVIRONMENT_RENDERER_EXPERIENCE_H
