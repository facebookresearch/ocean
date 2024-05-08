/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/openglstereo/win/OpenGLStereo.h"
#include "application/ocean/demo/rendering/openglstereo/win/OpenGLStereoMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"

#include "ocean/media/Manager.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/DirectShow.h"
#endif

#include "ocean/platform/win/System.h"

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::registerDirectShowLibrary();
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	try
	{
		OpenGLStereoMainWindow mainWindow(hInstance, String::toWString(std::string("OpenGL Stereo, ") + Build::buildString()));
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		ocean_assert(false && "An unhandled exception occurred!");
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::unregisterDirectShowLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
