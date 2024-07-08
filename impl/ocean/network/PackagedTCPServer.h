/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_PACKAGED_TCP_SERVER_H
#define FACEBOOK_NETWORK_PACKAGED_TCP_SERVER_H

#include "ocean/network/Network.h"
#include "ocean/network/TCPServer.h"
#include "ocean/network/PackagedSocket.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements a TCP server with internal package handling
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT PackagedTCPServer :
	virtual public TCPServer,
	virtual protected PackagedSocket
{
	protected:

		/**
		 * Definition of a container
		 */
		class ConnectionMemory
		{
			public:

				/// The queue holding all memory blocks recently received and not yet process.
				MemoryBlockQueue memoryQueue_;

				/// The memory block to be filled.
				MemoryBlock currentMemory_;

				/// The memory block for the package header.
				MemoryBlock currentPackageHeaderMemory_;
		};

		/**
		 * Definition of an unordered map mapping connection ids to ConnectionMemory objects.
		 */
		typedef std::unordered_map<ConnectionId, ConnectionMemory> ConnectionMemoryMap;

	public:

		/**
		 * Creates a new TCP server.
		 */
		PackagedTCPServer() = default;

		/**
		 * Destructs a TCP server.
		 */
		~PackagedTCPServer() override = default;

	protected:

		/**
		 * Internal event function to send data.
		 * @see ConnectionOrientedServer::onSend().
		 */
		size_t onSend(const ConnectionId connectionId, const void* data, const size_t size) override;

		/**
		 * Internal event function for received data.
		 * @see ConnectionOrientedServer::onReceived().
		 */
		void onReceived(const ConnectionId connectionId, const void* data, const size_t size) override;

	protected:

		/// The map mapping connection ids to ConnectionMemory objects.
		ConnectionMemoryMap connectionMemoryMap_;
};

}

}

#endif // FACEBOOK_NETWORK_PACKAGED_TCP_SERVER_H
