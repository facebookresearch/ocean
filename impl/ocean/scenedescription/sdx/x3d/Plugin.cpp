// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdx/x3d/Plugin.h"
#include "ocean/scenedescription/sdx/x3d/X3DLibrary.h"

bool pluginLoad()
{
	Ocean::SceneDescription::SDX::X3D::X3DLibrary::registerLibrary();

	return true;
}

bool pluginUnload()
{
	return Ocean::SceneDescription::SDX::X3D::X3DLibrary::unregisterLibrary();
}
