/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/PackagedConnectionlessClient.h"
#include "ocean/network/Data.h"

#include "ocean/base/Thread.h"

#include "ocean/math/Numeric.h"

#include <cerrno>

#ifdef __GNUC__
	#include <sys/select.h>
#endif

namespace Ocean
{

namespace Network
{

PackagedConnectionlessClient::PackagedConnectionlessClient() :
	Client()
{
	// nothing to do here
}

PackagedConnectionlessClient::~PackagedConnectionlessClient()
{
	// nothing to do here
}

PackagedConnectionlessClient::SocketResult PackagedConnectionlessClient::send(const Address4& address, const Port& port, const void* data, const size_t size)
{
	if (size == 0)
	{
		return SR_SUCCEEDED;
	}

	if (data == nullptr)
	{
		ocean_assert(false && "Invalid input!");
		return SR_FAILED;
	}

	if (size >= size_t(NumericT<int>::maxValue()))
	{
		return SR_FAILED;
	}

	const ScopedLock scopedLock(lock_);

	ocean_assert(maximalPackageSize_ != 0);
	ocean_assert(packageManagmentHeaderSize() < maximalPackageSize_);

	if (clientPackageBuffer_.size() != maximalPackageSize_)
	{
		clientPackageBuffer_.resize(maximalPackageSize_);
	}

	if (clientPackageBuffer_.empty() || socketId_ == invalidSocketId())
	{
		return SR_FAILED;
	}

	const size_t maximalPayloadSize = maximalPackageSize_ - packageManagmentHeaderSize();

	sockaddr_in receiver = {};
	receiver.sin_family = AF_INET;
	receiver.sin_addr.s_addr = address;
	receiver.sin_port = port;

	const MessageId messageId =  ++messageCounter_;

	unsigned int packageIndex = 0u;
	const unsigned int totalPackages = (unsigned int)(size / maximalPayloadSize + min(size_t(1), size % maximalPayloadSize));

	size_t pendingBytes = size;
	size_t dataStartPosition = 0;

	const unsigned char* data8 = (const unsigned char*)(data);

	while (pendingBytes != 0)
	{
		ocean_assert(packageIndex < totalPackages);

		const unsigned int headerValues[5] =
		{
			Data::toBigEndian((unsigned int)(messageId)),
			Data::toBigEndian((unsigned int)(size)),
			Data::toBigEndian((unsigned int)(dataStartPosition)),
			Data::toBigEndian((unsigned int)(packageIndex)),
			Data::toBigEndian((unsigned int)(totalPackages))
		};

		static_assert(sizeof(headerValues) == packageManagmentHeaderSize(), "Header size mismatch");
		memcpy(clientPackageBuffer_.data(), headerValues, sizeof(headerValues));

		const size_t packageDataSize = min(maximalPayloadSize, pendingBytes);
		memcpy(clientPackageBuffer_.data() + packageManagmentHeaderSize(), data8, packageDataSize);

		const int sendSize = int(packageDataSize + packageManagmentHeaderSize());

		// Retry on EAGAIN/EWOULDBLOCK (send buffer full on non-blocking socket)
		constexpr unsigned int maxRetries = 100u;

		bool sendSucceeded = false;

		for (unsigned int retry = 0u; retry < maxRetries; ++retry)
		{
			const int result = sendto(socketId_, (const char*)(clientPackageBuffer_.data()), sendSize, 0, (sockaddr*)&receiver, sizeof(receiver));

			if (result == sendSize)
			{
				sendSucceeded = true;
				break;
			}

			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				// Send buffer is full, wait briefly and retry
				Thread::sleep(1u);
				continue;
			}

			break;
		}

		if (!sendSucceeded)
		{
			Log::debug() << "PackagedConnectionlessClient: Failed to send package " << packageIndex << "/" << totalPackages << ", size: " << sendSize << " bytes, errno: " << errno;
			return SR_FAILED;
		}

		packageIndex++;

		ocean_assert(pendingBytes >= packageDataSize);
		pendingBytes -= packageDataSize;
		data8 += packageDataSize;
		dataStartPosition += packageDataSize;
	}

	return SR_SUCCEEDED;
}

}

}
