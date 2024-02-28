// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_BLOB_JNI_DEVICESBLOBJNI_H
#define META_OCEAN_DEVICES_BLOB_JNI_DEVICESBLOBJNI_H

#include "ocean/devices/blob/jni/JNI.h"

/**
 * Java native interface function to register the Devices::Blob component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup devicesblobjni
 */
extern "C" jboolean Java_com_facebook_ocean_devices_blob_DevicesBlobJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the Devices::Blob component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup devicesblobjni
 */
extern "C" jboolean Java_com_facebook_ocean_devices_blob_DevicesBlobJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_DEVICES_BLOB_JNI_DEVICESBLOBJNI_H
