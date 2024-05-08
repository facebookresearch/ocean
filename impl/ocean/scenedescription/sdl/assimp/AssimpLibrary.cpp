/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/assimp/AssimpLibrary.h"
#include "ocean/scenedescription/sdl/assimp/AssimpScene.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace Assimp
{

AssimpLibrary::AssimpLibrary() :
	Library(nameAssimpLibrary(), TYPE_TRANSIENT, 100u /*priority*/)
{
	registerFileExtension("3ds", "3D-Studio File Format");
	registerFileExtension("dae", "Collada File Format");
	registerFileExtension("dxf", "DXF File Format");
	registerFileExtension("fbx", "FBX (Filmbox) File Format");
	registerFileExtension("gltf", "GL Transmission File Format");
	registerFileExtension("glb", "GL Transmission File Format");
	registerFileExtension("obj", "Wavefront OBJ Format");
}

AssimpLibrary::~AssimpLibrary()
{
	// nothing to do here
}

bool AssimpLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<AssimpLibrary>(nameAssimpLibrary());
}

bool AssimpLibrary::unregisterLibrary()
{
	return Library::unregisterLibrary(nameAssimpLibrary());
}

LibraryRef AssimpLibrary::create()
{
	return LibraryRef(new AssimpLibrary());
}

SceneRef AssimpLibrary::loadTransient(const std::string& filename, const std::string& /*fileExtension*/, float* /*progress*/, bool* /*cancel*/)
{
	return SceneRef(new AssimpScene(filename));
}

}

}

}

}
