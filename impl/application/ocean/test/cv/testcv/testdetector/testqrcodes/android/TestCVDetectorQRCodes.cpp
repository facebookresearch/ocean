// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/test/cv/testcv/testdetector/testqrcodes/android/TestCVDetectorQRCodes.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testcv/testdetector/testqrcodes/TestCVDetectorQRcodes.h"

using namespace Ocean;

void Java_com_facebook_ocean_app_test_cv_testcv_testdetector_testqrcodes_android_TestCVDetectorQRCodesActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const std::string functions = Platform::Android::Utilities::toAString(env, testFunctions);

	Test::TestCV::TestDetector::TestQRCodes::testCVDetectorQRCodesAsynchronous(testDuration, functions);
}
