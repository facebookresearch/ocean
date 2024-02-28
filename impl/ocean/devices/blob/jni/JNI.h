// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_BLOB_JNI_JNI_H
#define META_OCEAN_DEVICES_BLOB_JNI_JNI_H

#include "ocean/devices/blob/Blob.h"

#include <jni.h>

namespace Ocean
{

namespace Devices
{

namespace Blob
{

namespace JNI
{

/**
 * @ingroup devicesblob
 * @defgroup devicesblobjni Ocean Devices Blob JNI Library
 * @{
 * The Ocean Devices Blob JNI Library provides a Java Native Interface for Android.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Devices::Blob::JNI Namespace of the Devices Blob JNI library.<p>
 * The Namespace Ocean::Devices::Blob::JNI is used in the entire Ocean Devices Blob JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_DEVICES_BLOB_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_DEVICES_BLOB_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_BLOB_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_BLOB_JNI_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_DEVICES_BLOB_JNI_JNI_H
