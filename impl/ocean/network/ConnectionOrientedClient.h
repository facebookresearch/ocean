/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_CONNECTION_ORIENTED_CLIENT_H
#define FACEBOOK_NETWORK_CONNECTION_ORIENTED_CLIENT_H

#include "ocean/network/Network.h"
#include "ocean/network/Address4.h"
#include "ocean/network/Client.h"

#include "ocean/base/Callback.h"

namespace Ocean
{

namespace Network
{

/**
 * This class is the base class for all connection oriented clients.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT ConnectionOrientedClient : virtual public Client
{
	public:

		/**
		 * Definition of a data receive callback function.
		 * The first parameter provides the received data, which must be copied
		 * The second parameter provides the size of the received data, in bytes
		 */
		typedef Callback<void, const void*, const size_t> ReceiveCallback;

		/**
		 * Definition of a disconnection callback function.
		 */
		typedef Callback<void> DisconnectCallback;

	public:

		/**
		 * Connects to a connection-oriented server.
		 * @param address Remote address of the server to connect
		 * @param port Remote port of the server to connect
		 * @param timeout The timeout in milliseconds, with range [0, infinity)
		 * @return True, if succeeded
		 */
		virtual bool connect(const Address4& address, const Port& port, const unsigned int timeout = 1000u);

		/**
		 * Reconnects the client by the usage of the most recent address and port.
		 * @param timeout The timeout in milliseconds, with range [0, infinity)
		 * @return True, if succeeded
		 */
		virtual bool connect(const unsigned int timeout = 1000u);

		/**
		 * Disconnects the client.
		 * @return True, if the client was connected
		 */
		virtual bool disconnect();

		/**
		 * Returns whether this client is connected.
		 * @return True, if so
		 */
		virtual bool isConnected() const;

		/**
		 * Sends data via the established connection.
		 * If the resource is busy SR_FAILED will be returned and the caller needs to send the data later<br>
		 * @param data The data to send, can be nullptr if 'size == 0'
		 * @param size The size of the data to send, in bytes, with range [0, infinity)
		 * @return SR_SUCCEEDED, if succeeded
		 */
		virtual SocketResult send(const void* data, const size_t size);

		/**
		 * Sets a message via the established connection.
		 * @param message The message to send, must be valid
		 * @return SR_SUCCEEDED, if succeeded
		 */
		inline SocketResult send(const std::string& message);

		/**
		 * Returns the (remote) receiver address.
		 * @return Receiver address
		 */
		inline Address4 receiverAddress() const;

		/**
		 * Returns the (remote) receiver port.
		 * @return Receiver port
		 */
		inline Port receiverPort() const;

		/**
		 * Sets the receive callback function.
		 * @param callback The callback function to set
		 */
		inline void setReceiveCallback(const ReceiveCallback& callback);

		/**
		 * Sets the disconnect callback function.
		 * @param callback The callback function to set
		 */
		inline void setDisconnectCallback(const DisconnectCallback& callback);

	protected:

		/**
		 * Creates a new connection oriented client.
		 */
		ConnectionOrientedClient();

		/**
		 * Destructs a connection oriented client.
		 */
		~ConnectionOrientedClient() override;

		/**
		 * The scheduler event function.
		 * Socket::onScheduler().
		 */
		bool onScheduler() override;

		/**
		 * Internal event function to send data.
		 * @param data The data to send, must be valid
		 * @param size The size of the data in bytes, with range [1, infinity)
		 * @return The number of bytes which have been sent, with range [0, size]
		 */
		virtual size_t onSend(const void* data, const size_t size);

		/**
		 * Internal event function for received data.
		 * @param data The data that has been received, must be valid
		 * @param size The size of the data in bytes, with range [1, infinity)
		 */
		virtual void onReceived(const void* data, const size_t size);

	protected:

		/// Receiver address
		Address4 receiverAddress_;

		/// Receiver port
		Port receiverPort_;

		/// Receive callback function.
		ReceiveCallback receiveCallback_;

		/// Disconnect callback function.
		DisconnectCallback disconnectCallback_;

		/// Determines whether a connection is established.
		bool isConnected_ = false;
};

inline ConnectionOrientedClient::SocketResult ConnectionOrientedClient::send(const std::string& message)
{
	ocean_assert(strlen(message.c_str()) == message.length() && message.c_str()[message.length()] == '\0');
	return send(message.c_str(), message.length() + 1);
}

inline Address4 ConnectionOrientedClient::receiverAddress() const
{
	const ScopedLock scopedLock(lock_);
	return receiverAddress_;
}

inline Port ConnectionOrientedClient::receiverPort() const
{
	const ScopedLock scopedLock(lock_);
	return receiverPort_;
}

inline void ConnectionOrientedClient::setReceiveCallback(const ReceiveCallback& callback)
{
	const ScopedLock scopedLock(lock_);
	receiveCallback_ = callback;
}

inline void ConnectionOrientedClient::setDisconnectCallback(const DisconnectCallback& callback)
{
	const ScopedLock scopedLock(lock_);
	disconnectCallback_ = callback;
}

}

}

#endif // FACEBOOK_NETWORK_CONNECTION_ORIENTED_CLIENT_H
