/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_APPLE_H
#define META_OCEAN_PLATFORM_APPLE_APPLE_H

#include "ocean/platform/Platform.h"

#include "ocean/base/ScopedObject.h"

#ifndef OCEAN_PLATFORM_BUILD_APPLE
	#error This library is available on Apple platforms only!
#endif

#include <CoreFoundation/CoreFoundation.h>

#include <CoreGraphics/CoreGraphics.h>

namespace Ocean
{

namespace Platform
{

namespace Apple
{

/**
 * @ingroup platform
 * @defgroup platformapple Ocean Platform Apple Library
 * @{
 * The Ocean Apple Library provides specific functionalities for Apple platforms.
 * The library is available on Apple platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Apple Namespace of the Platform Apple library.<p>
 * The Namespace Ocean::Platform::Apple is used in the entire Ocean Platform Apple Library.
 */

/**
 * Definition of a scoped object holding a CFStringRef object.
 * The wrapped CFStringRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup platformapple
 */
typedef ScopedObjectCompileTime<CFStringRef, CFTypeRef, void, CFRelease> ScopedCFStringRef;

/**
 * Definition of a scoped object holding a CFURLRef object.
 * The wrapped CFURLRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup platformapple
 */
typedef ScopedObjectCompileTime<CFURLRef, CFTypeRef, void, CFRelease> ScopedCFURLRef;

/**
 * Definition of a scoped object holding a CFDataRef object.
 * The wrapped CFDataRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup platformapple
 */
typedef ScopedObjectCompileTime<CFDataRef, CFTypeRef, void, CFRelease> ScopedCFDataRef;

/**
 * Definition of a scoped object holding a CFMutableDataRef object.
 * The wrapped CFMutableDataRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup platformapple
 */
typedef ScopedObjectCompileTime<CFMutableDataRef, CFTypeRef, void, CFRelease> ScopedCFMutableDataRef;

/**
 * Definition of a scoped object holding a CGImageRef object.
 * The wrapped CGImageRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup platformapple
 */
typedef ScopedObjectCompileTimeVoid<CGImageRef, CGImageRelease> ScopedCGImageRef;

/**
 * Definition of a scoped object holding a CGDataProviderRef object.
 * The wrapped CGDataProviderRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup platformapple
 */
typedef ScopedObjectCompileTimeVoid<CGDataProviderRef, CGDataProviderRelease> ScopedCGDataProviderRef;

/**
 * Definition of a scoped object holding a CGColorSpaceRef object.
 * The wrapped CGColorSpaceRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup platformapple
 */
typedef ScopedObjectCompileTimeVoid<CGColorSpaceRef, CGColorSpaceRelease> ScopedCGColorSpaceRef;

/**
 * Definition of a scoped object holding a CFNumberRef object.
 * The wrapped CFNumberRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup platformapple
 */
typedef ScopedObjectCompileTime<CFNumberRef, CFTypeRef, void, CFRelease> ScopedCFNumberRef;

/**
 * Definition of a scoped object holding a CFDictionaryRef object.
 * The wrapped CFDictionaryRef object will be released automatically once the scoped object does not exist anymore.
 * @ingroup platformapple
 */
typedef ScopedObjectCompileTime<CFDictionaryRef, CFTypeRef, void, CFRelease> ScopedCFDictionaryRef;

/**
 * Definition of a scoped object holding a CFType object.
 * The wrapped CFType object will be released automatically once the scoped object does not exist anymore.
 * @ingroup platformapple
 */
typedef ScopedObjectCompileTimeVoid<CFTypeRef, CFRelease> ScopedCFTypeRef;

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_APPLE_H
