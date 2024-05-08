/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ANDROID_JNI_JNI_H
#define META_OCEAN_DEVICES_ANDROID_JNI_JNI_H

#include "ocean/devices/android/Android.h"

#include <jni.h>

namespace Ocean
{

namespace Devices
{

namespace Android
{

namespace JNI
{

/**
 * @ingroup devicesandroid
 * @defgroup devicesandroidjni Ocean Devices Android JNI Library
 * @{
 * The Ocean Devices Android JNI Library provides a Java Native Interface for Android.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Devices::Android::JNI Namespace of the Devices Android JNI library.<p>
 * The Namespace Ocean::Devices::Android::JNI is used in the entire Ocean Devices Android JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_DEVICES_ANDROID_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_DEVICES_ANDROID_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_ANDROID_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_ANDROID_JNI_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_DEVICES_ANDROID_JNI_JNI_H
