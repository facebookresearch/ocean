// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

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
