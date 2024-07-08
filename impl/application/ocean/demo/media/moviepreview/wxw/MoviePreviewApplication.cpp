/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/moviepreview/wxw/MoviePreviewApplication.h"
#include "application/ocean/demo/media/moviepreview/wxw/MoviePreviewMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/String.h"

#include "ocean/platform/wxwidgets/System.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/directshow/DirectShow.h"
		#include "ocean/media/mediafoundation/MediaFoundation.h"
		#include "ocean/media/wic/WIC.h"
	#elif defined(__APPLE__)
		#include "ocean/media/avfoundation/AVFoundation.h"
		#include "ocean/media/imageio/ImageIO.h"
	#endif
#else
	#include "ocean/base/PluginManager.h"
#endif

// WxWidgets specific implementation of e.g. the application entry function
IMPLEMENT_APP(MoviePreviewApplication)

using namespace Ocean;

bool MoviePreviewApplication::OnInit()
{
	Messenger::get().setFileOutput("demomediamoviepreview_output.txt");
	Messenger::get().setOutputType(Messenger::OUTPUT_FILE);

	const std::string frameworkPath(Platform::WxWidgets::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::DirectShow::registerDirectShowLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::registerMediaFoundationLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::registerAVFLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	MoviePreviewMainWindow* mainWindow = new MoviePreviewMainWindow((std::wstring(L"Movie preview, ") + Ocean::String::toWString(Ocean::Build::buildString())).c_str(), wxPoint(50, 50), wxSize(800, 600));
	mainWindow->Show(true);
	SetTopWindow(mainWindow);

	return true;
}

void MoviePreviewApplication::CleanUp()
{

#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::unregisterAVFLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::unregisterMediaFoundationLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::DirectShow::unregisterDirectShowLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
#else
	PluginManager::get().release();
#endif

	wxApp::CleanUp();

}
