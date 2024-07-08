/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/UDPServer.h"

namespace Ocean
{

namespace Network
{

UDPServer::UDPServer() :
	Server(),
	ConnectionlessServer()
{
	buildSocket();
}

UDPServer::~UDPServer()
{
	stop();
	releaseSocket();
}

bool UDPServer::start()
{
	const ScopedLock scopedLock(lock_);

	return Server::start();
}

bool UDPServer::buildSocket(const Address4& localAddress, const Port localPort)
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ != invalidSocketId())
	{
		return true;
	}

	socketId_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ocean_assert(socketId_ != invalidSocketId());

	if (setBlockingMode(false) == false)
	{
		ocean_assert(false && "This should never happen.");
		releaseSocket();
		return false;
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = localAddress.isValid() ? (unsigned int)(localAddress) : (unsigned int)(INADDR_ANY);
	address.sin_port = localPort;

	if (bind(socketId_, (sockaddr*)&address, sizeof(address)) != 0)
	{
		ocean_assert(false && "This should never happen.");
		releaseSocket();
		return false;
	}

	return true;
}

}

}
