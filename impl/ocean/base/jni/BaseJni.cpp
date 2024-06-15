/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/jni/BaseJni.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Processor.h"
#include "ocean/base/String.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/math/Math.h"

#include "ocean/platform/android/Battery.h"
#include "ocean/platform/android/NativeInterfaceManager.h"
#include "ocean/platform/android/Utilities.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_base_BaseJni_initialize(JNIEnv* env, jobject javaThis, jstring messageOutputType)
{
	return JNI::BaseJni::initialize(Platform::Android::Utilities::toAString(env, messageOutputType));
}

jboolean Java_com_meta_ocean_base_BaseJni_initializeWithMessageOutput(JNIEnv* env, jobject javaThis, jint messageOutputType, jstring outputFile)
{
	return JNI::BaseJni::initialize(Messenger::MessageOutput(messageOutputType), Platform::Android::Utilities::toAString(env, outputFile));
}

jboolean Java_com_meta_ocean_base_BaseJni_setCurrentActivity(JNIEnv* env, jobject javaThis, jobject activity)
{
	ocean_assert(env != nullptr);

	Platform::Android::NativeInterfaceManager::get().setCurrentActivity(activity);

	return true;
}

jboolean Java_com_meta_ocean_base_BaseJni_forceProcessorCoreNumber(JNIEnv* env, jobject javaThis, jint cores)
{
	return JNI::BaseJni::forceProcessorCoreNumber((unsigned int)cores);
}

jboolean Java_com_meta_ocean_base_BaseJni_setWorkerPoolCapacity(JNIEnv* env, jobject javaThis, jint capacity)
{
	return JNI::BaseJni::setWorkerPoolCapacity((unsigned int)capacity);
}

void Java_com_meta_ocean_base_BaseJni_debug(JNIEnv* env, jobject javaThis, jstring message)
{
	JNI::BaseJni::debug(Platform::Android::Utilities::toAString(env, message));
}

void Java_com_meta_ocean_base_BaseJni_information(JNIEnv* env, jobject javaThis, jstring message)
{
	JNI::BaseJni::information(Platform::Android::Utilities::toAString(env, message));
}

void Java_com_meta_ocean_base_BaseJni_warning(JNIEnv* env, jobject javaThis, jstring message)
{
	JNI::BaseJni::warning(Platform::Android::Utilities::toAString(env, message));
}

void Java_com_meta_ocean_base_BaseJni_error(JNIEnv* env, jobject javaThis, jstring message)
{
	JNI::BaseJni::error(Platform::Android::Utilities::toAString(env, message));
}

jstring Java_com_meta_ocean_base_BaseJni_popMessages(JNIEnv* env, jobject javaThis)
{
	return Platform::Android::Utilities::toJavaString(env, JNI::BaseJni::popMessages());
}

void Java_com_meta_ocean_base_BaseJni_exit(JNIEnv* env, jobject javaThis, jint exitValue)
{
	std::exit(exitValue);
}

namespace Ocean
{

namespace JNI
{

bool BaseJni::initialize(const std::string& messageOutputType)
{
	std::string outputFile;

	Messenger::MessageOutput messageOutput = Messenger::OUTPUT_DISCARDED;

	if (messageOutputType.empty())
	{
		messageOutput = Messenger::OUTPUT_STANDARD;
	}
	else
	{
		const std::vector<std::string> tokens = Utilities::separateValues(messageOutputType);

		for (const std::string& token : tokens)
		{
			if (token == "STANDARD")
			{
				messageOutput = Messenger::MessageOutput(messageOutput | Messenger::OUTPUT_STANDARD);
			}
			else if (token == "QUEUED")
			{
				messageOutput = Messenger::MessageOutput(messageOutput | Messenger::OUTPUT_QUEUED);
			}
			else if (token == "DISCARDED")
			{
				messageOutput = Messenger::OUTPUT_DISCARDED;
				outputFile.clear();
				break;
			}
			else if (token == "DEBUG_WINDOW")
			{
				messageOutput = Messenger::MessageOutput(messageOutput | Messenger::OUTPUT_DEBUG_WINDOW);
			}
			else
			{
				messageOutput = Messenger::MessageOutput(messageOutput | Messenger::OUTPUT_FILE);
				outputFile = token;
			}
		}
	}

	ocean_assert(messageOutput != Messenger::OUTPUT_DISCARDED || outputFile.empty());

	return initialize(messageOutput, outputFile);
}

bool BaseJni::initialize(const Messenger::MessageOutput messageOutputType, const std::string& outputFile)
{
	ocean_assert(outputFile.empty() || (messageOutputType & Messenger::OUTPUT_FILE) == Messenger::OUTPUT_FILE);

	if (!outputFile.empty())
	{
		Messenger::get().setFileOutput(outputFile);
	}

	Messenger::get().setOutputType(messageOutputType);

	Log::info() << "Build: " << Build::buildString();
	Log::info() << "Time: " << DateTime::localString();
	Log::info() << " ";
	Log::info() << "Floating point precision: " << sizeof(Scalar);
	Log::info() << " ";
	Log::info() << "Battery capacity: " << Platform::Android::Battery::currentCapacity() << "%";
	Log::info() << " ";

	return true;
}

bool BaseJni::forceProcessorCoreNumber(const unsigned int cores)
{
	if (cores >= 1u && cores <= 1024u)
	{
		return Processor::get().forceCores(cores);
	}

	return false;
}

bool BaseJni::setWorkerPoolCapacity(const unsigned int capacity)
{
	if (capacity >= 1u && capacity <= 1024u)
	{
		return WorkerPool::get().setCapacity(capacity);
	}

	return false;
}

void BaseJni::debug(const std::string& message)
{
	Ocean::Log::debug() << message;
}

void BaseJni::information(const std::string& message)
{
	Ocean::Log::info() << message;
}

void BaseJni::warning(const std::string& message)
{
	Ocean::Log::warning() << message;
}

void BaseJni::error(const std::string& message)
{
	Ocean::Log::error() << message;
}

std::string BaseJni::popMessages()
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
