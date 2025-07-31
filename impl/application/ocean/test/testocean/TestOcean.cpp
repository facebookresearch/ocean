/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/testocean/TestOcean.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/String.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Random.h"

#include "ocean/platform/System.h"

#include "ocean/system/Memory.h"
#include "ocean/system/OperatingSystem.h"
#include "ocean/system/Process.h"

#include "ocean/test/testbase/TestBase.h"

#include "ocean/test/testcv/TestCV.h"

#include "ocean/test/testcv/testadvanced/TestCVAdvanced.h"

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/test/testcv/testsegmentation/TestCVSegmentation.h"

//#include "ocean/test/testcv/testsynthesis/TestCVSynthesis.h"

#include "ocean/test/testgeometry/TestGeometry.h"

#include "ocean/test/testmath/TestMath.h"

//#include "ocean/test/testrendering/TestRendering.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(__APPLE__)
		#include "ocean/media/imageio/ImageIO.h"
	#elif defined(_WINDOWS)
		#include "ocean/media/wic/WIC.h"
	#endif
#endif

using namespace Ocean;

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int argc, wchar_t* argv[])
#elif defined(__APPLE__) || defined(__linux__)
	// main function on OSX and Linux platforms
	int main(int argc, char* argv[])
#else
	#error Missing implementation.
#endif
{
#ifdef OCEAN_COMPILER_MSC
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
	std::string mediaFilename = frameworkPath + std::string("/res/application/ocean/test/cv/testcv/testdetector/tropical-island-with-toucans_800x800.jpg");

#ifdef OCEAN_DEBUG
	double testDuration = 0.1;
#else
	double testDuration = 2;
#endif // OCEAN_DEBUG

	std::string outputFilename, libraryList;

	if (argc >= 2)
	{
		// special case that the first argument is a '?':
		if (String::toWString(argv[1]) == std::wstring(L"?"))
		{
			std::cout << "Ocean Framework test:" << std::endl << std::endl;
			std::cout << "Optional arguments: " << std::endl;
			std::cout << "Parameter 1: [test image filename e.g., \"image.png\" or default \"\"]" << std::endl;
			std::cout << "Parameter 2: [log output file e.g., \"log.txt\" or default \"\"]" << std::endl;
			std::cout << "Parameter 3: [libraries to test e.g., \"cv, geometry\" or default \"\"]" << std::endl;
			std::cout << "Parameter 4: [duration for each test in seconds e.g., \"2.5\" or default \"\"]" << std::endl << std::endl;
			std::cout << "Examples:" << std::endl;
			std::cout << "image.png \"\" \"\" 0.5" << std::endl;
			std::cout << "(test image image.png, output to the console, all libraries will be tested, each test takes approx. 0.5 seconds)" << std::endl << std::endl;
			std::cout << "\"\" output.log \"base, cv, geometry\"" << std::endl;
			std::cout << "(default test image, output to output.log file, three specific libraries will be tested, with default test duration)" << std::endl << std::endl;
			std::cout << "tropical-island-with-toucans_800x800.jpg ocean_framework_test.log" << std::endl;
			std::cout << "(standardized test)" << std::endl << std::endl;

			return 0;
		}

		const std::string file(String::trim(String::toAString(argv[1]), '\"'));

		if (!file.empty())
			mediaFilename = file;
	}

	if (argc >= 3)
	{
		const std::string file(String::trim(String::toAString(argv[2])));

		if (!file.empty())
			outputFilename = file;
	}

	if (argc >= 4)
		libraryList = String::trim(String::toAString(argv[3]));

	if (argc >= 5)
	{
		const std::string duration(String::trim(String::toAString(argv[4])));

		if (!duration.empty())
		{
			const double value = atof(duration.c_str());
			if (value > 0)
				testDuration = value;
		}
	}

	if (outputFilename.empty() || outputFilename == "STANDARD")
		Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);
	else
	{
		Messenger::get().setOutputType(Messenger::OUTPUT_FILE);
		Messenger::get().setFileOutput(outputFilename);
	}

	const Timestamp startTimestamp(true);

	Log::info() << "Ocean Framework test:";
	Log::info() << " ";
	Log::info() << "Platform: " << Build::buildString();
	Log::info() << " ";
	Log::info() << "Start: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	Log::info() << "Library list: " << (libraryList.empty() ? "All libraries" : libraryList);
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
	Log::info() << "Test with: " << String::toAString(sizeof(Scalar)) << "byte floats";
	Log::info() << " ";

	const unsigned long long startVirtualMemory = System::Memory::processVirtualMemory();

	Log::info() << "Currently used memory: " << String::insertCharacter(String::toAString(startVirtualMemory >> 10), ',', 3, false) << "KB";
	Log::info() << " ";

	unsigned int startedTests = 0u;
	unsigned int succeededTests = 0u;

	std::vector<std::string> tests(Utilities::separateValues(String::toLower(libraryList), ',', true, true));
	const std::set<std::string> testSet(tests.begin(), tests.end());

	try
	{
		if (testSet.empty() || testSet.find("base") != testSet.end())
		{
			startedTests++;

			Log::info() << "\n\n\n\n\n\n";
			if (Test::TestBase::testBase(testDuration, worker))
			{
				succeededTests++;
			}
		}

		if (testSet.empty() || testSet.find("math") != testSet.end())
		{
			startedTests++;

			Log::info() << "\n\n\n\n\n\n";
			if (Test::TestMath::testMath(testDuration, worker))
			{
				succeededTests++;
			}
		}

		if (testSet.empty() || testSet.find("cv") != testSet.end())
		{
			startedTests++;

			Log::info() << "\n\n\n\n\n\n";
			if (Test::TestCV::testCV(testDuration, worker))
			{
				succeededTests++;
			}
		}

		if (testSet.empty() || testSet.find("geometry") != testSet.end())
		{
			startedTests++;

			Log::info() << "\n\n\n\n\n\n";
			if (Test::TestGeometry::testGeometry(testDuration, worker))
			{
				succeededTests++;
			}
		}

		if (testSet.empty() || testSet.find("cvadvanced") != testSet.end())
		{
			startedTests++;

			Log::info() << "\n\n\n\n\n\n";
			if (Test::TestCV::TestAdvanced::testCVAdvanced(testDuration, worker))
			{
				succeededTests++;
			}
		}

		if (testSet.empty() || testSet.find("cvdetector") != testSet.end())
		{
			startedTests++;

			Log::info() << "\n\n\n\n\n\n";
			if (Test::TestCV::TestDetector::testCVDetector(testDuration, worker, mediaFilename))
			{
				succeededTests++;
			}
		}

		if (testSet.empty() || testSet.find("cvsegmentation") != testSet.end())
		{
			startedTests++;

			Log::info() << "\n\n\n\n\n\n";
			if (Test::TestCV::TestSegmentation::testCVSegmentation(testDuration, worker))
			{
				succeededTests++;
			}
		}
	}
	catch (...)
	{
		ocean_assert(false && "Unhandled exception!");
		Log::info() << "Unhandled exception!";
	}

	ocean_assert(succeededTests <= startedTests);

	const unsigned long long stopVirtualMemory = System::Memory::processVirtualMemory();

	Log::info() << " ";
	Log::info() << "Currently used memory: " << String::insertCharacter(String::toAString(stopVirtualMemory >> 10), ',', 3, false) << "KB (+ " << String::insertCharacter(String::toAString((stopVirtualMemory - startVirtualMemory) >> 10), ',', 3, false) << "KB)";
	Log::info() << " ";

	const Timestamp endTimestamp(true);

	Log::info() << "Time elapsed: " << DateTime::seconds2string(double(endTimestamp - startTimestamp), true);
	Log::info() << "End: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	if (succeededTests == startedTests)
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Ocean Framework test succeeded.";
	}
	else
	{
		Log::info() << (testSet.empty() ? "Entire" : "Partial") << " Ocean Framework test FAILED!";
	}

	Log::info() << " ";

	if (Messenger::get().outputType() == Messenger::OUTPUT_STANDARD)
	{
		Log::info() << "Press a key to exit.";
		getchar();
	}

	if (startedTests == succeededTests)
	{
		return 0;
	}

	return 1;
}
