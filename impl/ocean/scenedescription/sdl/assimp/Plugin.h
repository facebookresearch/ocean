/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_PLUGIN_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_PLUGIN_H

#include "ocean/scenedescription/sdl/assimp/Assimp.h"

extern "C" bool OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT pluginLoad();

extern "C" bool OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT pluginUnload();

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_PLUGIN_H
