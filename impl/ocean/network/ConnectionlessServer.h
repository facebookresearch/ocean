/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_CONNECTIONLESS_SERVER_H
#define FACEBOOK_NETWORK_CONNECTIONLESS_SERVER_H

#include "ocean/network/Network.h"
#include "ocean/network/ConnectionlessClient.h"
#include "ocean/network/Server.h"

#include "ocean/base/Callback.h"

namespace Ocean
{

namespace Network
{

/**
 * This class is the base class for all connectionless server.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT ConnectionlessServer :
	virtual public ConnectionlessClient,
	virtual public Server
{
	public:

		/**
		 * Definition of a data callback function.
		 * Parameter 0 provides the address of the sender
		 * Parameter 1 provides the port of the sender
		 * Parameter 2 provides the buffer that has been received, must be copied
		 * Parameter 3 provides the size of the received buffer, in bytes
		 */
		typedef Callback<void, const Address4&, const Port&, const void*, const size_t> ReceiveCallback;

	public:

		/**
		 * Destructs a connectionless server object.
		 */
		~ConnectionlessServer() override;

		/**
		 * Sets the receive data callback function.
		 * @param callback The callback function to be called if a new message arrives
		 */
		inline void setReceiveCallback(const ReceiveCallback& callback);

	protected:

		/**
		 * Creates a new connectionless server object.
		 */
		ConnectionlessServer();

		/**
		 * The scheduler event function.
		 * Socket::onScheduler().
		 */
		bool onScheduler() override;

	protected:

		/// Data callback function called on new message arrivals.
		ReceiveCallback receiveCallback_;
};

inline void ConnectionlessServer::setReceiveCallback(const ReceiveCallback& callback)
{
	const ScopedLock scopedLock(lock_);

	receiveCallback_ = callback;
}

}

}

#endif // FACEBOOK_NETWORK_CONNECTIONLESS_SERVER_H
