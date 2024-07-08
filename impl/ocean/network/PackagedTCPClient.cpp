/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/PackagedTCPClient.h"

namespace Ocean
{

namespace Network
{

size_t PackagedTCPClient::onSend(const void* data, const size_t size)
{
	ocean_assert(data != nullptr && size >= 1);

	if (size > maximalPackagedMessageSize())
	{
		return 0;
	}

	const PackageHeader packageHeader(size);

	if (TCPClient::onSend(&packageHeader, sizeof(PackageHeader)) != sizeof(PackageHeader))
	{
		return 0;
	}

	return TCPClient::onSend(data, size);
}

void PackagedTCPClient::onReceived(const void* data, const size_t size)
{
	memoryQueue_.emplace(data, size);

	while (!memoryQueue_.empty())
	{
		if (currentPackageHeaderMemory_.size() == 0)
		{
			currentPackageHeaderMemory_.resize(sizeof(PackageHeader));
		}

		if (currentPackageHeaderMemory_.remainingBytes() != 0)
		{
			if (extractNextPackage(memoryQueue_, currentPackageHeaderMemory_))
			{
				PackageHeader packageHeader;
				memcpy(&packageHeader, currentPackageHeaderMemory_.data(), sizeof(PackageHeader));

				if (packageHeader.isValid() && packageHeader.size() <= maximalPackagedMessageSize())
				{
					ocean_assert(currentMemory_.size() == 0);
					currentMemory_.resize(packageHeader.size());
				}
				else
				{
					Log::warning() << "Invalid TCP package";

					currentPackageHeaderMemory_.resize(0);
				}
			}
		}

		if (currentMemory_.remainingBytes() != 0)
		{
			if (extractNextPackage(memoryQueue_, currentMemory_))
			{
				if (receiveCallback_)
				{
					receiveCallback_(currentMemory_.data(), currentMemory_.size());
				}

				currentMemory_.resize(0);
				currentPackageHeaderMemory_.resize(0);
			}
		}
	}
}

}

}
