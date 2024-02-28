// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_RENDERING_GLES_APPLE_PLUGIN_H
#define META_OCEAN_RENDERING_GLES_APPLE_PLUGIN_H

#include "ocean/rendering/glescenegraph/apple/Apple.h"

/**
 * Tries to load the plugin and initializes all internal 3rd party libraries.
 * Make sure that the plugin will be loaded only once!
 * @return True, if succeeded
 * @see pluginUnload().
 * @ingroup renderinggles
 */
extern "C" bool OCEAN_RENDERING_GLES_EXPORT pluginLoad();

/**
 * Tries to unload the plugin and all internal resources of 3rd party libraries.
 * Make sure that all resources of this plugin has been released before!
 * @return True, if succeeded
 * @see pluginLoad().
 * @ingroup renderinggles
 */
extern "C" bool OCEAN_RENDERING_GLES_EXPORT pluginUnload();

#endif // META_OCEAN_RENDERING_GLES_APPLE_PLUGIN_H
