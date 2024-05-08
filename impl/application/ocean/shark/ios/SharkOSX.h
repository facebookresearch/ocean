/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_SHARK_OSX_SHARK_H
#define FACEBOOK_SHARK_OSX_SHARK_H

#include "application/ocean/shark/Shark.h"

namespace Ocean
{

namespace Shark
{

namespace OSX
{

/**
 * @ingroup shark
 * @defgroup sharkosx Ocean Shark VR/AR viewer for Apple mobile platforms.
 * @{
 * Shark is the VR/AR viewer of the Ocean framework for mobile platforms.<br>
 * This implementation is especially for iOS platforms.<br>
 * It used the entire framework functionality and like e.g. rendering and tracking support.<br>
 * For desktop platforms with higher computational power Ocean provides the Orca viewer.<br>
 * The application is platform dependent and up to this moment available on OSX and IOS platforms only.
 * @}
 */

/**
 * @namespace Ocean::Shark::OSX Namespace of the Ocean Shark VR/AR viewer application for OSX and IOS applications.
 */

#ifndef __APPLE__
	#error This application is available on OSX/IOS platforms only!
#endif

}

}

}

#endif // FACEBOOK_SHARK_OSX_SHARK_H
