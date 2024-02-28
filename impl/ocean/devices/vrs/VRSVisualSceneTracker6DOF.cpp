// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/VRSVisualSceneTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

VRSVisualSceneTracker6DOF::VRSVisualSceneTracker6DOF(const std::string& name, const DeviceType& type) :
	Device(name, type),
	Measurement(name, type),
	Tracker(name, type),
	OrientationTracker3DOF(name),
	PositionTracker3DOF(name),
	Tracker6DOF(name),
	SceneTracker6DOF(name),
	VRSTracker(name, type),
	VRSTracker6DOF(name, type),
	VRSSceneTracker6DOF(name, type),
	VisualTracker(name, type)
{
	// nothing to do here
}

VRSVisualSceneTracker6DOF::~VRSVisualSceneTracker6DOF()
{
	// nothing to do here
}

}

}

}
