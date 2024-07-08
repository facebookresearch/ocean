/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/offline/structurefrommotion/win/SFMApplication.h"
#include "application/ocean/demo/tracking/offline/structurefrommotion/win/SFMMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/Maintenance.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"

#include "ocean/network/MaintenanceUDPConnector.h"
#include "ocean/network/MaintenanceTCPConnector.h"
#include "ocean/network/Resolver.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/DirectShow.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
	#include "ocean/media/wic/WIC.h"
#endif

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	RandomI::initialize();

	CommandArguments commandArguments;
	commandArguments.registerNamelessParameters("Optional the first parameter is interpreted as input parameter");
	commandArguments.registerParameter("input", "i", "The input media file to be process");
	commandArguments.registerParameter("port", "p", "The explicit network port of the receiver, 6000 by default", Value(6000));
	commandArguments.registerParameter("noDatabaseOutput", "ndo", "Disables writing of a resulting tracking database file");
	commandArguments.registerParameter("noDatabaseInput", "ndi", "Disables reading of the database input file if available");
	commandArguments.registerParameter("skipProcessing", "sp", "Skips the processing of the input in case a tracking database file is available and loaded");
	commandArguments.registerParameter("exitWhenDone", "e", "The application will exit immediately once the input is processed");
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(Platform::Utilities::parseCommandLine(lpCmdLine));

	if (commandArguments.hasValue("help", nullptr, false))
	{
		std::cout << commandArguments.makeSummary() << std::endl;
		return 0;
	}

	const Value inputValue = commandArguments.value("input", false, 0);

	if (!inputValue.isString())
	{
		std::cout << "No input defined";
		return 1;
	}

	const int port = commandArguments.value("port").intValue();

	if (port < 0 || port > 65536)
	{
		std::cout << "Invalid port " << port << std::endl;
		return 1;
	}

	Messenger::get().setOutputType(Messenger::OUTPUT_MAINTENANCE);
	Maintenance::get().setActive(true);

#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::registerDirectShowLibrary();
	Media::MediaFoundation::registerMediaFoundationLibrary();
	Media::WIC::registerWICLibrary();
#else
	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA | PluginManager::TYPE_RENDERING));
#endif

	try
	{
		Network::MaintenanceTCPConnector maintenanceConnector;
		maintenanceConnector.configurateAsSender(Network::Resolver::resolveFirstIp4("localhost"), Network::Port(6000, Network::Port::TYPE_READABLE));

		SFMMainWindow mainWindow(hInstance, std::wstring(L"Structure from Motion, ") + String::toWString(Build::buildString()) + L" " + lpCmdLine, commandArguments);
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		Log::error() << "Unhandled exception!";
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
	Media::MediaFoundation::unregisterMediaFoundationLibrary();
	Media::DirectShow::unregisterDirectShowLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
