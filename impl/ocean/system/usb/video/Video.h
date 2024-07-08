/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_VIDEO_VIDEO_H
#define META_OCEAN_SYSTEM_USB_VIDEO_VIDEO_H

#include "ocean/system/System.h"

namespace Ocean
{

namespace System
{

namespace USB
{

namespace Video
{

/**
 * @ingroup systemusb
 * @defgroup systemusbvideo Ocean System USB Video Library
 * @{
 * The Ocean System USB Video Library provides access to Video devices via USB.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::System::USB::Video Namespace of the System library USB Video.<p>
 * The Namespace Ocean::System::USB::Video is used in the entire Ocean System USB Video Library.
 */

// Defines OCEAN_SYSTEM_USB_VIDEO_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_SYSTEM_USB_VIDEO_EXPORT
		#define OCEAN_SYSTEM_USB_VIDEO_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_SYSTEM_USB_VIDEO_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_SYSTEM_USB_VIDEO_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_SYSTEM_USB_VIDEO_VIDEO_H
