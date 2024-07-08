/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/network/maintenancereceiver/osx/MRApplication.h"

#include "application/ocean/demo/network/maintenancereceiver/MaintenanceConnector.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/String.h"

using namespace Ocean;

int main(int argc, const char* argv[])
{
	Messenger::get().setOutputStream(std::cout);
	Messenger::get().setOutputType(Messenger::OUTPUT_STREAM);

	Log::info() << "Maintenance Receiver demo application";
	Log::info() << " ";

	const bool useTCPConnector = argc < 2 || String::toLower(String::trim(std::string(argv[1]), '\"')) != std::string("udp");

	unsigned short portValue = 0;
	if (argc >= 3)
	{
		int value = 0;
		if (String::isInteger32(std::string(argv[2]), &value) && value >= 0 && value <= 65535)
			portValue = (unsigned short)value;
	}

	if (portValue == 0)
	{
		std::cout << "Please enter the port of this " << (useTCPConnector ? std::string("TCP") : std::string("UDP")) << " receiver, e.g. \"6000\"" << std::endl;

		std::cin >> portValue;

		if (std::cin.fail())
			portValue = 0;
	}

	startConnector(useTCPConnector, portValue);

	std::cout << "Press a key to exit" << std::endl;
	getchar();

	getchar();

	return 0;
}
