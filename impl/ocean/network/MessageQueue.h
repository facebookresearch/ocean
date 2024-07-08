/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_MESSAGE_QUEUE_H
#define FACEBOOK_NETWORK_MESSAGE_QUEUE_H

#include "ocean/network/Network.h"

#include "ocean/base/Lock.h"

#include <map>
#include <queue>

namespace Ocean
{

namespace Network
{

/**
 * This class implements a message queue.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT MessageQueue
{
	public:

		/**
		 * Definition of a message id.
		 */
		typedef unsigned int Id;

		/**
		 * Returns an invalid message id.
		 * @return Invalid message id
		 */
		static constexpr Id invalidId();

	private:

		/**
		 * Definition of a message pair.
		 */
		typedef std::pair<std::string, std::string> Message;

		/**
		 * Definition of a queue holding message pairs.
		 */
		typedef std::queue<Message> Queue;

		/**
		 * Definition of a map mapping message ids to message queues.
		 */
		typedef std::unordered_map<Id, Queue> MessageMap;

	public:

		/**
		 * Creates a new message queue.
		 */
		MessageQueue() = default;

		/**
		 * Move constructor.
		 * @param messageQueue The message queue object to be moved
		 */
		MessageQueue(MessageQueue&& messageQueue);

		/**
		 * Creates a new message queue.
		 * @param oldMessageAge The capacity of the queue, with range [1, infinity)
		 */
		explicit MessageQueue(const unsigned int oldMessageAge);

		/**
		 * Destructs a message queue.
		 */
		~MessageQueue();

		/**
		 * Pushes a new message.
		 * Beware: To avoid a message overflow all messages older than the defined 'age' will be released.
		 * @param id Message id
		 * @param message The message to push
		 * @param value Optional message value to push
		 * @return True, if succeeded
		 */
		bool push(const Id id, const std::string& message, const std::string& value);

		/**
		 * Gets the front message for a given message id.
		 * @param id Message id to get the front message for
		 * @param message Resulting message
		 * @param value Resulting optional value
		 * @return True, if succeeded
		 */
		bool front(const Id id, std::string& message, std::string& value);

		/**
		 * Gets the front message for a given message id and waits a specified time if no message is available.
		 * @param id Message id to get the front message for
		 * @param timeout Time to wait in seconds
		 * @param message Resulting message
		 * @param value Resulting optional value
		 * @return True, if a message could be returned within the specified wait time
		 */
		bool front(const Id id, const double timeout, std::string& message, std::string& value);

		/**
		 * Pops the front message with a specified message id from the queue.
		 * @param id Message id to pop the front message for
		 */
		void pop(const Id id);

		/**
		 * Returns and pops the front message for a given message id and waits a specified time if no message is available.
		 * @param id Message id to get the front message for
		 * @param timeout Time to wait in seconds
		 * @return Resulting message, if any
		 */
		std::string pop(const Id id, const double timeout);

		/**
		 * Returns and pops the front message for a given message id and waits a specified time if no message is available.
		 * @param id Message id to get the front message for
		 * @param timeout Time to wait in seconds
		 * @param message Resulting message
		 * @param value Resulting optional value
		 * @return True, if a message could be popped within the specified wait time
		 */
		bool pop(const Id id, const double timeout, std::string& message, std::string& value);

		/**
		 * Clears the entire message queue.
		 */
		void clear();

		/**
		 * Clears all messages with a specified message id.
		 * @param id Message id to clear all messages for
		 */
		void clear(const Id id);

		/**
		 * Returns a unique message id within this message queue.
		 * @return Unique message id
		 */
		Id uniqueId();

		/**
		 * Returns the last unique message id again.
		 * Beware: Do no use this id directly, use it for control purpose only!
		 * @return Last unique id
		 */
		Id lastUniqueId();

		/**
		 * Move operator.
		 * @param messageQueue The message queue to be moved
		 * @return Reference to this object
		 */
		MessageQueue& operator=(MessageQueue&& messageQueue);

	protected:

		/// Map holding all messages.
		MessageMap messageMap_;

		/// Definition of an message age of old and therefore unused messages.
		unsigned int oldMessageAge_ = 500u;

		/// Message id counter.
		Id idCounter_ = invalidId();

		/// Map lock.
		Lock lock_;
};

constexpr MessageQueue::Id MessageQueue::invalidId()
{
	return Id(0);
}

}

}

#endif // FACEBOOK_NETWORK_MESSAGE_QUEUE_H
