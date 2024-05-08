/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/TCPClient.h"

#ifndef _WINDOWS
	#include <errno.h>

	#include <sys/select.h>
#endif

namespace Ocean
{

namespace Network
{

TCPClient::TCPClient() :
	ConnectionOrientedClient()
{
	buildSocket();
}

TCPClient::~TCPClient()
{
	releaseSocket();
}

bool TCPClient::connect(const Address4& address, const Port& port, const unsigned int timeout)
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ == invalidSocketId())
	{
		return false;
	}

	if (receiverAddress_.isNull() == false || receiverPort_.isNull() == false)
	{
		return false;
	}

	if (isConnected_)
	{
		return address == receiverAddress_ && port == receiverPort_;
	}

	sockaddr_in receiver;
	receiver.sin_family = AF_INET;
	receiver.sin_addr.s_addr = address;
	receiver.sin_port = port;

	ocean_assert(socketId_ != invalidSocketId());

	// we are in non blocking mode therefore at this moment no connection is guaranteed
	const int connectResult = ::connect(socketId_, (sockaddr*)&receiver, sizeof(receiver));

	if (connectResult != 0)
	{
#ifdef _WINDOWS
		const int errorValue = WSAGetLastError();

		// check whether the socket was connected already
		if (errorValue == WSAEISCONN)
		{
			return ConnectionOrientedClient::connect(address, port);
		}

		// sometimes WSAEINVAL is provides although all parameters are valid
		if (errorValue == WSAEINVAL)
		{
			return false;
		}

		ocean_assert(errorValue == WSAEWOULDBLOCK || errorValue == WSAEALREADY);
#else
		const int errorValue = errno;

		if (errorValue == ECONNREFUSED)
		{
			return false;
		}

		if (errorValue == EISCONN)
		{
			return ConnectionOrientedClient::connect(address, port);
		}

		// sometimes WSAEINVAL is provides although all parameters are valid
		if (errorValue == EINVAL)
		{
			return false;
		}

		ocean_assert(errorValue == EINPROGRESS || errorValue == EALREADY);
#endif

		fd_set writeSockets;
		FD_ZERO(&writeSockets);
		FD_SET(socketId_, &writeSockets);

#ifdef _WINDOWS
		timeval timeoutVal = {long(timeout / 1000u), long(timeout % 1000u)};
#else
		timeval timeoutVal = {time_t(timeout / 1000u), suseconds_t(timeout % 1000u)};
#endif

		// check whether the connection has been established successfully
		if (select(int(socketId_ + 1), nullptr, &writeSockets, nullptr, &timeoutVal) != 1)
		{
			return false;
		}

#ifndef _WINDOWS
		int optErrorValue = -1;
		socklen_t optErrorLength = sizeof(optErrorValue);
		getsockopt(socketId_, SOL_SOCKET, SO_ERROR, (void*)(&optErrorValue), &optErrorLength);

		if (optErrorValue != 0)
		{
			return false;
		}
#endif
	}

	return ConnectionOrientedClient::connect(address, port);
}

bool TCPClient::connect(const unsigned int timeout)
{
	const ScopedLock scopedLock(lock_);

	if (isConnected_)
	{
		return true;
	}

	if (receiverAddress_.isNull() || receiverPort_.isNull())
	{
		return false;
	}

	sockaddr_in receiver;
	receiver.sin_family = AF_INET;
	receiver.sin_addr.s_addr = receiverAddress_;
	receiver.sin_port = receiverPort_;

	// we are in non blocking mode therefore at this moment no connection is guaranteed
	const int connectResult = ::connect(socketId_, (sockaddr*)(&receiver), sizeof(receiver));

	if (connectResult != 0)
	{
#ifdef _WINDOWS
		ocean_assert(WSAGetLastError() == WSAEWOULDBLOCK);
#else
		ocean_assert(errno == EINPROGRESS);
#endif

		fd_set writeSockets;
		FD_ZERO(&writeSockets);
		FD_SET(socketId_, &writeSockets);

#ifdef _WINDOWS
		timeval timeoutVal = {long(timeout / 1000u), long(timeout % 1000u)};
#else
		timeval timeoutVal = {time_t(timeout / 1000u), suseconds_t(timeout % 1000u)};
#endif

		// check whether the connection has been established successfully
		if (select(int(socketId_ + 1), nullptr, &writeSockets, nullptr, &timeoutVal) != 1)
		{
			return false;
		}
	}

	return ConnectionOrientedClient::connect();
}

bool TCPClient::disconnect()
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ == invalidSocketId())
	{
		return false;
	}

#ifdef _WINDOWS

	const int result = shutdown(socketId_, SD_BOTH);

	if (result != 0)
	{
		if (WSAGetLastError() != WSAENOTCONN)
		{
			return false;
		}
	}

#else
	const int result = shutdown(socketId_, SHUT_RDWR);

	if (result != 0)
	{
		if (errno != ENOTCONN)
		{
			return false;
		}
	}

#endif

	return ConnectionOrientedClient::disconnect();
}

bool TCPClient::buildSocket()
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
