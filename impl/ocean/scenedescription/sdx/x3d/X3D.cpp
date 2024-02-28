// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DLibrary.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

std::string nameX3DLibrary()
{
	return std::string("SDX X3D");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerX3DLibrary()
{
	X3DLibrary::registerLibrary();
}

bool unregisterX3DLibrary()
{
	return X3DLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}

}
