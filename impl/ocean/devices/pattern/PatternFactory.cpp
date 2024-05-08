/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/pattern/PatternFactory.h"
#include "ocean/devices/pattern/PatternTracker6DOF.h"
#include "ocean/devices/pattern/StaticPatternTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Pattern
{

PatternFactory::PatternFactory() :
	Factory(namePatternLibrary())
{
	registerDevice(PatternTracker6DOF::deviceNamePatternTracker6DOF(), PatternTracker6DOF::deviceTypePatternTracker6DOF(), InstanceFunction::createStatic(&PatternFactory::createTracker6DOF));
	registerDevice(StaticPatternTracker6DOF::deviceNameStaticPatternTracker6DOF(), StaticPatternTracker6DOF::deviceTypeStaticPatternTracker6DOF(), InstanceFunction::createStatic(&PatternFactory::createTracker6DOF));
}

bool PatternFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new PatternFactory()));
}

bool PatternFactory::unregisterFactory()
{
	return Factory::unregisterFactory(namePatternLibrary());
}

Device* PatternFactory::createTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	if (name == PatternTracker6DOF::deviceNamePatternTracker6DOF())
	{
		ocean_assert_and_suppress_unused(deviceType == PatternTracker6DOF::deviceTypePatternTracker6DOF(), deviceType);

		return new PatternTracker6DOF();
	}
	else
	{
		ocean_assert(name == StaticPatternTracker6DOF::deviceNameStaticPatternTracker6DOF());
		ocean_assert(deviceType == StaticPatternTracker6DOF::deviceTypeStaticPatternTracker6DOF());

		return new StaticPatternTracker6DOF();
	}
}

}

}

}
