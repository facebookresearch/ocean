// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/test/unifiedfeatures/testunifiedfeatures/android/TestUnifiedFeatures.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testunifiedfeatures/TestUnifiedFeatures.h"

using namespace Ocean;

void Java_com_facebook_ocean_app_test_unifiedfeatures_testunifiedfeatures_android_TestUnifiedFeaturesActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const std::string functions(Platform::Android::Utilities::toAString(env, testFunctions));

	Test::TestUnifiedFeatures::testAsynchron(testDuration, functions);
}
