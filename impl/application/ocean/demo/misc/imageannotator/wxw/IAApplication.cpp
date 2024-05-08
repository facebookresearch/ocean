/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/imageannotator/wxw/IAApplication.h"
#include "application/ocean/demo/misc/imageannotator/wxw/IAMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"

#ifdef _WINDOWS
	#include "ocean/media/wic/WICLibrary.h"
#else
	#include "ocean/media/imageio/IIOLibrary.h"
#endif

#include "ocean/platform/wxwidgets/System.h"

// WxWidgets specific implementation of e.g. the application entry function
IMPLEMENT_APP(IAApplication)

bool IAApplication::OnInit()
{
	const std::string frameworkPath(Ocean::Platform::WxWidgets::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Ocean::Build::buildString());
	PluginManager::get().loadPlugins(Ocean::PluginManager::TYPE_MEDIA);
#endif

	IAMainWindow* mainWindow = new IAMainWindow((std::wstring(L"Image Annotator, ") + Ocean::String::toWString(Ocean::Build::buildString())).c_str(), wxPoint(50, 50), wxSize(800, 800));
	mainWindow->Show(true);
	SetTopWindow(mainWindow);

	return true;
}

void IAApplication::CleanUp()
{

#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
#else
	PluginManager::get().release();
#endif

}
