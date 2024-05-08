/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_PACKAGED_UDP_CLIENT_H
#define FACEBOOK_NETWORK_PACKAGED_UDP_CLIENT_H

#include "ocean/network/Network.h"
#include "ocean/network/PackagedConnectionlessClient.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements a UDP client able to send larger messages as normally restricted by the UDP protocol.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT PackagedUDPClient : virtual public PackagedConnectionlessClient
{
	public:

		/**
		 * Creates a new UDP client.
		 */
		PackagedUDPClient();

		/**
		 * Destructs a UDP client.
		 */
		~PackagedUDPClient() override;

	protected:

		/**
		 * Build the UDP socket.
		 * @param localAddress The local address to which the socket will be bound, a default address to bound the socket to any local address
		 * @param localPort The local port to which the socket will be bound, a default port to bound the socket to any free port
		 * @return True, if succeeded
		 */
		bool buildSocket(const Address4& localAddress = Address4(), const Port localPort = Port());
};

}

}

#endif // FACEBOOK_NETWORK_PACKAGED_UDP_CLIENT_H
