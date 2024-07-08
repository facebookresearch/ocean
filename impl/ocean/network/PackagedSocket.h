/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_PACKAGED_SOCKET_H
#define FACEBOOK_NETWORK_PACKAGED_SOCKET_H

#include "ocean/network/Network.h"
#include "ocean/network/Socket.h"

#include "ocean/base/Memory.h"

#include "ocean/io/Bitstream.h"

#include <queue>

namespace Ocean
{

namespace Network
{

/**
 * This class is the base class for all packaged sockets.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT PackagedSocket : virtual public Socket
{
	protected:

		/**
		 * This class implements a memory block.
		 */
		class OCEAN_NETWORK_EXPORT MemoryBlock
		{
			public:

				/**
				 * Default constructor creating an empty memory block.
				 */
				MemoryBlock() = default;

				/**
				 * Creates a new memory block.
				 * @param size The number of bytes the memory bock will contain, with range [1, infinity)
				 */
				explicit MemoryBlock(const size_t size);

				/**
				 * Creates a new memory block and copied memory from source data.
				 * @param data The source data to be copied, must be valid
				 * @param size The number of bytes in the source data, with range [1, infinity)
				 */
				MemoryBlock(const void* data, const size_t size);

				/**
				 * Returns the pointer to the start of the memory block.
				 * @return Pointer to the start of the memory
				 */
				inline const void* data() const;

				/**
				 * Returns the pointer to the start of the memory block.
				 * @return Pointer to the start of the memory
				 */
				inline void* data();

				/**
				 * Returns the pointer to the offset location within the memory block.
				 * @return Pointer to offset location within the memory
				 */
				inline const void* offsetData() const;

				/**
				 * Returns the pointer to the offset location within the memory block.
				 * @return Pointer to offset location within the memory
				 */
				inline void* offsetData();

				/**
				 * Returns the overall number of bytes in the memory block.
				 * @return The memory block's size in bytes (the capacity may still be larger), with range [0, infinity)
				 */
				inline size_t size() const;

				/**
				 * Returns the number of remaining bytes.
				 * @return The memory's remaining bytes, with range [0, infinity)
				 */
				inline size_t remainingBytes() const;

				/**
				 * Resets the memory offset location back to the start.
				 */
				inline void resetOffset();

				/**
				 * Moves the offset within the memory block.
				 * @param delta The number of bytes the offset will be moved, with range [0, remainingBytes()]
				 */
				inline void moveOffset(const size_t delta);

				/**
				 * Resizes the memory block.
				 * @param size The new size in bytes, with range [0, infinity)
				 */
				void resize(const size_t size);

				/**
				 * Returns whether this memory block holds memory.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/// The actual memory, can be larger than 'size_'.
				Memory memory_;

				/// The actual size of the usable memory, in bytes.
				size_t size_ = 0;

				/// The offset within the memory.
				size_t offset_ = 0;
		};

		/**
		 * Definition of a queue holding memory blocks.
		 */
		typedef std::queue<MemoryBlock> MemoryBlockQueue;

		/**
		 * Definition of a package header.
		 */
		class PackageHeader
		{
			public:

				/**
				 * Creates an invalid header.
				 */
				PackageHeader() = default;

				/**
				 * Creates a valid header with specified size field.
				 * @param size The number of bytes the payload will have, with range [1, infinity)
				 */
				explicit inline PackageHeader(const size_t size);

				/**
				 * Returns whether this header is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns the number of bytes the payload will have.
				 * @return The payload size in bytes, with range [1, infinity)
				 */
				inline size_t size() const;

			protected:

				/**
				 * Returns the unique tag for this header.
				 * @return The header's unique tag
				 */
				static constexpr uint64_t oceanTag();

			protected:

				/// The header's tag.
				uint64_t tag_ = 0ull;

				/// The header's version.
				uint64_t version_ = 0ull;

				/// The number of bytes the payload will have.
				uint64_t size_ = 0ull;
		};

		static_assert(sizeof(PackageHeader) == sizeof(uint64_t) * 3, "Invalid header!");

	public:

		/**
		 * Definition of a message id.
		 */
		typedef uint32_t MessageId;

		/**
		 * Returns an invalid message id.
		 * @return Invalid message id
		 */
		static constexpr MessageId invalidMessageId();

		/**
		 * Returns the size of the package management header in bytes.
		 * @return The header's size in bytes
		 */
		static constexpr size_t packageManagmentHeaderSize();

		/**
		 * Returns the maximal size of a packaged message.
		 * @return The maximal size of a packaged message in bytes
		 */
		static constexpr size_t maximalPackagedMessageSize();

	protected:

		/**
		 * Creates a new packaged socket.
		 */
		PackagedSocket();

		/**
		 * Extracts a memory block from a queue with memory block.
		 * @param sourceQueue The queue from which the memory will be extracted
		 * @param targetMemory The target memory block to be filled, must be valid, must have remaining bytes
		 * @return True, if the target memory block has been filled and no byte is remaining anymore
		 */
		static bool extractNextPackage(MemoryBlockQueue& sourceQueue, MemoryBlock& targetMemory);
};

inline const void* PackagedSocket::MemoryBlock::data() const
{
	ocean_assert(size_ < memory_.size());

	return memory_.constdata<uint8_t>();
}

inline void* PackagedSocket::MemoryBlock::data()
{
	ocean_assert(size_ <= memory_.size());

	return memory_.data<uint8_t>();
}

inline const void* PackagedSocket::MemoryBlock::offsetData() const
{
	ocean_assert(size_ < memory_.size());
	ocean_assert(offset_ < size_);

	return memory_.constdata<uint8_t>() + offset_;
}

inline void* PackagedSocket::MemoryBlock::offsetData()
{
	ocean_assert(size_ <= memory_.size());
	ocean_assert(offset_ < size_);

	return memory_.data<uint8_t>() + offset_;
}

inline size_t PackagedSocket::MemoryBlock::size() const
{
	return size_;
}

inline void PackagedSocket::MemoryBlock::resetOffset()
{
	offset_ = 0;
}

inline void PackagedSocket::MemoryBlock::moveOffset(const size_t delta)
{
	ocean_assert(delta <= remainingBytes());

	offset_ += delta;

	ocean_assert(offset_ <= size_);
}

inline size_t PackagedSocket::MemoryBlock::remainingBytes() const
{
	ocean_assert(offset_ <= size_);
	return size_ - offset_;
}

inline bool PackagedSocket::MemoryBlock::isValid() const
{
	return bool(memory_);
}

inline PackagedSocket::PackageHeader::PackageHeader(const size_t size) :
	tag_(oceanTag()),
	size_(size)
{
	// nothing to do here
}

inline bool PackagedSocket::PackageHeader::isValid() const
{
	return tag_ == oceanTag() && version_ == 0ull && size_ != 0ull;
}

inline size_t PackagedSocket::PackageHeader::size() const
{
	return size_t(size_);
}

constexpr uint64_t PackagedSocket::PackageHeader::oceanTag()
{
	return IO::Tag::string2tag("_OCNPKG_");
}

constexpr PackagedSocket::MessageId PackagedSocket::invalidMessageId()
{
	return MessageId(-1);
}

constexpr size_t PackagedSocket::packageManagmentHeaderSize()
{
	 return sizeof(MessageId) + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int);
}

constexpr size_t PackagedSocket::maximalPackagedMessageSize()
{
	return 1024 * 1024 * 1024; // 1GB
}

}

}

#endif // FACEBOOK_NETWORK_PACKAGED_SOCKET_H
