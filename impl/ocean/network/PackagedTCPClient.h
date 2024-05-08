/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_PACKAGED_TCP_CLIENT_H
#define FACEBOOK_NETWORK_PACKAGED_TCP_CLIENT_H

#include "ocean/network/Network.h"
#include "ocean/network/TCPClient.h"
#include "ocean/network/PackagedSocket.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements a TCP client with internal package handling.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT PackagedTCPClient :
	virtual public TCPClient,
	virtual protected PackagedSocket
{
	public:

		/**
		 * Creates a new TCP client.
		 */
		PackagedTCPClient() = default;

		/**
		 * Destructs a TCP client.
		 */
		~PackagedTCPClient() override = default;

	protected:

		/**
		 * Internal event function to send data.
		 * @see ConnectionOrientedClient::onSend().
		 */
		size_t onSend(const void* data, const size_t size) override;

		/**
		 * Internal event function for received data.
		 * @see ConnectionOrientedClient::onReceived().
		 */
		void onReceived(const void* data, const size_t size) override;

	protected:

		/// The queue holding all memory blocks recently received and not yet process.
		MemoryBlockQueue memoryQueue_;

		/// The memory block to be filled.
		MemoryBlock currentMemory_;

		/// The memory block for the package header.
		MemoryBlock currentPackageHeaderMemory_;
};

}

}

#endif // FACEBOOK_NETWORK_PACKAGED_TCP_CLIENT_H
