// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/test/cv/testcv/testadvanced/android/TestCVAdvanced.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

using namespace Ocean;

void Java_com_meta_ocean_app_test_cv_testcv_testadvanced_android_TestCVAdvancedActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jint testWidth, jint testHeight, jstring testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const std::string functions = Platform::Android::Utilities::toAString(env, testFunctions);

	Test::TestCV::TestAdvanced::testCVAdvancedAsynchron(testDuration, testWidth, testHeight, functions);
}
