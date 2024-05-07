// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef NDK_APPLICATION_TEST_TESTCV_ANDROID_MAIN_H
#define NDK_APPLICATION_TEST_TESTCV_ANDROID_MAIN_H

#include "application/ocean/test/cv/ApplicationTestCV.h"

#include <jni.h>

/**
 * @ingroup applicationtestcv
 * @defgroup applicationtestcvtestcvandroid Computer Vision Test (Android)
 * @{
 * The test application validates the accuracy and measures the performance of the Computer Vision library.<br>
 * This application is platform dependent and is implemented for android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the cv test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testWidth Width of the test frame in pixel, with range [32, infinity)
 * @param testHeight Height of the test frame in pixel, with range [32, infinity)
 * @param testFunctions Test functions to be invoked
 * @ingroup applicationtestcvtestcvandroid
 */
extern "C" void Java_com_facebook_ocean_app_test_cv_testcv_android_TestCVActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jint testWidth, jint testHeight, jstring testFunctions);

#endif // NDK_APPLICATION_TEST_TESTCV_ANDROID_MAIN_H
