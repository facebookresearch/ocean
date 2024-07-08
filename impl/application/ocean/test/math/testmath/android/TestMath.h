/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_TEST_TESTMATH_ANDROID_TEST_MATH_H
#define FACEBOOK_APPLICATION_OCEAN_TEST_TESTMATH_ANDROID_TEST_MATH_H

#include "application/ocean/test/math/ApplicationTestMath.h"

#include <jni.h>

/**
 * @ingroup applicationtestmath
 * @defgroup applicationtestmathtestmathandroid Math Test (Android)
 * @{
 * The test application validates the accuracy and measures the performance of the Math library.<br>
 * This application is platform dependent and is implemented for Android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the math test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testFunctions Test functions to be invoked
 * @ingroup applicationtestmathtestmathandroid
 */
extern "C" void Java_com_meta_ocean_app_test_math_testmath_android_TestMathActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions);

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_TESTMATH_ANDROID_TEST_MATH_H
