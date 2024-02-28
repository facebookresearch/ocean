// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_SCENE_TRACKER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_SCENE_TRACKER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements an experience for a scene tracker.
 * @ingroup xrplayground
 */
class SceneTrackerExperience : public XRPlaygroundExperience
{
	protected:

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

		/**
		 * Definition of a map mapping object point ids to object points.
		 */
		typedef std::unordered_map<Index64, Vector3> ObjectPointMap;

		/**
		 * Definition of a pair combining 3D object points with their object point ids.
		 */
		typedef std::pair<Vectors3, Indices64> ObjectPointPair;

		/**
		 * Definition of a vector holding pairs of 3D object points and their ids.
		 */
		typedef std::vector<ObjectPointPair> ObjectPointPairs;

		/**
		 * Definition of a class holding a mesh.
		 */
		class MeshRenderingObject
		{
			public:

				/// The Transform node holding the mesh.
				Rendering::TransformRef renderingTransform_;

				/// The number of vertices this mesh holds.
				size_t numberVertices_ = 0;

				/// The color of the mesh.
				RGBAColor color_;

				/// The timestamp the mesh has been updated the last time.
				Timestamp lastUpdateTimestamp_ = Timestamp(false);
		};

		/**
		 * Definition of an unordered map mapping unique mesh ids to mesh objects.
		 */
		typedef std::unordered_map<Index32, MeshRenderingObject> MeshRenderingObjectMap;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~SceneTrackerExperience() override;

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
		 * Creates a new GravityAndHeadingNativeExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		SceneTrackerExperience() = default;

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
		Devices::SceneTracker6DOF::SharedSceneElements recentSceneElements_;

		/// The map mapping the ids of all object points to their object points.
		ObjectPointMap objectPointMap_;

		/// The map mapping mesh ids to mesh objects.
		MeshRenderingObjectMap meshRenderingObjectMap_;

		/// The round robin mesh index to be considered during the next update iteration.
		size_t roundRobinMeshUpdateIndex = 0;

		/// The timestamp when the last rendering object was updated.
		Timestamp renderingObjectTimestamp_;

		/// The experience's lock.
		Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_SCENE_TRACKER_EXPERIENCE_H
