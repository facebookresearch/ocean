/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_GL_VIEW_H
#define META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_GL_VIEW_H

#include "ocean/platform/android/application/jni/JNI.h"

#include <jni.h>

/**
 * Java native interface function to initialize the application view.
 * @param env JNI environment
 * @param javaThis JNI object
 * @return True, if succeeded
 * @ingroup platformandroidapplicationjni
 */
extern "C" jboolean Java_com_meta_ocean_platform_android_application_GLView_initialize(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to release the application view.
 * @param env JNI environment
 * @param javaThis JNI object
 * @return True, if succeeded
 * @ingroup platformandroidapplicationjni
 */
extern "C" jboolean Java_com_meta_ocean_platform_android_application_GLView_release(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function resizing the framebuffer.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param width Horizontal window size position in pixel
 * @param height Vertical window size in pixel
 * @ingroup platformandroidapplicationjni
 */
extern "C" jboolean Java_com_meta_ocean_platform_android_application_GLView_resize(JNIEnv* env, jobject javaThis, jint width, jint height);

/**
 * Java native interface function rendering the next frame.
 * @param env JNI environment
 * @param javaThis JNI object
 * @return True, if succeeded
 * @ingroup platformandroidapplicationjni
 */
extern "C" jboolean Java_com_meta_ocean_platform_android_application_GLView_render(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function for touch down events.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param x Horizontal touch position in pixel
 * @param y Vertical touch position in pixel
 * @ingroup platformandroidapplicationjni
 */
extern "C" void Java_com_meta_ocean_platform_android_application_GLView_onTouchDown(JNIEnv* env, jobject javaThis, jfloat x, jfloat y);

/**
 * Java native interface function for (touch) move events.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param x Horizontal (touch) move position in pixel
 * @param y Vertical (touch) move position in pixel
 * @ingroup platformandroidapplicationjni
 */
extern "C" void Java_com_meta_ocean_platform_android_application_GLView_onTouchMove(JNIEnv* env, jobject javaThis, jfloat x, jfloat y);

/**
 * Java native interface function for touch up events.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param x Horizontal touch position in pixel
 * @param y Vertical touch position in pixel
 * @ingroup platformandroidapplicationjni
 */
extern "C" void Java_com_meta_ocean_platform_android_application_GLView_onTouchUp(JNIEnv* env, jobject javaThis, jfloat x, jfloat y);

/**
 * Java native interface function for resume events.
 * @param env JNI environment
 * @param javaThis JNI object
 * @ingroup platformandroidapplicationjni
 */
extern "C" void Java_com_meta_ocean_platform_android_application_GLView_onActivityResume(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function for pause events.
 * @param env JNI environment
 * @param javaThis JNI object
 * @ingroup platformandroidapplicationjni
 */
extern "C" void Java_com_meta_ocean_platform_android_application_GLView_onActivityPause(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function for stop events.
 * @param env JNI environment
 * @param javaThis JNI object
 * @ingroup platformandroidapplicationjni
 */
extern "C" void Java_com_meta_ocean_platform_android_application_GLView_onActivityStop(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function for destroy events.
 * @param env JNI environment
 * @param javaThis JNI object
 * @ingroup platformandroidapplicationjni
 */
extern "C" void Java_com_meta_ocean_platform_android_application_GLView_onActivityDestroy(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function for permission granted events.
 * @param env JNI environment
 * @param javaThis JNI object
 * @param permission The permission which has been granted
 * @ingroup platformandroidapplicationjni
 */
extern "C" void Java_com_meta_ocean_platform_android_application_GLView_onPermissionGranted(JNIEnv* env, jobject javaThis, jstring permission);

/**
 * Java native interface function to register the instance function for the corresponding native C++ class.
 * @param env JNI environment
 * @param javaThis JNI object
 * @ingroup platformandroidapplicationjni
 */
extern "C" void Java_com_meta_ocean_platform_android_application_GLView_registerInstance(JNIEnv* env, jobject javaThis);

#endif // META_OCEAN_PLATFORM_ANDROID_APPLICATION_JNI_GL_VIEW_H
