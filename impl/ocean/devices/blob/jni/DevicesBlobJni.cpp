// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/blob/jni/DevicesBlobJni.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_devices_blob_DevicesBlobJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
	Devices::Blob::registerBlobLibrary();
	return true;
}

jboolean Java_com_facebook_ocean_devices_blob_DevicesBlobJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
	Devices::Blob::unregisterBlobLibrary();
	return true;
}
