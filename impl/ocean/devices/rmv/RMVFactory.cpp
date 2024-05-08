/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/rmv/RMVFactory.h"
#include "ocean/devices/rmv/RMVTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace RMV
{

RMVFactory::RMVFactory() :
	Factory(nameRMVLibrary())
{
	registerDevice(RMVTracker6DOF::deviceNameRMVTracker6DOF(), RMVTracker6DOF::deviceTypeRMVTracker6DOF(), InstanceFunction::createStatic(&RMVFactory::createRMVTracker6DOF));
}

bool RMVFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new RMVFactory()));
}

bool RMVFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameRMVLibrary());
}

Device* RMVFactory::createRMVTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(name == RMVTracker6DOF::deviceNameRMVTracker6DOF(), name);
	ocean_assert_and_suppress_unused(deviceType == RMVTracker6DOF::deviceTypeRMVTracker6DOF(), deviceType);

	return new RMVTracker6DOF();
}

}

}

}
