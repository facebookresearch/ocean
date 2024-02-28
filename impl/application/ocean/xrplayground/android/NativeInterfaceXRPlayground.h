// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_ANDROID_NATIVE_INTERFACE_XR_PLAYGROUND_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_ANDROID_NATIVE_INTERFACE_XR_PLAYGROUND_H

#include "application/ocean/xrplayground/android/XRPlaygroundAndroid.h"

namespace Ocean
{

namespace XRPlayground
{

namespace Android
{

/**
 * Java native interface function to set or to update the user's login information.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI object from which this function called, must be valid
 * @param accountType The type of the account, must be valid
 * @param userId The user's id to be set, can be empty
 * @param userToken The user's token to be set, must be valid
 * @return True, if succeeded
 * @ingroup xrplaygroundandroid
 */
extern "C" jboolean Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_setUserToken(JNIEnv* env, jobject javaThis, jstring accountType, jstring userId, jstring userToken);

/**
 * Java native interface function loading a new content.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI object from which this function called, must be valid
 * @param filename Filename of the content to be loaded, an empty string to unload all previously loaded content
 * @param replace State determining whether already existing content will be removed first, or whether the content will be added instead
 * @return True, if succeeded
 * @ingroup xrplaygroundandroid
 */
extern "C" jboolean Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_loadContent(JNIEnv* env, jobject javaThis, jstring filename, jboolean replace);

/**
 * Java native interface function to start VRS recording.
 * The VRS file will be written to "/sdcard/VRSRecordings/"
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI object from which this function called, must be valid
 * @return True, if succeeded
 */
extern "C" jboolean Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_startVRSRecording(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to stop VRS recording.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI object from which this function called, must be valid
 * @return True, if succeeded
 */
extern "C" jboolean Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_stopVRSRecording(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to return the names of the individual experience groups.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI object from which this function called, must be valid
 * @return The number of experience groups, with range [0, infinity)
 */
extern "C" jobjectArray Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_experienceGroups(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to return the names of all experiences in an experience group.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI object from which this function called, must be valid
 * @param groupIndex The index of the experience group for which all names will be returned
 * @return The names of experiences
 */
extern "C" jobjectArray Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_experienceNamesInGroup(JNIEnv* env, jobject javaThis, jint groupIndex);

/**
 * Java native interface function to return the URLs of all experiences in an experience group.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI object from which this function called, must be valid
 * @param groupIndex The index of the experience group for which all URLS will be returned
 * @return The URLs of experiences
 */
extern "C" jobjectArray Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_experienceURLsInGroup(JNIEnv* env, jobject javaThis, jint groupIndex);

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_ANDROID_NATIVE_INTERFACE_XR_PLAYGROUND_H
