/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/ConnectionlessClient.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Network
{

ConnectionlessClient::ConnectionlessClient() :
	Client()
{
	// nothing to do here
}

ConnectionlessClient::SocketResult ConnectionlessClient::send(const Address4& address, const Port& port, const void* data, const size_t size)
{
	// we could send everything provided
	if (data == nullptr || size == 0)
	{
		return SR_SUCCEEDED;
	}

	if (size >= size_t(NumericT<int>::maxValue()) || !address.isValid() || !port.isValid())
	{
		return SR_FAILED;
	}

	const ScopedLock scopedLock(lock_);

	if (socketId_ == invalidSocketId())
	{
		return SR_FAILED;
	}

	sockaddr_in receiver;
	receiver.sin_family = AF_INET;
	receiver.sin_addr.s_addr = address;
	receiver.sin_port = port;

	if (int(size) == sendto(socketId_, (const char*)(data), int(size), 0, (sockaddr*)&receiver, sizeof(receiver)))
	{
		return SR_SUCCEEDED;
	}

	return SR_FAILED;
}

}

}
