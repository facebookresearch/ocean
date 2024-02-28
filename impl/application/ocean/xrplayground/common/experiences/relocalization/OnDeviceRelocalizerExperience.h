// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ON_DEVICE_RELOCALIZER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ON_DEVICE_RELOCALIZER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/Tracker6DOF.h"

#include "ocean/interaction/experiences/AnchoredContentManager.h"

#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements an experience based on On-Device relocalizer against a map located on device.
 * @ingroup xrplayground
 */
class OnDeviceRelocalizerExperience : public XRPlaygroundExperience
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
    	~OnDeviceRelocalizerExperience() override;

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
		 * Starts the relocalizer for a given map.
		 * @param filename The filename of the map
		 * @return True, if succeeded
		 */
		bool start(const std::string& filename);

		/**
		 * Stops the relocalizer.
		 * @return True, if succeeded
		 */
		bool stop();

		/**
		 * Creates a new WorldLayerCircleExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

    protected:

		/**
		 * Protected default constructor.
		 */
		OnDeviceRelocalizerExperience();

		/**
		 * Event function informing that the content manger has removed several content objects.
		 * @param removedObjects The object which have been removed in the content manager (e.g., because the left the engagement radius)
		 */
		void onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects);

		/**
		 * Event function for new tracking samples from the world tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from world tracker
		 */
		void onWorldTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample);

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

	protected:

		/// The manager for anchored content, will take care of visibility and poses of the virtual content.
		AnchoredContentManager anchoredContentManager_;

		/// The 6-DOF tracker doing the relocalization.
		Devices::Tracker6DOFRef relocalizerTracker6DOF_;

		/// The subscription id for samples events from the world tracker.
		Devices::Measurement::SampleEventSubscription worldTrackerSampleSubscribe_;

		/// The group object holding the feature point cloud to be rendered.
		Rendering::GroupRef renderingGroup_;

		/// The 3D locations of all map features, if known or updated.
		Vectors3 objectPoints_;

		/// The lock for the object points.
		Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_ON_DEVICE_RELOCALIZER_EXPERIENCE_H
