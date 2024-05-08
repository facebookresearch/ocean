/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_NETWORK_JNI_NETWORKJNI_H
#define META_OCEAN_NETWORK_JNI_NETWORKJNI_H

#include "ocean/network/jni/JNI.h"

/**
 * Java native interface function to set the current local IP address.
 * @param env The JNI environment, must be valid
 * @param javaThis The NetworkJni JNI object
 * @param localIPAddress The current local IP address, in big endian notation
 * @return True, if succeeded
 * @ingroup platformandroid
 */
extern "C" jboolean Java_com_meta_ocean_network_NetworkJni_setCurrentLocalIPAddress(JNIEnv* env, jobject javaThis, jint localIPAddress);

#endif // META_OCEAN_NETWORK_JNI_NETWORKJNI_H
