/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_SOCKET_H
#define FACEBOOK_NETWORK_SOCKET_H

#include "ocean/network/Network.h"
#include "ocean/network/Address4.h"
#include "ocean/network/NetworkResource.h"
#include "ocean/network/Port.h"

#include "ocean/base/Lock.h"

#include <vector>

namespace Ocean
{

namespace Network
{

/**
 * This class is the base class for all sockets.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT Socket
{
	friend class SocketScheduler;
	friend class Resolver;

	public:

#ifdef _WINDOWS

		/**
		 * Definition of a socket id.
		 */
		typedef SOCKET SocketId;

#else
		/**
		 * Definition of a socket id.
		 */
		typedef int SocketId;

#endif

		/**
		 * Returns an invalid socket id.
		 * @return Invalid socket id
		 */
		static constexpr SocketId invalidSocketId();

		/**
		 * Definition of individual result values.
		 */
		enum SocketResult
		{
			/// The function succeeded.
			SR_SUCCEEDED = 0,
			/// The function failed.
			SR_FAILED,
			/// The function did not succeed as the resource was busy (not free, or would have blocked if configurated as blocking).
			SR_BUSY,
			/// The function could not succeed as the resource was not connected.
			SR_NOT_CONNECTED
		};

		/**
		 * Definition of a vector holding 8 bit values.
		 */
		typedef std::vector<uint8_t> Buffer;

	public:

		/**
		 * Destructs a socket object.
		 */
		virtual ~Socket();

		/**
		 * Returns the socket id.
		 * @return Socket id
		 */
		inline SocketId id() const;

		/**
		 * Returns the own address of this socket.
		 * If the systems supports more than one network address use this function to determine which address is used for this socket.
		 * @return Own address
		 */
		Address4 address() const;

		/**
		 * Returns the own port of this socket.
		 * @return Own port
		 */
		Port port() const;

		/**
		 * Sets the own address of this socket.
		 * If the systems supports more than one network address use this function to define which address to use for this socket.<br>
		 * However, usually is not necessary to define the local address.
		 * @param address The address to use for this socket
		 * @return True, if succeeded
		 */
		virtual bool setAddress(const Address4& address);

		/**
		 * Sets the own port of this socket.
		 * @param port Own port to set
		 * @return True, if succeeded
		 */
		virtual bool setPort(const Port& port);

		/**
		 * Returns whether this socket is valid.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Sets the blocking mode of a socket.
		 * @param socketId The id of the socket for which the mode will be set, must be valid
		 * @param blocking True, if the socket will block
		 * @return True, if succeeded
		 */
		static bool setBlockingMode(const SocketId socketId, const bool blocking);

	protected:

		/**
		 * Creates a new socket object.
		 */
		Socket();

		/**
		 * Disabled copy constructor.
		 * @param object The object which would be copied
		 */
		Socket(const Socket& object) = delete;

		/**
		 * Sets the blocking mode of this socket.
		 * @param blocking True, if the socket will block
		 * @return True, if succeeded
		 */
		bool setBlockingMode(const bool blocking);

		/**
		 * Returns the maximal message size in bytes.
		 * Beware: Connection oriented socket have no message restrictions, in those cases the specified default value will be returned.
		 * @param defaultSize Default message size for connection oriented sockets
		 * @return Maximal message size
		 */
		size_t maximalMessageSize(const size_t defaultSize = 65536);

		/**
		 * Releases the socket.
		 * @return True, if succeeded
		 */
		bool releaseSocket();

		/**
		 * The scheduler event function.
		 * @return True, if the event function was busy; False, if the event function did nothing
		 */
		virtual bool onScheduler();

		/**
		 * Disabled copy operator.
		 * @param object The object which would be copied
		 * @return Reference to this object
		 */
		Socket& operator=(const Socket& object) = delete;

	protected:

		/// Socket id.
		SocketId socketId_ = invalidSocketId();

		/// Socket lock.
		mutable Lock lock_;

		/// The network resource object.
		NetworkResource networkResource_;
};

constexpr Socket::SocketId Socket::invalidSocketId()
{
#ifdef _WINDOWS
	return INVALID_SOCKET;
#else
	return SocketId(-1);
#endif
}

inline Socket::SocketId Socket::id() const
{
	return socketId_;
}

inline Socket::operator bool() const
{
	return socketId_ != invalidSocketId();
}

}

}

#endif // FACEBOOK_NETWORK_SOCKET_H
