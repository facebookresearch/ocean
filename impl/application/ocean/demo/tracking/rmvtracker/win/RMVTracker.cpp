/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/rmvtracker/win/RMVTracker.h"
#include "application/ocean/demo/tracking/rmvtracker/win/RMVTrackerMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/DirectShow.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
	#include "ocean/media/wic/WIC.h"
#endif

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	Messenger::get().setFileOutput("demotrackingrmvtrackeroutput.txt");
	Messenger::get().setOutputType(Messenger::MessageOutput(Messenger::OUTPUT_FILE | Messenger::OUTPUT_DEBUG_WINDOW));

	const IO::Directory environmentDirectory(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	IO::File absoluteFile("cameracalibration.occ");

	if (!absoluteFile.exists())
	{
		const IO::File relativeFile("/data/cameracalibration/cameracalibration.occ");

		absoluteFile = environmentDirectory + relativeFile;
	}

	if (absoluteFile.exists())
	{
		IO::CameraCalibrationManager::get().registerCalibrationFile(absoluteFile());
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::registerDirectShowLibrary();
	Media::MediaFoundation::registerMediaFoundationLibrary();
	Media::WIC::registerWICLibrary();
#else
	PluginManager::get().collectPlugins(environmentDirectory() + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	const Platform::Utilities::Commands commands(Platform::Utilities::parseCommandLine(lpCmdLine));

	std::string mediaFilename;
	if (commands.size() >= 1)
	{
		mediaFilename = String::toAString(commands[0]);
	}

	std::string patternFilename;
	if (commands.size() >= 2)
	{
		patternFilename = String::toAString(commands[1]);
	}

	try
	{
		RMVTrackerMainWindow mainWindow(hInstance, std::wstring(L"RMV feature tracker, ") + String::toWString(Build::buildString()), mediaFilename, patternFilename);
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		// nothing to do here
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
	Media::MediaFoundation::unregisterMediaFoundationLibrary();
	Media::DirectShow::unregisterDirectShowLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
