// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/base/console/Console.h"

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

	std::cout << "Press a key to exit" << std::endl;
	getchar();

	return 0;
}
