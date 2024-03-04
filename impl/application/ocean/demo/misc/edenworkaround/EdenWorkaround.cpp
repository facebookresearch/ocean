// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Messenger.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

using namespace Ocean;

/**
 * Returns whether a given file could be opened.
 * @param file The file to check
 * @return True, if succeeded
 */
bool canFileBeOpened(const IO::File& file);

/**
 * Overwrites a given file with some information.
 * @param file The file to overwrite
 * @return True, if succeeded
 */
bool overwriteFile(const IO::File& file);

/**
 * Treats a given file.
 * First, the file will be overwritten with some information.
 * Afterwards, the file will be reverted with HG
 */
bool treatFile(const IO::File& file, const bool debugOutput);

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int argc, wchar_t* argv[])
#elif defined(__APPLE__)
	// main function on OSX platforms
	int main(int argc, char* argv[])
#else
	#error Missing implementation.
#endif
{
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	CommandArguments commandArguments;
	commandArguments.registerNamelessParameters("Individual files can be provided as parameters to treat only the given files");
	commandArguments.registerParameter("directory", "d", "The directory which will be handled recursively");
	commandArguments.registerParameter("debugOutput", "do", "If defined, debug output will be shown");
	commandArguments.registerParameter("listFilesOnly", "lfo", "If defined, impacted files will be listed but not treated");
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(argv, size_t(argc));

	if (commandArguments.hasValue("help", nullptr, false))
	{
		Log::info() << "Eden workaround tool";
		Log::info().newLine();

		Log::info() << commandArguments.makeSummary();
		return 0;
	}

	const bool debugOutput = commandArguments.hasValue("debugOutput");

	const bool listFilesOnly = commandArguments.hasValue("listFilesOnly");

	Value directoryValue;
	if (commandArguments.hasValue("directory", &directoryValue, true) && directoryValue.isString())
	{
		const IO::Directory directory(directoryValue.stringValue());

		if (!directory.exists())
		{
			Log::error() << "The provided directory '" << directory() << "' does not exist";
			return 1;
		}

		HighPerformanceTimer timer;

		const IO::Files files = directory.findFiles("*", true);

		Log::info() << "Found: " << files.size() << " files in " << directory();

		IO::Files filesImpacted;
		IO::Files filesHandled;
		IO::Files filesFailed;

		for (const IO::File& file : files)
		{
			if (!canFileBeOpened(file))
			{
				if (listFilesOnly)
				{
					filesImpacted.emplace_back(file);
				}
				else
				{
					if (treatFile(file, debugOutput))
					{
						filesHandled.emplace_back(file);
					}
					else
					{
						filesFailed.emplace_back(file);
					}
				}
			}
		}

		if (filesImpacted.empty() && filesFailed.empty() && filesHandled.empty())
		{
			Log::info() << "No impacted file";
		}
		else
		{
			if (!filesImpacted.empty())
			{
				Log::info().newLine();
				Log::info() << "The following " << filesImpacted.size() << " files are impacted:";

				for (const IO::File& file : filesImpacted)
				{
					Log::info() << file();
				}
			}

			if (!filesFailed.empty())
			{
				Log::info().newLine();
				Log::info() << "The following " << filesFailed.size() << " files could not be processed:";

				for (const IO::File& file : filesFailed)
				{
					Log::info() << file();
				}
			}

			if (!filesHandled.empty())
			{
				Log::info().newLine();
				Log::info() << "The following " << filesHandled.size() << " files could be handled successfully:";

				for (const IO::File& file : filesHandled)
				{
					Log::info() << file.name();
				}
			}
		}

		Log::info().newLine();
		Log::info() << "Processed in " << timer.seconds() << " seconds";
	}
	else
	{
		// we treat individual files

		for (const std::string& filename : commandArguments.namelessValues())
		{
			const IO::File file(filename);

			if (file.exists())
			{
				if (!canFileBeOpened(file))
				{
					if (listFilesOnly)
					{
						Log::info() << "Impacted file: '" << file() << "'";
					}
					else
					{
						if (treatFile(file, debugOutput))
						{
							Log::info() << "Handled '" << file.name() << "' successfully";
						}
						else
						{
							Log::info() << "Failed to handle file '" << file.name() << "'";
						}
					}
				}
				else
				{
					Log::info() << "Skipped '" << file.name() << "'";
				}
			}
			else
			{
				Log::info() << "The input '" << filename << "' is not a file";
			}
		}
	}

	Log::info() << "Press a key to exit";
	getchar();

	return 0;
}

bool canFileBeOpened(const IO::File& file)
{
	std::ifstream stream(file().c_str(), std::ios::binary);

	return stream.is_open();
}

bool overwriteFile(const IO::File& file)
{
	std::ofstream stream(file().c_str(), std::ios::binary);

	if (!stream.is_open())
	{
		return false;
	}

	stream << "Overwritten in EdenWorkaround tool at " << DateTime::localString() << std::endl;

	return stream.good();
}

bool treatFile(const IO::File& file, const bool debugOutput)
{
	// first, we try to overwrite the file with any information

	if (overwriteFile(file))
	{
		const IO::Directory directory(file);

		const std::string hgRevertCommand = "cd \"" + directory() + "\" && hg revert --rev .^ \"" + file() + "\"";

		if (debugOutput)
		{
			Log::info() << "File overwritten, now trying to revert with command '" << hgRevertCommand << "'";
		}

		// now, we revert the overwritten file to the latest HG state

		const int result = system(hgRevertCommand.c_str());

		if (result == 0)
		{
			if (debugOutput)
			{
				Log::info() << "HG revert succeeded";
			}

			if (canFileBeOpened(file))
			{
				return true;
			}
			else
			{
				Log::error() << "Although HG revert succeeded, the file '" << file.name() << "' is still not accessible";
			}
		}
		else
		{
			Log::error() << "Failed to revert file " << file() << ", error: " << GetLastError();
		}
	}

	return false;
}
