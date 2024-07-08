/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_UTILITIES_H
#define META_OCEAN_PLATFORM_ANDROID_UTILITIES_H

#include "ocean/platform/android/Android.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * This class implements helper functions for android platforms.
 * @ingroup platformandroid
 */
class OCEAN_PLATFORM_ANDROID_EXPORT Utilities
{
	public:

		/**
		 * Converts a Java native string to a std string.
		 * @param env The Java environment, must be valid
		 * @param javaString Java string to be converted, may be nullptr
		 * @return Resulting std string
		 */
		static std::string toAString(JNIEnv* env, jstring javaString);

		/**
		 * Converts a std string to a Java native string.
		 * @param env The Java environment, must be valid
		 * @param stdString Std string to be converted
		 * @return Resulting Java native string
		 */
		static jstring toJavaString(JNIEnv* env, const std::string& stdString);

		/**
		 * Converts a vector of std strings to a Java array with native strings.
		 * @param env The Java environment, must be valid
		 * @param strings Std strings to be converted, can be empty
		 * @return Resulting Java native string
		 */
		static jobjectArray toJavaStringArray(JNIEnv* env, const std::vector<std::string>& strings);

		/**
		 * Converts a Java native list with string to a vector of strings.
		 * @param env The Java environment, must be valid
		 * @param javaStringList Java list with strings to be converted, must be valid
		 * @param strings The resulting vector of strings
		 * @return True, if succeeded
		 */
		static bool toVector(JNIEnv* env, jobject javaStringList, std::vector<std::string>& strings);

		/**
		 * Converts a Java native list with integer to a vector of integers.
		 * @param env The Java environment, must be valid
		 * @param javaIntegerList Java list with integers to be converted, must be valid
		 * @param values The resulting vector of integers
		 * @return True, if succeeded
		 */
		static bool toVector(JNIEnv* env, jobject javaIntegerList, std::vector<int>& values);

		/**
		 * Returns the class name of an object.
		 * @param env The Java environment, must be valid
		 * @param object The object for which the class name will be returned, must be valid
		 * @param name The resulting class name
		 * @return True, if succeeded
		 */
		static bool className(JNIEnv* env, jobject object, std::string& name);

		/**
		 * Returns the version code stored in the application manifest.
		 * @param javaVM The Java virtual machine, must be valid
		 * @param activity The Android main activity, must be valid
		 * @param versionCode The resulting version code of the application
		 * @param versionName The resulting version name of the application
		 * @return True, if succeeded
		 */
		static bool manifestVersion(JavaVM* javaVM, jobject activity, int& versionCode, std::string& versionName);

		/**
		 * Deprecated.
		 *
		 * Returns the version code stored in the application manifest.
		 * @param javaVM The Java virtual machine, must be valid
		 * @param activity The Android main activity, must be valid
		 * @param versionCode The version code of the application
		 * @return True, if succeeded
		 */
		static bool manifestVersionCode(JavaVM* javaVM, jobject activity, int& versionCode);

		/**
		 * Returns the minimum and target SDK versions stored in the application manifest.
		 * @param env The Java environment, must be valid
		 * @param activity The Android main activity, must be valid
		 * @param minSdkVersion The resulting minimum SDK version of the application as defined in its manifest
		 * @param targetSdkVersion The resulting target SDK version of the application as defined in its manifest
		 * @return True, if succeeded
		 */
		static bool manifestSdkVersions(JNIEnv* env, jobject activity, unsigned int& minSdkVersion, unsigned int& targetSdkVersion);

		/**
		 * Return the Android SDK version of the system.
		 * @param env The Java environment, must be valid
		 * @param version The resulting Android SDK version, only valid if this function returns 'true'
		 * @return True, if succeeded, otherwise false
		 */
		static bool androidSdkVersion(JNIEnv* env, unsigned int& version);

		/**
		 * Returns the value of a specific system property.
		 * @param name The name of the system property, must be valid
		 * @param value The resulting value
		 * @return True, if succeeded
		 */
		static bool systemPropertyValue(const std::string& name, std::string& value);

		/**
		 * Returns the brand string of the device.
		 * @param env The Java environment, must be valid
		 * @param brand The resulting model string
		 * @return True, if succeeded
		 */
		static bool deviceBrand(JNIEnv* env, std::string& brand);

		/**
		 * Returns the model string of the device.
		 * @param env The Java environment, must be valid
		 * @param model The resulting model string
		 * @return True, if succeeded
		 */
		static bool deviceModel(JNIEnv* env, std::string& model);

		/**
		 * Returns the name of the package.
		 * @param packageName The resulting package name
		 * @return True, if succeeded
		 */
		static bool determinePackageName(std::string& packageName);

		/**
		 * Sends an explicit intent to another component or application.
		 * @param env The Java environment, must be valid
		 * @param activity The Android main activity, must be valid
		 * @param packageName The name of the package to which the intent will be sent, must be valid
		 * @param className The name of the class or activity to which the intent will be sent, must be valid
		 * @param extraText The data that will be sent with the intent, must be valid
		 * @return True, if the intent was sent successfully, otherwise false
		 */
		static bool sendIntentToComponent(JNIEnv* env, jobject activity, const std::string& packageName, const std::string& className, const std::string& extraText);

		/**
		 * Starts an new activity from a root activity.
		 * @param env The Java environment, must be valid
		 * @param rootActivity The root activity from which the new activity will be started, must be valid
		 * @param activityClassName The name of the class of the activity to start, must be valid
		 * @return True, if succeeded
		 */
		static bool startActivity(JNIEnv* env, jobject rootActivity, const std::string& activityClassName);

		/**
		 * Joins a Wi-Fi network (WPA2-PSK).
		 * This only works with Android SDK version 28 or below.
		 * @param env The Java environment, must be valid
		 * @param activity The root activity from which the new activity will be started, must be valid
		 * @param ssid The Wi-Fi network name (SSID), the network must be using WPA2, must be valid
		 * @param password The Wi-Fi network password, must be valid
		 * @return True, if joined successfully, otherwise false
		 **/
		static bool connectToWifi(JNIEnv* env, jobject activity, const std::string& ssid, const std::string& password);

		/**
		 * Sends an intent to the OS to request it to join a Wi-Fi network (WPA2-PSK).
		 * The intent is send to the system settings app to save and connect to a new Wi-Fi network. This will display a notification to user to accept or reject the new Wi-Fi network.
		 * Additional notes:
		 *  * The Wi-Fi network must use the security protocol WPA2-PSK.
		 *  * This function requires Android SDK version 29 or higher.
		 *  * This function requires the permissions `android.permission.CHANGE_NETWORK_STATE` and `android.permission.WRITE_SETTINGS` to be set in the app manifest to work.
		 *  * Rejecting a new network does NOT revoke the permission `android.permission.CHANGE_NETWORK_STATE` again. Re-tries are possible.
		 *  * The newly added networks do NOT have to reachable at the time of adding them (they will be stored for when they become available).
		 *  * The system app receiving the intent sent here must implement a handler for the action type `Settings.ACTION_WIFI_ADD_NETWORKS`. Otherwise this function will have no effect (and still return `true`).
		 * @param env The Java environment, must be valid
		 * @param activity The root activity from which the new activity will be started, must be valid
		 * @param ssid The Wi-Fi network name (SSID), the network must be using WPA2, must be valid
		 * @param password The Wi-Fi network password, must be valid
		 * @return True, if the intent was sent successfully, otherwise false
		 **/
		static bool sendIntentToConnectToWifi(JNIEnv* env, jobject activity, const std::string& ssid, const std::string& password);

		/**
		 * Returns current Wi-Fi network name (SSID).
		 * @param env The Java environment, must be valid
		 * @param activity The root activity from which the new activity will be started, must be valid
		 * @param ssid The resulting Wi-Fi network name (SSID), will be empty if there is no Wi-Fi connection
		 * @return True, if succeeded
		 */
		static bool currentWifiSsid(JNIEnv* env, jobject activity, std::string& ssid);
};

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_UTILITIES_H
