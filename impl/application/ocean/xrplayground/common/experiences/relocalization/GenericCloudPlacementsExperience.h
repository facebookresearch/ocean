// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_GENERIC_CLOUD_PLACEMENTS_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_GENERIC_CLOUD_PLACEMENTS_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Text.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements an experience based on Coud Relocalization with Placements.
 * The experience creates 3D coordinate systems at all placement locations (e.g., spatial anchors) as received though cloud reloc.<br>
 * Thus, the experience is generic for all cloud reloc locations providing placements.
 * @ingroup xrplayground
 */
class GenericCloudPlacementsExperience : public XRPlaygroundExperience
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
		~GenericCloudPlacementsExperience() override;

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
		 * Starts the relocalization.
		 * @param jsonConfiguration The JSON configuration to be used for relocalization
		 */
		void startRelocalization(const std::string& jsonConfiguration);

		/**
		 * Creates a new GravityAndHeadingNativeExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		GenericCloudPlacementsExperience() = default;

		/**
		 * Event function for lost or found objects from the cloud reloc tracker.
		 * @param sender The tracker sending the event, will be valid
		 * @param found True, if the provided object ids belong to objects which have been found; False, if the ids belong to objects which have been lost
		 * @param objectIds The ids of all objects associated with the event
		 * @param timestamp The event timestamp
		 */
		void onTrackerObjectEvent(const Devices::Tracker* sender, const bool found, const Devices::Measurement::ObjectIdSet& objectIds, const Timestamp& timestamp);

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		/**
		 * Shows the user interaction on iOS.
		 * This function contains platform specific code for iOS.
		 * @param userInterface The application's user interface
		 * @param configurations The configurations the user can select
		 */
		void showConfigurationsIOS(const Interaction::UserInterface& userInterface, const std::vector<std::pair<std::string, std::string>>& configurations);

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		/**
		 * Returns the list of selectable configurations.
		 * @return The list of configuration pairs; first entry readable entry, second entry the actual configuration
		 */
		static std::vector<std::pair<std::string, std::string>> selectableConfigurations();

	protected:

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The scene tracker providing access to cloud relocalization anchors and corresponding 2D/3D feature correspondences.
		Devices::SceneTracker6DOFRef cloudRelocalizerTracker6DOF_;

		/// The event subscription for object events from the cloud tracker.
		Devices::Tracker::TrackerObjectEventSubscription trackerObjectEventSubscription_;

		/// The main object id of the cloud anchor.
		Devices::Tracker::ObjectId mainAnchorObjectId_ = Devices::Tracker::invalidObjectId();

		/// The rendering absolute transform allowing to show a selection menu.
		Rendering::AbsoluteTransformRef renderingMenuTransform_;

		/// The text node for the current state.
		Rendering::TextRef renderingTextState_;

		/// The coordinate system which will be rendered at each placement.
		Rendering::TransformRef renderingCoordinateSystem_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_GENERIC_CLOUD_PLACEMENTS_EXPERIENCE_H
