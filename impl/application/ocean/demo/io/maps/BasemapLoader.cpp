/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Messenger.h"

#include "ocean/io/image/Image.h"

#include "ocean/io/File.h"

#include "ocean/io/maps/Basemap.h"
#include "ocean/io/maps/Utilities.h"

using namespace Ocean;

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

	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	CommandArguments commandArguments;
	commandArguments.registerParameter("file", "f", "The input pbf file containing a Basemap tile");
	commandArguments.registerParameter("level", "l", "The level of the Basemap tile (the z coordinate)");
	commandArguments.registerParameter("tileX", "x", "The horizontal/longitude coordinate of the Basemap tile");
	commandArguments.registerParameter("tileY", "y", "The vertical/latitude coordinate of the Basemap tile");
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(argv, size_t(argc));

	if (commandArguments.hasValue("help", nullptr, false))
	{
		Log::info() << "Ocean Basemap loader demo:";
		Log::info() << " ";
		Log::info() << commandArguments.makeSummary();

		return 0;
	}

	Value fileValue;
	if (!commandArguments.hasValue("file", &fileValue) || !fileValue.isString())
	{
		Log::error() << "Need 'file' parameter";
		return 1;
	}

	Value levelValue;
	if (!commandArguments.hasValue("level", &levelValue) || !levelValue.isInt())
	{
		Log::error() << "Need 'level' parameter";
		return 1;
	}

	Value tileXValue;
	if (!commandArguments.hasValue("tileX", &tileXValue) || !tileXValue.isInt())
	{
		Log::error() << "Need 'tileX' parameter";
		return 1;
	}

	Value tileYValue;
	if (!commandArguments.hasValue("tileY", &tileYValue) || !tileYValue.isInt())
	{
		Log::error() << "Need 'tileY' parameter";
		return 1;
	}

	const int level = levelValue.intValue();

	if (level < 1 || level > 22)
	{
		Log::error() << "Invalid level parameter";
		return 1;
	}

	const int tileX = tileXValue.intValue();
	const int tileY = tileYValue.intValue();

	if (tileX < 0 || tileY < 0)
	{
		Log::error() << "Invalid tile coordinate";
		return 1;
	}

	const IO::Maps::Basemap::TileIndexPair tileIndexPair((unsigned int)(tileY), (unsigned int)(tileX));

	const IO::File file(fileValue.stringValue());

	if (!file.exists())
	{
		Log::error() << "The input file does not exist";
		return 1;
	}

	std::ifstream stream(file().c_str(), std::ios::binary);
	stream.seekg(0, std::ios_base::end);
	const std::istream::pos_type fileSize = stream.tellg();
	stream.seekg(0, std::ios_base::beg);

	if (fileSize == 0)
	{
		Log::error() << "The input file is empty";
		return 1;
	}

	std::vector<uint8_t> tileData(fileSize);
	stream.read((char*)(tileData.data()), tileData.size());

	if (!stream.good())
	{
		Log::error() << "Failed to read the input file";
		return 1;
	}

	const IO::Maps::Basemap::SharedTile tile = IO::Maps::Basemap::newTileFromPBFData((unsigned int)(level), tileIndexPair, tileData.data(), tileData.size());

	if (!tile)
	{
		Log::error() << "Failed to parse the input file";
		return 1;
	}

	const Frame frame = IO::Maps::Utilities::drawToImage(*tile);

	if (!frame.isValid())
	{
		Log::error() << "Failed to draw tile to image";
		return 1;
	}

	const IO::File imageFile(file.base() + ".png");

	if (!IO::Image::writeImage(frame, imageFile()))
	{
		Log::info() << "Failed to write output image";
		return 1;
	}

	Log::info() << "Successfully wrote output image '" << imageFile() << "'";

	return 0;
}
