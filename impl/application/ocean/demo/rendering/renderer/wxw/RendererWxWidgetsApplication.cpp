/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/renderer/wxw/RendererWxWidgetsApplication.h"
#include "application/ocean/demo/rendering/renderer/wxw/RendererWxWidgetsMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"

#include "ocean/rendering/Manager.h"

#include "ocean/rendering/glescenegraph/windows/Windows.h"

#include "ocean/rendering/globalillumination/GlobalIllumination.h"

#include "ocean/platform/wxwidgets/System.h"

using namespace Ocean;

// WxWidgets specific implementation of e.g. the application entry function
IMPLEMENT_APP(RendererWxWidgetsApplication)

bool RendererWxWidgetsApplication::OnInit()
{
	RandomI::initialize();

	const std::string frameworkPath(Platform::WxWidgets::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	Rendering::GLESceneGraph::Windows::registerGLESceneGraphEngine();
	Rendering::GlobalIllumination::registerGlobalIlluminationEngine();
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_RENDERING);
#endif

	RendererWxWidgetsMainWindow* mainWindow = new RendererWxWidgetsMainWindow((std::wstring(L"Renderer WxWidgets, ") + String::toWString(Build::buildString())).c_str(), wxPoint(50, 50), wxSize(400, 400));
	mainWindow->Show(true);
	SetTopWindow(mainWindow);

	mainWindow->initalize();

	return true;
}

int RendererWxWidgetsApplication::OnExit()
{
#ifdef OCEAN_RUNTIME_STATIC
	Rendering::GlobalIllumination::unregisterGlobalIlluminationEngine();
	Rendering::GLESceneGraph::unregisterGLESceneGraphEngine();
#else
	PluginManager::get().release();
#endif

	return wxApp::OnExit();
}
