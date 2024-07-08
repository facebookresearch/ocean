/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_CLIENT_H
#define FACEBOOK_NETWORK_CLIENT_H

#include "ocean/network/Network.h"
#include "ocean/network/Socket.h"

namespace Ocean
{

namespace Network
{

/**
 * This class is the base class for all clients.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT Client : virtual public Socket
{
	protected:

		/**
		 * Creates a new client.
		 */
		Client();

		/**
		 * Destructs a client.
		 */
		~Client() override;

	protected:

		/// The socket buffer of this client.
		Buffer socketBuffer_;
};

}

}

#endif // FACEBOOK_NETWORK_CLIENT_H
