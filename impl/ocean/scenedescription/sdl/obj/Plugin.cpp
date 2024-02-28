// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
