/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/ARKit.h"
#include "ocean/devices/arkit/AKFactory.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

const std::string& nameARKitLibrary()
{
	static const std::string name("ARKit tracking library");
	return name;
}

#if defined(OCEAN_RUNTIME_STATIC)

void registerARKitLibrary()
{
	Ocean::Devices::ARKit::AKFactory::registerFactory();
}

bool unregisterARKitLibrary()
{
	return Ocean::Devices::ARKit::AKFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_STATIC)

}

}

}
