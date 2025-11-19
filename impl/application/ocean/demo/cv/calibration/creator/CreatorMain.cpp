/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/CommandArguments.h"
#include "ocean/base/String.h"

#include "ocean/cv/calibration/SVGImage.h"
#include "ocean/cv/calibration/Utilities.h"

#include "ocean/io/File.h"

using namespace Ocean;
using namespace Ocean::CV::Calibration;

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int argc, wchar_t* argv[])
#elif defined(__APPLE__) || defined(__linux__)
	// main function on OSX platforms
	int main(int argc, char* argv[])
#else
	#error Missing implementation.
#endif
{
#ifdef OCEAN_COMPILER_MSC
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

#ifdef OCEAN_DEACTIVATED_MESSENGER
	#warning The messenger is currently deactivated.
#endif

	CommandArguments commandArguments;
	commandArguments.registerParameter("boardSeed", "bs", "The seed of the calibration board", Value(0));
	commandArguments.registerParameter("paper", "p", "The paper size for the SVG image, either 'a3', 'a4', 'letter', or 'tabloid'", Value("a4"));
	commandArguments.registerParameter("boardDimension", "bd", "The explicit marker dimension of the calibration board to create, \n\te.g., '6x9' to create a board with 6 horizontal markers and 9 vertical markers,\n\tOnly for used for image types 'board' or 'board_with_dot'");
	commandArguments.registerParameter("output", "o", "The optional explicit output file for created image");
	commandArguments.registerParameter("imageType", "it", "The type of the image to create, possible values are:\n\t'board' for a calibration board;\n\t'markers' for an image with all possible markers;\n\t'points' for two images with different sized points (black dots and white dots);\n'\t'board_with_dot' for a calibration board with center dot", Value("board"));
	commandArguments.registerParameter("debugInformation", "di", "If defined, the resulting image will contain additional debug information");
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(argv, size_t(argc));

	if (commandArguments.hasValue("help", nullptr, false))
	{
		Log::info() << "Ocean Camera Calibration Pattern Creator:";
		Log::info() << " ";
		Log::info() << commandArguments.makeSummary();

		return 0;
	}

	std::string paper;
	if (!commandArguments.hasValue("paper", paper))
	{
		Log::error() << "No valid paper defined";
		return 1;
	}

	const MetricSize::PaperType paperType = MetricSize::translatePaperType(paper);

	if (paperType == MetricSize::PT_INVALID)
	{
		Log::error() << "Invalid paper type: " << paper;
		return 1;
	}

	const std::string paperTypeString = MetricSize::translatePaperType(paperType);

	MetricSize paperWidth;
	MetricSize paperHeight;
	if (!MetricSize::determinePaperSize(paperType, paperWidth, paperHeight))
	{
		Log::error() << "Failed to determine the paper size";
		return 1;
	}

	std::string imageType;
	if (!commandArguments.hasValue("imageType", imageType))
	{
		Log::error() << "Image type needs to be specified";
		return 1;
	}

	std::string output = commandArguments.value<std::string>("output", std::string(), false);

	if (imageType == "markers")
	{
		if (output.empty())
		{
			output = "markers_" + paperTypeString + ".svg";
		}

		if (!SVGImage::writeMarkerTestImage(output, paperWidth, paperHeight, true))
		{
			Log::error() << "Failed to write SVG image '" << output << "'";
			return 1;
		}
	}
	else if (imageType == "points")
	{
		for (const bool blackDots : {true, false})
		{
			IO::File outputFile;

			if (!output.empty())
			{
				outputFile = IO::File(output);
			}
			else
			{
				outputFile = IO::File("points_" + paperTypeString + ".svg");
			}

			if (blackDots)
			{
				outputFile = IO::File(outputFile.base() + "_blackDots." + outputFile.extension());
			}
			else
			{
				outputFile = IO::File(outputFile.base() + "_whiteDots." + outputFile.extension());
			}

			if (!SVGImage::writePointTestImage(outputFile(), paperWidth, paperHeight, blackDots))
			{
				Log::error() << "Failed to write SVG image '" << output << "'";
				return 1;
			}
		}
	}
	else
	{
		ocean_assert(imageType == "board" || imageType == "board_with_dot");

		const bool withCenterDot = imageType == "board_with_dot";

		int32_t boardSeed = -1;
		if (!commandArguments.hasValue("boardSeed", boardSeed) || boardSeed < 0)
		{
			Log::error() << "Invalid calibration board id";
			return 1;
		}

		CalibrationBoard calibrationBoard;

		std::string boardDimension;
		if (commandArguments.hasValue("boardDimension", boardDimension))
		{
			int32_t horizontalMarkers = -1;
			int32_t verticalMarkers = -1;

			if (CV::Calibration::Utilities::parseMarkerDimension(boardDimension, horizontalMarkers, verticalMarkers))
			{
				ocean_assert(horizontalMarkers > 0 && verticalMarkers > 0);

				size_t optimalVerticalMarkers = 0;
				if (CV::Calibration::CalibrationBoard::determineOptimalMarkerGrid(paperWidth / paperHeight, horizontalMarkers, optimalVerticalMarkers) && verticalMarkers != optimalVerticalMarkers)
				{
					Log::warning() << "Creating calibration board with " << horizontalMarkers << "x" << verticalMarkers << " markers, however, the optimal marker dimension would be " << horizontalMarkers << "x" << optimalVerticalMarkers;
				}

				if (!CalibrationBoard::createCalibrationBoard((unsigned int)(boardSeed), size_t(horizontalMarkers), size_t(verticalMarkers), calibrationBoard))
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
			calibrationBoard = CV::Calibration::Utilities::createBoardForPaper(paperType, (unsigned int)(boardSeed));
		}

		if (!calibrationBoard.isValid())
		{
			Log::error() << "Invalid calibration board";
			return 1;
		}

		if (output.empty())
		{
			output = "calibrationBoard_" + paperTypeString + "_" + String::toAString(boardSeed) + "_" + String::toAString(calibrationBoard.xMarkers()) + "x" + String::toAString(calibrationBoard.yMarkers()) + ".svg";
		}

		const bool debugImage = commandArguments.hasValue("debugInformation");

		if (!SVGImage::writeCalibrationBoardImage(output, paperWidth, paperHeight, calibrationBoard, MetricSize::UT_MILLIMETER, 4u, debugImage, withCenterDot))
		{
			Log::error() << "Failed to write SVG image '" << output << "'";
			return 1;
		}
	}

	return 0;
}
