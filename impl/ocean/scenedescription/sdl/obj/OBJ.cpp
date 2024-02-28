// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdl/obj/OBJ.h"
#include "ocean/scenedescription/sdl/obj/OBJLibrary.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

std::string nameOBJLibrary()
{
	return std::string("SDL OBJ");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerOBJLibrary()
{
	OBJLibrary::registerLibrary();
}

bool unregisterOBJLibrary()
{
	return OBJLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}
