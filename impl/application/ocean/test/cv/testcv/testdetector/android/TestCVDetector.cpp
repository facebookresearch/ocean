/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/cv/testcv/testdetector/android/TestCVDetector.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

using namespace Ocean;

void Java_com_meta_ocean_app_test_cv_testcv_testdetector_android_TestCVDetectorActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testImageFilename, jstring testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const std::string imageFilename = Platform::Android::Utilities::toAString(env, testImageFilename);
	const std::string functions = Platform::Android::Utilities::toAString(env, testFunctions);

	Test::TestCV::TestDetector::testCVDetectorAsynchron(testDuration, imageFilename, functions);
}
