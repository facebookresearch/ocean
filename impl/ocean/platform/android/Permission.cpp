/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/Permission.h"
#include "ocean/platform/android/ScopedJNIEnvironment.h"
#include "ocean/platform/android/ScopedJNIObject.h"
#include "ocean/platform/android/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

bool Permission::hasPermission(JavaVM* javaVM, jobject activity, const std::string& permission, bool& state, const bool translate)
{
	ocean_assert(javaVM != nullptr);
	ocean_assert(activity != nullptr);
	ocean_assert(!permission.empty());

	const ScopedJNIEnvironment scopedJNIEnvironment(javaVM);

	if (!scopedJNIEnvironment)
	{
		return false;
	}

	const std::string androidPermission = translate ? translatePermission(scopedJNIEnvironment.jniEnv(), permission) : permission;

	const ScopedJClass javaClassPackageManager(scopedJNIEnvironment, scopedJNIEnvironment.jniEnv()->FindClass("android/content/pm/PackageManager"));

	if (!javaClassPackageManager.isValid())
	{
		return false;
	}

	jfieldID fieldId = scopedJNIEnvironment.jniEnv()->GetStaticFieldID(javaClassPackageManager, "PERMISSION_GRANTED", "I");

	if (fieldId == nullptr)
	{
		return false;
	}

	const jint permissionGrantedValue = scopedJNIEnvironment.jniEnv()->GetStaticIntField(javaClassPackageManager, fieldId);

	const ScopedJClass javaClassContext(scopedJNIEnvironment, scopedJNIEnvironment.jniEnv()->FindClass("android/content/Context"));

	if (!javaClassContext.isValid())
	{
		return false;
	}

	jmethodID methodId = scopedJNIEnvironment.jniEnv()->GetMethodID(javaClassContext, "checkSelfPermission", "(Ljava/lang/String;)I");

	if (methodId == nullptr)
	{
		return false;
	}

	const ScopedJString jStringAndroidPermission(scopedJNIEnvironment, Utilities::toJavaString(scopedJNIEnvironment.jniEnv(), androidPermission));

	if (!jStringAndroidPermission.isValid())
	{
		return false;
	}

	const jint permissionResult = scopedJNIEnvironment.jniEnv()->CallIntMethod(activity, methodId, *jStringAndroidPermission);

	state = permissionResult == permissionGrantedValue;

	return true;
}

bool Permission::requestPermissions(JavaVM* javaVM, jobject activity, const std::vector<std::string>& permissions, const bool translate)
{
	ocean_assert(javaVM != nullptr);
	ocean_assert(activity != nullptr);
	ocean_assert(!permissions.empty());

	if (permissions.empty())
	{
		return false;
	}

	const ScopedJNIEnvironment scopedJNIEnvironment(javaVM);

	if (!scopedJNIEnvironment)
	{
		return false;
	}

	const ScopedJObjectArray permissionArray (scopedJNIEnvironment, scopedJNIEnvironment.jniEnv()->NewObjectArray(jsize(permissions.size()), scopedJNIEnvironment.jniEnv()->FindClass("java/lang/String"), scopedJNIEnvironment.jniEnv()->NewStringUTF("")));

	if (!permissionArray.isValid())
	{
		return false;
	}

	for (size_t n = 0; n < permissions.size(); ++n)
	{
		const std::string androidPermission = translate ? translatePermission(scopedJNIEnvironment.jniEnv(), permissions[n]) : permissions[n];

		if (androidPermission.empty())
		{
			return false;
		}

		scopedJNIEnvironment.jniEnv()->SetObjectArrayElement(permissionArray, jsize(n), Utilities::toJavaString(scopedJNIEnvironment.jniEnv(), androidPermission));
	}

	const ScopedJClass javaClassActivity(scopedJNIEnvironment, scopedJNIEnvironment.jniEnv()->FindClass("android/app/Activity"));

	if (!javaClassActivity.isValid())
	{
		return false;
	}

	jmethodID methodId = scopedJNIEnvironment.jniEnv()->GetMethodID(javaClassActivity, "requestPermissions", "([Ljava/lang/String;I)V");

	if (methodId == nullptr)
	{
		return false;
	}

	scopedJNIEnvironment.jniEnv()->CallVoidMethod(activity, methodId, *permissionArray, 0);

	return true;
}

std::string Permission::translatePermission(JNIEnv* jniEnv, const std::string& permission)
{
	ocean_assert(jniEnv != nullptr);
	ocean_assert(!permission.empty());

	jclass javaClassManifestPermission = jniEnv->FindClass("android/Manifest$permission");

	if (javaClassManifestPermission == nullptr)
	{
		return std::string();
	}

	jfieldID fieldId = jniEnv->GetStaticFieldID(javaClassManifestPermission, permission.c_str(), "Ljava/lang/String;");

	if (fieldId == nullptr)
	{
		return std::string();
	}

	jstring fieldValue = (jstring)(jniEnv->GetStaticObjectField(javaClassManifestPermission, fieldId));

	if (fieldValue == nullptr)
	{
		return std::string();
	}

	return Utilities::toAString(jniEnv, fieldValue);
}

}

}

}
