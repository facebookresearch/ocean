/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_APPLICATION_OCEAN_TEST_TESTNETWORK_ANDROID_TEST_NETWORK_H
#define META_APPLICATION_OCEAN_TEST_TESTNETWORK_ANDROID_TEST_NETWORK_H

#include "application/ocean/test/network/ApplicationTestNetwork.h"

#include <jni.h>

/**
 * @ingroup applicationtestnetwork
 * @defgroup applicationtestnetworktestnetworkandroid Network Test (Android)
 * @{
 * The test application validates the accuracy and measures the performance of the Network library.<br>
 * This application is platform dependent and is implemented for Android platforms.
 * @}
 */

/**
 * Native interface function to invoke the network test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testFunctions Test functions to be invoked
 * @ingroup applicationtestnetworktestnetworkandroid
 */
extern "C" void Java_com_meta_ocean_app_test_network_testnetwork_android_TestNetworkActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions);

#endif // META_APPLICATION_OCEAN_TEST_TESTNETWORK_ANDROID_TEST_NETWORK_H
