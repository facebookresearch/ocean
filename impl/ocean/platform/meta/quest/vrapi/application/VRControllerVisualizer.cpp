// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/application/VRControllerVisualizer.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

namespace Application
{

void VRControllerVisualizer::visualizeControllersInWorld(const TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp, const Scalar controllerRayLength)
{
	ocean_assert(isValid());

	if (!isValid())
	{
		return;
	}

	for (const TrackedRemoteDevice::RemoteType remoteType : {TrackedRemoteDevice::RT_LEFT, TrackedRemoteDevice::RT_RIGHT})
	{
		HomogenousMatrix4 world_T_remoteDevice;
		if (!trackedRemoteDevice.pose(remoteType, &world_T_remoteDevice, /* world_T_remoteDeviceRing */ nullptr, timestamp))
		{
			world_T_remoteDevice.toNull();
		}

		visualizeControllerInWorld(remoteType == TrackedRemoteDevice::RT_LEFT ? CT_LEFT : CT_RIGHT, world_T_remoteDevice, controllerRayLength);
	}
}

}

}

}

}

}

}
