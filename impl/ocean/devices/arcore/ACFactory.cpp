/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arcore/ACFactory.h"
#include "ocean/devices/arcore/ACDepthTracker6DOF.h"
#include "ocean/devices/arcore/ACPlaneTracker6DOF.h"
#include "ocean/devices/arcore/ACSceneTracker6DOF.h"
#include "ocean/devices/arcore/ACWorldTracker6DOF.h"
#include "ocean/devices/arcore/ARSessionManager.h"

#include "ocean/platform/android/NativeInterfaceManager.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

ACFactory::ACFactory() :
	Factory(nameARCoreLibrary())
{
#if defined(__NDK_MAJOR__) && defined(__ANDROID_API__)
	Log::debug() << "Devices::ARCore uses Android SDK version " << __NDK_MAJOR__ << " and API level " << __ANDROID_API__;
#endif

	registerDevices();
}

bool ACFactory::registerFactory()
{
	return Factory::registerFactory(std::unique_ptr<Factory>(new ACFactory()));
}

bool ACFactory::unregisterFactory()
{
	return Factory::unregisterFactory(nameARCoreLibrary());
}

void ACFactory::update(unsigned int textureId)
{
	ARSessionManager::get().update(textureId);
}

void ACFactory::registerDevices()
{
	registerDevice(ACWorldTracker6DOF::deviceNameACWorldTracker6DOF(), ACWorldTracker6DOF::deviceTypeACWorldTracker6DOF(), InstanceFunction::createStatic(createACTracker6DOF));
	registerDevice(ACSceneTracker6DOF::deviceNameACSceneTracker6DOF(), ACSceneTracker6DOF::deviceTypeACSceneTracker6DOF(), InstanceFunction::createStatic(createACTracker6DOF));
	registerDevice(ACDepthTracker6DOF::deviceNameACDepthTracker6DOF(), ACDepthTracker6DOF::deviceTypeACDepthTracker6DOF(), InstanceFunction::createStatic(createACTracker6DOF));
	registerDevice(ACPlaneTracker6DOF::deviceNameACPlaneTracker6DOF(), ACPlaneTracker6DOF::deviceTypeACPlaneTracker6DOF(), InstanceFunction::createStatic(createACTracker6DOF));
}

Device* ACFactory::createACTracker6DOF(const std::string& name, const Device::DeviceType& deviceType)
{
	if (!ARSessionManager::get().isARCoreAvailable())
	{
		Log::warning() << "ARCore is not available";
		return nullptr;
	}

	if (name == ACWorldTracker6DOF::deviceNameACWorldTracker6DOF())
	{
		ocean_assert(deviceType == ACWorldTracker6DOF::deviceTypeACWorldTracker6DOF());

		return new ACWorldTracker6DOF();
	}
	else if (name == ACSceneTracker6DOF::deviceNameACSceneTracker6DOF())
	{
		ocean_assert(deviceType == ACSceneTracker6DOF::deviceTypeACSceneTracker6DOF());

		return new ACSceneTracker6DOF();
	}
	else if (name == ACDepthTracker6DOF::deviceNameACDepthTracker6DOF())
	{
		ocean_assert(deviceType == ACDepthTracker6DOF::deviceTypeACDepthTracker6DOF());

		return new ACDepthTracker6DOF();
	}
	else
	{
		ocean_assert(name == ACPlaneTracker6DOF::deviceNameACPlaneTracker6DOF());
		ocean_assert(deviceType == ACPlaneTracker6DOF::deviceTypeACPlaneTracker6DOF());

		return new ACPlaneTracker6DOF();
	}
}

}

}

}
