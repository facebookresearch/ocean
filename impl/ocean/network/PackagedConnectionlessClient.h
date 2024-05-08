/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_PACKAGED_CONNECTIONLESS_CLIENT_H
#define FACEBOOK_NETWORK_PACKAGED_CONNECTIONLESS_CLIENT_H

#include "ocean/network/Network.h"
#include "ocean/network/Address4.h"
#include "ocean/network/Client.h"
#include "ocean/network/PackagedSocket.h"
#include "ocean/network/Port.h"

namespace Ocean
{

namespace Network
{


/**
 * This class is the base class for all packaged connectionless clients.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT PackagedConnectionlessClient :
	virtual public Client,
	virtual public PackagedSocket
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

		/**
		 * Returns the maximal size of a single package for this client.
		 * @return Maximal package size in bytes.
		 */
		inline size_t maximalPackageSize() const;

	protected:

		/**
		 * Creates a new packaged connectionless client object.
		 */
		PackagedConnectionlessClient();

		/**
		 * Destructs a packaged connectionless client object.
		 */
		~PackagedConnectionlessClient() override;

	protected:

		/// Client message counter.
		MessageId messageCounter_ = 0u;

		/// Maximal package size of this connectionless socket (including the header).
		size_t maximalPackageSize_ = 0;

		/// Intermediate buffer storing individual parts of a large message.
		Buffer clientPackageBuffer_;
};

inline PackagedConnectionlessClient::SocketResult PackagedConnectionlessClient::send(const Address4& address, const Port& port, const std::string& message)
{
	ocean_assert(strlen(message.c_str()) == message.length() && message.c_str()[message.length()] == '\0');
	return send(address, port, message.c_str(), message.length() + 1);
}

inline size_t PackagedConnectionlessClient::maximalPackageSize() const
{
	return maximalPackageSize_;
}

}

}

#endif // FACEBOOK_NETWORK_PACKAGED_CONNECTIONLESS_CLIENT_H
