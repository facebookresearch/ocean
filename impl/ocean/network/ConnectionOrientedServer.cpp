/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/ConnectionOrientedServer.h"

#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Numeric.h"

#ifndef _WINDOWS
	#include <errno.h>
	#include <fcntl.h>
	#include <unistd.h>
#endif

namespace Ocean
{

namespace Network
{

ConnectionOrientedServer::ConnectionOrientedServer() :
	Server()
{
	// nothing to do here
}

ConnectionOrientedServer::~ConnectionOrientedServer()
{
	// nothing to do here
}

ConnectionOrientedServer::SocketResult ConnectionOrientedServer::send(const ConnectionId connection, const void* data, const size_t size)
{
	// we could send everything provided
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

	if (connectionMap_.find(connection) == connectionMap_.end())
	{
		return SR_NOT_CONNECTED;
	}

	const size_t result = onSend(connection, data, size);

	if (result == size)
	{
		return SR_SUCCEEDED;
	}

#ifdef _WINDOWS
	if (result == size_t(SOCKET_ERROR))
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return SR_BUSY;
		}
	}
#else
	if (result == -1)
	{
		if (errno == EWOULDBLOCK)
		{
			return SR_BUSY;
		}
	}
#endif

	return SR_FAILED;
}

ConnectionOrientedServer::SocketResult ConnectionOrientedServer::send(const ConnectionId connectionId, const std::string& message)
{
	ocean_assert(strlen(message.c_str()) == message.length() && message.c_str()[message.length()] == '\0');
	return send(connectionId, message.c_str(), message.length() + 1);
}

size_t ConnectionOrientedServer::connections() const
{
	const ScopedLock scopedLock(lock_);

	return connectionMap_.size();
}

bool ConnectionOrientedServer::connectionProperties(const ConnectionId connectionId, Address4& address, Port& port)
{
	const ScopedLock scopedLock(lock_);

	const ConnectionMap::const_iterator iConnection = connectionMap_.find(connectionId);
	if (iConnection == connectionMap_.end())
	{
		return false;
	}

	address = iConnection->second.address();
	port = iConnection->second.port();
	return true;
}

bool ConnectionOrientedServer::onScheduler()
{
	const ScopedLock scopedLock(lock_);

	if (!schedulerIsActive_ || socketId_ == invalidSocketId() || connectionRequestCallback_.isNull())
	{
		return false;
	}

	ocean_assert(!buffer_.empty());

	sockaddr_in request;

	// handle listen events
#if (defined(__clang__) && !defined(_MSC_VER)) || defined(__GNUC__)
	socklen_t length = sizeof(request);
#else
	int length = sizeof(request);
#endif

	SocketId requestSocketId = accept(socketId_, (sockaddr*)(&request), &length);
	if (requestSocketId != invalidSocketId())
	{
		const bool setBlockingResult = setBlockingMode(requestSocketId, false);
		ocean_assert_and_suppress_unused(setBlockingResult, setBlockingResult);

		ocean_assert(length == sizeof(request));
		bool allowConnection = true;

		const Address4 remoteAddress(request.sin_addr.s_addr);
		const Port remotePort(request.sin_port);

		if (connectionRequestCallback_)
		{
			allowConnection = connectionRequestCallback_(remoteAddress, remotePort, connectionCounter_);
		}

		if (allowConnection)
		{
			ocean_assert(connectionMap_.find(connectionCounter_) == connectionMap_.end());
			connectionMap_[connectionCounter_++] = ConnectionObject(requestSocketId, remoteAddress, remotePort);
		}
		else
		{
#ifdef _WINDOWS
			closesocket(requestSocketId);
#else
			close(requestSocketId);
#endif
		}

		requestSocketId = invalidSocketId();
	}

	bool busy = false;

	// handle receptions
	for (ConnectionMap::iterator iConnection = connectionMap_.begin(); iConnection != connectionMap_.end(); /* noop */)
	{
		const int received = int(::recv(iConnection->second.id(), (char*)(buffer_.data()), int(buffer_.size()), 0));

		// check whether the error has to be handled
		if (received < 0)
		{

#ifdef _WINDOWS
			const int error = WSAGetLastError();

			if (error == WSAECONNRESET)
			{
				// Connection reset by peer.
				// An existing connection was forcibly closed by the remote host.

				if (disconnectCallback_)
				{
					disconnectCallback_(iConnection->first);
				}

				iConnection = connectionMap_.erase(iConnection);
				continue;
			}
#endif

		}
		// check whether we received data
		else if (received > 0)
		{
			ocean_assert(received <= int(buffer_.size()));

			onReceived(iConnection->first, buffer_.data(), size_t(received));

			busy = true;
		}
		// the connection has been closed by the client
		else if (received == 0)
		{
			if (disconnectCallback_)
			{
				disconnectCallback_(iConnection->first);
			}

			iConnection = connectionMap_.erase(iConnection);
			continue;
		}

		++iConnection;
	}

	return busy;
}

size_t ConnectionOrientedServer::onSend(const ConnectionId connectionId, const void* data, const size_t size)
{
	ConnectionMap::const_iterator iConnection = connectionMap_.find(connectionId);
	ocean_assert(iConnection != connectionMap_.cend());

	ocean_assert(data != nullptr && size >= 1);
	ocean_assert(size < size_t(NumericT<int>::maxValue()));

	size_t bytesSent = 0;

	constexpr double timeout = 2.0; // seconds

	Timestamp startTimestamp(true);

	while (startTimestamp + timeout > Timestamp(true))
	{
		const size_t remainingBytes = size - bytesSent;
		ocean_assert(remainingBytes <= size);

		const auto result = ::send(iConnection->second.id(), (const char*)(data) + bytesSent, int(remainingBytes), 0);

		if (result >= 0)
		{
			bytesSent += size_t(result);

			if (bytesSent == size)
			{
				break;
			}

			if (result > 0)
			{
				startTimestamp.toNow();
			}
		}

		Thread::sleep(1u);
	}

	return bytesSent;
}

void ConnectionOrientedServer::onReceived(const ConnectionId connectionId, const void* data, const size_t size)
{
	ocean_assert(data != nullptr && size >= 1);

	if (receiveCallback_)
	{
		receiveCallback_(connectionId, data, size);
	}
}

}

}
