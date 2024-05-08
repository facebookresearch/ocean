/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_MAINTENANCE_UDP_CONNECTOR_H
#define FACEBOOK_NETWORK_MAINTENANCE_UDP_CONNECTOR_H

#include "ocean/network/Network.h"
#include "ocean/network/PackagedUDPClient.h"
#include "ocean/network/PackagedUDPServer.h"

#include "ocean/base/Maintenance.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include <queue>

namespace Ocean
{

namespace Network
{

/**
 * This class implements a network connector between two maintenance managers using the UDP protocol.
 * The connector allows to transmit maintenance data from one manager to another manager by application of a network.<br>
 * This connector can be either a sender or a receiver (not both concurrently) depending on the configuration.<br>
 * The sender extracts maintenance data from the local maintenance manager and sends this data to the remote connector (configured as receiver).<br>
 * The receiver receives maintenance data from a remote connector (configured as sender) and places this data into the local maintenance manager.<br>
 * @see configurateAsSender(), configurateAsReceiver(), MaintenanceTCPConnector.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT MaintenanceUDPConnector :
	public Maintenance::Connector,
	protected Thread
{
	protected:

		/**
		 * Definition of a vector holding bytes.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * Definition of a buffer queue.
		 */
		typedef std::queue<Buffer> BufferQueue;

	public:

		/**
		 * Creates a new maintenance connector object.
		 */
		MaintenanceUDPConnector();

		/**
		 * Destructs a maintenance connector object.
		 */
		~MaintenanceUDPConnector() override;

		/**
		 * Returns whether this connector is configured as sender.
		 * @return True, if so
		 * @see configurateAsSender(), isReceiver().
		 */
		inline bool isSender() const;

		/**
		 * Returns whether this connector is configured as receiver.
		 * @return True, if so
		 * @see configurateAsReceiver(), isSender().
		 */
		inline bool isReceiver() const;

		/**
		 * Configures this connector as sender.
		 * @param targetAddress The address of the remote host to which the maintenance data will be sent
		 * @param targetPort The port of the remote host to which the maintenance data will be sent
		 * @see configurateAsReceiver().
		 */
		void configurateAsSender(const Address4& targetAddress, Port& targetPort);

		/**
		 * Configures this connector as receiver.
		 * @param port The local port of this receiver connector
		 */
		void configurateAsReceiver(const Port& port);

	protected:

		/**
		 * Internal thread run function.
		 */
		void threadRun() override;

		/**
		 * UDP data receive event function.
		 * @param senderAddress The address of the sender from which the data has been sent
		 * @param senderPort The port of the sender from which the data has been sent
		 * @param buffer The buffer which has been sent
		 * @param bufferSize the size of the buffer
		 * @param messageId The id of the received data provided by the UDP client
		 */
		void onReceiveUDPMessage(const Address4& senderAddress, const Port& senderPort, const void* buffer, const size_t bufferSize, const PackagedUDPServer::MessageId messageId);

	protected:

		/// The lock of this connector.
		mutable Lock lock_;

		/// The target address if this connector is a sender.
		Address4 clientTargetAddress_;

		/// The target port if this connector is a sender.
		Port clientTargetPort_;

		/// The source port if this connector is a receiver.
		Port serverSourcePort_;

		/// The UDP client of this connector, used if this connector is a sender.
		PackagedUDPClient udpClient;

		/// The UDP server of this connector, used if this connector is a receiver.
		PackagedUDPServer udpServer;

		/// The queue of maintenance data.
		BufferQueue bufferQueue_;
};

inline bool MaintenanceUDPConnector::isSender() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert((!clientTargetAddress_.isValid() || !clientTargetPort_.isValid()) || !serverSourcePort_.isValid());

	return clientTargetAddress_.isValid() && clientTargetPort_.isValid();
}

inline bool MaintenanceUDPConnector::isReceiver() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert((!clientTargetAddress_.isValid() || !clientTargetPort_.isValid()) || !serverSourcePort_.isValid());

	return serverSourcePort_.isValid();
}

}

}

#endif // FACEBOOK_NETWORK_MAINTENANCE_UDP_CONNECTOR_H
