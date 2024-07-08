/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_NATIVE_APPLICATION_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_NATIVE_APPLICATION_H

#include "ocean/platform/meta/quest/openxr/application/Application.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/platform/meta/quest/Device.h"

#include "ocean/platform/openxr/Instance.h"

#include <openxr/openxr.h>

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#include <android/native_window_jni.h>
	#include <android_native_app_glue.h>
	#include <android/window.h>
#endif

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

namespace Application
{

/**
 * This class implements a basic OpenXR application for Quest devices based on an Android NativeActivity.
 * @ingroup platformmetaquestopenxrapplication
 */
class OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_EXPORT NativeApplication
{
	protected:

		/**
		 * Definition of an unordered map mapping from Android permissions to a counter which is used to delay the announcement of Android permissions;
		 * Positive counter values indicate that the permission has been granted and the counter is used to delay the announcement of the granted permission, cf. `handlePendingAndroidPermissions()` for details.
		 * Negative counter values are used to store the state of a permission.
		 */
		typedef std::unordered_map<std::string, int> AndroidPermissionMap;

		/// The counter value for permissions not yet requested.
		static constexpr int permissionNotYetRequested_ = -2;

		//// The counter value for permissions not yet granted.
		static constexpr int permissionNotYetGranted_ = -1;

		/**
		 * Definition of an unordered set holding strings.
		 */
		typedef std::unordered_set<std::string> StringSet;

		/**
		 * Definition of a vector holding XrViewConfigurationView objects.
		 */
		typedef std::vector<XrViewConfigurationView> XrViewConfigurationViews;

	public:

		/**
		 * Destructs this object.
		 */
		virtual ~NativeApplication();

		/**
		 * The run method of this application.
		 * Calling this function will start the application logic and will return before the application closes.
		 * True, if the application closes after a successful execution; False, if the application did not start successfully
		 */
		virtual bool run();

		/**
		 * Returns the device type that this application is configured for
		 * @return The device type
		 */
		inline Device::DeviceType deviceType() const;

		/**
		 * Returns the device name that this application is configured for
		 * @return The device name
		 */
		inline std::string deviceName() const;

		/**
		 * Requests an Android permission that needs to be granted by the user.
		 * In case the user grants the permission, the callback function `onPermissionGranted()` will be invoked (when requesting the permission for the first time).
		 * @param permission The name of the Android permission to request, must be valid
		 * @see onPermissionGranted().
		 */
		void requestAndroidPermission(std::string&& permission);

		/**
		 * Requests several Android permissions that needs to be granted by the user.
		 * In case the user grants the permission, the callback function `onPermissionGranted()` will be invoked (when requesting the permission for the first time).
		 * @param permissions The names of the Android permissions to request, must be valid
		 * @see onPermissionGranted().
		 */
		inline void requestAndroidPermissions(std::vector<std::string>&& permissions);

		/**
		 * Returns the Android permissions which have been granted by the user.
		 * @return The currently granted Android permissions
		 */
		StringSet grantedAndroidPermissions() const;

	protected:

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit NativeApplication(struct android_app* androidApp);

#endif // OCEAN_PLATFORM_BUILD_ANDROID

		/**
		 * Disabled copy constructor.
		 * @param nativeApplication Application object which would have been copied
		 */
		NativeApplication(const NativeApplication& nativeApplication) = delete;

		/**
		 * Returns the names of the necessary OpenXR extensions the application needs.
		 * @return The application's necessary OpenXR extensions
		 */
		virtual StringSet necessaryOpenXRExtensionNames() const;

		/**
		 * Main loop of the application.
		 */
		virtual void applicationLoop();

		/**
		 * Processes all pending Android events via ALooper_pollOnce.
		 */
		virtual void processAndroidEvents();

		/**
		 * Processes all pending OpenXR events.
		 */
		virtual void processOpenXREvents();

		/**
		 * Event functions for changed reference spaces.
		 * @param xrReferenceSpaceType The type of the reference which has changed
		 * @param previous_T_changed The transformation between the changed space and the previous space, invalid if unknown
		 * @param changeTime The time after which xrLocateSpace or xrLocateViews will return values that respect the change
		 */
		virtual void onChangedReferenceSpace(const XrReferenceSpaceType xrReferenceSpaceType, const HomogenousMatrix4& previous_T_changed, const XrTime& changeTime);

		/**
		 * Returns the current state of the OpenXR session.
		 * @return The session's state
		 */
		inline XrSessionState xrSessionState() const;

		/**
		 * Creates the OpenXR session.
		 * @param xrViewConfigurationViews The view configuration(s) for the frame buffer(s)
		 * @return True, if succeeded
		 */
		virtual bool createOpenXRSession(const XrViewConfigurationViews& xrViewConfigurationViews) = 0;

		/**
		 * Releases the OpenXR session.
		 */
		virtual void releaseOpenXRSession() = 0;

		/**
		 * Event function called after OpenXR has been initialized.
		 */
		virtual void onOpenXRInstanceInitialized();

		/**
		 * Event function called whenever the state of the OpenXR session changed.
		 * @param xrEventDataSessionStateChanged The new OpenXR session state
		 */
		virtual void onOpenXRSessionChanged(const XrEventDataSessionStateChanged& xrEventDataSessionStateChanged);

		/**
		 * Event function called whenever the session is ready, when the session state changed to XR_SESSION_STATE_READY.
		 */
		virtual void onOpenXRSessionReady();

		/**
		 * Event function called whenever the session is stopping, when the session state changed to XR_SESSION_STATE_STOPPING.
		 */
		virtual void onOpenXRSessionStopping();

		/**
		 * Event function call when an Android (or Oculus) permission is granted
		 * @param permission The name of the permission that has been granted
		 */
		virtual void onAndroidPermissionGranted(const std::string& permission);

		/**
		 * Event function call when the Android Activity is started.
		 */
		virtual void onActivityStart();

		/**
		 * Event function call when the Android Activity is resumed.
		 */
		virtual void onActivityResume();

		/**
		 * Event function call when the Android Activity is paused.
		 */
		virtual void onActivityPause();

		/**
		 * Event function call when the Android Activity is stopped.
		 */
		virtual void onActivityStop();

		/**
		 * Event function call when the Android Activity is destroyed.
		 */
		virtual void onActivityDestroy();

		/**
		 * Event function call when the window of the Android Activity is initialized.
		 */
		virtual void onActivityInitWindow();

		/**
		 * Event function call when the window of the Android Activity is terminated.
		 */
		virtual void onActivityTermWindow();

		/**
		 * Event function called at startup to set e.g., windows flags.
		 */
		virtual void onStartup();

		/**
		 * Idle event function called within the main loop whenever all Android related events have been processed.
		 */
		virtual void onIdle() = 0;

		/**
		 * Checks if a specific permission has been granted.
		 * @param permission The string identifier of a permission, must be valid
		 * @param translate Optionally translate permissions internally (short to long name), must be `false` in conjunction with Oculus permission such as `com.oculus.permission.USE_SCENE`
		 * @return True if the specified permission has been granted, otherwise false
		 */
		bool isAndroidPermissionGranted(const std::string& permission, const bool translate = false) const;

		/**
		 * Checks if any item from a set of pending permissions has been granted to the app
		 * If a pending permission has been found to be granted, it will be removed from the set of pending permissions and added to the set of granted permission.
		 * This will also call `onPermissionGranted()` in order to notify the application about the permission changes.
		 *
		 * Important note:
		 *
		 * In Android, when permissions are granted by the user for the firs time, there seems to be a delay
		 * for some permissions between when (A) they are marked as granted, i.e., `hasPermission()` returns
		 * `true` and (B) when the permission is actually granted and can be used. Any action that requires
		 * such an affected permission that is run in the time between (A) and (B) is still likely to fail (as
		 * if the permission had not been granted in the  first place). The permission will be usable after
		 * a few moments (i.e., after a few milliseconds or after a few iterations of the main loop of the app).
		 *
		 * On consecutive starts of the app and assuming all requested permissions have been granted before,
		 * this is no longer a problem; `hasPermission()` returns `true` and the permission is immediately
		 * usable.
		 *
		 * The function applies a short delay before announcing them via `onPermissionGranted()` to all
		 * permissions.
		 *
		 * @param firstCheck True when this function is called for the first time since the app started, otherwise false; cf. the note above.
		 */
		void handlePendingAndroidPermissions(const bool firstCheck);

		/**
		 * Disabled copy operator.
		 * @param nativeApplication Application object which would have been copied
		 * @return Reference to this object
		 */
		NativeApplication& operator=(const NativeApplication& nativeApplication) = delete;

		/**
		 * Android's command event function.
		 * @param androidApp The android app object
		 * @param cmd The command
		 */
		static void onAndroidCommand(struct android_app* androidApp, int32_t cmd);

	protected:

		/// The OpenXR instance used by this application.
		Platform::OpenXR::Instance xrInstance_;

		/// The OpenXR view configuration type to be used.
		XrViewConfigurationType xrViewConfigurationType_ = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

		/// The Java native interface envrionment.
		JNIEnv* jniEnv_ = nullptr;

		// The android app object as provided in the main function of the native activity.
		struct android_app* androidApp_ = nullptr;

#endif

		/// True, if the application has been resumed; False, if the application has e.g., been paused or stopped.
		bool applicationResumed_ = false;

		/// The native Android window, if any.
		ANativeWindow* androidNativeWindow_ = nullptr;

		/// The device type that this application is configured for.
		Device::DeviceType deviceType_ = Meta::Quest::Device::DT_UNKNOWN;

		/// The map mapping the names of pending Android permissions to a delay counter.
		AndroidPermissionMap pendingAndroidPermissionMap_;

		/// The set holding the names of granted Android permissions.
		StringSet grantedAndroidPermissionSet_;

		/// The lock for Android permissions.
		mutable Lock androidPermissionLock_;

	private:

		/// The current OpenXR session state.
		XrSessionState xrSessionState_ = XR_SESSION_STATE_UNKNOWN;

		/// True, if the OpenXR session is ready to be used.
		bool xrSessionIsReady_ = false;
};

inline Device::DeviceType NativeApplication::deviceType() const
{
	return deviceType_;
}

inline std::string NativeApplication::deviceName() const
{
	return Device::deviceName(deviceType());
}

inline void NativeApplication::requestAndroidPermissions(std::vector<std::string>&& permissions)
{
	const ScopedLock scopedLock(androidPermissionLock_);

	for (std::string& permission : permissions)
	{
		requestAndroidPermission(std::move(permission));
	}
}

inline XrSessionState NativeApplication::xrSessionState() const
{
	return xrSessionState_;
}

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_NATIVE_APPLICATION_H
