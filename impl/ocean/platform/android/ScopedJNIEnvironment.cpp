/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/ScopedJNIEnvironment.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

ScopedJNIEnvironment::ScopedJNIEnvironment(JavaVM* javaVM) :
	javaVM_(javaVM),
	jniEnv_(nullptr),
	threadAttachedExplicitly_(false)
{
	ocean_assert(javaVM_ != nullptr);

	const jint result = javaVM_->GetEnv((void**)&jniEnv_, JNI_VERSION_1_6);

	switch (result)
	{
		case JNI_OK:
		{
			ocean_assert(threadAttachedExplicitly_ == false);
			ocean_assert(jniEnv_ != nullptr);
			break;
		}

		case JNI_EDETACHED:
		{
			ocean_assert(jniEnv_ == nullptr);

			if (javaVM_->AttachCurrentThread(&jniEnv_, nullptr) != JNI_OK)
			{
				ocean_assert(jniEnv_ == nullptr);
				Log::error() << "Failed to attach current thread!";
			}
			else
			{
				ocean_assert(jniEnv_ != nullptr);

				threadAttachedExplicitly_ = true;
			}

			break;
		}

		default:
		{
			Log::error() << "Unknown error when getting JNI environment!";
		}
	}
}

ScopedJNIEnvironment::~ScopedJNIEnvironment()
{
	if (threadAttachedExplicitly_)
	{
		ocean_assert(jniEnv_ != nullptr);

		const jint result = javaVM_->DetachCurrentThread();
		ocean_assert_and_suppress_unused(result == JNI_OK, result);
	}
}

}

}

}
