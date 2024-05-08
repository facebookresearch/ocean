/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/ConnectionlessServer.h"

namespace Ocean
{

namespace Network
{

ConnectionlessServer::ConnectionlessServer() :
	ConnectionlessClient(),
	Server()
{
	// nothing to do here
}

ConnectionlessServer::~ConnectionlessServer()
{
	// nothing to do here
}

bool ConnectionlessServer::onScheduler()
{
	ocean_assert(!buffer_.empty());

	const ScopedLock scopedLock(lock_);

	if (!receiveCallback_ || !schedulerIsActive_ || socketId_ == invalidSocketId())
	{
		return false;
	}

	sockaddr_in senderAddress;
#if defined(OCEAN_POSIX_AVAILABLE)
	socklen_t senderAddressSize = sizeof(senderAddress);
#else
	int senderAddressSize = sizeof(senderAddress);
#endif

	const int size = int(recvfrom(socketId_, (char*)(buffer_.data()), int(buffer_.size()), 0, (sockaddr*)&senderAddress, &senderAddressSize));

	if (size > 0)
	{
		receiveCallback_(Address4(senderAddress.sin_port), Port(senderAddress.sin_port), buffer_.data(), size_t(size));
		return true;
	}

	return false;
}

}

}
