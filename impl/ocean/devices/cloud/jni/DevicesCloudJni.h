// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_CLOUD_JNI_DEVICESCLOUDJNI_H
#define META_OCEAN_DEVICES_CLOUD_JNI_DEVICESCLOUDJNI_H

#include "ocean/devices/cloud/jni/JNI.h"

/**
 * Java native interface function to register the Devices::Cloud component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup devicescloudjni
 */
extern "C" jboolean Java_com_facebook_ocean_devices_cloud_DevicesCloudJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the Devices::Cloud component.
 * @param env JNI environment
 * @param javaThis JNI object
 * @return True, if succeeded
 * @ingroup devicescloudjni
 */
extern "C" jboolean Java_com_facebook_ocean_devices_cloud_DevicesCloudJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_DEVICES_CLOUD_JNI_DEVICESCLOUDJNI_H
