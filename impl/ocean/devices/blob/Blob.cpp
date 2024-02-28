// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/blob/Blob.h"
#include "ocean/devices/blob/BlobFactory.h"

namespace Ocean
{

namespace Devices
{

namespace Blob
{

std::string nameBlobLibrary()
{
	return std::string("Blob Tracker library");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerBlobLibrary()
{
	BlobFactory::registerFactory();
}

bool unregisterBlobLibrary()
{
	return BlobFactory::unregisterFactory();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
