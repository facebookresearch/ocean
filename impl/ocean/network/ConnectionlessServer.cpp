// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
