/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_IOS_IOS_H
#define META_OCEAN_PLATFORM_APPLE_IOS_IOS_H

#include "ocean/platform/apple/Apple.h"

#if (!defined(TARGET_IPHONE_SIMULATOR) || TARGET_IPHONE_SIMULATOR != 1) && (!defined(TARGET_OS_IPHONE) || TARGET_OS_IPHONE != 1)
	#error This library is available on Apple iOS platforms only!
#endif

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace IOS
{

/**
 * @ingroup platformapple
 * @defgroup platformappleios Ocean Platform Apple iOS Library
 * @{
 * The Ocean Apple iOSX Library provides specific functionalities for Apple iOS platforms.
 * The library is available on iOS platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Apple::IOS Namespace of the Platform Apple iOS library.<p>
 * The Namespace Ocean::Platform::Apple::IOS is used in the entire Ocean Platform Apple iOS Library.
 */

}

}

}
	
}

#endif // META_OCEAN_PLATFORM_APPLE_IOS_IOS_H
