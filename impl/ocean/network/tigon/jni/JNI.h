// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_NETWORK_TIGON_JNI_JNI_H
#define META_OCEAN_NETWORK_TIGON_JNI_JNI_H

#include "ocean/network/tigon/Network.h"

#include <jni.h>

namespace Ocean
{

namespace Network
{

namespace Tigon
{

namespace JNI
{

/**
 * @ingroup networktigon
 * @defgroup networktigonjni Ocean Network Tigon JNI Library
 * @{
 * The Ocean Network Tigon JNI Library provides a Java Native Interface for Android.
 * The library is available on Android platforms only.
 * @}
 */

/**
 * @namespace Ocean::Network::Tigon::JNI Namespace of the Network Tigon JNI library.<p>
 * The Namespace Ocean::Network::Tigon JNI is used in the entire Ocean Network Tigon JNI Library.
 */

#ifndef _ANDROID
	#error This library is available on android platforms only!
#endif // _ANDROID

// Defines OCEAN_NETWORK_TIGON_JNI_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_ANDROID_WIN_EXPORT
		#define OCEAN_NETWORK_TIGON_JNI_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_NETWORK_TIGON_JNI_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_NETWORK_TIGON_JNI_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_NETWORK_TIGON_JNI_JNI_H
