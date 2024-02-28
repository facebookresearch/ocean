// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/android/jni/MediaAndroidJni.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_media_android_MediaAndroidJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
	Media::Android::registerAndroidLibrary();
	return true;
}

jboolean Java_com_facebook_ocean_media_android_MediaAndroidJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
	Media::Android::unregisterAndroidLibrary();
	return true;
}
