/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/calibration/detector/win/Detector.h"
#include "application/ocean/demo/cv/calibration/detector/win/DetectorMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"

#include "ocean/cv/calibration/Utilities.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/File.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/DirectShow.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
	#include "ocean/media/wic/WIC.h"
#endif

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);

#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::registerDirectShowLibrary();
	Media::MediaFoundation::registerMediaFoundationLibrary();
	Media::WIC::registerWICLibrary();
#endif // OCEAN_RUNTIME_STATIC

	CommandArguments commandArguments;
	commandArguments.registerParameter("calibrationBoard", "cb", "The calibration board to be used, either the file containing a calibration board or a seed-based definition e.g., 'calibrationBoard_0_8x11");
	commandArguments.registerParameter("measuredWidth", "mw", "The measured width of the calibration board in millimeters.");
	commandArguments.registerParameter("measuredHeight", "mh", "The measured height of the calibration board in millimeters.");
	commandArguments.registerParameter("cameraCalibration", "cc", "The filename of a camera calibration file containing the calibration for the input source (*.occ)");
	commandArguments.registerParameter("input", "i", "Optional media file to use instead of live video");
	commandArguments.registerParameter("gaussianFilter", "gf", "The optional size of a Gaussian filter applied to all input frames before processing, 0 to disable (default), e.g., 3, 5, 7", Value(0));
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(lpCmdLine);

	if (commandArguments.hasValue("help", nullptr, false))
	{
		Log::info() << "Ocean Camera Calibration Pattern Detector:";
		Log::info() << " ";
		Log::info() << commandArguments.makeSummary();

		return 0;
	}

	std::string cameraCalibrationFile;
	if (commandArguments.hasValue("cameraCalibration", cameraCalibrationFile))
	{
		const IO::File file(cameraCalibrationFile);

		if (file.exists())
		{
			if (IO::CameraCalibrationManager::get().registerCalibrations(file()))
			{
				Log::info() << "Loaded camera calibration from file: " << cameraCalibrationFile;
			}
			else
			{
				Log::warning() << "Failed to load camera calibration from file: " << cameraCalibrationFile;
			}
		}
		else
		{
			Log::warning() << "Camera calibration file does not exist: " << cameraCalibrationFile;
		}
	}

	const std::string mediaFile = commandArguments.value<std::string>("input", std::string(), false);

	const uint32_t gaussianFilterSize = commandArguments.value<uint32_t>("gaussianFilter", 0u, true);

	if (gaussianFilterSize != 0u && (gaussianFilterSize < 3u || (gaussianFilterSize % 2u) == 0u))
	{
		Log::error() << "Invalid Gaussian filter size '" << gaussianFilterSize << "', must be 0 (disabled), or an odd value >= 3, e.g., 3, 5, 7.";
		return 1;
	}

	const MetricCalibrationBoard calibrationBoard = determineCalibrationBoard(commandArguments);

	try
	{
		DetectorMainWindow mainWindow(hInstance, String::toWString(std::string("Calibration Pattern (") + Build::buildString() + std::string(")")), calibrationBoard, mediaFile, gaussianFilterSize);
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
#endif // OCEAN_RUNTIME_STATIC

	return 0;
}

MetricCalibrationBoard determineCalibrationBoard(const CommandArguments& commandArguments)
{
	std::string calibrationBoardString;
	if (!commandArguments.hasValue("calibrationBoard", calibrationBoardString))
	{
		Log::error() << "Missing calibration board definition.";
		return MetricCalibrationBoard();
	}

	CalibrationBoard calibrationBoard;

	const IO::File calibrationBoardFile(calibrationBoardString);

	if (calibrationBoardFile.exists())
	{
		if (!CV::Calibration::Utilities::readCalibrationBoardFromFile(calibrationBoardFile(), calibrationBoard))
		{
			Log::error() << "The calibration board file '" << calibrationBoardFile() << "' could not be parsed.";
			return MetricCalibrationBoard();
		}
	}
	else
	{
		if (!CV::Calibration::Utilities::createCalibrationBoardFromSeed(calibrationBoardString, calibrationBoard))
		{
			Log::error() << "The calibration board type '" << calibrationBoardString << "' could not be parsed.";
			return MetricCalibrationBoard();
		}
	}

	ocean_assert(calibrationBoard.isValid());

	double width = 0.0;
	if (!commandArguments.hasValue("measuredWidth", width) || width <= 0.0)
	{
		Log::error() << "Invalid measured width.";
		return MetricCalibrationBoard();
	}

	double height = 0.0;
	if (!commandArguments.hasValue("measuredHeight", height) || height <= 0.0)
	{
		Log::error() << "Invalid measured height.";
		return MetricCalibrationBoard();
	}

	const MetricSize measuredWidth(width, MetricSize::UT_MILLIMETER);
	const MetricSize measuredHeight(height, MetricSize::UT_MILLIMETER);

	return MetricCalibrationBoard(std::move(calibrationBoard), measuredWidth, measuredHeight);
}
