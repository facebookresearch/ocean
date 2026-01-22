/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/openglstereo/win/OpenGLStereo.h"
#include "application/ocean/demo/rendering/openglstereo/win/OpenGLStereoMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/media/Manager.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/DirectShow.h"
#endif

#include "ocean/platform/win/System.h"

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::registerDirectShowLibrary();
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	CommandArguments commandArguments;
	commandArguments.registerParameter("cameracalibration", "cc", "The camera calibration file to be used");
	commandArguments.registerParameter("help", "h", "Show this help output");

	if (!commandArguments.parse(lpCmdLine))
	{
		Log::warning() << "Failure when parsing the command arguments";
	}

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return 0;
	}

	std::string cameraCalibrationFile = commandArguments.value<std::string>("cameracalibration", std::string(), true, 0u);

	if (cameraCalibrationFile.empty())
	{
		cameraCalibrationFile = "res/ocean/cv/calibration/camera_calibration.json";
	}

	if (IO::File(cameraCalibrationFile).exists())
	{
		if (IO::CameraCalibrationManager::get().registerCalibrations(cameraCalibrationFile))
		{
			Log::debug() << "Using camera calibration file: " << cameraCalibrationFile;
		}
	}

	try
	{
		OpenGLStereoMainWindow mainWindow(hInstance, String::toWString(std::string("OpenGL Stereo, ") + Build::buildString()));
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		ocean_assert(false && "An unhandled exception occurred!");
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::unregisterDirectShowLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
