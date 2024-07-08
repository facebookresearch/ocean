/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_PLUGIN_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_PLUGIN_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"

/**
 * Tries to load the plugin and initializes all internal 3rd party libraries.
 * Make sure that the plugin will be loaded only once!
 * @return True, if succeeded
 * @see pluginUnload().
 * @ingroup scenedescriptionsdxx3d
 */
extern "C" bool OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT pluginLoad();

/**
 * Tries to unload the plugin and all internal resources of 3rd party libraries.
 * Make sure that all resources of this plugin has been released before!
 * @return True, if succeeded
 * @see pluginLoad().
 * @ingroup scenedescriptionsdxx3d
 */
extern "C" bool OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT pluginUnload();

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_PLUGIN_H
