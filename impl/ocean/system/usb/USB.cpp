/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/usb/USB.h"

namespace Ocean
{

namespace System
{

namespace USB
{

void releaseTransfer(libusb_transfer* libusbTransfer)
{
	ocean_assert(libusbTransfer != nullptr);

	libusb_free_transfer(libusbTransfer);
}

}

}

}

