// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/devices/blob/jni/DevicesBlobJni.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_devices_blob_DevicesBlobJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#if defined(OCEAN_RUNTIME_STATIC)
	Devices::Blob::registerBlobLibrary();
#endif

	return true;
}

jboolean Java_com_meta_ocean_devices_blob_DevicesBlobJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#if defined(OCEAN_RUNTIME_STATIC)
	Devices::Blob::unregisterBlobLibrary();
#endif

	return true;
}
