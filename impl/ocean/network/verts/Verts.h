// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_VERTS_VERTS_H
#define FACEBOOK_NETWORK_VERTS_VERTS_H

#include "ocean/network/Network.h"

namespace Ocean
{

namespace Network
{

namespace Verts
{

/**
 * @ingroup network
 * @defgroup networkverts Ocean Network Verts Library
 * @{
 * The Ocean Network Verts Library provides access to networking functionalities using Verts.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Network::Verts Namespace of the Network Verts library.<p>
 * The Namespace Ocean::Network::Verts is used in the entire Ocean Network Verts Library.
 */

// Defines OCEAN_NETWORK_VERTS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_NETWORK_VERTS_EXPORT
		#define OCEAN_NETWORK_VERTS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_NETWORK_VERTS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_NETWORK_VERTS_EXPORT
#endif

}

}

}

#endif // FACEBOOK_NETWORK_VERTS_VERTS_H
