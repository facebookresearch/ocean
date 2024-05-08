/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/Socket.h"
#include "ocean/network/Address4.h"
#include "ocean/network/SocketScheduler.h"

#include "ocean/base/Timestamp.h"

#ifndef _WINDOWS
	#include <fcntl.h>
	#include <unistd.h>
#endif

namespace Ocean
{

namespace Network
{

Socket::Socket()
{
	SocketScheduler::get().registerSocket(*this);
}

Socket::~Socket()
{
	if (socketId_ != invalidSocketId())
#ifdef _WINDOWS
		closesocket(socketId_);
#else
		close(socketId_);
#endif

	SocketScheduler::get().unregisterSocket(*this);

	const Timestamp startTimestamp(true);

	while (!SocketScheduler::get().isSocketUnregistered(*this) && startTimestamp + 5.0 > Timestamp(true))
	{
		Thread::sleep(1);
	}

	ocean_assert(SocketScheduler::get().isSocketUnregistered(*this));
}

Address4 Socket::address() const
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ == invalidSocketId())
	{
		return Address4();
	}

	sockaddr_in address;
#if defined(OCEAN_POSIX_AVAILABLE)
	socklen_t length = sizeof(address);
#else
	int length = sizeof(address);
#endif
	if (getsockname(socketId_, (sockaddr*)&address, &length) != 0)
	{
		return Address4();
	}

	ocean_assert(size_t(length) <= sizeof(address));

	return Address4(address.sin_addr.s_addr);
}

Port Socket::port() const
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ == invalidSocketId())
	{
		return Port();
	}

	sockaddr_in address;
#if defined(OCEAN_POSIX_AVAILABLE)
	socklen_t length = sizeof(address);
#else
	int length = sizeof(address);
#endif

	if (getsockname(socketId_, (sockaddr*)&address, &length) != 0)
	{
		return Port();
	}

	ocean_assert(size_t(length) <= sizeof(address));

	return Port(address.sin_port);
}

bool Socket::setAddress(const Address4& address)
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ == invalidSocketId())
	{
		return false;
	}

	sockaddr_in value;
	value.sin_family = AF_INET;
	value.sin_addr.s_addr = address;
	value.sin_port = 0;

	if (bind(socketId_, (sockaddr*)&value, sizeof(value)) != 0)
	{
		return false;
	}

	return true;
}

bool Socket::setPort(const Port& port)
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ == invalidSocketId())
	{
		return false;
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = port;

	if (bind(socketId_, (sockaddr*)&address, sizeof(address)) != 0)
	{
		return false;
	}

	return true;
}

bool Socket::setBlockingMode(const SocketId socketId, const bool blocking)
{
	ocean_assert(socketId != invalidSocketId());

#ifdef _WINDOWS

	u_long blockingMode = blocking ? 0 : 1;

	return ioctlsocket(socketId, FIONBIO, &blockingMode) != SOCKET_ERROR;

#else

	const int flags = fcntl(socketId, F_GETFL, 0);

	if (flags < 0)
	{
		return false;
	}

	if (blocking)
	{
		return fcntl(socketId, F_SETFL, flags & (~O_NONBLOCK)) == 0;
	}
	else
	{
		return fcntl(socketId, F_SETFL, flags | O_NONBLOCK) == 0;
	}

#endif
}

bool Socket::setBlockingMode(const bool blocking)
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ == invalidSocketId())
	{
		return false;
	}


	return setBlockingMode(socketId_, blocking);
}

size_t Socket::maximalMessageSize(const size_t defaultSize)
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ == invalidSocketId())
	{
		return 0;
	}

	int size = 0;

#if defined(OCEAN_POSIX_AVAILABLE)
	socklen_t sizeLength = sizeof(size);
#else
	int sizeLength = sizeof(size);
#endif

	if (getsockopt(socketId_, SOL_SOCKET, SO_RCVBUF, (char*)&size, &sizeLength) != 0)
	{
		Log::error() << "Could not receive a valid maximal message size of this socket.";
		return defaultSize;
	}

	if (size > 0)
	{
		return size_t(size);
	}

	return defaultSize;
}

bool Socket::releaseSocket()
{
	const ScopedLock scopedLock(lock_);

#ifdef _WINDOWS
	closesocket(socketId_);
#else
	close(socketId_);
#endif

	socketId_ = invalidSocketId();

	return true;
}

bool Socket::onScheduler()
{
	return false;
}

}

}
