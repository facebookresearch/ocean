/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testnetwork/TestPackagedTCPClient.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/network/PackagedTCPClient.h"

namespace Ocean
{

namespace Test
{

namespace TestNetwork
{

void TestPackagedTCPClient::ClientReceiver::onReceive(const void* data, const size_t size)
{
	std::vector<uint8_t> buffer(size);
	memcpy(buffer.data(), data, size);

	buffers_.emplace_back(std::move(buffer));
}

bool TestPackagedTCPClient::ServerReceiver::onConnectionRequest(const Network::Address4& /*senderAddress*/, const Network::Port& /*senderPort*/, const Network::PackagedTCPServer::ConnectionId connectionId)
{
	connectionId_ = connectionId;
	++numberConnectionRequests_;

	return true;
}

void TestPackagedTCPClient::ServerReceiver::onConnectionDisconnected(const Network::PackagedTCPServer::ConnectionId /*connectionId*/)
{
	++numberDisconnections_;
}

void TestPackagedTCPClient::ServerReceiver::onReceive(const Network::PackagedTCPServer::ConnectionId /*connectionId*/, const void* data, const size_t size)
{
	std::vector<uint8_t> buffer(size);
	memcpy(buffer.data(), data, size);

	buffers_.emplace_back(std::move(buffer));
}

bool TestPackagedTCPClient::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   PackagedTCPClient test:   ---";
	Log::info() << " ";

	allSucceeded = testSendReceive(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "PackagedTCPClient test succeeded.";
	}
	else
	{
		Log::info() << "PackagedTCPClient test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestPackagedTCPClient, SendReceive)
{
	EXPECT_TRUE(TestPackagedTCPClient::testSendReceive(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestPackagedTCPClient::testSendReceive(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "PackagedTCPClient & PackagedTCPServer test:";

	bool allSucceeded = true;
	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		Network::PackagedTCPServer tcpServer;

		ServerReceiver serverReceiver;
		tcpServer.setConnectionRequestCallback(Network::TCPServer::ConnectionRequestCallback::create(serverReceiver, &ServerReceiver::onConnectionRequest));
		tcpServer.setDisconnectCallback(Network::TCPServer::DisconnectCallback::create(serverReceiver, &ServerReceiver::onConnectionDisconnected));
		tcpServer.setReceiveCallback(Network::TCPServer::ReceiveCallback::create(serverReceiver, &ServerReceiver::onReceive));

		if (!tcpServer.start())
		{
			allSucceeded = false;
		}

		const Network::Port serverPort = tcpServer.port();

		Network::PackagedTCPClient tcpClient;

		ClientReceiver clientReceiver;
		tcpClient.setReceiveCallback(Network::TCPClient::ReceiveCallback::create(clientReceiver, &ClientReceiver::onReceive));

		if (!tcpClient.connect(Network::Address4::localHost(), serverPort))
		{
			allSucceeded = false;
		}

		const unsigned int numberClientSendRequests = RandomI::random(randomGenerator, 1u, 10u);

		std::vector<Buffer> clientSendBuffers;

		for (unsigned int n = 0u; n < numberClientSendRequests; ++n)
		{
			const unsigned int bytes = RandomI::random(randomGenerator, 1u, 20000u);

			Buffer buffer(bytes);
			for (uint8_t& element : buffer)
			{
				element = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			if (tcpClient.send(buffer.data(), buffer.size()) != Network::PackagedTCPClient::SR_SUCCEEDED)
			{
				allSucceeded = false;
			}

			Thread::sleep(10u);

			clientSendBuffers.emplace_back(std::move(buffer));
		}

		Thread::sleep(50u);

		const unsigned int numberServerSendRequests = RandomI::random(randomGenerator, 1u, 10u);
		std::vector<Buffer> serverSendBuffers;

		if (serverReceiver.connectionId_ != Network::PackagedTCPServer::invalidConnectionId())
		{
			for (unsigned int n = 0u; n < numberServerSendRequests; ++n)
			{
				const unsigned int bytes = RandomI::random(randomGenerator, 1u, 20000u);

				Buffer buffer(bytes);
				for (uint8_t& element : buffer)
				{
					element = uint8_t(RandomI::random(randomGenerator, 255u));
				}

				if (tcpServer.send(serverReceiver.connectionId_, buffer.data(), buffer.size()) != Network::PackagedTCPClient::SR_SUCCEEDED)
				{
					allSucceeded = false;
				}

				Thread::sleep(10u);

				serverSendBuffers.emplace_back(std::move(buffer));
			}
		}
		else
		{
			allSucceeded = false;
		}

		Thread::sleep(50u);

		const bool disconnectClient = RandomI::random(randomGenerator, 1u) == 0u;

		if (disconnectClient)
		{
			if (!tcpClient.disconnect())
			{
				allSucceeded = false;
			}
		}
		else
		{
			if (!tcpServer.stop())
			{
				allSucceeded = false;
			}
		}

		Thread::sleep(50u);

		if (serverReceiver.numberConnectionRequests_ != 1u)
		{
			allSucceeded = false;
		}

		if (disconnectClient && serverReceiver.numberDisconnections_ != 1u)
		{
			allSucceeded = false;
		}

		if (clientSendBuffers.size() != serverReceiver.buffers_.size())
		{
			allSucceeded = false;
		}
		else
		{
			for (size_t n = 0; n < clientSendBuffers.size(); ++n)
			{
				const Buffer& sourceBuffer = clientSendBuffers[n];
				const Buffer& targetBuffer = serverReceiver.buffers_[n];

				if (sourceBuffer.size() != targetBuffer.size())
				{
					allSucceeded = false;
				}
				else
				{
					if (memcmp(sourceBuffer.data(), targetBuffer.data(), sourceBuffer.size()) != 0)
					{
						allSucceeded = false;
					}
				}
			}
		}

		if (serverSendBuffers.size() != clientReceiver.buffers_.size())
		{
			allSucceeded = false;
		}
		else
		{
			for (size_t n = 0; n < serverSendBuffers.size(); ++n)
			{
				const Buffer& sourceBuffer = serverSendBuffers[n];
				const Buffer& targetBuffer = clientReceiver.buffers_[n];

				if (sourceBuffer.size() != targetBuffer.size())
				{
					allSucceeded = false;
				}
				else
				{
					if (memcmp(sourceBuffer.data(), targetBuffer.data(), sourceBuffer.size()) != 0)
					{
						allSucceeded = false;
					}
				}
			}
		}

#ifdef OCEAN_USE_GTEST
		// one execution is enough for GTest
		break;
#endif
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

}

}

}
