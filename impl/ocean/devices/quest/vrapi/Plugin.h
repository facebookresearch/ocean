// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_QUEST_VRAPI_PLUGIN_H
#define META_OCEAN_DEVICES_QUEST_VRAPI_PLUGIN_H

#include "ocean/devices/quest/vrapi/VrApi.h"

#if defined(OCEAN_RUNTIME_SHARED)

/**
 * Tries to load the plugin and initializes all internal 3rd party libraries.
 * Make sure that the plugin will be loaded only once!
 * @return True, if succeeded
 * @see pluginUnload().
 * @ingroup devicesquestvrapi
 */
extern "C" OCEAN_DEVICES_QUEST_VRAPI_EXPORT bool pluginLoad();

/**
 * Tries to unload the plugin and all internal resources of 3rd party libraries.
 * Make sure that all resources of this plugin has been released before!
 * @return True, if succeeded
 * @see pluginLoad().
 * @ingroup devicesquestvrapi
 */
extern "C" OCEAN_DEVICES_QUEST_VRAPI_EXPORT bool pluginUnload();

/**
 * Returns informations about the used 3rd party libraries.
 * @return Name and version of the 3rd party libraries
 * @ingroup devicesquestvrapi
 */
extern "C" OCEAN_DEVICES_QUEST_VRAPI_EXPORT const char* pluginVersion();

#endif // defined(OCEAN_RUNTIME_SHARED)

#endif // META_OCEAN_DEVICES_QUEST_VRAPI_PLUGIN_H
