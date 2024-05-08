// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/base/jni/BaseJni.h"

#include "ocean/platform/android/NativeInterfaceManager.h"
#include "ocean/platform/android/Utilities.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_base_BaseJni_initialize(JNIEnv* env, jobject javaThis, jstring messageOutputFile)
{
	return JNI::BaseJni::initialize(Platform::Android::Utilities::toAString(env, messageOutputFile));
}

jboolean Java_com_meta_ocean_base_BaseJni_initializeWithMessageOutput(JNIEnv* env, jobject javaThis, jint messageOutput, jstring messageOutputFile)
{
	return JNI::BaseJni::initialize(Messenger::MessageOutput(messageOutput), Platform::Android::Utilities::toAString(env, messageOutputFile));
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
