/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/MaintenanceUDPConnector.h"

namespace Ocean
{

namespace Network
{

MaintenanceUDPConnector::MaintenanceUDPConnector() :
	Thread("MaintenanceUDPConnector thread")
{
	startThread();

	udpServer.setReceiveCallback(PackagedUDPServer::ReceiveCallback::create(*this, &MaintenanceUDPConnector::onReceiveUDPMessage));
}

MaintenanceUDPConnector::~MaintenanceUDPConnector()
{
	udpServer.setReceiveCallback(PackagedUDPServer::ReceiveCallback());

	stopThreadExplicitly();
}

void MaintenanceUDPConnector::configurateAsSender(const Address4& address, Port& port)
{
	ocean_assert(address.isValid() && port.isValid());

	const ScopedLock scopedLock(lock_);

	clientTargetAddress_ = address;
	clientTargetPort_ = port;

	udpServer.stop();
	serverSourcePort_ = Port();
}

void MaintenanceUDPConnector::configurateAsReceiver(const Port& port)
{
	ocean_assert(port.isValid());

	const ScopedLock scopedLock(lock_);

	clientTargetAddress_ = Address4();
	clientTargetPort_ = Port();

	serverSourcePort_ = port;
	udpServer.setPort(serverSourcePort_);
	udpServer.start();
}

void MaintenanceUDPConnector::threadRun()
{
	std::string maintenanceName, maintenanceTag;
	unsigned long long maintenanceId;
	Maintenance::Buffer maintenanceBuffer;
	Timestamp maintenanceTimestamp;

	Buffer encodedBuffer;

	while (!shouldThreadStop())
	{
		TemporaryScopedLock temporalScopedLock(lock_);

		ocean_assert((!clientTargetAddress_.isValid() || !clientTargetPort_.isValid()) || !serverSourcePort_.isValid());

		// check whether we are the sender or a receiver
		if (clientTargetAddress_.isValid() && clientTargetPort_.isValid())
		{
			// we are a sender

			if (Maintenance::get().receive(maintenanceName, maintenanceId, maintenanceTag, maintenanceBuffer, maintenanceTimestamp))
			{
				temporalScopedLock.release();

				encodeData(maintenanceName, maintenanceId, maintenanceTag, maintenanceBuffer, maintenanceTimestamp, 0, encodedBuffer);
				udpClient.send(clientTargetAddress_, clientTargetPort_, encodedBuffer.data(), encodedBuffer.size());
			}
			else
			{
				sleep(1u);
			}
		}
		else
		{
			// we are a receiver

			if (!bufferQueue_.empty())
			{
				encodedBuffer = std::move(bufferQueue_.front());
				bufferQueue_.pop();

				temporalScopedLock.release();

				if (decodeData(encodedBuffer.data(), encodedBuffer.size(), maintenanceName, maintenanceId, maintenanceTag, maintenanceBuffer, maintenanceTimestamp))
				{
					place(maintenanceName, maintenanceId, maintenanceTag, std::move(maintenanceBuffer), maintenanceTimestamp);
				}
			}
			else
			{
				sleep(1u);
			}
		}
	}
}

void MaintenanceUDPConnector::onReceiveUDPMessage(const Address4& senderAddress, const Port& senderPort, const void* buffer, const size_t bufferSize, const PackagedUDPServer::MessageId messageId)
{
	ocean_assert(senderAddress.isValid() && senderPort.isValid());
	OCEAN_SUPPRESS_UNUSED_WARNING(senderAddress);
	OCEAN_SUPPRESS_UNUSED_WARNING(senderPort);
	OCEAN_SUPPRESS_UNUSED_WARNING(messageId);

	Buffer receivedBuffer(bufferSize);
	memcpy(receivedBuffer.data(), buffer, receivedBuffer.size());

	const ScopedLock scopedLock(lock_);

	bufferQueue_.push(std::move(receivedBuffer));
}

}

}
