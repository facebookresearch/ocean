/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/cv/testcv/testopencv/android/TestCVOpenCV.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testcv/testopencv/TestOpenCV.h"

using namespace Ocean;

void Java_com_meta_ocean_app_test_cv_testcv_testopencv_android_TestCVOpenCVActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring imageFilename, jstring testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const std::string filename(Platform::Android::Utilities::toAString(env, testFunctions));
	const std::string functions(Platform::Android::Utilities::toAString(env, testFunctions));

	Test::TestCV::TestOpenCV::testCVOpenCVAsynchron(testDuration, filename, functions);
}
