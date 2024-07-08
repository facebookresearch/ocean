/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_NETWORK_H
#define FACEBOOK_NETWORK_NETWORK_H

#ifndef _WINDOWS
	#include <sys/socket.h>
	#include <netinet/in.h>
#endif

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

namespace Ocean
{

namespace Network
{

/**
 * @defgroup network Ocean Network Library
 * @{
 * The Ocean Network Library provides a huge number of network functionalities.<br>
 * The library supports connection less and connection oriented clients and servers.<br>
 * Additionally, this library holds a powerful streaming infrastructure streaming various data types.<br>
 *
 * For connection less transmission use a UDP client or server.<br>
 * For connection oriented transmission use a TCP client or server.<br>
 * Also, this library provides the possibility to resolve network addresses or services.<br>
 * @see UDPClient, UDPServer, TCPClient, TCPServer, Resolver.
 *
 * The streaming server is implemented as an on-demand streaming server.<br>
 * Configuration tasks are done using a TCP connection, the data is streamed via a UDP connection.<br>
 * The server is independent from the streaming data and supports different streaming channels.<br>
 * Additionally, each channel accepts more than one subscriber allowing several clients to receive the same stream.<br>
 *
 * However, each streaming client can receive one provides channel only.<br>
 * That means that you have to use more than one client to receive more than one data stream.<br>
 * @see StreamingServer, StreamingClient.
 *
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Network Namespace of the Network library.<p>
 * The Namespace Ocean::Network is used in the entire Ocean Network Library.
 */

// Defines OCEAN_NETWORK_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_NETWORK_EXPORT
		#define OCEAN_NETWORK_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_NETWORK_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_NETWORK_EXPORT
#endif

}

}

#endif
