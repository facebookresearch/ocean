/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/liveundistort/win/LiveUndistort.h"
#include "application/ocean/demo/cv/liveundistort/win/LiveUndistortMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/PluginManager.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/media/Utilities.h"

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
	CommandArguments commandArguments("Demo application for live camera undistortion");
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as input parameter");
	commandArguments.registerParameter("help", "h", "Showing this help output.");
	commandArguments.registerParameter("input", "i", "Input to be used for undistortion, e.g., a video file or live camera");
	commandArguments.registerParameter("resolution", "r", "Optional: the resolution of the input, e.g., \"1280x720\"");
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

	std::string input;
	if (!commandArguments.hasValue("input", input, false, 0u) || input.empty())
	{
		input = commandArguments.value<std::string>("input", std::string(), true);
	}

	std::string resolution;
	commandArguments.hasValue("resolution", resolution, false, 0u);

	try
	{
		LiveUndistortMainWindow mainWindow(hInstance, String::toWString(std::string("Live Undistort (") + Build::buildString() + std::string(")")), input, resolution);
		mainWindow.initialize();
		mainWindow.start();
	}
	catch (...)
	{
		ocean_assert(false && "Unhandled exception");
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
