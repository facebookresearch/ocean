/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_JNI_JNI_H
#define META_OCEAN_MEDIA_ANDROID_JNI_JNI_H

#include "ocean/media/android/Android.h"

#include <jni.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

namespace JNI
{

/**
 * @ingroup mediaandroid
 * @defgroup mediaandroidjni Ocean Media Android JNI Library
 * @{
 * The Ocean Media Android JNI Library provides a Java Native Interface for Android.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Media::Android::JNI Namespace of the Media Android JNI library.<p>
 * The Namespace Ocean::Media::Android::JNI is used in the entire Ocean Media Android JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_MEDIA_ANDROID_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_MEDIA_ANDROID_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_ANDROID_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_ANDROID_JNI_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_JNI_JNI_H
