/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_OSXSWIFT_OSX_WRAPPER_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_OSXSWIFT_OSX_WRAPPER_H

/**
 * @ingroup applicationdemoplatform
 * @defgroup applicationdemoplatformosxswiftosx OSX Swift (OSX)
 * @{
 * The demo application demonstrates the implementation of a simple osx window application based on swift.<br>
 * This application is platform dependent and is implemented for osx platforms.<br>
 * @}
 */

/**
 * Definition of a struct providing a two values.
 * @ingroup applicationdemoplatformosxswiftosx
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
 * Returns the build string of the ocean framework.
 * The returning buffer of the string must not be released by the caller.
 * @return The pointer to the build string
 * @ingroup applicationdemoplatformosxswiftosx
 */
const char* oceanBaseBuildStringStatic(void);

/**
 * Returns the build date or build time.
 * The resulting buffer of the string must be released by the caller.
 * @param selection The selection defining the resulting value, either 'date' or 'time'
 * @return The resulting buffer which must be released
 * @see releaseString().
 * @ingroup applicationdemoplatformosxswiftosx
 */
const char* oceanBaseDateTimeString(const char* selection);

/**
 * Returns the current timestamp as unix timestamp.
 * @return The current unix timestamp
 * @ingroup applicationdemoplatformosxswiftosx
 */
double oceanBaseTimestamp(void);

/**
 * Returns a struct object.
 * @return The struct object
 * @ingroup applicationdemoplatformosxswiftosx
 */
struct Struct oceanBaseStructObject(void);

/**
 * Returns the pointer to a struct object.
 * The returning buffer must not be released by the caller.
 * @return The pointer to the struct
 * @ingroup applicationdemoplatformosxswiftosx
 */
struct Struct* oceanBaseStructStatic(void);

/**
 * Releases a buffer.
 * @param value The buffer to release
 * @ingroup applicationdemoplatformosxswiftosx
 */
void oceanReleaseString(const char* value);

#ifdef __cplusplus
}
#endif

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_PLATFORM_OSXSWIFT_OSX_WRAPPER_H
