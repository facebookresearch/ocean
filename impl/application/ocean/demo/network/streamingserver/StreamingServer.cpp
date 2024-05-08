/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/network/streamingserver/StreamingServer.h"

#include "ocean/base/Scheduler.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/network/StreamingServer.h"

using namespace Ocean;
using namespace Ocean::Network;

// Forward declaration
void onChannelA(const StreamingServer::State state);
// Forward declaration
void onChannelB(const StreamingServer::State state);

// Forward declaration
void createStreamingDataChannelA();
// Forward declaration
void createStreamingDataChannelB();

StreamingServer::ChannelId channelIdA = StreamingServer::invalidChannelId();
StreamingServer::ChannelId channelIdB = StreamingServer::invalidChannelId();

bool pushChannelA = false;
bool pushChannelB = false;

StreamingServer* serverPointer = nullptr;

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int /*argc*/, wchar_t** /*argv*/)
#elif defined(__APPLE__)
	// main function on OSX platforms
	int main(int /*argc*/, char** /*argv*/)
#else
	#error Missing implementation.
#endif
{
	Messenger::get().setOutputStream(std::cout);

	std::cout << std::endl << "Please enter the listening port of this streaming server, e.g. \"6000\"" << std::endl;

	unsigned short portValue;
	std::cin >> portValue;

	if (std::cin.good() && portValue != 0)
	{
		Scheduler::get().registerFunction(Scheduler::Callback(&createStreamingDataChannelA));
		Scheduler::get().registerFunction(Scheduler::Callback(&createStreamingDataChannelB));

		StreamingServer server;
		serverPointer = &server;

		if (server.setPort(Port(portValue, Port::TYPE_READABLE)))
		{
			std::cout << "Set the listening port of the streaming server to \"" << server.port().readable() << "\"." << std::endl << std::endl;
		}
		else
		{
			std::cout << "ERROR:  Could not set the listening port of the streaming server!" << std::endl << std::endl;
		}

		channelIdA = server.registerChannel("Channel A", "some data", StreamingServer::ChannelCallback(&onChannelA));
		if (channelIdA == StreamingServer::invalidChannelId())
		{
			std::cout << "Registered a first channel." << std::endl;
		}

		channelIdB = server.registerChannel("Channel B", "some data", StreamingServer::ChannelCallback(&onChannelB));
		if (channelIdB == StreamingServer::invalidChannelId())
		{
			std::cout << "Registered a second channel." << std::endl;
		}

		if (server.enable())
		{
			std::cout << "Enabled the streaming server." << std::endl << std::endl;
		}
		else
		{
			std::cout << "ERROR: Could not enabled the streaming server!" << std::endl << std::endl;
		}

		std::cout << "Press a key to exit" << std::endl;
		getchar();
		getchar();

		Scheduler::get().unregisterFunction(Scheduler::Callback(&createStreamingDataChannelA));
		Scheduler::get().unregisterFunction(Scheduler::Callback(&createStreamingDataChannelB));

		server.release();
		server.disable();

		return 0;
	}

	std::cout << "Press a key to exit" << std::endl;
	getchar();
	getchar();

	return 0;
}

void onChannelA(const StreamingServer::State state)
{
	switch (state)
	{
		case StreamingServer::STATE_START:
			pushChannelA = true;
			std::cout << "ChannelA: Start." << std::endl;
			break;

		case StreamingServer::STATE_PAUSE:
			pushChannelA = false;
			std::cout << "ChannelA: Pause." << std::endl;
			break;

		case StreamingServer::STATE_STOP:
			pushChannelA = false;
			std::cout << "ChannelA: Stop." << std::endl;
			break;

		case StreamingServer::STATE_TYPE_CHANGED:
			break;
	}
}

void onChannelB(const StreamingServer::State state)
{
	switch (state)
	{
		case StreamingServer::STATE_START:
			pushChannelB = true;
			std::cout << "ChannelB: Start." << std::endl;
			break;

		case StreamingServer::STATE_PAUSE:
			pushChannelB = false;
			std::cout << "ChannelB: Pause." << std::endl;
			break;

		case StreamingServer::STATE_STOP:
			pushChannelB = false;
			std::cout << "ChannelB: Stop." << std::endl;
			break;

		case StreamingServer::STATE_TYPE_CHANGED:
			break;
	}
}

void createStreamingDataChannelA()
{
	if (pushChannelA)
	{
		static unsigned int size = 1024 * 1024;
		static unsigned char* buffer = new unsigned char[size];

		serverPointer->stream(channelIdA, buffer, size);
	}
}

void createStreamingDataChannelB()
{
	if (pushChannelB)
	{
		static unsigned int size = 1024 * 1024;
		static unsigned char* buffer = new unsigned char[size];

		serverPointer->stream(channelIdB, buffer, size);
	}
}
