/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/network/tcpserver/TSApplication.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/Numeric.h"

#include "ocean/network/PackagedTCPServer.h"
#include "ocean/network/TCPServer.h"

using namespace Ocean;
using namespace Ocean::Network;

/**
 * This class holds the relevant connection information in a singleton.
 */
class Connections : public Singleton<Connections>
{
	friend class Singleton<Connections>;

	public:

		/**
		 * Definition of a map mapping connections ids to size values.
		 */
		using IdMap = std::unordered_map<Network::TCPServer::ConnectionId, size_t>;

	public:

		/// The connections' timestamp.
		Timestamp timestamp_;

		/// The map mapping ids to size values.
		IdMap idMap_;

		/// The connections' lock.
		Lock lock_;

	protected:

		/**
		 * Default constructor.
		 */
		Connections() = default;
};

static bool onConnection(const Address4& address, const Port& port, const TCPServer::ConnectionId connectionId)
{
	Log::info() << " ";
	Log::info() << "Accepted connection (" << connectionId << ") from: " << address.readable() << ", " << port.readable();
	Log::info() << " ";

	Connections::get().timestamp_.toNow();

	const ScopedLock scopedLock(Connections::get().lock_);

	ocean_assert(Connections::get().idMap_.find(connectionId) == Connections::get().idMap_.end());
	Connections::get().idMap_[connectionId] = 0;

	return true;
}

static void onDisconnect(const TCPServer::ConnectionId connectionId)
{
	Log::info() << " ";
	Log::info() << "Connection (" << connectionId << ") has been disconnected by the client";
	Log::info() << " ";

	Connections::get().timestamp_.toNow();

	const ScopedLock scopedLock(Connections::get().lock_);
	ocean_assert(Connections::get().idMap_.find(connectionId) != Connections::get().idMap_.end());

	Connections::get().idMap_.erase(connectionId);
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

	RandomI::initialize();

	CommandArguments commandArguments;
	commandArguments.registerParameter("port", "p", "The port of the server e.g., '6000'", Value(6000));
	commandArguments.registerParameter("packaged", "d", "When specified the Packaged TCP Client is used. Otherwise the default TCP Client is used");
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(argv, size_t(argc));

	if (commandArguments.hasValue("help", nullptr, false))
	{
		Log::info() << "TCP Server demo application:";
		Log::info() << " ";
		Log::info() << commandArguments.makeSummary();

		return 0;
	}

	std::shared_ptr<TCPServer> tcpServer;

	if (commandArguments.hasValue("packaged"))
	{
		Log::info() << "Using packaged TCP Server";

		tcpServer = std::make_shared<PackagedTCPServer>();
	}
	else
	{
		tcpServer = std::make_shared<TCPServer>();
	}

	tcpServer->setConnectionRequestCallback(TCPServer::ConnectionRequestCallback::createStatic(&onConnection));
	tcpServer->setDisconnectCallback(TCPServer::DisconnectCallback::createStatic(&onDisconnect));

	Log::info() << "TCP Server demo application";
	Log::info() << " ";

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

	Log::info() << "The server as the following port: " << hostPort.readable();

	tcpServer->setPort(hostPort);

	TCPServer::Buffer buffer(1000 * 1000);
	for (size_t n = 0; n < buffer.size(); ++n)
	{
		buffer[n] = uint8_t(RandomI::random(255u));
	}

	if (!tcpServer->start())
	{
		Log::info() << "Failed to start the TCP server!";
		return 1;
	}

	Log::info() << "Started the TCP server...";
	Log::info() << " ";

	constexpr double timeout = 5.0 * 60.0;

	Connections::get().timestamp_.toNow();

	while (Connections::get().timestamp_ + timeout > Timestamp(true))
	{
		Log::info() << "Waiting for an incoming connection for at most 5 minutes";

		Timestamp nextOutputTimestamp(true);

		while (tcpServer->connections() == 0 && Connections::get().timestamp_ + timeout > Timestamp(true))
		{
			if (Timestamp(true) > nextOutputTimestamp)
			{
				Log::info() << ".";
				nextOutputTimestamp = Timestamp(true) + 2.0;
			}

			Thread::sleep(1u);
		}

		if (tcpServer->connections() == 0)
		{
			Log::info() << "No incoming connection request!";
		}
		else
		{
			Log::info() << "Sending data for at most 1 minutes to each client";

			nextOutputTimestamp.toNow();

			while (Connections::get().timestamp_ + timeout > Timestamp(true) && tcpServer->connections() != 0)
			{
				TemporaryScopedLock scopedLock(Connections::get().lock_);

					Connections::IdMap idMapCopy(Connections::get().idMap_);

				scopedLock.release();

				for (Connections::IdMap::value_type& connectionPair : idMapCopy)
				{
					if (tcpServer->send(connectionPair.first, buffer.data(), buffer.size()) == Socket::SR_SUCCEEDED)
					{
						connectionPair.second += buffer.size();
					}
					else
					{
						Log::info() << "Failed to send data!";
					}
				}

				scopedLock.relock(Connections::get().lock_);

					for (const Connections::IdMap::value_type& connectionPair : idMapCopy)
					{
						Connections::IdMap::iterator iConnection = Connections::get().idMap_.find(connectionPair.first);

						if (iConnection != Connections::get().idMap_.end())
						{
							iConnection->second = connectionPair.first;
						}
					}

					if (!Connections::get().idMap_.empty() && Timestamp(true) >= nextOutputTimestamp)
					{
						Log::info() << "Data sent:";

						for (const Connections::IdMap::value_type& connectionPair : Connections::get().idMap_)
						{
							Log::info() << "Connection (" << connectionPair.first << ") : " << (connectionPair.second >> 10) << "KB";
						}
						Log::info();

						nextOutputTimestamp = Timestamp(true) + 2.0;
					}

				scopedLock.release();

				Thread::sleep(1u);
			}
		}
	}

	Log::info() << "Releasing server...";

	tcpServer = nullptr;

	Log::info() << "Server released";

	return 0;
}
