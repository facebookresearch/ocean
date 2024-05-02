// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/arcore/jni/DevicesARCoreJni.h"

#include "ocean/devices/arcore/ARCore.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_devices_arcore_DevicesARCoreJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#if defined(OCEAN_RUNTIME_STATIC)
	Devices::ARCore::registerARCoreLibrary();
#endif

	return true;
}

jboolean Java_com_facebook_ocean_devices_arcore_DevicesARCoreJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#if defined(OCEAN_RUNTIME_STATIC)
	Devices::ARCore::unregisterARCoreLibrary();
#endif

	return true;
}
