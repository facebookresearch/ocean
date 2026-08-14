/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/NativeInterfaceManager.h"
#include "ocean/platform/android/Utilities.h"

/**
 * The VM calls JNI_OnLoad when the native library is loaded.
 * @param vm Virtual machine object
 * @param reserved Reserved parameter
 * @return JNI version
 * @ingroup platformandroid
 */
JNIEXPORT __attribute__((weak)) jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
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

NativeInterfaceManager::ScopedThreadAttachment::ScopedThreadAttachment(JavaVM& javaVM)
{
	JNIEnv* jniEnv = nullptr;

	const jint result = javaVM.GetEnv((void**)(&jniEnv), JNI_VERSION_1_6);

	if (result == JNI_OK)
	{
		// the thread is attached already, e.g., a thread created by Java, this object must not detach such a thread

		jniEnv_ = jniEnv;

		return;
	}

	if (result != JNI_EDETACHED)
	{
		Log::error() << "Failed to determine the JNI environment, GetEnv() returned " << result << "!";
		return;
	}

	// the thread is a thread owned by Ocean, Java does not know this thread and does not control its lifetime,
	// a daemon thread is not awaited when the virtual machine is destroyed, so that such a thread can never block the shutdown of the runtime

	if (javaVM.AttachCurrentThreadAsDaemon(&jniEnv, nullptr) != JNI_OK)
	{
		Log::error() << "Failed to attach the current thread to the Java virtual machine!";
		return;
	}

	attachedJavaVM_ = &javaVM;
	jniEnv_ = jniEnv;
}

NativeInterfaceManager::ScopedThreadAttachment::~ScopedThreadAttachment()
{
	if (attachedJavaVM_ == nullptr)
	{
		return;
	}

	// the thread may have been detached by other code in the meantime, so that this object must not detach the thread a second time,
	// 'GetEnv()' reads thread-local state of the runtime while 'DetachCurrentThread()' would modify global state which may be torn down already

	JNIEnv* jniEnv = nullptr;

	if (attachedJavaVM_->GetEnv((void**)(&jniEnv), JNI_VERSION_1_6) == JNI_OK)
	{
		// a Java exception which is still pending is handed to Thread.dispatchUncaughtException while the thread detaches, which terminates the process

		Utilities::clearPotentialException(*jniEnv);

		attachedJavaVM_->DetachCurrentThread();
	}

	attachedJavaVM_ = nullptr;
	jniEnv_ = nullptr;
}

NativeInterfaceManager::NativeInterfaceManager()
{
	// nothing to do here
}

NativeInterfaceManager::~NativeInterfaceManager()
{
	// this destructor is invoked during static de-initialization, on an arbitrary thread which may be detached already,
	// therefore no JNI function must be invoked here, the global reference of the activity is released together with the process
}

JavaVM* NativeInterfaceManager::virtualMachine() const
{
	return virtualMachine_.load(std::memory_order_acquire);
}

JNIEnv* NativeInterfaceManager::environment()
{
	JavaVM* javaVM = virtualMachine();

	if (javaVM == nullptr)
	{
		return nullptr;
	}

	// the object attaches the thread on demand and detaches the thread when the thread ends

	static thread_local ScopedThreadAttachment scopedThreadAttachment(*javaVM);

	return scopedThreadAttachment.jniEnv(*javaVM);
}

jobject NativeInterfaceManager::currentActivity() const
{
	const ScopedLock scopedLock(lock_);

	return currentActivity_;
}

bool NativeInterfaceManager::setVirtualMachine(JavaVM* virtualMachine)
{
	ocean_assert(virtualMachine != nullptr);

	if (virtualMachine == nullptr)
	{
		return false;
	}

	// the virtual machine is set once, the first caller wins

	JavaVM* expectedVirtualMachine = nullptr;

	if (!virtualMachine_.compare_exchange_strong(expectedVirtualMachine, virtualMachine, std::memory_order_release, std::memory_order_acquire))
	{
		// Ocean's JNI_OnLoad() is a weak symbol, so that the same virtual machine may be provided several times, e.g., by several libraries

		ocean_assert(expectedVirtualMachine == virtualMachine && "A different virtual machine is set already!");

		return expectedVirtualMachine == virtualMachine;
	}

#ifdef OCEAN_DEBUG
	Log::info() << "Virtual machine assigned.";
#endif

	return true;
}

void NativeInterfaceManager::setCurrentActivity(jobject activity)
{
	JNIEnv* jniEnv = environment();

	if (jniEnv == nullptr)
	{
		Log::error() << "Failed to set the current activity, the JNI environment is unknown!";

		return;
	}

	jobject newActivity = nullptr;

	if (activity != nullptr)
	{
		// the caller keeps the ownership of 'activity', e.g., ANativeActivity owns the reference it provides via 'ANativeActivity::clazz'

		newActivity = jniEnv->NewGlobalRef(activity);

		if (newActivity == nullptr)
		{
			// NewGlobalRef() throws an OutOfMemoryError, the exception must not stay pending as the calling thread may never return to Java

			if (jniEnv->ExceptionCheck() == JNI_TRUE)
			{
				jniEnv->ExceptionClear();
			}

			Log::error() << "Failed to create a global reference for the current activity!";

			return;
		}
	}

	TemporaryScopedLock scopedLock(lock_);

		const jobject previousActivity = currentActivity_;
		currentActivity_ = newActivity;

	scopedLock.release();

	if (previousActivity != nullptr)
	{
		jniEnv->DeleteGlobalRef(previousActivity);
	}
}

bool NativeInterfaceManager::isValid() const
{
	return virtualMachine() != nullptr;
}

}

}

}
