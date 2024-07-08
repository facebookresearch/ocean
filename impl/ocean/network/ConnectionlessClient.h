/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_CONNECTIONLESS_CLIENT_H
#define FACEBOOK_NETWORK_CONNECTIONLESS_CLIENT_H

#include "ocean/network/Network.h"
#include "ocean/network/Address4.h"
#include "ocean/network/Client.h"
#include "ocean/network/Port.h"

namespace Ocean
{

namespace Network
{

/**
 * This class is the base class for all connectionless clients.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT ConnectionlessClient : virtual public Client
{
	public:

		/**
		 * Sends data to a specified recipient.
		 * @param address Recipient address
		 * @param port Recipient port
		 * @param data The data to send, can be nullptr if 'size == 0'
		 * @param size The size of the data to send in bytes, with range [0, infinity)
		 * @return SR_SUCCEEDED, if succeeded
		 */
		SocketResult send(const Address4& address, const Port& port, const void* data, const size_t size);

		/**
		 * Sends a message to a specified recipient.
		 * @param address Recipient address
		 * @param port Recipient port
		 * @param message The message to send, must be valid
		 * @return SR_SUCCEEDED, if succeeded
		 */
		inline SocketResult send(const Address4& address, const Port& port, const std::string& message);

	protected:

		/**
		 * Creates a new connectionless client object.
		 */
		ConnectionlessClient();
};

inline ConnectionlessClient::SocketResult ConnectionlessClient::send(const Address4& address, const Port& port,const std::string& message)
{
	ocean_assert(strlen(message.c_str()) == message.length() && message.c_str()[message.length()] == '\0');
	return send(address, port, message.c_str(), message.length() + 1);
}

}

}

#endif // FACEBOOK_NETWORK_CONNECTIONLESS_CLIENT_H
