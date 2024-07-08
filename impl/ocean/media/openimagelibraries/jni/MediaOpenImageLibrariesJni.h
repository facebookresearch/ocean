/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_OPENIMAGELIBRARIES_JNI_OPENIMAGELIBRARIESJNI_H
#define META_OCEAN_MEDIA_OPENIMAGELIBRARIES_JNI_OPENIMAGELIBRARIESJNI_H

#include "ocean/media/openimagelibraries/jni/JNI.h"

/**
 * Java native interface function to register the media Open Image Libraries component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup mediaoiljni
 */
extern "C" jboolean Java_com_meta_ocean_media_openimagelibraries_MediaOpenImageLibrariesJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the media Open Image Libraries component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup mediaoiljni
 */
extern "C" jboolean Java_com_meta_ocean_media_openimagelibraries_MediaOpenImageLibrariesJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_MEDIA_OPENIMAGELIBRARIES_JNI_OPENIMAGELIBRARIESJNI_H
