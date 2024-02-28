// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_CLOUD_JNI_JNI_H
#define META_OCEAN_DEVICES_CLOUD_JNI_JNI_H

#include "ocean/devices/cloud/Cloud.h"

#include <jni.h>

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

namespace JNI
{

/**
 * @ingroup devicescloud
 * @defgroup devicescloudjni Ocean Devices Cloud JNI Library
 * @{
 * The Ocean Devices Cloud JNI Library provides a Java Native Interface for Android.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Devices::Cloud::JNI Namespace of the Devices Cloud JNI library.<p>
 * The Namespace Ocean::Devices::Cloud::JNI is used in the entire Ocean Devices Cloud JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_DEVICES_CLOUD_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_DEVICES_CLOUD_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_CLOUD_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_CLOUD_JNI_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_DEVICES_CLOUD_JNI_JNI_H
