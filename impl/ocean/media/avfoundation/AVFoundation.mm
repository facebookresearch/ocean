// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/avfoundation/AVFoundation.h"
#include "ocean/media/avfoundation/AVFLibrary.h"

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

std::string nameAVFLibrary()
{
	return std::string("AVFoundation");
}

#if defined(OCEAN_RUNTIME_STATIC)

void registerAVFLibrary()
{
	AVFLibrary::registerLibrary();
}

bool unregisterAVFLibrary()
{
	return AVFLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
