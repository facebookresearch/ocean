// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/cloud/jni/DevicesCloudJni.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_devices_cloud_DevicesCloudJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
	Devices::Cloud::registerCloudLibrary();
	return true;
}

jboolean Java_com_facebook_ocean_devices_cloud_DevicesCloudJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
	Devices::Cloud::unregisterCloudLibrary();
	return true;
}
