/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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

#if defined(OCEAN_RUNTIME_STATIC)

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
