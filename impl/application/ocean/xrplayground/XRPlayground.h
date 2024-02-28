// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_XR_PLAYGROUND_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_XR_PLAYGROUND_H

#include "ocean/base/Base.h"

#include <cstdlib>
#include <iostream>

namespace Ocean
{

namespace XRPlayground
{

/**
 * @ingroup application
 * @defgroup xrplayground The Ocean powered XRPlayground app framework.
 * @{
 * The XRPlayground app is a lightweight app for rapid prototyping XR-related features.
 * @}
 */

/**
 * @namespace Ocean::XRPlayground Namespace of the Ocean powered XRPlayground app framework.
 */

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#ifdef OCEAN_PLATFORM_BUILD_ANDROID
		#define XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID
	#endif

	#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS
		#define XR_PLAYGROUND_PLATFORM_BUILD_PHONE_IOS
	#endif

	#ifdef OCEAN_PLATFORM_BUILD_MOBILE
		#define XR_PLAYGROUND_PLATFORM_BUILD_PHONE
	#endif
#endif

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_XR_PLAYGROUND_H
