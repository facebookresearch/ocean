/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/maintenanceviewer/win/MVApplication.h"
#include "application/ocean/demo/misc/maintenanceviewer/win/MVMainWindow.h"

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

#include "ocean/platform/win/System.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/DirectShow.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
	#include "ocean/media/wic/WIC.h"

	#include "ocean/rendering/glescenegraph/windows/Windows.h"
#endif

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	RandomI::initialize();

	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);

	CommandArguments commandArguments;
	commandArguments.registerParameter("port", "p", "The explicit network port the receiver will use, 6000 by default", Value(6000));
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(Platform::Utilities::parseCommandLine(lpCmdLine));

	if (commandArguments.hasValue("help", nullptr, false))
	{
		std::cout << commandArguments.makeSummary() << std::endl;
		return 0;
	}

	const int port = commandArguments.value("port").intValue();

	if (port < 0 || port > 65536)
	{
		std::cout << "Invalid port " << port << std::endl;
		return 1;
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::registerDirectShowLibrary();
	Media::MediaFoundation::registerMediaFoundationLibrary();
	Media::WIC::registerWICLibrary();
	Rendering::GLESceneGraph::Windows::registerGLESceneGraphEngine();
#else
	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	if (!frameworkPath.empty())
	{
		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	}
	else
	{
		PluginManager::get().collectPlugins(Platform::Win::System::processDirectory()() + std::string("plugins"));
	}

	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA | PluginManager::TYPE_RENDERING));
#endif

	try
	{
		Network::MaintenanceTCPConnector maintenanceConnector;
		maintenanceConnector.configurateAsReceiver(Network::Port(uint16_t(port), Network::Port::TYPE_READABLE));

		MVMainWindow mainWindow(hInstance, std::wstring(L"Maintenance Viewer, ") + String::toWString(Build::buildString()));
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		Log::error() << "Unhandled exception!";
	}

#ifdef OCEAN_RUNTIME_STATIC
	Rendering::GLESceneGraph::unregisterGLESceneGraphEngine();
	Media::WIC::unregisterWICLibrary();
	Media::MediaFoundation::unregisterMediaFoundationLibrary();
	Media::DirectShow::unregisterDirectShowLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
