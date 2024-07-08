/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/slam/SLAM.h"
#include "ocean/devices/slam/SLAMFactory.h"

namespace Ocean
{

namespace Devices
{

namespace SLAM
{

std::string nameSLAMLibrary()
{
	return std::string("SLAM Tracker library");
}

#if defined(OCEAN_RUNTIME_STATIC) || defined(_ANDROID)

void registerSLAMLibrary()
{
	SLAMFactory::registerFactory();
}

bool unregisterSLAMLibrary()
{
	return SLAMFactory::unregisterFactory();
}

#endif // OCEAN_RUNTIME_STATICF

}

}

}
