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
		typedef Callback<void, const Address4&, const Port&, const void*, const size_t, const MessageId> ReceiveCallback;

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
				 * Sets the number of packages which are still missing.
				 * @param packages The number of remaining packages
				 */
				inline void setRemaininigPackages(const unsigned int packages);

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
		typedef std::map<Triple, MessageData> MessageMap;

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
	buffer_(messageData.buffer_)
{
	// nothing to do here
}

inline PackagedConnectionlessServer::MessageData::MessageData(MessageData&& messageData) noexcept :
	retireTimestamp_(messageData.retireTimestamp_),
	remainingPackages_(messageData.remainingPackages_),
	buffer_(std::move(messageData.buffer_))
{
	messageData.retireTimestamp_.toInvalid();
	messageData.remainingPackages_ = 0u;
}

inline PackagedConnectionlessServer::MessageData::MessageData(const Timestamp retireTimestamp, const size_t size, const unsigned int remainingPackages) :
	retireTimestamp_(retireTimestamp),
	remainingPackages_(remainingPackages),
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

inline void PackagedConnectionlessServer::MessageData::setRemaininigPackages(const unsigned int packages)
{
	remainingPackages_ = packages;
}

inline PackagedConnectionlessServer::MessageData& PackagedConnectionlessServer::MessageData::operator=(const MessageData& messageData)
{
	retireTimestamp_ = messageData.retireTimestamp_;
	remainingPackages_ = messageData.remainingPackages_;
	buffer_ = messageData.buffer_;

	return *this;
}

inline PackagedConnectionlessServer::MessageData& PackagedConnectionlessServer::MessageData::operator=(MessageData&& messageData) noexcept
{
	if (this != &messageData)
	{
		retireTimestamp_ = messageData.retireTimestamp_;
		remainingPackages_ = messageData.remainingPackages_;
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
