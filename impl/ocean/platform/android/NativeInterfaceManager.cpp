/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/NativeInterfaceManager.h"

#ifdef __GNUC__
	#include <sys/syscall.h>
	#include <unistd.h>
#endif

/**
 * The VM calls JNI_OnLoad when the native library is loaded.
 * @param vm Virtual machine object
 * @param reserved Reserved parameter
 * @return JNI version
 * @ingroup platformandroid
 */
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
	Ocean::Log::info() << "JNI_OnLoad invoked.";
	Ocean::Platform::Android::NativeInterfaceManager::get().setVirtualMachine(vm);

	return JNI_VERSION_1_6;
}

namespace Ocean
{

namespace Platform
{

namespace Android
{

NativeInterfaceManager::NativeInterfaceManager()
{
	// nothing to do here
}

NativeInterfaceManager::~NativeInterfaceManager()
{
	// nothing to do here
}

JavaVM* NativeInterfaceManager::virtualMachine()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(virtualMachine_ != nullptr);

	return virtualMachine_;
}

JNIEnv* NativeInterfaceManager::environment()
{
	const ScopedLock scopedLock(lock_);

	if (!virtualMachine_)
	{
		return nullptr;
	}

	const pid_t threadId = syscall(__NR_gettid);

	const ThreadEnvironmentMap::const_iterator i = threadEnvironmentMap_.find(threadId);
	if (i != threadEnvironmentMap_.end())
	{
		return i->second;
	}

	JNIEnv* environment = nullptr;
	if (virtualMachine_->GetEnv((void**)&environment, JNI_VERSION_1_6) == JNI_EDETACHED)
	{
		if (virtualMachine_->AttachCurrentThread(&environment, nullptr) < 0)
		{
			Log::error() << "Failed to attach the environment to the current thread!";
		}
		else
		{
			Log::debug() << "Attached the environment to the current thread.";
		}
	}

	if (environment)
	{
		threadEnvironmentMap_.insert(std::make_pair(threadId, environment));
	}
	else
	{
		Log::error() << "Failed to receive enviornment!";
	}

	return environment;
}

jobject NativeInterfaceManager::currentActivity()
{
	const ScopedLock scopedLock(lock_);

	return currentActivity_;
}

bool NativeInterfaceManager::setVirtualMachine(JavaVM* virtualMachine)
{
	ocean_assert(virtualMachine_ == nullptr);

	const ScopedLock scopedLock(lock_);

	if (!virtualMachine_)
	{
#ifdef OCEAN_DEBUG
		Log::info() << "Virtual machine assigned.";
#endif

		virtualMachine_ = virtualMachine;

		return true;
	}

	return false;
}

void NativeInterfaceManager::setCurrentActivity(jobject activity)
{
	const ScopedLock scopedLock(lock_);

	JNIEnv* env = environment();
	ocean_assert(env != nullptr);

	currentActivity_ = ScopedJObject(*env, activity);

	currentActivity_.makeGlobal();
}

}

}

}
