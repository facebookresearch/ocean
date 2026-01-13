/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/calibration/picturetaker/win/PictureTakerMain.h"
#include "application/ocean/demo/cv/calibration/picturetaker/win/PictureTakerMainWindow.h"
#include "application/ocean/demo/cv/calibration/picturetaker/win/Resource.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/Messenger.h"

#include "ocean/media/Utilities.h"

#include "ocean/platform/win/System.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/DSLibrary.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
	#include "ocean/media/wic/WIC.h"
#endif

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	Messenger::get().setOutputType(Messenger::MessageOutput(Messenger::OUTPUT_DEBUG_WINDOW | Messenger::OUTPUT_STANDARD));

	CommandArguments commandArguments("Picture Taker - Captures images from a webcam for camera calibration.");
	commandArguments.registerParameter("input", "i", "The input source to be used, e.g., 'LiveVideoId:0' or a URL", Value("LiveVideoId:0"));
	commandArguments.registerParameter("resolution", "r", "The preferred resolution of the input, e.g., '640x480' or '1280x720'", Value("1920x1080"));
	commandArguments.registerParameter("pixelformat", "p", "The preferred pixel format, e.g., 'YUYV16'", Value("YUYV16"));
	commandArguments.registerParameter("output", "o", "The output directory for the captured images, otherwise a default directory will be used");
	commandArguments.registerParameter("help", "h", "Displays this help");

	if (!commandArguments.parse(lpCmdLine))
	{
		Log::warning() << "Failure when parsing the command arguments";
	}

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return 0;
	}

	const std::string inputSource = commandArguments.value<std::string>("input", std::string(), true, 0u);
	const std::string resolutionString = commandArguments.value<std::string>("resolution", std::string(), true);
	const std::string pixelFormatString = commandArguments.value<std::string>("pixelformat", std::string(), true);
	const std::string outputDirectory = commandArguments.value<std::string>("output", std::string(), true);

	unsigned int preferredWidth = 0u;
	unsigned int preferredHeight = 0u;

	if (resolutionString.empty() || !Media::Utilities::parseResolution(resolutionString, preferredWidth, preferredHeight))
	{
		Log::error() << "Invalid resolution format: '" << resolutionString << "'";
		return 1;
	}

	if (pixelFormatString.empty())
	{
		Log::error() << "Invalid pixel format: '" << pixelFormatString << "'";
		return 1;
	}

	const FrameType::PixelFormat preferredPixelFormat = FrameType::translatePixelFormat(pixelFormatString);

	if (preferredPixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		Log::error() << "Invalid pixel format: '" << pixelFormatString << "'";
		return 1;
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::registerDirectShowLibrary();
	Media::MediaFoundation::registerMediaFoundationLibrary();
	Media::WIC::registerWICLibrary();
#else
	const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif

	try
	{
		PictureTakerMainWindow mainWindow(hInstance, std::wstring(L"Picture Taker, ") + String::toWString(Build::buildString()), inputSource, preferredWidth, preferredHeight, preferredPixelFormat, outputDirectory);
		mainWindow.initialize(hInstance, IDI_ICON);
		mainWindow.start();
	}
	catch(...)
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
