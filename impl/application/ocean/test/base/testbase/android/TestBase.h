/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_TEST_BASE_TESTBASE_ANDROID_TEST_BASE_H
#define FACEBOOK_APPLICATION_OCEAN_TEST_BASE_TESTBASE_ANDROID_TEST_BASE_H

#include "application/ocean/test/base/ApplicationTestBase.h"

#include <jni.h>

/**
 * @ingroup applicationtestbase
 * @defgroup applicationtestbasetestbaseandroid Base Test (Android)
 * @{
 * The test application validates the accuracy and measures the performance of the Base library.<br>
 * This application is platform dependent and is implemented for Android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the base test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testFunctions Test functoins to be invoked
 * @ingroup applicationtestbasetestbaseandroid
 */
extern "C" void Java_com_meta_ocean_test_base_testbase_TestBaseActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions);

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_BASE_TESTBASE_ANDROID_TEST_BASE_H
