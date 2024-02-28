// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_GENERIC_CLOUD_RELOCALIZER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_GENERIC_CLOUD_RELOCALIZER_EXPERIENCE_H

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
 * This class implements an experience based on Coud Relocalization.
 * The experience creates the 3D content to render on the fly based on the very first successful cloud relocalization response.<br>
 * Thus, the experience is generic for all cloud reloc locations.
 * @ingroup xrplayground
 */
class GenericCloudRelocalizationExperience : public XRPlaygroundExperience
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
		~GenericCloudRelocalizationExperience() override;

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
		GenericCloudRelocalizationExperience() = default;

		/**
		 * Creates the and anchors the mesh based on the relocalization result.
		 * @param engine The rendering engine to be used, must be valid
		 * @param objectPoints The object points of the relocalization result, at least one
		 * @param imagePoints The image points of the relocalization result, at least one
		 * @return True, if succeeded
		 */
		bool createAndAnchorMesh(const Rendering::EngineRef& engine, const Vectors3& objectPoints, const Vectors2& imagePoints);

		/**
		 * Event function for new tracking samples from the cloud reloc tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from world tracker
		 */
		void onCloudRelocTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample);

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
		void showReleaseEnvironmentsIOS(const Interaction::UserInterface& userInterface, const std::vector<std::pair<std::string, std::string>>& configurations);

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

		/// The subscription for samples events from the cloud reloc tracker.
		Devices::Measurement::SampleEventSubscription cloudRelocTrackerSampleSubscription_;

		/// The object id of the cloud anchor.
		Devices::Tracker::ObjectId anchorObjectId_ = Devices::Tracker::invalidObjectId();

		/// The rendering absolute transform allowing to show a selection menu.
		Rendering::AbsoluteTransformRef renderingMenuTransform_;

		/// The text node for the current state.
		Rendering::TextRef renderingTextState_;

		/// True, if the visualization of the cloud reloc pose has been created already.
		bool cloudRelocalizationVisualizationCreated_ = false;

		/// The 3D object points used during the first relocalization.
		Vectors3 firstRelocalizationObjectPoints_;

		/// The 2D image points used during the first relocalization.
		Vectors2 firstRelocalizationImagePoints_;

		/// The delay between the very first relocalization invocation and the first successful relocalization reponse, in seconds.
		double delayUntilFirstSuccessfulRelocalization_ = -1.0;

		/// The timestamp of the last successful relocalization.
		Timestamp lastSuccessfulRelocalizationTimestamp_;

		/// The timestamp of the last failed relocalization.
		Timestamp lastFailedRelocalizationTimestamp_;

		/// The scale of the transformation between anchor and local world.
		float lastScale_ = -1.0f;

		/// The timestamp of the last arrived sampled.
		Timestamp latestSampleTimestamp_;

		/// The number of features currently used for frame-to-frame tracking.
		int numberFeaturesFrameToFrameTracking_ = -1;

		/// The experience's lock.
		Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_GENERIC_CLOUD_RELOCALIZER_EXPERIENCE_H
