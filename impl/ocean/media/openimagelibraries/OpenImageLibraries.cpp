/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
