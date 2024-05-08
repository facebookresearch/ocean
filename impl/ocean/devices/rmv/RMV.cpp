/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/rmv/RMV.h"
#include "ocean/devices/rmv/RMVFactory.h"

namespace Ocean
{

namespace Devices
{

namespace RMV
{

 std::string nameRMVLibrary()
{
	return std::string("RMV Tracker library");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerRMVLibrary()
{
	RMVFactory::registerFactory();
}

bool unregisterRMVLibrary()
{
	return RMVFactory::unregisterFactory();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
