// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/math/cameraundistortion/win/CameraUndistortion.h"
#include "application/ocean/demo/math/cameraundistortion/win/CameraUndistortionMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"

#include "ocean/io/Directory.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#include "ocean/platform/win/System.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/wic/WIC.h"
#endif

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::registerWICLibrary();
#else
	const IO::Directory pluginPath(Platform::Win::System::processDirectory() + IO::Directory("plugins"));

	if (pluginPath.exists())
	{
		PluginManager::get().collectPlugins(pluginPath());
	}

	if (PluginManager::get().unloadedPlugins().empty())
	{
		const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	}

	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	const Platform::Utilities::Commands commands(Platform::Utilities::parseCommandLine(lpCmdLine));

	std::string mediaFile;
	if (!commands.empty())
	{
		mediaFile = String::toAString(commands.front());
	}

	try
	{
		CameraUndistortionMainWindow mainWindow(hInstance, String::toWString(std::string("Distorted Frame (") + Build::buildString() + std::string(")")), mediaFile);
		mainWindow.initialize();
		mainWindow.start();
	}
	catch (...)
	{
		ocean_assert(false && "Unhandled exception");
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}