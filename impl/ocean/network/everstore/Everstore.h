// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_NETWORK_EVERSTORE_EVERSTORE_H
#define FACEBOOK_NETWORK_NETWORK_EVERSTORE_EVERSTORE_H

#include "ocean/network/Network.h"

namespace Ocean
{

namespace Network
{

namespace Everstore
{

/**
 * @ingroup network
 * @defgroup networkeverstore Ocean Network Everstore Library
 * @{
 * The Ocean Network Everstore Library provides access to Everstore functionalities.<br>
 * The library is available on devservers only.
 * @}
 */

/**
 * @namespace Ocean::Network::Everstore Namespace of the Network Everstore library.<p>
 * The Namespace Ocean::Network::Everstore is used in the entire Ocean Network Everstore Library.
 */

// Defines OCEAN_NETWORK_EVERSTORE_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_NETWORK_EVERSTORE_EXPORT
		#define OCEAN_NETWORK_EVERSTORE_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_NETWORK_EVERSTORE_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_NETWORK_EVERSTORE_EXPORT
#endif

}

}

}

#endif // FACEBOOK_NETWORK_NETWORK_EVERSTORE_EVERSTORE_H
