// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_H
#define FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_H

#include "ocean/network/Network.h"

namespace Ocean
{

namespace Network
{

namespace Tigon
{

/**
 * @ingroup network
 * @defgroup networktigon Ocean Network Tigon Library
 * @{
 * The Ocean Network Tigon Library provides access to networking functionalities using Tigon.
 * Functionalities include e.g., HTTP requests or GraphQL queries.<br>
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Network::Tigon Namespace of the Network Tigon library.<p>
 * The Namespace Ocean::Network::Tigon is used in the entire Ocean Network Tigon Library.
 */

// Defines OCEAN_NETWORK_TIGON_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_NETWORK_TIGON_EXPORT
		#define OCEAN_NETWORK_TIGON_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_NETWORK_TIGON_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_NETWORK_TIGON_EXPORT
#endif

}

}

}

#endif // FACEBOOK_NETWORK_NETWORK_TIGON_TIGON_H
