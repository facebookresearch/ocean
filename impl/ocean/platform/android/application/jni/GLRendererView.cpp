/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/application/jni/GLRendererView.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/platform/android/application/GLRendererView.h"

using namespace Ocean;
using namespace Ocean::Platform::Android;
using namespace Ocean::Platform::Android::Application;

jboolean Java_com_meta_ocean_platform_android_application_GLRendererView_setFovX(JNIEnv* env, jobject javaThis, jdouble angle)
{
	try
	{
		Log::info() << "Setting the FovX to " << Numeric::rad2deg(angle);

		GLView::get<GLRendererView>().setFovX(angle);
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

jboolean Java_com_meta_ocean_platform_android_application_GLRendererView_setBackgroundColor(JNIEnv* env, jobject javaThis, jdouble red, jdouble green, jdouble blue)
{
	return GLView::get<GLRendererView>().setBackgroundColor(RGBAColor(float(red), float(green), float(blue)));
}

void Java_com_meta_ocean_platform_android_application_GLRendererView_registerInstance(JNIEnv* env, jobject javaThis)
{
	GLView::registerInstanceFunction(GLRendererView::createInstance, true /*isBaseClass*/);
}
