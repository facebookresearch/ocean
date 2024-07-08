/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_TEST_TESTCV_TESTLIBYUV_ANDROID_TEST_CV_LIBYUV_H
#define FACEBOOK_APPLICATION_OCEAN_TEST_TESTCV_TESTLIBYUV_ANDROID_TEST_CV_LIBYUV_H

#include "application/ocean/test/cv/ApplicationTestCV.h"

#include <jni.h>

/**
 * @ingroup applicationtestcv
 * @defgroup applicationtestcvtestlibyuvandroid libyuv Benchmark Test (Android)
 * @{
 * The test application measures the performance of the libyuv 3rdparty library.<br>
 * This application is platform dependent and is implemented for Android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the CV Libyuv benchmark test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param skipValidation True, to skip the validation and to apply just the benchmarking
 * @param testFunctions Test functions to be invoked
 * @ingroup applicationtestcvtestlibyuvandroid
 */
extern "C" void Java_com_meta_ocean_test_cv_testcv_testlibyuv_TestCVLibyuvActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jboolean skipValidation, jstring testFunctions);

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_TESTCV_TESTLIBYUV_ANDROID_TEST_CV_LIBYUV_H
