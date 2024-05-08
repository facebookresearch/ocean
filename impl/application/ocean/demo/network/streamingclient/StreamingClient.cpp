/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/network/streamingclient/StreamingClient.h"

#include "ocean/network/Resolver.h"
#include "ocean/network/StreamingClient.h"

using namespace Ocean;
using namespace Ocean::Network;

void onStreamingData(const void* data, const size_t size)
{
	ocean_assert_and_suppress_unused(data != nullptr || size != 0, data);

	std::cout << "Data received from server: " << size << " bytes" << std::endl;
}

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

	std::cout << "Please enter the host providing the streaming server, e.g. \"localhost\":" << std::endl;

	char hostName[4096];
	std::cin >> hostName;

	if (std::cin.good() && strlen(hostName) > 0)
	{
		std::cout << std::endl << "Please enter the port of the streaming server, e.g. \"6000\"" << std::endl;

		unsigned short portValue;
		std::cin >> portValue;

		if (std::cin.good() && portValue != 0)
		{
			StreamingClient client;

			if (client.connect(Resolver::resolveFirstIp4(hostName), Port(portValue, Port::TYPE_READABLE)))
			{
				std::cout << "Streaming client successfully connected with the server." << std::endl << std::endl;
			}
			else
			{
				std::cout << "ERROR: Streaming client could not establish a connection with the server." << std::endl << std::endl;
			}

			StreamingClient::Channels channels = client.selectableChannels();
			if (channels.empty() == false)
			{
				std::cout << "The streaming server provides " << channels.size() << " channels:" << std::endl;

				for (unsigned int n = 0; n < channels.size(); n++)
				{
					std::cout << channels[n].c_str() << std::endl;
				}

				std::cout << std::endl;
			}
			else
			{
				std::cout << "No selectable channels could be resolved from the streaming server." << std::endl << std::endl;
			}

			client.setReceiveCallback(StreamingClient::ReceiveCallback(&onStreamingData));

			client.start();

			std::cout << "Press a key to exit" << std::endl << std::endl;
			getchar();
			getchar();

			client.disconnect();

			return 0;
		}
	}

	std::cout << "Invalid host or port!" << std::endl;
	std::cout << "Press a key to exit" << std::endl << std::endl;
	getchar();
	getchar();

	return 0;
}
