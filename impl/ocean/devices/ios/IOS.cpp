/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/ios/IOS.h"
#include "ocean/devices/ios/IOSFactory.h"

namespace Ocean
{

namespace Devices
{

namespace IOS
{

const std::string& nameIOSLibrary()
{
	static const std::string name("IOS sensor library");
	return name;
}

#if defined(OCEAN_RUNTIME_STATIC)

void registerIOSLibrary()
{
	Ocean::Devices::IOS::IOSFactory::registerFactory();
}

bool unregisterIOSLibrary()
{
	return Ocean::Devices::IOS::IOSFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_STATIC)

}

}

}
