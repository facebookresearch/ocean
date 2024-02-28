// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_USER_INTERFACE_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_USER_INTERFACE_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/rendering/AbsoluteTransform.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID
	#include <jni.h>
#endif

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to realize a simple platform specific user interface.
 * @ingroup xrplayground
 */
class UserInterfaceExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~UserInterfaceExperience() override;

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
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

		/**
		 * Event function when a new selection has been made.
		 * @param selection The new selection which has been made
		 */
		static void onSelection(const std::string& selection);

	protected:

		/**
		 * Protected default constructor.
		 */
		UserInterfaceExperience() = default;

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		/**
		 * Shows the menu on iOS.
		 * This function contains platform specific code for iOS.
		 * @param userInterface The application's user interface
		 */
		void showMenuIOS(const Interaction::UserInterface& userInterface);

#endif

		/**
		 * Returns an instance to the user interface experience.
		 * This function is just a helper and workaround to access the instance from a static function.<br>
		 * Use with case, do not free the instance, no multi-threading handling.
		 * @return Pointer to the user interface experience, can be modified
		 */
		static UserInterfaceExperience*& instance();

	protected:

		/// The timestamp when the menu will be shown.
		Timestamp menuShowTimestamp_;

		/// The absolute transformation which is part of the experience's scene.
		Rendering::AbsoluteTransformRef absoluteTransform_;

		/// The selection which has been made; empty if no selection has been made yet.
		std::string recentSelection_;

		/// Lock for the recent selection.
		Lock lock_;
};

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID

/**
 * Java native interface function for selection events.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI object from which this function called, must be valid
 * @param selection The selection which has been made
 * @ingroup xrplayground
 */
extern "C" void Java_com_facebook_ocean_app_xrplayground_android_UserInterfaceExperienceActivity_onSelection(JNIEnv* env, jobject javaThis, jstring selection);

#endif // XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_USER_INTERFACE_EXPERIENCE_H
