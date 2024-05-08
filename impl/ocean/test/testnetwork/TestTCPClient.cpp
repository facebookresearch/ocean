/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testnetwork/TestTCPClient.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/network/TCPClient.h"

namespace Ocean
{

namespace Test
{

namespace TestNetwork
{

bool TestTCPClient::Receiver::onConnectionRequest(const Network::Address4& /*senderAddress*/, const Network::Port& /*senderPort*/, const Network::TCPServer::ConnectionId /*connectionId*/)
{
	++numberConnectionRequests_;

	return true;
}

void TestTCPClient::Receiver::onConnectionDisconnected(const Network::TCPServer::ConnectionId /*connectionId*/)
{
	++numberDisconnections_;
}

void TestTCPClient::Receiver::onReceive(const Network::TCPServer::ConnectionId /*connectionId*/, const void* data, const size_t size)
{
	std::vector<uint8_t> buffer(size);
	memcpy(buffer.data(), data, size);

	buffers_.emplace_back(std::move(buffer));
}

bool TestTCPClient::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   TCPClient test:   ---";
	Log::info() << " ";

	allSucceeded = testSendReceive(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "TCPClient test succeeded.";
	}
	else
	{
		Log::info() << "TCPClient test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestTCPClient, SendReceive)
{
	EXPECT_TRUE(TestTCPClient::testSendReceive(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestTCPClient::testSendReceive(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "TCPClient & TCPServer test:";

	bool allSucceeded = true;
	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		Network::TCPServer tcpServer;

		Receiver receiver;
		tcpServer.setConnectionRequestCallback(Network::TCPServer::ConnectionRequestCallback::create(receiver, &Receiver::onConnectionRequest));
		tcpServer.setDisconnectCallback(Network::TCPServer::DisconnectCallback::create(receiver, &Receiver::onConnectionDisconnected));
		tcpServer.setReceiveCallback(Network::TCPServer::ReceiveCallback::create(receiver, &Receiver::onReceive));

		if (!tcpServer.start())
		{
			allSucceeded = false;
		}

		const Network::Port serverPort = tcpServer.port();

		Network::TCPClient tcpClient;

		if (!tcpClient.connect(Network::Address4::localHost(), serverPort))
		{
			allSucceeded = false;
		}

		const unsigned int numberSendRequests = RandomI::random(randomGenerator, 1u, 10u);

		std::vector<Buffer> buffers;

		for (unsigned int n = 0u; n < numberSendRequests; ++n)
		{
			const unsigned int bytes = RandomI::random(randomGenerator, 1u, 20000u);

			Buffer buffer(bytes);
			for (uint8_t& element : buffer)
			{
				element = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			if (tcpClient.send(buffer.data(), buffer.size()) != Network::TCPClient::SR_SUCCEEDED)
			{
				allSucceeded = false;
			}

			Thread::sleep(10u);

			buffers.emplace_back(std::move(buffer));
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

		if (receiver.numberConnectionRequests_ != 1u)
		{
			allSucceeded = false;
		}

		if (disconnectClient && receiver.numberDisconnections_ != 1u)
		{
			allSucceeded = false;
		}

		size_t sourceBufferSize = 0;
		for (const Buffer& buffer : buffers)
		{
			sourceBufferSize += buffer.size();
		}

		size_t targetBufferSize = 0;
		for (const Buffer& buffer : receiver.buffers_)
		{
			targetBufferSize += buffer.size();
		}

		if (sourceBufferSize != targetBufferSize)
		{
			allSucceeded = false;
		}
		else
		{
			Buffer sourceBuffer(sourceBufferSize);
			uint8_t* sourceData = sourceBuffer.data();

			for (const Buffer& buffer : buffers)
			{
				memcpy(sourceData, buffer.data(), buffer.size());
				sourceData += buffer.size();
			}
			ocean_assert(sourceData == sourceBuffer.data() + sourceBuffer.size());

			Buffer targetBuffer(targetBufferSize);
			uint8_t* targeteData = targetBuffer.data();

			for (const Buffer& buffer : receiver.buffers_)
			{
				memcpy(targeteData, buffer.data(), buffer.size());
				targeteData += buffer.size();
			}
			ocean_assert(targeteData == targetBuffer.data() + targetBuffer.size());

			ocean_assert(sourceBuffer.size() == targetBuffer.size());

			if (memcmp(sourceBuffer.data(), targetBuffer.data(), sourceBuffer.size()) != 0)
			{
				allSucceeded = false;
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
