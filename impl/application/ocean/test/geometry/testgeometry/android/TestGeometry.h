// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_TEST_GEOMETRY_TESTGEOMETRY_ANDROID_TEST_GEOMETRY_H
#define FACEBOOK_APPLICATION_OCEAN_TEST_GEOMETRY_TESTGEOMETRY_ANDROID_TEST_GEOMETRY_H

#include "application/ocean/test/geometry/ApplicationTestGeometry.h"

#include <jni.h>

/**
 * @ingroup applicationtestgeometry
 * @defgroup applicationtestgeometrytestgeometryandroid Geometry Test (Android)
 * @{
 * The test application validates the accuracy and measures the performance of the Geometry library.<br>
 * This application is platform dependent and is implemented for Android platforms.<br>
 * @}
 */

/**
 * Native interface function to invoke the geometry test.
 * @param env Native interface environment
 * @param javaThis JNI object
 * @param testDuration Duration of each test in seconds, with range (0, infinity)
 * @param testFunctions Test functions to be invoked
 * @ingroup applicationtestgeometrytestgeometryandroid
 */
extern "C" void Java_com_facebook_ocean_app_test_geometry_testgeometry_android_TestGeometryActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions);

#endif // FACEBOOK_APPLICATION_OCEAN_TEST_GEOMETRY_TESTGEOMETRY_ANDROID_TEST_GEOMETRY_H
