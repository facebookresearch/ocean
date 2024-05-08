/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef NDK_APPLICATION_TEST_TESTCV_TESTDETECTOR_ANDROID_TEST_CV_DETECTOR_H
#define NDK_APPLICATION_TEST_TESTCV_TESTDETECTOR_ANDROID_TEST_CV_DETECTOR_H

#include "application/ocean/test/cv/ApplicationTestCV.h"

#include <jni.h>

/**
 * @ingroup applicationtestcv
 * @defgroup applicationtestcvtestcvdetectorandroid Computer Vision Detector Test (Android)
 * @{
 * The test application validates the accuracy and measures the performance of the Computer Vision Detector library.<br>
 * This application is platform dependent and is implemented for android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the Computer Vision Detector test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testFilename The filename of the image to be used for testing
 * @param testFunctions Test functions to be invoked
 * @ingroup applicationtestcvtestcvdetectorandroid
 */
extern "C" void Java_com_meta_ocean_app_test_cv_testcv_testdetector_android_TestCVDetectorActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFilename, jstring testFunctions);

#endif // NDK_APPLICATION_TEST_TESTCV_TESTDETECTOR_ANDROID_TEST_CV_DETECTOR_H
