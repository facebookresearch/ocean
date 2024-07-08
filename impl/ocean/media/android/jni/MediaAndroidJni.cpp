/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/jni/MediaAndroidJni.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_media_android_MediaAndroidJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	Media::Android::registerAndroidLibrary();
#endif

	return true;
}

jboolean Java_com_meta_ocean_media_android_MediaAndroidJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	Media::Android::unregisterAndroidLibrary();
#endif

	return true;
}
