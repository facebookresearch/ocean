/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/videopreview/win/VideoPreviewMain.h"
#include "application/ocean/demo/media/videopreview/win/VideoPreviewMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"

#include "ocean/platform/win/System.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/DSLibrary.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
#endif

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::registerDirectShowLibrary();
	Media::MediaFoundation::registerMediaFoundationLibrary();
#else
	const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif

	try
	{
		VideoPreviewMainWindow mainWindow(hInstance, std::wstring(L"Video Preview, ") + String::toWString(Build::buildString()));
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		// nothing to do here
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::MediaFoundation::unregisterMediaFoundationLibrary();
	Media::DirectShow::unregisterDirectShowLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
