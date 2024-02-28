// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/openimagelibraries/jni/MediaOpenImageLibrariesJni.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_media_openimagelibraries_MediaOpenImageLibrariesJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
	Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();
	return true;
}

jboolean Java_com_facebook_ocean_media_openimagelibraries_MediaOpenImageLibrariesJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
	Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
	return true;
}
