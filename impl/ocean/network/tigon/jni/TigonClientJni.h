// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_CLIENT_NATIVE_INTERFACE_H
#define FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_CLIENT_NATIVE_INTERFACE_H

#include <jni.h>

/**
 * Java native interface function to set or to update the Tigon service for Tigon requests.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param tigonServiceHolder The holder of the Tigon service, must be valid
 * @param executor The executor to be used to execute the queries, can be null to use a pure native executor
 * @return True, if succeeded
 * @ingroup platformandroid
 */
extern "C" jboolean Java_com_facebook_ocean_network_tigon_TigonClientJni_setTigonService(JNIEnv* env, jobject javaThis, jobject tigonServiceHolder, jobject executor);

/**
 * Java native interface function to release the Tigon service for Tigon requests.
 * @param env JNI environment
 * @param javaThis JNI object
 * @return True, if succeeded
 * @ingroup platformandroid
 */
extern "C" jboolean Java_com_facebook_ocean_network_tigon_TigonClientJni_releaseTigonService(JNIEnv* env, jobject javaThis);

#endif // FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_CLIENT_NATIVE_INTERFACE_H
