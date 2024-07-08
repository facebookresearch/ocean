/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/ConnectionOrientedClient.h"

#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Numeric.h"

#include <climits>

#ifndef _WINDOWS
	 #include <errno.h>
#endif

namespace Ocean
{

namespace Network
{

ConnectionOrientedClient::ConnectionOrientedClient() :
	Client()
{
	// nothing to do here
}

ConnectionOrientedClient::~ConnectionOrientedClient()
{
	disconnect();
}

bool ConnectionOrientedClient::connect(const Address4& address, const Port& port, const unsigned int /*timeout*/)
{
	// the actual implementation for the connection must be done before in a derived class

	const ScopedLock scopedLock(lock_);

	ocean_assert(socketId_ != invalidSocketId());

	if (socketBuffer_.empty())
	{
		socketBuffer_.resize(min(maximalMessageSize(), size_t(262144)));
	}

	receiverAddress_ = address;
	receiverPort_ = port;
	isConnected_ = true;

	return true;
}

bool ConnectionOrientedClient::connect(const unsigned int /*timeout*/)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(socketId_ != invalidSocketId());

	isConnected_ = true;
	return true;
}

bool ConnectionOrientedClient::disconnect()
{
	const ScopedLock scopedLock(lock_);

	// address and port will be stored for a possible reconnection
	isConnected_ = false;
	return true;
}

bool ConnectionOrientedClient::isConnected() const
{
	const ScopedLock scopedLock(lock_);

	return isConnected_;
}

ConnectionOrientedClient::SocketResult ConnectionOrientedClient::send(const void* data, const size_t size)
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

	if (socketId_ == invalidSocketId() || receiverAddress().isNull() || receiverPort().isNull())
	{
		return SR_NOT_CONNECTED;
	}

	const size_t result = onSend(data, size);

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

bool ConnectionOrientedClient::onScheduler()
{
	const ScopedLock scopedLock(lock_);

	if (socketId_ == invalidSocketId() || !isConnected_)
	{
		return false;
	}

	ocean_assert(!socketBuffer_.empty());
	const int received = int(::recv(socketId_, (char*)(socketBuffer_.data()), int(socketBuffer_.size()), 0));

	// check whether the error has to be handled
	if (received < 0)
	{

#ifdef _WINDOWS
		const int error = WSAGetLastError();

		if (error == WSAECONNRESET)
		{
			// Connection reset by peer.
			// An existing connection was forcibly closed by the remote host.

			disconnect();

			if (disconnectCallback_)
			{
				disconnectCallback_();
			}

			return false;
		}
#endif

	}
	// check whether we received data
	else if (received > 0)
	{
		ocean_assert(received <= int(socketBuffer_.size()));

		onReceived(socketBuffer_.data(), size_t(received));
		return true;
	}
	// check whether the connection has been closed by the server
	else if (received == 0)
	{
		disconnect();

		if (disconnectCallback_)
		{
			disconnectCallback_();
		}

		return false;
	}

	return false;
}

size_t ConnectionOrientedClient::onSend(const void* data, const size_t size)
{
	ocean_assert(data != nullptr && size >= 1);
	ocean_assert(size < size_t(NumericT<int>::maxValue()));

	size_t bytesSent = 0;

	constexpr double timeout = 2.0; // seconds

	Timestamp startTimestamp(true);

	while (startTimestamp + timeout > Timestamp(true))
	{
		const size_t remainingBytes = size - bytesSent;
		ocean_assert(remainingBytes <= size);

		const auto result = ::send(socketId_, (const char*)(data) + bytesSent, int(remainingBytes), 0);

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

void ConnectionOrientedClient::onReceived(const void* data, const size_t size)
{
	ocean_assert(data != nullptr && size >= 1);

	if (receiveCallback_)
	{
		receiveCallback_(data, size);
	}
}

}

}
