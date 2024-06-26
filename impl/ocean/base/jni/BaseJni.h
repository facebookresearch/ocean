/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_JNI_BASE_JNI_H
#define META_OCEAN_BASE_JNI_BASE_JNI_H

#include "ocean/base/jni/JNI.h"

#include "ocean/base/Messenger.h"

#include <jni.h>

/**
 * Java native interface function to initialize the Ocean Framework.
 * Several individual message output types can be specified and combined:
 * - "OUTPUT_STANDARD": To write all messages to the standard output (e.g., std::cout on desktop platforms, or Android logcat on Android platforms).
 * - "OUTPUT_QUEUED": To queue all messages and to explicitly pop the messages later (e.g., to display messages in a debug window).
 * - "<filename>": To write all messages to a define file.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI Base object
 * @param messageOutputType The type of the message output to be used, empty to use 'OUTPUT_STANDARD'
 * @return True, if succeeded
 * @ingroup basejni
 */
extern "C" jboolean Java_com_meta_ocean_base_BaseJni_initialize(JNIEnv* env, jobject javaThis, jstring messageOutputType);

/**
 * Java native interface function to initialize the Ocean Framework.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI Base object
 * @param messageOutputType The type of the message output to be used.
 * @param outputFile The name of the file to which messages will be written, 'messageOutputType' must contain 'OUTPUT_FILE', empty otherwise
 * @return True, if succeeded
 * @ingroup basejni
 */
extern "C" jboolean Java_com_meta_ocean_base_BaseJni_initializeWithMessageOutput(JNIEnv* env, jobject javaThis, jint messageOutputType, jstring outputFile);

/**
 * Java native interface function to set the current activity.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI Base object
 * @param activity The current activity to be set or updated, can be nullptr to remove the previously set activity
 * @return True, if succeeded
 * @ingroup basejni
 */
extern "C" jboolean Java_com_meta_ocean_base_BaseJni_setCurrentActivity(JNIEnv* env, jobject javaThis, jobject activity);

/**
 * Forces a specific number of processor cores.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param cores CPU cores to be forced during initialization
 * @return True, if succeeded
 * @ingroup basejni
 */
extern "C" jboolean Java_com_meta_ocean_base_BaseJni_forceProcessorCoreNumber(JNIEnv* env, jobject javaThis, jint cores);

/**
 * Sets or changes the maximal capacity of the worker pool.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param capacity The maximal number of worker objects the worker pool may provide
 * @return True, if succeeded
 * @ingroup basejni
 */
extern "C" jboolean Java_com_meta_ocean_base_BaseJni_setWorkerPoolCapacity(JNIEnv* env, jobject javaThis, jint capacity);

/**
 * Java native interface function to forward a debug message to the framework.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param message The debug message to forward
 * @ingroup basejni
 * @see Java_com_meta_ocean_base_BaseJni_information().
 */
extern "C" void Java_com_meta_ocean_base_BaseJni_debug(JNIEnv* env, jobject javaThis, jstring message);

/**
 * Java native interface function to forward an information message to the framework.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param message The information message to forward
 * @ingroup basejni
 * @see Java_com_meta_ocean_NativeInterface_warning(), Java_com_meta_ocean_NativeInterface_error().
 */
extern "C" void Java_com_meta_ocean_base_BaseJni_information(JNIEnv* env, jobject javaThis, jstring message);

/**
 * Java native interface function to forward an information message to the framework.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param message The warning message to forward
 * @ingroup basejni
 * @see Java_com_meta_ocean_NativeInterface_information(), Java_com_meta_ocean_NativeInterface_error().
 */
extern "C" void Java_com_meta_ocean_base_BaseJni_warning(JNIEnv* env, jobject javaThis, jstring message);

/**
 * Java native interface function to forward an information message to the framework.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param message The error message to forward
 * @ingroup basejni
 * @see Java_com_meta_ocean_NativeInterface_information(), Java_com_meta_ocean_NativeInterface_warning().
 */
extern "C" void Java_com_meta_ocean_base_BaseJni_error(JNIEnv* env, jobject javaThis, jstring message);

/**
 * Pops all messages that a currently waiting in the message queue.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return The messages that have been popped.
 * @ingroup basejni
 */
extern "C" jstring Java_com_meta_ocean_base_BaseJni_popMessages(JNIEnv* env, jobject javaThis);

/**
 * Exits the application by invoking the exit() command.
 * Beware: Commonly Android decides whether an application will be terminated or not so that this function should be a workaround only.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param exitValue The exit value to be used
 * @ingroup basejni
 */
extern "C" void Java_com_meta_ocean_base_BaseJni_exit(JNIEnv* env, jobject javaThis, jint exitValue);

namespace Ocean
{

namespace JNI
{

/**
 * This class implements a base native interface.
 * @ingroup basejni
 */
class OCEAN_BASE_JNI_EXPORT BaseJni
{
	public:

		/**
		 * Initializes the Ocean framework.
		 * Several individual message output types can be specified and combined:
		 * - "STANDARD": To write all messages to the standard output (e.g., std::cout on desktop platforms, or Android logcat on Android platforms).
		 * - "QUEUED": To queue all messages and to explicitly pop the messages later (e.g., to display messages in a debug window).
		 * - <filename>: To write all messages to a define file.
		 * @param messageOutputType The type of the message output to be used, empty to use 'STANDARD'
		 * @return True, if succeeded
		 */
		static bool initialize(const std::string& messageOutputType);

		/**
		 * Initializes the Ocean framework.
		 * @param messageOutputType The type of the message output to be used.
 		 * @param outputFile The name of the file to which messages will be written, 'messageOutputType' must contain 'OUTPUT_FILE', empty otherwise
		 * @return True, if succeeded
		 */
		static bool initialize(const Messenger::MessageOutput messageOutputType, const std::string& outputFile);

		/**
		 * Forces a specific number of processor cores.
		 * @param cores CPU cores to be forced during initialization
		 * @return True, if succeeded
		 */
		static bool forceProcessorCoreNumber(const unsigned int cores);

		/**
		 * Sets or changes the maximal capacity of the worker pool.
		 * @param capacity The maximal number of worker objects the worker pool may provide
		 * @return True, if succeeded
		 */
		static bool setWorkerPoolCapacity(const unsigned int capacity);

		/**
		 * Java native interface function to forward a debug message to the framework.
		 * @param message The debug message to forward
		 */
		static void debug(const std::string& message);

		/**
		 * Java native interface function to forward an information message to the framework.
		 * @param message The information message to forward
		 */
		static void information(const std::string& message);

		/**
		 * Java native interface function to forward a warning message to the framework.
		 * @param message The warning message to forward
		 */
		static void warning(const std::string& message);

		/**
		 * Java native interface function to forward an error message to the framework.
		 * @param message The error message to forward
		 */
		static void error(const std::string& message);

		/**
		 * Pops all messages that a currently waiting in the message queue.
		 * @return The messages that have been popped.
		 */
		static std::string popMessages();
};

}

}

#endif // META_OCEAN_BASE_JNI_BASE_JNI_H
