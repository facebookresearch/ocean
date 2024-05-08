/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/PackagedConnectionlessServer.h"

namespace Ocean
{

namespace Network
{

PackagedConnectionlessServer::PackagedConnectionlessServer() :
	PackagedConnectionlessClient(),
	Server()
{
	// nothing to do here
}

PackagedConnectionlessServer::~PackagedConnectionlessServer()
{
	// nothing to do here
}

bool PackagedConnectionlessServer::onScheduler()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(maximalPackageSize_ != 0);
	ocean_assert(packageManagmentHeaderSize() < maximalPackageSize_);

	if (packageBuffer_.size() != maximalPackageSize_)
	{
		packageBuffer_.resize(maximalPackageSize_);
	}

	if (packageBuffer_.empty() || !receiveCallback_ || !schedulerIsActive_ || socketId_ == invalidSocketId())
	{
		return false;
	}

	sockaddr_in senderAddress;
#if defined(OCEAN_POSIX_AVAILABLE)
	socklen_t senderAddressSize = sizeof(senderAddress);
#else
	int senderAddressSize = sizeof(senderAddress);
#endif

	bool busy = false;

	while (true)
	{
		int size = int(recvfrom(socketId_, (char*)(packageBuffer_.data()), (int)(packageBuffer_.size()), 0, (sockaddr*)&senderAddress, &senderAddressSize));

		if (size <= 0)
		{
			break;
		}

		busy = true;

		const Timestamp currentTimestamp(true);

		if ((unsigned int)(size) > packageManagmentHeaderSize())
		{
			const MessageId messageId = Data::fromBigEndian(((unsigned int*)packageBuffer_.data())[0]);
			const unsigned int messageSize = Data::fromBigEndian(((unsigned int*)packageBuffer_.data())[1]);
			const unsigned int dataStartPosition = Data::fromBigEndian(((unsigned int*)packageBuffer_.data())[2]);
			const unsigned int packageIndex = Data::fromBigEndian(((unsigned int*)packageBuffer_.data())[3]);
			const unsigned int totalPackages = Data::fromBigEndian(((unsigned int*)packageBuffer_.data())[4]);

			OCEAN_SUPPRESS_UNUSED_WARNING(packageIndex);

			const Triple messageTriple(Address4(senderAddress.sin_addr.s_addr), Port(senderAddress.sin_port), messageId);

			MessageMap::iterator i = connectionlessServerMessageMap.find(messageTriple);
			if (i == connectionlessServerMessageMap.end())
			{
				i = connectionlessServerMessageMap.insert(std::make_pair(messageTriple, MessageData(Timestamp(false), size_t(messageSize), totalPackages))).first;
			}

			const size_t payloadSize = size - packageManagmentHeaderSize();
			ocean_assert(payloadSize < packageBuffer_.size());

			if (dataStartPosition + payloadSize > i->second.size())
			{
				connectionlessServerMessageMap.erase(i);

				if (receiveCallback_)
				{
					receiveCallback_(messageTriple.address(), messageTriple.port(), nullptr, 0, messageTriple.messageId());
				}
			}
			else
			{
				memcpy(i->second.buffer() + dataStartPosition, packageBuffer_.data() + packageManagmentHeaderSize(), payloadSize);
				i->second.setRetireTimestamp(Timestamp(currentTimestamp + maximalMessageTime_));

				ocean_assert(i->second.remainingPackages() >= 1u);
				i->second.setRemaininigPackages(i->second.remainingPackages() - 1u);

				if (i->second.remainingPackages() == 0u)
				{
					receiveCallback_(i->first.address(), i->first.port(), i->second.buffer(), i->second.size(), i->first.messageId());
					connectionlessServerMessageMap.erase(i);
				}
			}
		}

		for (MessageMap::const_iterator i = connectionlessServerMessageMap.begin(); i != connectionlessServerMessageMap.end(); /* noop */)
		{
			if (i->second.retireTimestamp() < currentTimestamp)
			{
				i = connectionlessServerMessageMap.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	return busy;
}

}

}
