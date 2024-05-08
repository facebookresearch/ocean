/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_EMPTY_PLUGIN_H
#define META_OCEAN_INTERACTION_EMPTY_PLUGIN_H

#include "ocean/interaction/empty/Empty.h"

/**
 * Tries to load the plugin and initializes all internal 3rd party libraries.
 * Make sure that the plugin will be loaded only once!
 * @return True, if succeeded
 * @ingroup interactionepy
 */
extern "C" OCEAN_INTERACTION_EPY_EXPORT bool pluginLoad();

/**
 * Tries to unload the plugin and all internal resources of 3rd party libraries.
 * Make sure that all resources of this plugin has been released before!
 * @return True, if succeeded
 * @ingroup interactionepy
 */
extern "C" OCEAN_INTERACTION_EPY_EXPORT bool pluginUnload();

/**
 * Returns informations about the used 3rd party libraries.
 * @return Name and version of the 3rd party libraries
 * @ingroup interactionepy
 */
extern "C" OCEAN_INTERACTION_EPY_EXPORT const char* pluginVersion();

#endif // META_OCEAN_INTERACTION_EMPTY_PLUGIN_H
