// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/pattern/jni/DevicesPatternJni.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_devices_pattern_DevicesPatternJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#if defined(OCEAN_RUNTIME_STATIC)
	Devices::Pattern::registerPatternLibrary();
#endif

	return true;
}

jboolean Java_com_facebook_ocean_devices_pattern_DevicesPatternJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#if defined(OCEAN_RUNTIME_STATIC)
	Devices::Pattern::unregisterPatternLibrary();
#endif

	return true;
}
