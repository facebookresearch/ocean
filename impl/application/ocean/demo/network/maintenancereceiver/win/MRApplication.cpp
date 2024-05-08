/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/network/maintenancereceiver/win/MRApplication.h"

#include "application/ocean/demo/network/maintenancereceiver/MaintenanceConnector.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/String.h"

using namespace Ocean;

int wmain(int argc, wchar_t* argv[])
{
#ifdef OCEAN_COMPILER_MSC
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	Messenger::get().setOutputStream(std::cout);
	Messenger::get().setOutputType(Messenger::OUTPUT_STREAM);

	Log::info() << "Maintenance Receiver demo application";
	Log::info() << " ";

	const bool useTCPConnector = argc < 2 || String::toLower(String::trim(String::toAString(argv[1]), '\"')) != std::string("udp");

	unsigned short portValue = 0;
	if (argc >= 3)
	{
		int value = 0;
		if (String::isInteger32(String::toAString(argv[2]), &value) && value >= 0 && value <= 65535)
			portValue = (unsigned short)value;
	}

	if (portValue == 0)
	{
		Log::info() << "Please enter the port of this " << (useTCPConnector ? std::string("TCP") : std::string("UDP")) << " receiver, e.g. \"6000\"";

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
