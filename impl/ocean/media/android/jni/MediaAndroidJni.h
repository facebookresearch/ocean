// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_ANDROID_JNI_MEDIANDROIDJNI_H
#define META_OCEAN_MEDIA_ANDROID_JNI_MEDIANDROIDJNI_H

#include "ocean/media/android/jni/JNI.h"

/**
 * Java native interface function to register the media android component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup mediaandroidjni
 */
extern "C" jboolean Java_com_facebook_ocean_media_android_MediaAndroidJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the media android component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup mediaandroidjni
 */
extern "C" jboolean Java_com_facebook_ocean_media_android_MediaAndroidJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_MEDIA_ANDROID_JNI_MEDIANDROIDJNI_H
