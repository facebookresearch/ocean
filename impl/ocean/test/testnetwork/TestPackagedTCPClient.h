/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTNETWORK_TEST_PACKAGED_TCP_CLIENT_H
#define META_OCEAN_TEST_TESTNETWORK_TEST_PACKAGED_TCP_CLIENT_H

#include "ocean/test/testnetwork/TestNetwork.h"

#include "ocean/network/PackagedTCPServer.h"

namespace Ocean
{

namespace Test
{

namespace TestNetwork
{

/**
 * This class implements test for PackagedTCPClient.
 * @ingroup testnetwork
 */
class OCEAN_TEST_NETWORK_EXPORT TestPackagedTCPClient
{
	protected:

		/**
		 * Definition of a vector holding bytes.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * This class implements a receiver for clients.
		 */
		class OCEAN_TEST_NETWORK_EXPORT ClientReceiver
		{
			public:

				/**
				 * Event function for receiving data.
				 * @param data The data that has been received
				 * @param size The number of bytes
				 */
				void onReceive(const void* data, const size_t size);

			public:

				/// The memory buffers.
				std::vector<Buffer> buffers_;
		};

		/**
		 * This class implements a receiver for servers.
		 */
		class OCEAN_TEST_NETWORK_EXPORT ServerReceiver
		{
			public:

				/**
				 * Event function for connection requests.
				 * @param senderAddress The address of the sender
				 * @param senderPort The port of the sender
				 * @param connectionId The id of the connection
				 */
				bool onConnectionRequest(const Network::Address4& senderAddress, const Network::Port& senderPort, const Network::PackagedTCPServer::ConnectionId connectionId);

				/**
				 * Event function for a disconnected connection.
				 * @param connectionId The id of the disconnected connection
				 */
				void onConnectionDisconnected(const Network::PackagedTCPServer::ConnectionId connectionId);

				/**
				 * Event function for receiving data.
				 * @param connectionId The id of the connection from which the data has been received
				 * @param data The data that has been received
				 * @param size The number of bytes
				 */
				void onReceive(const Network::PackagedTCPServer::ConnectionId connectionId, const void* data, const size_t size);

			public:

				/// The id of the latest connection request.
				Network::PackagedTCPServer::ConnectionId connectionId_ = Network::PackagedTCPServer::invalidConnectionId();

				/// The number of connection requests.
				unsigned int numberConnectionRequests_ = 0u;

				/// The number of disconnections.
				unsigned int numberDisconnections_ = 0u;

				/// The memory buffers.
				std::vector<Buffer> buffers_;
		};

	public:

		/**
		 * Tests all PackagedTCPClient functions.
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

#endif // META_OCEAN_TEST_TESTNETWORK_TEST_PACKAGED_TCP_CLIENT_H
