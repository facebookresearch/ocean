// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

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
 * @param outputFilename Optional filename for the output information
 * @param testDuration Duration of each test in seconds
 * @ingroup applicationtestiotestioandroid
 */
extern "C" jstring Java_com_facebook_ocean_app_test_io_testio_android_TestIOActivity_invokeTest(JNIEnv* env, jobject javaThis, jstring outputFilename, jdouble testDuration);

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_TESTIO_ANDROID_TEST_IO_H
