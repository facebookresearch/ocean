/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_SCOPED_JNI_GLOBAL_OBJECT_H
#define META_OCEAN_PLATFORM_ANDROID_SCOPED_JNI_GLOBAL_OBJECT_H

#include "ocean/platform/android/Android.h"
#include "ocean/platform/android/ScopedJNIEnvironment.h"
#include "ocean/platform/android/ScopedJNILocalObject.h"

#include "ocean/base/Messenger.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

// Forward declaration.
template <typename T> class ScopedJNIGlobalObject;

/**
 * Definition of a scoped object encapsulating a global reference to a jclass object.
 * @see ScopedJNIGlobalObject
 * @ingroup platformandroid
 */
using ScopedGlobalJClass = ScopedJNIGlobalObject<jclass>;

/**
 * Definition of a scoped object encapsulating a global reference to a jobject object.
 * @see ScopedJNIGlobalObject
 * @ingroup platformandroid
 */
using ScopedGlobalJObject = ScopedJNIGlobalObject<jobject>;

/**
 * Definition of a scoped object encapsulating a global reference to a jobjectArray object.
 * @see ScopedJNIGlobalObject
 * @ingroup platformandroid
 */
using ScopedGlobalJObjectArray = ScopedJNIGlobalObject<jobjectArray>;

/**
 * Definition of a scoped object encapsulating a global reference to a jstring object.
 * @see ScopedJNIGlobalObject
 * @ingroup platformandroid
 */
using ScopedGlobalJString = ScopedJNIGlobalObject<jstring>;

/**
 * This class implements a scoped global reference to a JNI object.
 * A global reference can be used on any thread and stays valid until it is released explicitly,
 * therefore this object is the correct choice whenever a reference needs to be stored as a member.
 * The object always creates and owns its own global reference.
 * A local reference can be handed over, in which case it is deleted as soon as the global reference exists, any other reference stays owned by the caller.
 * The following code example shows the intended usage of this object:
 * @code
 * class MyClass
 * {
 *     public:
 *
 *         bool initialize(JNIEnv& jniEnv);
 *
 *     protected:
 *
 *         // the global reference can be used on any thread, and is released when this object is disposed
 *         ScopedGlobalJClass javaClass_;
 * };
 *
 * bool MyClass::initialize(JNIEnv& jniEnv)
 * {
 *     ScopedJClass javaClass(jniEnv, jniEnv.FindClass("com/meta/ocean/Example"));
 *
 *     if (!javaClass)
 *     {
 *         return false;
 *     }
 *
 *     // the local reference is handed over, on success it is deleted and only the global reference remains
 *     javaClass_ = ScopedGlobalJClass(std::move(javaClass));
 *
 *     return javaClass_.isValid();
 * }
 * @endcode
 * @tparam T The data type of the JNI object e.g., jobject, jclass
 * @see ScopedJNILocalObject.
 * @ingroup platformandroid
 */
template <typename T>
class ScopedJNIGlobalObject
{
	public:

		/**
		 * Default constructor.
		 */
		ScopedJNIGlobalObject() = default;

		/**
		 * Move constructor.
		 * @param object The object to be moved
		 */
		ScopedJNIGlobalObject(ScopedJNIGlobalObject<T>&& object) noexcept;

		/**
		 * Creates a new global reference for a given local reference and takes over the ownership of that local reference.
		 * The local reference is deleted immediately, so that only the new global reference remains.
		 * This is the intended way to turn a local reference into a global one.
		 * The local reference is kept in case the global reference could not be created.
		 * @param localObject The local reference to be turned into a global reference, can be invalid
		 */
		explicit inline ScopedJNIGlobalObject(ScopedJNILocalObject<T>&& localObject);

		/**
		 * Creates a new global reference for a given reference, the caller keeps the ownership of the provided reference.
		 * The provided reference is not touched, as this object cannot know whether the reference is local, global, or owned by someone else.
		 * Beware: In case the provided reference is a local reference which the caller owns, the caller must still release it, so that the constructor taking a ScopedJNILocalObject is the better choice in that case.
		 * @param jniEnvironment The JNI environment of the calling thread, must be valid
		 * @param object The reference for which a new global reference will be created, can be nullptr
		 */
		inline ScopedJNIGlobalObject(JNIEnv& jniEnvironment, T object);

		/**
		 * Creates a new global reference for a given reference, the caller keeps the ownership of the provided reference.
		 * The provided reference is not touched, as this object cannot know whether the reference is local, global, or owned by someone else.
		 * @param scopedJNIEnvironment The associated scoped JNI environment
		 * @param object The reference for which a new global reference will be created, can be nullptr
		 */
		inline ScopedJNIGlobalObject(const ScopedJNIEnvironment& scopedJNIEnvironment, T object);

		/**
		 * Destructs this scoped object and deletes the global reference.
		 */
		inline ~ScopedJNIGlobalObject();

		/**
		 * Deletes the global reference and makes this object invalid.
		 * The reference can be deleted through the JNI environment of an attached thread only, so that this function must be called from a thread which is attached to the virtual machine.
		 * Beware: The reference is not deleted if the calling thread is not attached, the virtual machine reclaims it once it ends.
		 * This object does not attach the thread itself, as an attach allocates a Java thread object and can block until a running garbage collection has finished.
		 * A thread which releases global references regularly should be attached once for its entire lifetime, e.g., through NativeInterfaceManager::environment().
		 */
		inline void release();

		/**
		 * Returns whether the calling thread is attached to the virtual machine of this reference, and thus whether this object can be released on the calling thread.
		 * An object without a reference can always be released, as there is nothing to delete.
		 * Beware: A thread must not be attached just to release a reference, instead a thread which releases global references should be attached for its entire lifetime, e.g., through NativeInterfaceManager::environment().
		 * @return True, if so
		 */
		inline bool isThreadAttached() const;

		/**
		 * Returns whether this scoped object holds a valid reference.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns the encapsulated reference.
		 * @return The encapsulated reference, nullptr if no reference is encapsulated
		 */
		inline const T& object() const;

		/**
		 * Returns the encapsulated reference, must not be called if no reference is encapsulated.
		 * @see isValid().
		 * @return The encapsulated reference
		 */
		inline const T& operator*() const;

		/**
		 * Returns whether this scoped object holds a valid reference.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param object The object to be moved
		 * @return Reference to this object
		 */
		ScopedJNIGlobalObject<T>& operator=(ScopedJNIGlobalObject<T>&& object) noexcept;

	protected:

		/**
		 * Disables copy constructor.
		 * @param object The object which would be copied
		 */
		ScopedJNIGlobalObject(const ScopedJNIGlobalObject& object) = delete;

		/**
		 * Disables copy operator.
		 * @param object The object which would be copied
		 * @return Reference to this object
		 */
		ScopedJNIGlobalObject& operator=(const ScopedJNIGlobalObject& object) = delete;

	protected:

		/// The Java virtual machine of the environment which has created the global reference.
		JavaVM* javaVM_ = nullptr;

		/// The encapsulated global reference.
		T object_ = T();
};

template <typename T>
inline ScopedJNIGlobalObject<T>::ScopedJNIGlobalObject(ScopedJNIGlobalObject<T>&& object) noexcept
{
	*this = std::move(object);
}

template <typename T>
inline ScopedJNIGlobalObject<T>::ScopedJNIGlobalObject(ScopedJNILocalObject<T>&& localObject)
{
	if (localObject.object_ == nullptr)
	{
		return;
	}

	ocean_assert(localObject.jniEnvironment_ != nullptr);

	if (localObject.jniEnvironment_ == nullptr)
	{
		return;
	}

	// the environment of the local reference belongs to the thread which has created it,
	// using it on any other thread would corrupt the runtime state of that thread, so that the reference must not be promoted here

	if (!localObject.verifyCreatingThread())
	{
		return;
	}

	*this = ScopedJNIGlobalObject<T>(*localObject.jniEnvironment_, localObject.object_);

	if (object_ == nullptr)
	{
		// the local reference is kept, so that the caller can still use it, e.g., to create a diagnostic
		return;
	}

	localObject.release();
}

template <typename T>
inline ScopedJNIGlobalObject<T>::ScopedJNIGlobalObject(JNIEnv& jniEnvironment, T object)
{
	if (object == nullptr)
	{
		return;
	}

	if (jniEnvironment.GetJavaVM(&javaVM_) != JNI_OK)
	{
		ocean_assert(false && "Failed to determine the Java virtual machine!");

		javaVM_ = nullptr;
		return;
	}

	object_ = T(jniEnvironment.NewGlobalRef(object));

	if (object_ == nullptr)
	{
		// a failing NewGlobalRef() may leave an OutOfMemoryError pending, the exception must not stay pending as the calling thread may never return to Java

		if (jniEnvironment.ExceptionCheck() == JNI_TRUE)
		{
			jniEnvironment.ExceptionClear();
		}

		Log::error() << "ScopedJNIGlobalObject: Failed to create the global reference!";

		javaVM_ = nullptr;
	}
}

template <typename T>
inline ScopedJNIGlobalObject<T>::ScopedJNIGlobalObject(const ScopedJNIEnvironment& scopedJNIEnvironment, T object)
{
	ocean_assert(scopedJNIEnvironment.isValid());

	if (!scopedJNIEnvironment.isValid())
	{
		return;
	}

	*this = ScopedJNIGlobalObject<T>(*scopedJNIEnvironment.jniEnv(), object);
}

template <typename T>
inline ScopedJNIGlobalObject<T>::~ScopedJNIGlobalObject()
{
	release();
}

template <typename T>
inline void ScopedJNIGlobalObject<T>::release()
{
	if (object_ == nullptr)
	{
		return;
	}

	ocean_assert(javaVM_ != nullptr);

	JNIEnv* jniEnvironment = nullptr;

	if (javaVM_->GetEnv((void**)(&jniEnvironment), JNI_VERSION_1_6) == JNI_OK)
	{
		// the calling thread is attached to the virtual machine and owns the returned environment,
		// so that the reference can be deleted right here, this is the expected case

		jniEnvironment->DeleteGlobalRef(object_);
	}
	else
	{
		// the calling thread is not attached, the reference cannot be deleted without an environment,
		// attaching the thread here would allocate a Java thread object and could block until a running garbage collection has finished,
		// which is not acceptable in a destructor, so that the reference is abandoned and reclaimed once the virtual machine ends,
		// a thread which releases global references can be attached for its entire lifetime with a call to NativeInterfaceManager::environment()

		Messenger::writeToDebugOutput("ScopedJNIGlobalObject: The global reference is not deleted, the thread is not attached to the virtual machine!");

		ocean_assert(false && "The global reference is leaked, release this object from a thread which is attached to the virtual machine!");
	}

	javaVM_ = nullptr;
	object_ = T();
}

template <typename T>
inline bool ScopedJNIGlobalObject<T>::isThreadAttached() const
{
	if (object_ == nullptr)
	{
		return true;
	}

	ocean_assert(javaVM_ != nullptr);

	JNIEnv* jniEnvironment = nullptr;

	return javaVM_->GetEnv((void**)(&jniEnvironment), JNI_VERSION_1_6) == JNI_OK;
}

template <typename T>
inline bool ScopedJNIGlobalObject<T>::isValid() const
{
	return object_ != nullptr;
}

template <typename T>
inline const T& ScopedJNIGlobalObject<T>::object() const
{
	return object_;
}

template <typename T>
inline const T& ScopedJNIGlobalObject<T>::operator*() const
{
	ocean_assert(isValid());

	return object_;
}

template <typename T>
inline ScopedJNIGlobalObject<T>::operator bool() const
{
	return isValid();
}

template <typename T>
ScopedJNIGlobalObject<T>& ScopedJNIGlobalObject<T>::operator=(ScopedJNIGlobalObject<T>&& object) noexcept
{
	if (this != &object)
	{
		release();

		javaVM_ = object.javaVM_;
		object_ = object.object_;

		object.javaVM_ = nullptr;
		object.object_ = T();
	}

	return *this;
}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_SCOPED_JNI_GLOBAL_OBJECT_H
