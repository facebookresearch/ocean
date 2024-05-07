// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "application/ocean/test/io/testio/android/TestIO.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/String.h"

#include "ocean/io/IO.h"

#include "ocean/platform/android/Utilities.h"

#include "ocean/test/testio/TestIO.h"

#include "ocean/system/Process.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace Ocean;

jstring Java_com_facebook_ocean_app_test_io_testio_android_TestIOActivity_invokeTest(JNIEnv* env, jobject javaThis, jstring outputFilename, jdouble testDuration)
{
	std::ostringstream stream;

	const std::string output = Platform::Android::Utilities::toAString(env, outputFilename);

	if (output.empty())
	{
		Messenger::get().setOutputStream(stream);
		Messenger::get().setOutputType(Messenger::OUTPUT_STREAM);
	}
	else
	{
		if (String::toUpper(output) == std::string("STANDARD"))
				Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);
		else
		{
			Ocean::Messenger::get().setFileOutput(output);
			Ocean::Messenger::get().setOutputType(Ocean::Messenger::OUTPUT_FILE);
		}
	}

	System::Process::setPriority(System::Process::PRIORITY_REALTIME);
	Log::info() << "Process priority set to realtime";
	Log::info() << " ";

	try
	{
		Log::info() << "Starting Ocean IO Library test";
		Log::info() << "Platform: " << Build::buildString();
		Log::info() << "Build: " << Build::buildString();
		Log::info() << "Time: " << DateTime::localString();
		Log::info() << " ";

		Log::info() << "Duration for each test: " << testDuration << "s";
		Log::info() << " ";

		const bool allSucceeded = Ocean::Test::TestIO::testIO(testDuration);

		if (allSucceeded)
			Log::info() << "Entire IO library test succeeded!";
		else
			Log::info() << "Entire IO library test FAILED!";
	}
	catch(const Ocean::Exception& exception)
	{
		Log::error() << "Unhandled exception:\n";
		Log::error() << exception.what();
	}

	if (output.empty())
		return env->NewStringUTF(stream.str().c_str());

	std::string outputString("Finished\n\nPlease find the output at the following location:\n");
	outputString += output;

	return env->NewStringUTF(outputString.c_str());
}
