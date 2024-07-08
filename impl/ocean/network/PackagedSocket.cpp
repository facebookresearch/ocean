/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/PackagedSocket.h"

namespace Ocean
{

namespace Network
{

PackagedSocket::MemoryBlock::MemoryBlock(const size_t size) :
	memory_(size)
{
	size_ = memory_.size();
	ocean_assert(size_ >= 1);
}

PackagedSocket::MemoryBlock::MemoryBlock(const void* data, const size_t size) :
	memory_(size)
{
	size_ = memory_.size();
	ocean_assert(size_ >= 1);

	memcpy(memory_.data(), data, memory_.size());
}

void PackagedSocket::MemoryBlock::resize(const size_t size)
{
	if (size > memory_.size())
	{
		memory_ = Memory(size);
		size_ = memory_.size();
	}
	else
	{
		size_ = size;
		offset_ = 0;
	}
}

PackagedSocket::PackagedSocket() :
	Socket()
{
	static_assert(sizeof(unsigned int) == 4, "Invalid data type!");
	static_assert(sizeof(PackagedSocket::MessageId) + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int) == 5 * 4, "Invalid data type!");

	ocean_assert(packageManagmentHeaderSize() == 5 * sizeof(unsigned int));
}

bool PackagedSocket::extractNextPackage(MemoryBlockQueue& sourceQueue, MemoryBlock& targetMemory)
{
	ocean_assert(targetMemory.isValid() && targetMemory.remainingBytes() != 0);

	if (sourceQueue.empty())
	{
		return false;
	}

	while (!sourceQueue.empty() && targetMemory.remainingBytes() != 0)
	{
		MemoryBlock& sourceMemory = sourceQueue.front();
		ocean_assert(sourceMemory.isValid() && sourceMemory.remainingBytes() != 0);

		const size_t bytesToRead = std::min(sourceMemory.remainingBytes(), targetMemory.remainingBytes());

		memcpy(targetMemory.offsetData(), sourceMemory.offsetData(), bytesToRead);

		targetMemory.moveOffset(bytesToRead);
		sourceMemory.moveOffset(bytesToRead);

		if (sourceMemory.remainingBytes() == 0)
		{
			sourceQueue.pop();
		}
	}

	return targetMemory.remainingBytes() == 0;
}

}

}
