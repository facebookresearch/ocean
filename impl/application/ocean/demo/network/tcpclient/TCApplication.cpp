/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/network/tcpclient/TCApplication.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Numeric.h"

#include "ocean/network/PackagedTCPClient.h"
#include "ocean/network/Resolver.h"
#include "ocean/network/TCPClient.h"

using namespace Ocean;
using namespace Ocean::Network;

/**
 * Event function for new data received from the TCP server.
 * @param data The data which has been received
 * @param size The number of bytes which have been received
 */
static void onReceiveData(const void* /*data*/, const size_t size)
{
	static size_t totalSize = 0;
	totalSize += size;

	static Timestamp nextOutputTimestamp(true);

	if (Timestamp(true) >= nextOutputTimestamp)
	{
		Log::info() << "Data received";
		Log::info() << (totalSize >> 10) << "KB";
		Log::info() << " ";

		nextOutputTimestamp = Timestamp(true) + 2.0;
	}
}

/**
 * Event function for a disconnect from the server.
 */
static void onDisconnect()
{
	Log::info() << "Connection has been disconnected by the server";
}

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int argc, wchar_t* argv[])
#elif defined(__APPLE__) || defined(__linux__)
	// main function on OSX platforms
	int main(int argc, char* argv[])
#else
	#error Missing implementation.
#endif
{
#ifdef OCEAN_COMPILER_MSC
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	CommandArguments commandArguments;
	commandArguments.registerParameter("server", "s", "The address/host of the server e.g., 'localhost'", Value("localhost"));
	commandArguments.registerParameter("port", "p", "The port of the server e.g., '6000'", Value(6000));
	commandArguments.registerParameter("packaged", "d", "When specified the Packaged TCP Client is used. Otherwise the default TCP Client is used");
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(argv, size_t(argc));

	if (commandArguments.hasValue("help", nullptr, false))
	{
		Log::info() << "TCP Client demo application:";
		Log::info() << " ";
		Log::info() << commandArguments.makeSummary();

		return 0;
	}

	std::shared_ptr<TCPClient> tcpClient;

	if (commandArguments.hasValue("packaged"))
	{
		Log::info() << "Using packaged TCP Client";

		tcpClient = std::make_shared<PackagedTCPClient>();
	}
	else
	{
		tcpClient = std::make_shared<TCPClient>();
	}

	tcpClient->setReceiveCallback(TCPClient::ReceiveCallback::createStatic(&onReceiveData));
	tcpClient->setDisconnectCallback(TCPClient::DisconnectCallback::createStatic(&onDisconnect));

	Log::info() << "TCP Client demo application";
	Log::info() << " ";

	Value addressValue;
	if (!commandArguments.hasValue("server", &addressValue, true) || !addressValue.isString())
	{
		Log::info() << "Invalid address value";
		return 1;
	}

	const Address4 hostAddress = Resolver::resolveFirstIp4(addressValue.stringValue());

	if (!hostAddress.isValid())
	{
		Log::info() << "Failed to resolve address: " << addressValue.stringValue();
		return 1;
	}

	Value portValue;
	if (!commandArguments.hasValue("port", &portValue, true) || !portValue.isInt() || !NumericT<uint16_t>::isInsideValueRange(portValue.intValue()))
	{
		Log::info() << "Invalid port value";
		return 1;
	}

	const Port hostPort(uint16_t(portValue.intValue()), Port::TYPE_READABLE);

	if (!hostPort.isValid())
	{
		Log::info() << "Invalid port value";
		return 1;
	}

	Log::info() << "The host as the following address: " << hostAddress.readable() << " and port: " << hostPort.readable();

	Log::info() << "Trying to connect with the host for 5 minutes";

	constexpr double timeout = 5.0 * 60.0;

	Timestamp startTimestamp(true);

	Timestamp nextOutputTimestamp(true);

	while (!tcpClient->isConnected() && startTimestamp + timeout > Timestamp(true))
	{
		tcpClient->connect(hostAddress, hostPort);

		if (Timestamp(true) > nextOutputTimestamp)
		{
			Log::info() << ".";
			nextOutputTimestamp = Timestamp(true) + 2.0;
		}

		Thread::sleep(1u);
	}

	Log::info() << " ";

	if (tcpClient->isConnected())
	{
		Log::info() << "Connection established";
		Log::info() << " ";

		std::cout << "Now we receive data from the server for 5 minute";

		startTimestamp.toNow();

		while (tcpClient->isConnected() && startTimestamp + timeout > Timestamp(true))
		{
			Thread::sleep(1u);
		}

		if (tcpClient->isConnected())
		{
			if (tcpClient->disconnect())
			{
				Log::info() << "Connection disconnected by the client";
			}
			else
			{
				Log::info() << "Failed to disconnect the connection";
			}
		}
		else
		{
			Log::info() << "Connection disconnected by the server";
		}
	}
	else
	{
		Log::info() << "Failed to establish a connection";
	}

	Log::info() << "Releasing client...";

	tcpClient = nullptr;

	Log::info() << "Client released";

	return 0;
}
