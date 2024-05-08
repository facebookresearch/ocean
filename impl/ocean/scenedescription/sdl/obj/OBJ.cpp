/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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

#if defined(OCEAN_RUNTIME_STATIC)

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
