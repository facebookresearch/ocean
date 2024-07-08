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

jboolean Java_com_meta_ocean_platform_android_application_GLFrameView_setFovX(JNIEnv* env, jobject javaThis, jdouble angle)
{
	try
	{
		Log::info() << "Setting the FovX to " << Numeric::rad2deg(angle);

		GLView::get<GLFrameView>().setFovX(angle);
	}
	catch (const std::exception& exception)
	{
		Log::error() << exception.what();
		return false;
	}
	catch (...)
	{
		Log::error() << "Uncaught exception occured!";
		return false;
	}

	return true;
}

jboolean Java_com_meta_ocean_platform_android_application_GLFrameView_setBackgroundColor(JNIEnv* env, jobject javaThis, jdouble red, jdouble green, jdouble blue)
{
	return GLView::get<GLFrameView>().setBackgroundColor(RGBAColor(float(red), float(green), float(blue)));
}

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
