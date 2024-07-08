/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/network/tcpclient/android/ClientThread.h"

#include "ocean/base/Messenger.h"

#include "ocean/network/Resolver.h"
#include "ocean/network/TCPClient.h"

using namespace Ocean;

ClientThread::ClientThread() :
	threadReceivedBytes(0),
	threadHostPort(0)
{
	// nothing to do here
}

ClientThread::~ClientThread()
{
	stopThread();
}

bool ClientThread::connect(const std::string& hostAddress, const unsigned short readablePort)
{
	if (isThreadActive() || isThreadInvokedToStart())
		return false;

	threadHostAddress = hostAddress;
	threadHostPort = readablePort;

	return startThread();
}

void ClientThread::threadRun()
{
	threadReceivedBytes = 0;

	Log::info() << "TCP Client demo application";

	Log::info() << "Trying to resolve the provided TCP server address: " << threadHostAddress;

	const Network::Address4 hostAddress = Network::Resolver::resolveFirstIp4(threadHostAddress);

	if (!hostAddress.isValid())
		Log::info() << "The address of the host could not be resolved!";
	else
	{
		Log::info() << "The host has the following address: " << hostAddress.readable();

		const Network::Port hostPort(threadHostPort, Network::Port::TYPE_READABLE);

		if (!hostPort.isValid())
			Log::info() << "The provided port of the host is invalid!";
		else
		{
			Log::info() << "The host has the following port: " << hostPort.readable();
			Log::info() << "Trying to connect with the host for at most 1 minutes";

			Network::TCPClient client;

			const Timestamp startTimestamp(true);
			while (!shouldThreadStop() && !client.isConnected() && startTimestamp + 1 * 60 > Timestamp(true))
				client.connect(hostAddress, hostPort, 10u);

			Log::info() << " ";

			if (!client.isConnected())
				Log::info() << "Failed to establish a connection";
			else
			{
				threadReceivedTimestamp.toNow();

				Log::info() << " ";
				Log::info() << "Connection established";
				Log::info() << " ";
				Log::info() << "Now we receive data from the server for at most 1 minute";
				Log::info() << " ";

				client.setReceiveCallback(Network::TCPClient::ReceiveCallback::create(*this, &ClientThread::onReceiveData));
				client.setDisconnectCallback(Network::TCPClient::DisconnectCallback::create(*this, &ClientThread::onDisconnect));

				const Timestamp startTimestamp(true);
				while (!shouldThreadStop() && client.isConnected() && startTimestamp + 1 * 60 > Timestamp(true))
					Thread::sleep(1u);

				client.disconnect();

				Log::info() << "Connection disconnected by the us (the client)";
			}
		}
	}
}

void ClientThread::onReceiveData(const void* data, const size_t size)
{
	threadReceivedBytes += size;

	const Timestamp currentTimestamp(true);
	static Timestamp nextOutputTimestamp(true);

	if (currentTimestamp >= nextOutputTimestamp)
	{
		const double duration = currentTimestamp - threadReceivedTimestamp;
		const double kbBytesPerSecond = duration > 0 ? double(threadReceivedBytes >> 10) / duration : 0;

		Log::info() << "Data received: " << (threadReceivedBytes >> 10) << "KB, " << kbBytesPerSecond << "KB/s";

		nextOutputTimestamp = currentTimestamp + 2;
	}
}

void ClientThread::onDisconnect()
{
	Log::info() << "Connection has been disconnected by the server";
}
