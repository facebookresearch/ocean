/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/io/testio/TestIO.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/testio/TestIO.h"

#include "ocean/io/Directory.h"

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

#ifdef OCEAN_DEBUG
	constexpr double defaultTestDuration = 0.1;
#else
	constexpr double defaultTestDuration = 2.0;
#endif // OCEAN_DEBUG

	CommandArguments commandArguments;
	commandArguments.registerParameter("output", "o", "The optional output file for the test log, e.g., log.txt");
	commandArguments.registerParameter("functions", "f", "The optional subset of functions to test, e.g., \"compression\"");
	commandArguments.registerParameter("duration", "d", "The test duration for each test in seconds, e.g., 1.0", Value(defaultTestDuration));
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(argv, size_t(argc));

	if (commandArguments.hasValue("help", nullptr, false))
	{
		std::cout << commandArguments.makeSummary() << std::endl;
		return 0;
	}

	double testDuration = defaultTestDuration;
	std::string outputFilename;
	std::string functionList;

	Value durationValue;
	if (commandArguments.hasValue("duration", &durationValue, true) && durationValue.isFloat64(true))
	{
		testDuration = durationValue.float64Value(true);
	}

	Value outputValue;
	if (commandArguments.hasValue("output", &outputValue) && outputValue.isString())
	{
		outputFilename = outputValue.stringValue();
	}

	Value functionsValue;
	if (commandArguments.hasValue("functions", &functionsValue) && functionsValue.isString())
	{
		functionList = functionsValue.stringValue();
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

	Log::info() << "Ocean Framework test for the IO library";
	Log::info() << "Platform: " << Build::buildString();
	Log::info() << " ";
	Log::info() << "Start: " << DateTime::stringDate() << ", " << DateTime::stringTime() << " UTC";
	Log::info() << " ";

	Log::info() << "Function list: " << (functionList.empty() ? "All functions" : functionList);
	Log::info() << " ";

	Log::info() << "Duration for each test: " << testDuration << "s";

	RandomI::initialize();

	Log::info() << "Random generator initialized";
	Log::info() << " ";

	bool resultValue = 1;

	try
	{
		if (Test::TestIO::testIO(testDuration, functionList))
		{
			resultValue = 0;
		}
	}
	catch (...)
	{
		ocean_assert(false && "Unhandled exception!");
		Log::info() << "Unhandled exception!";
	}

	const Timestamp endTimestamp(true);

	Log::info() << " ";
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
