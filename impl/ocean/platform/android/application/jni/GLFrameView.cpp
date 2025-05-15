/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/application/jni/GLFrameView.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/platform/android/application/GLFrameView.h"

using namespace Ocean;
using namespace Ocean::Platform::Android;
using namespace Ocean::Platform::Android::Application;

jboolean Java_com_meta_ocean_platform_android_application_GLFrameView_setFrameMedium(JNIEnv* env, jobject javaThis, jstring url, jstring type, jint preferredWidth, jint preferredHeight, jboolean adjustFov)
{
	const std::string urlValue(Platform::Android::Utilities::toAString(env, url));
	const std::string typeValue(String::toUpper(Platform::Android::Utilities::toAString(env, type)));

	return GLView::get<GLFrameView>().setBackgroundMedium(urlValue, typeValue, preferredWidth, preferredHeight, adjustFov);
}

void Java_com_meta_ocean_platform_android_application_GLFrameView_registerInstance(JNIEnv* env, jobject javaThis)
{
	GLView::registerInstanceFunction(GLFrameView::createInstance, true /*isBaseClass*/);
}
