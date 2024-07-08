/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/advanced/poissonblending/wxw/PoissonBlendingApplication.h"
#include "application/ocean/demo/cv/advanced/poissonblending/wxw/PoissonBlendingMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/String.h"

#include "ocean/platform/wxwidgets/System.h"

#include "ocean/media/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/wic/WIC.h"
#else
	#include "ocean/base/PluginManager.h"
#endif

// WxWidgets specific implementation of e.g. the application entry function
IMPLEMENT_APP(PoissonBlendingApplication)

bool PoissonBlendingApplication::OnInit()
{
	Messenger::get().setFileOutput("democvadvancedpoissonblending_output.txt");
	Messenger::get().setOutputType(Messenger::OUTPUT_FILE);

	const std::string frameworkPath(Platform::WxWidgets::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::registerWICLibrary();
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	PoissonBlendingMainWindow* mainWindow = new PoissonBlendingMainWindow((std::wstring(L"PoissonBlending Viewer, ") + String::toWString(Build::buildString())).c_str(), wxPoint(50, 50), wxSize(800, 600));
	mainWindow->Show(true);
	SetTopWindow(mainWindow);

	return true;
}

void PoissonBlendingApplication::CleanUp()
{
#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
#else
	PluginManager::get().release();
#endif
}
