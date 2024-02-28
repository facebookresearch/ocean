// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/mapbuilding/jni/DevicesMapBuildingJni.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_devices_mapbuilding_DevicesMapBuildingJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
	Devices::MapBuilding::registerMapBuildingLibrary();
	return true;
}

jboolean Java_com_facebook_ocean_devices_mapbuilding_DevicesMapBuildingJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
	Devices::MapBuilding::unregisterMapBuildingLibrary();
	return true;
}
