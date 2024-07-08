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

namespace Ocean
{

namespace Platform
{

namespace Android
{

// Forward declaration.
template <typename T> class ScopedJNIObject;

/**
 * Definition of a scoped object encapsulating a jclass object.
 * @see ScopedJNIObject
 * @ingroup platformandroid
 */
typedef ScopedJNIObject<jclass> ScopedJClass;

/**
 * Definition of a scoped object encapsulating a jobject object.
 * @see ScopedJNIObject
 * @ingroup platformandroid
 */
typedef ScopedJNIObject<jobject> ScopedJObject;

/**
 * Definition of a scoped object encapsulating a jobjectArray object.
 * @see ScopedJNIObject
 * @ingroup platformandroid
 */
typedef ScopedJNIObject<jobjectArray> ScopedJObjectArray;

/**
 * Definition of a scoped object encapsulating a jstring object.
 * @see ScopedJNIObject
 * @ingroup platformandroid
 */
typedef ScopedJNIObject<jstring> ScopedJString;

/**
 * This class implements a scoped JNI jobject/jclass/jstring.
 * The scoped object encapsulates a jobject and deletes the local reference when the scoped ends.
 * @tparam T The data type of the JNI object e.g., jobject, jclass, jstring
 * @ingroup platformandroid
 */
template <typename T>
class ScopedJNIObject
{
	public:

		/**
		 * Default constructor.
		 */
		ScopedJNIObject() = default;

		/**
		 * Move constructor.
		 * @param object The object to be moved
		 */
		ScopedJNIObject(ScopedJNIObject<T>&& object);

		/**
		 * Creates a new scoped object for a given jobject.
		 * Beware: This object must not live longer than the provided scoped JNI environment!
		 * @param scopedJNIEnvironment The associated scoped JNI environment
		 * @param object The jobject to be encapsulated, can be nullptr
		 */
		inline ScopedJNIObject(const ScopedJNIEnvironment& scopedJNIEnvironment, T object);

		/**
		 * Creates a new scoped object for a given jobject.
		 */
		inline ScopedJNIObject(JNIEnv& jniEnvironment, T object);

		/**
		 * Destructs this scoped object and deletes the local reference to the jobject.
		 */
		inline ~ScopedJNIObject();

		/**
		 * Makes this scoped object a global object.
		 * Global objects can be access from any thread.
		 */
		inline void makeGlobal();

		/**
		 * Releases this encapsulated jobject and deletes the local reference to the jobject.
		 */
		inline void release();

		/**
		 * Returns whether this scoped object holds a valid jobject.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this object is global.
		 * @return True, if so
		 */
		inline bool isGlobal() const;

		/**
		 * Returns the pointer to the encapsulated object.
		 * @return The encapsulated object, nullptr if no object is encapsulated
		 */
		inline const T& object() const;

		/**
		 * Cast operator.
		 * @return The encapsulated object, nullptr if no object is encapsulated
		 */
		inline operator T() const;

		/**
		 * Returns the pointer to the encapsulated object, must not be called if no object is encapsulated.
		 * @see isValid().
		 * @return The encapsulated object, nullptr if no object is encapsulated
		 */
		inline const T& operator->() const;

		/**
		 * Returns the pointer to the encapsulated object, must not be called if no object is encapsulated.
		 * @see isValid().
		 * @return The encapsulated object, nullptr if no object is encapsulated
		 */
		inline const T& operator*() const;

		/**
		 * Returns whether this scoped object holds a valid jobject.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param object The object to be moved
		 * @return Reference to this object
		 */
		ScopedJNIObject<T>& operator=(ScopedJNIObject<T>&& object);

	protected:

		/**
		 * Disables copy constructor.
		 * @param object The object which would be copied
		 */
		ScopedJNIObject(const ScopedJNIObject& object) = delete;

		/**
		 * Disables copy operator.
		 * @param object The object which would be copied
		 * @return Reference to this object
		 */
		ScopedJNIObject& operator=(const ScopedJNIObject& object) = delete;

	protected:

		/// The corresponding JNI environment.
		JNIEnv* jniEnvironment_ = nullptr;

		/// The encapsulated object.
		T object_ = T();

		/// The encapsulated global object.
		T globalObject_ = T();
};

template <typename T>
inline ScopedJNIObject<T>::ScopedJNIObject(ScopedJNIObject<T>&& object)
{
	*this = std::move(object);
}

template <typename T>
inline ScopedJNIObject<T>::ScopedJNIObject(const ScopedJNIEnvironment& scopedJNIEnvironment, T object) :
	jniEnvironment_(scopedJNIEnvironment.jniEnv()),
	object_(object)
{
	// nothing to do here
}

template <typename T>
inline ScopedJNIObject<T>::ScopedJNIObject(JNIEnv& jniEnvironment, T object) :
	jniEnvironment_(&jniEnvironment),
	object_(object)
{
	// nothing to do here
}

template <typename T>
inline ScopedJNIObject<T>::~ScopedJNIObject()
{
	release();
}

template <typename T>
inline void ScopedJNIObject<T>::makeGlobal()
{
	ocean_assert(isValid());

	if (globalObject_ == nullptr)
	{
		globalObject_ = T(jniEnvironment_->NewGlobalRef(object_));
	}
}

template <typename T>
inline void ScopedJNIObject<T>::release()
{
	if (globalObject_ != nullptr)
	{
		ocean_assert(jniEnvironment_ != nullptr);
		jniEnvironment_->DeleteGlobalRef(globalObject_);
		globalObject_ = nullptr;
	}

	if (object_)
	{
		ocean_assert(jniEnvironment_ != nullptr);
		jniEnvironment_->DeleteLocalRef(object_);
		object_ = nullptr;
	}
}

template <typename T>
inline bool ScopedJNIObject<T>::isValid() const
{
	return object_ != nullptr;
}

template <typename T>
inline bool ScopedJNIObject<T>::isGlobal() const
{
	return globalObject_ != nullptr;
}

template <typename T>
inline const T& ScopedJNIObject<T>::object() const
{
	if (globalObject_ != nullptr)
	{
		ocean_assert(object_ != nullptr);
		return globalObject_;
	}

	return object_;
}

template <typename T>
inline ScopedJNIObject<T>::operator T() const
{
	return object();
}

template <typename T>
inline const T& ScopedJNIObject<T>::operator->() const
{
	ocean_assert(isValid());

	return object();
}

template <typename T>
inline const T& ScopedJNIObject<T>::operator*() const
{
	ocean_assert(isValid());

	return object();
}

template <typename T>
inline ScopedJNIObject<T>::operator bool() const
{
	return isValid();
}

template <typename T>
ScopedJNIObject<T>& ScopedJNIObject<T>::operator=(ScopedJNIObject<T>&& object)
{
	if (this != &object)
	{
		release();

		jniEnvironment_ = object.jniEnvironment_;
		object_ = std::move(object.object_);
		globalObject_ = std::move(object.globalObject_);

		object.jniEnvironment_ = nullptr;
		object.object_ = T();
		object.globalObject_ = T();
	}

	return *this;
}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_SCOPED_JNI_OBJECT_H
