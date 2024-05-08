/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/histogram/wxw/HistogramApplication.h"
#include "application/ocean/demo/cv/histogram/wxw/HistogramMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/String.h"

#include "ocean/platform/wxwidgets/System.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/wic/WIC.h"
#else
	#include "ocean/base/PluginManager.h"
#endif

// WxWidgets specific implementation of e.g. the application entry function
IMPLEMENT_APP(HistogramApplication)

bool HistogramApplication::OnInit()
{
	Messenger::get().setFileOutput("democvhistogram_output.txt");
	Messenger::get().setOutputType(Messenger::OUTPUT_FILE);

	const std::string frameworkPath(Platform::WxWidgets::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::registerWICLibrary();
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	HistogramMainWindow* mainWindow = new HistogramMainWindow((std::wstring(L"Histogram Viewer, ") + String::toWString(Build::buildString())).c_str(), wxPoint(50, 50), wxSize(800, 600));
	mainWindow->Show(true);

	SetTopWindow(mainWindow);

	return true;
}

void HistogramApplication::CleanUp()
{

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
#else
	PluginManager::get().release();
#endif

}
