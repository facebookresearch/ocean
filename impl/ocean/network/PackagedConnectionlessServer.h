/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_PACKAGED_CONNECTIONLESS_SERVER_H
#define FACEBOOK_NETWORK_PACKAGED_CONNECTIONLESS_SERVER_H

#include "ocean/network/Network.h"
#include "ocean/network/PackagedConnectionlessClient.h"
#include "ocean/network/Server.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Network
{

/**
 * This class is the base class for all package connectionless server.
 * The server reassembles the datagrams a PackagedConnectionlessClient produced, see PackagedSocket::packageManagmentHeaderSize() for the layout.
 * A message is identified by (sender address, sender port, message id), and its buffer is sized from the first datagram that arrives.
 * The receive callback is invoked once every fragment has been seen, an incomplete message is dropped after maximalMessageTime_ seconds.
 * Because UDP neither guarantees order nor delivery, a message may never complete, and datagrams of different messages may interleave.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT PackagedConnectionlessServer :
	virtual public PackagedConnectionlessClient,
	virtual public Server
{
	public:

		/**
		 * Definition of a data callback function.
		 * Parameter 0 provides the address of the sender.<br>
		 * Parameter 1 provides the port of the sender.<br>
		 * Parameter 2 provides the received buffer, which must be copied, nullptr if the message could not be delivered correctly
		 * Parameter 3 provides the size of the received buffer, in bytes; 0 if the message could not be delivered correctly
		 * Parameter 4 provides the id of the message to which the received buffer belongs
		 */
		using ReceiveCallback = Callback<void, const Address4&, const Port&, const void*, const size_t, const MessageId>;

	protected:

		/**
		 * This class implements a message.
		 */
		class MessageData
		{
			public:

				/**
				 * Creates an empty data object.
				 */
				inline MessageData() = default;

				/**
				 * Copy constructor.
				 * @param messageData Message object to be copied
				 */
				inline MessageData(const MessageData& messageData);

				/**
				 * Move constructor.
				 * @param messageData Message object to be moved
				 */
				inline MessageData(MessageData&& messageData) noexcept;

				/**
				 * Creates a new message data object.
				 * @param retireTimestamp The timestamp at which this message will be retired as no further packages arrived in the meantime.
				 * @param size The size of the entire message, in bytes
				 * @param remainingPackages The number of packages in which the entire message is divided
				 */
				inline MessageData(const Timestamp retireTimestamp, const size_t size, const unsigned int remainingPackages);

				/**
				 * Returns the retire timestamp.
				 * @return Retire timestamp
				 */
				inline Timestamp retireTimestamp() const;

				/**
				 * Returns the number of remaining packages.
				 * @return Remaining packages
				 */
				inline unsigned int remainingPackages() const;

				/**
				 * Returns the size of the message buffer, in bytes.
				 * @return The number of bytes of the message buffer
				 */
				inline size_t size() const;

				/**
				 * Returns the message buffer.
				 * @return Message buffer
				 */
				inline const uint8_t* buffer() const;

				/**
				 * Returns the message buffer.
				 * @return Message buffer
				 */
				inline uint8_t* buffer();

				/**
				 * Sets or changes the retire timestamp.
				 * @param timestamp The new timestamp
				 */
				inline void setRetireTimestamp(const Timestamp timestamp);

				/**
				 * Marks one package of this message as received.
				 * A package which has already been received is ignored, so that a duplicated datagram cannot complete a message with missing payload.
				 * @param packageIndex The index of the received package, an index outside of the message is ignored
				 * @return True, if this package had not been received before
				 */
				inline bool setPackageReceived(const unsigned int packageIndex);

				/**
				 * Assign operator.
				 * @param messageData Message data object to copy
				 * @return Reference to this object
				 */
				inline MessageData& operator=(const MessageData& messageData);

				/**
				 * Move operator.
				 * @param messageData Message data object to move
				 * @return Reference to this object
				 */
				inline MessageData& operator=(MessageData&& messageData) noexcept;

			protected:

				/// The timestamp at which this message will be retired as no further packages arrived in the meantime.
				Timestamp retireTimestamp_ = Timestamp(false);

				/// The number of packages which are still missing.
				unsigned int remainingPackages_ = 0u;

				/// The received state of each package of this message.
				std::vector<uint8_t> receivedPackages_;

				/// The entire buffer of the message.
				Buffer buffer_;
		};

		/**
		 * Definition of a triple storing an address, a port and a message id.
		 */
		class Triple
		{
			public:

				/**
				 * Creates a new triple object.
				 * @param address The address of the object
				 * @param port The port of the object
				 * @param messageId The id of the object
				 */
				inline Triple(const Address4& address, const Port& port, const MessageId messageId);

				/**
				 * Returns the address of this object.
				 * @return The address
				 */
				inline const Address4& address() const;

				/**
				 * Returns the port of this object.
				 * @return The port
				 */
				inline const Port& port() const;

				/**
				 * Returns the message id of this object.
				 * @return The message id
				 */
				inline MessageId messageId() const;

				/**
				 * Compares two triple objects.
				 * @param triple The second triple object
				 * @return True, if the left object is lesser than the right one
				 */
				inline bool operator<(const Triple& triple) const;

			protected:

				/// The address of this object.
				Address4 address_;

				/// The port of this object.
				Port port_;

				/// The message id of this object.
				unsigned int messageId_ = invalidMessageId();
		};

		/**
		 * Definition of a map mapping message ids to massage data objects.
		 */
		using MessageMap = std::map<Triple, MessageData>;

	public:

		/**
		 * Destructs a connectionless server object.
		 */
		~PackagedConnectionlessServer() override;

		/**
		 * Sets the receive data callback function.
		 * @param callback the callback function to be called if a new message arrives.
		 */
		inline void setReceiveCallback(const ReceiveCallback& callback);

	protected:

		/**
		 * Creates a new connectionless server object.
		 */
		PackagedConnectionlessServer();

		/**
		 * The scheduler event function.
		 * Socket::onScheduler().
		 */
		bool onScheduler() override;

	protected:

		/// Data callback function called on new message arrivals.
		ReceiveCallback receiveCallback_;

		/// The time between the first package of a large message and the decision to retire the message if still packages are missing, in seconds.
		double maximalMessageTime_ = 5.0;

		/// Intermediate buffer storing individual parts of a large message.
		Buffer packageBuffer_;

		/// The map holding all partially received message.
		MessageMap connectionlessServerMessageMap;
};

inline PackagedConnectionlessServer::MessageData::MessageData(const MessageData& messageData) :
	retireTimestamp_(messageData.retireTimestamp_),
	remainingPackages_(messageData.remainingPackages_),
	receivedPackages_(messageData.receivedPackages_),
	buffer_(messageData.buffer_)
{
	// nothing to do here
}

inline PackagedConnectionlessServer::MessageData::MessageData(MessageData&& messageData) noexcept :
	retireTimestamp_(messageData.retireTimestamp_),
	remainingPackages_(messageData.remainingPackages_),
	receivedPackages_(std::move(messageData.receivedPackages_)),
	buffer_(std::move(messageData.buffer_))
{
	messageData.retireTimestamp_.toInvalid();
	messageData.remainingPackages_ = 0u;
}

inline PackagedConnectionlessServer::MessageData::MessageData(const Timestamp retireTimestamp, const size_t size, const unsigned int remainingPackages) :
	retireTimestamp_(retireTimestamp),
	remainingPackages_(remainingPackages),
	receivedPackages_(remainingPackages, 0u),
	buffer_(size)
{
	ocean_assert(buffer_.empty() || remainingPackages_ != 0u);
}

inline Timestamp PackagedConnectionlessServer::MessageData::retireTimestamp() const
{
	return retireTimestamp_;
}

inline unsigned int PackagedConnectionlessServer::MessageData::remainingPackages() const
{
	return remainingPackages_;
}

inline size_t PackagedConnectionlessServer::MessageData::size() const
{
	return buffer_.size();
}

inline const uint8_t* PackagedConnectionlessServer::MessageData::buffer() const
{
	return buffer_.data();
}

inline uint8_t* PackagedConnectionlessServer::MessageData::buffer()
{
	return buffer_.data();
}

inline void PackagedConnectionlessServer::MessageData::setRetireTimestamp(const Timestamp timestamp)
{
	retireTimestamp_ = timestamp;
}

inline bool PackagedConnectionlessServer::MessageData::setPackageReceived(const unsigned int packageIndex)
{
	// a peer may announce a different package count for a message which is already in flight, so the index is checked against this message
	if (size_t(packageIndex) >= receivedPackages_.size() || receivedPackages_[packageIndex] != 0u)
	{
		return false;
	}

	receivedPackages_[packageIndex] = 1u;

	ocean_assert(remainingPackages_ >= 1u);
	--remainingPackages_;

	return true;
}

inline PackagedConnectionlessServer::MessageData& PackagedConnectionlessServer::MessageData::operator=(const MessageData& messageData)
{
	retireTimestamp_ = messageData.retireTimestamp_;
	remainingPackages_ = messageData.remainingPackages_;
	receivedPackages_ = messageData.receivedPackages_;
	buffer_ = messageData.buffer_;

	return *this;
}

inline PackagedConnectionlessServer::MessageData& PackagedConnectionlessServer::MessageData::operator=(MessageData&& messageData) noexcept
{
	if (this != &messageData)
	{
		retireTimestamp_ = messageData.retireTimestamp_;
		remainingPackages_ = messageData.remainingPackages_;
		receivedPackages_ = std::move(messageData.receivedPackages_);
		buffer_ = std::move(messageData.buffer_);

		messageData.retireTimestamp_.toInvalid();
		messageData.remainingPackages_ = 0u;
	}

	return *this;
}

inline PackagedConnectionlessServer::Triple::Triple(const Address4& address, const Port& port, const MessageId messageId) :
	address_(address),
	port_(port),
	messageId_(messageId)
{
	// nothing to do here
}

inline const Address4& PackagedConnectionlessServer::Triple::address() const
{
	return address_;
}

inline const Port& PackagedConnectionlessServer::Triple::port() const
{
	return port_;
}

inline PackagedConnectionlessServer::MessageId PackagedConnectionlessServer::Triple::messageId() const
{
	return messageId_;
}

inline bool PackagedConnectionlessServer::Triple::operator<(const Triple& triple) const
{
	return address_ < triple.address_ || (address_ == triple.address_ && (port_ < triple.port_ || (port_ == triple.port_ && messageId_ < triple.messageId_)));
}

inline void PackagedConnectionlessServer::setReceiveCallback(const ReceiveCallback& callback)
{
	const ScopedLock scopedLock(lock_);

	receiveCallback_ = callback;
}

}

}

#endif // FACEBOOK_NETWORK_PACKAGED_CONNECTIONLESS_SERVER_H
