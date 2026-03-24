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

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);

#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::registerDirectShowLibrary();
	Media::MediaFoundation::registerMediaFoundationLibrary();
	Media::WIC::registerWICLibrary();
#endif // OCEAN_RUNTIME_STATIC

	CommandArguments commandArguments;
	commandArguments.registerParameter("boardSeed", "bs", "The seed of the calibration board", Value(0));
	commandArguments.registerParameter("paper", "p", "The paper size for the calibration board, either 'a3', 'a4', 'letter', or 'tabloid'", Value("a4"));
	commandArguments.registerParameter("boardDimension", "bd", "The explicit marker dimension of the calibration board, \n\te.g., '6x9' to create a board with 6 horizontal markers and 9 vertical markers");
	commandArguments.registerParameter("calibrationBoardFile", "cbf", "The path to an existing calibration board JSON file to load");
	commandArguments.registerParameter("cameraCalibration", "cc", "The filename of a camera calibration file containing the calibration for the input source (*.occ)");
	commandArguments.registerParameter("input", "i", "Optional media file to use instead of live video");
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(lpCmdLine);

	if (commandArguments.hasValue("help", nullptr, false))
	{
		Log::info() << "Ocean Camera Calibration Pattern Detector:";
		Log::info() << " ";
		Log::info() << commandArguments.makeSummary();

		return 0;
	}

	// Check for camera calibration file
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

	std::string mediaFile = commandArguments.value<std::string>("input", std::string(), false);

	CV::Calibration::MetricCalibrationBoard calibrationBoard;

	std::string calibrationBoardFile;
	if (commandArguments.hasValue("calibrationBoardFile", calibrationBoardFile))
	{
		CV::Calibration::CalibrationBoard board;

		if (!CV::Calibration::Utilities::readCalibrationBoardFromFile(calibrationBoardFile, board))
		{
			Log::error() << "Failed to load calibration board file: " << calibrationBoardFile;
			return 1;
		}

		std::string paper;
		if (!commandArguments.hasValue("paper", paper))
		{
			Log::error() << "No valid paper defined for loaded calibration board";
			return 1;
		}

		const CV::Calibration::MetricSize::PaperType paperType = CV::Calibration::MetricSize::translatePaperType(paper);

		if (paperType == CV::Calibration::MetricSize::PT_INVALID)
		{
			Log::error() << "Invalid paper type: " << paper;
			return 1;
		}

		CV::Calibration::MetricSize paperWidth;
		CV::Calibration::MetricSize paperHeight;
		if (!CV::Calibration::MetricSize::determinePaperSize(paperType, paperWidth, paperHeight))
		{
			Log::error() << "Failed to determine the paper size";
			return 1;
		}

		calibrationBoard = CV::Calibration::MetricCalibrationBoard(std::move(board), paperWidth, paperHeight);

		if (!calibrationBoard.isValid())
		{
			Log::error() << "Failed to create metric calibration board from loaded file";
			return 1;
		}

		Log::info() << "Loaded calibration board from file: " << calibrationBoardFile;
	}
	else
	{
		std::string paper;
		if (!commandArguments.hasValue("paper", paper))
		{
			Log::error() << "No valid paper defined";
			return 1;
		}

		const CV::Calibration::MetricSize::PaperType paperType = CV::Calibration::MetricSize::translatePaperType(paper);

		if (paperType == CV::Calibration::MetricSize::PT_INVALID)
		{
			Log::error() << "Invalid paper type: " << paper;
			return 1;
		}

		CV::Calibration::MetricSize paperWidth;
		CV::Calibration::MetricSize paperHeight;
		if (!CV::Calibration::MetricSize::determinePaperSize(paperType, paperWidth, paperHeight))
		{
			Log::error() << "Failed to determine the paper size";
			return 1;
		}

		int32_t boardSeed = -1;
		if (!commandArguments.hasValue("boardSeed", boardSeed) || boardSeed < 0)
		{
			Log::error() << "Invalid calibration board seed";
			return 1;
		}

		std::string boardDimension;
		if (commandArguments.hasValue("boardDimension", boardDimension))
		{
			int32_t horizontalMarkers = -1;
			int32_t verticalMarkers = -1;

			if (CV::Calibration::Utilities::parseMarkerDimension(boardDimension, horizontalMarkers, verticalMarkers))
			{
				ocean_assert(horizontalMarkers > 0 && verticalMarkers > 0);

				if (!CV::Calibration::MetricCalibrationBoard::createMetricCalibrationBoard((unsigned int)(boardSeed), size_t(horizontalMarkers), size_t(verticalMarkers), paperWidth, paperHeight, calibrationBoard))
				{
					Log::error() << "Failed to create calibration board with custom dimension " << horizontalMarkers << "x" << verticalMarkers;
					return 1;
				}
			}
			else
			{
				Log::error() << "Failed to parse marker dimension '" << boardDimension << "'";
				return 1;
			}
		}
		else
		{
			const CV::Calibration::CalibrationBoard board = CV::Calibration::Utilities::createBoardForPaper(paperType, (unsigned int)(boardSeed));

			if (!board.isValid())
			{
				Log::error() << "Failed to create calibration board for paper type";
				return 1;
			}

			calibrationBoard = CV::Calibration::MetricCalibrationBoard(CV::Calibration::CalibrationBoard(board), paperWidth, paperHeight);

			if (!calibrationBoard.isValid())
			{
				Log::error() << "Failed to create metric calibration board";
				return 1;
			}
		}
	}

	try
	{
		DetectorMainWindow mainWindow(hInstance, String::toWString(std::string("Calibration Pattern (") + Build::buildString() + std::string(")")), calibrationBoard, mediaFile);
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
