/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/jni/DevicesAndroidJni.h"

#include "ocean/devices/android/Android.h"
#include "ocean/devices/android/AndroidFactory.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_devices_android_DevicesAndroidJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#if defined(OCEAN_RUNTIME_STATIC)
	Devices::Android::registerAndroidLibrary();
#endif

	return true;
}

jboolean Java_com_meta_ocean_devices_android_DevicesAndroidJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#if defined(OCEAN_RUNTIME_STATIC)
	Devices::Android::unregisterAndroidLibrary();
#endif

	return true;
}

jboolean Java_com_meta_ocean_devices_android_DevicesAndroidJni_newGPSLocation(JNIEnv* env, jobject javaThis, jdouble latitude, jdouble longitude, jfloat altitude, jfloat direction, jfloat speed, jfloat accuracy, jfloat altitudeAccuracy, jfloat directionAccuracy, jfloat speedAccuracy, jdouble timestamp)
{
	Devices::Android::AndroidFactory::newGPSLocation(latitude, longitude, altitude, direction, speed, accuracy, altitudeAccuracy, directionAccuracy, speedAccuracy, Ocean::Timestamp(timestamp));
	return true;
}
