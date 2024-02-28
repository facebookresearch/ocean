// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "application/ocean/test/unifiedfeatures/ApplicationTestUnifiedFeatures.h"

#include <jni.h>

/**
 * @ingroup applicationtestunifiedfeatures
 * @defgroup applicationtestunifiedfeaturestestunifiedfeaturesandroid Unified Features Test (Android)
 * @{
 * The test application validates the accuracy and measures the performance of the Unified Features library.<br>
 * This application is platform dependent and is implemented for Android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the unified features test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testFunctions Test functoins to be invoked
 * @ingroup applicationtestunifiedfeaturestestunifiedfeaturesandroid
 */
extern "C" void Java_com_facebook_ocean_app_test_unifiedfeatures_testunifiedfeatures_android_TestUnifiedFeaturesActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions);

