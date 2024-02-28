// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
#include "ocean/media/openimagelibraries/OILLibrary.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

std::string nameOpenImageLibrariesLibrary()
{
	return std::string("OpenImageLibraries");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerOpenImageLibrariesLibrary()
{
	OILLibrary::registerLibrary();
}

bool unregisterOpenImageLibrariesLibrary()
{
	return OILLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
