/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/obj/Plugin.h"
#include "ocean/scenedescription/sdl/obj/OBJLibrary.h"

bool pluginLoad()
{
	Ocean::SceneDescription::SDL::OBJ::OBJLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::SceneDescription::SDL::OBJ::OBJLibrary::unregisterLibrary();
}
