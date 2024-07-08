/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTNETWORK_TEST_TCP_CLIENT_H
#define META_OCEAN_TEST_TESTNETWORK_TEST_TCP_CLIENT_H

#include "ocean/test/testnetwork/TestNetwork.h"

#include "ocean/network/TCPServer.h"

namespace Ocean
{

namespace Test
{

namespace TestNetwork
{

/**
 * This class implements test for TCPClient.
 * @ingroup testnetwork
 */
class OCEAN_TEST_NETWORK_EXPORT TestTCPClient
{
	protected:

		/**
		 * Definition of a vector holding bytes.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * This class implements a receiver.
		 */
		class Receiver
		{
			public:

				/**
				 * Event function for connection requests.
				 * @param senderAddress The address of the sender
				 * @param senderPort The port of the sender
				 * @param connectionId The id of the connection
				 */
				bool onConnectionRequest(const Network::Address4& senderAddress, const Network::Port& senderPort, const Network::TCPServer::ConnectionId connectionId);

				/**
				 * Event function for a disconnected connection.
				 * @param connectionId The id of the disconnected connection
				 */
				void onConnectionDisconnected(const Network::TCPServer::ConnectionId connectionId);

				/**
				 * Event function for receiving data.
				 * @param connectionId The id of the connection from which the data has been received
				 * @param data The data that has been received
				 * @param size The number of bytes
				 */
				void onReceive(const Network::TCPServer::ConnectionId connectionId, const void* data, const size_t size);

			public:

				/// The number of connection requests.
				unsigned int numberConnectionRequests_ = 0u;

				/// The number of disconnections.
				unsigned int numberDisconnections_ = 0u;

				/// The memory buffers.
				std::vector<Buffer> buffers_;
		};

	public:

		/**
		 * Tests all TCPClient functions.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests sending and receiving data.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testSendReceive(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTNETWORK_TEST_TCP_CLIENT_H
