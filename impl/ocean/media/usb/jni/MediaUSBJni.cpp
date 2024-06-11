/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/usb/jni/MediaUSBJni.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_media_usb_MediaUSBJni_registerLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	Media::USB::registerUSBLibrary();
#endif

	return true;
}

jboolean Java_com_meta_ocean_media_usb_MediaUSBJni_unregisterLibrary(JNIEnv* env, jobject javaThis)
{
#ifdef OCEAN_RUNTIME_STATIC
	Media::USB::unregisterUSBLibrary();
#endif

	return true;
}
