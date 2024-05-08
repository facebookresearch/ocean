/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/renderer/win/RendererWin.h"
#include "application/ocean/demo/rendering/renderer/win/RendererWinMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/win/System.h"

#include "ocean/rendering/Manager.h"

#include "ocean/rendering/glescenegraph/windows/Windows.h"
#include "ocean/rendering/globalillumination/GlobalIllumination.h"

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	Rendering::GLESceneGraph::Windows::registerGLESceneGraphEngine();
	Rendering::GlobalIllumination::registerGlobalIlluminationEngine();
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_RENDERING);
#endif

	try
	{
		RendererWinMainWindow mainWindow(hInstance, String::toWString(std::string("Render Windows, ") + Build::buildString()).c_str());
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		ocean_assert(false && "An un-handled exception occurred!");
	}

#ifdef OCEAN_RUNTIME_STATIC
	Rendering::GLESceneGraph::unregisterGLESceneGraphEngine();
	Rendering::GlobalIllumination::unregisterGlobalIlluminationEngine();
#else
	PluginManager::get().release();
#endif

	return 0;
}
