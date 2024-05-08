/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/application/jni/GLView.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/platform/android/application/GLView.h"

using namespace Ocean::Platform::Android;
using namespace Ocean::Platform::Android::Application;

jboolean Java_com_meta_ocean_platform_android_application_GLView_initialize(JNIEnv* env, jobject javaThisd)
{
	return GLView::get().initialize();
}

jboolean Java_com_meta_ocean_platform_android_application_GLView_release(JNIEnv* env, jobject javaThis)
{
	return GLView::get().release();
}

jboolean Java_com_meta_ocean_platform_android_application_GLView_resize(JNIEnv* env, jobject javaThis, jint width, jint height)
{
	return GLView::get().resize(width, height);
}

jboolean Java_com_meta_ocean_platform_android_application_GLView_render(JNIEnv* env, jobject javaThis)
{
	return GLView::get().render();
}

void Java_com_meta_ocean_platform_android_application_GLView_onTouchDown(JNIEnv* env, jobject javaThis, jfloat x, jfloat y)
{
	GLView::get().onTouchDown(x, y);
}

void Java_com_meta_ocean_platform_android_application_GLView_onTouchMove(JNIEnv* env, jobject javaThis, jfloat x, jfloat y)
{
	GLView::get().onTouchMove(x, y);
}

void Java_com_meta_ocean_platform_android_application_GLView_onTouchUp(JNIEnv* env, jobject javaThis, jfloat x, jfloat y)
{
	GLView::get().onTouchUp(x, y);
}

void Java_com_meta_ocean_platform_android_application_GLView_onActivityResume(JNIEnv* env, jobject javaThis)
{
	GLView::get().onResume();
}

void Java_com_meta_ocean_platform_android_application_GLView_onActivityPause(JNIEnv* env, jobject javaThis)
{
	GLView::get().onPause();
}

void Java_com_meta_ocean_platform_android_application_GLView_onActivityStop(JNIEnv* env, jobject javaThis)
{
	GLView::get().onStop();
}

void Java_com_meta_ocean_platform_android_application_GLView_onActivityDestroy(JNIEnv* env, jobject javaThis)
{
	GLView::get().onDestroy();
}

void Java_com_meta_ocean_platform_android_application_GLView_onPermissionGranted(JNIEnv* env, jobject javaThis, jstring permission)
{
	GLView::get().onPermissionGranted(Utilities::toAString(env, permission));
}

void Java_com_meta_ocean_platform_android_application_GLView_registerInstance(JNIEnv* env, jobject javaThis)
{
	GLView::registerInstanceFunction(GLView::createInstance, true /*isBaseClass*/);
}
