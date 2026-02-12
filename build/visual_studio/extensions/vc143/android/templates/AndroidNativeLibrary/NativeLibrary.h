/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <jni.h>

#include <string>

namespace $safeprojectname$
{

/**
 * This class implements the main native library interface.
 * The class demonstrates how to combine a C++ class with extern "C" JNI functions.
 * The class provides the actual implementation, while the JNI functions (declared below) serve as the bridge between Java/Kotlin code and this C++ implementation.
 * @ingroup $safeprojectname$
 */
class NativeLibrary
{
	public:

		/**
		 * Returns the library version string.
		 * @return The version string in semantic versioning format (e.g., "1.0.0")
		 */
		static std::string version();

		/**
		 * Adds two integer values.
		 * @param valueA The first integer value
		 * @param valueB The second integer value
		 * @return The sum of both values
		 */
		static int add(const int valueA, const int valueB);

		/**
		 * Initializes the native library.
		 * This function must be called before using any other library functions.
		 * @return True, if the initialization succeeded
		 */
		static bool initialize();

		/**
		 * Releases all resources and shuts down the native library.
		 * @return True, if the shutdown succeeded
		 */
		static bool shutdown();

	protected:

		/// True, if the library has been initialized.
		static bool libraryIsInitialized_;
};

}

/*
 * JNI Function Declarations
 *
 * The following functions provide the JNI interface for Java/Kotlin code.
 * Update the package name prefix (com_example_app) to match your actual package structure.
 *
 * Example: For package "com.mycompany.myapp", use "com_mycompany_myapp"
 */

extern "C"
{

/**
 * Returns the library version string.
 * @param env The JNI environment, must be valid
 * @param thiz The calling Java object
 * @return The version string as a Java string
 */
JNIEXPORT jstring JNICALL
Java_com_example_app_NativeLibrary_getVersion(JNIEnv* env, jobject thiz);

/**
 * Adds two integer values.
 * @param env The JNI environment, must be valid
 * @param thiz The calling Java object
 * @param valueA The first integer value
 * @param valueB The second integer value
 * @return The sum of both values
 */
JNIEXPORT jint JNICALL
Java_com_example_app_NativeLibrary_add(JNIEnv* env, jobject thiz, jint valueA, jint valueB);

/**
 * Initializes the native library.
 * @param env The JNI environment, must be valid
 * @param thiz The calling Java object
 * @return JNI_TRUE if initialization succeeded, JNI_FALSE otherwise
 */
JNIEXPORT jboolean JNICALL
Java_com_example_app_NativeLibrary_initialize(JNIEnv* env, jobject thiz);

/**
 * Shuts down the native library and releases resources.
 * @param env The JNI environment, must be valid
 * @param thiz The calling Java object
 * @return JNI_TRUE if shutdown succeeded, JNI_FALSE otherwise
 */
JNIEXPORT jboolean JNICALL
Java_com_example_app_NativeLibrary_shutdown(JNIEnv* env, jobject thiz);

} // extern "C"
