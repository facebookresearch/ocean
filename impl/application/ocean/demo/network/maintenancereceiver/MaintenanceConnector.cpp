/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/network/maintenancereceiver/MaintenanceConnector.h"

#include "ocean/base/ObjectRef.h"
#include "ocean/base/Thread.h"

#include "ocean/network/MaintenanceTCPConnector.h"
#include "ocean/network/MaintenanceUDPConnector.h"

using namespace Ocean;

bool startConnector(const bool useTCPConnector, const unsigned short port)
{
	const Network::Port hostPort(port, Network::Port::TYPE_READABLE);

	if (!hostPort.isValid())
	{
		Log::info() << "The port of the receiver is invalid!";
		return false;
	}
	else
	{
		Log::info() << "The " << (useTCPConnector ? std::string("TCP") : std::string("UDP")) << " receiver has started on port: " << hostPort.readable();
		Log::info() << " ";

		ObjectRef<Maintenance::Connector> maintenanceConnector;

		if (useTCPConnector)
		{
			Network::MaintenanceTCPConnector* connector = new Network::MaintenanceTCPConnector();
			connector->configurateAsReceiver(hostPort);

			maintenanceConnector = ObjectRef<Maintenance::Connector>(connector);
		}
		else
		{
			Network::MaintenanceUDPConnector* connector = new Network::MaintenanceUDPConnector();
			connector->configurateAsReceiver(hostPort);

			maintenanceConnector = ObjectRef<Maintenance::Connector>(connector);
		}

		std::string name, tag;
		unsigned long long id;
		Maintenance::Buffer buffer;
		Timestamp timestamp;

		while (true)
		{
			if (Maintenance::get().receive(name, id, tag, buffer, timestamp))
			{
				if (name.empty())
					Log::info() << std::string((char*)buffer.data(), buffer.size());
				else
					Log::info() << name + std::string(": ") + std::string((char*)buffer.data(), buffer.size());
			}
			else
				Thread::sleep(1u);
		}
	}

	return true;
}
