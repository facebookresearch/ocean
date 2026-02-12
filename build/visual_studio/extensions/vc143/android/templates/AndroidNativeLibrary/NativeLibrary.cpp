/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "NativeLibrary.h"

#include <android/log.h>

namespace $safeprojectname$
{

bool NativeLibrary::libraryIsInitialized_ = false;

std::string NativeLibrary::version()
{
	return "1.0.0";
}

int NativeLibrary::add(const int valueA, const int valueB)
{
	return valueA + valueB;
}

bool NativeLibrary::initialize()
{
	if (libraryIsInitialized_)
	{
		return true;
	}

	__android_log_print(ANDROID_LOG_INFO, "$safeprojectname$", "Initializing native library");

	libraryIsInitialized_ = true;

	return true;
}

bool NativeLibrary::shutdown()
{
	if (!libraryIsInitialized_)
	{
		return true;
	}

	__android_log_print(ANDROID_LOG_INFO, "$safeprojectname$", "Shutting down native library");

	libraryIsInitialized_ = false;

	return true;
}

}

/*
 * JNI Functions
 *
 * The following functions provide the JNI interface for Java/Kotlin code.
 * Update the package name prefix (com_example_app) to match your actual package structure.
 *
 * Example: For package "com.mycompany.myapp", use "com_mycompany_myapp"
 */

extern "C"
{

JNIEXPORT jstring JNICALL
Java_com_example_app_NativeLibrary_getVersion(JNIEnv* env, jobject /*thiz*/)
{
	const std::string version = $safeprojectname$::NativeLibrary::version();

	return env->NewStringUTF(version.c_str());
}

JNIEXPORT jint JNICALL
Java_com_example_app_NativeLibrary_add(JNIEnv* /*env*/, jobject /*thiz*/, jint valueA, jint valueB)
{
	return $safeprojectname$::NativeLibrary::add(valueA, valueB);
}

JNIEXPORT jboolean JNICALL
Java_com_example_app_NativeLibrary_initialize(JNIEnv* /*env*/, jobject /*thiz*/)
{
	return $safeprojectname$::NativeLibrary::initialize() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_example_app_NativeLibrary_shutdown(JNIEnv* /*env*/, jobject /*thiz*/)
{
	return $safeprojectname$::NativeLibrary::shutdown() ? JNI_TRUE : JNI_FALSE;
}

} // extern "C"
