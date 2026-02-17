/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testnetwork/TestPackagedTCPClient.h"

#include "ocean/test/TestResult.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/network/PackagedTCPClient.h"

#include "ocean/test/Validation.h"

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

bool TestPackagedTCPClient::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("PackagedTCPClient test");
	Log::info() << " ";

	if (selector.shouldRun("sendreceive"))
	{
		testResult = testSendReceive(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

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
			OCEAN_SET_FAILED(validation);
		}

		const Network::Port serverPort = tcpServer.port();

		Network::PackagedTCPClient tcpClient;

		ClientReceiver clientReceiver;
		tcpClient.setReceiveCallback(Network::TCPClient::ReceiveCallback::create(clientReceiver, &ClientReceiver::onReceive));

		if (!tcpClient.connect(Network::Address4::localHost(), serverPort))
		{
			OCEAN_SET_FAILED(validation);
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

			OCEAN_EXPECT_EQUAL(validation, tcpClient.send(buffer.data(), buffer.size()), Network::PackagedTCPClient::SR_SUCCEEDED);

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

				OCEAN_EXPECT_EQUAL(validation, tcpServer.send(serverReceiver.connectionId_, buffer.data(), buffer.size()), Network::PackagedTCPClient::SR_SUCCEEDED);

				Thread::sleep(10u);

				serverSendBuffers.emplace_back(std::move(buffer));
			}
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}

		Thread::sleep(50u);

		const bool disconnectClient = RandomI::boolean(randomGenerator);

		if (disconnectClient)
		{
			OCEAN_EXPECT_TRUE(validation, tcpClient.disconnect());
		}
		else
		{
			OCEAN_EXPECT_TRUE(validation, tcpServer.stop());
		}

		Thread::sleep(50u);

		OCEAN_EXPECT_EQUAL(validation, serverReceiver.numberConnectionRequests_, 1u);

		if (disconnectClient)
		{
			OCEAN_EXPECT_EQUAL(validation, serverReceiver.numberDisconnections_, 1u);
		}

		OCEAN_EXPECT_EQUAL(validation, clientSendBuffers.size(), serverReceiver.buffers_.size());

		if (clientSendBuffers.size() == serverReceiver.buffers_.size())
		{
			for (size_t n = 0; n < clientSendBuffers.size(); ++n)
			{
				const Buffer& sourceBuffer = clientSendBuffers[n];
				const Buffer& targetBuffer = serverReceiver.buffers_[n];

				OCEAN_EXPECT_EQUAL(validation, sourceBuffer.size(), targetBuffer.size());

				if (sourceBuffer.size() == targetBuffer.size())
				{
					OCEAN_EXPECT_EQUAL(validation, memcmp(sourceBuffer.data(), targetBuffer.data(), sourceBuffer.size()), 0);
				}
			}
		}

		OCEAN_EXPECT_EQUAL(validation, serverSendBuffers.size(), clientReceiver.buffers_.size());

		if (serverSendBuffers.size() == clientReceiver.buffers_.size())
		{
			for (size_t n = 0; n < serverSendBuffers.size(); ++n)
			{
				const Buffer& sourceBuffer = serverSendBuffers[n];
				const Buffer& targetBuffer = clientReceiver.buffers_[n];

				OCEAN_EXPECT_EQUAL(validation, sourceBuffer.size(), targetBuffer.size());

				if (sourceBuffer.size() == targetBuffer.size())
				{
					OCEAN_EXPECT_EQUAL(validation, memcmp(sourceBuffer.data(), targetBuffer.data(), sourceBuffer.size()), 0);
				}
			}
		}

#ifdef OCEAN_USE_GTEST
		// one execution is enough for GTest
		break;
#endif
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
