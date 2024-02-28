// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_OCEANTRACKERDEMOS_FLOOR_TRACKER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_OCEANTRACKERDEMOS_FLOOR_TRACKER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/Tracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements an basic experience showing how to anchor/place objects on the floor plane.
 * @ingroup xrplayground
 */
class FloorTrackerExperience : public XRPlaygroundExperience
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
		~FloorTrackerExperience() override;

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
		FloorTrackerExperience() = default;

        /**
		 * Event function for new tracking samples from the floor tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from floor tracker
		 */
		void onTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample);

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

	protected:

		/// The platform independent 6-DOF floor tracker.
		Devices::Tracker6DOFRef floorTracker6DOF_;

		/// The rendering Transform object placed at floor level.
		Rendering::TransformRef renderingTransform_;

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

        /// The subscription object for sample events from the floor tracker.
		Devices::Measurement::SampleEventSubscription floorTrackerSampleEventSubscription_;

#else

        /// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

#endif
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_OCEANTRACKERDEMOS_FLOOR_TRACKER_EXPERIENCE_H
