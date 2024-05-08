/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/media/testmedia/android/TestMedia.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testmedia/TestMedia.h"

using namespace Ocean;

void Java_com_meta_ocean_app_test_media_testmedia_android_TestMediaActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const std::string functions = Platform::Android::Utilities::toAString(env, testFunctions);

	Test::TestMedia::testMediaAsynchron(testDuration, functions);
}
