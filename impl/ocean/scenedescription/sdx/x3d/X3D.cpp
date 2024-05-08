/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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

#if defined(OCEAN_RUNTIME_STATIC)

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
