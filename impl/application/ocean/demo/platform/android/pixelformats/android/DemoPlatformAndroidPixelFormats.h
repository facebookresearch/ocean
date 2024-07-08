/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef NDK_APPLICATION_DEMO_PLATFORM_ANDOIRD_DEMO_PLATFORM_ANDROID_PIXEL_FORMATS_H
#define NDK_APPLICATION_DEMO_PLATFORM_ANDOIRD_DEMO_PLATFORM_ANDROID_PIXEL_FORMATS_H

#include "application/ocean/demo/platform/ApplicationDemoPlatform.h"

#include <jni.h>

/**
 * @ingroup applicationdemoplatform
 * @defgroup applicationdemoplatformandroidpixelforamts Demo Platform Android (Android)
 * @{
 * The test application is a simple pixel format demo for Android.<br>
 * This application is platform dependent and is implemented for android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the test/demo function.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param bitmap The Java bitmap object in which the channel will be set
 * @param channel The index of the image channel to be set, with range [0, channel - 1]
 * @param value The value to be set
 * @ingroup applicationtestcvtestcvandroid
 */
extern "C" bool Java_com_meta_ocean_app_demo_platform_android_pixelformats_android_DemoPlatformAndroidPixelFormatsActivity_setChannelToValue(JNIEnv* env, jobject javaThis, jobject bitmap, jint channel, jint value);

#endif // NDK_APPLICATION_DEMO_PLATFORM_ANDOIRD_DEMO_PLATFORM_ANDROID_PIXEL_FORMATS_H
