/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/PackagedTCPServer.h"

namespace Ocean
{

namespace Network
{

size_t PackagedTCPServer::onSend(const ConnectionId connectionId, const void* data, const size_t size)
{
	ocean_assert(data != nullptr && size >= 1);

	if (size > maximalPackagedMessageSize())
	{
		return 0;
	}

	const PackageHeader packageHeader(size);

	if (TCPServer::onSend(connectionId, &packageHeader, sizeof(PackageHeader)) != sizeof(PackageHeader))
	{
		return 0;
	}

	return TCPServer::onSend(connectionId, data, size);
}

void PackagedTCPServer::onReceived(const ConnectionId connectionId, const void* data, const size_t size)
{
	ConnectionMemory& connectionMemory = connectionMemoryMap_[connectionId];

	MemoryBlockQueue& memoryQueue = connectionMemory.memoryQueue_;
	MemoryBlock& currentMemory = connectionMemory.currentMemory_;
	MemoryBlock& currentPackageHeaderMemory = connectionMemory.currentPackageHeaderMemory_;

	memoryQueue.emplace(data, size);

	while (!memoryQueue.empty())
	{
		if (currentPackageHeaderMemory.size() == 0)
		{
			currentPackageHeaderMemory.resize(sizeof(PackageHeader));
		}

		if (currentPackageHeaderMemory.remainingBytes() != 0)
		{
			if (extractNextPackage(memoryQueue, currentPackageHeaderMemory))
			{
				PackageHeader packageHeader;
				memcpy(&packageHeader, currentPackageHeaderMemory.data(), sizeof(PackageHeader));

				if (packageHeader.isValid() && packageHeader.size() <= maximalPackagedMessageSize())
				{
					ocean_assert(currentMemory.size() == 0);
					currentMemory.resize(packageHeader.size());
				}
				else
				{
					Log::warning() << "Invalid TCP package";

					currentPackageHeaderMemory.resize(0);
				}
			}
			else
			{
				break;
			}
		}

		if (currentMemory.remainingBytes() != 0)
		{
			if (extractNextPackage(memoryQueue, currentMemory))
			{
				if (receiveCallback_)
				{
					receiveCallback_(connectionId, currentMemory.data(), currentMemory.size());
				}

				currentMemory.resize(0);
				currentPackageHeaderMemory.resize(0);
			}
		}
	}
}

}

}
