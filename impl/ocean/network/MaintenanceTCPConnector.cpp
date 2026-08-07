/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/MaintenanceTCPConnector.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Network
{

MaintenanceTCPConnector::MaintenanceTCPConnector() :
	Thread("MaintenanceTCPConnector thread")
{
	startThread();

	tcpServer_.setConnectionRequestCallback(TCPServer::ConnectionRequestCallback::create(*this, &MaintenanceTCPConnector::onConnectionRequest));
	tcpServer_.setReceiveCallback(TCPServer::ReceiveCallback::create(*this, &MaintenanceTCPConnector::onReceiveTCPData));
}

MaintenanceTCPConnector::~MaintenanceTCPConnector()
{
	tcpServer_.setConnectionRequestCallback(TCPServer::ConnectionRequestCallback());
	tcpServer_.setReceiveCallback(TCPServer::ReceiveCallback());

	stopThreadExplicitly();
}

void MaintenanceTCPConnector::configurateAsSender(const Address4& address, const Port& port)
{
	ocean_assert(address.isValid() && port.isValid());

	const ScopedLock scopedLock(lock_);

	clientTargetAddress_ = address;
	clientTargetPort_ = port;

	tcpServer_.stop();
	serverSourcePort_ = Port();
}

void MaintenanceTCPConnector::configurateAsReceiver(const Port& port)
{
	ocean_assert(port.isValid());

	const ScopedLock scopedLock(lock_);

	clientTargetAddress_ = Address4();
	clientTargetPort_ = Port();

	serverSourcePort_ = port;
	tcpServer_.setPort(serverSourcePort_);
	tcpServer_.start();
}

void MaintenanceTCPConnector::threadRun()
{
	std::string maintenanceName;
	std::string maintenanceTag;
	uint64_t maintenanceId = uint64_t(-1);
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

					const uint64_t encodedSenderBufferSize = uint64_t(encodedSenderBuffer_.size());
					memcpy(encodedSenderBuffer_.data(), &encodedSenderBufferSize, sizeof(uint64_t));
				}

				const TCPClient::SocketResult sendResult = tcpClient_.send(encodedSenderBuffer_.data(), encodedSenderBuffer_.size());

				if (sendResult == TCPClient::SR_SUCCEEDED)
				{
					encodedSenderBuffer_.clear();
				}
				else if (sendResult == TCPClient::SR_NOT_CONNECTED)
				{
					tcpClient_.connect(clientTargetAddress_, clientTargetPort_, 10u);
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

			for (BufferQueueMap::value_type& connectionQueuePair : bufferQueueMap_)
			{
				if (!connectionQueuePair.second.empty() && extractRelatedBuffer(connectionQueuePair.second, encodedReceiverBuffers[bufferIndex]))
				{
					++bufferIndex;
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

bool MaintenanceTCPConnector::onConnectionRequest(const Address4& address, const Port& port, TCPServer::ConnectionId /*connectionId*/)
{
	ocean_assert_and_suppress_unused(address.isValid(), address);
	ocean_assert_and_suppress_unused(port.isValid(), port);

	return true;
}

bool MaintenanceTCPConnector::extractRelatedBuffer(BufferQueue& bufferQueue, Buffer& buffer)
{
	if (bufferQueue.empty() || bufferQueue.front().size() < 8)
	{
		return false;
	}

	uint64_t bufferSize;
	memcpy(&bufferSize, bufferQueue.front().data(), sizeof(uint64_t));

	// the size covers the 8 byte header itself, and a broken prefix cannot be resynchronized so the queue is dropped
	ocean_assert(bufferSize > 8ull && bufferSize < 1024ull * 1024ull * 1024ull * 64ull);
	if (bufferSize <= 8ull || bufferSize >= 1024ull * 1024ull * 1024ull * 64ull)
	{
		bufferQueue.clear();
		return false;
	}

	uint64_t availableBytes = 0ull;
	for (BufferQueue::const_iterator iQueue = bufferQueue.begin(); availableBytes < bufferSize && iQueue != bufferQueue.end(); ++iQueue)
	{
		availableBytes += uint64_t(iQueue->size());
	}

	if (availableBytes < bufferSize)
	{
		return false;
	}

	if (!NumericT<size_t>::isInsideValueRange(bufferSize))
	{
		return false;
	}

	buffer.resize(size_t(bufferSize));

	uint8_t* data = buffer.data();
	size_t remaining = size_t(bufferSize);

	while (!bufferQueue.empty())
	{
		Buffer& frontBuffer = bufferQueue.front();

		const size_t bytes = std::min(remaining, frontBuffer.size());

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
