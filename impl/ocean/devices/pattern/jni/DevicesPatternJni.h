// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_PATTERN_JNI_DEVICESPATTERNJNI_H
#define META_OCEAN_DEVICES_PATTERN_JNI_DEVICESPATTERNJNI_H

#include "ocean/devices/pattern/jni/JNI.h"

/**
 * Java native interface function to register the Devices::Pattern component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup devicespatternjni
 */
extern "C" jboolean Java_com_facebook_ocean_devices_pattern_DevicesPatternJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the Devices::Pattern component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup devicespatternjni
 */
extern "C" jboolean Java_com_facebook_ocean_devices_pattern_DevicesPatternJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_DEVICES_PATTERN_JNI_DEVICESPATTERNJNI_H
