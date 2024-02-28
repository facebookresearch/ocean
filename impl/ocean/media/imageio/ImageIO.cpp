// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/imageio/ImageIO.h"
#include "ocean/media/imageio/IIOLibrary.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

std::string nameImageIOLibrary()
{
	return std::string("ImageIO");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerImageIOLibrary()
{
	IIOLibrary::registerLibrary();
}

bool unregisterImageIOLibrary()
{
	return IIOLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
