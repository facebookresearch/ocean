/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_SOCKET_SCHEDULER_H
#define FACEBOOK_NETWORK_SOCKET_SCHEDULER_H

#include "ocean/network/Network.h"

#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

namespace Ocean
{

namespace Network
{

// Forward declaration.
class Socket;

/**
 * This class implements a high performance scheduler for socket events.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT SocketScheduler :
	public Singleton<SocketScheduler>,
	protected Thread
{
	friend class Socket;
	friend class Singleton<SocketScheduler>;

	protected:

		/**
		 * Definition of a set holding socket pointers.
		 */
		typedef std::unordered_set<Socket*> SocketSet;

	protected:

		/**
		 * Creates a new scheduler object.
		 */
		SocketScheduler();

		/**
		 * Destructs a scheduler object.
		 */
		~SocketScheduler() override;

		/**
		 * Register socket.
		 * @param socket The socket to be registered
		 */
		void registerSocket(Socket& socket);

		/**
		 * Unregister socket.
		 * Beware: The unregister process is not completed immediately!<br>
		 * @param socket The socket to be unregistered
		 */
		void unregisterSocket(Socket& socket);

		/**
		 * Checks whether a specific socket has been unregistered successfully.
		 * @param socket The socket which is checked
		 * @return True, if the socket has been unregistered successfully
		 */
		bool isSocketUnregistered(Socket& socket) const;

		/**
		 * The internal run function.
		 */
		void threadRun() override;

	protected:

		/// The active sockets of this scheduler.
		SocketSet activeSockets_;

		/// The set of sockets which are requested to be registered.
		SocketSet registerSockets_;

		/// The set of sockets which are requested to be unregistered.
		SocketSet unregisterSockets_;

		/// The lock of this scheduler.
		mutable Lock lock_;
};

}

}

#endif // FACEBOOK_NETWORK_SOCKET_SCHEDULER_H
