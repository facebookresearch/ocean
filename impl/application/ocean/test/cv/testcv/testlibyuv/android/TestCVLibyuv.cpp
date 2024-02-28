// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/test/cv/testcv/testlibyuv/android/TestCVLibyuv.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testcv/testlibyuv/TestLibyuv.h"

using namespace Ocean;

void Java_com_facebook_ocean_test_cv_testcv_testlibyuv_TestCVLibyuvActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jboolean skipValidation, jstring testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const std::string functions(Platform::Android::Utilities::toAString(env, testFunctions));

	Test::TestCV::TestLibyuv::testCVLibyuvAsynchron(testDuration, skipValidation, functions);
}
