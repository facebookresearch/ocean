/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/math/discretecosinetransform/win/DiscreteCosineTransform.h"
#include "application/ocean/demo/math/discretecosinetransform/win/DiscreteCosineTransformMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/Exception.h"
#include "ocean/base/PluginManager.h"

#include "ocean/media/wic/WIC.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	Messenger::get().setFileOutput("demomathdiscretecosinetransform_output.txt");
	Messenger::get().setOutputType(Messenger::OUTPUT_FILE);

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::registerWICLibrary();
#else
	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
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
		DiscreteCosineTransformMainWindow mainWindow(hInstance, std::wstring(L"Demo Math DiscreteCosineTransform ") + String::toWString(Build::buildString()), mediaFile);
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(const Exception& e)
	{
		Log::error() << "Unhandled exception: " << e.what();
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
