// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_ANDROID_NATIVE_INTERFACE_MANAGER_H
#define META_OCEAN_PLATFORM_ANDROID_NATIVE_INTERFACE_MANAGER_H

#include "ocean/platform/android/Android.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include <map>

/**
 * The VM calls JNI_OnLoad when the native library is loaded.
 * @param vm Virtual machine object
 * @param reserved Reserved parameter
 * @return JNI version
 * @ingroup platformandroid
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved);

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * This class implements the manager of the java native interface as singleton.
 * @ingroup platformandroid
 */
class OCEAN_PLATFORM_ANDROID_EXPORT NativeInterfaceManager : public Singleton<NativeInterfaceManager>
{
	friend class Singleton<NativeInterfaceManager>;

	private:

		/**
		 * Definition of a map mapping thread ids to Java native interface environments.
		 */
		typedef std::map<pid_t, JNIEnv*> ThreadEnvironmentMap;

	public:

		/**
		 * Returns the JNI virtual machine object.
		 * @return The JNI virtual machine object
		 */
		JavaVM* virtualMachine();

		/**
		 * Returns the java native interface environment object for the current calling thread.
		 * @return JNI environment object.
		 */
		JNIEnv* environment();

		/**
		 * Returns the current activity.
		 * @return The current activity, may be nullptr if no activity is set
		 */
		jobject currentActivity();

		/**
		 * Sets the virtual machine object of this manager.
		 * @param virtualMachine Virtual machine to be set
		 * @return True, if succeeded
		 */
		bool setVirtualMachine(JavaVM* virtualMachine);

		/**
		 * Sets or changes the current activity.
		 * @param activity The current activity to be set or updated, can be nullptr to remove the previously set activity
		 */
		void setCurrentActivity(jobject activity);

	protected:

		/**
		 * Creates a new manger object.
		 * This object will be created indirectly by the singleton object.
		 * @see Singleton::get().
		 */
		NativeInterfaceManager();

		/**
		 * Destructs a manger object.
		 */
		virtual ~NativeInterfaceManager();

	protected:

		/// Manager lock.
		Lock lock_;

		/// JNI virtual machine object.
		JavaVM* virtualMachine_;

		/// The JNI object of the current activity with global reference, nullptr if not set.
		jobject currentActivity_;

		/// Map holding Java native environments individually for each thread.
		ThreadEnvironmentMap threadEnvironmentMap_;
};

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_NATIVE_INTERFACE_MANAGER_H
