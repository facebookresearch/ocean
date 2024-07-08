/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/PackagedConnectionlessClient.h"
#include "ocean/network/Data.h"

#include "ocean/base/Exception.h"
#include "ocean/base/Thread.h"

#include "ocean/math/Numeric.h"

#include <climits>

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

	sockaddr_in receiver;
	receiver.sin_family = AF_INET;
	receiver.sin_addr.s_addr = address;
	receiver.sin_port = port;

	const MessageId messageId =  ++messageCounter_;

	unsigned int packageIndex = 0u;
	const unsigned int totalPackages = (unsigned int)(size / maximalPayloadSize + min(size_t(1), size % maximalPayloadSize));

	size_t pendingBytes = size;
	size_t dataStartPosition = 0;

	const unsigned char* data8 = (unsigned char*)data;

	while (pendingBytes != 0)
	{
		ocean_assert(packageIndex < totalPackages);

		((unsigned int*)clientPackageBuffer_.data())[0] = Data::toBigEndian((unsigned int)(messageId));
		((unsigned int*)clientPackageBuffer_.data())[1] = Data::toBigEndian((unsigned int)(size));
		((unsigned int*)clientPackageBuffer_.data())[2] = Data::toBigEndian((unsigned int)(dataStartPosition));
		((unsigned int*)clientPackageBuffer_.data())[3] = Data::toBigEndian((unsigned int)(packageIndex));
		((unsigned int*)clientPackageBuffer_.data())[4] = Data::toBigEndian((unsigned int)(totalPackages));

		const size_t packageDataSize = min(maximalPayloadSize, pendingBytes);
		memcpy(clientPackageBuffer_.data() + packageManagmentHeaderSize(), data8, packageDataSize);

		const int sendSize = int(packageDataSize + packageManagmentHeaderSize());

		if (sendSize != sendto(socketId_, (const char*)(clientPackageBuffer_.data()), sendSize, 0, (sockaddr*)&receiver, sizeof(receiver)))
		{
			return SR_FAILED;
		}

		packageIndex++;

		ocean_assert(pendingBytes >= packageDataSize);
		pendingBytes -= packageDataSize;
		data8 += packageDataSize;
		dataStartPosition += packageDataSize;

		if (pendingBytes != 0)
		{
			// as we do not have any feedback regarding the connection or the receiver's cache we will wait some time
			Thread::sleep(10u);
		}
	}

	return SR_SUCCEEDED;
}

}

}
