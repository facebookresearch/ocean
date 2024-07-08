/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/cv/testcv/testsegmentation/android/TestCVSegmentation.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testcv/testsegmentation/TestCVSegmentation.h"

using namespace Ocean;

void Java_com_meta_ocean_app_test_cv_testcv_testsegmentation_android_TestCVSegmentationActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jint testWidth, jint testHeight, jstring testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const std::string functions = Platform::Android::Utilities::toAString(env, testFunctions);

	Test::TestCV::TestSegmentation::testCVSegmentationAsynchron(testDuration, testWidth, testHeight, functions);
}
