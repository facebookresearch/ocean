// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_NATIVE_INTERFACE_XR_PLAYGROUND_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_NATIVE_INTERFACE_XR_PLAYGROUND_H

#include "application/ocean/xrplayground/oculus/XRPlaygroundOculus.h"

namespace Ocean
{

namespace XRPlayground
{

namespace Oculus
{

/**
 * Java native interface function to set or to update the user's login information.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI object from which this function called, must be valid
 * @param accountType The type of the account, must be valid
 * @param userId The user's id to be set, can be empty
 * @param userToken The user's token to be set, must be valid
 * @return True, if succeeded
 * @ingroup xrplaygroundoculus
 */
extern "C" jboolean Java_com_facebook_ocean_app_xrplayground_oculus_NativeInterfaceXRPlayground_setUserToken(JNIEnv* env, jobject javaThis, jstring accountType, jstring userId, jstring userToken);

/**
 * Java native interface function to set the experience which will be started directly after launch.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI object from which this function called, must be valid
 * @param experience The name of the experience to start
 * @return True, if succeeded
 * @ingroup xrplaygroundoculus
 */
extern "C" jboolean Java_com_facebook_ocean_app_xrplayground_oculus_NativeInterfaceXRPlayground_setLaunchExperience(JNIEnv* env, jobject javaThis, jstring experience);

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_NATIVE_INTERFACE_XR_PLAYGROUND_H
