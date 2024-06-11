/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_USB_JNI_JNI_H
#define META_OCEAN_MEDIA_USB_JNI_JNI_H

#include "ocean/media/usb/USB.h"

#include <jni.h>

namespace Ocean
{

namespace Media
{

namespace USB
{

namespace JNI
{

/**
 * @ingroup mediausb
 * @defgroup mediausbjni Ocean Media USB JNI Library
 * @{
 * The Ocean Media USB JNI Library provides a Java Native Interface for USB.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Media::USB::JNI Namespace of the Media USB JNI library.<p>
 * The Namespace Ocean::Media::USB::JNI is used in the entire Ocean Media USB JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_MEDIA_USB_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_USB_WIN_EXPORT
		#define OCEAN_MEDIA_USB_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_USB_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_USB_JNI_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_JNI_JNI_H
