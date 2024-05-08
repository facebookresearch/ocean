/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/tracking/testtracking/testoculustags/TestOculusTags.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"

#include "ocean/system/Memory.h"
#include "ocean/system/OperatingSystem.h"
#include "ocean/system/Process.h"

#include "ocean/test/testtracking/testoculustags/TestOculusTags.h"

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

#ifdef OCEAN_DEACTIVATED_MESSENGER
	#warning The messenger is currently deactivated.
#endif

#ifdef OCEAN_DEBUG
	double testDuration = 0.1;
#else
	double testDuration = 2;
#endif

	std::string outputFilename, functionList;

	if (argc >= 2)
	{
		// special case that the first argument is a '?' or "--help"":
		if (String::toWString(argv[1]) == std::wstring(L"?") || String::toWString(argv[1]) == std::wstring(L"--help"))
		{
			std::cout << "Ocean Framework test for the Tracking Oculus Tag library:" << std::endl << std::endl;
			std::cout << "Optional arguments: " << std::endl;
			std::cout << "Parameter 1: [log output file e.g., \"log.txt\" or default \"\"]" << std::endl;
			std::cout << "Parameter 2: [functions to test e.g., \"stresstest\" or default \"\"]" << std::endl;
			std::cout << "Parameter 3: [duration for each test in seconds e.g., \"2.5\" or default \"\"]" << std::endl << std::endl;
			std::cout << "Examples:" << std::endl;
			std::cout << "\"\" \"\" 0.5" << std::endl;
			std::cout << "(output to the console, all functions will be tested, each test takes approx. 0.5 seconds)" << std::endl << std::endl;
			std::cout << "output.log \"stresstest\"" << std::endl;
			std::cout << "(output to output.log file, one specific function will be tested, with default test duration)" << std::endl << std::endl;

			return 0;
		}

		const std::string file(String::trim(String::toAString(argv[1])));

		if (!file.empty())
		{
			outputFilename = file;
		}
	}

	if (argc >= 3)
	{
		functionList = String::trim(String::toAString(argv[2]));
	}

	if (argc >= 4)
	{
		const std::string duration(String::trim(String::toAString(argv[3])));

		if (!duration.empty())
		{
			const double value = atof(duration.c_str());
			if (value > 0)
			{
				testDuration = value;
			}
		}
	}

	if (outputFilename.empty() || outputFilename == "STANDARD")
	{
		Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);
	}
	else
	{
		Messenger::get().setOutputType(Messenger::OUTPUT_FILE);
		Messenger::get().setFileOutput(outputFilename);
	}

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test for the Tracking library:";
	Log::info() << " ";
	Log::info() << "Platform: " << Build::buildString();
	Log::info() << " ";
	Log::info() << "Start: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	Log::info() << "Function list: " << (functionList.empty() ? "All functions" : functionList);
	Log::info() << "Duration for each test: " << String::toAString(testDuration, 1u) << "s";
	Log::info() << " ";

	RandomI::initialize();
	System::Process::setPriority(System::Process::PRIORITY_ABOVE_NORMAL);

	Log::info() << "Random generator initialized";
	Log::info() << "Process priority set to above normal";
	Log::info() << " ";

	Worker worker;

	Log::info() << "Operating System: " << String::toAString(System::OperatingSystem::name());
	Log::info() << "Processor: " << Processor::brand();
	Log::info() << "Used worker threads: " << worker.threads();
	Log::info() << " ";

	const unsigned long long startVirtualMemory = System::Memory::processVirtualMemory();

	Log::info() << "Currently used memory: " << String::insertCharacter(String::toAString(startVirtualMemory >> 10), ',', 3, false) << "KB";
	Log::info() << " ";

	int resultValue = 1;

	try
	{
		if (Test::TestTracking::TestOculusTags::test(testDuration, worker, functionList))
		{
			resultValue = 0;
		}
	}
	catch (...)
	{
		ocean_assert(false && "Unhandled exception!");
		Log::info() << "Unhandled exception!";
	}

	const unsigned long long stopVirtualMemory = System::Memory::processVirtualMemory();

	Log::info() << " ";
	Log::info() << "Currently used memory: " << String::insertCharacter(String::toAString(stopVirtualMemory >> 10), ',', 3, false) << "KB (+ " << String::insertCharacter(String::toAString((stopVirtualMemory - startVirtualMemory) >> 10), ',', 3, false) << "KB)";
	Log::info() << " ";

	const Timestamp endTimestamp(true);

	Log::info() << "Time elapsed: " << DateTime::seconds2string(double(endTimestamp - startTimestamp), true);
	Log::info() << "End: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	if (Messenger::get().outputType() == Messenger::OUTPUT_STANDARD)
	{
		std::cout << "Press a key to exit.";
		getchar();
	}

	return resultValue;
}
