// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_BLOB_PLUGIN_H
#define META_OCEAN_DEVICES_BLOB_PLUGIN_H

#include "ocean/devices/blob/Blob.h"

#if defined(OCEAN_RUNTIME_SHARED)

/**
 * Tries to load the plugin and initializes all internal 3rd party libraries.
 * Make sure that the plugin will be loaded only once!
 * @return True, if succeeded
 * @see pluginUnload().
 * @ingroup devicesblob
 */
extern "C" bool OCEAN_DEVICES_BLOB_EXPORT pluginLoad();

/**
 * Tries to unload the plugin and all internal resources of 3rd party libraries.
 * Make sure that all resources of this plugin has been released before!
 * @return True, if succeeded
 * @see pluginLoad().
 * @ingroup devicesblob
 */
extern "C" bool OCEAN_DEVICES_BLOB_EXPORT pluginUnload();

#endif // defined(OCEAN_RUNTIME_SHARED)

#endif // META_OCEAN_DEVICES_BLOB_PLUGIN_H
