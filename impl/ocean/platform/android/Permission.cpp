/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/Permission.h"
#include "ocean/platform/android/ScopedJNIEnvironment.h"
#include "ocean/platform/android/ScopedJNILocalObject.h"
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

	ocean_assert(scopedJNIEnvironment->ExceptionCheck() == JNI_FALSE);

	const std::string androidPermission = translate ? translatePermission(scopedJNIEnvironment.jniEnv(), permission) : permission;

	JNIEnv& jniEnvironment = *scopedJNIEnvironment.jniEnv();

	const ScopedJClass javaClassPackageManager(Utilities::findClass(jniEnvironment, "android/content/pm/PackageManager"));

	if (!javaClassPackageManager.isValid())
	{
		return false;
	}

	const jfieldID fieldId = Utilities::getStaticFieldId(jniEnvironment, *javaClassPackageManager, "PERMISSION_GRANTED", "I");

	if (fieldId == nullptr)
	{
		return false;
	}

	const jint permissionGrantedValue = jniEnvironment.GetStaticIntField(*javaClassPackageManager, fieldId);

	const ScopedJClass javaClassContext(Utilities::findClass(jniEnvironment, "android/content/Context"));

	if (!javaClassContext.isValid())
	{
		return false;
	}

	const jmethodID methodId = Utilities::getMethodId(jniEnvironment, *javaClassContext, "checkSelfPermission", "(Ljava/lang/String;)I");

	if (methodId == nullptr)
	{
		return false;
	}

	const ScopedJString jStringAndroidPermission(jniEnvironment, Utilities::toJavaString(&jniEnvironment, androidPermission));

	if (!jStringAndroidPermission.isValid())
	{
		return false;
	}

	// a thrown exception makes checkSelfPermission() return 0, which is the value of PERMISSION_GRANTED, so that the result must not be interpreted before the call is known to have succeeded

	int32_t permissionResult = 0;

	if (!Utilities::callIntMethod(jniEnvironment, activity, methodId, permissionResult, *jStringAndroidPermission))
	{
		return false;
	}

	state = permissionResult == permissionGrantedValue;

	return true;
}

bool Permission::requestPermissions(JavaVM* javaVM, jobject activity, const Strings& permissions, const bool translate)
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

	ocean_assert(scopedJNIEnvironment->ExceptionCheck() == JNI_FALSE);

	JNIEnv& jniEnvironment = *scopedJNIEnvironment.jniEnv();

	const ScopedJClass javaClassString(Utilities::findClass(jniEnvironment, "java/lang/String"));

	if (!javaClassString.isValid())
	{
		return false;
	}

	const ScopedJString emptyString(jniEnvironment, jniEnvironment.NewStringUTF(""));

	const ScopedJObjectArray permissionArray(jniEnvironment, jniEnvironment.NewObjectArray(jsize(permissions.size()), *javaClassString, *emptyString));

	if (!permissionArray.isValid())
	{
		return false;
	}

	for (size_t n = 0; n < permissions.size(); ++n)
	{
		const std::string androidPermission = translate ? translatePermission(&jniEnvironment, permissions[n]) : permissions[n];

		if (androidPermission.empty())
		{
			return false;
		}

		const ScopedJString jStringAndroidPermission(jniEnvironment, Utilities::toJavaString(&jniEnvironment, androidPermission));

		jniEnvironment.SetObjectArrayElement(*permissionArray, jsize(n), *jStringAndroidPermission);
	}

	const ScopedJClass javaClassActivity(Utilities::findClass(jniEnvironment, "android/app/Activity"));

	if (!javaClassActivity.isValid())
	{
		return false;
	}

	const jmethodID methodId = Utilities::getMethodId(jniEnvironment, *javaClassActivity, "requestPermissions", "([Ljava/lang/String;I)V");

	if (methodId == nullptr)
	{
		return false;
	}

	return Utilities::callVoidMethod(jniEnvironment, activity, methodId, *permissionArray, 0);
}

std::string Permission::translatePermission(JNIEnv* jniEnv, const std::string& permission)
{
	ocean_assert(jniEnv != nullptr);
	ocean_assert(jniEnv->ExceptionCheck() == JNI_FALSE);
	ocean_assert(!permission.empty());

	const ScopedJClass javaClassManifestPermission(Utilities::findClass(*jniEnv, "android/Manifest$permission"));

	if (!javaClassManifestPermission.isValid())
	{
		return std::string();
	}

	const jfieldID fieldId = Utilities::getStaticFieldId(*jniEnv, *javaClassManifestPermission, permission, "Ljava/lang/String;");

	if (fieldId == nullptr)
	{
		return std::string();
	}

	const ScopedJString fieldValue(*jniEnv, jstring(jniEnv->GetStaticObjectField(*javaClassManifestPermission, fieldId)));

	if (!fieldValue.isValid())
	{
		return std::string();
	}

	return Utilities::toAString(jniEnv, *fieldValue);
}

}

}

}
