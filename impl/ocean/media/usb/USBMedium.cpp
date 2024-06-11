/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/usb/USBMedium.h"

namespace Ocean
{

namespace Media
{

namespace USB
{

USBMedium::USBMedium(const std::string& url) :
	Medium(url)
{
	libraryName_ = nameUSBLibrary();
}

}

}

}
