// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/test/cv/testcv/android/TestCV.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testcv/TestCV.h"

using namespace Ocean;

void Java_com_facebook_ocean_app_test_cv_testcv_android_TestCVActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jint testWidth, jint testHeight, jstring testFunctions)
{
	ocean_assert(testWidth >= 32 && testHeight >= 32);
	ocean_assert(testDuration > 0.0);

	const std::string functions = Platform::Android::Utilities::toAString(env, testFunctions);

	Test::TestCV::testCVAsynchron(testDuration, testWidth, testHeight, functions);
}
