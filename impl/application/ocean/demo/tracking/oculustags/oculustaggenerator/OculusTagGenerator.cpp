/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/oculustags/oculustaggenerator/OculusTagGenerator.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/String.h"

#include "ocean/io/File.h"

#include "ocean/media/openimagelibraries/Image.h"

#include "ocean/tracking/oculustags/Utilities.h"

#include <iomanip>

using namespace Ocean;

void displayUsage(const std::string& applicationName)
{
	Log::info() << "Usage:\n"
		 << "    " << applicationName << " [-h|--help] [-o|--output DIRECTORY] [-s|--size TAG_SIZE] [-b|--border BORDER] [TAG_ID0, TAG_ID1, ...]"
		 << "\n"
		 << "\n"
		 << "Parameters:\n"
		 << "    -h | --help              : Display this usage help and exit\n"
		 << "    -o | --output DIRECTORY  : Location where the generated Oculus tags will be stored, default: ./\n"
		 << "    -s | --size TAG_SIZE     : Size of the tags in pixels, default: 400, range: [32, infinity)\n"
		 << "    -b | --border BORDER     : Extra border that is added around the tag. This is measured in multiples of modules, default: 0\n"
		 << "\n"
		 << "If no tag IDs are specified, all possible tags will be generated"
		 << "\n"
		 << "Example:\n"
		 << "    " << applicationName << " -s 1000 -b 1 -o ~/tmp\n"
		 << "\n";
}

bool parseArguments(const int argc, char** argv, std::string& directory, uint32_t& tagSize, uint32_t& extraBorder, std::vector<uint16_t>& tagIDs)
{
	if (argc < 2)
	{
		displayUsage(std::string(argv[0]));
		return false;
	}

	directory = "./";
	tagSize = 400u;
	extraBorder = 0u;
	tagIDs.clear();

	for (uint32_t i = 1u; i < argc; ++i)
	{
		const std::string parameter = std::string(argv[i]);
		ocean_assert(parameter.empty() == false);

		if (parameter == std::string("-h") || parameter == std::string("--help"))
		{
			displayUsage(std::string(argv[0]));
			return false;
		}
		else if (parameter == std::string("-o") || parameter == std::string("--output"))
		{
			if (i + 1u >= argc)
			{
				Log::error() << "DIRECTORY missing in \'" << parameter << " DIRECTORY\'";
				return false;
			}

			directory = std::string(argv[i + 1u]);

			if (directory.empty())
			{
				Log::error() << "DIRECTORY in \'" << parameter << " DIRECTORY\' must not be empty";
				return false;
			}

			if (directory.back() != '/')
			{
				directory += "/";
			}

			i += 1u;
		}
		else if (parameter == std::string("-s") || parameter == std::string("--size"))
		{
			if (i + 1u >= argc)
			{
				Log::error() << "TAG_SIZE missing in \'" << parameter << " TAG_SIZE\'";
				return false;
			}

			int32_t tagSizeI = 0;
			String::isInteger32(std::string(argv[i + 1u]), &tagSizeI);

			if (tagSizeI < 32)
			{
				Log::error() << "TAG_SIZE in \'" << parameter << " TAG_SIZE\' must be larger than 32";
				return false;
			}

			tagSize = uint32_t(tagSizeI);

			i += 1u;
		}
		else if (parameter == std::string("-b") || parameter == std::string("--border"))
		{
			if (i + 1u >= argc)
			{
				Log::error() << "BORDER missing in \'" << parameter << " BORDER\'";
				return false;
			}

			int32_t borderI = 0;
			String::isInteger32(std::string(argv[i + 1u]), &borderI);

			if (borderI < 0)
			{
				Log::error() << "BORDER missing in \'" << parameter << " BORDER\' cannot be negative";
				return false;
			}

			extraBorder = uint32_t(borderI);

			i += 1u;
		}
		else if (parameter[0] != '-')
		{
			int32_t tagIDI = 0;
			String::isInteger32(std::string(parameter), &tagIDI);

			if (tagIDI >= 1024)
			{
				Log::error() << "TAG_ID must be smaller than 1024, but it is \'" << tagIDI << "\'";
				return false;
			}

			tagIDs.push_back(uint16_t(tagIDI));
		}
		else
		{
			Log::error() << "Uknown parameter \'" << parameter << "\'";
			return false;
		}
	}

	// If the user did not specify any tag ID, all IDs will be generated.

	if (tagIDs.empty())
	{
		for (uint16_t tagID = uint16_t(0u); tagID < uint16_t(1024u); ++tagID)
		{
			tagIDs.push_back(tagID);
		}
	}

	return true;
}

int main(int argc, char** argv)
{
	// Direct all messages to the standard output
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	std::string directory;
	uint32_t tagSize;
	uint32_t extraBorder;
	std::vector<uint16_t> tagIDs;

	if (parseArguments(argc, argv, directory, tagSize, extraBorder, tagIDs) == false)
	{
		return 1;
	}

	ocean_assert(tagSize > 32u);

	Log::info() << "Directory:   " << directory;
	Log::info() << "Tag size:    " << tagSize;
	Log::info() << "Border:      " << tagSize;
	Log::info() << "No. tag IDs: " << tagIDs.size();

	bool finishedSuccessully = true;

	for (const uint16_t tagID : tagIDs)
	{
		ocean_assert(tagID < 1024u);

		const Tracking::OculusTags::OculusTag::ReflectanceType reflectanceTypes[2] =
		{
			Tracking::OculusTags::OculusTag::RT_REFLECTANCE_NORMAL,
			Tracking::OculusTags::OculusTag::RT_REFLECTANCE_INVERTED
		};

		const std::string filenamePrefices[2] =
		{
			std::string("n_"),
			std::string("i_"),
		};

		for (uint32_t rI = 0u; rI < 2u; ++rI)
		{
			const Tracking::OculusTags::OculusTag::ReflectanceType reflectanceType = reflectanceTypes[rI];
			ocean_assert(reflectanceType != Tracking::OculusTags::OculusTag::RT_REFLECTANCE_UNDEFINED);

			const Frame tagFrame = Tracking::OculusTags::Utilities::generateTagImage(tagID, reflectanceType, tagSize, extraBorder);

			std::ostringstream oss;
			oss << directory << filenamePrefices[rI] << std::setw(4) << std::setfill('0') << tagID << ".png";
			const std::string outputFilename = oss.str();

			const bool savedFile = Media::OpenImageLibraries::Image::writeImage(tagFrame, outputFilename);

			if (savedFile == false)
			{
				Log::error() << "Failed to save to file \'" << outputFilename << "\'!!";
			}
			else
			{
				Log::info() << "Saved file \'" << outputFilename << "\' ...";
			}

			finishedSuccessully = savedFile && finishedSuccessully;
		}
	}

	return finishedSuccessully ? 0 : 1;
}
