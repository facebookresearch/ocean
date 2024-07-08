/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_PLUGIN_H
#define META_OCEAN_RENDERING_GI_PLUGIN_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"

/**
 * Tries to load the plugin and initializes all internal 3rd party libraries.
 * Make sure that the plugin will be loaded only once!
 * @return True, if succeeded
 * @see pluginUnload().
 * @ingroup renderinggi
 */
extern "C" bool OCEAN_RENDERING_GI_EXPORT pluginLoad();

/**
 * Tries to unload the plugin and all internal resources of 3rd party libraries.
 * Make sure that all resources of this plugin has been released before!
 * @return True, if succeeded
 * @see pluginLoad().
 * @ingroup renderinggi
 */
extern "C" bool OCEAN_RENDERING_GI_EXPORT pluginUnload();

/**
 * Returns informations about the used 3rd party libraries.
 * @return Name and version of the 3rd party libraries
 * @ingroup renderinggi
 */
extern "C" OCEAN_RENDERING_GI_EXPORT const char* pluginVersion();

#endif // META_OCEAN_RENDERING_GI_PLUGIN_H
