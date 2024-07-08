/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/MaintenanceTCPConnector.h"

namespace Ocean
{

namespace Network
{

MaintenanceTCPConnector::MaintenanceTCPConnector() :
	Thread("MaintenanceTCPConnector thread")
{
	startThread();

	tcpServer.setConnectionRequestCallback(TCPServer::ConnectionRequestCallback::create(*this, &MaintenanceTCPConnector::onConnectionRequest));
	tcpServer.setReceiveCallback(TCPServer::ReceiveCallback::create(*this, &MaintenanceTCPConnector::onReceiveTCPData));
}

MaintenanceTCPConnector::~MaintenanceTCPConnector()
{
	tcpServer.setConnectionRequestCallback(TCPServer::ConnectionRequestCallback());
	tcpServer.setReceiveCallback(TCPServer::ReceiveCallback());

	stopThreadExplicitly();
}

void MaintenanceTCPConnector::configurateAsSender(const Address4& address, const Port& port)
{
	ocean_assert(address.isValid() && port.isValid());

	const ScopedLock scopedLock(lock_);

	clientTargetAddress_ = address;
	clientTargetPort_ = port;

	tcpServer.stop();
	serverSourcePort_ = Port();
}

void MaintenanceTCPConnector::configurateAsReceiver(const Port& port)
{
	ocean_assert(port.isValid());

	const ScopedLock scopedLock(lock_);

	clientTargetAddress_ = Address4();
	clientTargetPort_ = Port();

	serverSourcePort_ = port;
	tcpServer.setPort(serverSourcePort_);
	tcpServer.start();
}

void MaintenanceTCPConnector::threadRun()
{
	static_assert(sizeof(unsigned long long) == 8, "Invalid data type!");

	std::string maintenanceName, maintenanceTag;
	unsigned long long maintenanceId = (unsigned long long)(-1);
	Maintenance::Buffer maintenanceBuffer;
	Timestamp maintenanceTimestamp;

	Buffers encodedReceiverBuffers;

	while (!shouldThreadStop())
	{
		TemporaryScopedLock temporalScopedLock(lock_);

		ocean_assert((!clientTargetAddress_.isValid() || !clientTargetPort_.isValid()) || !serverSourcePort_.isValid());

		// check whether we are the sender or a receiver
		if (clientTargetAddress_.isValid() && clientTargetPort_.isValid())
		{
			// we are a sender

			if (!encodedSenderBuffer_.empty() || Maintenance::get().receive(maintenanceName, maintenanceId, maintenanceTag, maintenanceBuffer, maintenanceTimestamp))
			{
				temporalScopedLock.release();

				if (encodedSenderBuffer_.empty())
				{
					encodeData(maintenanceName, maintenanceId, maintenanceTag, maintenanceBuffer, maintenanceTimestamp, 8, encodedSenderBuffer_);

					((unsigned long long*)encodedSenderBuffer_.data())[0] = (unsigned long long)encodedSenderBuffer_.size();
				}

				const TCPClient::SocketResult sendResult = tcpClient.send(encodedSenderBuffer_.data(), encodedSenderBuffer_.size());

				if (sendResult == TCPClient::SR_SUCCEEDED)
				{
					encodedSenderBuffer_.clear();
				}
				else if (sendResult == TCPClient::SR_NOT_CONNECTED)
				{
					tcpClient.connect(clientTargetAddress_, clientTargetPort_, 10u);
					sleep(1u);
				}
			}
			else
			{
				sleep(1u);
			}
		}
		else
		{
			// we are a receiver

			encodedReceiverBuffers.resize(bufferQueueMap_.size());
			size_t bufferIndex = 0;

			for (BufferQueueMap::iterator i = bufferQueueMap_.begin(); i != bufferQueueMap_.end(); ++i)
			{
				if (!i->second.empty() && extractRelatedBuffer(i->second, encodedReceiverBuffers[bufferIndex]))
				{
					bufferIndex++;
				}
			}

			temporalScopedLock.release();

			for (size_t n = 0; n < bufferIndex; ++n)
			{
				ocean_assert(encodedReceiverBuffers[n].size() > 8);
				if (decodeData(encodedReceiverBuffers[n].data() + 8, encodedReceiverBuffers[n].size() - 8, maintenanceName, maintenanceId, maintenanceTag, maintenanceBuffer, maintenanceTimestamp))
				{
					place(maintenanceName, maintenanceId, maintenanceTag, std::move(maintenanceBuffer), maintenanceTimestamp);
				}
			}

			if (bufferIndex == 0)
			{
				sleep(1u);
			}
		}
	}
}

void MaintenanceTCPConnector::onReceiveTCPData(const TCPServer::ConnectionId connectionId, const void* data, const size_t size)
{
	ocean_assert(data != nullptr && size != 0);

	Buffer receivedBuffer(size);
	memcpy(receivedBuffer.data(), data, receivedBuffer.size());

	const ScopedLock scopedLock(lock_);

	bufferQueueMap_[connectionId].push_back(std::move(receivedBuffer));
}

bool MaintenanceTCPConnector::onConnectionRequest(const Address4& address, const Port& port, TCPServer::ConnectionId connectionId)
{
	ocean_assert(address.isValid() && port.isValid());

	OCEAN_SUPPRESS_UNUSED_WARNING(address);
	OCEAN_SUPPRESS_UNUSED_WARNING(port);
	OCEAN_SUPPRESS_UNUSED_WARNING(connectionId);

	return true;
}

bool MaintenanceTCPConnector::extractRelatedBuffer(BufferQueue& bufferQueue, Buffer& buffer)
{
	if (bufferQueue.empty() || bufferQueue.front().size() < 8)
	{
		return false;
	}

	const unsigned long long bufferSize = ((unsigned long long*)bufferQueue.front().data())[0];

	ocean_assert(bufferSize < 1024ull * 1024ull * 1024ull * 64ull);
	if (bufferSize >= 1024ull * 1024ull * 1024ull * 64ull)
	{
		return false;
	}

	unsigned long long availableBytes = 0u;
	for (BufferQueue::const_iterator i = bufferQueue.begin(); availableBytes < bufferSize && i != bufferQueue.end(); ++i)
	{
		availableBytes += (unsigned long long)i->size();
	}

	if (availableBytes < bufferSize)
	{
		return false;
	}

	buffer.resize(size_t(bufferSize));

	unsigned char* data = buffer.data();
	size_t remaining = size_t(bufferSize);

	while (!bufferQueue.empty())
	{
		Buffer& frontBuffer = bufferQueue.front();

		const size_t bytes = min(remaining, frontBuffer.size());

		memcpy(data, frontBuffer.data(), bytes);
		data += bytes;

		if (remaining < frontBuffer.size())
		{
			Buffer newBuffer(frontBuffer.size() - remaining);
			memcpy(newBuffer.data(), frontBuffer.data() + remaining, newBuffer.size());

			remaining = 0;

			frontBuffer = std::move(newBuffer);
			break;
		}
		else
		{
			bufferQueue.pop_front();
		}

		ocean_assert(bytes <= remaining);
		remaining -= bytes;
	}

	ocean_assert(remaining == 0);

	return true;
}

}

}
