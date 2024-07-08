/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_PERMISSION_H
#define META_OCEAN_PLATFORM_ANDROID_PERMISSION_H

#include "ocean/platform/android/Android.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * This class implements functions for Android application permissions.
 * @ingroup platformandroid
 */
class OCEAN_PLATFORM_ANDROID_EXPORT Permission
{
	public:

	/**
	 * Returns whether the Android app has a specific permission.
	 * @param javaVM The Java virtual machine, must be valid
	 * @param activity The Android main activity, must be valid
	 * @param permission The permission to be checked, must be valid
	 * @param translate If true, translates short permission name to Android-internal full permission name, note: must be `false` for Oculus permissions, e.g., `com.oculus.permission.USE_SCENE`
	 * @param state True, if the app has the specified permission; False, if the app does not have the specified permission
	 * @return True, if succeeded
	 */
	static bool hasPermission(JavaVM* javaVM, jobject activity, const std::string& permission, bool& state, const bool translate = true);

	/**
	 * Requests several permissions for the Android app.
	 * @param javaVM The Java virtual machine, must be valid
	 * @param activity The Android main activity, must be valid
	 * @param permissions The permissions to be requested, at least one
	 * @param translate If true, translates short permission name to Android-internal full permission name, note: must be `false` for Oculus permissions, e.g., `com.oculus.permission.USE_SCENE`
	 * @return True, if the request was invoked successfully, does not check whether the permission was granted
	 */
	static bool requestPermissions(JavaVM* javaVM, jobject activity, const std::vector<std::string>& permissions, const bool translate = true);

	/**
	 * Determines Android's internal permission name for a readable permission.
	 * @param jniEnv The Java environment, attached with the current thread, must be valid
	 * @param permission The readable permission
	 * @return Android's internal permission
	 */
	static std::string translatePermission(JNIEnv* jniEnv, const std::string& permission);
};

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_PERMISSION_H
