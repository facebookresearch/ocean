/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_TEST_TRACKING_TESTTRACKING_ANDROID_TEST_TRACKING_H
#define FACEBOOK_APPLICATION_OCEAN_TEST_TRACKING_TESTTRACKING_ANDROID_TEST_TRACKING_H

#include "application/ocean/test/tracking/ApplicationTestTracking.h"

#include <jni.h>

/**
 * @ingroup applicationtesttracking
 * @defgroup applicationtesttrackingtesttrackingandroid Tracking Test (Android)
 * @{
 * The test application validates the accuracy and measures the performance of the Tracking library.<br>
 * This application is platform dependent and is implemented for Android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the tracking test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testFunctions Test functoins to be invoked
 * @ingroup applicationtesttrackingtesttrackingandroid
 */
extern "C" void Java_com_meta_ocean_app_test_tracking_testtracking_android_TestTrackingActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions);

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_TRACKING_TESTTRACKING_ANDROID_TEST_TRACKING_H
