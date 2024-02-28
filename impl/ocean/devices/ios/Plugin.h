// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_IOS_PLUGIN_H
#define META_OCEAN_DEVICES_IOS_PLUGIN_H

#include "ocean/devices/ios/IOS.h"

#if defined(OCEAN_RUNTIME_SHARED)

/**
 * Tries to load the plugin and initializes all internal 3rd party libraries.
 * Make sure that the plugin will be loaded only once!
 * @return True, if succeeded
 * @see pluginUnload().
 * @ingroup devicesios
 */
extern "C" bool OCEAN_DEVICES_IOS_EXPORT pluginLoad();

/**
 * Tries to unload the plugin and all internal resources of 3rd party libraries.
 * Make sure that all resources of this plugin has been released before!
 * @return True, if succeeded
 * @see pluginLoad().
 * @ingroup devicesios
 */
extern "C" bool OCEAN_DEVICES_IOS_EXPORT pluginUnload();

#endif // defined(OCEAN_RUNTIME_SHARED)

#endif // META_OCEAN_DEVICES_IOS_PLUGIN_H
