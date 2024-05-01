// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_BASE_JNI_BASEJNI_H
#define META_OCEAN_BASE_JNI_BASEJNI_H

#include "ocean/base/jni/JNI.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Processor.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/math/Math.h"

#include "ocean/platform/android/Battery.h"

/**
 * Java native interface function to initialize the Ocean Framework.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI Base object
 * @param messageOutputFile Output file of all messages, otherwise all messages will be queued
 * @return True, if succeeded
 * @ingroup basejni
 */
extern "C" jboolean Java_com_facebook_ocean_base_BaseJni_initialize(JNIEnv* env, jobject javaThis, jstring messageOutputFile);

/**
 * Java native interface function to initialize the Ocean Framework.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI Base object
 * @param messageOutput The message output type to be used, can be a combintation of any enum value from 'MessageOutput'
 * @param messageOutputFile The explicit filename of the file to which the messages will be written, relevant if messageOutput contains OUTPUT_FILE
 * @return True, if succeeded
 * @ingroup basejni
 */
extern "C" jboolean Java_com_facebook_ocean_base_BaseJni_initializeWithMessageOutput(JNIEnv* env, jobject javaThis, jint messageOutput, jstring messageOutputFile);

/**
 * Java native interface function to set the current activity.
 * @param env The JNI environment, must be valid
 * @param javaThis The JNI Base object
 * @param activity The current activity to be set or updated, can be nullptr to remove the previously set activity
 * @return True, if succeeded
 * @ingroup basejni
 */
extern "C" jboolean Java_com_facebook_ocean_base_BaseJni_setCurrentActivity(JNIEnv* env, jobject javaThis, jobject activity);

/**
 * Forces a specific number of processor cores.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param cores CPU cores to be forced during initialization
 * @return True, if succeeded
 * @ingroup basejni
 */
extern "C" jboolean Java_com_facebook_ocean_base_BaseJni_forceProcessorCoreNumber(JNIEnv* env, jobject javaThis, jint cores);

/**
 * Sets or changes the maximal capacity of the worker pool.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param capacity The maximal number of worker objects the worker pool may provide
 * @return True, if succeeded
 * @ingroup basejni
 */
extern "C" jboolean Java_com_facebook_ocean_base_BaseJni_setWorkerPoolCapacity(JNIEnv* env, jobject javaThis, jint capacity);

/**
 * Java native interface function to forward an information message to the framework.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param message The information message to forward
 * @ingroup basejni
 * @see Java_com_facebook_ocean_NativeInterface_warning(), Java_com_facebook_ocean_NativeInterface_error().
 */
extern "C" void Java_com_facebook_ocean_base_BaseJni_information(JNIEnv* env, jobject javaThis, jstring message);

/**
 * Java native interface function to forward an information message to the framework.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param message The warning message to forward
 * @ingroup basejni
 * @see Java_com_facebook_ocean_NativeInterface_information(), Java_com_facebook_ocean_NativeInterface_error().
 */
extern "C" void Java_com_facebook_ocean_base_BaseJni_warning(JNIEnv* env, jobject javaThis, jstring message);

/**
 * Java native interface function to forward an information message to the framework.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param message The error message to forward
 * @ingroup basejni
 * @see Java_com_facebook_ocean_NativeInterface_information(), Java_com_facebook_ocean_NativeInterface_warning().
 */
extern "C" void Java_com_facebook_ocean_base_BaseJni_error(JNIEnv* env, jobject javaThis, jstring message);

/**
 * Pops all messages that a currently waiting in the message queue.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return The messages that have been popped.
 * @ingroup basejni
 */
extern "C" jstring Java_com_facebook_ocean_base_BaseJni_popMessages(JNIEnv* env, jobject javaThis);

/**
 * Exits the application by invoking the exit() command.
 * Beware: Commonly Android decides whether an application will be terminated or not so that this function should be a workaround only.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param exitValue The exit value to be used
 * @ingroup basejni
 */
extern "C" void Java_com_facebook_ocean_base_BaseJni_exit(JNIEnv* env, jobject javaThis, jint exitValue);

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
		 * @param outputFile Message output file or device
		 * @return True, if succeeded
		 */
		static inline bool initialize(const std::string& outputFile);

		/**
		 * Initializes the Ocean framework.
		 * @param messageOutput The message output to be used
 		 * @param outputFile The explicit filename of the file to which the messages will be written, relevant if messageOutput contains OUTPUT_FILE
		 * @return True, if succeeded
		 */
		static inline bool initialize(const Messenger::MessageOutput messageOutput, const std::string& outputFile);

		/**
		 * Forces a specific number of processor cores.
		 * @param cores CPU cores to be forced during initialization
		 * @return True, if succeeded
		 */
		static inline bool forceProcessorCoreNumber(const unsigned int cores);

		/**
		 * Sets or changes the maximal capacity of the worker pool.
		 * @param capacity The maximal number of worker objects the worker pool may provide
		 * @return True, if succeeded
		 */
		static inline bool setWorkerPoolCapacity(const unsigned int capacity);

		/**
		 * Java native interface function to forward an information message to the framework.
		 * @param message The information message to forward
		 */
		static inline void information(const std::string& message);

		/**
		 * Java native interface function to forward a warning message to the framework.
		 * @param message The warning message to forward
		 */
		static inline void warning(const std::string& message);

		/**
		 * Java native interface function to forward an error message to the framework.
		 * @param message The error message to forward
		 */
		static inline void error(const std::string& message);

		/**
		 * Pops all messages that a currently waiting in the message queue.
		 * @return The messages that have been popped.
		 */
		static inline std::string popMessages();
};

inline bool BaseJni::initialize(const std::string& outputFile)
{
	if (outputFile.empty())
	{
		return initialize(Messenger::OUTPUT_QUEUED, std::string());
	}
	else
	{
		if (String::toUpper(outputFile) == std::string("STANDARD"))
		{
			return initialize(Messenger::OUTPUT_STANDARD, std::string());
		}
		else
		{
			return initialize(Messenger::OUTPUT_FILE, outputFile);
		}
	}
}

inline bool BaseJni::initialize(const Messenger::MessageOutput messageOutput, const std::string& outputFile)
{
	ocean_assert((messageOutput & Messenger::OUTPUT_FILE) != Messenger::OUTPUT_FILE || !outputFile.empty());

	if (!outputFile.empty())
	{
		Messenger::get().setFileOutput(outputFile);
	}

	Messenger::get().setOutputType(messageOutput);

	Log::info() << "Build: " << Build::buildString();
	Log::info() << "Time: " << DateTime::localString();
	Log::info() << " ";
	Log::info() << "Floating point precision: " << sizeof(Scalar);
	Log::info() << " ";
	Log::info() << "Battery capacity: " << Platform::Android::Battery::currentCapacity() << "%";
	Log::info() << " ";

	return true;
}

inline bool BaseJni::forceProcessorCoreNumber(const unsigned int cores)
{
	if (cores >= 1u && cores <= 1024u)
	{
		return Processor::get().forceCores(cores);
	}

	return false;
}

inline bool BaseJni::setWorkerPoolCapacity(const unsigned int capacity)
{
	if (capacity >= 1u && capacity <= 1024u)
	{
		return WorkerPool::get().setCapacity(capacity);
	}

	return false;
}

inline void BaseJni::information(const std::string& message)
{
	Ocean::Log::info() << message;
}

inline void BaseJni::warning(const std::string& message)
{
	Ocean::Log::warning() << message;
}

inline void BaseJni::error(const std::string& message)
{
	Ocean::Log::error() << message;
}

inline std::string BaseJni::popMessages()
{
	std::string result;

	std::string message;
	bool isNew = false;

	while (true)
	{
		message = Messenger::get().popMessage(Messenger::TYPE_UNDEFINED, &isNew);

		if (message.empty())
		{
			break;
		}

		if (!result.empty())
		{
			result += std::string("\n");
		}

		result += message;
	}

	return result;
}

}

}

#endif // META_OCEAN_BASE_JNI_BASEJNI_H
