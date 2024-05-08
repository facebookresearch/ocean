/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_TCP_SERVER_H
#define FACEBOOK_NETWORK_TCP_SERVER_H

#include "ocean/network/Network.h"
#include "ocean/network/ConnectionOrientedServer.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements a TCP server.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT TCPServer : virtual public ConnectionOrientedServer
{
	public:

		/**
		 * Creates a new TCP server.
		 */
		TCPServer();

		/**
		 * Destructs a TCP server.
		 */
		~TCPServer() override;

		/**
		 * Starts the server.
		 * Beware: If a specific listening port is expected the own port has to be set before
		 * @see Server::start().
		 */
		bool start() override;

		/**
		 * Disconnects a specified connection.
		 * @see ConnectionOrientedServer::disconnect().
		 */
		bool disconnect(const ConnectionId connection) override;

	protected:

		/**
		 * Builds the TCP socket.
		 * @return True, if succeeded
		 */
		bool buildSocket();
};

}

}

#endif // FACEBOOK_NETWORK_TCP_SERVER_H
