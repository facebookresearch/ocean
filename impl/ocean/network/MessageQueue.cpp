/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/MessageQueue.h"

#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Network
{

MessageQueue::MessageQueue(MessageQueue&& messageQueue)
{
	*this = std::move(messageQueue);
}

MessageQueue::MessageQueue(const unsigned int oldMessageAge) :
	oldMessageAge_(max(oldMessageAge, 1u))
{
	// nothing to do here
}

MessageQueue::~MessageQueue()
{
	// nothing to do here
}

bool MessageQueue::push(const Id id, const std::string& message, const std::string& value)
{
	const ScopedLock scopedLock(lock_);

	// old messages will be removed
	for (MessageMap::iterator iMessage = messageMap_.begin(); iMessage != messageMap_.end(); /* noop */)
	{
		if (iMessage->first + oldMessageAge_ < id)
		{
			iMessage = messageMap_.erase(iMessage);
		}
		else
		{
			break;
		}
	}

	MessageMap::iterator iMessage = messageMap_.find(id);
	if (iMessage == messageMap_.end())
	{
		iMessage = messageMap_.insert(std::make_pair(id, Queue())).first;
	}

	ocean_assert(iMessage != messageMap_.end());

	iMessage->second.push(Message(message, value));
	return true;
}

bool MessageQueue::front(const Id id, std::string& message, std::string& value)
{
	const ScopedLock scopedLock(lock_);

	MessageMap::iterator iMessage = messageMap_.find(id);

	if (iMessage == messageMap_.end() || iMessage->second.empty())
	{
		return false;
	}

	message = iMessage->second.front().first;
	value = iMessage->second.front().second;
	iMessage->second.pop();

	return true;
}

bool MessageQueue::front(const Id id, const double timeout, std::string& message, std::string& value)
{
	TemporaryScopedLock temporaryScopedLock(lock_);

	MessageMap::iterator iMessage = messageMap_.find(id);

	if (iMessage == messageMap_.end() || iMessage->second.empty())
	{
		temporaryScopedLock.release();

		Timestamp startTimestamp(true);

		while (true)
		{
			if (startTimestamp + timeout < Timestamp(true))
			{
				Log::warning() << "Timeout in message queue.";
				return false;
			}

			temporaryScopedLock.relock(lock_);

			iMessage = messageMap_.find(id);
			if (iMessage != messageMap_.end() && iMessage->second.empty() == false)
			{
				break;
			}

			// we have to give other thread the chance to insert a message
			temporaryScopedLock.release();

			Thread::sleep(1);
		}
	}

	ocean_assert(iMessage != messageMap_.end() && iMessage->second.empty() == false);

	message = iMessage->second.front().first;
	value = iMessage->second.front().second;

	return true;
}

void MessageQueue::pop(const Id id)
{
	const ScopedLock scopedLock(lock_);

	MessageMap::iterator iMessage = messageMap_.find(id);

	if (iMessage != messageMap_.end())
	{
		iMessage->second.pop();
	}
}

std::string MessageQueue::pop(const Id id, const double timeout)
{
	std::string message, value;

	if (front(id, timeout, message, value))
		pop(id);

	return message;
}

bool MessageQueue::pop(const Id id, const double timeout, std::string& message, std::string& value)
{
	if (front(id, timeout, message, value))
	{
		pop(id);
		return true;
	}

	return false;
}

void MessageQueue::clear()
{
	const ScopedLock scopedLock(lock_);

	messageMap_.clear();
}

void MessageQueue::clear(const Id id)
{
	const ScopedLock scopedLock(lock_);

	MessageMap::iterator iMessage = messageMap_.find(id);

	if (iMessage != messageMap_.cend())
	{
		while (iMessage->second.empty() == false)
		{
			iMessage->second.pop();
		}
	}
}

MessageQueue::Id MessageQueue::uniqueId()
{
	const ScopedLock scopedLock(lock_);
	return ++idCounter_;
}

MessageQueue::Id MessageQueue::lastUniqueId()
{
	return idCounter_;
}

MessageQueue& MessageQueue::operator=(MessageQueue&& messageQueue)
{
	if (this != &messageQueue)
	{
		messageMap_ = std::move(messageQueue.messageMap_);

		oldMessageAge_ = messageQueue.oldMessageAge_;
		messageQueue.oldMessageAge_ = 500u;

		idCounter_ = messageQueue.idCounter_;
		messageQueue.idCounter_ = invalidId();
	}

	return *this;
}

}

}
