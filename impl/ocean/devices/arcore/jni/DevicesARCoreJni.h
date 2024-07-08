/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARCORE_DEVICESARCOREJNI_H
#define META_OCEAN_DEVICES_ARCORE_DEVICESARCOREJNI_H

#include "ocean/devices/arcore/jni/JNI.h"

/**
 * Java native interface function to register the Devices::ARCore component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup devicesarcorejni
 */
extern "C" jboolean Java_com_meta_ocean_devices_arcore_DevicesARCoreJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the Devices::ARCore component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup devicesarcorejni
 */
extern "C" jboolean Java_com_meta_ocean_devices_arcore_DevicesARCoreJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_DEVICES_ARCORE_DEVICESARCOREJNI_H
