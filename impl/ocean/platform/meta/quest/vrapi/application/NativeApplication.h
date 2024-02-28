// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_NATIVE_APPLICATION_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_NATIVE_APPLICATION_H

#include "ocean/platform/meta/quest/vrapi/application/Application.h"

#include "ocean/platform/meta/quest/Device.h"

#include <VrApi.h>

#include <android/native_window_jni.h>
#include <android_native_app_glue.h>
#include <android/window.h>

#include <unordered_map>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

namespace Application
{

/**
 * This class implements a basic Quest application based on an Android NativeActivity using VrApi.
 * @ingroup platformmetaquestvrapiapplication
 */
class OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT NativeApplication
{
	public:

		/**
		 * Definition of a set of Android permissions.
		 */
		typedef std::unordered_set<std::string> AndroidPermissionsSet;

	protected:

		/**
		 * Definition of a mapping from Android permissions to an index which is used to delay the announcement of Android permissions;
		 * index value `-1` denotes permission as not granted, positive values indicate the number of iterations of the main application loop before announcing the permission, cf. `updateGrantedPermissions()` for details.
		 */
		typedef std::unordered_map<std::string, int> AndroidPermissionMap;

	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit NativeApplication(struct android_app* androidApp);

		/**
		 * Destructs this object.
		 */
		virtual ~NativeApplication();

		/**
		 * The run method of this application.
		 * Calling this function will start the application logic and will return before the application closes.
		 * True, if the application closes after a successful exectuion; False, if the application did not start successfully
		 */
		virtual bool run();

		/**
		 * Returns the ovrJava object of this application.
		 * @return The java object
		 */
		inline const ovrJava& java() const;

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

	protected:

		/**
		 * Disabled copy constructor.
		 * @param nativeApplication Application object which would have been copied
		 */
		NativeApplication(const NativeApplication& nativeApplication) = delete;

		/**
		 * Main loop loop of the application.
		 */
		virtual void applicationLoop();

		/**
		 * Processes all pending Android events via ALooper_pollAll.
		 */
		virtual void processAndroidEvents();

		/**
		 * Returns whether the VrApi VR mode is currently entered.
		 * @return True, if so
		 */
		virtual bool isVRModeEntered() const;

		/**
		 * Shows the system's `Confirm Quit Menu`.
		 */
		virtual void showSystemConfirmQuitMenu();

		/**
		 * Returns the set of permissions that are required by the app and that have to be requested from the operating system
		 */
		virtual AndroidPermissionsSet androidPermissionsToRequest();

		/**
		 * Event function call when an Android (or Oculus) permission is granted
		 * @param permission The name of the permission that has been granted
		 */
		virtual void onPermissionGranted(const std::string& permission);

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
		 * Event function called before the VrApi will been initialized.
		 * Overwrite this function to specify custom initialization parameters.
		 * @param initParams The initialization parameters which will be used to initialize the VrApi, the parameters can be changed
		 */
		virtual void onPreVrApiInitialize(ovrInitParms& initParams);

		/**
		 * Event function called after the vrapi has been initialized.
		 */
		virtual void onVrApiInitialized();

		/**
		 * Idle event function called within the main loop whenever all Android related events have been processed.
		 */
		virtual void onIdle();

		/**
		 * Checks if a specific permission has been granted
		 * @param permission The string identifier of a permission, must be valid
		 * @param translate Optionally translate permissions internally (short to long name), must be `false` in conjunction with Oculus permission such as `com.oculus.permission.ACCESS_MR_SENSOR_DATA`
		 * @return True if the specified permission has been granted, otherwise false
		 */
		bool hasPermission(const std::string& permission, const bool translate = false);

		/**
		 * Requests a set of Android permissions
		 * @note The return value `true` only means that the permissions have been requested successfully. It does not mean that they have been granted; permissions must be checked separately, e.g., using `NativeApplication::hasPermission()`.
		 * @param permissions The permissions that will be requested
		 * @return True if the permissions have been requested successfully, otherwise false
		 */
		bool requestPermissions(const AndroidPermissionsSet& permissions);

		/**
		 * Checks if any item from a set of missing permissions has been granted to the app
		 * If a missing permission has been found to be granted, it will be removed from the set of missing permissions and added to the set of granted permission.
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
		 * @param missingPermissions A set of permissions that the app still need to be granted; the function returns immediately if this set is empty
		 * @param firstCheck True when this function is called for the first time since the app started, otherwise false; cf. the note above.
		 * @return The number of remaining missing permissions, range: [0, missingPermissions.size()]
		 */
		size_t updateGrantedPermissions(AndroidPermissionMap& missingPermissions, const bool firstCheck);

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

		// The android app object as provided in the main function of the native activity.
		struct android_app* androidApp_ = nullptr;

		/// The OVR Java object.
		ovrJava ovrJava_ = {};

		/// True, if the application has been resumed; False, if the application has e.g., been paused or stopped.
		bool applicationResumed_ = false;

		/// The native Android window, if any.
		ANativeWindow* androidNativeWindow_ = nullptr;

	private:

		/// The device type that this application is configured for.
		Device::DeviceType deviceType_ = Device::DT_UNKNOWN;
};

inline const ovrJava& NativeApplication::java() const
{
	return ovrJava_;
}

inline Device::DeviceType NativeApplication::deviceType() const
{
	return deviceType_;
}

inline std::string NativeApplication::deviceName() const
{
	return Device::deviceName(deviceType());
}

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_NATIVE_APPLICATION_H
