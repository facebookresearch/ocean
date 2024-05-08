/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/assimp/Assimp.h"
#include "ocean/scenedescription/sdl/assimp/AssimpLibrary.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace Assimp
{

std::string nameAssimpLibrary()
{
	return std::string("SDL Assimp");
}

#if defined(OCEAN_RUNTIME_STATIC)

void registerAssimpLibrary()
{
	AssimpLibrary::registerLibrary();
}

bool unregisterAssimpLibrary()
{
	return AssimpLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}
