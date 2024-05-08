/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/jni/MediaOpenImageLibrariesJni.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_media_openimagelibraries_MediaOpenImageLibrariesJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
	Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();
	return true;
}

jboolean Java_com_meta_ocean_media_openimagelibraries_MediaOpenImageLibrariesJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
	Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
	return true;
}
