/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/obj/OBJLibrary.h"
#include "ocean/scenedescription/sdl/obj/OBJParser.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

OBJLibrary::OBJLibrary() :
	Library(nameOBJLibrary(), TYPE_TRANSIENT, 90u /*priority*/)
{
	registerFileExtension("obj", "Wavefront OBJ Format");
}

OBJLibrary::~OBJLibrary()
{
	// nothing to do here
}

bool OBJLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<OBJLibrary>(nameOBJLibrary());
}

bool OBJLibrary::unregisterLibrary()
{
	return Library::unregisterLibrary(nameOBJLibrary());
}

LibraryRef OBJLibrary::create()
{
	return LibraryRef(new OBJLibrary());
}

SceneRef OBJLibrary::loadTransient(const std::string& filename, const std::string& /*fileExtension*/, float* progress, bool* cancel)
{
	OBJParser parser(filename, progress);
	return parser.parse(cancel);
}

}

}

}

}
