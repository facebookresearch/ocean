// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/test/math/testmath/android/TestMath.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testmath/TestMath.h"

using namespace Ocean;

void Java_com_facebook_ocean_app_test_math_testmath_android_TestMathActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const std::string functions = Platform::Android::Utilities::toAString(env, testFunctions);

	Test::TestMath::testMathAsynchron(testDuration, functions);
}
