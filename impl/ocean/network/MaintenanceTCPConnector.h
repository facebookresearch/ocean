/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_MAINTENANCE_TCP_CONNECTOR_H
#define FACEBOOK_NETWORK_MAINTENANCE_TCP_CONNECTOR_H

#include "ocean/network/Network.h"
#include "ocean/network/TCPClient.h"
#include "ocean/network/TCPServer.h"

#include "ocean/base/Maintenance.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include <deque>

namespace Ocean
{

namespace Network
{

/**
 * This class implements a network connector between two maintenance managers using the TCP protocol.
 * The connector allows to transmit maintenance data from one manager to another manager by application of a network.<br>
 * This connector can be either a sender or a receiver (not both concurrently) depending on the configuration.<br>
 * The sender extracts maintenance data from the local maintenance manager and sends this data to the remote connector (configured as receiver).<br>
 * The receiver receives maintenance data from a remote connector (configured as sender) and places this data into the local maintenance manager.<br>
 * Beware: The maintenance connector must be released explicitly before the program terminates.<br>
 * @see configurateAsSender(), configurateAsReceiver(), release(), MaintenanceUDPConnector.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT MaintenanceTCPConnector :
	public Maintenance::Connector,
	protected Thread
{
	protected:

		/**
		 * Definition of a vector holding bytes.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * Definition of a vector storing buffers.
		 */
		typedef std::vector<Buffer> Buffers;

		/**
		 * Definition of a buffer queue.
		 */
		typedef std::deque<Buffer> BufferQueue;

		/**
		 * Definition of a map mapping connection ids to buffer queues.
		 */
		typedef std::unordered_map<TCPServer::ConnectionId, BufferQueue> BufferQueueMap;

	public:

		/**
		 * Creates a new maintenance connector object.
		 */
		MaintenanceTCPConnector();

		/**
		 * Destructs a maintenance connector object.
		 */
		~MaintenanceTCPConnector() override;

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
		void configurateAsSender(const Address4& targetAddress, const Port& targetPort);

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
		 * TCP data receive event function.
		 * @param connectionId The id of the connection from which the data has been received
		 * @param data The data which has been received
		 * @param size The size of the data, in bytes
		 */
		void onReceiveTCPData(const TCPServer::ConnectionId connectionId, const void* data, const size_t size);

		/**
		 * Connection request event function.
		 * @param address The Address of the remote client requesting the connection
		 * @param port The port of the remote client requesting the connection
		 * @param connectionId The id of the connection
		 * @return True, to accept the connection
		 */
		bool onConnectionRequest(const Address4& address, const Port& port, TCPServer::ConnectionId connectionId);

		/**
		 * Extracts one related buffer from a queue of messages.
		 * @param bufferQueue The queue of buffers from which the related buffer will be extracted
		 * @param buffer The resulting related buffer, if any
		 * @return True, if one related buffer could be extracted
		 */
		static bool extractRelatedBuffer(BufferQueue& bufferQueue, Buffer& buffer);

	protected:

		/// The lock of this connector.
		mutable Lock lock_;

		/// The target address if this connector is a sender.
		Address4 clientTargetAddress_;

		/// The target port if this connector is a sender.
		Port clientTargetPort_;

		/// The source port if this connector is a receiver.
		Port serverSourcePort_;

		/// The TCP client of this connector, used if this connector is a sender.
		TCPClient tcpClient;

		/// The TCP server of this connector, used if this connector is a receiver.
		TCPServer tcpServer;

		/// The map of received buffers.
		BufferQueueMap bufferQueueMap_;

		/// The encoded buffer for the sender
		Buffer encodedSenderBuffer_;
};

inline bool MaintenanceTCPConnector::isSender() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert((!clientTargetAddress_.isValid() || !clientTargetPort_.isValid()) || !serverSourcePort_.isValid());

	return clientTargetAddress_.isValid() && clientTargetPort_.isValid();
}

inline bool MaintenanceTCPConnector::isReceiver() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert((!clientTargetAddress_.isValid() || !clientTargetPort_.isValid()) || !serverSourcePort_.isValid());

	return serverSourcePort_.isValid();
}

}

}

#endif // FACEBOOK_NETWORK_MAINTENANCE_TCP_CONNECTOR_H
