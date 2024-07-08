/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_NATIVE_SCOPED_JNI_ENVIRONMENT_H
#define META_OCEAN_PLATFORM_ANDROID_NATIVE_SCOPED_JNI_ENVIRONMENT_H

#include "ocean/platform/android/Android.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * This class implements a scoped access to a JNI environment with attached current thread.
 * Do not use/call this object out of two individual threads.
 * @ingroup platformandroid
 */
class OCEAN_PLATFORM_ANDROID_EXPORT ScopedJNIEnvironment
{
	public:

		/**
		 * Creates a new scoped object, acquires the JNI environment and attaches the thread.
		 * @param javaVM The virtual Java machine, must be valid
		 */
		explicit ScopedJNIEnvironment(JavaVM* javaVM);

		/**
		 * Destructs this object.
		 */
		~ScopedJNIEnvironment();

		/**
		 * Returns the JNI environment which is attached with the current thread.
		 * @return The JNI environment, can be nullptr if invalid
		 */
		inline JNIEnv* jniEnv() const;

		/**
		 * Returns whether this scoped object holds a valid JNI environment.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns the JNI environment which is attached with the current thread.
		 * @return The JNI environment, can be nullptr if invalid
		 */
		inline JNIEnv* operator->() const;

		/**
		 * Returns whether this scoped object holds a valid JNI environment.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Disabled copy constructor.
		 * @param scopedJNIEnvironment Object which would be copied
		 */
		ScopedJNIEnvironment(const ScopedJNIEnvironment& scopedJNIEnvironment) = delete;

		/**
		 * Disabled copy operator.
		 * @param scopedJNIEnvironment Object which would be copied
		 * @return Reference to this object
		 */
		ScopedJNIEnvironment& operator=(const ScopedJNIEnvironment& scopedJNIEnvironment) = delete;

	protected:

		/// The virtual Java machine.
		JavaVM* javaVM_ = nullptr;

		/// The JNI environment.
		JNIEnv* jniEnv_ = nullptr;

		/// True, if the thread has been attached; False, if the thread was attached already.
		bool threadAttachedExplicitly_ = false;
};

inline JNIEnv* ScopedJNIEnvironment::jniEnv() const
{
	return jniEnv_;
}

inline bool ScopedJNIEnvironment::isValid() const
{
	return jniEnv_ != nullptr;
}

inline JNIEnv* ScopedJNIEnvironment::operator->() const
{
	ocean_assert(isValid());

	return jniEnv();
}

inline ScopedJNIEnvironment::operator bool() const
{
	return isValid();
}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_NATIVE_SCOPED_JNI_ENVIRONMENT_H
