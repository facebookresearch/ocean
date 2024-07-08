/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
