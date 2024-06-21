/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DLibrary.h"
#include "ocean/scenedescription/sdx/x3d/ClassicParser.h"

#ifdef OCEAN_HAS_TINYXML2
	#include "ocean/scenedescription/sdx/x3d/XMLParser.h"
#endif

#include "ocean/base/String.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DLibrary::X3DLibrary() :
	Library(nameX3DLibrary(), TYPE_PERMANENT, 200u /*priority*/)
{
	registerFileExtension("x3d", "X3D, Extensible 3D - XML encoding");
	registerFileExtension("x3dv", "X3D, Extensible 3D - VRML encoding");
	registerFileExtension("ox3d", "Ocean X3D, X3D including extensions (XML encoding)");
	registerFileExtension("ox3dv", "Ocean X3D, X3D including extensions (VRML encoding)");
	registerFileExtension("wrl", "VRML, Virtual Reality Modeling Language");
}

X3DLibrary::~X3DLibrary()
{
	// nothing to do here
}

bool X3DLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<X3DLibrary>(nameX3DLibrary());
}

bool X3DLibrary::unregisterLibrary()
{
	return Library::unregisterLibrary(nameX3DLibrary());
}

LibraryRef X3DLibrary::create()
{
	return LibraryRef(new X3DLibrary());
}

SceneRef X3DLibrary::loadPermanent(const std::string& filename, const std::string& fileExtension, const Rendering::EngineRef& engine, const Timestamp& timestamp, float* progress, bool* cancel)
{
	ocean_assert(timestamp.isValid());

	const std::string extension(String::toLower(fileExtension));

	if (extension == "x3d" || extension == "ox3d")
	{
#ifdef OCEAN_HAS_TINYXML2
		XMLParser xmlParser(filename, progress, cancel);
		return xmlParser.parse(*this, engine, Timestamp());
#else
		ocean_assert(false && "Disabled because tinyxml2 is not available");
		return SceneRef();
#endif
	}
	else if (extension == "x3dv" || extension == "ox3dv" || extension == "wrl")
	{
		ClassicParser classicParser(filename, progress, cancel);
		return classicParser.parse(*this, engine, timestamp);
	}

	ocean_assert(false && "This should never happen!");
	return SceneRef();
}

}

}

}

}
