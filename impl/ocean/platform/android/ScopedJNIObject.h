/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_SCOPED_JNI_OBJECT_H
#define META_OCEAN_PLATFORM_ANDROID_SCOPED_JNI_OBJECT_H

#include "ocean/platform/android/Android.h"
#include "ocean/platform/android/ScopedJNIEnvironment.h"

#include "ocean/base/Messenger.h"

/**
 * Define OCEAN_ENABLE_JNI_REFERENCE_VERIFICATION to verify in a release build that a local reference is released on the thread which has created it.
 * Debug builds verify this always, the macro exists for issues which cannot be reproduced in a debug build.
 * @see ScopedJNILocalObject.
 * @ingroup platformandroid
 */
#if defined(OCEAN_DEBUG) || defined(OCEAN_ENABLE_JNI_REFERENCE_VERIFICATION)
	#define OCEAN_VERIFY_JNI_REFERENCE_THREAD
#endif

namespace Ocean
{

namespace Platform
{

namespace Android
{

// Forward declaration.
template <typename T> class ScopedJNILocalObject;

// Forward declaration.
template <typename T> class ScopedJNIGlobalObject;

/**
 * Definition of a scoped object encapsulating a local reference to a jclass object.
 * @see ScopedJNILocalObject
 * @ingroup platformandroid
 */
using ScopedJClass = ScopedJNILocalObject<jclass>;

/**
 * Definition of a scoped object encapsulating a local reference to a jobject object.
 * @see ScopedJNILocalObject
 * @ingroup platformandroid
 */
using ScopedJObject = ScopedJNILocalObject<jobject>;

/**
 * Definition of a scoped object encapsulating a local reference to a jobjectArray object.
 * @see ScopedJNILocalObject
 * @ingroup platformandroid
 */
using ScopedJObjectArray = ScopedJNILocalObject<jobjectArray>;

/**
 * Definition of a scoped object encapsulating a local reference to a jstring object.
 * @see ScopedJNILocalObject
 * @ingroup platformandroid
 */
using ScopedJString = ScopedJNILocalObject<jstring>;

/**
 * This class implements a scoped local reference to a JNI object.
 * A local reference belongs to the thread which has created it and is valid until that thread returns to Java,
 * therefore this object must be used as a short living function-local object only.
 * Use ScopedJNIGlobalObject instead whenever the reference needs to be stored, or needs to be used on another thread.
 * The following code example shows the intended usage of this object:
 * @code
 * void functionUsingJavaObjects(JNIEnv& jniEnv, jobjectArray javaArray, const jsize size)
 * {
 *     // the reference is deleted when the scope ends, also if the function returns early
 *     const ScopedJClass javaClassList(jniEnv, jniEnv.FindClass("java/util/List"));
 *
 *     if (!javaClassList)
 *     {
 *         return;
 *     }
 *
 *     for (jsize n = 0; n < size; ++n)
 *     {
 *         // the reference is deleted at the end of each iteration, so that only one reference is alive at a time
 *         const ScopedJString javaString(jniEnv, jstring(jniEnv.GetObjectArrayElement(javaArray, n)));
 *     }
 * }
 *
 * class MyClass
 * {
 *     protected:
 *
 *         // WRONG: a local reference must not be stored, use ScopedGlobalJClass instead
 *         ScopedJClass javaClass_;
 * };
 * @endcode
 * @tparam T The data type of the JNI object e.g., jobject, jclass, jstring
 * @see ScopedJNIGlobalObject.
 * @ingroup platformandroid
 */
template <typename T>
class ScopedJNILocalObject
{
	friend class ScopedJNIGlobalObject<T>;

	public:

		/**
		 * Default constructor.
		 */
		ScopedJNILocalObject() = default;

		/**
		 * Move constructor.
		 * @param object The object to be moved
		 */
		ScopedJNILocalObject(ScopedJNILocalObject<T>&& object) noexcept;

		/**
		 * Creates a new scoped object for a given local reference, the object takes over the ownership of the reference.
		 * @param jniEnvironment The JNI environment of the calling thread, must be valid
		 * @param object The local reference to be encapsulated, can be nullptr
		 */
		inline ScopedJNILocalObject(JNIEnv& jniEnvironment, T object);

		/**
		 * Creates a new scoped object for a given local reference, the object takes over the ownership of the reference.
		 * Beware: This object must not live longer than the provided scoped JNI environment!
		 * @param scopedJNIEnvironment The associated scoped JNI environment
		 * @param object The local reference to be encapsulated, can be nullptr
		 */
		inline ScopedJNILocalObject(const ScopedJNIEnvironment& scopedJNIEnvironment, T object);

		/**
		 * Destructs this scoped object and deletes the local reference.
		 */
		inline ~ScopedJNILocalObject();

		/**
		 * Deletes the local reference and makes this object invalid.
		 * Beware: This function must be called on the thread which has created the reference.
		 */
		inline void release();

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
		ScopedJNILocalObject<T>& operator=(ScopedJNILocalObject<T>&& object) noexcept;

	protected:

		/**
		 * Disables copy constructor.
		 * @param object The object which would be copied
		 */
		ScopedJNILocalObject(const ScopedJNILocalObject& object) = delete;

		/**
		 * Disables copy operator.
		 * @param object The object which would be copied
		 * @return Reference to this object
		 */
		ScopedJNILocalObject& operator=(const ScopedJNILocalObject& object) = delete;

		/**
		 * Verifies that the calling thread is still the thread which has created the reference.
		 * The verification is based on GetEnv() which is a lookup of thread-local state of the runtime, the function neither attaches the thread nor allocates.
		 * The function reports success without verifying anything if the verification is not enabled for this build.
		 * Beware: The verification compares the environment of the calling thread with the environment of the creating thread,
		 * so that a misuse stays undetected whenever the runtime has re-used the address of an environment which has been released in the meantime.
		 * @return True, if the reference can be deleted; False, if the reference does not exist anymore or belongs to another thread
		 */
		inline bool verifyCreatingThread() const;

	protected:

		/// The JNI environment of the thread which has created the reference.
		JNIEnv* jniEnvironment_ = nullptr;

		/// The encapsulated local reference.
		T object_ = T();

		/// The Java virtual machine of the creating thread, nullptr if the verification is not enabled for this build.
		/// The member exists in every build so that the size of this object does not depend on the verification, which would break the one definition rule.
		JavaVM* verificationJavaVM_ = nullptr;
};

template <typename T>
inline ScopedJNILocalObject<T>::ScopedJNILocalObject(ScopedJNILocalObject<T>&& object) noexcept
{
	*this = std::move(object);
}

template <typename T>
inline ScopedJNILocalObject<T>::ScopedJNILocalObject(JNIEnv& jniEnvironment, T object) :
	jniEnvironment_(&jniEnvironment),
	object_(object)
{
#ifdef OCEAN_VERIFY_JNI_REFERENCE_THREAD
	if (object_ != nullptr)
	{
		jniEnvironment.GetJavaVM(&verificationJavaVM_);
	}
#endif // OCEAN_VERIFY_JNI_REFERENCE_THREAD
}

template <typename T>
inline ScopedJNILocalObject<T>::ScopedJNILocalObject(const ScopedJNIEnvironment& scopedJNIEnvironment, T object)
{
	ocean_assert(scopedJNIEnvironment.isValid());

	if (!scopedJNIEnvironment.isValid())
	{
		// without an environment the reference could never be deleted, so that this object must not take over the ownership
		return;
	}

	*this = ScopedJNILocalObject<T>(*scopedJNIEnvironment.jniEnv(), object);
}

template <typename T>
inline ScopedJNILocalObject<T>::~ScopedJNILocalObject()
{
	release();
}

template <typename T>
inline void ScopedJNILocalObject<T>::release()
{
	if (object_ == nullptr)
	{
		jniEnvironment_ = nullptr;
		return;
	}

	ocean_assert(jniEnvironment_ != nullptr);

	if (verifyCreatingThread())
	{
		jniEnvironment_->DeleteLocalRef(object_);
	}

	jniEnvironment_ = nullptr;
	object_ = T();
	verificationJavaVM_ = nullptr;
}

template <typename T>
inline bool ScopedJNILocalObject<T>::verifyCreatingThread() const
{
	if (verificationJavaVM_ == nullptr)
	{
		// the verification is not enabled for this build, or the virtual machine could not be determined
		return true;
	}

	JNIEnv* callingJNIEnvironment = nullptr;

	if (verificationJavaVM_->GetEnv((void**)(&callingJNIEnvironment), JNI_VERSION_1_6) != JNI_OK)
	{
		// the thread has been detached in the meantime, which has released the thread's local references already,
		// deleting the reference would access the environment of a thread which does not exist anymore

		Messenger::writeToDebugOutput("ScopedJNILocalObject: The thread has been detached, the local reference does not exist anymore!");
		ocean_assert(false && "The thread has been detached, the local reference does not exist anymore!");

		return false;
	}

	if (callingJNIEnvironment != jniEnvironment_)
	{
		// a local reference belongs to the thread which has created it, deleting it here would modify the reference table of the calling thread

		Messenger::writeToDebugOutput("ScopedJNILocalObject: The local reference must be released on the thread which has created it!");
		ocean_assert(false && "The local reference must be released on the thread which has created it!");

		return false;
	}

	return true;
}

template <typename T>
inline bool ScopedJNILocalObject<T>::isValid() const
{
	return object_ != nullptr;
}

template <typename T>
inline const T& ScopedJNILocalObject<T>::object() const
{
	return object_;
}

template <typename T>
inline const T& ScopedJNILocalObject<T>::operator*() const
{
	ocean_assert(isValid());

	return object_;
}

template <typename T>
inline ScopedJNILocalObject<T>::operator bool() const
{
	return isValid();
}

template <typename T>
ScopedJNILocalObject<T>& ScopedJNILocalObject<T>::operator=(ScopedJNILocalObject<T>&& object) noexcept
{
	if (this != &object)
	{
		release();

		jniEnvironment_ = object.jniEnvironment_;
		object_ = object.object_;

		verificationJavaVM_ = object.verificationJavaVM_;

		object.jniEnvironment_ = nullptr;
		object.object_ = T();
		object.verificationJavaVM_ = nullptr;
	}

	return *this;
}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_SCOPED_JNI_OBJECT_H
