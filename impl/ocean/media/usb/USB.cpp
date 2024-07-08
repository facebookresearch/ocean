/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/usb/USB.h"
#include "ocean/media/usb/USBLibrary.h"

namespace Ocean
{

namespace Media
{

namespace USB
{

std::string nameUSBLibrary()
{
	return std::string("USB");
}

#ifdef OCEAN_RUNTIME_STATIC

void registerUSBLibrary()
{
	USBLibrary::registerLibrary();
}

bool unregisterUSBLibrary()
{
	return USBLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
