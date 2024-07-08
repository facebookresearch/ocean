/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_TCP_CLIENT_H
#define FACEBOOK_NETWORK_TCP_CLIENT_H

#include "ocean/network/Network.h"
#include "ocean/network/ConnectionOrientedClient.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements a TCP client.
 * A TCP client can send and receive data to (and from) one remove TCP client.<br>
 * TCP is a stream-based network protocol which does not provide any message boundaries.<br>
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT TCPClient : virtual public ConnectionOrientedClient
{
	public:

		/**
		 * Creates a new TCP client.
		 */
		TCPClient();

		/**
		 * Destructs a TCP client.
		 */
		~TCPClient() override;

		/**
		 * Connects to a TCP server.
		 * @see ConnectionOrientedClient::connect().
		 */
		bool connect(const Address4& address, const Port& port, const unsigned int timeout = 1000u) override;

		/**
		 * Reconnects the client by the usage of the most recent address and port.
		 * @see ConnectionOrientedClient::connect().
		 */
		bool connect(const unsigned int timeout = 1000u) override;

		/**
		 * Disconnects the client.
		 * @see ConnectionOrientedClient::disconnect().
		 */
		bool disconnect() override;

	protected:

		/**
		 * Build the TCP socket.
		 * @return True, if succeeded
		 */
		bool buildSocket();
};

}

}

#endif // FACEBOOK_NETWORK_TCP_CLIENT_H
