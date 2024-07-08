/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/TCPServer.h"

#ifndef _WINDOWS
	#include <errno.h>
	#include <unistd.h>
#endif

namespace Ocean
{

namespace Network
{

TCPServer::TCPServer() :
	ConnectionOrientedServer()
{
	buildSocket();
}

TCPServer::~TCPServer()
{
	const ScopedLock scopedLock(lock_);

	for (ConnectionMap::const_iterator i = connectionMap_.cbegin(); i != connectionMap_.cend(); ++i)
	{
#ifdef _WINDOWS
		closesocket(i->second.id());
#else
		close(i->second.id());
#endif
	}

	connectionMap_.clear();

	releaseSocket();
}

bool TCPServer::start()
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ == invalidSocketId())
	{
		return false;
	}

	if (listen(socketId_, SOMAXCONN) != 0)
	{
		return false;
	}

	if (port().isNull())
	{
		return false;
	}

	return ConnectionOrientedServer::start();
}

bool TCPServer::disconnect(const ConnectionId connectionId)
{
	const ScopedLock scopedLock(lock_);

	ConnectionMap::iterator i = connectionMap_.find(connectionId);
	if (i == connectionMap_.end())
	{
		return false;
	}

#ifdef _WINDOWS
	const int result = shutdown(i->second.id(), SD_BOTH);

	if (result != 0)
	{
		if (WSAGetLastError() != WSAENOTCONN)
		{
			return false;
		}
	}

	closesocket(i->second.id());
#else
	const int result = shutdown(socketId_, SHUT_RDWR);

	if (result != 0)
	{
		if (errno != ENOTCONN)
		{
			return false;
		}
	}

	close(i->second.id());
#endif

	connectionMap_.erase(i);
	return result == 0;
}

bool TCPServer::buildSocket()
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ != invalidSocketId())
	{
		return true;
	}

	socketId_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ocean_assert(socketId_ != invalidSocketId());

	if (setBlockingMode(false) == false)
	{
		ocean_assert(false && "This should never happen.");
		releaseSocket();
		return false;
	}

	return true;
}

}

}
