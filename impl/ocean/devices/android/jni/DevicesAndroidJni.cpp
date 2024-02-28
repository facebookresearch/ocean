// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/android/jni/DevicesAndroidJni.h"

#include "ocean/devices/android/Android.h"
#include "ocean/devices/android/AndroidFactory.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_devices_android_DevicesAndroidJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
	Devices::Android::registerAndroidLibrary();
	return true;
}

jboolean Java_com_facebook_ocean_devices_android_DevicesAndroidJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
	Devices::Android::unregisterAndroidLibrary();
	return true;
}

jboolean Java_com_facebook_ocean_devices_android_DevicesAndroidJni_newGPSLocation(JNIEnv* env, jobject javaThis, jdouble latitude, jdouble longitude, jfloat altitude, jfloat direction, jfloat speed, jfloat accuracy, jfloat altitudeAccuracy, jfloat directionAccuracy, jfloat speedAccuracy, jdouble timestamp)
{
	Devices::Android::AndroidFactory::newGPSLocation(latitude, longitude, altitude, direction, speed, accuracy, altitudeAccuracy, directionAccuracy, speedAccuracy, Ocean::Timestamp(timestamp));
	return true;
}
