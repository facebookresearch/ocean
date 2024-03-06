// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/VRS.h"

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
	Media::VRS::registerVRSLibrary();

	return 0;
}
