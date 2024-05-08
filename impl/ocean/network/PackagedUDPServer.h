/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_PACKAGED_UDP_SERVER_H
#define FACEBOOK_NETWORK_PACKAGED_UDP_SERVER_H

#include "ocean/network/Network.h"
#include "ocean/network/PackagedConnectionlessServer.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements a packaged UDP server.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT PackagedUDPServer : virtual public PackagedConnectionlessServer
{
	public:

		/**
		 * Creates a new UDP server object.
		 */
		PackagedUDPServer();

		/**
		 * Destructs a UDP server object.
		 */
		~PackagedUDPServer() override;

		/**
		 * Starts the server.
		 * @see Server::start().
		 */
		bool start() override;

		/**
		 * Sets the own port of this socket.
		 * @see Socket::setPort().
		 */
		bool setPort(const Port& port) override;

	protected:

		/**
		 * Creates a UDP server socket.
		 * @param localAddress The local address to which the socket will be bound, a default address to bound the socket to any local address
		 * @param localPort The local port to which the socket will be bound, a default port to bound the socket to any free port
		 * @return True, if succeeded
		 */
		bool buildSocket(const Address4& localAddress = Address4(), const Port localPort = Port());
};


}

}

#endif // FACEBOOK_NETWORK_PACKAGED_UDP_SERVER_H
