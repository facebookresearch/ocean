// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
