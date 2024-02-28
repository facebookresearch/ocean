// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_PATTERN_JNI_JNI_H
#define META_OCEAN_DEVICES_PATTERN_JNI_JNI_H

#include "ocean/devices/pattern/Pattern.h"

#include <jni.h>

namespace Ocean
{

namespace Devices
{

namespace Pattern
{

namespace JNI
{

/**
 * @ingroup devicespattern
 * @defgroup devicespatternjni Ocean Devices Pattern JNI Library
 * @{
 * The Ocean Devices Pattern JNI Library provides a Java Native Interface for Android.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Devices::Pattern::JNI Namespace of the Devices Pattern JNI library.<p>
 * The Namespace Ocean::Devices::Pattern::JNI is used in the entire Ocean Devices Pattern JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_DEVICES_PATTERN_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_DEVICES_PATTERN_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_PATTERN_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_PATTERN_JNI_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_DEVICES_PATTERN_JNI_JNI_H
