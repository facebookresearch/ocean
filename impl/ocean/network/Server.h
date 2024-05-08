/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_SERVER_H
#define FACEBOOK_NETWORK_SERVER_H

#include "ocean/network/Network.h"
#include "ocean/network/Port.h"
#include "ocean/network/Socket.h"

namespace Ocean
{

namespace Network
{

/**
 * This class is the base class for all server.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT Server : virtual public Socket
{
	public:

		/**
		 * Destructs a server object.
		 */
		~Server() override;

		/**
		 * Starts the server.
		 * Beware: If a specific listening port is expected the own port has to be set before
		 * @return True, if succeeded
		 */
		virtual bool start();

		/**
		 * Stops the server.
		 * @return True, if succeeded
		 */
		virtual bool stop();

	protected:

		/**
		 * Creates a new server object.
		 */
		Server();

	protected:

		/// The socket buffer of this server.
		Buffer buffer_;

		/// True, if the server scheduler is active.
		bool schedulerIsActive_ = false;
};

}

}

#endif // FACEBOOK_NETWORK_SERVER_H
