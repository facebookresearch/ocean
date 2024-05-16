/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/io/testio/android/TestIO.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testio/TestIO.h"

using namespace Ocean;

void Java_com_meta_ocean_app_test_io_testio_android_TestIOActivity_invokeTest(JNIEnv* env, jobject javaThis, jdouble testDuration, jstring testFunctions)
{
	ocean_assert(testDuration > 0.0);

	const std::string functions = Platform::Android::Utilities::toAString(env, testFunctions);

	Test::TestIO::testIOAsynchron(testDuration, functions);
}
