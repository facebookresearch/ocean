/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_TEST_TESTMEDIA_ANDROID_TEST_MEDIA_H
#define FACEBOOK_APPLICATION_OCEAN_TEST_TESTMEDIA_ANDROID_TEST_MEDIA_H

#include "application/ocean/test/media/ApplicationTestMedia.h"

#include <jni.h>

/**
 * @ingroup applicationtestmedia
 * @defgroup applicationtestmediatestmediaandroid Media Test (Android)
 * @{
 * The test application validates the accuracy and measures the performance of the Media library.<br>
 * This application is platform dependent and is implemented for Android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the media test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testFunctions Test functions to be invoked
 * @ingroup applicationtestmediatestmediaandroid
 */
extern "C" void Java_com_meta_ocean_app_test_media_testmedia_android_TestMediaActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions);

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_TESTMEDIA_ANDROID_TEST_MEDIA_H
