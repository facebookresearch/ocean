// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_PLANE_TRACKER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_PLANE_TRACKER_EXPERIENCE_H

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
 * This class implements an experience for a plane tracker.
 * @ingroup xrplayground
 */
class PlaneTrackerExperience : public XRPlaygroundExperience
{
	protected:

		/**
		 * Re-definition of AnchoredContentManager.
		 */
		typedef Interaction::Experiences::AnchoredContentManager AnchoredContentManager;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~PlaneTrackerExperience() override;

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
		PlaneTrackerExperience() = default;

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

		/// The plane tracker providing access to be used in this experience.
		Devices::SceneTracker6DOFRef planeTracker6DOF_;

		/// The object id of the plane tracker.
		Devices::SceneTracker6DOF::ObjectId objectId_ = Devices::SceneTracker6DOF::invalidObjectId();

		/// The subscription object for sample events from the scene tracker.
		Devices::Measurement::SampleEventSubscription sceneTrackerSampleEventSubscription_;

		/// The rendering group node holding the rendering content.
		Rendering::GroupRef renderingGroup_;

		/// The most recent planes to process, empty otherwise.
		Devices::SceneTracker6DOF::SharedSceneElement recentSceneElement_;

		/// The experience's lock.
		Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_PLANE_TRACKER_EXPERIENCE_H
