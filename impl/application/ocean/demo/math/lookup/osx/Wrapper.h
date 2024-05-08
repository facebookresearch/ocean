/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_LOOKUP_OSX_WRAPPER_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_LOOKUP_OSX_WRAPPER_H

/**
 * @ingroup applicationdemomath
 * @defgroup applicationdemomathlookuposx Lookup (OSX)
 * @{
 * The demo application demonstrates the application of the Lookup class of the math library.<br>
 * This application is platform dependent and is implemented for osx platforms.<br>
 * @}
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Returns the build string of the ocean framework.
 * The returning buffer of the string must not be released by the caller.
 * @return The pointer to the build string
 * @ingroup applicationdemomathlookuposx
 */
const char* oceanBaseBuildStringStatic();

/**
 * Initialize the ocean random class.
 * @ingroup applicationdemomathlookuposx
 */
void wrapperRandomInitialize();

/**
 * Allocates a rame buffer for an image with dimension 800x800 and three channels (each with 8 bit color value).
 * The resulting buffer must be released by the caller.
 * @return The pointer to the allocated buffer
 * @see wrapperReleaseFrameData().
 * @ingroup applicationdemomathlookuposx
 */
unsigned char* wrapperAllocFrameData();

/**
 * Release a frame buffer.
 * @param data The buffer to release
 * @see wrapperAllocFrameData().
 * @ingroup applicationdemomathlookuposx
 */
void wrapperReleaseFrameData(const unsigned char* data);

/**
 * Determines the lookup frame and stores the result in the given buffer.
 * @param data The data buffer that receive the result, must be large enough for an image with dimension 800x800 and three color channels (each with 8 bit)
 * @param viewId The view id of the lookup frame, with range [0, 5]
 * @ingroup applicationdemomathlookuposx
 */
void wrapperDetermineLookupFrame(unsigned char* data, const unsigned int viewId);

#ifdef __cplusplus
}
#endif

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MATH_LOOKUP_OSX_WRAPPER_H
