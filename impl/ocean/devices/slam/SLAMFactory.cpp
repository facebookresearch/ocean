/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/slam/SLAMFactory.h"
#include "ocean/devices/slam/SLAMTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace SLAM
{

SLAMFactory::SLAMFactory() :
	Factory(nameSLAMLibrary())
{
	registerDevice(SLAMTracker6DOF::deviceNameSLAMTracker6DOF(), SLAMTracker6DOF::deviceTypeSLAMTracker6DOF(), InstanceFunction::createStatic(&SLAMFactory::createSLAMTracker6DOF));
}

bool SLAMFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new SLAMFactory()));
}

bool SLAMFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameSLAMLibrary());
}

Device* SLAMFactory::createSLAMTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	ocean_assert_and_suppress_unused(name == SLAMTracker6DOF::deviceNameSLAMTracker6DOF(), name);
	ocean_assert_and_suppress_unused(deviceType == SLAMTracker6DOF::deviceTypeSLAMTracker6DOF(), deviceType);

	return new SLAMTracker6DOF();
}

}

}

}
