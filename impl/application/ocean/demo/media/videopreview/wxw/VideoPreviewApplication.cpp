/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/videopreview/wxw/VideoPreviewApplication.h"
#include "application/ocean/demo/media/videopreview/wxw/VideoPreviewMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"

#if defined(_WINDOWS)
	#include "ocean/media/directshow/DirectShow.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
#include "ocean/media/wic/WIC.h"
#elif defined(__APPLE__)
	#include "ocean/media/avfoundation/AVFoundation.h"
#endif

#include "ocean/platform/wxwidgets/System.h"

using namespace Ocean;

// WxWidgets specific implementation of e.g. the application entry function
IMPLEMENT_APP(App)

bool App::OnInit()
{
	const std::string frameworkPath(Ocean::Platform::WxWidgets::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_SHARED
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Ocean::Build::buildString());
	PluginManager::get().loadPlugins(Ocean::PluginManager::TYPE_MEDIA);
#else
	OCEAN_APPLY_IF_WINDOWS(Media::DirectShow::registerDirectShowLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::registerMediaFoundationLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::registerAVFLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());
#endif

	MainWindow* mainWindow = new MainWindow((std::wstring(L"Video Preview, ") + Ocean::String::toWString(Ocean::Build::buildString())).c_str(), wxPoint(50, 50), wxSize(400, 400));
	mainWindow->Show(true);
	SetTopWindow(mainWindow);

	return true;
}

void App::CleanUp()
{

#ifdef OCEAN_RUNTIME_SHARED
	PluginManager::get().release();
#else
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::unregisterAVFLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::unregisterMediaFoundationLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::DirectShow::unregisterDirectShowLibrary());
#endif

	wxApp::CleanUp();
}
