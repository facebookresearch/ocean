/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_BUFFER_QUEUE_H
#define FACEBOOK_NETWORK_BUFFER_QUEUE_H

#include "ocean/network/Network.h"

#include "ocean/base/Lock.h"

#include <deque>
#include <vector>

namespace Ocean
{

namespace Network
{

/**
 * This class implements a thread-safe buffer queue.
 * @ingroup network
 */
class BufferQueue
{
	protected:

		/**
		 * Definition of a vector holding bytes.
		 */
		typedef std::vector<uint8_t> Buffer;

		/**
		 * Definition of a double-ended queue holding buffers.
		 */
		typedef std::deque<Buffer> Queue;

	public:

		/**
		 * Pushes a new buffer to the queue.
		 * @param data The data to push, will be copied, can be nullptr if 'size == 0'
		 * @param size The number of bytes to copy, with range [0, infinity)
		 */
		inline void push(const void* data, const size_t size);

		/**
		 * Pushes a new buffer to the queue.
		 * @param data The data to push, will be moved
		 */
		inline void push(Buffer&& data);

		/**
		 * Pops a buffer from the queue.
		 * @return The buffer, if any
		 */
		inline Buffer pop();

		/**
		 * Clears the buffer queue.
		 */
		inline void clear();

		/**
		 * Returns the number of buffers in this queue.
		 * @return The number of buffers
		 */
		inline size_t size() const;

		/**
		 * Returns whether this queue holds no buffers.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

	protected:

		/// The internal buffer queue.
		Queue queue_;

		/// The buffer lock.
		mutable Lock lock_;
};

inline void BufferQueue::push(const void* data, const size_t size)
{
	if (size == 0)
	{
		return;
	}

	ocean_assert(data != nullptr);

	const ScopedLock scopedLock(lock_);

	Buffer buffer(size);
	memcpy(buffer.data(), data, size);

	queue_.emplace_back(std::move(buffer));
}

inline void BufferQueue::push(Buffer&& data)
{
	ocean_assert(!data.empty());
	if (data.empty())
	{
		return;
	}

	const ScopedLock scopedLock(lock_);

	queue_.emplace_back(std::move(data));
}

inline BufferQueue::Buffer BufferQueue::pop()
{
	const ScopedLock scopedLock(lock_);

	if (queue_.empty())
	{
		return Buffer();
	}

	Buffer buffer(std::move(queue_.front()));
	queue_.pop_front();

	return buffer;
}

inline void BufferQueue::clear()
{
	const ScopedLock scopedLock(lock_);
	queue_.clear();
}

inline size_t BufferQueue::size() const
{
	const ScopedLock scopedLock(lock_);
	return queue_.size();
}

inline bool BufferQueue::isEmpty() const
{
	const ScopedLock scopedLock(lock_);
	return queue_.empty();
}

}

}

#endif // FACEBOOK_NETWORK_BUFFER_QUEUE_H
