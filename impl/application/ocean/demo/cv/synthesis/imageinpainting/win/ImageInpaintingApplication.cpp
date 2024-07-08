/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/synthesis/imageinpainting/win/ImageInpaintingApplication.h"
#include "application/ocean/demo/cv/synthesis/imageinpainting/win/ImageInpaintingMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"

#include "ocean/platform/wxwidgets/System.h"

// WxWidgets specific implementation of e.g. the application entry function
IMPLEMENT_APP(App)

using namespace Ocean;

bool App::OnInit()
{
	const std::string frameworkPath(Ocean::Platform::WxWidgets::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	MainWindow* mainWindow = new MainWindow((std::wstring(L"Image Inpainting, ") + Ocean::String::toWString(Ocean::Build::buildString())).c_str(), wxPoint(50, 50), wxSize(800, 800));
	mainWindow->Show(true);
	SetTopWindow(mainWindow);

	return true;
}

void App::CleanUp()
{
	// nothing to do here
}
