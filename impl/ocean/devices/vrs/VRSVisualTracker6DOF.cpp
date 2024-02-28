// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/VRSVisualTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

VRSVisualTracker6DOF::VRSVisualTracker6DOF(const std::string& name) :
    Device(name, deviceTypeVRSTracker6DOF()),
    Measurement(name, deviceTypeVRSTracker6DOF()),
    Tracker(name, deviceTypeVRSTracker6DOF()),
	VisualTracker(name, deviceTypeVRSVisualTracker6DOF()),
	OrientationTracker3DOF(name),
	PositionTracker3DOF(name),
	Tracker6DOF(name),
	VRSTracker(name, deviceTypeVRSVisualTracker6DOF()),
	VRSTracker6DOF(name, deviceTypeVRSVisualTracker6DOF())
{
	// nothing to do here
}

VRSVisualTracker6DOF::~VRSVisualTracker6DOF()
{
	// nothing to do here
}

}

}

}
