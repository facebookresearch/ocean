/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/base/console/Console.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/Processor.h"

using namespace Ocean;

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int /*argc*/, wchar_t** /*argv*/)
#elif defined(__APPLE__) || defined(__linux__)
	// main function on OSX platforms
	int main(int /*argc*/, char** /*argv*/)
#else
	#error Missing implementation.
#endif
{
#ifdef OCEAN_COMPILER_MSC
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	Log::info() << "Console demo application";
	Log::info() << " ";

	Log::info() << "Platform: " << Build::buildString();
	Log::info() << "Build: " << Build::buildString();
	Log::info() << "Time: " << DateTime::localString();
	Log::info() << " ";

	Log::info() << "While the hardware supports the following SIMD instructions:";
	Log::info() << Processor::translateInstructions(Processor::get().instructions());
	Log::info() << " ";

	Log::info() << "Processor: " << Processor::brand();
	Log::info() << " ";

	Log::info() << "... do something here ...";
	Log::info() << " ";

	Log::info() << "Finished";

	return 0;
}
