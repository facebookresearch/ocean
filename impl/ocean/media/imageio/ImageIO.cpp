/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
