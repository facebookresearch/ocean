/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_TEST_TESTCV_TESTOPENCV_ANDROID_TEST_OPEN_CV_H
#define FACEBOOK_APPLICATION_OCEAN_TEST_TESTCV_TESTOPENCV_ANDROID_TEST_OPEN_CV_H

#include "application/ocean/test/cv/ApplicationTestCV.h"

#include <jni.h>

/**
 * @ingroup applicationtestcv
 * @defgroup applicationtestcvtestopencvandroid OpenCV Test (Android)
 * @{
 * The test application measures the performance of the OpenCV 3rdparty library.<br>
 * This application is platform dependent and is implemented for Android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the math test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testImageFilename The filename of the test image
 * @param testFunctions Test functions to be invoked
 * @ingroup applicationtestmathtestmathandroid
 */
extern "C" void Java_com_meta_ocean_app_test_cv_testcv_testopencv_android_TestCVOpenCVActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testImageFilename, jstring testFunctions);

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_TESTCV_TESTOPENCV_ANDROID_TEST_OPEN_CV_H
