/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/assimp/Plugin.h"
#include "ocean/scenedescription/sdl/assimp/AssimpLibrary.h"

bool pluginLoad()
{
	Ocean::SceneDescription::SDL::Assimp::AssimpLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::SceneDescription::SDL::Assimp::AssimpLibrary::unregisterLibrary();
}
