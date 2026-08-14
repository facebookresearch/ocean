/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_NATIVE_INTERFACE_MANAGER_H
#define META_OCEAN_PLATFORM_ANDROID_NATIVE_INTERFACE_MANAGER_H

#include "ocean/platform/android/Android.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include <atomic>

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * This class implements the manager of the java native interface as singleton.
 * The manager holds the Java virtual machine and provides the JNI environment of any thread.
 * The manager can be used from any thread, threads are attached to the virtual machine on demand and are detached again when the thread ends.
 * Threads created by Java are attached already and are never detached by this manager.
 * The following code example shows the usage of this manager:
 * @code
 * using namespace Ocean::Platform::Android;
 *
 * // the virtual machine is set once, commonly in JNI_OnLoad()
 * jint JNI_OnLoad(JavaVM* virtualMachine, void* reserved)
 * {
 *     NativeInterfaceManager::get().setVirtualMachine(virtualMachine);
 *
 *     return JNI_VERSION_1_6;
 * }
 *
 * // afterwards, the environment can be accessed from any thread
 * void functionRunningOnAnyThread()
 * {
 *     JNIEnv* jniEnv = NativeInterfaceManager::get().environment();
 *
 *     if (jniEnv == nullptr)
 *     {
 *         return;
 *     }
 *
 *     // the environment must be used on this thread only, never hand it to another thread
 * }
 *
 * // within a JNI function the environment is provided already, and can simply be used or forwarded
 * extern "C" void Java_com_meta_ocean_Example_doSomething(JNIEnv* jniEnv, jobject javaThis)
 * {
 *     // 'jniEnv' is the environment of the calling thread, the manager would return the same environment
 *     ocean_assert(!NativeInterfaceManager::get().isValid() || jniEnv == NativeInterfaceManager::get().environment());
 * }
 * @endcode
 * @ingroup platformandroid
 */
class OCEAN_PLATFORM_ANDROID_EXPORT NativeInterfaceManager : public Singleton<NativeInterfaceManager>
{
	friend class Singleton<NativeInterfaceManager>;

	protected:

		/**
		 * This class implements the attachment of one individual thread to a Java virtual machine.
		 * The object is intended to be used as thread-local object only, so that the thread is detached when the thread ends.
		 * Threads created by Java are attached already and are never detached by this object.
		 */
		class ScopedThreadAttachment
		{
			public:

				/**
				 * Creates a new object and attaches the calling thread if the thread is not attached already.
				 * @param javaVM The Java virtual machine to which the calling thread will be attached
				 */
				explicit ScopedThreadAttachment(JavaVM& javaVM);

				/**
				 * Destructs this object and detaches the thread if this object has attached the thread.
				 */
				~ScopedThreadAttachment();

				/**
				 * Returns the JNI environment of the thread which has created this object.
				 * @param javaVM The Java virtual machine which has been provided when this object was created
				 * @return The JNI environment of the thread, nullptr if the thread could not be attached
				 */
				inline JNIEnv* jniEnv(JavaVM& javaVM) const;

			protected:

				/**
				 * Disabled copy constructor.
				 * @param scopedThreadAttachment Object which would be copied
				 */
				ScopedThreadAttachment(const ScopedThreadAttachment& scopedThreadAttachment) = delete;

				/**
				 * Disabled copy operator.
				 * @param scopedThreadAttachment Object which would be copied
				 * @return Reference to this object
				 */
				ScopedThreadAttachment& operator=(const ScopedThreadAttachment& scopedThreadAttachment) = delete;

			protected:

				/// The Java virtual machine to which this object has attached the thread, nullptr if this object has not attached the thread.
				JavaVM* attachedJavaVM_ = nullptr;

				/// The JNI environment of the thread, nullptr if the environment has not been determined yet.
				JNIEnv* jniEnv_ = nullptr;
		};

	public:

		/**
		 * Returns the JNI virtual machine object.
		 * @return The JNI virtual machine object, nullptr if not set
		 * @see isValid().
		 */
		JavaVM* virtualMachine() const;

		/**
		 * Returns the java native interface environment object for the current calling thread.
		 * The thread is attached to the virtual machine on demand, as a daemon thread, and is detached automatically when the thread ends.
		 * The environment is determined once per thread and is cached afterwards.
		 * Beware: The environment must be used on the calling thread only, it must never be stored or handed to another thread.
		 * Beware: Whenever other code detaches the thread, e.g., an explicit DetachCurrentThread() or a ScopedJNIEnvironment which has attached the thread itself, the environment becomes invalid.
		 * Beware: FindClass() resolves class names with the class loader of the Java code on the stack.
		 * A thread attached by this manager has no Java code on the stack, so that platform classes can be resolved but no classes of the application.
		 * Resolve application classes on a thread which has been called from Java, and keep the class as a global reference.
		 * @return JNI environment object, nullptr if the virtual machine is unknown or if the thread could not be attached
		 * @see isValid().
		 */
		JNIEnv* environment();

		/**
		 * Returns the current activity.
		 * Beware: The reference is owned by this manager, a concurrent setCurrentActivity() may release it.
		 * @return The current activity, may be nullptr if no activity is set
		 * @see setCurrentActivity().
		 */
		jobject currentActivity() const;

		/**
		 * Sets the virtual machine object of this manager.
		 * The virtual machine can be set once only, providing the same virtual machine again succeeds without any change.
		 * @param virtualMachine Virtual machine to be set, must be valid
		 * @return True, if succeeded; False, if a different virtual machine is set already
		 * @see isValid().
		 */
		bool setVirtualMachine(JavaVM* virtualMachine);

		/**
		 * Sets or changes the current activity.
		 * This manager creates and owns its own global reference, the caller keeps the ownership of the provided reference.
		 * Therefore, the reference can be a local reference as well as a global reference like ANativeActivity::clazz.
		 * @param activity The current activity to be set or updated, can be nullptr to remove the previously set activity
		 * @see currentActivity().
		 */
		void setCurrentActivity(jobject activity);

		/**
		 * Returns whether the virtual machine of this manager is set.
		 * @return True, if so
		 * @see setVirtualMachine().
		 */
		bool isValid() const;

	protected:

		/**
		 * Creates a new manager object.
		 * This object will be created indirectly by the singleton object.
		 * @see Singleton::get().
		 */
		NativeInterfaceManager();

		/**
		 * Destructs a manager object.
		 */
		virtual ~NativeInterfaceManager();

	protected:

		/// Manager lock, protecting the current activity only.
		mutable Lock lock_;

		/// JNI virtual machine object, atomic so that the environment can be determined without a lock.
		std::atomic<JavaVM*> virtualMachine_ = nullptr;

		/// The global reference to the JNI object of the current activity which is owned by this manager, nullptr if not set.
		jobject currentActivity_ = nullptr;
};

inline JNIEnv* NativeInterfaceManager::ScopedThreadAttachment::jniEnv(JavaVM& javaVM) const
{
#ifdef OCEAN_DEBUG
	{
		// the environment becomes invalid whenever other code detaches the thread, this cannot be detected in release builds
		JNIEnv* debugJniEnv = nullptr;
		javaVM.GetEnv((void**)(&debugJniEnv), JNI_VERSION_1_6);

		if (jniEnv_ != nullptr)
		{
			ocean_assert(debugJniEnv == jniEnv_ && "The thread has been detached by other code!");
		}
	}
#else
	OCEAN_SUPPRESS_UNUSED_WARNING(javaVM);
#endif // OCEAN_DEBUG

	return jniEnv_;
}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_NATIVE_INTERFACE_MANAGER_H
