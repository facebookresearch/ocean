/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/test/rendering/testrendering/TestRendering.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Worker.h"

#include "ocean/system/Process.h"

#include "ocean/test/testrendering/TestRendering.h"

#if defined(_WINDOWS)
	#include <windows.h>
#endif // defined(_WINDOWS)

using namespace Ocean;

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int /*argc*/, wchar_t** /*argv*/)
#elif defined(__APPLE__) || defined(__linux__)
	// main function on UNIX platforms
	int main(int /*argc*/, char** /*argv*/)
#else
	#error Missing implementation.
#endif
{
#if defined(_WINDOWS)
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	System::Process::setPriority(System::Process::PRIORITY_REALTIME);
	RandomI::initialize();

#ifdef OCEAN_DEBUG
		const double testDuration = 0.1;
#else
		const double testDuration = 2;
#endif

	Worker worker;

	Log::info() << "Platform: " << Build::buildString();
	Log::info() << "Used worker threads: " << worker.threads();
	Log::info() << "Test with: " << String::toAString(sizeof(Scalar)) << "byte floats";
	Log::info() << " ";
	Log::info() << " ";

	Test::TestRendering::testRendering(testDuration, worker);

	std::cout << "Press a key to exit.";

	getchar();

	return 0;
}
