/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_NETWORK_JNI_JNI_H
#define META_OCEAN_NETWORK_JNI_JNI_H

#include "ocean/network/Network.h"

#include <jni.h>

namespace Ocean
{

namespace Network
{

namespace JNI
{

/**
 * @ingroup network
 * @defgroup networkjni Ocean Network JNI Library
 * @{
 * The Ocean Network JNI Library provides a Java Native Interface for Android.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Network::JNI Namespace of the Network JNI library.<p>
 * The Namespace Ocean::Network::JNI is used in the entire Ocean Network JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_NETWORK_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_NETWORK_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_NETWORK_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_NETWORK_JNI_EXPORT
#endif

}

}

}

#endif // META_OCEAN_NETWORK_JNI_JNI_H
