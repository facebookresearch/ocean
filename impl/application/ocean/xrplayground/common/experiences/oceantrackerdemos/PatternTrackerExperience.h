// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_PATTERN_TRACKER_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_PATTERN_TRACKER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/media/FrameMedium.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/vrapi/application/VRTableMenu.h"
#endif

#include "ocean/rendering/Box.h"
#include "ocean/rendering/Text.h"

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

/**
 * This experience demonstrates the Pattern Tracker on VR headsets.
 * @ingroup xrplayground
 */
class PatternTrackerExperience :
	public XRPlaygroundExperience,
	protected Thread
{
	protected:

		/// Definition of a mapping from a human-readable description to a frame medium URL
		typedef std::map<std::string, std::string> FrameMediumUrlMap;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~PatternTrackerExperience() override;

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
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * The thread run function.
		 */
		void threadRun() override;

		/**
		 * Event callback function for handled content.
		 * @param content The content which has been handled
		 * @param succeeded True, if the content could be handled; False, if the content could not be handled
		 * @param scenes The rendering scenes that have been created when handling the content
		 */
		void onContentHandled(const std::string& content, const bool succeeded, const std::vector<Rendering::SceneRef>& scenes);

		/**
		 * Creates and displays a menu with available frame media for the current platform.
		 * @param timestamp The timestamp that is used to determine the head pose so that the menu can be in front of the user
		 */
		void showFrameMediumMenu(const Timestamp& timestamp);

		/**
		 * Returns a frame medium URL that has been selected from the menu.
		 * @param frameMediumUrl The URL of the frame medium type that has been selected from the menu.
		 * @param timestamp The time stamp for which the menu should be redrawn, must be valid
		 * @return True if an element from the menu has been selected, otherwise false.
		 */
		bool madeSelectionFromFrameMediumMenu(std::string& frameMediumUrl, const Timestamp& timestamp);

		/**
		 * Activates a new camera frame type.
		 * Any previously activated stream will be disabled.
		 * @param frameMediumUrl The frame medium be activated, must be valid and supported by the current platform.
		 * @return True if the activation was successful, otherwise false.
		 */
		bool activateFrameMedium(const std::string& frameMediumUrl);

		/**
		 * Returns a list of available camera frame type for a device type.
		 * @return A map from a human-readable description to a frame medium URL
		 */
		static FrameMediumUrlMap availableFrameMediums();

	protected:

		/// An additional rendering scene for the dinosaur.
		Rendering::SceneRef additionalScene_;

		/// The frame medium object of the camera that will be displayed.
		Media::FrameMediumRef frameMedium_;

		/// The lock that is used to allow thread-safe updates to the frame medium object.
		Lock frameMediumLock_;

		/// The box transformations that will be used as an overlay for the pattern displayed in Passthrough.
		Rendering::TransformRefs boxTransforms_;

		/// The boxes that will be used as an overlay for the pattern displayed in Passthrough and of which the sizes need to be adjusted to sizes of the pattern they are representing.
		std::vector<Rendering::BoxRef> boxes_;

		/// The material of the box
		std::vector<Rendering::MaterialRef> boxMaterials_;

		/// The lock for the detection results.
		Lock resultLock_;

		/// Indicates if new results are available for displaying.
		bool haveResults_ = false;

		/// The transformation from the coordinate space of the pattern to the camera.
		HomogenousMatrix4 camera_T_pattern_;

		/// The physical size of the pattern in the world (in meters).
		Vector2 patternDimension_;

		/// The cameras that belong to the frames above.
		SharedAnyCamera anyCamera_;

		/// The poses of the device cameras relative to the device origin.
		HomogenousMatrix4 device_T_camera_;

		/// The 6DOF pose if the device origin in world coordinates.
		HomogenousMatrix4 world_T_device_;

		/// The timestamp from when the code were detected.
		Timestamp detectionTimestamp_;

		/// Average detection time (in milliseconds) for all processed frames together.
		double averageDetectionTimeMs_ = -1.0;

		/// The table menu to select the camera frame type.
		Platform::Meta::Quest::VrApi::Application::VRTableMenu frameMediumMenu_;
};

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

class PatternTrackerExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		inline static std::unique_ptr<XRPlaygroundExperience> createExperience();
};

inline std::unique_ptr<XRPlaygroundExperience> PatternTrackerExperience::createExperience()
{
	ocean_assert(false && "This function is not meant to be called!");
	return nullptr;
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_PATTERN_TRACKER_EXPERIENCE_H
