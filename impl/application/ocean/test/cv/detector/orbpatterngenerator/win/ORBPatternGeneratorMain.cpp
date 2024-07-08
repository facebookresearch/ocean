/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/cv/detector/orbpatterngenerator/win/ORBPatternGeneratorMain.h"
#include "application/ocean/test/cv/detector/orbpatterngenerator/win/ORBPatternGenerator.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/media/Utilities.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

#include "ocean/system/Process.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/wic/WIC.h"
#endif

using namespace Ocean;

int wmain(int argc, wchar_t* argv[])
{

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::registerWICLibrary();
#else
	const IO::Directory pluginPath(Platform::Win::System::processDirectory() + IO::Directory("plugins"));
	if (pluginPath.exists())
	{
		PluginManager::get().collectPlugins(pluginPath());
	}

	if (PluginManager::get().unloadedPlugins().empty())
	{
		const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	}

	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	if (argc <= 1)
	{
		std::cout << "No image path specified" << std::endl;
	}
	else
	{
		std::string path;

		path = String::toAString(argv[1]);
		size_t position = path.find_last_of("/\\");

		if (position == path.length() - 1)
		{
			path.erase(position, path.length());
		}

		try
		{
			Timestamp startTime, endTime, timeGenerateTests, timeSearchBestTests;
			Worker worker;

			std::cout << "Load images..." << std::endl << std::endl;

			IO::Directory dir(path);
			if (!dir.exists())
			{
				std::cout << "Image directory not exists" << std::endl;
			}
			else
			{
				IO::Files files = dir.findFiles("*");

				Frames frames;
				frames.reserve(files.size());

				for (unsigned int i = 0u; i < files.size(); i++)
				{
					Frame tmpFrame = Media::Utilities::loadImage(path + "/" + files[i].name());

					if (tmpFrame.isValid())
					{
						frames.push_back(tmpFrame);
					}
				}

				if (frames.size() == 0)
				{
					std::cout << "No images could be load!" << std::endl << "Cannot calculate patterns.";
				}
				else
				{
					ORBPatternGenerator tests;

					std::cout << "Generating tests for the ORB descriptor:" << std::endl << std::endl;

					startTime.toNow();
					tests.generateTests(frames);
					endTime.toNow();
					timeGenerateTests = endTime - startTime;

					std::cout << std::endl << std::endl << "Searching best tests..." << std::endl;
					startTime.toNow();
					tests.searchBestTests(&worker);
					endTime.toNow();
					timeSearchBestTests = endTime - startTime;

					std::cout << std::endl << "creating txt" << std::endl;

					const std::vector<ORBPatternGenerator::PatternTest> result = tests.getResult();

					std::ofstream txtFile;
					txtFile.open (path + "/" + "generatedBinaryTests.txt");
					for(unsigned int i = 0; i < result.size(); i++)
					{
						txtFile << result[i].x1() << ", " << result[i].y1() << ", " << result[i].x2() << ", " << result[i].y2() << ",\n";
					}

					txtFile.close();

					std::cout << std::endl << "done" << std::endl << std::endl;

					unsigned int timestamp = (unsigned int)(double(timeGenerateTests) + 0.5);
					unsigned int day = timestamp / 86400;
					unsigned int hour = (timestamp - day * 86400) / 3600;
					unsigned int min = (timestamp - day * 86400 - hour * 3600) / 60;
					unsigned int sec = timestamp - day * 86400 - hour * 3600 - min * 60;
					std::cout << "time for calculating tests: " << day << "d " << hour << "h " << min << "min " << sec << "s   " << std::endl << std::endl;

					timestamp = (unsigned int)(double(timeSearchBestTests) + 0.5);
					day = timestamp / 86400;
					hour = (timestamp - day * 86400) / 3600;
					min = (timestamp - day * 86400 - hour * 3600) / 60;
					sec = timestamp - day * 86400 - hour * 3600 - min * 60;
					std::cout << "time for searching best tests: " << day << "d " << hour << "h " << min << "min " << sec << "s   " << std::endl << std::endl;
				}
			}
		}
		catch(...)
		{
			ocean_assert(false && "Unhandled exception!");
		}
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
#else
	PluginManager::get().release();
#endif

	Log::info() << "Press a key to exit.";
	getchar();

	return 0;
}
