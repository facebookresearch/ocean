/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_CONNECTION_ORIENTED_SERVER_H
#define FACEBOOK_NETWORK_CONNECTION_ORIENTED_SERVER_H

#include "ocean/network/Network.h"
#include "ocean/network/Address4.h"
#include "ocean/network/Server.h"

#include "ocean/base/Callback.h"

namespace Ocean
{

namespace Network
{

/**
 * This class is the base class for all connection oriented servers.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT ConnectionOrientedServer : virtual public Server
{
	public:

		/**
		 * Definition of a connection id.
		 */
		typedef unsigned int ConnectionId;

		/**
		 * Returns an invalid connection id.
		 * @return Invalid connection id
		 */
		static constexpr ConnectionId invalidConnectionId();

		/**
		 * Definition of a connection request callback function.<br>
		 * Parameter 0 provides the address of the requesting client.<br>
		 * Parameter 1 provides the port of the requesting client.<br>
		 * Parameter 2 provides the potential connection id if the connection is accepted.<br>
		 * Return True, to accept the connection.<br>
		 */
		typedef Callback<bool, const Address4&, const Port&, const ConnectionId> ConnectionRequestCallback;

		/**
		 * Definition of a disconnect callback function.<br>
		 * Parameter 0 provides the id of the connection which has been disconnected
		 */
		typedef Callback<void, const ConnectionId> DisconnectCallback;

		/**
		 * Definition of a receive callback function.
		 * Parameter 0 provides the id of the connection from which the data is provided.<br>
		 * Parameter 1 provides the buffer which has been received, must be copied.<br>
		 * Parameter 2 provides the size of the buffer which has been received, in bytes
		 */
		typedef Callback<void, const ConnectionId, const void*, const size_t> ReceiveCallback;

	protected:

		/**
		 * Definition of a connection object holding the socket and remote address and port.
		 */
		class ConnectionObject
		{
			public:

				/**
				 * Creates an empty connection object.
				 */
				inline ConnectionObject() = default;

				/**
				 * Creates a new connection object.
				 */
				inline ConnectionObject(const SocketId socketId, const Address4& address, const Port& port);

				/**
				 * Returns the socket id of this connection object.
				 * @return Socket id
				 */
				inline SocketId id() const;

				/**
				 * Returns the remote address of this connection object.
				 * @return Remote address
				 */
				inline const Address4& address() const;

				/**
				 * Returns the remote port of this connection object.
				 * @return Remote port
				 */
				inline const Port& port() const;

			protected:

				/// Socket id of the connection.
				SocketId socketId_ = invalidSocketId();

				/// Remote address of the connection.
				Address4 address_;

				/// Remote port of the connection.
				Port port_;
		};

		/**
		 * Definition of a map mapping connection ids to sockets.
		 */
		typedef std::unordered_map<ConnectionId, ConnectionObject> ConnectionMap;

	public:

		/**
		 * Sends data over a specified connection.
		 * @param connectionId The id of the connection which is used to send the data
		 * @param data The data to send, can be nullptr if 'size == 0'
		 * @param size The size of the data to send, in bytes, with range [0, infinity)
		 * @return SR_SUCCEEDED, if succeeded
		 */
		virtual SocketResult send(const ConnectionId connectionId, const void* data, const size_t size);

		/**
		 * Sends a message over a specified connection.
		 * @param connectionId The id of the connection which is used to send the data
		 * @param message The message to send, must not be empty
		 * @return SR_SUCCEEDED, if succeeded
		 */
		virtual SocketResult send(const ConnectionId connectionId, const std::string& message);

		/**
		 * Disconnects a specified connection.
		 * @param connectionId The id of the connection which will be disconnected
		 * @return True, if the connection was established
		 */
		virtual bool disconnect(const ConnectionId connectionId) = 0;

		/**
		 * Returns the number of active connections of this server.
		 * @return Number of active connections
		 */
		virtual size_t connections() const;

		/**
		 * Returns the remote address and port of a specified connection.
		 * @param connectionId The id of the connection from which the address and port will be returned
		 * @param address Returning remote address
		 * @param port Returning remote port
		 * @return True, if the connection exists
		 */
		bool connectionProperties(const ConnectionId connectionId, Address4& address, Port& port);

		/**
		 * Sets the connection request callback function.
		 * @param callback The callback function to set
		 */
		inline void setConnectionRequestCallback(const ConnectionRequestCallback& callback);

		/**
		 * Sets the disconnect callback function.
		 * @param callback The callback function to set
		 */
		inline void setDisconnectCallback(const DisconnectCallback& callback);

		/**
		 * Sets the receive callback function.
		 * @param callback The callback function to set
		 */
		inline void setReceiveCallback(const ReceiveCallback& callback);

	protected:

		/**
		 * Creates a new connection oriented server.
		 */
		ConnectionOrientedServer();

		/**
		 * Destructs a connection oriented server.
		 */
		~ConnectionOrientedServer() override;

		/**
		 * The scheduler event function.
		 * Socket::onScheduler().
		 */
		bool onScheduler() override;

		/**
		 * Internal event function to send data.
		 * @param connectionId The id of the connection
		 * @param data The data to send, must be valid
		 * @param size The size of the data in bytes, with range [1, infinity)
		 * @return The number of bytes which have been sent, with range [0, size]
		 */
		virtual size_t onSend(const ConnectionId connectionId, const void* data, const size_t size);

		/**
		 * Internal event function for received data.
		 * @param connectionId The id of the connection
		 * @param data The data that has been received, must be valid
		 * @param size The size of the data in bytes, with range [1, infinity)
		 */
		virtual void onReceived(const ConnectionId connectionId, const void* data, const size_t size);

	protected:

		/// Map holding all valid connections.
		ConnectionMap connectionMap_;

		/// Connection counter.
		ConnectionId connectionCounter_ = ConnectionId(invalidConnectionId() + 1);

		/// Connection request callback function.
		ConnectionRequestCallback connectionRequestCallback_;

		/// Disconnect callback function.
		DisconnectCallback disconnectCallback_;

		/// Receive callback function.
		ReceiveCallback receiveCallback_;
};

constexpr ConnectionOrientedServer::ConnectionId ConnectionOrientedServer::invalidConnectionId()
{
	return ConnectionId(-1);
}

inline ConnectionOrientedServer::ConnectionObject::ConnectionObject(const SocketId socketId, const Address4& address, const Port& port) :
	socketId_(socketId),
	address_(address),
	port_(port)
{
	// nothing to do here
}

inline ConnectionOrientedServer::SocketId ConnectionOrientedServer::ConnectionObject::id() const
{
	return socketId_;
}

inline const Address4& ConnectionOrientedServer::ConnectionObject::address() const
{
	return address_;
}

inline const Port& ConnectionOrientedServer::ConnectionObject::port() const
{
	return port_;
}

inline void ConnectionOrientedServer::setConnectionRequestCallback(const ConnectionRequestCallback& callback)
{
	const ScopedLock scopedLock(lock_);
	connectionRequestCallback_ = callback;
}

inline void ConnectionOrientedServer::setDisconnectCallback(const DisconnectCallback& callback)
{
	const ScopedLock scopedLock(lock_);
	disconnectCallback_ = callback;
}

inline void ConnectionOrientedServer::setReceiveCallback(const ReceiveCallback& callback)
{
	const ScopedLock scopedLock(lock_);
	receiveCallback_ = callback;
}

}

}

#endif // FACEBOOK_NETWORK_CONNECTION_ORIENTED_SERVER_H
