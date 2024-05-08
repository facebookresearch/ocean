/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_GL_FRAME_VIEW_H
#define META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_GL_FRAME_VIEW_H

#include "ocean/platform/android/application/jni/JNI.h"

#include <jni.h>

/**
 * Java native interface function to set or change the view's horizontal field of view.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param angle New horizontal field of view in radian
 * @return True, if succeeded
 * @ingroup platformandroidapplicationjni
 */
extern "C" jboolean Java_com_meta_ocean_platform_android_application_GLFrameView_setFovX(JNIEnv* env, jobject javaThis, jdouble angle);

/**
 * Java native interface function to set or change the view's background color.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param red Red color value, with range [0.0, 1.0]
 * @param green Green color value, with range [0.0, 1.0]
 * @param blue Blue color value, with range [0.0, 1.0]
 * @return True, if succeeded
 * @ingroup platformandroidapplicationjni
 */
extern "C" jboolean Java_com_meta_ocean_platform_android_application_GLFrameView_setBackgroundColor(JNIEnv* env, jobject javaThis, jdouble red, jdouble green, jdouble blue);

/**
 * Java native interface function to set or change the view's background media object.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param url URL of the media object to be used as background medium
 * @param type Hint defining the media object in more detail, possible values are "LIVE_VIDEO", "IMAGE", "MOVIE", "IMAGE_SEQUENCE" or "FRAME_STREAM", if no hint is given the first possible media object will be created
 * @param preferredWidth Preferred width of the medium in pixel, use 0 to use the default width
 * @param preferredHeight Preferred height of the medium in pixel, use 0 to use the default height
 * @param adjustFov True, to adjust the view's field of view to the field of the background automatically
 * @return True, if succeeded
 * @ingroup platformandroidapplicationjni
 */
extern "C" jboolean Java_com_meta_ocean_platform_android_application_GLFrameView_setFrameMedium(JNIEnv* env, jobject javaThis, jstring url, jstring type, jint preferredWidth, jint preferredHeight, jboolean adjustFov);

/**
 * Java native interface function to register the instance function for the corresponding native C++ class.
 * @param env JNI environment
 * @param javaThis JNI object
 * @ingroup platformandroidapplicationjni
 */
extern "C" void Java_com_meta_ocean_platform_android_application_GLFrameView_registerInstance(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_GL_FRAME_VIEW_H
