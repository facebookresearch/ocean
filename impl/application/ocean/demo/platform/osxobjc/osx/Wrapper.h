/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_OSXOBJC_OSX_WRAPPER_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_OSXOBJC_OSX_WRAPPER_H

#include "application/ocean/demo/platform/ApplicationDemoPlatform.h"

/**
 * @ingroup applicationdemoplatform
 * @defgroup applicationdemoplatformosxobjc OSX OBJC (OSX)
 * @{
 * The demo application demonstrates the implementation of a simple OSX window application based on Objective-C and Objective-C++.<br>
 * The application uses some functions from Ocean's Base library.<br>
 * Some functions are invoked directly, some functions are invoked via a Wrapper to show the difference.<br>
 * This application is platform dependent and is implemented for OSX platforms.
 * @}
 */

/**
 * Definition of a struct providing a two values.
 * @ingroup applicationdemoplatformosxobjc
 */
struct Struct
{
	/// Integer value.
	int valueInteger;

	/// Float value.
	float valueFloat;
};

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Allocates a buffer large enough to cover a given string and copies the string's content into the buffer.
 * The returning buffer of the string must not be released by the caller.
 * @param value The string value to be copied into the buffer
 * @return The pointer to the buffer
 * @ingroup applicationdemoplatformosxobjc
 */
const char* string2buffer(const std::string& value);

/**
 * Returns the build string of the ocean framework.
 * The returning buffer of the string must not be released by the caller.
 * @return The pointer to the build string
 * @ingroup applicationdemoplatformosxobjc
 */
const char* oceanBaseBuildStringStatic();

/**
 * Returns the build date or build time.
 * The resulting buffer of the string must be released by the caller.
 * @param selection The selection defining the resulting value, either 'date' or 'time'
 * @return The resulting buffer which must be released
 * @see releaseString().
 * @ingroup applicationdemoplatformosxobjc
 */
const char* oceanBaseDateTimeString(const char* selection);

/**
 * Returns the current timestamp as unix timestamp.
 * @return The current unix timestamp
 * @ingroup applicationdemoplatformosxobjc
 */
double oceanBaseTimestamp();

/**
 * Returns a struct object.
 * @return The struct object
 * @ingroup applicationdemoplatformosxobjc
 */
struct Struct oceanBaseStructObject();

/**
 * Returns the pointer to a struct object.
 * The returning buffer must not be released by the caller.
 * @return The pointer to the struct
 * @ingroup applicationdemoplatformosxobjc
 */
struct Struct* oceanBaseStructStatic();

/**
 * Releases a buffer.
 * @param value The buffer to release
 * @ingroup applicationdemoplatformosxobjc
 */
void oceanReleaseString(const char* value);

#ifdef __cplusplus
}
#endif

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_OSXOBJC_OSX_WRAPPER_H
