// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_XR_PLAYGROUND_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_XR_PLAYGROUND_APPLICATION_H

#include "application/ocean/xrplayground/oculus/XRPlaygroundOculus.h"
#include "application/ocean/xrplayground/oculus/ExperiencesMenu.h"

#include "ocean/base/Singleton.h"

#include "ocean/platform/meta/avatars/Manager.h"

#include "ocean/platform/meta/quest/vrapi/HandPoses.h"

#include "ocean/platform/meta/quest/vrapi/application/MRPassthroughVisualizer.h"
#include "ocean/platform/meta/quest/vrapi/application/VRControllerVisualizer.h"
#include "ocean/platform/meta/quest/vrapi/application/VRHandVisualizer.h"
#include "ocean/platform/meta/quest/vrapi/application/VRNativeApplication.h"

#include "ocean/rendering/Scene.h"

#include "metaonly/ocean/platform/meta/Login.h"

namespace Ocean
{

namespace XRPlayground
{

namespace Oculus
{

/**
 * This class implements the main view of the XRPlayground viewer for Oculus platforms.
 * @ingroup xrplaygroundoculus
 */
class XRPlaygroundApplication : public Platform::Meta::Quest::VrApi::Application::VRNativeApplication
{
	protected:

		/**
		 * Definition of a singleton holding the launch experience.
		 */
		class LaunchExperienceHolder : public Singleton<LaunchExperienceHolder>
		{
			friend class Singleton<LaunchExperienceHolder>;

			public:

				/**
				 * Sets the launch experience.
				 * @param experience The name of the experience to start when launching
				 * @return True, if succeeded
				 */
				bool setExperience(std::string&& experience);

				/**
				 * Returns the launch experience.
				 * @return Experience to be started when launching, empty if not set
				 */
				std::string experience() const;

			protected:

				/// The name of the experience to start when launching.
				std::string experience_;

				/// The holder's lock.
				mutable Lock lock_;
		};

	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit XRPlaygroundApplication(struct android_app* androidApp);

		/**
		 * Destructs this object.
		 */
		~XRPlaygroundApplication() override;

		/**
		 * Sets the launch experience.
		 * @param experience The name of the experience to start when launching
		 * @return True, if succeeded
		 */
		static bool setLaunchExperience(std::string&& experience);

	protected:

		/**
		 * Returns the set of permissions that are required by the app and that have to be requested from the operating system.
		 * @see NativeApplication::androidPermissionsToRequest().
		 */
		AndroidPermissionsSet androidPermissionsToRequest() override;

		/**
		 * Event function which is called once the app is ready to use.
		 * @param validInitialization True, if the initialization succeeded; False, if parts of the initialization failed
		 */
		void onReadyToUse(const bool validInitialization);

		/**
		 * Event function called after the framebuffer has been initialized.
		 * @see VRApplication::onFramebufferInitialized().
		 */
		void onFramebufferInitialized() override;

		/**
		 * Event function called before the framebuffer will be released.
		 * @see VRApplication::onFramebufferReleasing().
		 */
		void onFramebufferReleasing() override;

		/**
		 * Events function called when the VR mode has been entered.
		 * @see VRNativeApplication::onVrModeEntered().
		 */
		void onVrModeEntered() override;

		/**
		 * Events function called when the VR mode has been left.
		 * @see VRNativeApplication::onVrModeLeft().
		 */
		void onVrModeLeft() override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRApplication::onPreRender().
		 */
		void onPreRender(const Timestamp& renderTimestamp) override;

		/**
		 * Event functions for pressed buttons (e.g., from a tracked remote device).
		 * @see VRNativeApplication::onButtonPressed().
		 */
		void onButtonPressed(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp) override;

		/**
		 * Event functions for released buttons (e.g., from a tracked remote device).
		 * @see VRNativeApplication::onButtonReleased().
		 */
		void onButtonReleased(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp) override;

		/**
		 * Event function call when the Android Activity is started.
		 * @see NativeApplication::onActivityStart().
		 */
		void onActivityStart() override;

		/**
		 * The event function which is called when all resources should finally be released.
		 * @see NativeApplication::onReleaseResources().
		 */
		void onReleaseResources() override;

		/**
		 * Translates a login type to a user type.
		 * @param loginType The login type to translate
		 * @return The translated user type
		 */
		static Platform::Meta::Avatars::Manager::UserType translateLoginType(const Platform::Meta::Login::LoginType loginType);

	protected:

		/// Rendering Scene object holding additional information.
		Rendering::SceneRef scene_;

		/// The timestamp when the application was initialized.
		Timestamp initializationTimestamp_ = Timestamp(false);

		/// True, if the app has access to the assets.
		bool hasAssetsAccess_ = false;

		/// True, if Avatars is initialized
		bool avatarsInitialized_ = false;

		/// True, if the app is ready to use; False, if the app is still initializing.
		bool readyToUse_ = false;

		/// The unique id of the user.
		uint64_t userId_ = 0ull;

		/// The access token of the user.
		std::string accessToken_;

		/// The visualizer for controllers.
		Platform::Meta::Quest::VrApi::Application::VRControllerVisualizer vrControllerVisualizer_;

		/// The visualizer for hands.
		Platform::Meta::Quest::VrApi::Application::VRHandVisualizer vrHandVisualizer_;

		/// The accessor for most recent hand poses.
		Platform::Meta::Quest::VrApi::HandPoses handPoses_;

		/// The menu showing all experiences.
		ExperiencesMenu experiencesMenu_;

		/// The timestamp when the button Y was pressed, invalid if currently not pressed.
		Timestamp timestampPressedButtonY_;

		/// The timestamp when the button B was pressed, invalid if currently not pressed.
		Timestamp timestampPressedButtonB_;

		/// The visualizer for the MR passthrough feature.
		Platform::Meta::Quest::VrApi::Application::MRPassthroughVisualizer mrPassthroughVisualizer_;
};

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_XR_PLAYGROUND_APPLICATION_H
