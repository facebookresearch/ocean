/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_MAPBUILDING_JNI_DEVICESMAPBULDINGJNI_H
#define META_OCEAN_DEVICES_MAPBUILDING_JNI_DEVICESMAPBULDINGJNI_H

#include "ocean/devices/mapbuilding/jni/JNI.h"

/**
 * Java native interface function to register the Devices::MapBuilding component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup devicesmapbuildingjni
 */
extern "C" jboolean Java_com_meta_ocean_devices_mapbuilding_DevicesMapBuildingJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the Devices::MapBuilding component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup devicesmapbuildingjni
 */
extern "C" jboolean Java_com_meta_ocean_devices_mapbuilding_DevicesMapBuildingJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_DEVICES_MAPBUILDING_JNI_DEVICESMAPBULDINGJNI_H
