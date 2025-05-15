/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_GL_RENDERER_VIEW_H
#define META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_GL_RENDERER_VIEW_H

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
extern "C" jboolean Java_com_meta_ocean_platform_android_application_GLRendererView_setFovX(JNIEnv* env, jobject javaThis, jdouble angle);

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
extern "C" jboolean Java_com_meta_ocean_platform_android_application_GLRendererView_setBackgroundColor(JNIEnv* env, jobject javaThis, jdouble red, jdouble green, jdouble blue);

/**
 * Java native interface function to register the instance function for the corresponding native C++ class.
 * @param env JNI environment
 * @param javaThis JNI object
 * @ingroup platformandroidapplicationjni
 */
extern "C" void Java_com_meta_ocean_platform_android_application_GLRendererView_registerInstance(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_GL_RENDERER_VIEW_H
