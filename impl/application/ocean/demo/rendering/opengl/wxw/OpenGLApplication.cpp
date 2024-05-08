/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/opengl/wxw/OpenGLApplication.h"
#include "application/ocean/demo/rendering/opengl/wxw/OpenGLMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"

#include "ocean/io/Directory.h"

#ifdef __APPLE__
	#include "ocean/media/avfoundation/AVFoundation.h"
#endif

#ifdef _WINDOWS
	#include "ocean/media/directshow/DirectShow.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
	#include "ocean/media/wic/WIC.h"
#endif

#include "ocean/platform/wxwidgets/System.h"

// WxWidgets specific implementation of e.g. the application entry function
IMPLEMENT_APP(OpenGLApplication)

bool OpenGLApplication::OnInit()
{
	wxApp::OnInit();

#ifdef OCEAN_DEBUG
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);
#endif

#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::registerAVFLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::DirectShow::registerDirectShowLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::registerMediaFoundationLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
#else
	const IO::Directory pluginPath(Platform::WxWidgets::System::processDirectory() + IO::Directory("plugins"));
	if (pluginPath.exists())
		PluginManager::get().collectPlugins(pluginPath());

	if (PluginManager::get().unloadedPlugins().empty())
	{
		const std::string frameworkPath(Platform::WxWidgets::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	}

	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	OpenGLMainWindow* mainWindow = new OpenGLMainWindow((std::wstring(L"Rendering OpenGL, ") + String::toWString(Build::buildString())).c_str(), wxPoint(50, 50), wxSize(400, 400));
	mainWindow->Show(true);
	SetTopWindow(mainWindow);

	mainWindow->initalize();

	return true;
}

int OpenGLApplication::OnExit()
{
#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::unregisterMediaFoundationLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::DirectShow::unregisterDirectShowLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::unregisterAVFLibrary());
#else
	PluginManager::get().release();
#endif

	return wxApp::OnExit();
}
