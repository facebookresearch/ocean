// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/vrs/VRSVisualObjectTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

VRSVisualObjectTracker6DOF::VRSVisualObjectTracker6DOF(const std::string& name) :
    Device(name, deviceTypeVRSTracker6DOF()),
    Measurement(name, deviceTypeVRSTracker6DOF()),
    Tracker(name, deviceTypeVRSTracker6DOF()),
	ObjectTracker(name, deviceTypeVRSVisualObjectTracker6DOF()),
	VisualTracker(name, deviceTypeVRSVisualObjectTracker6DOF()),
	OrientationTracker3DOF(name),
	PositionTracker3DOF(name),
	Tracker6DOF(name),
	VRSTracker(name, deviceTypeVRSVisualObjectTracker6DOF()),
	VRSTracker6DOF(name, deviceTypeVRSVisualObjectTracker6DOF()),
	VRSVisualTracker6DOF(name)
{
	// nothing to do here
}

VRSVisualObjectTracker6DOF::~VRSVisualObjectTracker6DOF()
{
	// nothing to do here
}

VRSVisualObjectTracker6DOF::ObjectId VRSVisualObjectTracker6DOF::registerObject(const std::string& description, const Vector3& dimension)
{
	const Strings vrsObjectDescriptions = objectDescriptions();

	// first we check whether we have a perfect hit

	for (std::string vrsObjectDescription : vrsObjectDescriptions)
	{
		if (vrsObjectDescription == description)
		{
			const ObjectId externalObjectId = objectId(vrsObjectDescription);
			ocean_assert(externalObjectId != invalidObjectId());

			return externalObjectId;
		}
	}

	// now we check whether we have a file (matching the end of the path only)

	for (std::string vrsObjectDescription : vrsObjectDescriptions)
	{
		if (description.size() < vrsObjectDescription.size() && vrsObjectDescription.find(description) == vrsObjectDescription.size() - description.size())
		{
			const ObjectId externalObjectId = objectId(vrsObjectDescription);
			ocean_assert(externalObjectId != invalidObjectId());

			return externalObjectId;
		}
	}

	Log::warning() << "The VRS tracker does not have an object " << description;

	return invalidObjectId();
}

bool VRSVisualObjectTracker6DOF::unregisterObject(const ObjectId objectId)
{
	return false;
}

}

}

}
