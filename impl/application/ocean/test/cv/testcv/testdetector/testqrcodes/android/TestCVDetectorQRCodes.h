/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "application/ocean/test/cv/ApplicationTestCV.h"

#include <jni.h>

/**
 * @ingroup applicationtestcvdetectorqrcodes
 * @defgroup applicationtestcvdetectorqrcodesandroid Computer Vision Detector QR Codes Test (Android)
 * @{
 * The test application validates the accuracy and measures the performance of the Computer Vision Detector QR Codes library.<br>
 * This application is platform dependent and is implemented for android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the Computer Vision Detector QR Codes test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testFunctions Test functions to be invoked
 * @ingroup applicationtestcvtestcvdetectorandroid
 */
extern "C" void Java_com_meta_ocean_app_test_cv_testcv_testdetector_testqrcodes_android_TestCVDetectorQRCodesActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions);
