/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/rmvtracker/win/RMVTracker.h"
#include "application/ocean/demo/tracking/rmvtracker/win/RMVTrackerMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
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

	CommandArguments commandArguments("Demo application for RMV feature tracker");
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as media input, the second as pattern file");
	commandArguments.registerParameter("help", "h", "Showing this help output.");
	commandArguments.registerParameter("input", "i", "Input to be used for tracking, e.g., a video file or live camera");
	commandArguments.registerParameter("pattern", "p", "The pattern file to be used for tracking");
	commandArguments.registerParameter("calibration", "c", "Optional: the filename of the camera calibration file (*.occ or *.json)");

	commandArguments.parse(Platform::Utilities::parseCommandLine(lpCmdLine));

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return 0;
	}

	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	IO::File cameraCalibrationFile;

	Value calibrationValue;
	if (commandArguments.hasValue("calibration", &calibrationValue, false, 0u) && calibrationValue.isString())
	{
		const IO::File file(calibrationValue.stringValue());

		if (file.exists())
		{
			cameraCalibrationFile = file;
		}
	}

	if (cameraCalibrationFile.isNull())
	{
		const IO::File relativeFile("res/ocean/cv/calibration/camera_calibration.json");

		cameraCalibrationFile = IO::Directory(frameworkPath) + relativeFile;
	}

	if (cameraCalibrationFile.exists())
	{
		IO::CameraCalibrationManager::get().registerCalibrations(cameraCalibrationFile());
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::registerDirectShowLibrary();
	Media::MediaFoundation::registerMediaFoundationLibrary();
	Media::WIC::registerWICLibrary();
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	std::string mediaFilename;
	if (!commandArguments.hasValue("input", mediaFilename, false, 0u) || mediaFilename.empty())
	{
		mediaFilename = commandArguments.value<std::string>("input", std::string(), true);
	}

	std::string patternFilename;
	if (!commandArguments.hasValue("pattern", patternFilename, false, 0u) || patternFilename.empty())
	{
		patternFilename = commandArguments.value<std::string>("pattern", std::string(), true);
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
