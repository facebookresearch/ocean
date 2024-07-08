/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/mapbuilding/jni/DevicesMapBuildingJni.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_devices_mapbuilding_DevicesMapBuildingJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#if defined(OCEAN_RUNTIME_STATIC)
	Devices::MapBuilding::registerMapBuildingLibrary();
#endif

	return true;
}

jboolean Java_com_meta_ocean_devices_mapbuilding_DevicesMapBuildingJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#if defined(OCEAN_RUNTIME_STATIC)
	Devices::MapBuilding::unregisterMapBuildingLibrary();
#endif

	return true;
}
