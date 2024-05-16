/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_TEST_TESTIO_ANDROID_TEST_IO_H
#define FACEBOOK_APPLICATION_OCEAN_TEST_TESTIO_ANDROID_TEST_IO_H

#include "application/ocean/test/io/ApplicationTestIO.h"

#include <jni.h>

/**
 * @ingroup applicationtestio
 * @defgroup applicationtestiotestioandroid IO Test (Android)
 * @{
 * The test application validates the accuracy and measures the performance of the IO library.<br>
 * This application is platform dependent and is implemented for android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the io test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testFunctions Test functions to be invoked
 * @ingroup applicationtestiotestioandroid
 */
extern "C" void Java_com_meta_ocean_app_test_io_testio_android_TestIOActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions);

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_TESTIO_ANDROID_TEST_IO_H
